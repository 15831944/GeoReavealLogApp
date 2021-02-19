/****************************************************************************
 *	class		:	CKCBusyProgressCtrl
 *	author		:	Peter Mares / kinkycode.com (gui@ch.co.za)
 *	base class	:	CStatic (MFC)
 *	notes		:	Control to be used for progress indication when there are
 *					no lower and upper bounds available for a progress bar.
 *					Can also be used as a normal progress bar
 *
 *	Disclaimer	:	Its free, it feels good and its from South Africa :)
 ****************************************************************************/

#include "stdafx.h"
#include "KCBusyProgressCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKCBusyProgressCtrl

CKCBusyProgressCtrl::CKCBusyProgressCtrl(int nNumSteps, int nCurPos)
: m_nNumSteps(nNumSteps)
, m_nStep(1)
, m_nCurPos(nCurPos)
, m_colBkg( GetSysColor( COLOR_BTNFACE ) )
, m_colBlockFace( RGB(160, 190, 220) )
, m_colBlockEdge( RGB(50, 90, 135) )
, m_colBlockFaceHi( RGB(190, 220, 255) )
, m_colBlockEdgeHi( RGB(50, 90, 135) )
, m_pOldBmp(NULL)
, m_nIBPadding(1)
, m_rect(0,0,0,0)
, m_dBlockHeight(0.0L)
, m_dBlockWidth(0.0L)
, m_bBusyThrd(false)
, m_pThrd(NULL)
, m_nSpeed(100)
, m_nMode(BPC_MODE_BUSY)
, m_nLower(0)
, m_nUpper(100)
{
}

/////////////////////////////////////////////////////////////////////////////

CKCBusyProgressCtrl::~CKCBusyProgressCtrl()
{
	if ( m_pOldBmp )
	{
		m_memDC.SelectObject(m_pOldBmp);
		m_memBmp.DeleteObject();
		m_memDC.DeleteDC();
	}
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CKCBusyProgressCtrl, CStatic)
	//{{AFX_MSG_MAP(CKCBusyProgressCtrl)
	ON_WM_PAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(BPM_SETNUMSTEPS, OnSetNumSteps)
	ON_MESSAGE(BPM_SETCURPOS, OnSetCurPos)
	ON_MESSAGE(BPM_SETIBPAD, OnSetIBPad)
	ON_MESSAGE(BPM_SETSPEED, OnSetSpeed)
	ON_MESSAGE(BPM_SETRANGE, OnSetRange)
	ON_MESSAGE(BPM_SETMODE, OnSetMode)
	ON_MESSAGE(BPM_STARTBUSY, OnStartBusy)
	ON_MESSAGE(BPM_ENDBUSY, OnEndBusy)
	ON_MESSAGE(BPM_STEPIT, OnStepIt)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKCBusyProgressCtrl message handlers

void CKCBusyProgressCtrl::PreSubclassWindow() 
{
	DWORD			dwStyle = GetStyle();

//	dwStyle |= SS_OWNERDRAW;
	CStatic::PreSubclassWindow();
	Recalc();
}

/////////////////////////////////////////////////////////////////////////////

BOOL CKCBusyProgressCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	BOOL				bResult = CWnd::Create("STATIC", "BusyProgressCtrl", dwStyle, rect, pParentWnd, nID, pContext);
	
	Recalc();

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
//	function		:	Recalc()
//	description		:	Function used to recalculate the block sizes and
//						optionally get the current client area
/////////////////////////////////////////////////////////////////////////////
void CKCBusyProgressCtrl::Recalc()
{
	if ( m_rect.IsRectEmpty() )
		GetClientRect(&m_rect);

	CRect			tRect = m_rect;

	tRect.right -= (m_nNumSteps * m_nIBPadding);
	m_dBlockWidth = ((double)tRect.Width() / (double)m_nNumSteps);
	m_dBlockHeight = tRect.Height();
}

/////////////////////////////////////////////////////////////////////////////

void CKCBusyProgressCtrl::OnPaint() 
{
	CPaintDC			dc(this); // device context for painting

	// create a memory dc if needs be
	if ( !m_memDC.m_hDC )
	{
		m_memDC.CreateCompatibleDC(&dc);
		m_memBmp.CreateCompatibleBitmap(&dc, m_rect.Width(), m_rect.Height());
		m_pOldBmp = m_memDC.SelectObject(&m_memBmp);
	}

	DrawBackground(m_memDC, m_rect);
	DrawBlocks(m_memDC, m_rect);

	// render the final image
	dc.BitBlt(0, 0, m_rect.Width(), m_rect.Height(), &m_memDC, 0, 0, SRCCOPY);
}

/////////////////////////////////////////////////////////////////////////////

void CKCBusyProgressCtrl::DrawBackground(CDC& dc, CRect& rect)
{
	dc.FillSolidRect( &rect, m_colBkg );
}

/////////////////////////////////////////////////////////////////////////////

void CKCBusyProgressCtrl::DrawBlocks(CDC& dc, CRect& rect)
{
	CRect				bRect;
	double				dXOffset = 0;
	int					i;
	CPen				nPen, *pOldPen = NULL;
	CBrush				nBrush, *pOldBrush = NULL;

	// create some drawing tools
	nPen.CreatePen( PS_SOLID, 0, m_colBlockEdge );
	nBrush.CreateSolidBrush( m_colBlockFace );
	pOldPen = dc.SelectObject(&nPen);
	pOldBrush = dc.SelectObject(&nBrush);

	// create the initial rectangle
	bRect.top = 0; bRect.bottom = (int) m_dBlockHeight;
	bRect.left = 0; bRect.right = (int) m_dBlockWidth;
	for ( i = 0; i < m_nNumSteps; i++ )
	{
		if ( m_nMode & BPC_MODE_BUSY )
		{
			if ( i == m_nCurPos )
				DrawHiliteBlock(dc, bRect);
			else
				DrawBlock(dc, bRect);
		}
		else if ( m_nMode & BPC_MODE_PROGRESS )
		{
			double		dPerc = (((double) m_nLower + (double) m_nCurPos) / (double) m_nUpper);
			int			nFull = (int) (dPerc*(double)m_nNumSteps)-1;
			double		dPerc2 = (((dPerc * (double)m_nNumSteps)-1) - nFull);
			int			nPartial = (int)(m_dBlockWidth * dPerc2);

			if ( i <= nFull )
				DrawHiliteBlock(dc, bRect);
			else
			{
				DrawBlock(dc, bRect);
				if ( i == nFull + 1 && nPartial )
					DrawPartialBlock(dc, bRect, nPartial);
			}
		}
		// offset the rectangle a bit
		dXOffset += m_dBlockWidth + (double) m_nIBPadding;
		bRect.left = (int) dXOffset;
		bRect.right = (int)(dXOffset + m_dBlockWidth);
	}

	// cleanup after ourselves...
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
	nPen.DeleteObject();
	nBrush.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////

void CKCBusyProgressCtrl::DrawBlock(CDC& dc, CRect& rect)
{
	dc.Rectangle(&rect);
}

/////////////////////////////////////////////////////////////////////////////

void CKCBusyProgressCtrl::DrawHiliteBlock(CDC& dc, CRect& rect)
{
	CPen			nPen, *pOldPen = NULL;
	CBrush			nBrush, *pOldBrush = NULL;

	// use the correct tools ;)
	nPen.CreatePen( PS_SOLID, 0, m_colBlockEdgeHi );
	nBrush.CreateSolidBrush( m_colBlockFaceHi );
	pOldPen = dc.SelectObject(&nPen);
	pOldBrush = dc.SelectObject(&nBrush);

	// draw the block
	dc.Rectangle(&rect);

	// cleanup
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
	nPen.DeleteObject();
	nBrush.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////

void CKCBusyProgressCtrl::DrawPartialBlock(CDC& dc, CRect& rect, int nPartial)
{
	CRect			pRect = rect;

	pRect.DeflateRect(1, 1);
	pRect.right = pRect.left + nPartial;
	if ( pRect.right >= rect.right )
		pRect.right = rect.right - 1;
	dc.FillSolidRect(&pRect, m_colBlockFaceHi);
}

/////////////////////////////////////////////////////////////////////////////

void CKCBusyProgressCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	
	GetClientRect(&m_rect);
	if ( m_memDC.m_hDC )
	{
		// delete the dc to allow OnPaint to recreate the DC for the new size
		m_memDC.SelectObject(m_pOldBmp);
		m_memBmp.DeleteObject();
		m_memDC.DeleteDC();
	}
}

/////////////////////////////////////////////////////////////////////////////

void CKCBusyProgressCtrl::StepIt()
{
	if ( m_nMode & BPC_MODE_BUSY)
	{
		// do a check for the left to right movement
		if ( m_nCurPos == m_nNumSteps-1 )
			m_nStep = -1;
		// check if we're done moving from right to left..
		if ( m_nCurPos == 0 )
			m_nStep = 1;
	}
	else if ( m_nMode & BPC_MODE_PROGRESS )
	{
	}

	// update the position
	m_nCurPos += m_nStep;
	Invalidate();
	PumpMessages();
}

void CKCBusyProgressCtrl::SetCurPos(int nCurPos)						
{
	 m_nCurPos = nCurPos; 
	 Invalidate();
	 PumpMessages(); 
}
/////////////////////////////////////////////////////////////////////////////

void CKCBusyProgressCtrl::SetMode(int nMode)
{
	End(); 
	if ( m_nMode >= BPC_MODE_BUSY && m_nMode <= BPC_MODE_PROGRESS )
		m_nMode = nMode;
}

/////////////////////////////////////////////////////////////////////////////

void CKCBusyProgressCtrl::Start()
{
	if ( m_nMode & BPC_MODE_BUSY )
	{
		if ( m_pThrd )
			End();
		m_pThrd = AfxBeginThread(thrdBusy, this);
	}
	PumpMessages();
}

/////////////////////////////////////////////////////////////////////////////

void CKCBusyProgressCtrl::End()
{
	PumpMessages();
	if ( m_pThrd )
	{
		if ( m_bBusyThrd )
			m_bBusyThrd = false;
		WaitForSingleObject(m_pThrd->m_hThread, m_nSpeed*2);
		m_pThrd = NULL;
	}	
}

/////////////////////////////////////////////////////////////////////////////

UINT CKCBusyProgressCtrl::thrdBusy(LPVOID pParam)
{
	CKCBusyProgressCtrl*			pThis = (CKCBusyProgressCtrl*) pParam;

	if ( !pThis )
		return -1;

	pThis->m_bBusyThrd = true;
	while ( pThis->m_bBusyThrd )
	{
		pThis->StepIt();
		Sleep(pThis->m_nSpeed);
	}
	pThis->m_bBusyThrd = false;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////

void CKCBusyProgressCtrl::Reset()
{
	m_nCurPos = 0;
	m_nStep = 1;
	PumpMessages();
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CKCBusyProgressCtrl::OnSetNumSteps(WPARAM wParam, LPARAM lParam)
{
	SetNumSteps((int)wParam);
	Invalidate();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CKCBusyProgressCtrl::OnSetCurPos(WPARAM wParam, LPARAM lParam)
{
	SetCurPos( (int)wParam );
	Invalidate();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CKCBusyProgressCtrl::OnSetIBPad(WPARAM wParam, LPARAM lParam)
{
	SetInterBlockPadding( (int) wParam );
	Invalidate();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CKCBusyProgressCtrl::OnSetSpeed(WPARAM wParam, LPARAM lParam)
{
	SetSpeed( (int) wParam );
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CKCBusyProgressCtrl::OnSetRange(WPARAM wParam, LPARAM lParam)
{
	SetRange((int) wParam, (int) lParam );
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CKCBusyProgressCtrl::OnSetMode(WPARAM wParam, LPARAM lParam)
{
	SetMode( (int) wParam );
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CKCBusyProgressCtrl::OnStartBusy(WPARAM wParam, LPARAM lParam)
{
	Start();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CKCBusyProgressCtrl::OnEndBusy(WPARAM wParam, LPARAM lParam)
{
	End();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CKCBusyProgressCtrl::OnStepIt(WPARAM wParam, LPARAM lParam)
{
	StepIt();
	Invalidate();
	return 0;
}

void CKCBusyProgressCtrl::PumpMessages()
{
    // Must call Create() before using the dialog
    ASSERT(m_hWnd!=NULL);

    MSG msg;
    // Handle dialog messages
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if(!IsDialogMessage(&msg))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);  
      }
    }
}
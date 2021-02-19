// enedit.cpp : implementation file
//

#include "stdafx.h"
#include "enedit.h"
#include "themed.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnEdit

CEnEdit::CEnEdit(BOOL bComboStyle) : 
					m_bComboStyle(bComboStyle),
					m_bFirstShow(TRUE), 
					m_nButtonDown(-1)
{
}

CEnEdit::~CEnEdit()
{
}


BEGIN_MESSAGE_MAP(CEnEdit, CEdit)
	//{{AFX_MSG_MAP(CEnEdit)
	ON_WM_NCCALCSIZE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_NCPAINT()
	ON_WM_NCHITTEST()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(WM_HTHOTCHANGE, OnHotChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnEdit message handlers

BOOL CEnEdit::AddButton(UINT nID, LPCTSTR szCaption, LPCTSTR szTip, int nWidth)
{
	return InsertButton(m_aButtons.GetSize(), nID, szCaption, szTip, nWidth);
}

BOOL CEnEdit::InsertButton(int nPos, UINT nID, LPCTSTR szCaption, LPCTSTR szTip, int nWidth)
{
	if (nWidth <= 0 || !nID)
		return FALSE;

	nPos = max(nPos, 0);
	nPos = min(nPos, m_aButtons.GetSize());

	EDITBTN eb;

	eb.nID = nID;
	eb.sCaption = szCaption;
	eb.sTip = szTip;
	eb.nWidth = nWidth;

	m_aButtons.InsertAt(nPos, eb);

	// add rect to hot tracker
	m_hotTrack.AddRect(CRect(0, 0, 0, 0));

	if (GetSafeHwnd())
	{
		if (!eb.sTip.IsEmpty())
			m_tooltip.AddTool(this, eb.sTip, CRect(0, 0, 10, 10), nID);

		RecalcBtnRects();

		// force WM_NCCALCSIZE
		if (!m_bFirstShow)
			SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER); 
	}

	return TRUE;
}

void CEnEdit::PreSubclassWindow() 
{
	// create tooltip
	if (!m_tooltip.GetSafeHwnd() && m_tooltip.Create(this))
	{
		// add any existing buttons
		int nBtn = m_aButtons.GetSize();

		while (nBtn--)
		{
			const EDITBTN& eb = m_aButtons[nBtn];
			m_tooltip.AddTool(this, eb.sTip, CRect(), eb.nID);
		}
	}

	RecalcBtnRects();

	// hot tracking
	if (CThemed().AreControlsThemed())
		m_hotTrack.Initialize(this);

	CEdit::PreSubclassWindow();
}

BOOL CEnEdit::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent(pMsg);
	
	return CEdit::PreTranslateMessage(pMsg);
}

void CEnEdit::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	if (bCalcValidRects)
	{
		m_bFirstShow = FALSE; // in case we get here before OnNcPaint()
	
		int nBtn = m_aButtons.GetSize();

		while (nBtn--)
			lpncsp->rgrc[0].right -= m_aButtons[nBtn].nWidth;
	}
	
	CEdit::OnNcCalcSize(bCalcValidRects, lpncsp);
}

void CEnEdit::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	CEdit::OnNcLButtonDown(nHitTest, point);

	if (IsWindowEnabled())
	{
		int nBtn = ButtonHitTest(point);

		if (nBtn >= 0)
		{
			if (m_aButtons[nBtn].bEnabled)
			{
				SetCapture();
				m_nButtonDown = nBtn;
				
				SendMessage(WM_NCPAINT);
			}
		}
		else
			SetFocus();
	}
}

void CEnEdit::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CEdit::OnLButtonUp(nFlags, point);

	if (m_nButtonDown == -1)
		return;

	ClientToScreen(&point);
	int nBtnDown = m_nButtonDown;
	int nBtnUp = ButtonHitTest(point);

	// update UI
	ReleaseCapture();
	m_nButtonDown = -1;
	
	SendMessage(WM_NCPAINT);

	// process
	if (nBtnDown == nBtnUp)
	{
		// call derived class first
		OnBtnClick(m_aButtons[nBtnUp].nID);

		// then parent
		GetParent()->SendMessage(WM_EN_BTNCLICK, GetDlgCtrlID(), m_aButtons[nBtnUp].nID);
	}
}

void CEnEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	CEdit::OnMouseMove(nFlags, point);

	if (m_nButtonDown != -1)
	{
		ClientToScreen(&point);

		if (ButtonHitTest(point) != m_nButtonDown)
		{
			ReleaseCapture();
			m_nButtonDown = -1;
			SendMessage(WM_NCPAINT);
		}
	}
}

void CEnEdit::OnSize(UINT nType, int cx, int cy) 
{
	CEdit::OnSize(nType, cx, cy);
	
	// update tool rects
	RecalcBtnRects();
}

void CEnEdit::RecalcBtnRects()
{
	int nBtn = m_aButtons.GetSize();
	
	while (nBtn--)
	{
		CRect rBtn = GetButtonRect((UINT)nBtn);
		ScreenToClient(rBtn);

		m_tooltip.SetToolRect(this, m_aButtons[nBtn].nID, rBtn);

		// update hottracker too
		m_hotTrack.UpdateRect(nBtn, rBtn);
	}
}

CRect CEnEdit::GetButtonRect(int nBtn) const
{
	int nOffset = 0;

	for (int nIndex = 0; nIndex < nBtn; nIndex++)
		nOffset += m_aButtons[nIndex].nWidth;

	CRect rButton;
	GetClientRect(rButton);

	rButton.left = rButton.right + nOffset;
	rButton.right += nOffset + m_aButtons[nBtn].nWidth;

	ClientToScreen(rButton);

	return rButton;
}

void CEnEdit::OnNcPaint() 
{
	if (m_bFirstShow)
	{
		m_bFirstShow = FALSE;
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER); 
	}

	Default();

	// draw the icon and browse button
	CWindowDC dc(this);

	CRect rWindow;
	GetWindowRect(rWindow);

	NcPaint(&dc, rWindow);
}

void CEnEdit::NcPaint(CDC* pDC, const CRect& rWindow)
{
	CPoint ptCursor(::GetMessagePos());
	int nBtn = m_aButtons.GetSize();
	
	while (nBtn--)
		DrawButton(pDC, rWindow, nBtn, ptCursor);
}

int CEnEdit::ButtonHitTest(CPoint ptScreen) const
{
	int nBtn = m_aButtons.GetSize();
	
	while (nBtn--)
	{
		if (GetButtonRect(nBtn).PtInRect(ptScreen))
			return nBtn;
	}

	return -1;
}

int CEnEdit::ButtonHitTest(UINT nID) const
{
	int nBtn = m_aButtons.GetSize();
	
	while (nBtn--)
	{
		if (m_aButtons[nBtn].nID == nID)
			return nBtn;
	}

	return -1;
}

void CEnEdit::EnableButton(UINT nID, BOOL bEnable)
{
	int nBtn = ButtonHitTest(nID);

	if (nBtn >= 0 && m_aButtons[nBtn].bEnabled != bEnable)
	{
		m_aButtons[nBtn].bEnabled = bEnable;
		SendMessage(WM_NCPAINT);
	}
}

void CEnEdit::SetButtonTip(UINT nID, LPCTSTR szTip)
{
	int nBtn = ButtonHitTest(nID);

	if (nBtn >= 0 && m_aButtons[nBtn].sTip.Compare(szTip) != 0)
	{
		m_aButtons[nBtn].sTip = szTip;

		if (GetSafeHwnd())
		{
			m_tooltip.UpdateTipText(szTip, this, m_aButtons[nBtn].nID);
			SendMessage(WM_NCPAINT);
		}
	}
}

CRect CEnEdit::GetButtonRect(UINT nID)
{
	int nBtn = ButtonHitTest(nID);

	if (nBtn >= 0)
		return GetButtonRect((UINT)nBtn);

	return CRect(0, 0, 0, 0);
}

void CEnEdit::DrawButton(CDC* pDC, const CRect& rWindow, int nBtn, const CPoint& ptCursor) const
{
	CRect rBtn = GetButtonRect(nBtn);

	if (rBtn.IsRectEmpty())
		return;

	BOOL bThemed = CThemed().AreControlsThemed();
	BOOL bHot = rBtn.PtInRect(ptCursor);
	rBtn.OffsetRect(-rWindow.TopLeft());

	// nasty business here because the API function DrawThemeEdge() is not theme aware!
	// and drawing a themed combostyle button will also draw the arrow which we don't want
	if (!m_bComboStyle || bThemed)	// draw as button type (for now)
	{
		UINT nFlags = DFCS_ADJUSTRECT | DFCS_BUTTONPUSH;
		
		if (!IsWindowEnabled() || !m_aButtons[nBtn].bEnabled)
			nFlags |= DFCS_INACTIVE;
		
		else if (m_nButtonDown == nBtn)
			nFlags |= DFCS_PUSHED;
		
		else if (bHot)
			nFlags |= DFCS_HOT;
		
		// clip the drawing rect to prevent window getting the parent bkgnd color wrong
		CRect rClip(rBtn);

		if (bThemed)
			rBtn.InflateRect(1, 1);
		
		// for now
		CThemed::DrawFrameControl(this, pDC, rBtn, DFC_BUTTON, nFlags, rClip);
	}
	else // unthemed combo style
	{
		if (m_nButtonDown == nBtn)
			pDC->DrawEdge(rBtn, BDR_RAISEDOUTER, BF_ADJUST | BF_RECT | BF_MIDDLE | BF_FLAT);
		else
		{
			pDC->DrawEdge(rBtn, BDR_RAISEDOUTER, BF_ADJUST | BF_RECT | BF_MIDDLE);
			pDC->DrawEdge(rBtn, BDR_RAISEDINNER, BF_ADJUST | BF_RECT | BF_MIDDLE);
		}
	}
	
	// draw custom caption
	if (m_nButtonDown == nBtn)
		rBtn.OffsetRect(1, 1);

	pDC->SelectStockObject(ANSI_VAR_FONT);
	pDC->SetTextAlign(TA_CENTER | TA_TOP);
	pDC->SetBkMode(TRANSPARENT);

	CString sCaption(m_aButtons[nBtn].sCaption);
	const int CHAR_HEIGHT = pDC->GetTextExtent("A").cy;
	BOOL bEnabled = IsWindowEnabled() && m_aButtons[nBtn].bEnabled;

	if (!sCaption.IsEmpty())
	{
		CPoint ptText(rBtn.left + (rBtn.Width() + 1) / 2, rBtn.top - (CHAR_HEIGHT - rBtn.Height()) / 2 - 1);

		if (bEnabled)
		{
			pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
			pDC->ExtTextOut(ptText.x, ptText.y, 0, rBtn, sCaption, NULL);
		}
		else
		{
			// draw embossed: dark over pale
			ptText.Offset(1, 1);
			pDC->SetTextColor(GetSysColor(COLOR_3DHIGHLIGHT));
			pDC->ExtTextOut(ptText.x, ptText.y, 0, rBtn, sCaption, NULL);

			ptText.Offset(-1, -1);
			pDC->SetTextColor(GetSysColor(COLOR_3DSHADOW));
			pDC->ExtTextOut(ptText.x, ptText.y, 0, rBtn, sCaption, NULL);
		}
	}
}

UINT CEnEdit::OnNcHitTest(CPoint point) 
{
	int nBtn = ButtonHitTest(point);

	if (nBtn >= 0)
		return HTBORDER;

	return CEdit::OnNcHitTest(point);
}

LRESULT CEnEdit::OnHotChange(WPARAM wp, LPARAM lp)
{
	LRESULT lr = Default();

	// wp has prev hot rect index
	// lp has new hot rect index
	ASSERT (((int)wp != -1 || (int)lp != -1) && (int)wp != (int)lp);

	CWindowDC dc(this);
	CPoint ptCursor(::GetMessagePos());

	CRect rWindow;
	GetWindowRect(rWindow);
	int nBtn = m_aButtons.GetSize();

	if (wp != -1 && lp != -1)
		int a = 5;

	if ((int)wp != -1 && m_aButtons[wp].bEnabled)
		DrawButton(&dc, rWindow, wp, ptCursor);

	if ((int)lp != -1 && m_aButtons[lp].bEnabled)
		DrawButton(&dc, rWindow, lp, ptCursor);

	return lr;
}




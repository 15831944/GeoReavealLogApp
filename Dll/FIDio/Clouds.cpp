/****************************************************************/
/*																*/
/*  CloudsCtrl.cpp												*/
/*																*/
/*  Implementation of the CCloudsCtrl class.					*/
/*	Attempt to simulate clouds using MFC 						*/
/*																*/
/*  Programmed by Pablo van der Meer							*/
/*  Copyright Pablo Software Solutions 2003						*/
/*  http://www.pablovandermeer.nl								*/
/*																*/
/*  Last updated: October 19, 2003								*/
/*																*/
/****************************************************************/


#include "stdafx.h"
#include "Clouds.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CCloudsCtrl::CCloudsCtrl()
{
	// protected bitmaps to restore the memory DC's
	m_pOldBitmap = NULL;

	m_pCloudPlasma1 = NULL;
	m_pCloudPlasma2 = NULL;
	m_pCloudPlasma3 = NULL;

	m_nDensity = 8;

	m_bInterActive = FALSE;
	m_nSpeed = 2;
	
	m_ptDirection = m_ptMove = CPoint(m_nSpeed, 1);
	m_ptOffset1 = m_ptOffset2 = m_ptOffset3 = CPoint(0,0);

	m_pPixels = NULL;
	m_nSizeOfBits = 0;
	m_hBitmap = NULL;

	m_strCredits = ""; 
}


CCloudsCtrl::~CCloudsCtrl()
{
	if (m_pOldBitmap != NULL)
		m_MainDC.SelectObject(m_pOldBitmap);  

	if (m_pCloudPlasma1)
		delete[] m_pCloudPlasma1;
	if (m_pCloudPlasma2)
		delete[] m_pCloudPlasma2;
	if (m_pCloudPlasma3)
		delete[] m_pCloudPlasma3;

	if(m_hBitmap)
		::DeleteObject(m_hBitmap);
}


BEGIN_MESSAGE_MAP(CCloudsCtrl, CStatic)
	//{{AFX_MSG_MAP(CCloudsCtrl)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/********************************************************************/
/*																	*/
/* Function name : PreSubclassWindow								*/
/* Description   : Initialize some stuff							*/
/*																	*/
/********************************************************************/
void CCloudsCtrl::PreSubclassWindow() 
{
	CStatic::PreSubclassWindow();

	CRect rect;
	GetClientRect(&rect);

	m_nWidth = rect.Width();
	m_nHeight = rect.Height();

	m_pCloudPlasma1 = new int[m_nWidth * m_nHeight];
	m_pCloudPlasma2 = new int[m_nWidth * m_nHeight];
	m_pCloudPlasma3 = new int[m_nWidth * m_nHeight];

	int nBits = (((m_nWidth * 32 + 31) & ~31) >> 3) * m_nHeight;

	memset(&m_bmpInfo, 0, sizeof(m_bmpInfo));
	m_bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bmpInfo.bmiHeader.biWidth = m_nWidth;
	m_bmpInfo.bmiHeader.biHeight = m_nHeight;
	m_bmpInfo.bmiHeader.biSizeImage = nBits;
	m_bmpInfo.bmiHeader.biPlanes = 1;
	m_bmpInfo.bmiHeader.biBitCount = 32;

	CDC *pDC = GetDC();
	m_hBitmap = CreateDIBSection(pDC->m_hDC, &m_bmpInfo, DIB_RGB_COLORS, (void**)&m_pPixels, NULL, 0);

	// create credits dc
	LoadCredits(pDC);

	ReleaseDC(pDC);

	m_nSizeOfBits = (nBits - (m_nWidth * 2))/sizeof(RGBQUAD);

	// initialize randomizer
	srand((unsigned)time(NULL));

	// initialize random noise
	GeneratePlasma(m_pCloudPlasma1);
	GeneratePlasma(m_pCloudPlasma2);
	GeneratePlasma(m_pCloudPlasma3);

	// initialize blue color map
	SetExponentialClouds(FALSE);

	// set scroll counter
	m_nCounter = m_nHeight + 25;

	DrawCtrl();

	SetTimer(1, 50, NULL);
}


/********************************************************************/
/*																	*/
/* Function name : OnPaint											*/
/* Description   : Called when the application makes a request to	*/
/*				   repaint a portion of the window.					*/
/*																	*/
/********************************************************************/
void CCloudsCtrl::OnPaint() 
{
	CPaintDC dc(this); 
	
	CRect rect;
	GetClientRect(&rect);

	CDC memDC;
	CBitmap memBitmap;
	CBitmap* oldBitmap;

	memDC.CreateCompatibleDC(&dc);
	memBitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	oldBitmap = (CBitmap *)memDC.SelectObject(&memBitmap);

	if (memDC.GetSafeHdc() != NULL)
	{
		// copy image to memory dc
		memDC.BitBlt(0, 0, rect.Width(), rect.Height(), &m_MainDC, 0, 0, SRCCOPY);
		// send result to the display
		dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
	}
	memDC.SelectObject(oldBitmap);
}


/********************************************************************/
/*																	*/
/* Function name : DrawCtrl											*/
/* Description   : Render the clouds to a bitmap.					*/
/*																	*/
/********************************************************************/
void CCloudsCtrl::DrawCtrl()
{
	CRect rect;
	GetClientRect(&rect);

	// in case we haven't established the memory dc's
	CClientDC dc(this);

	// if we don't have one yet, set up a memory dc for the control 
	if (m_MainDC.GetSafeHdc() == NULL)
	{
		m_MainDC.CreateCompatibleDC(&dc);
		m_MainBitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
		m_pOldBitmap = m_MainDC.SelectObject(&m_MainBitmap);
	}

	// update cloud image
	UpdateCloudImage();

	// copy cloud image to decive context
	SetDIBitsToDevice(m_MainDC.m_hDC,
					  0,
					  0,
					  m_nWidth,
					  m_nHeight,
					  0,
					  0,
					  0,
					  m_nHeight,
					  (LPVOID)m_pPixels,
					  &m_bmpInfo,
					  DIB_RGB_COLORS);

	// copy credits in memory dc
	m_MainDC.BitBlt(0, m_nCounter-=3, m_nWidth, m_nHeight, &m_CreditsDC, 0, 0, MERGEPAINT);

	if (m_nCounter < -m_nHeight)
	{
		m_nCounter = m_nHeight;
	}

	// finally, force redraw
	Invalidate();
} 


/********************************************************************/
/*																	*/
/* Function name : GeneratePlasma									*/
/* Description   : Fill the noise array with random integers		*/
/*																	*/
/* Credits:		   Partially based on code by Andrea Griffini.		*/ 
/*																	*/
/********************************************************************/
void CCloudsCtrl::GeneratePlasma(int byteArray[])
{
	int noiseGrid[256][256];
	
    // initialize to zero
    for (int i=0; i < 256; i++) 
	{
		for (int j=0; j < 256; j++) 
		{
			noiseGrid[i][j] = 0;
		}
    }

	// create plasma
	for(int z = 256; z >= 2; z /= 2)
	{
		int nDensity = z * m_nDensity;

		for(int x = 0; x < 256; x += z)
		{
			int x1 = (z + x) % 256;
			int x2 = z/2 + x;

			for(int y = 0; y < 256; y += z)
			{
				int y1 = (z + y) % 256;
				int y2 = z/2 + y;

				// top-left
				int nValue1 = noiseGrid[x][y];
				// top-right
				int nValue2 = noiseGrid[x1][y];
				// bottom-left
				int nValue3 = noiseGrid[x][y1];
				// bottom-right
				int nValue4 = noiseGrid[x1][y1];
				
				// central point
				noiseGrid[x2][y2] = GetRandomValue((nValue1 + nValue2 + nValue3 + nValue4) /4, nDensity);

				// top point (of top row) 
				if (x == 0)
					noiseGrid[x2][y] = GetRandomValue((nValue1 + nValue2) /2, nDensity);

				// left point (of left column) 
				if (y == 0)
					noiseGrid[x][y2] = GetRandomValue((nValue1 + nValue3) /2, nDensity);

				// bottom point
				noiseGrid[x2][y1] = GetRandomValue((nValue3 + nValue4) / 2, nDensity);

				// right point
				noiseGrid[x1][y2] = GetRandomValue((nValue2 + nValue4) / 2, nDensity);
			}
		}
	}

	// tile generated noise grid into noise array
	double xStep = (double)256 / (double)m_nWidth;
    double yStep = (double)256 / (double)m_nHeight;

	int nCount = 0;

    for(int row = 0; row < m_nHeight; row++)
    {
		int y = (int)((double)row * yStep);

        for(int col = 0; col < m_nWidth; col++)
        {
			int x = (int)((double)col * xStep);
            byteArray[nCount++] = noiseGrid[x][y];
		}
	}
}


/********************************************************************/
/*																	*/
/* Function name : GetRandomValue									*/
/* Description   : Get random value (within the range 0 .. 255)		*/
/*																	*/
/********************************************************************/
int CCloudsCtrl::GetRandomValue(int nBase, int nDelta)
{
	double random = (double)(rand()%1000)/(double)1000;

	int nValue = nBase + (int)(random * (double)nDelta) - nDelta/2;
    nValue = nValue < 0 ? 0 : nValue > 255 ? 255 : nValue;
	return nValue;
}


/********************************************************************/
/*																	*/
/* Function name : CloudExpCurve									*/
/* Description   : Used to let the plasma more look like a cloud	*/
/*																	*/
/********************************************************************/
double CCloudsCtrl::CloudExpCurve(double value)
{
	double nSharpness = 0.575;
	double nCoverRate = 0.475;
	double nSkyRate = 1 - nCoverRate;

	double c1 = __max(0, value - nSkyRate);
	double c2 = 1.0/nSharpness - 1.0;
	return pow(sin(c1/(1-nSkyRate)*3.14159265359/2.0), c2);
}


/********************************************************************/
/*																	*/
/* Function name : UpdateCloudImage									*/
/* Description   : Update cloud image								*/
/*																	*/
/********************************************************************/
void CCloudsCtrl::UpdateCloudImage()
{
	if (!m_bInterActive)
    {
		m_ptMove = m_ptDirection;
	}

	// calculate offsets
	m_ptOffset1.x = (m_ptOffset1.x + m_nWidth + m_ptMove.x) % m_nWidth;
	m_ptOffset1.y = (m_ptOffset1.y + m_nHeight + m_ptMove.y) % m_nHeight;

	m_ptOffset2.x = (m_ptOffset2.x + m_nWidth + m_ptMove.x/2) % m_nWidth;
	m_ptOffset2.y = (m_ptOffset2.y + m_nHeight + m_ptMove.y/2) % m_nHeight;

	m_ptOffset3.x = (m_ptOffset2.x + m_nWidth + m_ptMove.x/3) % m_nWidth;
	m_ptOffset3.y = (m_ptOffset2.y + m_nHeight + m_ptMove.y/3) % m_nHeight;

	// walk through rows
	for(int row = 0; row < m_nHeight; row++)
	{
		int nCount = row * m_nWidth;
		
		int nIndex1 = ((row + m_ptOffset1.y) % m_nHeight) * m_nWidth;
		int nIndex2 = ((row + m_ptOffset2.y) % m_nHeight) * m_nWidth;
		int nIndex3 = ((row + m_ptOffset3.y) % m_nHeight) * m_nWidth;
				
		// walk through columns
		for(int col = 0; col < m_nWidth; col++)
		{
			// calculate average intensity of pixel
			int nValue1 = m_pCloudPlasma1[(col + m_ptOffset1.x) % m_nWidth + nIndex1];
			int nValue2 = m_pCloudPlasma2[(col + m_ptOffset2.x) % m_nWidth + nIndex2];
			int nValue3 = m_pCloudPlasma3[(col + m_ptOffset3.x) % m_nWidth + nIndex3];

			// blend colors
			int nBlueValue = (nValue1 + nValue2 + nValue3) / 3;

			// set blue value for this pixel
			m_pPixels[nCount + col] = m_BlueColorMap[nBlueValue];
		}
	}

}


/********************************************************************/
/*																	*/
/* Function name : OnTimer											*/
/* Description   : Handle timer events								*/
/*																	*/
/********************************************************************/
void CCloudsCtrl::OnTimer(UINT nIDEvent) 
{
	DrawCtrl();		
	CStatic::OnTimer(nIDEvent);
}


/********************************************************************/
/*																	*/
/* Function name : OnMouseMove										*/
/* Description   : Change movement direction						*/
/*																	*/
/********************************************************************/
void CCloudsCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_ptCursor.x < point.x)
		m_ptMove.x = -m_nSpeed;
	else
	if (m_ptCursor.x > point.x)
		m_ptMove.x = m_nSpeed;
	else
		m_ptMove.x = 0;

	if (m_ptCursor.y < point.y)
		m_ptMove.y = m_nSpeed;
	else
	if (m_ptCursor.y > point.y)
		m_ptMove.y = -m_nSpeed;
	else
		m_ptMove.y = 0;

	m_ptCursor = point;

	CStatic::OnMouseMove(nFlags, point);
}


BOOL CCloudsCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;//CStatic::OnEraseBkgnd(pDC);
}


/********************************************************************/
/*																	*/
/* Function name : SetDensity										*/
/* Description   : Set cloud density								*/
/*																	*/
/********************************************************************/
void CCloudsCtrl::SetDensity(int nValue)
{
	m_nDensity = nValue;
}


/********************************************************************/
/*																	*/
/* Function name : SetInteractive									*/
/* Description   : Set interactive mode								*/
/*																	*/
/********************************************************************/
void CCloudsCtrl::SetInteractive(BOOL bFlag)
{
	m_bInterActive = bFlag;
}


/********************************************************************/
/*																	*/
/* Function name : SetSpeed											*/
/* Description   : Set speed of the clouds							*/
/*																	*/
/********************************************************************/
void CCloudsCtrl::SetSpeed(int nValue)
{
	m_nSpeed = nValue;
}


/********************************************************************/
/*																	*/
/* Function name : LoadCredits										*/
/* Description   : Create credits picture into device context		*/
/*																	*/
/********************************************************************/
void CCloudsCtrl::LoadCredits(CDC *pDC)
{
	CBitmap bitmap;
	CBitmap *pOldBitmap;

	// Create the DC
	m_CreditsDC.CreateCompatibleDC(pDC);

	bitmap.CreateCompatibleBitmap(pDC, m_nWidth, m_nHeight);		

	pOldBitmap = (CBitmap *)m_CreditsDC.SelectObject(&bitmap);

	CRect rect(0, 0, m_nWidth, m_nHeight);

	CFont fontTitle, fontBold, fontNormal;

	m_CreditsDC.FillSolidRect(&rect, RGB(255,255,255));

	// create a bunch of fonts
	fontTitle.CreateFont(38, 0, 0, 0, 
 							FW_BOLD, FALSE, FALSE, 0, 
   							DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,
                PROOF_QUALITY,
                VARIABLE_PITCH | 0x04 | FF_DONTCARE,
                (LPSTR)"¡• È");

	fontBold.CreateFont(18, 0, 0, 0, 
 							FW_NORMAL, FALSE, FALSE, 0, 
   							DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,
                PROOF_QUALITY,
                VARIABLE_PITCH | 0x04 | FF_DONTCARE,
                (LPSTR)"ÀŒÃÂ");

	fontNormal.CreateFont(14, 0, 0, 0, 
 							FALSE, FALSE, FALSE, 0, 
   							ANSI_CHARSET,
                OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,
                PROOF_QUALITY,
                VARIABLE_PITCH | 0x04 | FF_DONTCARE,
                (LPSTR)"Arial");
	
	int nSavedDC = m_CreditsDC.SaveDC();

	CString strSub;
	int nCount=0;
	// draw each line, based on specified type
	while(AfxExtractSubString(strSub, m_strCredits, nCount++, '\n'))
	{
		TCHAR nType = 0;

		if (!strSub.IsEmpty())
			nType = strSub.GetAt(0);

		switch(nType)
		{
			case '\t':	// title
				m_CreditsDC.SetTextColor(RGB(0,0,0));
				m_CreditsDC.SelectObject(fontTitle);
				strSub.TrimLeft('\t');
				m_CreditsDC.DrawText(strSub, strSub.GetLength(), &rect, DT_TOP | DT_CENTER | DT_NOPREFIX | DT_SINGLELINE);
				break;
			case '\r':	// bold
				m_CreditsDC.SetTextColor(RGB(0,0,0));
				m_CreditsDC.SelectObject(fontBold);
				strSub.TrimLeft('\r');
				m_CreditsDC.DrawText(strSub, strSub.GetLength(), &rect, DT_TOP | DT_CENTER | DT_NOPREFIX | DT_SINGLELINE);
				break;
			default:	// normal
				m_CreditsDC.SetTextColor(RGB(0,0,0));
				m_CreditsDC.SelectObject(fontNormal);
				m_CreditsDC.DrawText(strSub, strSub.GetLength(), &rect, DT_TOP | DT_CENTER | DT_NOPREFIX | DT_SINGLELINE);
				break;
		}
		// next line
		TEXTMETRIC tm;  
		m_CreditsDC.GetTextMetrics(&tm);
		rect.top += tm.tmHeight;
	}

	m_CreditsDC.SelectObject(pOldBitmap);

	m_CreditsDC.RestoreDC(nSavedDC);
}


/********************************************************************/
/*																	*/
/* Function name : SetCredits										*/
/* Description   : Member function to set credits text.				*/
/*																	*/
/********************************************************************/
void CCloudsCtrl::SetCredits(LPCTSTR lpszCredits)
{
	m_strCredits = lpszCredits;
}


/********************************************************************/
/*																	*/
/* Function name : SetExponentialClouds								*/
/* Description   : Use exponential function to generate clouds		*/
/*																	*/
/********************************************************************/
void CCloudsCtrl::SetExponentialClouds(BOOL bFlag)
{
	// initialize blue color map
    for(int i = 0; i < 256; i++)
	{
		int nValue = bFlag ? (int)(255*CloudExpCurve(i/256.0)) : i;

		m_BlueColorMap[i] = nValue << 16 | nValue << 8 | 255;
	}
}

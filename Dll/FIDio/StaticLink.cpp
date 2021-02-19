/****************************************************************/
/*																*/
/*  StaticLink.cpp	 											*/
/*																*/
/*  Implementation of the CStaticLink class.					*/
/*																*/
/*  Programmed by Pablo van der Meer							*/
/*  Copyright Pablo Software Solutions 2002						*/
/*	http://www.pablovandermeer.nl								*/
/*																*/
/*  Last updated: 15 may 2002									*/
/*																*/
/****************************************************************/


#include "stdafx.h"
#include "FIDRes.h"
#include "StaticLink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CStaticLink::CStaticLink()
{
	m_clrText = RGB(0,0,255);
	m_strURL = "http://10.132.200.177";
	m_hCursor = AfxGetApp()->LoadCursor(FID_IDC_FINGER);
}

CStaticLink::~CStaticLink()
{
}


BEGIN_MESSAGE_MAP(CStaticLink, CStatic)
	//{{AFX_MSG_MAP(CStaticLink)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_NCHITTEST()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/********************************************************************/
/*																	*/
/* Function name : CtlColor											*/
/* Description   : Change text color of static control.				*/
/*																	*/
/********************************************************************/
HBRUSH CStaticLink::CtlColor(CDC* pDC, UINT nCtlColor) 
{
    pDC->SetTextColor(m_clrText);
    pDC->SetBkMode(TRANSPARENT);

    // return hollow brush to let parent background color shine through
    return (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
}


/********************************************************************/
/*																	*/
/* Function name : PreSubclassWindow								*/
/* Description   : Called just before the window is subclassed.		*/
/*																	*/
/********************************************************************/
void CStaticLink::PreSubclassWindow() 
{
	// set up font
	LOGFONT lf;
    GetFont()->GetObject(sizeof(lf), &lf);
    lf.lfUnderline = TRUE;
    m_font.CreateFontIndirect(&lf);

	SetFont(&m_font);
	
	CStatic::PreSubclassWindow();
}


/********************************************************************/
/*																	*/
/* Function name : SetURL											*/
/* Description   : Member function to set URL.						*/
/*																	*/
/********************************************************************/
void CStaticLink::SetURL(LPCTSTR lpszURL)
{
	m_strURL = lpszURL;
}


/********************************************************************/
/*																	*/
/* Function name : OnNcHitTest										*/
/* Description   : 'Activate' mouse messages.						*/
/*																	*/
/********************************************************************/
UINT CStaticLink::OnNcHitTest(CPoint point) 
{
	return HTCLIENT;
}


/********************************************************************/
/*																	*/
/* Function name : OnSetCursor										*/
/* Description   : Change default cursor to finger.					*/
/*																	*/
/********************************************************************/
BOOL CStaticLink::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor) 
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}	
	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}


/********************************************************************/
/*																	*/
/* Function name : OnLButtonDown									*/
/* Description   : Link has been clicked, launch explorer.			*/
/*																	*/
/********************************************************************/
void CStaticLink::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_strURL.IsEmpty())
	{
		// get it from window text
		GetWindowText(m_strURL);  
	}
 
    // open URL in the browser.
	ShellExecute(NULL, "open", m_strURL, NULL, NULL, SW_SHOWNORMAL);	

	CStatic::OnLButtonDown(nFlags, point);
}

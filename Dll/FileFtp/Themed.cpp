// Themed.cpp: implementation of the CThemed class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Themed.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// theme function definitions
typedef BOOL (STDAPICALLTYPE *PFNISTHEMEACTIVE)();
typedef DWORD (STDAPICALLTYPE *PFNGETTHEMEAPPPROPERTIES)();
typedef HTHEME (STDAPICALLTYPE *PFNOPENTHEMEDATA)(HWND hwnd, LPCWSTR pszClassList);
typedef HRESULT (STDAPICALLTYPE *PFNCLOSETHEMEDATA)(HTHEME hTheme);
typedef HRESULT (STDAPICALLTYPE *PFNDRAWTHEMEBACKGROUND)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, 
									   const RECT *pRect, const RECT *pClipRect);
typedef HRESULT (STDAPICALLTYPE *PFNDRAWTHEMETEXT)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, 
								 LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, 
								 DWORD dwTextFlags2, const RECT *pRect);
typedef HRESULT (STDAPICALLTYPE *PFNDRAWTHEMEEDGE)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, 
								 const RECT *pDestRect, UINT uEdge, UINT uFlags, 
								 RECT *pContentRect);
typedef HRESULT (STDAPICALLTYPE *PFNDRAWTHEMEICON)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, 
								 const RECT *pRect, HIMAGELIST himl, int iImageIndex);
typedef HRESULT (STDAPICALLTYPE *PFNDRAWTHEMEBORDER)(HTHEME hTheme, HDC hdc, int iStateId, const RECT *pRect);

#define WSTR(string) (LPCWSTR)COleVariant(string).bstrVal

#ifndef DFCS_TRANSPARENT
#	define DFCS_TRANSPARENT        0x0800
#	define DFCS_HOT                0x1000
#	define DFC_POPUPMENU           5
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HMODULE CThemed::s_hUxTheme = NULL;

CThemed::CThemed(CWnd* pWnd, LPCTSTR szClassList) : m_hTheme(NULL)
{
	InitUxTheme();

	if (pWnd && pWnd->GetSafeHwnd())
		Open(pWnd, szClassList);
}

CThemed::~CThemed()
{
	Close();
}

BOOL CThemed::InitUxTheme()
{
	if (!s_hUxTheme)
		s_hUxTheme = LoadLibrary("UxTheme.dll");

	return (NULL != s_hUxTheme);
}

BOOL CThemed::SupportsTheming(int nFlag)
{
	return (IsThemeActive() && (GetAppThemeProperties() & nFlag));
}

BOOL CThemed::Open(const CWnd* pWnd, LPCTSTR szClassList)
{
	if (pWnd && pWnd->GetSafeHwnd())
	{
		if (!s_hUxTheme)
		{
			if (!(s_hUxTheme = LoadLibrary("UxTheme.dll")))
				return FALSE;
		}
		else
			Close();

		CString sClassList(szClassList);

		if (sClassList.IsEmpty())
		{
			::GetClassName(pWnd->GetSafeHwnd(), sClassList.GetBuffer(128), 128);
			sClassList.ReleaseBuffer();
		}

		m_hTheme = OpenThemeData(pWnd->GetSafeHwnd(), WSTR(sClassList));

		return (NULL != m_hTheme);
	}

	return FALSE;
}

BOOL CThemed::Close()
{
	if (CloseThemeData(m_hTheme))
		m_hTheme = NULL;

	return (NULL == m_hTheme);
}

BOOL CThemed::AreControlsThemed()
{
	return SupportsTheming(STAP_ALLOW_CONTROLS);
}

BOOL CThemed::IsNonClientThemed()
{
	return SupportsTheming(STAP_ALLOW_NONCLIENT);
}

BOOL CThemed::IsWebContentThemed()
{
	return SupportsTheming(STAP_ALLOW_WEBCONTENT);
}

// static 
BOOL CThemed::DrawFrameControl(const CWnd* pWnd, CDC* pDC, LPRECT pRect, UINT nType, UINT nState, LPCRECT pClip)
{
	CThemed th;

	if (th.SupportsTheming(STAP_ALLOW_CONTROLS) && th.SupportsTheming(STAP_ALLOW_NONCLIENT))
	{
		// determine the appropriate class, part and state
		CString sThClass;
		int nThPart = 0, nThState = 0;

		if (!th.GetThemeClassPartState(nType, nState, sThClass, nThPart, nThState))
			return FALSE;

		if (!th.Open(pWnd, sThClass))
			return FALSE;

		th.DrawBackground(pDC, nThPart, nThState, pRect, pClip);

		return TRUE;
	}
	else
		return pDC->DrawFrameControl(pRect, nType, nState);
}

BOOL CThemed::DrawEdge(const CWnd* pWnd, CDC* pDC, LPRECT pRect, UINT nType, UINT nState, UINT nEdge, UINT nFlags)
{
	CThemed th;

	if (th.SupportsTheming(STAP_ALLOW_CONTROLS) && th.SupportsTheming(STAP_ALLOW_NONCLIENT))
	{
		// determine the appropriate class, part and state
		CString sThClass;
		int nThPart = 0, nThState = 0;

		if (!th.GetThemeClassPartState(nType, nState, sThClass, nThPart, nThState))
			return FALSE;

		if (!th.Open(pWnd, sThClass))
			return FALSE;

		th.DrawEdge(pDC, nThPart, nThState, pRect, nEdge, nFlags);

		return TRUE;
	}
	else
		return pDC->DrawFrameControl(pRect, nType, nState);
}

BOOL CThemed::DrawBackground(CDC* pDC, int nPart, int nState, const CRect& rect, LPCRECT prClip)
{
	ASSERT (m_hTheme);
	ASSERT_VALID (pDC);

	return DrawThemeBackground(m_hTheme, *pDC, nPart, nState, rect, prClip);
}

BOOL CThemed::DrawText(CDC* pDC, int nPart, int nState, const CString& sText, DWORD dwTextFlags, DWORD dwTextFlags2, const CRect& rect)
{
	ASSERT (m_hTheme);
	ASSERT_VALID (pDC);

	return DrawThemeText(m_hTheme, *pDC, nPart, nState, WSTR(sText), sText.GetLength(), dwTextFlags, dwTextFlags2, rect);
}

BOOL CThemed::DrawEdge(CDC* pDC, int nPart, int nState, const CRect& rDest, UINT nEdge, UINT nFlags, LPRECT prContent)
{
	ASSERT (m_hTheme);
	ASSERT_VALID (pDC);

	return DrawThemeEdge(m_hTheme, *pDC, nPart, nState, rDest, nEdge, nFlags, prContent);
}

BOOL CThemed::DrawIcon(CDC* pDC, int nPart, int nState, const CRect& rect, const CImageList* pIL, int nImage)
{
	ASSERT (m_hTheme);
	ASSERT_VALID (pDC);
	ASSERT_VALID (pIL);

	return DrawThemeIcon(m_hTheme, *pDC, nPart, nState, rect, *pIL, nImage);
}

BOOL CThemed::DrawBorder(CDC* pDC, int nState, const CRect& rect)
{
	ASSERT (m_hTheme);
	ASSERT_VALID (pDC);

	return DrawThemeBorder(m_hTheme, *pDC, nState, rect);
}

// -----------------------------------------------------------------------------------------------------------

BOOL CThemed::IsThemeActive()
{
	if (s_hUxTheme)
	{
		PFNISTHEMEACTIVE fnIsThemeActive = (PFNISTHEMEACTIVE)GetProcAddress(s_hUxTheme, "IsThemeActive");

		if (fnIsThemeActive)
			return fnIsThemeActive();
	}

	return FALSE;
}

DWORD CThemed::GetAppThemeProperties()
{
	if (s_hUxTheme)
	{
		PFNGETTHEMEAPPPROPERTIES fnGetThemeAppProperties = (PFNGETTHEMEAPPPROPERTIES)GetProcAddress(s_hUxTheme, "GetThemeAppProperties");

		if (fnGetThemeAppProperties)
			return fnGetThemeAppProperties();
	}

	return 0;
}

HTHEME CThemed::OpenThemeData(HWND hwnd, LPCWSTR pszClassList)
{
	if (s_hUxTheme)
	{
		PFNOPENTHEMEDATA fnOpenThemeData = (PFNOPENTHEMEDATA)GetProcAddress(s_hUxTheme, "OpenThemeData");

		if (fnOpenThemeData)
			return fnOpenThemeData(hwnd, pszClassList);
	}

	return NULL;
}

BOOL CThemed::CloseThemeData(HTHEME hTheme)
{
	if (s_hUxTheme && hTheme)
	{
		PFNCLOSETHEMEDATA fnCloseThemeData = (PFNCLOSETHEMEDATA)GetProcAddress(s_hUxTheme, "CloseThemeData");

		if (fnCloseThemeData)
			return SUCCEEDED(fnCloseThemeData(m_hTheme));
	}

	return FALSE;
}

BOOL CThemed::DrawThemeBackground(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect,
						const RECT *pClipRect)
{
	if (s_hUxTheme && m_hTheme)
	{
		PFNDRAWTHEMEBACKGROUND fnDrawThemeBackground = (PFNDRAWTHEMEBACKGROUND)GetProcAddress(s_hUxTheme, "DrawThemeBackground");

		if (fnDrawThemeBackground)
			return (SUCCEEDED(fnDrawThemeBackground(m_hTheme, hdc, iPartId, iStateId, pRect, pClipRect)));
	}

	return FALSE;
}

BOOL CThemed::DrawThemeText(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, 
					DWORD dwTextFlags, DWORD dwTextFlags2, const RECT *pRect)
{
	if (s_hUxTheme && m_hTheme)
	{
		PFNDRAWTHEMETEXT fnDrawThemeText = (PFNDRAWTHEMETEXT)GetProcAddress(s_hUxTheme, "DrawThemeText");

		if (fnDrawThemeText)
			return (SUCCEEDED(fnDrawThemeText(m_hTheme, hdc, iPartId, iStateId, pszText, iCharCount, dwTextFlags, dwTextFlags2, pRect)));
	}

	return FALSE;
}

BOOL CThemed::DrawThemeEdge(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pDestRect, UINT uEdge, 
					UINT uFlags, RECT *pContentRect)
{
	if (s_hUxTheme && m_hTheme)
	{
		PFNDRAWTHEMEEDGE fnDrawThemeEdge = (PFNDRAWTHEMEEDGE)GetProcAddress(s_hUxTheme, "DrawThemeEdge");

		if (fnDrawThemeEdge)
			return (SUCCEEDED(fnDrawThemeEdge(m_hTheme, hdc, iPartId, iStateId, pDestRect, uEdge, uFlags, pContentRect)));
	}

	return FALSE;
}

BOOL CThemed::DrawThemeIcon(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect, HIMAGELIST himl, 
					int iImageIndex)
{
	if (s_hUxTheme && m_hTheme)
	{
		PFNDRAWTHEMEICON fnDrawThemeIcon = (PFNDRAWTHEMEICON)GetProcAddress(s_hUxTheme, "DrawThemeIcon");

		if (fnDrawThemeIcon)
			return (SUCCEEDED(fnDrawThemeIcon(m_hTheme, hdc, iPartId, iStateId, pRect, himl, iImageIndex)));
	}

	return FALSE;
}

BOOL CThemed::DrawThemeBorder(HTHEME hTheme, HDC hdc, int iStateId, const RECT *pRect)
{
	if (s_hUxTheme && m_hTheme)
	{
		PFNDRAWTHEMEBORDER fnDrawThemeBorder = (PFNDRAWTHEMEBORDER)GetProcAddress(s_hUxTheme, "DrawThemeBorder");

		if (fnDrawThemeBorder)
			return (SUCCEEDED(fnDrawThemeBorder(m_hTheme, hdc, iStateId, pRect)));
	}

	return FALSE;
}

// -----------------------------------------------------------------------------------------------------------

BOOL CThemed::GetThemeClassPartState(int nType, int nState, CString& sThClass, int& nThPart, int& nThState)
{
	sThClass.Empty();
	nThPart = 0;
	nThState = 0;

	switch (nType)
	{
	case DFC_BUTTON:
		{
			sThClass = "BUTTON";
			nThState = PBS_NORMAL;
			
			if (nState & DFCS_BUTTONPUSH) 
			{
				nThPart = BP_PUSHBUTTON;
				
				if (nState & (DFCS_CHECKED | DFCS_PUSHED))
					nThState = PBS_PRESSED;
				
				else if (nState & DFCS_INACTIVE)
					nThState = PBS_DISABLED;
				
				else if (nState & DFCS_HOT)
					nThState = PBS_HOT;
			}
/*			else if (nState & DFCS_BUTTONRADIO) 
			{
				nThPart = BP_RADIOBUTTON;
			}
			else if (nState & DFCS_BUTTONCHECK) 
			{
				nThPart = BP_CHECKBOX;
			
			  if (nState & (DFCS_CHECKED | DFCS_PUSHED))
			  nThState = PBS_PRESSED;
			  
				else if (nState & DFCS_INACTIVE)
				nThState = PBS_DISABLED;
				
				  else if (nState & DFCS_HOT)
				  nThState = PBS_HOT;
				  }
			*/				else 
			return FALSE;
		}
		break;
		
	case DFC_CAPTION:
		break;
		
	case DFC_MENU:
		break;
		
	case DFC_POPUPMENU:
		break;
		
	case DFC_SCROLL:
		{
			if (nState & DFCS_SCROLLCOMBOBOX) 
			{
				sThClass = "COMBOBOX";
				nThPart = CP_DROPDOWNBUTTON;
				nThState = CBXS_NORMAL;
				
				if (nState & (DFCS_CHECKED | DFCS_PUSHED))
					nThState = CBXS_PRESSED;
				
				else if (nState & DFCS_INACTIVE)
					nThState = CBXS_DISABLED;
				
				else if (nState & DFCS_HOT)
					nThState = CBXS_HOT;
			}
			else if (nState & DFCS_SCROLLSIZEGRIP)
			{
				sThClass = "SCROLLBAR";
				nThPart = SBP_SIZEBOX;
				nThState = (nState & DFCS_SCROLLLEFT) ? SZB_LEFTALIGN : SZB_RIGHTALIGN;
			}
			// else
		}
		break;
	}

	return (sThClass.IsEmpty() || (nThPart && nThState));
}

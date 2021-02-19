// Themed.h: interface for the CThemed class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THEMED_H__2FA586FE_B790_4315_93B9_01000725B68A__INCLUDED_)
#define AFX_THEMED_H__2FA586FE_B790_4315_93B9_01000725B68A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "uxtheme.h"
#include "TmSchema.h" // for part and state enumerations

#if (WINVER < 0x0500)
#	define DFCS_TRANSPARENT        0x0800
#	define DFCS_HOT                0x1000
#	define DFC_POPUPMENU           5
#endif

class CThemed  
{
public:
	CThemed(CWnd* pWnd = NULL, LPCTSTR szClassList = NULL);
	virtual ~CThemed();

	BOOL AreControlsThemed();
	BOOL IsNonClientThemed();
	BOOL IsWebContentThemed();

	static BOOL DrawFrameControl(const CWnd* pWnd, CDC* pDC, LPRECT pRect, UINT nType, UINT nState, LPCRECT prClip = NULL);
	static BOOL DrawEdge(const CWnd* pWnd, CDC* pDC, LPRECT pRect, UINT nType, UINT nState, UINT nEdge, UINT nFlags);
	static BOOL DrawCaption(const CWnd* pWnd, CDC* pDC, LPRECT pRect, UINT nFlags);

	BOOL Open(const CWnd* pWnd, LPCTSTR szClassList);
	BOOL IsValid() { return (s_hUxTheme && m_hTheme); }
	BOOL Close();

	BOOL DrawBackground(CDC* pDC, int nPart, int nState, const CRect& rect, LPCRECT prClip = NULL);
	BOOL DrawText(CDC* pDC, int nPart, int nState, const CString& sText, DWORD dwTextFlags, DWORD dwTextFlags2, const CRect& pRect);
	BOOL DrawEdge(CDC* pDC, int nPart, int nState, const CRect& rDest, UINT nEdge, UINT nFlags, LPRECT prContent = NULL);
	BOOL DrawIcon(CDC* pDC, int nPart, int nState, const CRect& rect, const CImageList* pIL, int nImage);
	BOOL DrawBorder(CDC* pDC, int nState, const CRect& rect);

protected:
	static HMODULE s_hUxTheme;
	HTHEME m_hTheme;

protected:
	BOOL SupportsTheming(int nFlag);
	BOOL IsThemeActive();
	BOOL InitUxTheme();

	// uxtheme functions
	DWORD GetAppThemeProperties();
	HTHEME OpenThemeData(HWND hwnd, LPCWSTR pszClassList);
	BOOL CloseThemeData(HTHEME hTheme);
	BOOL DrawThemeBackground(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect, 
					const RECT *pClipRect);
	BOOL DrawThemeText(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, 
					DWORD dwTextFlags, DWORD dwTextFlags2, const RECT *pRect);
	BOOL DrawThemeEdge(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pDestRect, UINT uEdge, 
					UINT uFlags, RECT *pContentRect);
	BOOL DrawThemeIcon(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect, HIMAGELIST himl, 
					int iImageIndex);
	BOOL DrawThemeBorder(HTHEME hTheme, HDC hdc, int iStateId, const RECT *pRect);

	// helpers
	static BOOL GetThemeClassPartState(int nType, int nState, CString& sThClass, int& nThPart, int& nThState); 
};

#endif // !defined(AFX_THEMED_H__2FA586FE_B790_4315_93B9_01000725B68A__INCLUDED_)

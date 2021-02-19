/*----------------------------------------------------------------------------*/
/*            CFaderWnd header                                                */
/*----------------------------------------------------------------------------*/
//
//	File:			FaderWnd.h
//	Author:		Phil J. Pearson
//	Created:	20 June 2000  14:26
//	Last Mod: 20 June 2000  14:47
//
/*----------------------------------------------------------------------------
/*
/* Copyright (C) 2000 by Real World Software
/* All Rights Reserved.
/*
/*----------------------------------------------------------------------------*/

#pragma once

#ifndef WS_EX_LAYERED
/////////////////////////////////////////////////////////////////////////////
// Definitions copied from WinUser.h
// They're normally available if you do:
//	#define _WIN32_WINNT 0x0500
//   but we don't need all that stuff for this class
//
#define WS_EX_LAYERED           0x00080000
#define ULW_ALPHA               0x00000002
#endif // ndef WS_EX_LAYERED


typedef BOOL (WINAPI *lpfnUpdateLayeredWindow)(HWND hWnd, HDC hdcDst, POINT *pptDst, SIZE *psize, HDC hdcSrc, 
																								POINT *pptSrc, COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags);


/////////////////////////////////////////////////////////////////////////////
// CFaderWnd window

class CFaderWnd : public CWnd
{
// Construction
public:
	CFaderWnd(CWnd *pWndToFade, UINT nFadeTime = 2000, BYTE byAlpha = 255);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFaderWnd)
	protected:
	virtual void PostNcDestroy() { delete this; }
	//}}AFX_VIRTUAL

// Implementation
private:
	virtual ~CFaderWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFaderWnd)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	lpfnUpdateLayeredWindow m_pUpdateLayeredWindow;

	BLENDFUNCTION m_Blend;
	CWnd			 *m_pWndToFade;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


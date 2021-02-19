// WinClasses.cpp: implementation of the CWinClasses class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WinClasses.h"
#include "wclassdefines.h"

#include <afxpriv.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// helper function for everyone to use
void TRACEWND(LPCTSTR szFunctionName, HWND hWnd)
{
#ifdef _DEBUG
	if (hWnd)
	{
		CString sText; 
		//		pWnd->GetWindowText(sText); 
		TRACE ("%s(%s, %s, id = %d)\n", szFunctionName, CWinClasses::GetClass(hWnd), sText, GetDlgCtrlID(hWnd)); 
	}
#endif
}

#define ADDCONTROL(ctrlname) { CString sCtrl(ctrlname); sCtrl.MakeLower(); s_mapCtrlClasses[sCtrl] = NULL; }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapStringToPtr CWinClasses::s_mapCtrlClasses;

CString CWinClasses::GetClass(HWND hWnd)
{
	static char szWndClass[128] = "";
	
	if (hWnd)
	{
		::GetClassName(hWnd, szWndClass, 127);
		_strlwr(szWndClass);

		return CString(szWndClass);
	}

	return "";
}

BOOL CWinClasses::IsClass(HWND hWnd, LPCTSTR szClass)
{
	if (hWnd)
	{
		char szWndClass[128] = "";

		::GetClassName(hWnd, szWndClass, 127);
		return IsClass(szClass, szWndClass);
	}

	return FALSE;
}

CString CWinClasses::GetClassEx(HWND hWnd)
{
	CString sClass = GetClass(hWnd);
	
	if (sClass.Find("Afx") == 0) // its an mfc framework base or derived class
	{
		// can do the check if pWnd is permanent else mfc will not yet
		// have hooked up
		CWnd* pWnd = CWnd::FromHandlePermanent(hWnd);

		if (pWnd)
		{
			// must do the check in order of most derived class first
			if (pWnd->IsKindOf(RUNTIME_CLASS(CView)))
				return WC_MFCVIEW;
			
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)))
				return WC_MFCMDIFRAME;
			
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
				return WC_MFCMDICHILD;
			
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd)))
				return WC_MFCMINIDOCKFRAME;
			
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)))
				return WC_MFCMINIFRAME;
			
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd))) // this is the catch all for frame wnds
				return WC_MFCFRAME;
			
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
				return WC_MFCSPLITTER;
			
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CDialogBar)))
				return WC_MFCDIALOGBAR;
			
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CControlBar)))
				return WC_CONTROLBAR;
			
			else 
				return WC_MFCWND; // catch all for all window classes
		}
	}
	
	return sClass;
}

BOOL CWinClasses::IsControlClass(HWND hWnd)
{
	return IsControlClass(GetClass(hWnd));
}

BOOL CWinClasses::IsControlClass(LPCTSTR szClass)
{
	// init
	if (!s_mapCtrlClasses.GetCount())
	{
		ADDCONTROL(WC_BUTTON);
		ADDCONTROL(WC_STATIC);
		ADDCONTROL(WC_EDIT);  
		ADDCONTROL(WC_COMBOBOX);
		ADDCONTROL(WC_COMBOLBOX);
		ADDCONTROL(WC_LISTBOX);  
		ADDCONTROL(WC_SCROLLBAR);
		ADDCONTROL(WC_TOOLBAR);  
		ADDCONTROL(WC_SPIN);     
		ADDCONTROL(WC_PROGRESS); 
		ADDCONTROL(WC_SLIDER);   
		ADDCONTROL(WC_HOTKEY);   
		ADDCONTROL(WC_SHELLDLLDEFVIEW);  
		ADDCONTROL(WC_STATUSBAR);        
		ADDCONTROL(WC_ANIMATE);          
		ADDCONTROL(WC_RICHEDIT);         
		ADDCONTROL(WC_RICHEDIT20);       
		ADDCONTROL(WC_DATETIMEPICK);     
		ADDCONTROL(WC_MONTHCAL);         
		ADDCONTROL(WC_REBAR);            
		ADDCONTROL(WC_TOOLTIPS); 
		ADDCONTROL(WC_THUMBNAILVIEW); 
		ADDCONTROL(WC_IE); 
		ADDCONTROL(WC_SHDOCOBJVW); 
		ADDCONTROL(WC_SHELLEMBEDDING); 
	}

	void* pTemp;
	CString sClass(szClass);
	sClass.MakeLower();

	return s_mapCtrlClasses.Lookup(sClass, pTemp);
}


// FlowChart.h : main header file for the FLOWCHART application
//

#if !defined(AFX_FLOWCHART_H__689B1C36_68B5_4EB7_9B9B_AFB624193BC6__INCLUDED_)
#define AFX_FLOWCHART_H__689B1C36_68B5_4EB7_9B9B_AFB624193BC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CFlowChartApp:
// See FlowChart.cpp for the implementation of this class
//

class CFlowChartApp : public CWinApp
{
public:
	CFlowChartApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlowChartApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CFlowChartApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
//	virtual BOOL InitApplication();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLOWCHART_H__689B1C36_68B5_4EB7_9B9B_AFB624193BC6__INCLUDED_)

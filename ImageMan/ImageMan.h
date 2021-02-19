// ImageMan.h: main header file for the LOGPLOT application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CImageManApp:
// See LogPlot.cpp for the implementation of this class
//

class CImageManApp : public CWinApp
{
public:
	BOOL m_ProCall;
	short m_DepPos,m_ThickPos;
	BOOL m_Nib,m_MoveDep,m_PointDep,m_PlusValue;
	CString m_TempPath;
	void DelDirAllFile(CString DirName);
	CString m_UnDoFilePath,m_DataBuf,m_UnDoPath;
	CImageManApp();
	CString m_PlotFileName;
	void SetDevMode(CSize size);
	void AppendFilterSuffix(CString& filter, OPENFILENAME& ofn,
			CDocTemplate* pTemplate, CString* pstrDefaultExt);
	BOOL DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags,
			BOOL bOpenFileDialog, CDocTemplate* pTemplate);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageManApp)
	public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
//	COleTemplateServer m_server;
		// Server object for document creation

	//{{AFX_MSG(CImageManApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();
	afx_msg void OnPrintPageSetup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	QGdiPlus m_GdiPlus;
};


/////////////////////////////////////////////////////////////////////////////

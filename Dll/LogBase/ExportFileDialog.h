#if !defined(AFX_EXPORTFILEDIALOG_H__37BCC3F4_4268_4ABC_A2A2_66E5C11EE795__INCLUDED_)
#define AFX_EXPORTFILEDIALOG_H__37BCC3F4_4268_4ABC_A2A2_66E5C11EE795__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExportFileDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExportFileDialog dialog

class CExportFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CExportFileDialog)

public:
	CString m_strType;
	CExportFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExportFileDialog)
	virtual void OnTypeChange();
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CExportFileDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPORTFILEDIALOG_H__37BCC3F4_4268_4ABC_A2A2_66E5C11EE795__INCLUDED_)

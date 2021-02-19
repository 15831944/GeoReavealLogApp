// editdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditdlg dialog

class CEditdlg : public CDialog
{
private:
	LOGFONT logfont;
    CString repltext;
	CString findtext;
    DWORD dwFlags;
	int nExitCode;
public:
	CString szFileName,m_ParaExt,m_MethodName;
	BOOL bReadOnly;    
	CMenu m_Menu;
// Construction
public:
	CEditdlg(CWnd* pParent = NULL);	// standard constructor 

// Dialog Data
	//{{AFX_DATA(CEditdlg)
	enum { IDD = IDD_EDITDLG };
	CEdit m_Editbox;  
	//}}AFX_DATA
	virtual long FileLength(int hFile);
	virtual BOOL FileOpenDlg();
	virtual BOOL FileSaveDlg();	
	virtual BOOL FileRead(CString lpstrFileName);
	virtual BOOL FileWrite(CString lpstrFileName);

    virtual BOOL PopFindFindText ();
    virtual BOOL PopFindNextText ();
	virtual BOOL PopFindReplaceText ();
	virtual BOOL PopFindValidFind (void); 

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support 
	
	CFindReplaceDialog* pFindReplaceDialog;
	BOOL bIsReplaceDialog;
	


protected:
	// Generated message map functions
	//{{AFX_MSG(CEditdlg)
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditFind();
	afx_msg void OnEditPaste();
	afx_msg void OnEditReplace();
	afx_msg void OnEditSelectAll();
	afx_msg void OnEditUndo();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveDefault();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFontFontset();
	afx_msg void OnUpdateFontFontset(CCmdUI* pCmdUI);
	afx_msg void OnEditNext();
	afx_msg void OnUpdateEditNext(CCmdUI* pCmdUI);
	afx_msg LONG FindReplaceHelper(UINT wParam, LONG lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditbox();
	afx_msg void OnEditExit();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnEditReadonly();
	afx_msg void OnUpdateEditReadonly(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnUpdateEditFind(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditReplace(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEditAbout();
	afx_msg void OnEditorHelpIndex();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CAboutEdit dialog

class CAboutEdit : public CDialog
{
// Construction
public:
	CAboutEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAboutEdit)
	enum { IDD = IDD_ABOUTEDIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAboutEdit)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

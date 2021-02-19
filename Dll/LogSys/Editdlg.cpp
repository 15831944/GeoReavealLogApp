// editdlg.cpp : implementation file
//

#include "stdafx.h" 
#include "LogSySres.h"
#include "editdlg.h"
#include <io.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef _UNIX_
static char szPathSep = '/';
#else
static char szPathSep = '\\';
#endif
static const UINT NEAR nFindMsg = ::RegisterWindowMessage(FINDMSGSTRING);
/////////////////////////////////////////////////////////////////////////////
// CEditdlg dialog
extern BOOL FileExist(const char* s);

BOOL FileExist(const char* s)
{
    struct _finddata_t c_file;
	return (_findfirst((char*)s, &c_file) != -1L);
}

CEditdlg::CEditdlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditdlg::IDD, pParent)
{
 	findtext = "";
	repltext = "";
	szFileName = "";
	dwFlags = FR_DOWN  | FR_HIDEWHOLEWORD;
	pFindReplaceDialog = NULL;
	bIsReplaceDialog = FALSE;
	bReadOnly = FALSE;
	nExitCode = 0;
	LOGFONT TempFont=
	{
		-14,					    //lfHeight
		0,							//lfWidth
		0,							//lfEscapement
		0,							//lfOrientation
		100,						//lfWeight
		0,							//lfItalic
		0,							//lfUnderLine;
		0,							//lfStrikeOut;
		ANSI_CHARSET,				//lfCharSet
		OUT_TT_ONLY_PRECIS,			//lfOutPrecision
		CLIP_DEFAULT_PRECIS,		//lfClipPrecision
		DEFAULT_QUALITY,			//lfQuality
		VARIABLE_PITCH | FF_SWISS,	//lfPitchAndFamily
		"Times New Roman" 			//lfFaceName
	};
	HFONT hFont = ::CreateFontIndirect (&TempFont) ;
	m_Editbox.SendMessage(WM_SETFONT, (WPARAM) hFont);
	logfont=TempFont;
}

void CEditdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditdlg)
	DDX_Control(pDX, IDC_EDITBOX, m_Editbox);
	//}}AFX_DATA_MAP
} 

BEGIN_MESSAGE_MAP(CEditdlg, CDialog)
	//{{AFX_MSG_MAP(CEditdlg)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_REPLACE, OnEditReplace)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_SAVE, OnFileSave)
	ON_COMMAND(ID_SAVE_AS_DEFAULT_PARA, OnFileSaveDefault)
	ON_COMMAND(ID_FONT_FONTSET, OnFontFontset)
	ON_UPDATE_COMMAND_UI(ID_FONT_FONTSET, OnUpdateFontFontset)
	ON_COMMAND(ID_EDIT_NEXT, OnEditNext)
	ON_UPDATE_COMMAND_UI(ID_EDIT_NEXT, OnUpdateEditNext)
	ON_REGISTERED_MESSAGE(nFindMsg, FindReplaceHelper)
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_EDITBOX, OnChangeEditbox)
	ON_COMMAND(ID_EDIT_EXIT, OnEditExit)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_EDIT_READONLY, OnEditReadonly)
	ON_UPDATE_COMMAND_UI(ID_EDIT_READONLY, OnUpdateEditReadonly)
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, OnUpdateEditFind)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REPLACE, OnUpdateEditReplace)
	ON_WM_CREATE()
	ON_COMMAND(ID_EDITOR_ABOUT, OnEditAbout)
	ON_COMMAND(ID_EDITOR_HELP_INDEX, OnEditorHelpIndex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditdlg message handlers

//////////////////////////////////////////////////////////////////////////////
//Edit text     
     
void CEditdlg::OnInitMenuPopup(CMenu* pMenu, UINT, BOOL bSysMenu)
{
	BOOL m_bAutoMenuEnable = TRUE;       // auto enable on by default
//	_AfxCancelModes(m_hWnd);

	if (bSysMenu)
		return;     // don't support system menu

	ASSERT(pMenu != NULL);
	// check the enabled state of various menu items

	CCmdUI state;
	state.m_pMenu = pMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// determine if menu is popup in top-level menu and set m_pOther to
	//  it if so (m_pParentMenu == NULL indicates that it is secondary popup)
	HMENU hParentMenu;
//	if (_afxTrackingMenu == pMenu->m_hMenu)
//		state.m_pParentMenu = pMenu;    // parent == child for tracking popup
//	else
	{
		CWnd* pParent = GetTopLevelParent();
			// child windows don't have menus -- need to go to the top!
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pMenu->m_hMenu)
				{
					// when popup is found, m_pParentMenu is containing menu
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
	  state.m_nIndex++)
	{
		state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// possibly a popup menu, route to first item of that popup
			state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // first item of popup can't be routed to
			}
			state.DoUpdate(this, FALSE);    // popups are never auto disabled
		}
		else
		{
			// normal menu item
			// Auto enable/disable if frame window has 'm_bAutoMenuEnable'
			//    set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, m_bAutoMenuEnable && state.m_nID < 0xF000);
		}
	}
}
void CEditdlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_Editbox.m_hWnd != NULL) m_Editbox.MoveWindow(0, 0, cx, cy);
}

BOOL CEditdlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_Menu.LoadMenu(IDR_EDITMENU);
	SetMenu(&m_Menu);
	
	CRect rect;
	GetClientRect(&rect);
	m_Editbox.MoveWindow(0, 0, rect.right, rect.bottom);

	LOGFONT TempFont=
	{
		-13,							//lfHeight
		0,							//lfWidth
		0,							//lfEscapement
		0,							//lfOrientation
		100,						//lfWeight
		0,							//lfItalic
		0,							//lfUnderLine;
		0,							//lfStrikeOut;
		ANSI_CHARSET,				//lfCharSet
		OUT_TT_ONLY_PRECIS,			//lfOutPrecision
		CLIP_DEFAULT_PRECIS,		//lfClipPrecision
		DEFAULT_QUALITY,			//lfQuality
		VARIABLE_PITCH | FF_SWISS,	//lfPitchAndFamily
		"宋体"			 			//lfFaceName
	};
	HFONT hFont = ::CreateFontIndirect (&TempFont) ;
//	GetObject (GetStockObject (SYSTEM_FONT), sizeof (LOGFONT), (LPSTR) &logfont);
//	HFONT hFont = ::CreateFontIndirect (&logfont) ;
	m_Editbox.SendMessage(WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG(1, 0));
    m_Editbox.SetModify(FALSE);
    m_Editbox.EmptyUndoBuffer( );
    m_Editbox.SetReadOnly(bReadOnly);

    if (szFileName !="") 
	{
		if (!FileExist(szFileName)) 
		{
		   	CString m_TemplatePath;
		   	CString m_TemplateName;
			m_TemplateName = m_TemplatePath + AfxGetApp()->m_pszExeName+szPathSep+m_MethodName+"."+m_ParaExt;
			m_TemplateName = m_TemplatePath + AfxGetApp()->m_pszExeName+szPathSep+m_MethodName+"."+m_ParaExt;
			if (FileExist(m_TemplateName))
			{
				CString msg;
				AfxFormatString1(msg,IDS_OPEN_PARFILE_ERROR,szFileName);
				if (AfxMessageBox(msg,MB_ICONEXCLAMATION|MB_YESNOCANCEL) == IDYES) 
				//参数文件打开失败, 采用方法缺省参数吗？
				{
					FileRead(m_TemplateName);
					m_Editbox.SetModify(TRUE);
				}
			}
		}
		else if (!FileRead(szFileName))
			return FALSE;
		CString szMsg;
	    szMsg.LoadString(IDS_EDIT_FILE);
		SetWindowText(szMsg+szFileName);
		m_Editbox.SetSel(0, 0);
		m_Editbox.SetFocus();
    } 
	else 
	{
    	CString szMsg;
	    szMsg.LoadString(IDS_EDIT_FILE1);
		SetWindowText(szMsg);
    	m_Editbox.SetWindowText ("\0");
		m_Editbox.SetModify(FALSE);    
	}    
	return FALSE;  // return TRUE  unless you set the focus to a control
}

void CEditdlg::OnChangeEditbox()
{
	m_Editbox.SetModify();	
}

void CEditdlg::OnEditExit()
{
	if (m_Editbox.GetModify()) 
	{
		CString msg;
		AfxFormatString1(msg,IDS_SAVE_FILE,szFileName);
		UINT ret = AfxMessageBox(msg, MB_YESNOCANCEL | MB_ICONQUESTION );
		if (ret == IDCANCEL) return;
	 	if (ret == IDYES) 
		{
			if ((szFileName =="") && (!FileSaveDlg())) return;
    		if (!FileWrite (szFileName)) return;	
			nExitCode = 1;
	 	} 
	}
	EndDialog(nExitCode);
}

void CEditdlg::OnEditClear()
{
	m_Editbox.Clear();
}

void CEditdlg::OnEditCopy()
{
	m_Editbox.Copy();
}

void CEditdlg::OnEditCut()
{
	m_Editbox.Cut();
}

void CEditdlg::OnEditPaste()
{
	m_Editbox.Paste();
}

void CEditdlg::OnEditSelectAll()
{
	m_Editbox.SetSel(0, -1);
}

void CEditdlg::OnEditUndo()
{
	m_Editbox.Undo();
}

void CEditdlg::OnUpdateEditClear(CCmdUI* pCmdUI)
{
	int nStartChar, nEndChar;
	m_Editbox.GetSel(nStartChar, nEndChar );
	pCmdUI->Enable((nEndChar-nStartChar) !=0);
}

void CEditdlg::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	int nStartChar, nEndChar;
	m_Editbox.GetSel(nStartChar, nEndChar );
	pCmdUI->Enable((nEndChar-nStartChar) !=0);
}

void CEditdlg::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	int nStartChar, nEndChar;
	m_Editbox.GetSel(nStartChar, nEndChar );
	pCmdUI->Enable((nEndChar-nStartChar) !=0);
}
void CEditdlg::OnUpdateFontFontset(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_Editbox.GetWindowTextLength() != 0);
}



void CEditdlg::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsClipboardFormatAvailable(CF_TEXT));
}

void CEditdlg::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_Editbox.CanUndo());
}


////////////////////////////////////////////////////////////////////////////////
//Find and Replace

void CEditdlg::OnEditFind()
{
	if(pFindReplaceDialog)
		return;
	CString m_text;
	m_Editbox.GetWindowText(m_text);
	int nStartChar, nEndChar; 
	m_Editbox.GetSel(nStartChar, nEndChar);
	if(nStartChar != nEndChar)
   		findtext = m_text.Mid(nStartChar,nEndChar - nStartChar);
	pFindReplaceDialog = new CFindReplaceDialog();
	bIsReplaceDialog = FALSE;
	pFindReplaceDialog->Create(TRUE, findtext, NULL, dwFlags, this);
	pFindReplaceDialog->SendMessage(WM_INITDIALOG); 
}

void CEditdlg::OnEditNext()
{
	if(PopFindValidFind())
	{	
		if(!PopFindNextText())	MessageBeep(0xffffffff);
	}
	else
	{
		if(pFindReplaceDialog)
			return;
		CString m_text;
		m_Editbox.GetWindowText(m_text);
		int nStartChar, nEndChar; 
		m_Editbox.GetSel(nStartChar, nEndChar);
		if(nStartChar != nEndChar)
   		findtext = m_text.Mid(nStartChar,nEndChar - nStartChar);			
		pFindReplaceDialog = new CFindReplaceDialog();
		bIsReplaceDialog = FALSE;                                     
		pFindReplaceDialog->Create(TRUE, findtext, NULL, dwFlags, this);
		pFindReplaceDialog->SendMessage(WM_INITDIALOG);
	}
}


void CEditdlg::OnEditReplace()
{
	if(pFindReplaceDialog)
		return;
	CString m_text;
	m_Editbox.GetWindowText(m_text);
	int nStartChar, nEndChar; 
	m_Editbox.GetSel(nStartChar, nEndChar);
	if(nStartChar != nEndChar)
   		findtext = m_text.Mid(nStartChar,nEndChar - nStartChar);	
	pFindReplaceDialog = new CFindReplaceDialog();
	bIsReplaceDialog = TRUE;                      
	pFindReplaceDialog->Create(FALSE, findtext, repltext, dwFlags, this);
	pFindReplaceDialog->SendMessage(WM_INITDIALOG);  	
}

void CEditdlg::OnUpdateEditNext(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((m_Editbox.GetWindowTextLength() != 0) && (pFindReplaceDialog));	
}

LONG CEditdlg::FindReplaceHelper(UINT wParam, LONG lParam)
{
	CFindReplaceDialog* pDialogBox =
				CFindReplaceDialog::GetNotifier(lParam); 

	LPFINDREPLACE lpfr = (LPFINDREPLACE)lParam; 
//	FINDREPLACE	
	dwFlags = FR_HIDEWHOLEWORD;
	if(pDialogBox->SearchDown())
		dwFlags = dwFlags | FR_DOWN;
    if(pDialogBox->MatchCase())
		dwFlags = dwFlags | FR_MATCHCASE;
	if (pDialogBox -> MatchWholeWord()) 
		dwFlags = dwFlags | FR_WHOLEWORD;
	if(pDialogBox->FindNext() ||
	   pDialogBox->ReplaceCurrent() ||
	   pDialogBox->ReplaceAll())
    {
		
		findtext = pDialogBox->GetFindString();//find what?
		 
		if(bIsReplaceDialog)
			repltext = pDialogBox->GetReplaceString();//replace what?

		if(pDialogBox->FindNext())	
           	if(!PopFindFindText())	
				MessageBeep(0xffffffff);
		if(bIsReplaceDialog) 
		{
			if((pDialogBox->ReplaceCurrent()) || (pDialogBox->ReplaceAll()))
				if(!PopFindReplaceText()) MessageBeep(0xffffffff);
	    
			if(pDialogBox->ReplaceAll())  while(PopFindReplaceText());    	    	
		}        
		m_Editbox.SetModify();
	}
	else if(pDialogBox->IsTerminating())
	{
		delete pFindReplaceDialog;                      
		pFindReplaceDialog = NULL;
	}                                                  
	return 0;
} 
BOOL CEditdlg::PopFindFindText ()
{
	int         iPos, pos ;
	HLOCAL      hLocal;
    CString     m_text, m_findtext;
	CString     ftext;
	int nStartChar, nEndChar;
	int ioffset = HIWORD(m_Editbox.GetSel());
	                                                            
	hLocal = m_Editbox.GetHandle();
  	LocalLock(hLocal);
   
 	int textlen = m_Editbox.GetWindowTextLength();
    m_Editbox.GetWindowText(m_text);
    m_findtext = findtext;
    if(!pFindReplaceDialog->MatchCase()) 
	{
		m_text.MakeLower( );
		m_findtext.MakeLower( );
    }
	if(pFindReplaceDialog->SearchDown())    
	{
   		ftext = m_text.Right(textlen - ioffset);
		pos = ftext.Find(m_findtext);
		iPos = pos + ioffset ;
	}
	else 
	{
		
		m_Editbox.GetSel(nStartChar, nEndChar );
		if(nStartChar != nEndChar ) 
		{
			ioffset = ioffset - (nEndChar- nStartChar);
		}	
		ftext = m_text.Left(ioffset);
		CString szText=ftext;
		int i = -1;
		while (((i = szText.ReverseFind(m_findtext.GetAt(0))) != -1) && (szText.GetLength()>0)) 
		{ 
			CString szText1 = szText.Mid(i, m_findtext.GetLength());
			if (szText1 ==m_findtext) break;
			szText = szText.Left(i);
		}
		pos = i; 
    	iPos = pos;
	}
    LocalUnlock (hLocal) ;
    
	if(pos == -1)
      return FALSE ;
   	ioffset = iPos + strlen (m_findtext) ;
    m_Editbox.SetSel(iPos, ioffset);
    return TRUE ;
}

BOOL CEditdlg::PopFindNextText ()
{
    return PopFindFindText ();
}

BOOL CEditdlg::PopFindReplaceText ()
{
     if (!PopFindFindText ())
        return FALSE ;
     m_Editbox.ReplaceSel(repltext) ;
     return TRUE ;
}

BOOL CEditdlg::PopFindValidFind (void)
{
	return findtext != '\0';
}


////////////////////////////////////////////////////////////////////////
//File I/O

BOOL CEditdlg::FileOpenDlg ()
{
	CString strFilter1,strFilter2,strFilter3,strOpenFileTitle;
	strFilter1.LoadString(IDS_FILTER1);
	strFilter2.LoadString(IDS_FILTER2);
	strOpenFileTitle.LoadString(IDS_OPENFILE_TITLE);
	CFileDialog dlgFile(TRUE);
	dlgFile.m_ofn.Flags |= OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
	CString strFilter = strFilter1;
	strFilter += m_ParaExt + ")";
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("I*.") + m_ParaExt;
	strFilter += (TCHAR)'\0';   // last string
	dlgFile.m_ofn.nMaxCustFilter++;

	strFilter += _T(strFilter2);
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("I*.*");
	strFilter += (TCHAR)'\0';   // last string
	dlgFile.m_ofn.nMaxCustFilter++;

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.lpstrTitle = strOpenFileTitle;
	dlgFile.m_ofn.lpstrFile = szFileName.GetBuffer(512);
	BOOL bResult = dlgFile.DoModal() == IDOK ? TRUE : FALSE;
	szFileName.ReleaseBuffer();
	return bResult;
}

BOOL CEditdlg::FileSaveDlg ()
{
	CString strFilter1,strFilter2,strFilter3,strOpenFileTitle;
	strFilter1.LoadString(IDS_FILTER1);
	strFilter2.LoadString(IDS_FILTER2);
	strOpenFileTitle.LoadString(IDS_OPENFILE_TITLE);

	CFileDialog dlgFile(FALSE);
	dlgFile.m_ofn.Flags |= OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT ;
	dlgFile.m_ofn.lpstrDefExt = m_ParaExt;
	CString strFilter = _T(strFilter1) + m_ParaExt + _T(")");
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("I*.") + m_ParaExt;
	strFilter += (TCHAR)'\0';   // last string
	dlgFile.m_ofn.nMaxCustFilter++;

	strFilter += _T(strFilter2);
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("I*.*");
	strFilter += (TCHAR)'\0';   // last string
	dlgFile.m_ofn.nMaxCustFilter++;

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.lpstrTitle =strOpenFileTitle;
	dlgFile.m_ofn.lpstrFile = szFileName.GetBuffer(512);
	BOOL bResult = dlgFile.DoModal() == IDOK ? TRUE : FALSE;
	szFileName.ReleaseBuffer();
	return bResult;
}

long CEditdlg::FileLength (int hFile)
{
     long lCurrentPos = _llseek (hFile, 0L, 1) ;
     long lFileLength = _llseek (hFile, 0L, 2) ;
     
     _llseek (hFile, lCurrentPos, 0) ;

     return lFileLength ;
}

BOOL CEditdlg::FileRead (CString lpstrFileName)
{
    CFileException e;
	CFile f;
	char err[128];
	if (!f.Open(lpstrFileName, CFile::modeRead | CFile::shareExclusive, &e))
	{
		e.GetErrorMessage(err, 128);
		AfxMessageBox(IDS_WRITE_ERROR,MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	UINT nBytesRead;
	CString szBuffer;
	TRY
	{
		nBytesRead = f.Read( szBuffer.GetBuffer(f.GetLength()), f.GetLength());
		f.Close();
		szBuffer.ReleaseBuffer();
	}
	CATCH(CFileException,e1)
	{
		f.Abort();
		e1->GetErrorMessage(err, 128);
		AfxMessageBox(IDS_WRITE_ERROR,MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	END_CATCH
	szBuffer.ReleaseBuffer();
    m_Editbox.SetWindowText(szBuffer) ;
    return TRUE ;
}

BOOL CEditdlg::FileWrite (CString lpstrFileName)
{
    CFileException e;
	CFile f;
	char err[128];
	if (!f.Open( lpstrFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive, &e ))
	{
		e.GetErrorMessage(err, 128);
		AfxMessageBox(IDS_WRITE_ERROR,MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	CString szBuffer;
	m_Editbox.GetWindowText(szBuffer);
	TRY
	{
		f.Write(szBuffer, szBuffer.GetLength());
		f.Close();
	}
	CATCH(CFileException,e1)
	{
		f.Abort();
		e1->GetErrorMessage(err, 128);
		AfxMessageBox(IDS_WRITE_ERROR,MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	END_CATCH
    return TRUE ; 
}



/////////////////////////////////////////////////////////////////////////////
//File I/O message
void CEditdlg::OnFileNew()
{
	if (m_Editbox.GetModify()) 
	{
		int ret;
		CString msg;
		if (szFileName == "") szFileName = "UNTITLED";
		AfxFormatString1(msg,IDS_SAVE_CURRENT_FILE,szFileName) ;
		ret = AfxMessageBox (msg, MB_YESNOCANCEL | MB_ICONQUESTION);
		//char  szBuffer [128] ;
		//wsprintf (szBuffer, "保存当前文件于"+ (szFileName !="" ? szFileName : "UNTITLED")) ;
        //ret = MessageBox (szBuffer, "文件不保存", MB_YESNOCANCEL | MB_ICONQUESTION);
        if (ret == IDCANCEL) return;
        if (ret == IDYES) 
		{
			if ((szFileName =="") && (!FileSaveDlg())) return;
    		if (!FileWrite (szFileName)) return;
        }
    }

    m_Editbox.SetWindowText ("\0") ;
    szFileName  = "";
    m_Editbox.SetModify(FALSE);
	CString szMsg;
	szMsg.LoadString(IDS_EDIT_FILE);
    SetWindowText(szMsg);
}

void CEditdlg::OnFileOpen()
{
	if (m_Editbox.GetModify()) 
	{
		int ret;
		CString msg;
		if (szFileName == "") szFileName = "无标题";
		AfxFormatString1(msg,IDS_SAVE_CURRENT_FILE,szFileName) ;
		ret = AfxMessageBox (msg, MB_YESNOCANCEL | MB_ICONQUESTION);
        if (ret == IDCANCEL) return;
        if (ret == IDYES) 
		{
			if ((szFileName =="") && (!FileSaveDlg())) return;
    		if (!FileWrite (szFileName)) return;
        }
    }
    if (FileOpenDlg()) 
	{   
		if (!FileRead (szFileName)) return;
		else 
		{
			CString szMsg;
	        szMsg.LoadString(IDS_EDIT_FILE);
		    SetWindowText(szMsg+szFileName);
        	m_Editbox.SetModify(FALSE);
        }	
    }
}

void CEditdlg::OnFileSaveDefault()
{
    CString m_DefParaName;
//	GetUserTemplateDir(m_DefParaName);
	m_DefParaName = m_DefParaName + AfxGetApp()->m_pszExeName+szPathSep + m_MethodName + "." + m_ParaExt;
	FileWrite(m_DefParaName);
}

void CEditdlg::OnFileSave()
{
	if ((szFileName =="") && (!FileSaveDlg())) return;
    if (FileWrite (szFileName)) 
	{
		nExitCode = 1;
		m_Editbox.SetModify(FALSE);
	}
    else
	{
   	    CString msg;
		AfxFormatString1(msg,IDS_CANNT_WRITE_FILE,szFileName);
		AfxMessageBox (msg, MB_OK|MB_ICONSTOP) ;
	}
}

void CEditdlg::OnUpdateFileSave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_Editbox.GetModify());
}

void CEditdlg::OnFileSaveAs()
{
	if (!FileSaveDlg ()) return;
    if (FileWrite (szFileName)) 
	{
    	CString szMsg;
	    szMsg.LoadString(IDS_EDIT_FILE);
		SetWindowText(szMsg+szFileName);
    	m_Editbox.SetModify(FALSE); 
    }	
    else
    {
   	    CString msg;
		AfxFormatString1(msg,IDS_CANNT_WRITE_FILE,szFileName);
		AfxMessageBox (msg, MB_OK|MB_ICONSTOP) ;
	}
	
}

///////////////////////////////////////////////////////////////////
//Text Font

void CEditdlg::OnFontFontset()
{
	CHOOSEFONT cf ;

     cf.lStructSize      = sizeof (CHOOSEFONT) ;
     cf.hDC              = NULL ;
     cf.lpLogFont        = &logfont ;
     cf.iPointSize       = 0 ;
     cf.Flags            = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS
                                                  | CF_EFFECTS ;
     cf.rgbColors        = 0L ;
     cf.lCustData        = 0L ;
     cf.lpfnHook         = NULL ;
     cf.lpTemplateName   = NULL ;
     cf.hInstance        = NULL ;
     cf.lpszStyle        = NULL ;
     cf.nFontType        = 0 ;               // Returned from ChooseFont
     cf.nSizeMin         = 0 ;
     cf.nSizeMax         = 0 ;
	
	CFontDialog FontDialog(cf.lpLogFont, cf.Flags, NULL, this);
	if(FontDialog.DoModal() == IDOK) 
	{
		HFONT hFont = ::CreateFontIndirect (&logfont) ;
		m_Editbox.SendMessage(WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELONG(1, 0));
	}
	return;
}



void CEditdlg::OnEditReadonly()
{
	bReadOnly = !bReadOnly;
	m_Editbox.SetReadOnly(bReadOnly);
}

void CEditdlg::OnUpdateEditReadonly(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(bReadOnly);		
}

void CEditdlg::OnClose()
{
	OnEditExit();
}

void CEditdlg::OnUpdateEditFind(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_Editbox.GetWindowTextLength() != 0);	
}

void CEditdlg::OnUpdateEditReplace(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_Editbox.GetWindowTextLength() != 0);	
}


int CEditdlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

void CEditdlg::OnEditAbout() 
{
	CAboutEdit aboutDlg;
	aboutDlg.DoModal();
}

void CEditdlg::OnEditorHelpIndex() 
{
	CString szHelpName;
//	if (!GetLogTopHelpDir(szHelpName)) return;
	szHelpName = szHelpName + "Editor.hlp";
	::WinHelp(this->GetSafeHwnd(),szHelpName,HELP_FINDER, 0);
}
/////////////////////////////////////////////////////////////////////////////
// CAboutEdit dialog


CAboutEdit::CAboutEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAboutEdit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAboutEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutEdit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutEdit, CDialog)
	//{{AFX_MSG_MAP(CAboutEdit)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutEdit message handlers

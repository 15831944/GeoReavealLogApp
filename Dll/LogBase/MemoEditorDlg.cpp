/****************************************************************/
/*																*/
/*  MemoEditorDlg.cpp											*/
/*																*/
/*  Implementation of the CMMemoEditorDlg class.				*/
/*	This file is part of the DBF Explorer application.			*/
/*																*/
/*  Last updated: 25 Feb. 2004									*/
/*																*/
/****************************************************************/


#include "stdafx.h"
#include "MemoEditorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CMemoEditorDlg::CMemoEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialogResize(CMemoEditorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMemoEditorDlg)
	m_strText = _T("");
	//}}AFX_DATA_INIT
}


void CMemoEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogResize::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMemoEditorDlg)
	DDX_Text(pDX, IDC_TEXT, m_strText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMemoEditorDlg, CDialogResize)
	//{{AFX_MSG_MAP(CMemoEditorDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_DLGRESIZE_MAP(CMemoEditorDlg)
	DLGRESIZE_CONTROL(IDC_TEXT, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	DLGRESIZE_CONTROL(IDOK,  DLSZ_MOVE_X )
	DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X )
END_DLGRESIZE_MAP()

BOOL CMemoEditorDlg::OnInitDialog() 
{
	CDialogResize::OnInitDialog();	
	InitResizing();	
	CenterWindow();
	return TRUE;
}

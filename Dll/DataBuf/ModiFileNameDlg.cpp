// ModiFileNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModiFileNameDlg.h"
#include "CodeSP.h"
#include "_Comext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModiFileNameDlg dialog


CModiFileNameDlg::CModiFileNameDlg(char* Editer,CWnd* pParent /*=NULL*/)
	: CDialog(CModiFileNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModiFileNameDlg)
	m_ManualFileName = _T("");
	//}}AFX_DATA_INIT
	m_ManualFileName=(char*)Editer;
}


void CModiFileNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModiFileNameDlg)
	DDX_Control(pDX, IDC_MANUALFILENAME_EDIT, m_Edit);
	DDX_Control(pDX, IDC_STATUS_EDIT, m_ctlStatus);
	DDX_Text(pDX, IDC_MANUALFILENAME_EDIT, m_ManualFileName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModiFileNameDlg, CDialog)
	//{{AFX_MSG_MAP(CModiFileNameDlg)
	ON_EN_CHANGE(IDC_MANUALFILENAME_EDIT, OnChangeManualfilenameEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModiFileNameDlg message handlers

void CModiFileNameDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	CheckExist();
	if(bExist)
	{
		CString s;
		s.Format ("%s\n\n已存在，改写吗?        ",m_ManualFileName);
		int result=AfxMessageBox(s,MB_YESNO|MB_ICONQUESTION);
		if(result != IDYES) return;  //如果没选择Yes则返回
	//	if(IsFileName(m_ManualFileName))
	//		CFile::Remove(m_ManualFileName);
	}
	CDialog::OnOK();
}

BOOL CModiFileNameDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Edit.LineScroll(1,m_ManualFileName.GetLength ());
	// TODO: Add extra initialization here
	bExist=false;
	CheckExist();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CModiFileNameDlg::CheckExist()
{
	UpdateData();
	if(sp_bWriteExist)
		return;
	bExist=IsFilePathName(m_ManualFileName);
	if(bExist)
		m_ctlStatus.SetWindowText("文件已经存在");
	else
		m_ctlStatus.SetWindowText("文件不存在");
}

void CModiFileNameDlg::OnChangeManualfilenameEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CheckExist();
}

// BusyMessage.cpp : implementation file
//

#include "stdafx.h"
#include "BusyMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBusyMessage dialog


CBusyMessage::CBusyMessage(CWnd* pParent /*=NULL*/)
	: CDialog(CBusyMessage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBusyMessage)
	m_WaitMessage = _T("");
	//}}AFX_DATA_INIT
}

CBusyMessage::~CBusyMessage()
{
	if (m_hWnd != NULL) 
		DestroyWindow();
}

void CBusyMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBusyMessage)
	DDX_Control(pDX, IDC_BUSY, m_Busy);
	DDX_Text(pDX, IDC_WAITMSG, m_WaitMessage);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBusyMessage, CDialog)
	//{{AFX_MSG_MAP(CBusyMessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBusyMessage message handlers

BOOL CBusyMessage::OnInitDialog() 
{	
	CDialog::OnInitDialog();
	CenterWindow ();
	m_Busy.SetNumSteps(30);
	m_Busy.SetSpeed(20);
	m_Busy.Start ();
	
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CBusyMessage::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_Busy.End();
	KillTimer(0);
	return CDialog::DestroyWindow();
}

void CBusyMessage::SetWaitMessage(CString MsgStr)
{
	m_WaitMessage=MsgStr;
	UpdateData(FALSE);
}

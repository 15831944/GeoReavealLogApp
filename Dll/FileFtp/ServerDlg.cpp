// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServerDlg dialog

#define IDC_SERVER		1002
#define IDC_USERNAME	1003
#define IDC_PASSWORD	1004
#define IDC_ANONLOGIN	1005

CServerDlg::CServerDlg(LPCTSTR szServer, LPCTSTR szUsername, LPCTSTR szPassword, AL_TYPE nAnonymousLogin)
	: m_sServer(szServer), 
	  m_sUsername(szUsername), 
	  m_sPassword(szPassword), 
	  m_nAnonLogin(nAnonymousLogin)
{
	BOOL bShowAnonLogin = (m_nAnonLogin != ANONLOGIN_HIDE);
	int LOGINOFFSET = 0;

    AddRCControl("LTEXT", "", "服务器IP", 0, 0,7,10,34,10,IDC_STATIC);
    AddRCControl("COMBOBOX", "", "",CBS_DROPDOWN | WS_VSCROLL | WS_TABSTOP, 0,49,7,134,100, IDC_SERVER);

	if (bShowAnonLogin)
	{
		AddRCControl("CONTROL", "Button", "匿名方式", BS_AUTOCHECKBOX | WS_TABSTOP, 0, 49,29,134,10, IDC_ANONLOGIN);
		LOGINOFFSET = 20;
	}

	AddRCControl("LTEXT", "", "用户名",0, 0, 7,29 + LOGINOFFSET,35,10,IDC_STATIC);
	AddRCControl("COMBOBOX", "", "",CBS_DROPDOWN | WS_VSCROLL | WS_TABSTOP, 0,49,27 + LOGINOFFSET,134,100, IDC_USERNAME);
	AddRCControl("LTEXT", "", "口令",0, 0, 7,48 + LOGINOFFSET,34,10,IDC_STATIC);
	AddRCControl("EDITTEXT", "", "",ES_PASSWORD | ES_AUTOHSCROLL | WS_TABSTOP, 0,49,45 + LOGINOFFSET,134,14, IDC_PASSWORD);
	AddRCControl("CONTROL","Static", "",SS_ETCHEDHORZ, 0,7,65 + LOGINOFFSET,176,1,IDC_STATIC);
	AddRCControl("DEFPUSHBUTTON", "", "确认", WS_TABSTOP, 0, 77,74 + LOGINOFFSET,50,14,IDOK);
	AddRCControl("PUSHBUTTON", "", "取消", WS_TABSTOP, 0,133,74 + LOGINOFFSET,50,14,IDCANCEL);

	if (m_sServer.IsEmpty())
		m_sServer = AfxGetApp()->GetProfileString("RemoteSettings", "LastServer");

	if (m_sUsername.IsEmpty())
		m_sUsername = AfxGetApp()->GetProfileString("RemoteSettings", "LastUsername");

	if (m_nAnonLogin == ANONLOGIN_AUTO)
		m_nAnonLogin = AfxGetApp()->GetProfileInt("RemoteSettings", "LastAnonLogin", ANONLOGIN_NO) ? ANONLOGIN_YES : ANONLOGIN_NO;
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CRuntimeDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServerDlg)
	DDX_CBString(pDX, IDC_SERVER, m_sServer);
	DDX_CBString(pDX, IDC_USERNAME, m_sUsername);
	DDX_Text(pDX, IDC_PASSWORD, m_sPassword);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SERVER, m_cbServers);
	DDX_Control(pDX, IDC_USERNAME, m_cbUsernames);

	if (m_nAnonLogin >= ANONLOGIN_NO)
		DDX_Check(pDX, IDC_ANONLOGIN, (int&)m_nAnonLogin);

	if (pDX->m_bSaveAndValidate)
	{
		m_sServer.TrimLeft();
		m_sServer.TrimRight();
		m_sUsername.TrimLeft();
		m_sUsername.TrimRight();
		m_sPassword.TrimLeft();
		m_sPassword.TrimRight();
	}
}

BEGIN_MESSAGE_MAP(CServerDlg, CRuntimeDlg)
	//{{AFX_MSG_MAP(CServerDlg)
	ON_CBN_EDITCHANGE(IDC_SERVER, OnChangeServer)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ANONLOGIN, OnAnonLogin)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerDlg message handlers

void CServerDlg::OnChangeServer() 
{
	UpdateData();
	
	GetDlgItem(IDOK)->EnableWindow(!m_sServer.IsEmpty());
}

BOOL CServerDlg::OnInitDialog() 
{
	CRuntimeDlg::OnInitDialog();
	SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,WS_EX_TOOLWINDOW);
	// popuplate comboboxes from registry
	int nServer = AfxGetApp()->GetProfileInt("RemoteSettings", "ServerCount", 0);

	while (nServer--)
	{
		CString sServer, sItem;

		sItem.Format("Server%d", nServer);
		sServer = AfxGetApp()->GetProfileString("RemoteSettings", sItem);

		if (!sServer.IsEmpty() && m_cbServers.FindString(-1, sServer) == CB_ERR)
			m_cbServers.InsertString(0, sServer);
	}

	// add m_sServer as appropriate and select
	if (!m_sServer.IsEmpty() && m_cbServers.FindString(-1, m_sServer) == CB_ERR)
		m_cbServers.InsertString(0, m_sServer);

	m_cbServers.SelectString(-1, m_sServer);
	
	int nName = AfxGetApp()->GetProfileInt("RemoteSettings", "UsernameCount", 0);

	while (nName--)
	{
		CString sName, sItem;

		sItem.Format("Username%d", nName);
		sName = AfxGetApp()->GetProfileString("RemoteSettings", sItem);

		if (!sName.IsEmpty() && m_cbUsernames.FindString(-1, sName) == CB_ERR)
			m_cbUsernames.InsertString(0, sName);
	}

	// add m_sUsername as appropriate and select
	if (!m_sUsername.IsEmpty() && m_cbUsernames.FindString(-1, m_sUsername) == CB_ERR)
		m_cbUsernames.InsertString(0, m_sUsername);

	m_cbUsernames.SelectString(-1, m_sUsername);

	OnChangeServer();
	OnAnonLogin();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CServerDlg::OnOK() 
{
	CRuntimeDlg::OnOK();

	// save server list to registry and last selected item
	int nServer = m_cbServers.GetCount();

	AfxGetApp()->WriteProfileInt("RemoteSettings", "ServerCount", nServer);

	while (nServer--)
	{
		CString sServer, sItem;

		sItem.Format("Server%d", nServer);
		m_cbServers.GetLBText(nServer, sServer);

		AfxGetApp()->WriteProfileString("RemoteSettings", sItem, sServer);
	}

	AfxGetApp()->WriteProfileString("RemoteSettings", "LastServer", m_sServer);
	
	// save username list to registry and last selected item
	int nName = m_cbUsernames.GetCount();

	AfxGetApp()->WriteProfileInt("RemoteSettings", "UsernameCount", nName);

	while (nName--)
	{
		CString sName, sItem;

		sItem.Format("Username%d", nName);
		m_cbUsernames.GetLBText(nName, sName);

		if (!sName.IsEmpty())
			AfxGetApp()->WriteProfileString("RemoteSettings", sItem, sName);
	}

	AfxGetApp()->WriteProfileString("RemoteSettings", "LastUsername", m_sUsername);
	AfxGetApp()->WriteProfileInt("RemoteSettings", "LastAnonLogin", m_nAnonLogin);
}

void CServerDlg::OnAnonLogin()
{
	UpdateData();

	if (m_nAnonLogin != ANONLOGIN_HIDE)
	{
		GetDlgItem(IDC_USERNAME)->EnableWindow(m_nAnonLogin == ANONLOGIN_NO);
		GetDlgItem(IDC_PASSWORD)->EnableWindow(m_nAnonLogin == ANONLOGIN_NO);
	}
}


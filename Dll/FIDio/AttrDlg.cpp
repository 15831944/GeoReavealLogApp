// AttrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AttrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttrDlg dialog


CAttrDlg::CAttrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAttrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAttrDlg)
	m_Sdep = 0.0f;
	m_Edep = 0.0f;
	m_CurveMin = 0.0f;
	m_CurveMax = 0.0f;
	m_LineLog = -1;
	//}}AFX_DATA_INIT
}


void CAttrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAttrDlg)
	DDX_Control(pDX, IDC_LINEWIDTH_COMBO, m_LineWidth);
	DDX_Control(pDX, IDC_LINECOLOR_COMBO, m_LineColor);
	DDX_Text(pDX, IDC_SDEP_EDIT, m_Sdep);
	DDX_Text(pDX, IDC_EDEP_EDIT, m_Edep);
	DDX_Text(pDX, IDC_LEFT_EDIT, m_CurveMin);
	DDX_Text(pDX, IDC_RIGHT_EDIT, m_CurveMax);
	DDX_Radio(pDX, IDC_LINE_RADIO, m_LineLog);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAttrDlg, CDialog)
	//{{AFX_MSG_MAP(CAttrDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttrDlg message handlers

BOOL CAttrDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CWndShadow::Initialize(AfxGetInstanceHandle());
	m_Shadow.Create(GetSafeHwnd());

	// TODO: Add extra initialization here
	//线宽
	m_LineWidth.ResetContent();
	m_LineWidth.AddString("1");
	m_LineWidth.AddString("2");
	m_LineWidth.AddString("3");
	m_LineWidth.SetCurSel(LineWidth-1);
	//线的颜色
	m_LineColor.ResetContent();
	m_LineColor.AddString("1 红色");
	m_LineColor.AddString("2 绿色");
	m_LineColor.AddString("3 兰色");
	m_LineColor.SetCurSel(LineColor-1);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAttrDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true); //将当前窜修改的内容更新变量
	CString cs,cs1;
	m_LineWidth.GetLBText(m_LineWidth.GetCurSel(),cs);
	LineWidth=atoi((const char*)cs);
	m_LineColor.GetLBText(m_LineColor.GetCurSel(),cs);
	LineColor=atoi((const char*)cs);
	//对数据进行判断
	if(m_LineLog)
	{
		if(m_CurveMin<=0.0f || m_CurveMax<=0.0f)
		{
			AfxMessageBox("对数据刻度的横向比必须大于0!",MB_ICONSTOP);
			return;	
		}
	}
	CDialog::OnOK();
}

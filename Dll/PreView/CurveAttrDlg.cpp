// AttrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PreView.h"
#include "CurveAttrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCurveAttrDlg dialog


CCurveAttrDlg::CCurveAttrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCurveAttrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCurveAttrDlg)
	m_Sdep = 0.0f;
	m_Edep = 0.0f;
	m_CurveMin = 0.0f;
	m_CurveMax = 0.0f;
	m_LineLog = -1;
	//}}AFX_DATA_INIT
}


void CCurveAttrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCurveAttrDlg)
	DDX_Control(pDX, IDC_LINEWIDTH_COMBO, m_LineWidth);
	DDX_Control(pDX, IDC_LINECOLOR_COMBO, m_LineColor);
	DDX_Text(pDX, IDC_SDEP_EDIT, m_Sdep);
	DDX_Text(pDX, IDC_EDEP_EDIT, m_Edep);
	DDX_Text(pDX, IDC_LEFT_EDIT, m_CurveMin);
	DDX_Text(pDX, IDC_RIGHT_EDIT, m_CurveMax);
	DDX_Radio(pDX, IDC_LINE_RADIO, m_LineLog);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCurveAttrDlg, CDialog)
	//{{AFX_MSG_MAP(CCurveAttrDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCurveAttrDlg message handlers

BOOL CCurveAttrDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	//�߿�
	m_LineWidth.ResetContent();
	m_LineWidth.AddString("1");
	m_LineWidth.AddString("2");
	m_LineWidth.AddString("3");
	m_LineWidth.SetCurSel(LineWidth-1);
	//�ߵ���ɫ
	m_LineColor.ResetContent();
	m_LineColor.AddString("1 ��ɫ");
	m_LineColor.AddString("2 ��ɫ");
	m_LineColor.AddString("3 ��ɫ");
	m_LineColor.SetCurSel(LineColor-1);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCurveAttrDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true); //����ǰ���޸ĵ����ݸ��±���
	CString cs,cs1;
	m_LineWidth.GetLBText(m_LineWidth.GetCurSel(),cs);
	LineWidth=atoi((const char*)cs);
	m_LineColor.GetLBText(m_LineColor.GetCurSel(),cs);
	LineColor=atoi((const char*)cs);
	//�����ݽ����ж�
	if(m_LineLog)
	{
		if(m_CurveMin<=0.0f || m_CurveMax<=0.0f)
		{
			AfxMessageBox("�����ݿ̶ȵĺ���ȱ������0!",MB_ICONSTOP);
			return;	
		}
	}
	CDialog::OnOK();
}

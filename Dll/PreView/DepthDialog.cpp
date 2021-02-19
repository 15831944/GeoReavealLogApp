#include "stdafx.h"
#include "DepthDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CPlotDepthDialog dialog

CPlotDepthDialog::CPlotDepthDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPlotDepthDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlotDepthDialog)
	m_PlotSdep = 2000.;
	m_PlotEdep = 2500.;
	m_DepthScale=200.;
	//}}AFX_DATA_INIT
}

void CPlotDepthDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlotDepthDialog)
	DDX_Text(pDX, IDC_PLOT_STDEP, m_PlotSdep);
	DDX_Text(pDX, IDC_PLOT_ENDEP, m_PlotEdep);
	DDX_Text(pDX, IDC_DEP_SCALE,m_DepthScale);
	//}}AFX_DATA_MAP
}

void CPlotDepthDialog::SetDepth(double Sdep, double Edep)
{
	m_PlotSdep = ((int)(Sdep*1000+0.5)) / 1000.0;
	m_PlotEdep = ((int)(Edep*1000+0.5)) / 1000.0;	
}

void CPlotDepthDialog::GetDepth(double& Sdep, double& Edep)
{
	Sdep = m_PlotSdep;
	Edep = m_PlotEdep;	
}

BEGIN_MESSAGE_MAP(CPlotDepthDialog, CDialog)
	//{{AFX_MSG_MAP(CPlotDepthDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CPlotDepthDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,WS_EX_TOOLWINDOW);
	// TODO: Add extra initialization here
	CString str;
	str.Format ("%g",m_PlotSdep);
	SetDlgItemText (IDC_PLOT_STDEP,str);
	str.Format ("%g",m_PlotEdep);
	SetDlgItemText (IDC_PLOT_ENDEP,str);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CPlotDepthDialog message handlers

void CPlotDepthDialog::OnOK() 
{
	UpdateData(TRUE);
	if(m_PlotSdep>m_PlotEdep)
	{
		AfxMessageBox("起始深度应大于结束深度  ",MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	if(m_DepthScale<1)
	{
		AfxMessageBox("绘图比例应大于1  ",MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	EndDialog(IDOK);
}
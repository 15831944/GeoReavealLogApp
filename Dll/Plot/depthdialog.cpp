#include "stdafx.h"
#include "depthdialog.h"
#include "helpid.h"

/////////////////////////////////////////////////////////////////////////////
// CPlotDepthDialog dialog

CPlotDepthDialog::CPlotDepthDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPlotDepthDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlotDepthDialog)
	m_PlotSdep = 2000;
	m_PlotEdep = 2500;
	//}}AFX_DATA_INIT
}

void CPlotDepthDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlotDepthDialog)
	DDX_Text(pDX, IDC_PLOT_STDEP, m_PlotSdep);
	DDX_Text(pDX, IDC_PLOT_ENDEP, m_PlotEdep);
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
	if (m_PlotSdep > m_PlotEdep)
	{
		AfxMessageBox(IDS_DEPTH_ERROR,MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	EndDialog(IDOK);
}

/////////////////////////////////////////////////////////////////////////////
// CDepthScaleDialog dialog

CDepthScaleDialog::CDepthScaleDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDepthScaleDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDepthScaleDialog)
	m_DepthScale = 0.0;
	//}}AFX_DATA_INIT
}


void CDepthScaleDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDepthScaleDialog)
	DDX_Text(pDX, IDC_DEPTH_SCALE, m_DepthScale);
	DDV_MinMaxDouble(pDX, m_DepthScale, 1., 40000.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDepthScaleDialog, CDialog)
	//{{AFX_MSG_MAP(CDepthScaleDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDepthScaleDialog message handlers


void CDepthScaleDialog::OnOK() 
{
	UpdateData(TRUE);
	if (m_DepthScale < 1)
	{
		AfxMessageBox(IDS_DEPTHSCALE_BIGFIVE,MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	EndDialog(IDOK);
}

BOOL CDepthScaleDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,WS_EX_TOOLWINDOW);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

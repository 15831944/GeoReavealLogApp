//  Guage.cpp : implementation file
// CG: This file was added by the Guage Dialog component

#include "stdafx.h"
#include "Guage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGuage dialog

CGuage::CGuage(float sdep, float edep)
{
	m_Sdep = sdep;
	m_Edep = edep;
    m_bCancel = FALSE;
    m_nLower = 0;
    m_nUpper = 100;
    m_nStep = m_NextPos = 2;
	m_CurPos = 0;
    //{{AFX_DATA_INIT(CGuage)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    m_bParentDisabled = FALSE;
	m_szGuage="当前进度";
}

CGuage::~CGuage()
{
    if(m_hWnd!=NULL)
      DestroyWindow();
}

BOOL CGuage::DestroyWindow()
{
    ReEnableParent();
    return CDialog::DestroyWindow();
}

void CGuage::ReEnableParent()
{
    if(m_bParentDisabled && (m_pParentWnd!=NULL))
		m_pParentWnd->EnableWindow(TRUE);
    m_bParentDisabled=FALSE;
}

BOOL CGuage::Create(CWnd *pParent)
{
    // Get the true parent of the dialog
    m_pParentWnd = CWnd::GetSafeOwner(pParent);
    // m_bParentDisabled is used to re-enable the parent window
    // when the dialog is destroyed. So we don't want to set
    // it to TRUE unless the parent was already enabled.
 /*
	if((m_pParentWnd!=NULL)&& m_pParentWnd->IsWindowEnabled())
    {
		m_pParentWnd->EnableWindow(FALSE);
		m_bParentDisabled = TRUE;
    }
	*/
    if(!CDialog::Create(CGuage::IDD,pParent))
    {
		  ReEnableParent();
		  return FALSE;
    }
	CenterWindow();
    return TRUE;
}

void CGuage::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CGuage)
	DDX_Control(pDX, FID_IDC_PROGDLG_PERCENT, m_PercentCtrl);
	DDX_Control(pDX, FID_IDC_CURDEP, m_CurDepCtrl);
    DDX_Control(pDX, FID_IDC_PROGDLG_PROGRESS, m_Progress);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGuage, CDialog)
    //{{AFX_MSG_MAP(CGuage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CGuage::OnCancel()
{
    m_bCancel=TRUE;
}

void CGuage::SetDepthRange(float sdep,float edep)
{
    m_Sdep = sdep;
    m_Edep = edep;
    m_Progress.SetRange32(m_nLower,m_nUpper);
	CString szStr;
	szStr.Format("%.1f-%.1f",m_Sdep,m_Edep);
	GetDlgItem (IDC_DEPRANG)->SetWindowText(szStr);	
}
  
void CGuage::SetDepth(float dep)
{
	int pos = m_nLower + (int)((dep-m_Sdep) / (m_Edep-m_Sdep) * (m_nUpper-m_nLower) + 0.5);
	if (pos != m_CurPos) SetPos(pos);
}

void CGuage::SetRange(int nLower,int nUpper)
{
	m_nLower = nLower;
	m_nUpper = nUpper;
    m_Progress.SetRange32(m_nLower,m_nUpper);
}

int CGuage::SetPos(int nPos)
{
	PumpMessages();
    int iResult = m_Progress.SetPos(nPos);
    UpdatePercent(nPos);
	m_CurPos = nPos;
    return iResult;
}

int CGuage::SetStep(int nStep)
{
    m_nStep = nStep; // Store for later use in calculating percentage
    return m_Progress.SetStep(nStep);
}

int CGuage::OffsetPos(int nPos)
{
    PumpMessages();
    int iResult = m_Progress.OffsetPos(nPos);
    UpdatePercent(iResult+nPos);
    return iResult;
}

int CGuage::StepIt()
{
    PumpMessages();
    int iResult = m_Progress.StepIt();
    UpdatePercent(iResult+m_nStep);
	m_CurPos = iResult+m_nStep;
    return iResult;
}

void CGuage::PumpMessages()
{
    ASSERT(m_hWnd!=NULL);
    MSG msg;
    // Handle dialog messages
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if(!IsDialogMessage(&msg))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);  
      }
    }
}

BOOL CGuage::CheckCancelButton()
{
    // Process all pending messages
    PumpMessages();

    BOOL bResult = m_bCancel;
    m_bCancel = FALSE;
	if(bResult)
	{
		if(::MessageBox(this->m_hWnd,"确认中断该处理进程?            ","中断提示",MB_ICONQUESTION | MB_YESNO) == IDYES)
			return TRUE;
	}
    return FALSE;
}

void CGuage::UpdatePercent(int nNewPos)
{  
    int nDivisor = m_nUpper - m_nLower;
    ASSERT(nDivisor>0);  // m_nLower should be smaller than m_nUpper

    int nDividend = (nNewPos - m_nLower);
    ASSERT(nDividend>=0);   // Current position should be greater than m_nLower

    double percent = nDividend * 100.0 / nDivisor;

	double dep = m_Sdep + percent * (m_Edep - m_Sdep) / 100;
    char buffer[80];
	sprintf(buffer,"%.1f",dep);
	m_CurDepCtrl.SetWindowText(buffer);

    // Since the Progress Control wraps, we will wrap the percentage
    // along with it. However, don't reset 100% back to 0%
    int nPercent = (int)percent;
	if(nPercent!=100)
      nPercent %= 100;

    // Display the percentage
    CString strBuf;
    strBuf.Format(_T("%d%c"),nPercent,_T('%'));

	CString strCur; // get current percentage
    m_PercentCtrl.GetWindowText(strCur);
	m_NextPos = nNewPos + m_nStep;

    if (strCur != strBuf)
		m_PercentCtrl.SetWindowText(strBuf);
}    

/////////////////////////////////////////////////////////////////////////////
// CGuage message handlers

BOOL CGuage::OnInitDialog() 
{
    CDialog::OnInitDialog();
    m_Progress.SetRange32(m_nLower,m_nUpper);
    m_Progress.SetStep(m_nStep);
    m_Progress.SetPos(m_nLower);
	UpdatePercent(0);
	GetDlgItem (FID_IDC_STATIC)->SetWindowText(m_szGuage);
	CString szStr;
	szStr.Format("%.1f-%.1f",m_Sdep,m_Edep);
	GetDlgItem (IDC_DEPRANG)->SetWindowText(szStr);	
    return TRUE;  
}

int CGuage::SetTitle(const char *Title)
{
	this->GetDlgItem (FID_IDC_TTITLE)->SetWindowText (Title);
	return 1;
}

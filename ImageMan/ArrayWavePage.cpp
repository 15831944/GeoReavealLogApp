// WavePage.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "style.h"
#include "styledialog.h"
#include "ArrayWavePage.h"
#include "ArrayWaveDraw.h"
#include "Database.h"
#include "FIDIO.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArrayWavePage property page

IMPLEMENT_DYNCREATE(CArrayWavePage, CPropertyPage)

CArrayWavePage::CArrayWavePage(CArrayWaveObj* pWaveObj) : CPropertyPage(CArrayWavePage::IDD)
{
	//{{AFX_DATA_INIT(CArrayWavePage)
	m_WfAmp = 0;
	m_Level = 0;
	m_PlotAmp = 0;
	m_WaveType = -1;
	m_LeftVal = 0;
	m_RightVal = 0;
	m_StartTime = 0.0f;
	m_WaveWidth = 1;
	m_crNum = 0;
	//}}AFX_DATA_INIT
	ASSERT(pWaveObj != NULL);
	pObj = pWaveObj;
}

CArrayWavePage::~CArrayWavePage()  
{
}

void CArrayWavePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArrayWavePage)
	DDX_Text(pDX, IDC_WFAMP, m_WfAmp);
	DDX_Text(pDX, IDC_LEVEL, m_Level);
	DDV_MinMaxUInt(pDX, m_Level, 0, 100);
	DDX_Text(pDX, IDC_PLOTAMP, m_PlotAmp);
	DDV_MinMaxUInt(pDX, m_PlotAmp, 0, 100);
	DDX_Radio(pDX, IDC_WAVE, m_WaveType);
	DDX_Text(pDX, IDC_LEFT, m_LeftVal);
	DDX_Text(pDX, IDC_RIGHT, m_RightVal);
	DDX_Text(pDX, IDC_STARTIME, m_StartTime);
	DDX_Text(pDX, IDC_WAVE_WIDTH, m_WaveWidth);
	DDV_MinMaxInt(pDX, m_WaveWidth, 0, 20);
	DDX_Text(pDX, IDC_COLOR_GRAD, m_crNum);
	DDV_MinMaxInt(pDX, m_crNum, 6, 128);
	DDX_Control(pDX, IDC_WAVE_COLOR, m_Color);
	DDX_Control(pDX, IDC_WAVE_COLOR0, m_Color0);
	DDX_Control(pDX, IDC_WAVE_COLOR1, m_Color1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CArrayWavePage, CPropertyPage)
	//{{AFX_MSG_MAP(CArrayWavePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArrayWavePage message handlers

BOOL CArrayWavePage::OnInitDialog() 
{
	m_Color.SetColor(pObj->m_WaveColor);
	m_Color0.SetColor(pObj->m_WaveColor0);
	m_Color1.SetColor(pObj->m_WaveColor1);
	m_WaveType = pObj->m_WaveType;
	m_WaveWidth=pObj->m_WaveWidth;
	m_Level = pObj->m_Level;
	m_LeftVal = pObj->m_LeftValue;
	m_RightVal = pObj->m_RightValue;
	m_WfAmp = pObj->m_WfMax;
	m_PlotAmp = pObj->m_Amp;
	m_StartTime=pObj->m_StartTime ;
	m_crNum=pObj->m_crNum;
	CPropertyPage::OnInitDialog();
	return TRUE;
}

void CArrayWavePage::OnOK() 
{
	CString m_Name;
	pObj->m_WaveColor = m_Color.GetColor();
	pObj->m_WaveColor0 = m_Color0.GetColor();
	pObj->m_WaveColor1 = m_Color1.GetColor();
	pObj->m_WaveType = m_WaveType;
	pObj->m_WaveWidth=m_WaveWidth;
	pObj->m_Level = m_Level;
	pObj->m_LeftValue = m_LeftVal;
	pObj->m_RightValue = m_RightVal;
	pObj->m_Amp = m_PlotAmp;
	pObj->m_WfMax = m_WfAmp;
	pObj->m_StartTime=m_StartTime;
	pObj->m_crNum=m_crNum;
	CPropertyPage::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
// CArrayWaveBase property page

IMPLEMENT_DYNCREATE(CArrayWaveBase, CPropertyPage)

CArrayWaveBase::CArrayWaveBase(CArrayWaveObj* pWaveObj) : CPropertyPage(CArrayWaveBase::IDD)
{
	//{{AFX_DATA_INIT(CArrayWaveBase)
	m_AliasName = _T("");
	m_Unit = _T("");
	m_WellName = _T("");
	m_Edep = 0.0f;
	m_Sdep = 0.0f;
	m_DelayCurve = _T("");
	m_strDelayCurveNo = _T("1");
	m_PlusCurve = _T("");
	m_strPlusCurveNo = _T("1");
	//}}AFX_DATA_INIT
	ASSERT(pWaveObj != NULL);
	pObj = pWaveObj;
}

CArrayWaveBase::~CArrayWaveBase()
{
}

void CArrayWaveBase::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArrayWaveBase)
	DDX_Control(pDX, IDC_SDEP, m_SdepCtrl);
	DDX_Control(pDX, IDC_EDEP, m_EdepCtrl);
	DDX_Control(pDX, IDC_SLIDER_SDEP, m_SliderSdep);
	DDX_Control(pDX, IDC_SLIDER_EDEP, m_SliderEdep);
	DDX_Text(pDX, IDC_ALIAS_NAME, m_AliasName);
	DDX_Text(pDX, IDC_UNIT, m_Unit);
	DDX_Text(pDX, IDC_WELLNAME, m_WellName);
	DDX_Text(pDX, IDC_EDEP, m_Edep);
	DDX_Text(pDX, IDC_SDEP, m_Sdep);
	DDX_CBString(pDX, IDC_NAME, m_Name);
	DDX_CBString(pDX, IDC_DELAY_CURVE, m_DelayCurve);
	DDX_CBString(pDX, IDC_DELAY_CURVENO, m_strDelayCurveNo);
	DDX_CBString(pDX, IDC_PLUS_CURVE, m_PlusCurve);
	DDX_CBString(pDX, IDC_PLUS_CURVENO, m_strPlusCurveNo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CArrayWaveBase, CPropertyPage)
	//{{AFX_MSG_MAP(CArrayWaveBase)
	ON_BN_CLICKED(IDC_BROWSE_WELL, OnBrowseWell)
	ON_EN_CHANGE(IDC_SDEP, OnChangeSdep)
	ON_EN_CHANGE(IDC_EDEP, OnChangeEdep)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_WELLNAME, OnChangeWellname)
	ON_CBN_CLOSEUP(IDC_NAME, OnCloseupName)
	ON_CBN_CLOSEUP(IDC_PLUS_CURVE, OnCloseupPlusCurve)
	ON_CBN_CLOSEUP(IDC_DELAY_CURVE, OnCloseupDelayCurve)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArrayWaveBase message handlers

void CArrayWaveBase::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertyPage::OnOK();
	pObj->m_PlotSdep = m_Sdep;
	pObj->m_PlotEdep = m_Edep;
	pObj->m_UnitText = m_Unit;
	pObj->m_AliasName = m_AliasName;
	pObj->m_DelayCurve=m_DelayCurve;
	pObj->m_DelayCurveNo=atoi(m_strDelayCurveNo);
	pObj->m_PlusCurve=m_PlusCurve;
	pObj->m_PlusCurveNo=atoi(m_strPlusCurveNo);

	if (m_WellName == "Template") m_WellName = "########";
	pObj->m_strPathName = m_WellName + "." + m_Name; 
}

BOOL CArrayWaveBase::OnInitDialog() 
{
	CString path;
	CFIDIO::GetFullWellName(pObj->m_strPathName,m_WellName.GetBuffer(512));
	m_WellName.ReleaseBuffer();
	CFIDIO::GetFIDName(pObj->m_strPathName,m_Name.GetBuffer(512));
	m_Name.ReleaseBuffer();
	m_DelayCurve=pObj->m_DelayCurve;
	m_strDelayCurveNo.Format ("%d",pObj->m_DelayCurveNo );
	m_PlusCurve=pObj->m_PlusCurve;
	m_strPlusCurveNo.Format ("%d",pObj->m_PlusCurveNo );
	if (pObj->pDoc->GetDocumentType() == TEMPLATE_DOC)
	{
		m_WellName = "Template";
		CWnd* pWnd = GetDlgItem(IDC_BROWSE_WELL);
		if (pWnd != NULL) pWnd->ShowWindow(FALSE);
		pWnd = GetDlgItem(IDC_WELLNAME);
		if (pWnd != NULL) pWnd->EnableWindow(FALSE);
	}
	else AddCurveList(pObj->m_strPathName);

	m_AliasName = pObj->m_AliasName;
	m_Unit = pObj->m_UnitText;
	m_Sdep = (float)pObj->m_PlotSdep;
	m_Edep = (float)pObj->m_PlotEdep;
    
	CPropertyPage::OnInitDialog();
	if (pObj->pDoc->GetDocumentType() == TEMPLATE_DOC)	
		HideDepth();
	else if (InitDepthSlider()) 
	{
		EnableDepth(TRUE);
		SetSliderDepth(m_Sdep,m_Edep);
	}
	else EnableDepth(FALSE);
	return TRUE; 
}

void CArrayWaveBase::OnBrowseWell() 
{
	char path[512];
	CFIDIO::GetFileName(m_WellName,path,'I');
	strcat(path,".FID");

	CFileDialog dlgFile(TRUE);
	dlgFile.m_ofn.Flags |= OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	CString strFilter = "Well File(*.FID)";
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("*.FID");
	strFilter += (TCHAR)'\0';   // last string
	dlgFile.m_ofn.nMaxCustFilter++;
	strFilter += _T("All File(*.*)");
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';   // last string
	dlgFile.m_ofn.nMaxCustFilter++;

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.lpstrTitle = "Select Well File";
	dlgFile.m_ofn.lpstrFile = path;
	dlgFile.m_ofn.lpstrInitialDir=GetCurrentUserPath();
	BOOL bResult = dlgFile.DoModal() == IDOK ? TRUE : FALSE;
	if (bResult) 
	{
		CFIDIO::GetFullWellName(path,m_WellName.GetBuffer(512));
		m_WellName.ReleaseBuffer();
		CEdit* pEdit = (CEdit*) GetDlgItem(IDC_WELLNAME);
		if (pEdit != NULL) pEdit->SetWindowText(m_WellName);
		if (pObj->pDoc->GetDocumentType() == TEMPLATE_DOC)	return;
		if (InitDepthSlider()) 
		{
			SetSliderDepth(m_Sdep,m_Edep);
			if ((pObj->GetParentObj() == NULL) || (pObj->GetParentType() != 0)) 
				EnableDepth(TRUE);
		}
		else EnableDepth(FALSE);
		AddCurveList(m_WellName);
	}
}

void CArrayWaveBase::OnChangeSdep() 
{
	CWnd* pWnd = GetFocus();
	if ((pWnd != NULL) && (pWnd == &m_SdepCtrl))
	{
		UpdateData(TRUE);
		m_SliderSdep.SetPos((int)m_Sdep);
	}
}

void CArrayWaveBase::OnChangeEdep() 
{
	CWnd* pWnd = GetFocus();
	if ((pWnd != NULL) && (pWnd == &m_EdepCtrl))
	{
		UpdateData(TRUE);
		m_SliderEdep.SetPos((int)m_Edep);
	}
}

void CArrayWaveBase::HideDepth()
{
	CWnd* pWnd = GetDlgItem(IDC_SDEP);
	pWnd->ShowWindow(FALSE);
	pWnd = GetDlgItem(IDC_EDEP);
	pWnd->ShowWindow(FALSE);
	pWnd = GetDlgItem(IDC_STATIC_DEPTH);
	pWnd->ShowWindow(FALSE);
	pWnd = GetDlgItem(IDC_STATIC_DEPTO);
	pWnd->ShowWindow(FALSE);
	pWnd = GetDlgItem(IDC_SLIDER_EDEP);
	pWnd->ShowWindow(FALSE);
	pWnd = GetDlgItem(IDC_SLIDER_SDEP);
	pWnd->ShowWindow(FALSE);
	pWnd = GetDlgItem(IDC_STATIC_SDEP);
	pWnd->ShowWindow(FALSE);
	pWnd = GetDlgItem(IDC_STATIC_EDEP);
	pWnd->ShowWindow(FALSE);
}

void CArrayWaveBase::EnableDepth(BOOL bf)
{
	CWnd* pWnd = GetDlgItem(IDC_SDEP);
	pWnd->EnableWindow(bf);
	pWnd = GetDlgItem(IDC_EDEP);
	pWnd->EnableWindow(bf);
	pWnd = GetDlgItem(IDC_SLIDER_EDEP);
	pWnd->EnableWindow(bf);
	pWnd = GetDlgItem(IDC_SLIDER_SDEP);
	pWnd->EnableWindow(bf);
}

BOOL CArrayWaveBase::InitDepthSlider()
{
	BOOL bf = FALSE;
	int dep1=0,dep2=9999;
	CFIDIO* pFIDIO = new CFIDIO(m_WellName + "." + m_Name );
	if (pFIDIO->Open())
	{
		dep1 = (int) pFIDIO->GetStartDepth();
		dep2 = (int) pFIDIO->GetEndDepth();
		m_SliderSdep.SetRange(dep1,dep2);			
		m_SliderSdep.SetLineSize(1);			
		m_SliderSdep.SetPageSize((dep2-dep1) / 10);			
		m_SliderEdep.SetRange(dep1,dep2);			
		m_SliderSdep.SetLineSize(1);			
		m_SliderSdep.SetPageSize((dep2-dep1)/10);			
		bf = TRUE;
	}
	delete pFIDIO;
	char s1[10],s2[10];
	itoa(dep1,s1,10); 
	itoa(dep2,s2,10);
	GetDlgItem(IDC_STATIC_SDEP)->SetWindowText(s1);
	GetDlgItem(IDC_STATIC_EDEP)->SetWindowText(s2);
	return bf;
}

void CArrayWaveBase::SetSliderDepth(float sdep, float edep)
{
	m_SliderSdep.SetPos((int)sdep);
	m_SliderEdep.SetPos((int)edep);
}

void CArrayWaveBase::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CWnd* pWnd = GetFocus();
	if (pWnd == NULL) return;
	char s[10]; 
	if (pWnd == &m_SliderSdep)
	{
		m_Sdep = (float) m_SliderSdep.GetPos();
		sprintf(s,"%7.2f",m_Sdep);
		m_SdepCtrl.SetWindowText(s);
	}
	else if (pWnd == &m_SliderEdep)
	{
		m_Edep = (float) m_SliderEdep.GetPos();
		sprintf(s,"%7.2f",m_Edep);
		m_EdepCtrl.SetWindowText(s);
	}
}

void CArrayWaveBase::OnChangeWellname() 
{
	CString str;
	CEdit* pEdit = (CEdit*) GetDlgItem(IDC_WELLNAME);
	if ((pEdit == NULL) || (pEdit != GetFocus())) return;
	pEdit->GetWindowText(str);
	if (m_WellName !=  str)
	{
		m_WellName = str;
		if (InitDepthSlider()) 
		{
			SetSliderDepth(m_Sdep,m_Edep);
			if ((pObj->GetParentObj() == NULL) || (pObj->GetParentType() != 0)) 
				EnableDepth(TRUE);
		}
		else EnableDepth(FALSE);
	}
}

void CArrayWaveBase::OnCloseupName() 
{
	// TODO: Add your control notification handler code here
	CString str;
	CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_NAME);
	int nIndex = pBox->GetCurSel();
	if (nIndex == CB_ERR)
		pBox->GetWindowText(str);
	else
		pBox->GetLBText(nIndex, str);
	str.MakeUpper();
	if (m_Name != str)
	{
		m_Name = str;
		GetDlgItem(IDC_ALIAS_NAME)->GetWindowText(m_AliasName);
		if(m_AliasName.IsEmpty ())
			GetDlgItem(IDC_ALIAS_NAME)->SetWindowText (m_Name);
		if (InitDepthSlider()) 
		{
			SetSliderDepth(m_Sdep,m_Edep);
			if ((pObj->GetParentObj() == NULL) || (pObj->GetParentType() != 0)) 
				EnableDepth(TRUE);
		}
		else EnableDepth(FALSE);
		
		int xxx=m_Name.ReverseFind ('[');
		if(xxx>0)
		{
			CString s1,ss;
			ss=m_Name.Left (xxx);
			s1=ss.Left (2)+"GN"+ss.Mid (ss.GetLength ()-2);
			GetDlgItem(IDC_PLUS_CURVE)->SetWindowText (s1);
			s1=ss.Left (2)+"ST"+ss.Mid (ss.GetLength ()-2);
			GetDlgItem(IDC_DELAY_CURVE)->SetWindowText (s1);
			ss=m_Name.Mid (xxx+1);
			ss=ss.Left (ss.GetLength ()-1);
			GetDlgItem(IDC_PLUS_CURVENO)->SetWindowText (ss);
			GetDlgItem(IDC_DELAY_CURVENO)->SetWindowText (ss);
		}
	}
}

void CArrayWaveBase::AddCurveList(CString path)
{
	CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_NAME);
	CComboBox* pPlusBox = (CComboBox*) GetDlgItem(IDC_PLUS_CURVE);
	CComboBox* pDelayBox = (CComboBox*) GetDlgItem(IDC_DELAY_CURVE);
	if (pBox == NULL) return;
	pBox->ResetContent();
	if (pPlusBox == NULL) return; 
	pPlusBox->ResetContent();
	if (pDelayBox == NULL) return;
	pDelayBox->ResetContent();
	CString szDir;
	CFIDIO::GetPath(m_WellName,szDir.GetBuffer(512));
	szDir.ReleaseBuffer();

	CDbaseFile m_dBaseFile;
	if(m_dBaseFile.Open(szDir+"Curve.dbf")!=DBASE_SUCCESS)
		return;
	char szBuff[255];
	CString Name,Type;
	for(int rc=m_dBaseFile.GetFirstRecord();rc==DBASE_SUCCESS;rc=m_dBaseFile.GetNextRecord())
  	{
		if (m_dBaseFile.IsRecordDeleted())
			continue;
		m_dBaseFile.GetField(1, szBuff);
		Name=szBuff;
		Name.TrimLeft();Name.TrimRight();
		m_dBaseFile.GetField("曲线类型", szBuff);
		Type=szBuff;
		Type.TrimLeft();Type.TrimRight();
		if(Type=="阵列")
			pBox->AddString(Name);
		pPlusBox->AddString(Name);
		pDelayBox->AddString(Name);
	}
	m_dBaseFile.Close();

	pBox->SetWindowText(m_Name);
	pPlusBox->SetWindowText(m_PlusCurve);
	pDelayBox->SetWindowText(m_DelayCurve);

	CString str;
	CFIDIO *pFIDIO=NULL;
	pFIDIO= new CFIDIO(m_WellName + "." + m_PlusCurve );
	if(pFIDIO->Open())
	{
		CComboBox* pPlusBoxNo = (CComboBox*) GetDlgItem(IDC_PLUS_CURVENO);
		if(pPlusBox!=NULL)
		{
			pPlusBoxNo->ResetContent();
			pPlusBoxNo->AddString("1");
			if(pFIDIO->GetDimenSion()>2)
			{
				FIDCONTENT *FIDCont;
				FIDCont=pFIDIO->GetContent(2);
				for(int i=2;i<FIDCont->nps+1;i++)
				{
					str.Format ("%d",i);
					pPlusBoxNo->AddString(str); 
				}
			}
		}
		pPlusBoxNo->SetWindowText (m_strPlusCurveNo);
	}
	delete pFIDIO;	
	pFIDIO = new CFIDIO(m_WellName + "." + m_DelayCurve );
	if(pFIDIO->Open())
	{
		CComboBox* pDelayBoxNo = (CComboBox*) GetDlgItem(IDC_DELAY_CURVENO);
		if(pDelayBox!=NULL)
		{
			pDelayBoxNo->ResetContent();
			pDelayBoxNo->AddString("1");
			if(pFIDIO->GetDimenSion()>2)
			{
				FIDCONTENT *FIDCont;
				FIDCont=pFIDIO->GetContent(2);
				for(int i=2;i<FIDCont->nps+1;i++)
				{
					str.Format ("%d",i);
					pDelayBoxNo->AddString(str); 
				}
			}
			pDelayBoxNo->SetWindowText (m_strDelayCurveNo);
		}
	}
	delete pFIDIO;	
}

void CArrayWaveBase::OnCloseupPlusCurve() 
{
	// TODO: Add your control notification handler code here
	CString str;
	CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_PLUS_CURVE);
	int nIndex = pBox->GetCurSel();
	if (nIndex == CB_ERR)
		pBox->GetWindowText(str);
	else
		pBox->GetLBText(nIndex, str);
	str.MakeUpper();
	if (m_PlusCurve != str)
	{
		m_PlusCurve = str;
		CFIDIO* pFIDIO = new CFIDIO(m_WellName + "." + m_PlusCurve );
		if(pFIDIO->Open())
		{
			CComboBox* pPlusBox = (CComboBox*) GetDlgItem(IDC_PLUS_CURVENO);
			if(pPlusBox!=NULL)
			{
				pPlusBox->ResetContent();
				pPlusBox->AddString("1");
				if(pFIDIO->GetDimenSion() >2)
				{
					FIDCONTENT *FIDCont;
					FIDCont=pFIDIO->GetContent(2);
					for(int i=2;i<FIDCont->nps+1;i++)
					{
						str.Format ("%d",i);
						pPlusBox->AddString(str); 
					}
				}
			}
			pPlusBox->SetWindowText ("1");
		}
		delete pFIDIO;	
	}
}

void CArrayWaveBase::OnCloseupDelayCurve() 
{
	// TODO: Add your control notification handler code here
	CString str;
	CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_DELAY_CURVE);
	int nIndex = pBox->GetCurSel();
	if (nIndex == CB_ERR)
		pBox->GetWindowText(str);
	else
		pBox->GetLBText(nIndex, str);
	str.MakeUpper();
	if (m_DelayCurve != str)
	{
		m_DelayCurve = str;
		CFIDIO* pFIDIO = new CFIDIO(m_WellName + "." + m_DelayCurve );
		if(pFIDIO->Open())
		{
			CComboBox* pDelayBox = (CComboBox*) GetDlgItem(IDC_DELAY_CURVENO);
			if(pDelayBox!=NULL)
			{
				pDelayBox->ResetContent();
				pDelayBox->AddString("1");
				if(pFIDIO->GetDimenSion() >2)
				{
					FIDCONTENT *FIDCont;
					FIDCont=pFIDIO->GetContent(2);
					for(int i=2;i<FIDCont->nps+1;i++)
					{
						str.Format ("%d",i);
						pDelayBox->AddString(str); 
					}
				}
				pDelayBox->SetWindowText ("1");
			}
		}
		delete pFIDIO;	
	}
}

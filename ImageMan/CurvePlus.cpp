// CurveDep.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "TrackDraw.h"
#include "CurvePlus.h"
#include "FIDIO.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCurvePlus property page

IMPLEMENT_DYNCREATE(CCurvePlus, CDialog)

CCurvePlus::CCurvePlus(CCurveObj* pCurveObj) : CDialog(CCurvePlus::IDD)
{
	//{{AFX_DATA_INIT(CCurvePlus)
	m_Edep = 0.0f;
	m_Sdep = 0.0f;
	m_Rlev = 0.1f;
	m_WellName = _T("");
	m_Name = _T("");
	m_Edep0 = 0.0f;
	m_Sdep0 = 0.0f;
	m_IniDep=FALSE;

	m_Point1Dep=0.f;
	m_Point1Value=0.f;
	m_Point2Dep=0.f;
	m_Point2Value=0.f;
	m_MultiplyCoef=1.f;
	m_PlusCoef=0.f;
	m_SubPlusCoef=0.f;
	m_BSubPLus=FALSE;
	//}}AFX_DATA_INIT
	ASSERT(pCurveObj != NULL);
	pObj = pCurveObj;
	m_CurveStyle=0;
}

CCurvePlus::~CCurvePlus()
{
}

void CCurvePlus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MULTI_FACTOR, m_MultiplyCoef);
	DDX_Text(pDX, IDC_PLUS_FACTOR, m_PlusCoef);

	DDX_Text(pDX, IDC_POINT1DEP, m_Point1Dep);
	DDX_Text(pDX, IDC_POINT1VALUE, m_Point1Value);
	DDX_Text(pDX, IDC_POINT2DEP, m_Point2Dep);
	DDX_Text(pDX, IDC_POINT2VALUE, m_Point2Value);
	DDX_Radio(pDX, IDC_RADIO1, m_CurveStyle);
	//{{AFX_DATA_MAP(CCurvePlus)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCurvePlus, CDialog)
	//{{AFX_MSG_MAP(CCurvePlus)
	ON_BN_CLICKED(IDC_DATAPLUS_OK, OnOK)
	ON_BN_CLICKED(IDC_DATAPLUS_CANCEL, OnCANCEL)
	ON_BN_CLICKED(IDC_COMPUTECOEF, OnComputeCoef)
	ON_BN_CLICKED(IDC_PREVERGE, OnPreVerge)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCurveDep message handlers

BOOL CCurvePlus::OnInitDialog() 
{
	CFIDIO::GetFullWellName(pObj->m_strPathName, m_WellName.GetBuffer(512));
	m_WellName.ReleaseBuffer();
	
	CFIDIO::GetFIDName(pObj->m_strPathName, m_Name.GetBuffer(512));
	m_Name.ReleaseBuffer();
	CString str=m_Name+" = A * "+m_Name+" + B";
	GetDlgItem(IDC_NAME)->SetWindowText(str);

	CFIDIO* pFIDIO = new CFIDIO(m_WellName + "." + m_Name );
	if (pFIDIO->Open())
	{
		m_Sdep = pFIDIO->GetStartDepth();
		m_Edep = pFIDIO->GetEndDepth();
		m_Rlev=pFIDIO->GetDepthLevel();
	}	
	char s[255];
	if(m_IniDep)
	{
		m_Sdep0=((int)(m_Sdep0/m_Rlev)+1)*m_Rlev;
		m_Edep0=((int)(m_Edep0/m_Rlev)+1)*m_Rlev;
		if(m_Sdep0>m_Sdep)	m_Sdep=m_Sdep0;
		if(m_Edep0<m_Edep)	m_Edep=m_Edep0;
	}
	pFIDIO->SeekToDepth (m_Sdep);
	pFIDIO->Read(2,1,&m_Value0);
	delete pFIDIO;
	sprintf(s, "%10.4f 到 %10.4f", m_Sdep, m_Edep);
	GetDlgItem(IDC_STATIC_DEP)->SetWindowText(s);
	CDialog::OnInitDialog();
	return TRUE; 
}

void CCurvePlus::OnOK() 
{
	UpdateData ();
	char s[255];CString msg;
	float fbuf;
	strcpy(s,m_WellName);
	strcat(s,".");
	strcat(s,m_Name); 
	CFIDIO* pFIDtemp = NULL;
	pFIDtemp = new CFIDIO(s);
	if(!pFIDtemp->Open())
	{
		delete pFIDtemp;
		msg="open ";msg+=s;msg+=" file error";
		AfxMessageBox(msg);
		return;
	}
	int dim=pFIDtemp->GetDimenSion();
	FIDCONTENT* Content = pFIDtemp->GetContent(dim);
	if((Content != NULL)&&(dim==2)&&(Content->type == REPR_FLOAT))
	{
		pFIDtemp->Copy(m_WellName+".curvePlustemp");
  	    delete pFIDtemp;
		CFIDIO* pFIDW = NULL;
		pFIDW = new CFIDIO(s);
	    pFIDW->Open(); 
		CFIDIO* pFIDR = NULL;
		strcpy(s,m_WellName);
		strcat(s,".curvePlustemp"); 
		pFIDR = new CFIDIO(s);
	    if(!pFIDR->Open()) 
		{delete pFIDR;AfxMessageBox("open curvePlustemp file error");return;}
		unsigned long AllRecord=(unsigned long)((m_Edep-m_Sdep)/m_Rlev);
		unsigned long i=0;
		BeginWaitCursor();
		pFIDR->SeekToDepth (m_Sdep);
		pFIDW->SeekToDepth (m_Sdep);
		while(i<AllRecord)
		{
			pFIDR->Read(2,1,&fbuf);
			if((fbuf!=-9999.f)||(fbuf==-999.25f))
			{
				fbuf=fbuf*m_MultiplyCoef+m_PlusCoef;
				if(m_CurveStyle!=0)
				{
					if(fbuf>360.f) fbuf-=360.f;
					if(fbuf<0.f) fbuf+=360.f;
				}
				pFIDW->Write(2,1,&fbuf);
			}
			i++;pFIDR->Next();pFIDW->Next();
		}
		EndWaitCursor();
		pFIDR->Delete();delete pFIDR;delete pFIDW;
	}
	else
	{
		delete pFIDtemp;
	}
	CCurvePlus::EndDialog(IDOK);
}

void CCurvePlus::OnCANCEL()
{
	CCurvePlus::EndDialog(IDCANCEL);
}

void CCurvePlus::OnComputeCoef()
{
	UpdateData ();
	m_Point1Dep=((int)(m_Point1Dep/m_Rlev))*m_Rlev;
	m_Point2Dep=((int)(m_Point2Dep/m_Rlev))*m_Rlev;
	if((m_Point1Dep<m_Sdep)||(m_Point1Dep>m_Edep))
	{
		AfxMessageBox("第一刻度点深度错误   ");
		return;
	}
	if((m_Point2Dep<m_Sdep)||(m_Point2Dep>m_Edep))
	{
		AfxMessageBox("第二刻度点深度错误   ");
		return;
	}

	char s[255];CString msg;
	float fbuf1,fbuf2;
	strcpy(s,m_WellName);
	strcat(s,".");
	strcat(s,m_Name); 
	CFIDIO* pFIDtemp = NULL;
	pFIDtemp = new CFIDIO(s);
	if(!pFIDtemp->Open())
	{
		delete pFIDtemp;
		msg="open ";msg+=s;msg+=" file error";
		AfxMessageBox(msg);
		return;
	}
	int dim=pFIDtemp->GetDimenSion();
	FIDCONTENT* Content = pFIDtemp->GetContent(dim);
	if((Content != NULL)&&(dim==2)&&(Content->type == REPR_FLOAT))
	{
		pFIDtemp->SeekToDepth (m_Point1Dep);
		pFIDtemp->Read(2,1,&fbuf1);
		pFIDtemp->SeekToDepth (m_Point2Dep);
		pFIDtemp->Read(2,1,&fbuf2);
		float Daltaf=fbuf2-fbuf1;
		if(fabs(Daltaf)==0)
		{
			m_MultiplyCoef=1.f;
			m_PlusCoef=0.f;
		}
		else
		{
			m_MultiplyCoef=(m_Point2Value-m_Point1Value)/Daltaf;
			m_PlusCoef=m_Point1Value-m_MultiplyCoef*fbuf1;
		}
	}
	delete pFIDtemp;
	m_SubPlusCoef=m_Value0*m_MultiplyCoef+m_PlusCoef-m_Value0;
	if(m_BSubPLus)
		m_PlusCoef+=m_SubPlusCoef;
	else
		m_PlusCoef-=m_SubPlusCoef;
	UpdateData (FALSE);
}

void CCurvePlus::OnPreVerge()
{
	UpdateData ();
	m_BSubPLus=!m_BSubPLus;
	if(m_BSubPLus)
		m_PlusCoef+=m_SubPlusCoef;
	else
		m_PlusCoef-=m_SubPlusCoef;
	UpdateData (FALSE);
}
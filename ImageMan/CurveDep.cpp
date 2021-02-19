// CurveDep.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "TrackDraw.h"
#include "CurveDep.h"
#include "FIDIO.h"
#include <math.h> 
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCurveDep property page

IMPLEMENT_DYNCREATE(CCurveDep, CDialog)

CCurveDep::CCurveDep(CFIDDrawObj* pCurveObj) : CDialog(CCurveDep::IDD)
{
	//{{AFX_DATA_INIT(CCurveDep)
	m_Sdep = 0.0f;
	m_Edep = 0.0f;
	m_Name = _T("");
	m_WellName = _T("");
	m_Extend = 0.0f;
	m_ExSdep = 0.0f;
	m_ExEdep = 0.0f;
	m_DepPoint = 0.0f;
	m_NicetyDepPoint = 0.0f;
	m_BPreFixDep = TRUE;
	//}}AFX_DATA_INIT
	ASSERT(pCurveObj != NULL);
	pObj = pCurveObj;
//	m_psp.pszTitle = "Data";
//	m_psp.dwFlags |= PSP_USETITLE;
}

CCurveDep::~CCurveDep()
{
}

void CCurveDep::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCurveDep)
	DDX_Text(pDX, IDC_EXTENDDEP, m_Extend);
	DDX_Text(pDX, IDC_EXSDEP, m_ExSdep);
	DDX_Text(pDX, IDC_EXEDEP, m_ExEdep);
	DDX_Text(pDX, IDC_DEPPOINT, m_DepPoint);
	DDX_Text(pDX, IDC_NICETYDEPPOINT, m_NicetyDepPoint);
	DDX_Check(pDX, IDC_PREFIX, m_BPreFixDep);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCurveDep, CDialog)
	//{{AFX_MSG_MAP(CCurveDep)
	ON_BN_CLICKED(IDC_REVISEDEP_OK, OnOK)
	ON_BN_CLICKED(IDC_REVISEDEP_CANCEL, OnCANCEL)
	ON_BN_CLICKED(IDC_COMPUTECOEF, OnComputecoef)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCurveDep message handlers

BOOL CCurveDep::OnInitDialog() 
{
	CFIDIO::GetFullWellName(pObj->m_strPathName, m_WellName.GetBuffer(512));
	m_WellName.ReleaseBuffer();
	CFIDIO::GetFIDName(pObj->m_strPathName, m_Name.GetBuffer(512));
	m_Name.ReleaseBuffer();
	GetDlgItem(IDC_NAME)->SetWindowText(m_Name);

	CFIDIO* pFIDIO = new CFIDIO(m_WellName + "." + m_Name );
	if (pFIDIO->Open())
	{
		m_Sdep = pFIDIO->GetStartDepth();
		m_Edep = pFIDIO->GetEndDepth();
		m_Rlev=pFIDIO->GetDepthLevel();
	}
	delete pFIDIO;
	char s[255];
	sprintf(s, "%g    到  %g", m_Sdep, m_Edep);
	GetDlgItem(IDC_STATIC_DEP)->SetWindowText(s);
	m_ExSdep=((int)(m_ExSdep/m_Rlev)+1)*m_Rlev;
	m_ExEdep=((int)(m_ExEdep/m_Rlev)+1)*m_Rlev;
	GetDlgItem(IDC_STATIC_DEP)->SetWindowText(s);
	CDialog::OnInitDialog();
	return TRUE; 
}

void CCurveDep::OnOK() 
{
	CString msg;char s[255];
	short* ibuf = NULL;
	float* fbuf = NULL;
	char* cbuf = NULL;
	UpdateData();
	m_ExSdep=((int)(m_ExSdep/m_Rlev))*m_Rlev;
	m_ExEdep=((int)(m_ExEdep/m_Rlev))*m_Rlev;
	m_Extend=((int)(m_Extend/m_Rlev))*m_Rlev;
	if(fabs(m_Extend)>m_Rlev)
	{
		strcpy(s,m_WellName);
		strcat(s,".");
		strcat(s,m_Name); 
		CFIDIO* pFIDtemp = NULL;
		pFIDtemp = new CFIDIO(s);	
	    if(!pFIDtemp->Open()) 
		{	
			char ss[255];	
			strcpy(ss,"打开 ");	
			strcat(ss,s);
			strcat(ss," 错误!");
			AfxMessageBox(ss);
		}
		int dim=pFIDtemp->GetDimenSion();
		pFIDtemp->Copy(m_WellName+".curveflextemp");
		FIDCONTENT* Content = pFIDtemp->GetContent(dim);
		int nps = Content->nps;
		if (Content != NULL)
		{
			if (Content->type == REPR_FLOAT) 
				fbuf = new float[nps];
			else if (Content->type == REPR_CHAR) 
				cbuf = new char[nps];
			else 
				ibuf = new short[nps];
		}
		float dep=(m_Edep+m_Extend);
		if(m_BPreFixDep)
			pFIDtemp->SetDepth(m_Sdep,dep);
		delete pFIDtemp;

		float ExDalta=m_ExEdep-m_ExSdep;
		ExDalta=1-m_Extend/ExDalta;
		CFIDIO* pFIDW = NULL;
		pFIDW = new CFIDIO(s);
	    pFIDW->Open();
		pFIDW->SeekToDepth(m_ExSdep);
		
		CFIDIO* pFIDR = NULL;
		strcpy(s,m_WellName);
		strcat(s,".curveflextemp"); 
		pFIDR = new CFIDIO(s);
	    if(!pFIDR->Open()) 
			AfxMessageBox("open curveflextemp file error");		
		BeginWaitCursor();
		float x,depx=m_ExSdep;
		int i=0;
		int PointNum=(int)((fabs(m_ExEdep-m_ExSdep+m_Extend))/m_Rlev);
//      Contorl the Number writed to file BY academic number point
//		while(depx<m_ExEdep)
		while(i<PointNum)
		{
			x=ExDalta*((float)i);
			depx=m_ExSdep+((int)(x))*m_Rlev;
			pFIDR->SeekToDepth (depx);
			if(Content->type == REPR_FLOAT)
			{
				pFIDR->Read(dim,nps,fbuf);
				pFIDW->Write(dim,nps,fbuf);
			}
			else if(Content->type == REPR_CHAR)
			{
				pFIDR->Read(dim,nps,cbuf);
				pFIDW->Write(dim,nps,cbuf);
			}
			else 
			{
				pFIDR->Read(dim,nps,ibuf);
				pFIDW->Write(dim,nps,ibuf);					
			}			
			pFIDW->Next();
			i++;
		}
		if(m_BPreFixDep)
		{
			pFIDR->SeekToDepth(m_ExEdep);
			while(m_ExEdep<m_Edep)
			{
				if (Content->type == REPR_FLOAT)
				{
					pFIDR->Read(dim,nps,fbuf);
					pFIDW->Write(dim,nps,fbuf);
				}
				else if(Content->type == REPR_CHAR)
				{
					pFIDR->Read(dim,nps,cbuf);
					pFIDW->Write(dim,nps,cbuf);
				}
				else 
				{
					pFIDR->Read(dim,nps,ibuf);
					pFIDW->Write(dim,nps,ibuf);
				}
				pFIDR->Next();pFIDW->Next();m_ExEdep+=m_Rlev;
			}
		}
		EndWaitCursor();
		pFIDR->Delete();delete pFIDR;delete pFIDW;
		if (cbuf != NULL)	delete cbuf;
		if (ibuf != NULL)	delete ibuf;
		if (fbuf != NULL)	delete fbuf;
	}
	CCurveDep::EndDialog(IDOK);
}

void CCurveDep::OnCANCEL()
{
	CCurveDep::EndDialog(IDCANCEL);
}


void CCurveDep::OnComputecoef() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if((m_DepPoint<m_ExSdep)||(m_DepPoint>m_ExEdep))
	{
		AfxMessageBox("刻度点深度应在处理井段内    ");
		return;
	}
	m_Extend=(m_NicetyDepPoint-m_DepPoint)/(m_DepPoint-m_ExSdep)*(m_ExEdep-m_ExSdep);
	UpdateData(FALSE);
}

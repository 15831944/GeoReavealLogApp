// DataAnisotropy.cpp: implementation of the CDataAnisotropy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "flowchart.h"
#include "ImageAnisotropy.h"
#include "FlowNode.h"
#include "FlowChartDoc.h"
#include "ImageCreate.h"
#include "ImageGeoCorrect.h"
#include "AnisotropyDlg.h"
#include "FitCircle.h"
#include "FitEllipse.h"
#include "FIDio.h"
#include "Guage.h"
#include "LogProcess.h"
#include <math.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CImageAnisotropy,CFlowNode, 0)
CImageAnisotropy::CImageAnisotropy()
{
	m_pDoc=NULL;
}

CImageAnisotropy::~CImageAnisotropy()
{
}

CImageAnisotropy::CImageAnisotropy(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
	m_AzMax=50.f;			//��λ������  
	m_RxyMin=5.f;			//����������С���ֵ
	m_fWin=0.254f;          //5Ӣ��
	m_fRlev=0.0254f;		//��������
	m_NumMin=15.f;          //������
	m_iImage=1;				//ͼ��ѡ��1-��̬  2-��̬  3-�̶�
}

void CImageAnisotropy::Serialize(CArchive& ar)
{
	CFlowNode::Serialize(ar);	
	if (ar.IsStoring())
	{
		ar<<m_AzMax;
		ar<<m_RxyMin;
		ar<<m_fWin;
		ar<<m_fRlev;
		ar<<m_NumMin;
		ar<<m_iImage;
	}
	else
	{
		ar>>m_AzMax;
		ar>>m_RxyMin;
		ar>>m_fWin;
		ar>>m_fRlev;
		ar>>m_NumMin;
		ar>>m_iImage;
	}
}

void CImageAnisotropy::InitPara(CFlowChartDoc* pDoc)
{
	POSITION pos = pDoc->m_ParentList.GetHeadPosition();
	while (pos != NULL)
	{
		CFlowNode *pObj = (CFlowNode*)pDoc->m_ParentList.GetNext(pos);
		if (pObj != NULL) 
		{		
			if(pObj->IsKindOf(RUNTIME_CLASS(CImageCreat)))
			{
				m_szBufPath=((CImageCreat*)pObj)->m_szBufPath;
				m_szWellName=((CImageCreat*)pObj)->m_szWellName;
				m_szSeries=((CImageCreat*)pObj)->m_szSeries;
				m_PadNum=((CImageCreat*)pObj)->m_PadNum; 
				m_ButtonNum=((CImageCreat*)pObj)->m_ButtonNum; 
				m_ButtonMid=((CImageCreat*)pObj)->m_ButtonMid;
				m_DataNum=((CImageCreat*)pObj)->m_DataNum;
				m_CalNum=((CImageCreat*)pObj)->m_CalNum;
				m_StProDep=((CImageCreat*)pObj)->m_StProDep;
				m_EnProDep=((CImageCreat*)pObj)->m_EnProDep;
				m_Rlev=((CImageCreat*)pObj)->m_Rlev;
				m_WinLongth=((CImageCreat*)pObj)->m_WinLongth;          
				m_Bits=((CImageCreat*)pObj)->m_Bits; 
			}
		}
	}
	m_pDoc=pDoc;
}

BOOL CImageAnisotropy::Run()
{
	m_bResult=TRUE;
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	if(!m_pDoc->m_bAunAll)
	{
		CAnisotropyDlg dlg;
		dlg.m_RxyMin=m_RxyMin;
		dlg.m_AzMax=m_AzMax;
		dlg.m_fWin=m_fWin;
		dlg.m_fRlev=m_fRlev;
		dlg.m_NumMin=m_NumMin;                 //������;
		dlg.m_iImage=m_iImage;
		if(dlg.DoModal()==IDOK)
		{
			m_RxyMin=dlg.m_RxyMin;
			m_AzMax=dlg.m_AzMax;
			m_fWin=dlg.m_fWin;
			m_fRlev=dlg.m_fRlev;
			m_NumMin=dlg.m_NumMin;
			m_iImage=dlg.m_iImage;
			m_bResult=ImgAniso();
			if(m_bResult)
				OnWellRefresh();
		}
	}
	return m_bResult;
}

BOOL CImageAnisotropy::ImgAniso()
{
	int iWin=(int)(m_fWin/m_fRlev);
	int iRlev=(int)(m_fRlev/m_Rlev);
	if(iWin<1||iRlev<1) return FALSE;
	
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	BOOL bUserBreak=FALSE;
	
	//�������ָʾ��
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());	
	pGuage->ShowWindow(SW_SHOW);
	pGuage->SetTitle(" ���Ը������Է���...");

	char s[256],fn[256];
	strcpy(s,m_szWellName);//m_szBufPath);
	if(m_iImage==0)
		strcat(s,".STRMI");//");//DATAST
	else if(m_iImage==1)
		strcat(s,".DYNMI");//");//DATADY
	else
		strcat(s,".STRMI");//");//DATARS
	//��ͬ��ͷֱ����ͼ��ϳɵ����ݸ�����һ��
	//��ȡͼ���ļ�ÿ��ȵ��¼���ݸ���
	int Num = m_DataNum;//������������ÿ��ȵ��¼�������������ָ�
	CFIDIO *pFIDIO = new CFIDIO(s);
	if(!pFIDIO->Open()) 
		{delete pFIDIO;return FALSE;}		 
	FIDCONTENT* Content = pFIDIO->GetContent(3);
	m_DataNum = Content->nps;
	delete pFIDIO;

	CFIDIO::GetFileName(s,fn,'D');
	CFile Infp;
	if(Infp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		int nFitNum;
		int Block=m_DataNum*4;
		float *Value=NULL;
		Value=new float[m_DataNum];
		CFitEllipse FitEllips(m_DataNum);//�����Բ

		int i,k,kk=0,mm=0,nn;
		double cr,fratio,RDAZ,RDAZ0;
		float dep;
		CLogProcess *pLog= new CLogProcess(m_szWellName,NULL);
		pLog->StartDep=m_StProDep;
		pLog->EndDep=m_EnProDep;
		pLog->DepLevel=iRlev*m_Rlev;
		//��ʼ�����
		OUTCURVE *pOutCurve =new OUTCURVE[5];
		strcpy(pOutCurve[0].name,"RMIN");   //�͵�����
		strcpy(pOutCurve[1].name,"RMAX");	//�ߵ�����
		strcpy(pOutCurve[2].name,"RDXY");	//�������Դ�Сָʾ
		strcpy(pOutCurve[3].name,"RDAZ");	//�������Է�λ
		strcpy(pOutCurve[4].name,"RDAZ1");	//�ԳƸ������Է�λ
		float *OutXX=new float[5];
		pLog->InitOut(5,pOutCurve,(char *)OutXX);
		pLog->InitBuffer();

		float *pAz=NULL;
		pAz=new float[iWin+10];
		k=0;
		dep=m_StProDep;
		RDAZ0=0.f;
		while(k<RecordNum)
		{				
			Infp.Seek (k*Block,CFile::begin);
			Infp.Read (Value,Block);

			fratio=0.01f;
			//�����Բ�ķ�����ƽ���絼�� Radius
			for(i=0;i<m_DataNum;i++)
			{
				if(Value[i]>fratio) 
					fratio=Value[i];
			}
			fratio=3000.f/fratio;
			
			nFitNum=0;
			//����������iWin����Ȳ���
			for(i=0;i<m_DataNum;i++)
			{
				if(Value[i]>0.f)
				{
					FitEllips.m_points[nFitNum].x=(int)(sin(6.2831853*i/m_DataNum)*fratio*Value[i]);
					FitEllips.m_points[nFitNum].y=(int)(cos(6.2831853*i/m_DataNum)*fratio*Value[i]);
					nFitNum++;
				}
			}
			pAz[kk]=-999.25f;
			if(nFitNum>6)
			{
				FitEllips.frame_length=nFitNum;
				if(FitEllips.DoEllipseFit())
				{
					cr=(fabs(FitEllips.m_Rx-FitEllips.m_Ry))/FitEllips.m_Rx*90.f;
					if(cr>m_RxyMin)
					{
						RDAZ=FitEllips.m_theta_radians*180./PI;
						while(RDAZ>180.)
							RDAZ-=180.;
						pAz[kk]=(float)RDAZ;
						OutXX[0]=(float)FitEllips.m_Ry;
						OutXX[1]=(float)FitEllips.m_Rx;
						OutXX[2]=(float)cr;
						OutXX[3]=(float)RDAZ;
					}	
				}
			}
			kk++;
			if(mm>iWin-1)
			{
				//���㴰���ڷ�λ��׼��
				RDAZ0=0.f;
				nn=0;
				for(i=0;i<kk;i++)
				{
					if(pAz[i]>0.001f)
					{
						RDAZ0+=pAz[i];
						nn++;
					}
				}
				if(nn>iWin*m_NumMin/100.f)
				{
					RDAZ0/=nn;
					RDAZ=0.f;
					for(i=0;i<kk;i++)
					{
						if(pAz[i]>0.001f)
							RDAZ+=(pAz[i]-RDAZ0)*(pAz[i]-RDAZ0);
					}
					RDAZ/=nn;
					RDAZ=sqrt(RDAZ);		
					if(RDAZ<m_AzMax)
					{
						//��λ����������תУ��
						RDAZ0=OutXX[3];
						//�Գƻ������Է�λ
						RDAZ=RDAZ0+180.;
						while(RDAZ>360.) 
							RDAZ-=360.;
						if(RDAZ>RDAZ0)
						{
							OutXX[3]=(float)RDAZ0;
							OutXX[4]=(float)RDAZ;
						}
						else
						{
							OutXX[3]=(float)RDAZ;
							OutXX[4]=(float)RDAZ0;
						}
						pLog->Dep=dep;
						pLog->DatOut();
					}
				}
				for(i=1;i<kk;i++)
					pAz[i-1]=pAz[i];
				kk--;
			}
			mm++;
			k+=iRlev;
			dep=m_StProDep+m_Rlev*k;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton())
			{
				bUserBreak=TRUE;
				break;
			}						
		}	
		delete OutXX;delete pOutCurve;delete pLog;
		Infp.Close ();
		delete Value;delete pAz;
	}
	else bUserBreak=TRUE;
	delete pGuage;
	m_DataNum=Num;

	if(bUserBreak) return FALSE;
	else           return TRUE;
}
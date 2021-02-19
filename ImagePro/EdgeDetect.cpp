// DiffrenceCp.cpp: implementation of the CEdgeDetect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "flowchart.h"
#include "EdgeDetect.h"
#include "FlowNode.h"
#include "Gradient.h"
#include "FIDio.h"
#include "Guage.h"
#include "AbnormalDlg.h"
#include "ImageGeoCorrect.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CEdgeDetect,CFlowNode, 0)
CEdgeDetect::CEdgeDetect()
{
	m_pDoc=NULL;
}

CEdgeDetect::~CEdgeDetect()
{

}

CEdgeDetect::CEdgeDetect(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
	m_Win=0.6069f;
	m_WinRlev=0.1524f;
	m_flagValue=756.f;
	m_MinValue=0.f;
	m_MaxValue=1024.f;
	m_EdgeType=0;
	m_szEdgeName="动态图像";
}

void CEdgeDetect::Serialize(CArchive& ar)
{
	CFlowNode::Serialize(ar);	
	if (ar.IsStoring())
	{
		ar<<m_szEdgeName;
		ar<<m_Win;
		ar<<m_WinRlev;
		ar<<m_flagValue;
		ar<<m_EdgeType;
	}
	else
	{
		ar>>m_szEdgeName;
		ar>>m_Win;
		ar>>m_WinRlev;
		ar>>m_flagValue;
		ar>>m_EdgeType;
	}
}

BOOL CEdgeDetect::Run()
{
	m_bResult=TRUE;
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	if(!m_pDoc->m_bAunAll)
	{
		CAbnormalDlg dlg;
		dlg.m_szBufPath=m_szBufPath;
		dlg.m_EnProDep=m_EnProDep;
		dlg.m_StProDep=m_StProDep;
		dlg.m_Rlev=m_Rlev;
		dlg.m_Win=m_Win;
		dlg.m_WinRlev=m_WinRlev;
		dlg.m_flagValue=m_flagValue;
		dlg.m_MinValue=0.f;
		dlg.m_MaxValue=1024.f;
		dlg.m_szEdgeName=m_szEdgeName;
		dlg.m_EdgeType=m_EdgeType;
		dlg.m_PadNum=m_PadNum;
		dlg.m_ButtonNum=m_ButtonNum;
		dlg.m_DataNum=m_DataNum;
		if(dlg.DoModal()==IDOK)
		{
			m_Win=dlg.m_Win;
			m_WinRlev=dlg.m_WinRlev;
			m_flagValue=dlg.m_flagValue;
			m_EdgeType=dlg.m_EdgeType;
			m_szEdgeName=dlg.m_szEdgeName;
			CString OutName=dlg.m_szOutStaName;
			CString ImgName=dlg.m_szOutImgName;
			BOOL bCreatImg=dlg.m_bCreatImg;
			CString InName="EDGE";
			StaticEdge(InName,OutName);
			if(bCreatImg)
			{
				if(CreatImgData(InName))
				{
					CImageGeoCorrect Edge("检测图像几何校正",FALSE);
					Edge.m_szImageType ="检测图像";
					Edge.InitPara(m_pDoc);
					Edge.m_szInName="EDGE";
					Edge.m_szOutName=ImgName;
					Edge.Run();
				}
				else
					AfxMessageBox("创建检测图像失败        ");
			}
		}
	}
	return m_bResult;
}

void CEdgeDetect::InitPara(CFlowChartDoc* pDoc)
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

BOOL CEdgeDetect::StaticEdge(CString InName,CString OutName)
{
	int i;
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	float *Value=NULL;
	Value=new float[RecordNum];

	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());	
	pGuage->ShowWindow(SW_SHOW);
	CString ss=OutName+" 生成统计曲线...";
	pGuage->SetTitle(ss);
	BOOL bUserBreak=FALSE;
	int iWin=(int)(m_Win/m_Rlev);
	if(iWin<2) iWin=2;
	int iRlev=(int)(m_WinRlev/m_Rlev);
	if(iRlev<1) iRlev=1;
	if(iRlev>iWin-1) iRlev=iWin-1;

	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");strcat(s,InName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile Infp;
	if(Infp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		FIDCONTENT ContentW[2] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
									{"EDSTA","%",REPR_FLOAT,4,1,1,0.f,100.f,0.f}};
		ContentW[0].min = m_StProDep;
		ContentW[0].max = m_EnProDep;
		ContentW[0].rlev =m_Rlev*iRlev;
		
		strcpy(ContentW[1].name,OutName);
		ContentW[1].rlev=m_Rlev*iRlev;
		FIDINDEX IndexW;
		IndexW.dimension = 2;
		IndexW.Contentlist = ContentW;
		char adi[] = "==END==";
  		IndexW.adi = adi;

		CFIDIO* pFIDW = NULL;
		pFIDW= new CFIDIO(m_szWellName+"."+OutName,&IndexW);
		if(pFIDW->Open())
		{
			int Block=m_DataNum*4;
			float *Value;
			Value=new float[m_DataNum*iWin+10];
			int j,k=0,fNum;
			float fdata,dep;
			dep=m_StProDep;
			pFIDW->SeekToDepth(m_StProDep+m_Rlev*iWin/2);
			while(k<RecordNum-iWin)
			{
				Infp.Seek (k*Block,CFile::begin);
				Infp.Read (Value,Block*iWin);
				
				fNum=0;
				for(j=0;j<iWin;j++)
				{
					for(i=0;i<m_DataNum;i++)
					{	
						if(m_EdgeType==1)
						{
							if(Value[j*m_DataNum+i]<m_flagValue)	
							fNum++;
						}
						else
						{
							if(Value[j*m_DataNum+i]>m_flagValue)	
								fNum++;
						}
					}
				}
				fdata=fNum*100.f/m_DataNum/iWin;
				pFIDW->Write(2,1,&fdata);
				pFIDW->Next();
				k+=iRlev;
				dep=m_StProDep+m_Rlev*k;
				pGuage->SetDepth(dep);
				if (pGuage->CheckCancelButton())
				{
					bUserBreak=TRUE;
					break;
				}
			}
			delete Value;
		}
		delete pFIDW;
	}
	else bUserBreak=TRUE;
	Infp.Close ();
	delete pGuage;
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

BOOL CEdgeDetect::CreatImgData(CString InName)
{
	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");strcat(s,InName);
	CFIDIO::GetFileName(s,fn,'D');	
	BOOL bUserBreak=FALSE;
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		int Block=m_DataNum*4;
		//定义进度指示器
		float dep=m_StProDep;
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->ShowWindow(SW_SHOW);
		pGuage->SetTitle(m_szEdgeName+"生成图像数据...");
		
		float *fwf=NULL;
		fwf=new float[m_DataNum];
		int i,k=0;
		while(k<RecordNum)
		{
			fp.Seek (k*Block,CFile::begin);
			fp.Read (fwf,Block);	
			if(m_EdgeType==1)
			{
				for(i=0;i<m_DataNum;i++)
					if(fwf[i]>m_flagValue)
						fwf[i]=-9999.f;
			}
			else
			{
				for(i=0;i<m_DataNum;i++)
					if(fwf[i]<m_flagValue)
						fwf[i]=-9999.f;
			}
			fp.Seek (k*Block,CFile::begin);
			fp.Write(fwf,Block);
			k++;
			dep=m_StProDep+m_Rlev*k;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) break;
		}
		delete fwf;
		delete pGuage;
	}
	else bUserBreak=TRUE;
	fp.Close ();
	if(bUserBreak) return FALSE;
	else return TRUE;
}
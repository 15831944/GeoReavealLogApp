// CrackCompute.cpp: implementation of the CCrackCompute class.CCrackCompute
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "flowchart.h"
#include "CrackCompute.h"
#include "FlowNode.h"
#include "FlowChartDoc.h"
#include "ImageCreate.h"
#include "CrackDlg.h"
#include "FIDio.h"
#include "LogProcess.h"
#include "Guage.h"
#include "Integrate.h"
#include <math.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSinPoint::CSinPoint(float Dep,float Dip,float Dir,CString Flag)
{
	m_Dep=Dep;
	m_Dip=Dip;
	m_Dir=Dir;
	m_Flag=Flag;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CCrackCompute,CFlowNode, 0)
CCrackCompute::CCrackCompute()
{
	m_pDoc=NULL;
}

CCrackCompute::~CCrackCompute()
{
	POSITION pos = m_SinPoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_SinPoint.GetNext(pos);
	m_SinPoint.RemoveAll();
}

CCrackCompute::CCrackCompute(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_szCp="张开缝";
	m_WinWide=0.0254f;
	m_flagValue=200.f;
	m_ParA=1.00292f;
	m_ParB=0.000221684f;
	m_Win=1.f;
	m_WinRlev=0.125f;
	m_XM=2.f;
	m_XN=2.f;
}

void CCrackCompute::Serialize(CArchive& ar)
{
	CFlowNode::Serialize(ar);	
	if (ar.IsStoring())
	{
		ar<<m_szCp;
		ar<<m_WinWide;
		ar<<m_flagValue;
		ar<<m_ParA;
		ar<<m_ParB;
		ar<<m_FlagFre;
		ar<<m_Win;
		ar<<m_WinRlev;
		ar<<m_XM;
		ar<<m_XN;
	}
	else
	{
		ar>>m_szCp;
		ar>>m_WinWide;
		ar>>m_flagValue;
		ar>>m_ParA;
		ar>>m_ParB;
		ar>>m_FlagFre;
		ar>>m_Win;
		ar>>m_WinRlev;
		ar>>m_XM;
		ar>>m_XN;
	}
}
void CCrackCompute::CreatePoint()
{
	POSITION pos = m_SinPoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_SinPoint.GetNext(pos);
	m_SinPoint.RemoveAll();
	CString HandWorkFile=m_szWellName.Left(m_szWellName.ReverseFind('\\')+1)+"HandWork.List";
	if(DetectFileExist(HandWorkFile))
	{
		CString Flag;
		float Dep,Dip,Dir,Dip0,Dir0;
		TCHAR *token, *end;
		TCHAR buffer[1024];
		CStdioFile File;
		CFileException ex;
		if (!File.Open(HandWorkFile, CFile::modeRead | CFile::typeText)) 
			return;
		File.ReadString(buffer, 1024);	// Read Header off file	
		while (File.ReadString(buffer, 1024)) 
		{
			for (token=buffer, end=buffer; 
					*end && (*end != _T(',')) && (*end != _T('\n')); end++);
			if ((*end == _T('\0')) && (token == end)) token = NULL;
			*end = _T('\0');
			
			int i=0;CString str;
			while (token) 
			{
				str=token;
				while(str.Left(1)==" ")
					str.TrimLeft (" ");
				while(str.Right(1)==" ")
					str.TrimRight (" ");
				if(i==1)
					Dep=(float)atof(str);
				else if(i==2)
					Dip=(float)atof(str);
				else if(i==3)
					Dir=(float)atof(str);
				else if(i==4)
					Flag=str;
				else if(i==5)
					Dip0=(float)atof(str);
				else if(i==6)
					Dir0=(float)atof(str);
				// Get next token
				for (token=++end; *end && (*end != _T(',')) && (*end != _T('\n'));
						end++);
				if ((*end == _T('\0')) && (token == end)) token = NULL;
				*end = _T('\0');
				i++;
			}
			if(Dep<m_StProDep)
				continue;
			if(Dep>m_EnProDep)
				continue;
			if((Flag=="充填缝")||(Flag=="张开缝")
				||(Flag=="半充填缝")||(Flag=="层间缝"))
			{
				CSinPoint* pObj=new CSinPoint(Dep,Dip,Dir,Flag);
				pObj->m_Hight=(int)(m_Bits/2000.f*tan(Dip*PI/180.)/m_Rlev);
				pObj->m_Dip0=Dip0;
				pObj->m_Dir0=Dir0;
				m_SinPoint.AddTail(pObj);
			}
		}
		File.Close();
	}
}

BOOL CCrackCompute::Run()
{
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	m_bResult=TRUE;
	CreatePoint();
	if(!m_pDoc->m_bAunAll)
	{
		CCrackDlg Dlg(this);
		Dlg.m_szBufPath =m_szBufPath;
		Dlg.m_CurveName ="DATARS";
		Dlg.m_ProEnDep =m_EnProDep;
		Dlg.m_ProStDep =m_StProDep;
		Dlg.m_Rlev=m_Rlev;
		Dlg.m_szCp=m_szCp;
		Dlg.m_WinWide=m_WinWide;
		Dlg.m_flagValue=m_flagValue;
		Dlg.m_ParA=m_ParA;
		Dlg.m_ParB=m_ParB;
		Dlg.m_FlagFre=m_FlagFre;
		Dlg.m_Win=m_Win;
		Dlg.m_WinRlev=m_WinRlev;
		if(Dlg.DoModal()!=IDOK)
			return TRUE;
		else
		{
			m_szCp=Dlg.m_szCp;
			m_WinWide=Dlg.m_WinWide;
			m_flagValue=Dlg.m_flagValue;
			m_ParA=Dlg.m_ParA;
			m_ParB=Dlg.m_ParB;
			m_FlagFre=Dlg.m_FlagFre;
			m_Win=Dlg.m_Win;
			m_WinRlev=Dlg.m_WinRlev;
		}
	}
	StaticCrack("DATARS");
	return m_bResult;
}

void CCrackCompute::InitPara(CFlowChartDoc* pDoc)
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

BOOL CCrackCompute::StaticCrack(CString InName)
{
	//视裂缝长度            FVTL
	//视裂缝宽度            FVA
	//视裂缝水动力宽度      FVAH
	//视裂缝密度            FVDA
	//视裂缝密度            FVDC
	//视裂缝孔隙度          FVPA  

	POSITION pos = m_SinPoint.GetHeadPosition();
	if(pos== NULL) 
		return FALSE;
	CFIDIO* pFIDRXO=NULL;
	CString szRXOName=m_szBufPath+".RMFGPT";
	pFIDRXO = new CFIDIO(szRXOName);
	if(!pFIDRXO->Open()) 
		{delete pFIDRXO; return FALSE;}
	pFIDRXO->SeekToDepth(m_StProDep);

	int Off=(int)(m_WinWide/m_Rlev);
	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");
	strcat(s,InName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;	
	if(!fp.Open(fn,CFile::modeRead|CFile::typeBinary))
		return FALSE;
	int nps= m_PadNum*m_ButtonNum;
	int i,j,y,Num;
	int RecordNum,Block;
	int AllNum,CrackNum;
	float PVHight,dep;
	float pi2=(float)(PI*2.);
	float WinS=pi2*m_Bits/2000.f*m_Win;
	float RXO,tl,va,ax1,ax2;
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());	
	pGuage->ShowWindow(SW_SHOW);
	pGuage->SetTitle("统计裂缝储层参数表(Crack.List)...");
	CString str;
	CStdioFile File;
	double dtor=360./nps;
	CString CrackFile=m_szWellName.Left(m_szWellName.ReverseFind('\\')+1)+"Crack.List";
	if(File.Open(CrackFile, CFile::modeWrite | CFile::modeCreate| CFile::typeText)) 
	{
		int nb=1;
		str="序号,深度,裂缝角度,裂缝方位,裂缝类型,裂缝长度,裂缝宽度,水动力宽度";
		File.WriteString(str);
		while (pos!= NULL) 
		{
			CSinPoint *pSin=(CSinPoint *)m_SinPoint.GetNext(pos);
			if(pSin!=NULL)
			{
				if(m_szCp.Find(pSin->m_Flag)>-1)
				{		
					RecordNum=(int)((pSin->m_Dep-m_StProDep)/m_Rlev)-pSin->m_Hight-Off;
					if(RecordNum<0) RecordNum=0;
					Block=nps*(pSin->m_Hight+Off)*2;
					str.Format("统计%d裂缝储层参数(%g)...",nb,pSin->m_Dep);
					pGuage->SetTitle(str);	
					float* Value = NULL;
					Value = new float[Block];
					fp.Seek (RecordNum*nps*4,CFile::begin);
					fp.Read (Value,Block*4);
					//按裂缝轨迹搜索异常点数比
					AllNum=0;CrackNum=0;
					for(j=pSin->m_Hight-Off;j<pSin->m_Hight+Off;j++)
					{
						for(i=0;i<nps;i++)
						{
							y=j+(int)(cos((i*dtor-pSin->m_Dir)*PI/180.)*pSin->m_Hight);
							Num=y*nps+i;
							if(Num<0) Num=0;
							if(Num>Block-1) Num=Block-1;
							if(Value[Num]<m_flagValue)
								CrackNum++;
							AllNum++;
						}
					}
					delete Value;
					//裂缝在垂向上的跨越井段
					PVHight=m_Bits/2000.f*(float)(tan(pSin->m_Dip*PI/180.));
					// 计算裂缝长度
					// exp=sqrt(1+H*H*pow(sin(x-Dir),2)
					CIntegrate  Intgrt;
					char expres[256];
					str.Format("sqrt(1.0+%f*pow(sin(x-%f),2.0))",PVHight*PVHight,pSin->m_Dir*PI/180.);
					strcpy(expres,str);
					
					Intgrt.initial(expres,pi2,0);
					Intgrt.setError(0.0001);
					if(Intgrt.integrate())
						pSin->FVTL=(float)Intgrt.getvalue();
					else
						pSin->FVTL=0.f;

					if(AllNum>0)
					{
						//计算裂缝宽度
						pSin->FVA=m_WinWide*2.f*CrackNum/AllNum;
						//计算裂缝水动力宽度
						pFIDRXO->SeekToDepth(pSin->m_Dep);
						pFIDRXO->Read(2,1,&RXO);
						pSin->FVAH=m_ParA*pSin->FVTL*pSin->FVA*pow(RXO,m_ParB)*pow(m_Rm,1.f-m_ParB);
						//裂缝孔隙度
						pSin->FVPA=CrackNum*100.f/(PVHight*2.f*nps/m_Rlev);
						//裂缝在垂向上的跨越井段
						str.Format("\n%d,%g,%g,%g,%s,%g,%g,%g",
								nb,pSin->m_Dep,pSin->m_Dip0,pSin->m_Dir0,pSin->m_Flag,pSin->FVTL,pSin->FVA,pSin->FVAH);
						File.WriteString(str);
						nb++;
					}
				}
			}	
		}
	}
	File.Close();

	//初始化输出曲线
	CLogProcess *pLog;
	pLog = new CLogProcess(m_szWellName,NULL);
	pLog->StartDep=((int)(m_StProDep/m_WinRlev))*m_WinRlev;
	pLog->EndDep=((int)(m_EnProDep/m_WinRlev))*m_WinRlev;
	pLog->DepLevel=m_WinRlev;
	//初始化输出
	OUTCURVE *pOutCurve = NULL;
	pOutCurve = new OUTCURVE[6];
	strcpy(pOutCurve[0].name,"FVTL");
	strcpy(pOutCurve[1].name,"FVA");
	strcpy(pOutCurve[2].name,"FVAH");
	strcpy(pOutCurve[3].name,"FVDA");
	strcpy(pOutCurve[4].name,"FVPA");
	strcpy(pOutCurve[5].name,"FVDC");
	float *OutXX;
	OutXX = new float[6];
	pLog->InitOut(6,pOutCurve,(char *)OutXX);
	pLog->InitBuffer();

	float dir;
	int x1,x2,nn;
	float dep1,dep2;
	float WinDep1,WinDep2;
	float CraDep1,CraDep2;
	float FVTL,FVA,FVAH,FVDA,FVPA,FVDC;
	pGuage->SetTitle("统计裂缝储层参数曲线...");
	pGuage->SetDepthRange(pLog->StartDep,pLog->EndDep);
	int DPN=0;dep=pLog->StartDep;
	while(dep<m_EnProDep)
	{
		FVTL=FVA=FVAH=FVDA=FVPA=FVDC=0.f;
		WinDep1=dep-m_Win/2.f;
		WinDep2=dep+m_Win/2.f;
		POSITION pos = m_SinPoint.GetHeadPosition();
		while (pos!= NULL) 
		{
 		    CSinPoint *pSin=(CSinPoint *)m_SinPoint.GetNext(pos);
			if(pSin!=NULL)
			{
				if(m_szCp.Find(pSin->m_Flag)>-1)
				{		
					//裂缝在垂向上的跨越井段
					PVHight=m_Bits/2000.f*(float)(tan(pSin->m_Dip*PI/180.f));
					CraDep1=pSin->m_Dep-PVHight;
					CraDep2=pSin->m_Dep+PVHight;
					dep1=0.f;dep2=0.f;
					if(WinDep1<CraDep1)
						dep1=CraDep1;
					else
						dep1=WinDep1;
					if(WinDep2>CraDep2)
						dep2=CraDep2;
					else
						dep2=WinDep2;
					if(dep>=CraDep1&&dep<=CraDep2)
						FVDC++;
					if(dep2>dep1&&dep1>=CraDep1&&dep2<=CraDep2)
					{
						FVDA++;
						dir=(float)(pSin->m_Dir*PI/180.);
						ax1=acos(fabs(pSin->m_Dep-dep1)/PVHight)+dir;
						ax2=acos(fabs(dep2-pSin->m_Dep)/PVHight)+dir;
						if(dep2-dep1>=PVHight*2.f)
						{
							x1=0;
							x2=nps;
							ax1=0.f;
							ax2=pi2;
						}
						else if(dep2>=CraDep2)
						{
							x1=(int)(ax1*nps/pi2);
							x2=x1+nps/2;
							ax2=(float)(ax1+PI);
							if(x2>nps)
							{
								x2=x1;
								x1=x2-nps/2;
								ax2=ax1;
								ax1=(float)(ax2-PI);
							}
						}
						else if(dep1<=CraDep1)
						{
							x1=(int)(ax2*nps/pi2);
							x2=x1+nps/2;
							ax1=ax2;
							ax2=(float)(ax1+PI);
							if(x2>nps) 
							{
								x2=x1;
								x1=x1-nps/2;
								ax2=ax1;
								ax1=(float)(ax1-PI);
							}
						}
						else
						{
							if(ax1>ax2)
							{
								tl=ax1;
								ax1=ax2;
								ax2=tl;
							}
							x1=(int)(ax1*nps/pi2);
							if(x1<0) x1=0;
							x2=(int)(ax2*nps/pi2);
							if(x2>nps) x2=nps;
							x2=x1;
							ax2=ax1;
						}	
						// exp=sqrt(1+H*H*pow(sin(x-Dir),2)
						CIntegrate  Intgrt;
						char expres[256];
						str.Format("sqrt(1.0+%f*pow(sin(x-%f),2.0))",PVHight*PVHight,dir);
						strcpy(expres,str);
						Intgrt.initial(expres,ax2,ax1);
						Intgrt.setError(0.0001);
						if(Intgrt.integrate())
						{
							tl=(float)Intgrt.getvalue();
							FVTL+=tl;
						}
						RecordNum=(unsigned long)((pSin->m_Dep-m_StProDep)/m_Rlev)-pSin->m_Hight-Off;
						if(RecordNum<0) RecordNum=0;
						Block=nps*(pSin->m_Hight+Off)*2;	
						float* Value1 = NULL;
						Value1 = new float[Block];
						fp.Seek (RecordNum*nps*4,CFile::begin);
						fp.Read (Value1,Block*4);
						//按裂缝轨迹搜索异常点数比
						AllNum=0;CrackNum=0;
						for(j=pSin->m_Hight-Off;j<pSin->m_Hight+Off;j++)
						{
							for(i=x1;i<x2;i++)
							{
								y=j+(int)(cos((i*dtor-pSin->m_Dir)*PI/180.)*pSin->m_Hight);
								nn=y*nps+i;
								if(nn<0) nn=0;
								if(nn>Block-1) nn=Block-1;
								if(Value1[nn]<m_flagValue)
									CrackNum++;
								AllNum++;
							}
						}
						delete Value1;
						if(AllNum>0)
						{
							//计算裂缝宽度
							va=m_WinWide*2.f*CrackNum/AllNum;
							FVA+=va;
							//计算裂缝水动力宽度
							pFIDRXO->SeekToDepth(dep);
							pFIDRXO->Read(2,1,&RXO);
							FVAH+=m_ParA*tl*va*pow(RXO,m_ParB)*pow(m_Rm,1.f-m_ParB);
							//裂缝孔隙度
							FVPA+=CrackNum;
						}
					}
				}
			}	
		}
		OutXX[0]=FVTL/WinS;
		OutXX[1]=FVA;///WinS;
		OutXX[2]=FVAH;///WinS;
		OutXX[3]=FVDA/m_Win;
		OutXX[4]=FVPA*100.f/(m_Win*nps/m_Rlev);
		OutXX[5]=FVDC;
		pLog->Dep=dep;
		pLog->DatOut();
		dep=pLog->StartDep+DPN*m_WinRlev;
		pGuage->SetDepth(dep);
		if(pGuage->CheckCancelButton())
			break;
		DPN++;
	}
	fp.Close ();
	delete pFIDRXO;
	delete pGuage;
	delete OutXX;
	delete pOutCurve;
	delete pLog;
	return TRUE;
}
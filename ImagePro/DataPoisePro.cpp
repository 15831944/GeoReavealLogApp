// PadPoise.cpp: implementation of the CDataPoisePro class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FlowChart.h"
#include "DataPoisePro.h"
#include "FlowChartDoc.h"
#include "FlowNode.h"
#include "BackupData.h"
#include "DataPoisDlg.h"
#include "FitCircle.h"
#include "FIDio.h"
#include "Guage.h"
#include <math.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CDataPoisePro, CFlowNode, 0)
CDataPoisePro::CDataPoisePro()
{
	m_pDoc=NULL;
}

CDataPoisePro::~CDataPoisePro()
{

}

CDataPoisePro::CDataPoisePro(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
	m_szButNo=_T("");               //异常电扣号
	m_szPadNo=_T("");               //异常极板号

	m_MaxFre=60.f;                  //异常电扣频率值
	m_ScaleMin=-9999.f;             //异常电扣极小值
	m_ScaleMax=-9999.f;             //异常电扣极大值

	m_PadMax=-9999.f;               //异常极板极大值
	m_ButMin=-9999.f;               //滤波时处理的最小电扣值
	m_bFilter=TRUE;                 //平滑滤波处理
	m_bAutoPad=FALSE;               //极板均衡处理
	m_bAutoBut=FALSE;               //电扣均衡处理
	m_bLogScal=TRUE;                //对数刻度
}

void CDataPoisePro::Serialize(CArchive& ar)
{
	CFlowNode::Serialize(ar);
	if(ar.IsStoring())
	{
		ar<<m_szButNo;
		ar<<m_szPadNo;
		ar<<m_MaxFre;
		ar<<m_ScaleMin;
		ar<<m_ScaleMax;
		ar<<m_ButMin;
		ar<<m_bFilter;
		ar<<m_bAutoPad;
		ar<<m_bLogScal;
		ar<<m_bAutoBut;
	}
	else
	{
		ar>>m_szButNo;
		ar>>m_szPadNo;
		ar>>m_MaxFre;
		ar>>m_ScaleMin;
		ar>>m_ScaleMax;
		ar>>m_ButMin;
		ar>>m_bFilter;
		ar>>m_bAutoPad;
		ar>>m_bLogScal;
		ar>>m_bAutoBut;
	}
	if(m_MaxFre<0) m_MaxFre=60.f;
}

BOOL CDataPoisePro::Run()
{
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	if(!DataCopy("DATA00","DATAPS"))
		return FALSE;
	if(!m_pDoc->m_bAunAll)
	{
		CDataPoisDlg Dlg(this);
		Dlg.m_szBufPath =m_szBufPath;
		Dlg.m_CurveName0 ="DATAPS";
		Dlg.m_EnDep =m_EnProDep;
		Dlg.m_StDep =m_StProDep;
		Dlg.m_Rlev=m_Rlev;
		Dlg.m_szButNo=m_szButNo;
		Dlg.m_szPadNo=m_szPadNo;
		Dlg.m_MaxFre=m_MaxFre;
		Dlg.m_bFilter=m_bFilter;
		Dlg.m_ButMin=m_ButMin;

		Dlg.m_bLogScal=m_bLogScal;
		Dlg.m_bAutoPad=m_bAutoPad;
		Dlg.m_bAutoBut=m_bAutoBut;
		Dlg.m_ScaleMin=m_ScaleMin;
		Dlg.m_ScaleMax=m_ScaleMax;
		if(Dlg.DoModal()!=IDOK)
			return TRUE;
		m_szButNo=Dlg.m_szButNo;
		m_szPadNo=Dlg.m_szPadNo;
		m_MaxFre=Dlg.m_MaxFre;
		m_bFilter=Dlg.m_bFilter;
		m_ButMin=Dlg.m_ButMin;
		m_bLogScal=Dlg.m_bLogScal;
		m_bAutoPad=Dlg.m_bAutoPad;
		m_bAutoBut=Dlg.m_bAutoBut;
		m_ScaleMin=Dlg.m_ScaleMin;
		m_ScaleMax=Dlg.m_ScaleMax;
	}
	if(m_szPadNo!="")
		PadBalancePro("DATAPS");
	if(m_szButNo!="")
		ButBalancePro("DATAPS");
	if(m_bFilter)
		DataFilterPro(m_szBufPath,"DATAPS");
	m_bResult=DataScalePro(m_szBufPath,"DATAPS");
	//图像几何校正
	if(m_szSeries=="AMK2000"||m_szSeries=="ARI_16")
	{
		if(m_szSeries=="AMK2000")
			MITGeometryCorrect("DATAPS","SGDI");
	}
	return m_bResult;
}
BOOL CDataPoisePro::AutoButBalancePro(CString Name)
{
	int PadNum=m_PadNum;
	int ButNum=m_ButtonNum;
	int RecNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	int nWin=120,nSav=1;//3英寸
	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");strcat(s,Name);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	BOOL bBreak=FALSE;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		DWORD Block=m_DataNum*nWin*4;
		float *Value=new float[m_DataNum*nWin];

		int i,k,m,n;
		//电扣标准偏差、均值
		float AllQ,AllAver,tmp;
		//极板标准偏差、均值
		float *pButQ=new float[ButNum];
		float *pButAver=new float[ButNum];
		
		//定义进度指示器
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());	
		pGuage->SetTitle(Name+" 电扣均衡处理...");
		pGuage->ShowWindow(SW_SHOW);
		//
		//计算电扣极板均值标准偏差
		///////////////////////////////////////////////////////////////////////////////
		//
		m=0;
		int nWin0=nWin;
		while(m<RecNum)
		{
			//处理最后一个窗口
			if(m+nWin>=RecNum)
				nWin=RecNum-m-1;

			fp.Seek(m*m_DataNum*4L,CFile::begin);
			fp.Read(Value,Block);
			for(n=0;n<PadNum;n++)
			{
				//计算1个极板每个电扣窗口内的和值
				for(i=0;i<ButNum;i++)
				{
					k=0;
					pButAver[i]=0.f;
					for(k=0;k<nWin;k++)
						pButAver[i]+=Value[k*m_DataNum+n*ButNum+i];
				}
				//计算每个电扣均值
				for(i=0;i<ButNum;i++)
					pButAver[i]/=nWin;
				
				//计算极板均值
				AllAver=0.f;
				for(i=0;i<ButNum;i++)
					AllAver+=pButAver[i];
				AllAver/=ButNum;
				
				///////////////////////////////////////////////////////////////////////////////
				//计算标准偏差
				//计算1个极板所有电扣偏差平方和
				AllQ=0.f;
				for(k=0;k<nWin;k++)
					for(i=0;i<ButNum;i++)
					{
						tmp=(Value[k*m_DataNum+n*ButNum+i]-AllAver)*(Value[k*m_DataNum+n*ButNum+i]-AllAver);
						AllQ+=tmp;	
					}
				//计算极板标准差
				AllQ=sqrt(AllQ/(ButNum*nWin));
				//计算每个电扣窗口内的偏差平方和
				for(i=0;i<ButNum;i++)
				{
					pButQ[i]=0.f;
					for(k=0;k<nWin;k++)
					{
						tmp=(Value[k*m_DataNum+n*ButNum+i]-pButAver[i])*(Value[k*m_DataNum+n*ButNum+i]-pButAver[i]);
						pButQ[i]+=tmp;			
					}
				}
				//计算电扣标准差
				for(i=0;i<ButNum;i++)
					pButQ[i]=sqrt(pButQ[i]/nWin);
					
				//电扣自动均衡处理
				///////////////////////////////////////////////////////////////////////////////////////
				for(i=0;i<ButNum;i++)
				{
					if(pButQ[i]>0.f)
					{
						for(k=0;k<nWin;k++)
							Value[k*m_DataNum+n*ButNum+i]=(AllQ/pButQ[i])*(Value[k*m_DataNum+n*ButNum+i]-pButAver[i])+AllAver;
					}
				}
			}
			if(nWin0>nWin)
			{
				fp.Seek(m_DataNum*m*4L,CFile::begin);
				fp.Write(Value,Block);
				break;
			}
			fp.Seek(m_DataNum*m*4L,CFile::begin);
			fp.Write(Value,m_DataNum*nSav*4);

			m+=nSav;
			pGuage->SetDepth(m_StProDep+m_Rlev*m);
			if(pGuage->CheckCancelButton())
			{
				bBreak=TRUE;
				break;
			}
		}
		
		delete pGuage;delete Value;
		delete pButQ;delete pButAver;
	}
	else bBreak=TRUE;
	fp.Close ();
	return !bBreak;
}
BOOL CDataPoisePro::DataScalePro(CString Path,CString Name)
{
	char s[256],fn[256];
	strcpy(s,Path);strcat(s,".");strcat(s,Name);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	BOOL bBreak=FALSE;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		//定义进度指示器
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->SetTitle("数据归一化处理...");
		pGuage->ShowWindow(SW_SHOW);

		int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
		float* fwf=new float[m_DataNum];
		int Block=m_DataNum*4;
		int i,k;
		float lgMin,lgMax;
		if(m_bLogScal)
		{
			lgMin=log10(m_ScaleMin);
			lgMax=log10(m_ScaleMax);
			fp.Seek(0L,CFile::begin);
			for(k=0;k<RecordNum;k++)
			{
				fp.Read (fwf,Block);
				for(i=0;i<m_DataNum;i++)
				{
					if(fwf[i]>0.f)
						fwf[i]=(float)((log10(fwf[i])-lgMin)/(lgMax-lgMin)*1024.f);
					else
						fwf[i]=0.f;
				}
				fp.Seek (k*Block,CFile::begin);
				fp.Write (fwf,Block);
				pGuage->SetDepth(m_StProDep+m_Rlev*k);
				if(pGuage->CheckCancelButton()) 
				{
					bBreak=TRUE;
					break;
				}
			}
		}
		else
		{
			lgMin=m_ScaleMin;
			lgMax=m_ScaleMax;
			fp.Seek(0L,CFile::begin);
			for(k=0;k<RecordNum;k++)
			{
				fp.Read (fwf,Block);
				for(i=0;i<m_DataNum;i++)
					fwf[i]=(fwf[i]-lgMin)/(lgMax-lgMin)*1024.f;
					
				fp.Seek (k*Block,CFile::begin);
				fp.Write(fwf,Block);
				pGuage->SetDepth(m_StProDep+m_Rlev*k);
				if(pGuage->CheckCancelButton()) 
				{
					bBreak=TRUE;
					break;
				}
			}
		}
		delete fwf;delete pGuage;
		fp.Close ();
	}
	else bBreak=TRUE;
	return !bBreak;
}

void CDataPoisePro::InitPara(CFlowChartDoc* pDoc)
{
	POSITION pos = pDoc->m_ParentList.GetHeadPosition();
	while (pos != NULL)
	{
		CFlowNode *pObj = (CFlowNode*)pDoc->m_ParentList.GetNext(pos);
		if(pObj != NULL) 
		{		
			if(pObj->IsKindOf(RUNTIME_CLASS(CBackupData)))
			{
				m_szBufPath=((CBackupData*)pObj)->m_szBufPath;
				m_szWellName=((CBackupData*)pObj)->m_szWellName;
				m_szSeries=((CBackupData*)pObj)->m_szSeries;
				m_PadNum=((CBackupData*)pObj)->m_PadNum; 
				m_ButtonNum=((CBackupData*)pObj)->m_ButtonNum;	
				m_CalNum=((CBackupData*)pObj)->m_CalNum;
				m_StProDep=((CBackupData*)pObj)->m_StProDep;
				m_EnProDep=((CBackupData*)pObj)->m_EnProDep;
				m_Rlev=((CBackupData*)pObj)->m_Rlev;
				m_Bits=((CBackupData*)pObj)->m_Bits;
				m_ButtonMid=12;
				if(m_szSeries=="EMI")
					m_ButtonMid=13;
				m_DataNum=m_PadNum*m_ButtonNum;
			}
		}
	}
	m_pDoc=pDoc;
}

//数据区间刻度
BOOL CDataPoisePro::DataMaxMinPro(CString CurveName,float Max,float Min)
{
	int RecordNum;
	char s[256],fn[256];
	int Block=m_DataNum*4;
	RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	BOOL bUserBreak=FALSE;
	strcpy(s,m_szBufPath);
	strcat(s,".");strcat(s,CurveName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		//定义进度指示器
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->ShowWindow(SW_SHOW);
		CString ss="数据区间刻度...";
		pGuage->SetTitle(ss);
		float dep=m_StProDep;

		float *Value;
		Value=new float[m_DataNum+10];
		int i,k=0;
		float xxx=Max-Min;

		while(k<RecordNum)
		{
			fp.Seek (k*Block,CFile::begin);
			fp.Read (Value,Block);
				
			for(i=0;i<m_DataNum;i++)
				Value[i]=(Value[i]-Min)/xxx*1024.f;
			fp.Seek (k*Block,CFile::begin);
			fp.Write (Value,Block);
			k++;
			dep=m_StProDep+m_Rlev*k;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) 
			{
				bUserBreak=TRUE;
				break;
			}
		}
		delete Value;
		delete pGuage;
	}
	else bUserBreak=TRUE;
	fp.Close ();
	if(bUserBreak) return FALSE;
	else           return TRUE;
}
/////////////////////////////////////////////////////////
//                                                     //
//              指定极板异常处理                       //
//                                                     //
/////////////////////////////////////////////////////////
//计算所有极板平均值的平均值
//对各极的电扣值进行校正
BOOL CDataPoisePro::PadBalancePro(CString Name)
{
	if(m_szPadNo==_T(""))
		return TRUE;
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");strcat(s,Name);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	int iWin=1;//(int)(m_WinLongth/m_Rlev);
	BOOL bUserBreak=FALSE;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		//定义进度指示器
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());	
		pGuage->ShowWindow(SW_SHOW);
		float dep;
		
		int Block=m_DataNum*4;
		float *fPad;
		fPad=new float[m_PadNum];
		float *Value;
		Value=new float[m_DataNum*iWin+10];

		CString szPadNo=m_szPadNo;
		int PadNo,xxx;
		CString szNo;
		while(szPadNo!="")
		{
			xxx=szPadNo.Find(",");
			if(xxx>0)
			{
				szNo=szPadNo.Left (xxx);
				PadNo=atoi(szNo)-1;
				szPadNo=szPadNo.Mid (xxx+1);
			}
			else
			{
				PadNo=atoi(szPadNo)-1;
				szPadNo="";
			}
			if(PadNo<0) continue;
			if(PadNo>m_PadNum-1) continue;
			dep=m_StProDep;
			CString ss=Name+" "+szNo+"号极板均衡处理...";
			pGuage->SetTitle(ss);		
	
			float fdata,fcoef;
			int i,j,k=0;
			while(k<RecordNum)
			{
				fp.Seek (k*Block,CFile::begin);
				if(iWin+k==RecordNum-1) iWin=1;
				fp.Read (Value,Block*iWin);
				//计算窗口内第一个深度点各极板平均值
				for(i=0;i<m_PadNum;i++)
				{
					fdata=Value[i*m_ButtonNum];
					for(j=1;j<m_ButtonNum;j++)
						fdata=(fdata+Value[i*m_ButtonNum+j])/2.f;
					fPad[i]=fdata;
				}
				//计算各极板平均值
				for(int y=1;y<iWin;y++)
					for(i=0;i<m_PadNum;i++)
						for(j=1;j<m_ButtonNum;j++)
							fPad[i]=(fPad[i]+Value[y*m_DataNum+i*m_ButtonNum+j])/2.f;
				//计算其他极板的平均值
				i=PadNo+1;
				if(i>m_PadNum-1)
					i=0;
				fdata=fPad[i];
				for(i=0;i<m_PadNum;i++)
				{
					if(i==PadNo) continue;
					fdata=(fdata+fPad[i])/2.f;
				}
				//对异常极板的电扣值进行校正
				fcoef=fdata-fPad[PadNo];
				for(j=0;j<m_ButtonNum;j++)
					Value[PadNo*m_ButtonNum+j]+=fcoef;

				fp.Seek (k*Block,CFile::begin);
				fp.Write (Value,Block);				
				k++;
				dep=m_StProDep+m_Rlev*k;
				pGuage->SetDepth(dep);
				if (pGuage->CheckCancelButton())
				{
					bUserBreak=TRUE;
					break;
				}
			}
		}
		delete pGuage;
		delete Value;delete fPad;
	}
	else bUserBreak=TRUE;
	fp.Close ();
	if(bUserBreak) return FALSE;
	else           return TRUE;
}
//指定电扣异常处理
BOOL CDataPoisePro::ButBalancePro(CString Name)
{
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");strcat(s,Name);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	BOOL bUserBreak=FALSE;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		//定义进度指示器
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());	
		pGuage->ShowWindow(SW_SHOW);
		pGuage->SetTitle(Name+" 电扣异常值处理..");	
		int Block=m_DataNum*4;
		float *Value=NULL;
		Value=new float[m_DataNum+10];

		//检测指定的极板电扣号是否合法
		float *pBut=NULL;
		pBut=new float[m_DataNum+10];
		int i;
		for(i=0;i<m_DataNum+10;i++)
			pBut[i]=-999.25f;
			
		int BNo,xxx;
		CString s1,s2;
		s1=m_szButNo;
		s1.TrimLeft ();
		s1.TrimRight ();
		while(s1!="")
		{		
			xxx=s1.Find(",");
			if(xxx>0)
			{
				s2=s1.Left(xxx);
				s1=s1.Mid(xxx+1);
			}
			else
			{
				s2=s1;
				s1="";
			}
			BNo=atoi(s2)-1;		
			if(BNo<0)  continue;
			if(BNo>m_DataNum-1)  continue;
			pBut[BNo]=999.25f;
		}
		
		int lp,rp,n,k=0;	
		float dep=m_StProDep;
		while(k<RecordNum)
		{
			fp.Seek (k*Block,CFile::begin);
			fp.Read (Value,Block);
			//处理异常电扣值				
			for(i=0;i<m_DataNum;i++)
			{
				if(pBut[i]>0.f)
				{
					lp=i;n=0;
					do{
						lp--;
						if(lp<0) 
							lp=m_DataNum+lp;
						n++;
					}while(n<m_DataNum&&pBut[lp]>0.f);
					rp=i;n=0;
					do{
						rp++;
						if(rp>m_DataNum-1) 
							rp=rp%m_DataNum;
						n++;
					}while(n<m_DataNum&&pBut[rp]>0.f);
					if(n<m_DataNum)
						Value[i]=(Value[lp]+Value[rp])/2.f;
				}
			}
			fp.Seek (k*Block,CFile::begin);
			fp.Write (Value,Block);	

			k++;
			dep=m_StProDep+m_Rlev*k;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton())
			{
				bUserBreak=TRUE;
				break;
			}
		}
		delete pGuage;delete pBut;delete Value;
	}
	else bUserBreak=TRUE;
	fp.Close ();
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

BOOL CDataPoisePro::DataFilterPro(CString Path,CString Name)
{	
	BOOL bBreak=FALSE;
	char s[256],fn[256];
	strcpy(s,Path);
	strcat(s,".");strcat(s,Name);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
		float *Value=new float[m_DataNum*RecordNum];
		//读取曲线所有数据
		fp.Seek (0L,CFile::begin);
		fp.Read (Value,m_DataNum*RecordNum*4);

		//纵向平滑滤波处理
		//先处理小给定参数的数据，后3点钟形滤波
		int j,k;
		for(k=0;k<m_DataNum;k++)
		{
			for(j=1;j<RecordNum-1;j++)
			{
				if(Value[m_DataNum*j+k]<m_ButMin)
					Value[m_DataNum*j+k]=(Value[m_DataNum*(j-1)+k]+Value[m_DataNum*(j+1)+k])/2.f;
				Value[m_DataNum*j+k]=0.212f*Value[m_DataNum*(j-1)+k]+0.576f*Value[m_DataNum*j+k]+0.212f*Value[m_DataNum*(j+1)+k];
			}
		}
		//横向平滑滤波
		if(!bBreak)
		{
			for(j=0;j<RecordNum;j++)
			{
				for(k=1;k<m_DataNum-1;k++)
				{
					if(Value[m_DataNum*j+k]<m_ButMin)
						Value[m_DataNum*j+k]=(Value[m_DataNum*j+k-1]+Value[m_DataNum*j+k+1])/2.f;
					Value[m_DataNum*j+k]=0.212f*Value[m_DataNum*j+k-1]+0.576f*Value[m_DataNum*j+k]+0.212f*Value[m_DataNum*j+k+1];
				}
			}
			fp.Seek (0L,CFile::begin);
			fp.Write(Value,m_DataNum*RecordNum*4);
		}
		delete Value;
	}
	else bBreak=TRUE;
	fp.Close ();
	return !bBreak;
}

BOOL CDataPoisePro::MITGeometryCorrect(CString InName,CString OutName)
{
	CFIDIO* pFIDI = new CFIDIO(m_szBufPath+"."+InName);
	if(!pFIDI->Open())
	{
		delete pFIDI;return FALSE;
	}
	FIDCONTENT* Cont1 =pFIDI->GetContent(2);
	FIDCONTENT* Cont2 =pFIDI->GetContent(3);
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetTitle(OutName+" 图像井眼几何校正...");
	pGuage->ShowWindow(SW_SHOW);
	float dep=m_StProDep;;
	BOOL bUserBreak=FALSE;

	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"AZIM","DEG",REPR_SHORT,4,1,0,0.f,360.f,0.f},
							   {"RIMG","mm",REPR_FLOAT,4,60,60,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;
	//横向分辩率0.1英寸=2.54mm
	int Num=(int)(m_Bits*3.1416f/2.54f);
	m_Content[1].nps=Num;                              //图像点数 
	m_Content[1].min=Cont1->min;                  
	m_Content[1].max=Cont1->max; 
	m_Content[1].rlev = m_Bits;                        //钻头直径 mm
	strcpy(m_Content[1].unit,Cont1->unit); 

	m_Content[2].nps=Num;
	m_Content[2].npw=Num;                              
	m_Content[2].min=Cont2->min;                  
	m_Content[2].max=Cont2->max;               
	m_Content[2].rlev =0.f;                            //电扣间距(折算为电扣数)
	strcpy(m_Content[2].unit,Cont2->unit); 
	strcpy(m_Content[2].name,OutName);

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;

	CFIDIO* pFIDW = NULL;
	float *InValue,*OutValue;	
	int i,j,k,m,n;
	pFIDW = new CFIDIO(m_szWellName+"."+OutName,&m_Index);
	if(pFIDW->Open())
	{				
		i=0;
		InValue=new float[m_DataNum+10];
		OutValue=new float[Num+10];
		pFIDI->SeekToDepth(m_StProDep);
		pFIDW->SeekToDepth(m_StProDep);
		float pNum=(float)m_DataNum;
		while(i<RecordNum)
		{
			pFIDI->Read(3,m_DataNum,InValue);
			pFIDI->Next();
			OutValue[0]=InValue[0];
			for(j=1;j<m_DataNum;j++)
			{
				k=(int)(Num*j/m_DataNum);
				m=Num*(j-1)/m_DataNum;
				for(n=m;n<k;n++)
					OutValue[n]=InValue[j-1]+(InValue[j]-InValue[j-1])*(n-m)/(k-m);
			}
			//最后一个点和第一点进行插值处理
			k=Num;
			m=Num*(m_DataNum-1)/m_DataNum;
			for(n=m;n<k;n++)
				OutValue[n]=InValue[m_DataNum-1]+(InValue[0]-InValue[m_DataNum-1])*(n-m)/(k-m);

			OutValue[Num-1]=InValue[m_DataNum-1];
			pFIDW->Write(3,Num,OutValue);
			pFIDW->Next();
			i++;
			dep=m_StProDep+m_Rlev*i;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) 
			{
				bUserBreak=TRUE;
				break;
			}
		}	
		delete InValue;delete OutValue;
	}
	else bUserBreak=TRUE;
	delete pFIDI;delete pFIDW;
	delete pGuage;	
	if(bUserBreak) return FALSE;
	else    	   return TRUE;
}
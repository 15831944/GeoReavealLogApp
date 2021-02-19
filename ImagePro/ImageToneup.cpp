// ImageToneup.cpp: implementation of the CImageToneup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "flowchart.h"
#include "ImageToneup.h"
#include "FlowNode.h"
#include "FlowChartDoc.h"
#include "ImageCreate.h"
#include "ImageToneupDlg.h"
#include "FIDio.h"
#include "Guage.h"
#include "MultiScale.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CImageToneup,CFlowNode, 0)
CImageToneup::CImageToneup()
{
	m_pDoc=NULL;
}

CImageToneup::~CImageToneup()
{

}

CImageToneup::CImageToneup(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
	m_bGrads=FALSE;  
	m_bFilter=FALSE;    
	m_bSquare=FALSE; 
	m_bFreq=FALSE;  
	m_bMusic=TRUE;
	m_MaxPos=298;m_MinVal=3;
	m_bRepair=FALSE;
	m_nLevel = 128;
	m_radius=0.25f;
	m_WinLongth=0.6096f;
	m_RlevLongth=0.03048f;
}

void CImageToneup::Serialize(CArchive& ar)
{
	m_Ver=1.1f;
	CFlowNode::Serialize(ar);	
	if (ar.IsStoring())
	{
		ar<<m_bSquare; 
		ar<<m_bFilter;   
		ar<<m_bGrads;
		ar<<m_bFreq;
		ar<<m_bMusic;
		ar<<m_bRepair;
		ar<<m_nLevel;
		ar<<m_radius;
		ar<<m_WinLongth;
		ar<<m_RlevLongth;
		ar<<m_MaxPos;
		ar<<m_MinVal;
	}
	else
	{
		ar>>m_bSquare; 
		ar>>m_bFilter;   
		ar>>m_bGrads;
		ar>>m_bFreq;
		ar>>m_bMusic;
		ar>>m_bRepair;
		ar>>m_nLevel;
		ar>>m_radius;
		ar>>m_WinLongth;
		ar>>m_RlevLongth;
		if(m_Ver>1.f)
		{
			ar>>m_MaxPos;
			ar>>m_MinVal;
		}
	}
}

void CImageToneup::InitPara(CFlowChartDoc* pDoc)
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
				m_Bits=((CImageCreat*)pObj)->m_Bits; 
			}
		}
	}
	m_pDoc=pDoc;
}

BOOL CImageToneup::ACDataPro()
{	
	CString Name[3],szName[3];
	Name[0]="AMPIMG";szName[0]="幅度图像";
	Name[1]="TTIMG";szName[1]="时间图像";
	Name[2]="RIMG";szName[2]="半径图像";
	for(int i=0;i<3;i++)
	{
		if(m_bFreq)
			AllDataDynFreq(m_szWellName,Name[i]);
		if(m_bPadPro)
			AllDataExtentPro(m_szWellName,Name[i]);
		if(m_bSquare)
			AllDataDynSquare(m_szWellName,Name[i]);
	}
	return TRUE;
}

BOOL CImageToneup::Run()
{		
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	if(!m_pDoc->m_bAunAll)
	{
		CImageToneupDlg dlg(this);
		dlg.m_bGrads=m_bGrads;	
		dlg.m_bFreq=m_bFreq;
		dlg.m_bFilter=m_bFilter;
		dlg.m_Message=m_szSeries;
		dlg.m_bMusic=m_bMusic;
		dlg.m_MaxPos=m_MaxPos;
		dlg.m_MinVal=m_MinVal;
		dlg.m_bRepair=m_bRepair;
		dlg.m_nLevel=m_nLevel;
		dlg.m_radius=m_radius;	
		dlg.m_WinLongth=m_WinLongth;
		dlg.m_RlevLongth=m_RlevLongth;
		if(dlg.DoModal()!=IDOK)
			return FALSE;
	}
	m_bResult=TRUE;
	//声成像处理
	if((m_szSeries=="CBIL")||(m_szSeries=="CAST"))
	{
		ACDataPro();
		return m_bResult;
	}
	m_bResult=DataCopy("DATAST","DATADY");

	//分区频率直方图增强
	if(m_bResult&&m_bGrads&&m_szSeries!="ARI_16")
		m_bResult=PadDataDynFreq(m_szBufPath,"DATADY");
	//小波技术增强
	if(m_bResult&&m_bMusic)
		m_bResult=AllMultiScale(m_szBufPath,"DATADY");
	//井周频率直方图增强
	if(m_bResult&&m_bFreq)
		m_bResult=AllDataDynFreq(m_szBufPath,"DATADY");
	////全井壁调整刻度
	/*if(m_bResult)
		AllDataExtentPro(m_szBufPath,"DATADY");*/
	if(m_bResult&&m_bFilter)
		ImageFilterPro(m_szBufPath,"DATADY");
	//图像修复
	if(m_bResult&&m_bRepair)
		DataRepair(m_szBufPath,"DATADY");
	
	return m_bResult;
}

//动态刻度图像数据
//分窗长对各极进行直方图均衡
BOOL CImageToneup::PadDataDynFreq(CString Path,CString Name)
{
	int iWin=(int)(m_WinLongth/m_Rlev);
	if(iWin<10) iWin=10; 
	int iRlev=(int)(m_RlevLongth/m_Rlev);
	if(iRlev<1) iRlev=1;
	if(iRlev>iWin) iRlev=iWin/2;

	int Block=m_DataNum*4;
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev)-iWin;
	char s[256],fn[256];
	strcpy(s,Path);
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
		pGuage->SetTitle("分极板频率直方图增强...");
		float dep=m_StProDep;	
		float *fPad=new float[m_PadNum];
		float *Value=new float[m_DataNum*iWin+10];
		int i,j,k=0;

		int NNN,y,pos,Maxfq;
		int freq[301];
		float Min[10],Max[10],xMin,xMax,xxx;
		float MinP[2][10],MaxP[2][10];
		for(i=0;i<10;i++)
		{
			MinP[0][i]=0.f;
			MaxP[0][i]=1024.f;
			MinP[1][i]=0.f;
			MaxP[1][i]=1024.f;
			Min[i]=0.f;
			Max[i]=1024.f;
		}
		while(k<RecordNum)
		{
			fp.Seek (k*Block,CFile::begin);
			fp.Read (Value,Block*iWin);
			for(j=0;j<m_PadNum;j++)
			{
				//计算窗口内各区间频率点数
				for(i=0;i<300;i++)
					freq[i]=0;
				for(y=0;y<iWin;y++)
					for(i=0;i<m_ButtonNum;i++)
					{
						pos=(int)(Value[y*m_DataNum+j*m_ButtonNum+i]/1024.f*300.f);
						if(pos>-1&&pos<300)
							freq[pos]++;
					}
				
				//寻找最大频率作为参数检测标准
				Maxfq=1;
				for(i=0;i<300;i++)
					if(freq[i]>Maxfq)
						Maxfq=freq[i];
				for(i=0;i<300;i++)
					freq[i]=freq[i]*1000/Maxfq;
				//寻找窗口范围内的频率最大位置的值
				NNN=0;pos=0;
				for(i=0;i<300;i++)
					if(freq[i]>NNN)
					{
						NNN=freq[i];
						pos=i;
					}
				xMax=0.f;xMin=0.f;
				//寻找最大值
				for(i=pos;i<300;i++)
					if(freq[i]>m_MinVal)
						xMax=i*1024.f/300.f;
				//寻找最小值
				for(i=pos;i>0;i--)
					if(freq[i]>m_MinVal)
						xMin=i*1024.f/300.f;

				if(xMin>0.f&&xMax>0.f&&xMax>xMin)
				{
					//三点钟形滤波         0.212Ti-1+0.576Ti+0.212Ti+1
					Min[j]=MinP[0][j]*0.212f+xMin*0.576f+MinP[1][j]*0.212f;
					Max[j]=MaxP[0][j]*0.212f+xMax*0.576f+MaxP[1][j]*0.212f;
					xxx=Max[j]-Min[j];
					for(y=0;y<iRlev;y++)
					{
						for(i=0;i<m_ButtonNum;i++)
							Value[y*m_DataNum+j*m_ButtonNum+i]=
								(Value[y*m_DataNum+j*m_ButtonNum+i]-Min[j])/xxx*1024.f;
					}
					MinP[0][j]=MinP[1][j];
					MaxP[0][j]=MaxP[1][j];
					MinP[1][j]=Min[j];
					MaxP[1][j]=Max[j];
				}
				else
				{
					xxx=Max[j]-Min[j];
					for(y=0;y<iRlev;y++)
					{
						for(i=0;i<m_ButtonNum;i++)
							Value[y*m_DataNum+j*m_ButtonNum+i]=
								(Value[y*m_DataNum+j*m_ButtonNum+i]-Min[j])/xxx*1024.f;
					}
				}
			}
			fp.Seek (k*Block,CFile::begin);
			fp.Write (Value,Block*iRlev);				
			k+=iRlev;
			dep=m_StProDep+m_Rlev*k;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton())
			{
				bUserBreak=TRUE;
				break;
			}
		}
		//处理最后一个窗口内的数据增强///////////////////////////////////////////////////////////
		for(j=0;j<m_PadNum;j++)
		{
			//计算窗口内各区间频率点数
			for(i=0;i<300;i++)
				freq[i]=0;
			for(y=0;y<iWin;y++)
				for(i=0;i<m_ButtonNum;i++)
				{
					pos=(int)(Value[y*m_DataNum+j*m_ButtonNum+i]/1024.f*300.f);
					if(pos>-1&&pos<300)
						freq[pos]++;
				}
			//寻找窗口范围内的频率最大位置的值
			NNN=0;pos=0;
			for(i=0;i<300;i++)
				if(freq[i]>NNN)
				{
					NNN=freq[i];
					pos=i;
				}
			//计算频率千分数
			for(i=0;i<300;i++)
				freq[i]=freq[i]*3000/(m_ButtonNum*iWin);
			xMax=0.f;xMin=0.f;
			//寻找最大值
			for(i=pos;i<300;i++)
				if(freq[i]>m_MinVal)
					xMax=i*1024.f/300.f;
			//寻找最小值
			for(i=pos;i>0;i--)
				if(freq[i]>m_MinVal)
					xMin=i*1024.f/300.f;
			if(xMin>0.f&&xMax>0.f&&xMax>xMin)
			{
				//三点钟形滤波         0.212Ti-1+0.576Ti+0.212Ti+1
				Min[j]=MinP[0][j]*0.212f+xMin*0.576f+MinP[1][j]*0.212f;
				Max[j]=MaxP[0][j]*0.212f+xMax*0.576f+MaxP[1][j]*0.212f;
				xxx=Max[j]-Min[j];
				for(y=0;y<iWin;y++)
					for(i=0;i<m_ButtonNum;i++)
						Value[y*m_DataNum+j*m_ButtonNum+i]=(Value[y*m_DataNum+j*m_ButtonNum+i]-Min[j])/xxx*1024.f;
				MinP[0][j]=MinP[1][j];
				MaxP[0][j]=MaxP[1][j];
				MinP[1][j]=Min[j];
				MaxP[1][j]=Max[j];
			}
			else
			{
				xxx=Max[j]-Min[j];
				for(y=0;y<iWin;y++)
					for(i=0;i<m_ButtonNum;i++)
						Value[y*m_DataNum+j*m_ButtonNum+i]=(Value[y*m_DataNum+j*m_ButtonNum+i]-Min[j])/xxx*1024.f;

			}
		}
		fp.Seek (k*Block,CFile::begin);
		fp.Write (Value,Block*(iWin-2));	
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		delete pGuage;
		delete Value;delete fPad;
	}
	else bUserBreak=TRUE;
	fp.Close ();
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

//动态刻度图像数据
//分窗长进行直方图均衡
BOOL CImageToneup::AllDataDynFreq(CString Path,CString Name)
{
	int RecordNum;
	char s[256],fn[256];
	int Block=m_DataNum*4;
	RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	BOOL bUserBreak=FALSE;
	strcpy(s,Path);
	strcat(s,".");strcat(s,Name);
	int iWin=(int)(m_WinLongth/m_Rlev);
	if(iWin<10) iWin=10; 
	int iRlev=(int)(m_RlevLongth/m_Rlev);
	if(iRlev<1) iRlev=1;
	if(iRlev>iWin) iRlev=iWin/2;

	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;int freq[301];
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		//定义进度指示器
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->ShowWindow(SW_SHOW);
		CString ss="井周频率直方图增强...";
		pGuage->SetTitle(ss);
		float dep=m_StProDep;

		float *Value,*fwf;
		Value=new float[m_DataNum*iWin];
		fwf=new float[m_DataNum];
		int i,k,NNN,y,pos;
		float Min,Max,Min0,Max0,Min1,Max1,xMin,xMax,xxx;
		k=0;
		Min0=0.f;Max0=1024.f;
		Min1=0.f;Max1=1024.f;
		while(k<RecordNum-iWin)
		{			
			fp.Seek (k*Block,CFile::begin);
			fp.Read (Value,Block*iWin);
			//计算窗口内各区间频率点数
			for(i=0;i<300;i++)
				freq[i]=0;
			for(i=0;i<m_DataNum*iWin;i++)
			{
				pos=(int)(Value[i]/1024.f*300.f);
				if(pos>-1&&pos<300)
					freq[pos]++;
			}
			//计算频率千分数
			for(i=0;i<300;i++)
				freq[i]=freq[i]*3000/(m_DataNum*iWin);
			//寻找窗口范围内的频率最大位置的值
			NNN=0;pos=0;
			for(i=0;i<300;i++)
			{
				if(freq[i]>NNN)
				{
					NNN=freq[i];
					pos=i;
				}
			}
			xMax=0.f;xMin=0.f;
			for(i=pos;i<300;i++)//寻找最大值
				if(freq[i]>m_MinVal)
					xMax=i*1024.f/300.f;
			
			for(i=pos;i>0;i--)//寻找最小值
				if(freq[i]>m_MinVal)
					xMin=i*1024.f/300.f;
				
			if(xMin>0.f&&xMax>0.f&&xMax>xMin)
			{
				Min=xMin;
				Max=xMax;
				//三点钟形滤波         0.212Ti-1+0.576Ti+0.212Ti+1
				Min=Min0*0.212f+Min*0.576f+Min1*0.212f;
				Max=Max0*0.212f+Max*0.576f+Max1*0.212f;
				xxx=Max-Min;
				for(y=0;y<iRlev;y++)
					for(i=0;i<m_DataNum;i++)
						Value[y*m_DataNum+i]=(Value[y*m_DataNum+i]-Min)/xxx*1024.f;
				Min1=Min0;Max1=Max0;
				Min0=Min;Max0=Max;
			}
			else
			{	
				for(y=0;y<iRlev;y++)
					for(i=0;i<m_DataNum;i++)
						Value[y*m_DataNum+i]=(Value[y*m_DataNum+i]-Min)/xxx*1024.f;
			}
			fp.Seek (k*Block,CFile::begin);
			fp.Write (Value,Block*iRlev);
			k+=iRlev;
			dep=m_StProDep+m_Rlev*k;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) 
			{
				bUserBreak=TRUE;
				break;
			}
		}
		delete Value;delete fwf;
		delete pGuage;
	}
	else bUserBreak=TRUE;
	fp.Close ();
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

//平方指数增强
BOOL CImageToneup::AllDataDynSquare(CString Path,CString Name)
{
	int RecordNum;
	char s[256],fn[256];
	int Block=m_DataNum*4;
	RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	BOOL bUserBreak=FALSE;
	strcpy(s,Path);
	strcat(s,".");strcat(s,Name);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		//定义进度指示器
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->ShowWindow(SW_SHOW);
		CString ss="动态图像指数增强...";
		pGuage->SetTitle(ss);
		float dep=m_StProDep;

		float *Value;
		Value=new float[m_DataNum+10];
		int i,k;

		k=0;
		while(k<RecordNum)
		{
			fp.Seek (k*Block,CFile::begin);
			fp.Read (Value,Block);
				
			for(i=0;i<m_DataNum;i++)
				Value[i]*=Value[i];
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

//小波技术图像动态增强
BOOL CImageToneup::AllMultiScale(CString Path,CString Name)
{
	int RecordNum;
	RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	BOOL bUserBreak=FALSE;
	char s[256],fn[256];
	strcpy(s,Path);
	strcat(s,".");strcat(s,Name);
	int iWin=(int)(0.6096/m_Rlev);
	if(iWin<10) iWin=10; 
	int iRlev=iWin/3;

	int Block=m_DataNum*4;
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		//定义进度指示器
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->ShowWindow(SW_SHOW);
		pGuage->SetTitle("小波技术图像动态增强...");

		float *Value=new float[m_DataNum*iWin];
		float *fwf=new float[m_DataNum*iRlev];
		int i,j,k;
		
		//分解级数
		//各级长宽
		int Width=m_DataNum+10;
		int* pnMatrixInfo = new int[(1+m_nLevel)*5];
		int totleLength=0;
		pnMatrixInfo[0] = m_nLevel;
		pnMatrixInfo[1]= Width;	//w
		pnMatrixInfo[2]= iWin;//h
		pnMatrixInfo[3]= Width;	//Stride
		totleLength = iWin*Width;
		for(i=1;i<m_nLevel+1;i++)
		{
			pnMatrixInfo[1+i*3]= (pnMatrixInfo[1+i*3-3]+1)/2 ;//w
			pnMatrixInfo[2+i*3]= (pnMatrixInfo[2+i*3-3]+1)/2 ;//h
			pnMatrixInfo[3+i*3]=  pnMatrixInfo[1+i*3] ;//Stride
			totleLength += pnMatrixInfo[2+i*3]*pnMatrixInfo[3+i*3];
		}
		//灰度内存块
		BYTE* pPixelsGrey = new BYTE[pnMatrixInfo[2]*pnMatrixInfo[3]];
		BYTE* pDecomposition = new BYTE[totleLength];

		float data,dep;
		k=0;
		fp.Seek (k*Block,CFile::begin);
		fp.Read (Value,Block*iWin);
		k=0;
		while(k<RecordNum-iRlev)
		{	
			//处理左右边界
			for(i=0;i<iWin;i++)	
			{
				for(j=0;j<5;j++)
				{
					//左边5个数据
					data=Value[m_DataNum*i+j+m_DataNum-6]/6.5f;
					if(data>255.f) data=255.f;
					if(data<0.f) data=0.f;
					pPixelsGrey[Width*i+j]=(BYTE)(data);
					//右边5个数据
					data=Value[m_DataNum*i+j]/6.5f;
					if(data>255.f) data=255.f;
					if(data<0.f) data=0.f;
					pPixelsGrey[Width*i+j+Width-6]=(BYTE)(data);
				}
			}
			for(i=0;i<iWin;i++)
				for(j=0;j<m_DataNum;j++)
				{
					data=Value[m_DataNum*i+j]/6.5f;
					if(data>255.f) data=255.f;
					if(data<0.f) data=0.f;
					pPixelsGrey[Width*i+j+5]=(BYTE)(data);
				}
			MUSICA_Decomposition(pPixelsGrey,pDecomposition,pnMatrixInfo);	//分解
			MUSICA_Mapping(pDecomposition,pnMatrixInfo);//变换
			MUSICA_Reconstruction(pPixelsGrey,pDecomposition,pnMatrixInfo);//重构
			
			//处理前后边界
			//取滑动窗口内中间1/3的数据存盘
			for(i=0;i<iRlev;i++)
				for(j=0;j<m_DataNum;j++)
					fwf[m_DataNum*i+j]=pPixelsGrey[Width*i+j+Width*iRlev+5]*6.5f;
			fp.Seek (k*Block,CFile::begin);
			fp.Write(fwf,Block*iRlev);

			for(j=m_DataNum*iRlev;j<m_DataNum*iWin;j++)
				Value[j-m_DataNum*iRlev]=Value[j];
			k+=iRlev;
			dep=m_StProDep+m_Rlev*k;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) 
			{
				bUserBreak=TRUE;
				break;
			}
			
			fp.Seek (k*Block,CFile::begin);
			fp.Read(fwf,Block*iRlev);
			for(j=0;j<m_DataNum*iRlev;j++)
				Value[j+m_DataNum*(iWin-iRlev)]=fwf[j];
		}
		//处理不够一个滑动窗口的剩余数据
		if(k<RecordNum)
		{
			int iWin0=RecordNum-k;
			fp.Seek (k*Block,CFile::begin);
			fp.Read (fwf,Block*iWin0);
			for(j=m_DataNum*iRlev;j<m_DataNum*iWin0;j++)
				Value[j+m_DataNum*iRlev]=fwf[j];
			iWin0+=iRlev;
			for(j=iWin0*m_DataNum;j<iWin*m_DataNum;j++)
				Value[j]=0.f;
			//处理左右边界
			for(i=0;i<iWin;i++)	
			{
				for(j=0;j<5;j++)
				{
					//左边5个数据
					data=Value[m_DataNum*i+j+m_DataNum-6]/6.5f;
					if(data>255.f) data=255.f;
					if(data<0.f) data=0.f;
					pPixelsGrey[Width*i+j]=(BYTE)(data);
					//右边5个数据
					data=Value[m_DataNum*i+j]/6.5f;
					if(data>255.f) data=255.f;
					if(data<0.f) data=0.f;
					pPixelsGrey[Width*i+j+Width-6]=(BYTE)(data);
				}
			}
			for(i=0;i<iWin;i++)
				for(j=0;j<m_DataNum;j++)
				{
					data=Value[m_DataNum*i+j]/6.5f;
					if(data>255.f) data=255.f;
					if(data<0.f) data=0.f;
					pPixelsGrey[Width*i+j+5]=(BYTE)(data);
				}
			MUSICA_Decomposition(pPixelsGrey,pDecomposition,pnMatrixInfo);	//分解
			MUSICA_Mapping(pDecomposition,pnMatrixInfo);//变换
			MUSICA_Reconstruction(pPixelsGrey,pDecomposition,pnMatrixInfo);//重构
			
			//处理前后边界
			//取滑动窗口内中间1/3的数据存盘
			for(i=0;i<iWin0;i++)
				for(j=0;j<m_DataNum;j++)
					Value[m_DataNum*i+j]=pPixelsGrey[Width*i+j+Width*iRlev+5]*6.5f;
			
			fp.Seek (k*Block,CFile::begin);
			fp.Write(Value,Block*iWin0);
		}
		delete pPixelsGrey;
		delete pDecomposition;
		delete pnMatrixInfo;
		delete fwf;
		
		//将数据向上移动iRlev*m_Rlev米
		k=0;
		pGuage->SetTitle("数据整理...");
		while(k<RecordNum-iRlev)
		{	
			fp.Seek((k+iRlev)*Block,CFile::begin);
			fp.Read(Value,Block);	

			fp.Seek (k*Block,CFile::begin);
			fp.Write(Value,Block);
			k++;
			dep=m_StProDep+m_Rlev*k;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton()) 
			{
				bUserBreak=TRUE;
				break;
			}
		}
		delete Value;
		fp.Close ();
		delete pGuage;
	}
	else bUserBreak=TRUE;
	
	return !bUserBreak;
}
// DiffrenceCp.cpp: implementation of the CDiffrenceCp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "flowchart.h"
#include "DiffrenceCp.h"
#include "FlowNode.h"
#include "Gradient.h"
#include "FIDio.h"
#include "Guage.h"
#include "DiffrenceDlg.h"
#include "ImageGeoCorrect.h"

#include "Insert3D.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CDiffrenceCp,CFlowNode, 0)
CDiffrenceCp::CDiffrenceCp()
{
	m_pDoc=NULL;
}

CDiffrenceCp::~CDiffrenceCp()
{

}

CDiffrenceCp::CDiffrenceCp(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
	m_DaltaDep=0.0508f;
	m_szImageType="静态图像";
}

void CDiffrenceCp::Serialize(CArchive& ar)
{
	CFlowNode::Serialize(ar);	
	if(ar.IsStoring())
	{
		ar<<m_szImageType;
		ar<<m_DaltaDep;
	}
	else
	{
		ar>>m_szImageType;
		ar>>m_DaltaDep;
	}
}

//井眼图像三维重建后4个方位垂直切片
BOOL CDiffrenceCp::CreateSectionImage(CString InName,CString OutName,CString SectionName)
{
	int DataNum=0;
	CFIDIO* PadI=new CFIDIO(m_szWellName+"."+InName);
	if(PadI->Open())
	{
		m_StProDep=PadI->GetStartDepth();
		m_EnProDep=PadI->GetEndDepth();
		m_Rlev=PadI->GetDepthLevel();

		FIDCONTENT *Content = PadI->GetContent(3);
		if(Content != NULL)
			DataNum=Content->nps;
	}
	delete PadI;
	if(DataNum<1) 
	{
		AfxMessageBox(InName+"打开曲线创建失败!        ");
		return FALSE;
	}
	
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetTitle(InName+" 井眼图像三维重建...");
	pGuage->ShowWindow(SW_SHOW);
     
	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"PAD","none",REPR_SHORT,4,16,0,0.f,0.f,0.f},
							   {"DATAST","1/ohm",REPR_FLOAT,4,16,16,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;
	m_Content[1].nps=DataNum;                      
	m_Content[1].min=2.54f;                            
	m_Content[1].max=m_Bits*3.1416f;                   
	m_Content[1].rlev = m_Bits;                        

	m_Content[2].nps=DataNum;
	m_Content[2].npw=DataNum;                        
	m_Content[2].min=(float)m_PadNum;                  
	m_Content[2].max=(float)m_ButtonNum;               
	m_Content[2].rlev =0.f;                            

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;
	//初始化4条垂直切片图像
	double Radius=DataNum/2/PI;
	int Num=(int)(Radius*2);
	
	m_Content[1].nps=Num; 
	m_Content[2].nps=Num;
	m_Content[2].npw=Num; 
	CString szStr;
	int i,j,k=0;

	CFIDIO *pFIDW=NULL;
	for(i=0;i<4;i++)
	{
		strcpy(m_Content[2].name,SectionName);
		szStr.Format("%d",i*45);
		strcat(m_Content[2].name,szStr);
		szStr=m_szWellName+"."+SectionName+szStr;
		pFIDW = new CFIDIO(szStr,&m_Index);
		if(!pFIDW->Open())
		{	
			AfxMessageBox(szStr+" 初始化垂直切片图像失败!        ");
			delete pFIDW;
			delete pGuage;
			return FALSE;
		}
		pFIDW->Close();
		delete pFIDW;pFIDW=NULL;
	}
	char s[256],fn[256];
	strcpy(s,m_szWellName);
	strcat(s,".");strcat(s,InName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(!fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		AfxMessageBox(InName+" 打开曲线失败!        ");
		delete pGuage;
		return FALSE;
	}

	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	float *Value=new float[DataNum*RecordNum];			//图像原始数据
	//读取曲线所有数据
	fp.Seek (0L,CFile::begin);
	fp.Read (Value,DataNum*RecordNum*4);
	fp.Close ();	

	CFile *pFile=new CFile[4];
	for(i=0;i<4;i++)
	{
		szStr.Format("%d",i*45);
		szStr=m_szWellName+"."+SectionName+szStr;
		strcpy(s,szStr);
		CFIDIO::GetFileName(s,fn,'D');
		if(!pFile[i].Open(fn,CFile::modeReadWrite|CFile::typeBinary))
		{
			delete pFile;
			delete Value;
			delete pGuage;
			return TRUE;
		}
	}
	//反间距三维插值
	//XY方向为井周平面XY值
	//Z方向取ndy*2个深度采样，深度点位上取ndy个采样，下取ndy个采样
	int ndy=5;
	double x,y,z;
	vector<Point3D> input;
	Insert3D* pInterpolater = new Insert3D(DataNum,6);
	float **pData=new float* [Num];//垂直切片数据
	for(i=0;i<4;i++)
		pData[i]=new float[Num];
	
	for(j=ndy;j<RecordNum-ndy;j++)
	{
		input.clear();
		for(k=0;k<ndy*2;k++)
		{
			for(i=0;i<DataNum;i++)
			{
				if(Value[DataNum*(j+k-ndy)+i]!=-9999.f)
				{
					x=Radius*sin((i*2*PI)/DataNum);
					y=Radius*cos((i*2*PI)/DataNum);
					z=k-ndy;
					input.push_back(Point3D(x,y,z,Value[DataNum*(j+k-ndy)+i]));
				}
			}
		}
		z=0;
		for(k=0;k<4;k++)
		{
			for(i=0;i<Num;i++)
			{
				x=(Num/2-i)*sin(k*PI/4);
				y=(Num/2-i)*cos(k*PI/4);
				pData[k][i]=pInterpolater->GetInsertV(x,y,z,(Point3D*)&(*input.begin()),(Point3D*)&(*input.end()));
			}
			pFile[k].Write(pData[k],Num*4);
		}
		pGuage->SetDepth(m_StProDep+j*m_Rlev);
		if(pGuage->CheckCancelButton())
			break;
	}
	
	delete pInterpolater;
	for(i=0;i<4;i++)
		delete pData[i];
	delete pData;

	for(i=0;i<4;i++)
		pFile[i].Close ();
	delete pFile;
	delete Value;
	delete pGuage;
	return TRUE;
}

BOOL CDiffrenceCp::Run()
{
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	if(!m_pDoc->m_bAunAll)
	{
		CDiffrenceDlg dlg;
		dlg.m_szStr="静态图像,静态恢复图像,动态图像,动态恢复图像";
		dlg.m_szDifName=m_szImageType;
		dlg.m_DaltaDep=m_DaltaDep;
		if(dlg.DoModal()!=IDOK)
			return FALSE;
		m_szImageType=dlg.m_szDifName;
		m_DaltaDep=dlg.m_DaltaDep;
	}

	CString InName,OutName,SectionName;
	if(m_szImageType=="静态图像")
	{
		InName="STAMI";
		OutName="STA3D";
		SectionName="STACT";
	}
	if(m_szImageType=="静态恢复图像")
	{
		InName="STAPIC";
		OutName="STA3D";
		SectionName="STACT";
	}
	else if(m_szImageType=="动态恢复图像")
	{
		InName="DYNPIC";
		OutName="DYN3D";
		SectionName="DYNCT";
	}
	else
	{
		InName="DYNMI";
		OutName="DYN3D";
		SectionName="DYNCT";
	}
	m_bResult=CreateSectionImage(InName,OutName,SectionName);
	if(m_bResult)
		OnWellRefresh();
	return m_bResult;

	/*else
	{
		if(m_szSeries=="ARI_16")
			InName="ARIGPT";
		else if(m_szSeries=="ARI_16")
			InName="RADSGPT";
		else
			InName="DATAST";
	}*/
/************************************************************************/
/* 东北方向边缘检测                                                     */
/************************************************************************/
	//模板高度宽度及中心像素的坐标
	int iTempH,iTempW,iTempCX,iTempCY;
	//模板元素系数及元素数组
	float Temp[9];
	//初始化对话框变量
/*	iTempH=3;
	iTempW=3;
	iTempCX=1;
	iTempCY=1;
	
	//设模板为平均模板
	Temp[0]=1.0;
	Temp[1]=1.0;
	Temp[2]=1.0;
	Temp[3]=-1.0;
	Temp[4]=-2.0;
	Temp[5]=1.0;
	Temp[6]=-1.0;
	Temp[7]=-1.0;
	Temp[8]=1.0;*/
/************************************************************************/
/* 水平垂直方向边缘检测                                                 */
/************************************************************************/
	//设模板为平均模板
/*	Temp[0]=-1.0;
	Temp[1]=0.0;
	Temp[2]=0.0;
	Temp[3]=0.0;
	Temp[4]=1.0;
	Temp[5]=0.0;
	Temp[6]=0.0;
	Temp[7]=0.0;
	Temp[8]=0.0;
	
	//初始化对话框变量
	iTempH=3;
	iTempW=3;
	iTempCX=1;
	iTempCY=1;
	fCoef=0.5;*/
/************************************************************************/
/* 拉普拉斯锐化                                                         */
/************************************************************************/
	//设模板为平均模板
/*	Temp[0]=0.0;
	Temp[1]=-1.0;
	Temp[2]=0.0;
	Temp[3]=-1.0;
	Temp[4]=5.0;
	Temp[5]=-1.0;
	Temp[6]=0.0;
	Temp[7]=-1.0;
	Temp[8]=0.0;
	
	//初始化对话框变量
	iTempH=3;
	iTempW=3;
	iTempCX=1;
	iTempCY=1;
	fCoef=1.0;*/
/************************************************************************/
/* Sobel边缘检测算子对图像进行边缘检测                                  */
/************************************************************************/
	//设置Sobel模板参数
	iTempW=3;
	iTempH=3;
	iTempCX=1;
	iTempCY=1;

	Temp[0]=-1.0;
	Temp[1]=-2.0;
	Temp[2]=-1.0;
	Temp[3]=0.0;
	Temp[4]=0.0;
	Temp[5]=0.0;
	Temp[6]=1.0;
	Temp[7]=2.0;
	Temp[8]=1.0;
	
	//m_bResult=Template(InName,OutName,iTempH,iTempW,iTempCX,iTempCY,Temp);
	//设置Sobel模板参数
	Temp[0]=-1.0;
	Temp[1]=0.0;
	Temp[2]=1.0;
	Temp[3]=-2.0;
	Temp[4]=0.0;
	Temp[5]=2.0;
	Temp[6]=-1.0;
	Temp[7]=0.0;
	Temp[8]=1.0;
/************************************************************************/
/* Prewitt边缘检测算子对图像进行边缘检测                                */
/************************************************************************/
	Temp[0]=-1.0;
	Temp[1]=0.0;
	Temp[2]=1.0;
	Temp[3]=-1.0;
	Temp[4]=0.0;
	Temp[5]=1.0;
	Temp[6]=-1.0;
	Temp[7]=0.0;
	Temp[8]=1.0;
//	m_bResult=Template(InName,OutName,iTempH,iTempW,iTempCX,iTempCY,Temp);
/************************************************************************/
/* 拉普拉斯边缘检测                                                     */
/************************************************************************/
	//模板高度宽度及中心像素的坐标
/*	int iTempH,iTempW,iTempCX,iTempCY;
	//模板元素系数及元素数组
	FLOAT Temp[25];

	//初始化对话框变量
	iTempH=5;
	iTempW=5;
	iTempCX=3;
	iTempCY=3;
	
	//设模板为平均模板
	Temp[0]=-2.0;
	Temp[1]=-4.0;
	Temp[2]=-4.0;
	Temp[3]=-4.0;
	Temp[4]=-2.0;
	Temp[5]=-4.0;
	Temp[6]=0.0;
	Temp[7]=8.0;
	Temp[8]=0.0;
	Temp[9]=-4.0;
	Temp[10]=-4.0;
	Temp[11]=8.0;
	Temp[12]=24.0;
	Temp[13]=8.0;
	Temp[14]=-4.0;
	Temp[15]=-4.0;
	Temp[16]=0.0;
	Temp[17]=8.0;
	Temp[18]=0.0;
	Temp[19]=-4.0;
	Temp[20]=-2.0;
	Temp[21]=-4.0;
	Temp[22]=-4.0;
	Temp[23]=-4.0;
	Temp[23]=-2.0;
*/
//	m_bResult=Template(InName,OutName,iTempH,iTempW,iTempCX,iTempCY,Temp);
/************************************************************************/
/* 中值滤波                                                             */
/************************************************************************/	
	//初始化对话框变量
/*	iTempH=5;
	iTempW=5;
	iTempCX=2;
	iTempCY=2;
	m_bResult=MedianFilter(InName,"MEDIAN",iTempH,iTempW,iTempCX,iTempCY);*/
/************************************************************************/
/* 图像平滑                                                             */
/************************************************************************/	
	//设模板为平均模板
/*	Temp[0]=1.0;
	Temp[1]=2.0;
	Temp[2]=1.0;
	Temp[3]=2.0;
	Temp[4]=4.0;
	Temp[5]=2.0;
	Temp[6]=1.0;
	Temp[7]=2.0;
	Temp[8]=1.0;

	//初始化对话框变量
	iTempH=3;
	iTempW=3;
	iTempCX=1;
	iTempCY=1;
	m_bResult=Template("MEDIAN",OutName,iTempH,iTempW,iTempCX,iTempCY,Temp);*/
/************************************************************************/
/* 探测法边界跟踪                                                       */
/************************************************************************/	
//	m_bResult=TraceEdge(InName,OutName);
/************************************************************************/
/* k-均值聚类法图像分割                                                 */
/************************************************************************/	
	m_bResult=KAveIncise(InName,OutName,4);
//	m_bResult=DiffrencePro(InName,OutName);
	CImageGeoCorrect Edge("微分图像几何校正",FALSE);
	Edge.m_szImageType ="微分图像";
	Edge.InitPara(m_pDoc);
	Edge.m_szInName="DIFF";
	Edge.m_szOutName="DIFIMG";
	Edge.Run();
	return m_bResult;
}

void CDiffrenceCp::InitPara(CFlowChartDoc* pDoc)
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

BOOL CDiffrenceCp::DiffrencePro(CString InName,CString OutName)
{
	int m=(int)(m_DaltaDep/m_Rlev);
	if(m<1) return FALSE;
	CFIDIO* pFIDI = new CFIDIO(m_szBufPath+"."+InName);
	if(!pFIDI->Open())
	{
		delete pFIDI;return FALSE;
	}
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetTitle(m_szDifName+"微分处理...");
	pGuage->ShowWindow(SW_SHOW);
	float dep=m_StProDep;;
	BOOL bUserBreak=FALSE;

	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"PAD","none",REPR_SHORT,4,16,0,0.f,0.f,0.f},
							   {"DATAST","1/ohm",REPR_FLOAT,4,16,16,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;
	m_Content[1].nps=m_DataNum;                        
	m_Content[1].min=2.54f;                            
	m_Content[1].max=m_Bits*3.1416f;                   
	m_Content[1].rlev = m_Bits;                        

	m_Content[2].nps=m_DataNum;
	m_Content[2].npw=m_DataNum;                        
	m_Content[2].min=(float)m_PadNum;                  
	m_Content[2].max=(float)m_ButtonNum;               
	m_Content[2].rlev =0.f;                            
	strcpy(m_Content[2].name,OutName);

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;

	CFIDIO* pFIDW = NULL;
	int i,j,k;
	float ScaleMin=32767.f;
	float ScaleMax=-32767;
	pFIDW = new CFIDIO(m_szBufPath+"."+OutName,&m_Index);
	if(pFIDW->Open())
	{				
		i=0;
		float *Value=NULL;	
		float *Value0=NULL;
		Value=new float[m_DataNum+10];
		Value0=new float[m_DataNum*m];
		pFIDI->SeekToDepth(m_StProDep);
		pFIDW->SeekToDepth(m_StProDep);
		pFIDI->Read(3,m_DataNum*m,Value0);
		pFIDI->SeekToDepth(m_StProDep+m_Rlev*m);
		i=m;
		float fx,fy;
		while(i<RecordNum)
		{
			pFIDI->Read(3,m_DataNum,Value);
			pFIDI->Next();
			for(j=0;j<m_DataNum-m;j++)
			{
				fy=Value[j]-Value0[j];
				fx=Value0[j+m]-Value0[j];
				Value0[j]=(fx+fy)/2.f;
			}
			pFIDW->Write(3,m_DataNum,Value0);
			pFIDW->Next();
			i++;
			dep=m_StProDep+m_Rlev*i;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton())
			{
				bUserBreak=TRUE;
				break;
			}
			for(k=0;k<m-1;k++)
				for(j=0;j<m_DataNum;j++)
					Value0[k*m_DataNum+j]=Value0[(k+1)*m_DataNum+j];
			for(j=0;j<m_DataNum;j++)
				Value0[(m-1)*m_DataNum+j]=Value[j];
		}
		delete Value;delete Value0;
	}
	else bUserBreak=TRUE;
	delete pFIDI;delete pFIDW;

	pGuage->SetTitle("微分图像标准刻度...");
	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");strcat(s,OutName);
	CFIDIO::GetFileName(s,fn,'D');
	
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		int Block=m_DataNum*4;
		float *fVal=NULL;
		fVal=new float[m_DataNum+10];
		i=0;
		while(i<RecordNum)
		{
			fp.Seek (i*Block,CFile::begin);
			fp.Read (fVal,Block);
			for(j=0;j<m_DataNum;j++)
				fVal[j]=(fVal[j]-ScaleMin)/(ScaleMax-ScaleMin)*1024.f;
			fp.Seek (i*Block,CFile::begin);
			fp.Write (fVal,Block);				
			i++;
			dep=m_StProDep+m_Rlev*i;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton())
			{
				bUserBreak=TRUE;
				break;
			}
		}
		delete fVal;
	}
	else bUserBreak=TRUE;
	fp.Close ();
	delete pGuage;	
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

/************************************************************************/
/*Template(LPSTR lpDIB, int iTempH, int iTempW, int iTempCX, int iTempCY, FLOAT *fpTempArray, FLOAT fCoef)
LPSTR lpDIB               //指向原DIB图像的指针
int iTempH                //模板高度
int iTempW                //模板宽度                                    
int iTempCX               //模板中心像素X的坐标                           
int iTempCY               //模板中心像素Y的坐标
FLOAT *fpTempArray        //指向模板数组的指针     
FLOAT fCoef               //模板系数
返回值：BOOL              //成功返回TRUE,否则返回FALSE
说明：该函数用指定的模板来对图像进行卷积运算                            */
/************************************************************************/
BOOL CDiffrenceCp::Template(CString InName,CString OutName,int iTempH, int iTempW, int iTempCX, int iTempCY, FLOAT *fpTempArray)
{
	CFIDIO* pFIDI = new CFIDIO(m_szBufPath+"."+InName);
	if(!pFIDI->Open())
	{
		delete pFIDI;return FALSE;
	}
	int lHeight=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	int lWidth=m_DataNum;
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetTitle(m_szDifName+"微分处理...");
	pGuage->ShowWindow(SW_SHOW);
	float dep=m_StProDep;;
	BOOL bUserBreak=FALSE;

	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"PAD","none",REPR_SHORT,4,16,0,0.f,0.f,0.f},
							   {"DATAST","1/ohm",REPR_FLOAT,4,16,16,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;
	m_Content[1].nps=m_DataNum;                        
	m_Content[1].min=2.54f;                            
	m_Content[1].max=m_Bits*3.1416f;                   
	m_Content[1].rlev = m_Bits;                        

	m_Content[2].nps=m_DataNum;
	m_Content[2].npw=m_DataNum;                        
	m_Content[2].min=(float)m_PadNum;                  
	m_Content[2].max=(float)m_ButtonNum;               
	m_Content[2].rlev =0.f;                            
	strcpy(m_Content[2].name,OutName);

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;

	CFIDIO* pFIDW = NULL;
	int Num,l,j,k;
	pFIDW = new CFIDIO(m_szBufPath+"."+OutName,&m_Index);
	if(pFIDW->Open())
	{				
		float *Value=NULL;	
		Value=new float[m_DataNum+10];
		pFIDW->SeekToDepth(m_StProDep+m_Rlev*iTempCY);

		float *Value0=NULL;
		Value0=new float[m_DataNum*iTempH];
		pFIDI->SeekToDepth(m_StProDep);
		pFIDI->Read(3,m_DataNum*iTempH,Value0);
		pFIDI->SeekToDepth(m_StProDep+m_Rlev*iTempH);
		Num=iTempH;
		float fm,fCoef;
		fCoef=0.f;
		for(j=0;j<iTempH*iTempW;j++)
			fCoef+=fpTempArray[j];
		while(Num<lHeight)
		{
			pFIDI->Read(3,m_DataNum,Value);
			pFIDI->Next();
			for(j=0;j<m_DataNum;j++)
				Value0[(iTempH-1)*m_DataNum+j]=Value[j];
			
			//列(除去边缘iTempCX列避免越界)
			for(j=iTempCX;j<lWidth-iTempW+iTempCX+1;j++)
			{
				fm=0;
				for(k=0;k<iTempH;k++)
				{
					for(l=0;l<iTempW;l++)
					{
						//指向DIB第i-iTempCY+k行第j-iTempCX+l个像素的指针
						//计算平均值
						fm+=Value0[k*m_DataNum+j-iTempCX+l]*fpTempArray[k*iTempW+l];
					}
				}
				//乘上系数
				fm/=fCoef;
				Value[j]=(float)fabs(fm);
			}

			pFIDW->Write(3,m_DataNum,Value);
			pFIDW->Next();
			Num++;
			dep=m_StProDep+m_Rlev*Num;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton())
			{
				bUserBreak=TRUE;
				break;
			}
			for(k=0;k<iTempH-1;k++)
				for(j=0;j<m_DataNum;j++)
					Value0[k*m_DataNum+j]=Value0[(k+1)*m_DataNum+j];
			
		}
		delete Value;delete Value0;
	}
	else bUserBreak=TRUE;
	delete pFIDI;delete pFIDW;
	delete pGuage;	
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

/************************************************************************/
/*vFindMedianValue(float lpbArray, int iArrayLen)
参数：float lpbArray                 //指向要获取中值的数组中值       
int iArrayLen                        //数组长度
返回值：float                        //返回数组元素的中值
说明：该函数用冒泡法对一维数组进行排序，并返回数组元素的中值            */
/************************************************************************/
float CDiffrenceCp::FindMedianValue(float *lpbArray, int iArrayLen)
{
	//循环变量
	int i,j;
	//中间变量
	float bTemp;
	//用冒泡法对数组进行排序
	for (j=0;j<iArrayLen;j++)
	{
		for(i=0;i<iArrayLen-j-1;i++)
		{
			if (lpbArray[i]>lpbArray[i+1])
			{
				//互换
				bTemp=lpbArray[i];
				lpbArray[i]=lpbArray[i+1];
				lpbArray[i+1]=bTemp;
			}

		}

	}		
	//计算中值
	if ((iArrayLen&1)>0)
	{
		//数组有奇数个元素，返回中间一个元素
		bTemp=lpbArray[(iArrayLen+1)/2];
	} 
	else
	{
		//数组有偶数个元素，返回中间两个元素的平均值
		bTemp=(lpbArray[iArrayLen/2]+lpbArray[iArrayLen/2+1])/2;
	}
	//返回中值
	return bTemp;
}

/************************************************************************/
/*MedianFilter(int iFilterH, int iFilterW, int iFilterCX, int iFilterCY)
参数:
int iFilterH                     //滤波器高度
int iFilterW                     //滤波器宽度
int iFilterCX                    //滤波器中心元素X坐标
int iFilterCY                    //滤波器中心元素Y坐标
返回值：BOOL                     //成功返回TRUE,否则返回FALSE
该函数用指定的中值滤波器来对图像进行中值滤波操作                        */
/************************************************************************/
BOOL CDiffrenceCp::MedianFilter(CString InName,CString OutName,int iFilterH, int iFilterW, int iFilterCX, int iFilterCY)
{
	CFIDIO* pFIDI = new CFIDIO(m_szBufPath+"."+InName);
	if(!pFIDI->Open())
	{
		delete pFIDI;return FALSE;
	}
	int lHeight=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	int lWidth=m_DataNum;
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetTitle(m_szDifName+"微分处理...");
	pGuage->ShowWindow(SW_SHOW);
	float dep=m_StProDep;;
	BOOL bUserBreak=FALSE;

	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"PAD","none",REPR_SHORT,4,16,0,0.f,0.f,0.f},
							   {"DATAST","1/ohm",REPR_FLOAT,4,16,16,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;
	m_Content[1].nps=m_DataNum;                        
	m_Content[1].min=2.54f;                            
	m_Content[1].max=m_Bits*3.1416f;                   
	m_Content[1].rlev = m_Bits;                        

	m_Content[2].nps=m_DataNum;
	m_Content[2].npw=m_DataNum;                        
	m_Content[2].min=(float)m_PadNum;                  
	m_Content[2].max=(float)m_ButtonNum;               
	m_Content[2].rlev =0.f;                            
	strcpy(m_Content[2].name,OutName);

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;

	CFIDIO* pFIDW = NULL;
	int Num,l,j,k;
	pFIDW = new CFIDIO(m_szBufPath+"."+OutName,&m_Index);
	if(pFIDW->Open())
	{				
		float *Value=NULL;	
		Value=new float[m_DataNum+10];
		pFIDW->SeekToDepth(m_StProDep+m_Rlev*iFilterCY);

		float *Value0=NULL;
		Value0=new float[m_DataNum*iFilterH];
		pFIDI->SeekToDepth(m_StProDep);
		pFIDI->Read(3,m_DataNum*iFilterH,Value0);
		pFIDI->SeekToDepth(m_StProDep+m_Rlev*iFilterH);
		Num=iFilterH;

		float *lpbFilter=new float[iFilterH*iFilterW];
		while(Num<lHeight)
		{
			pFIDI->Read(3,m_DataNum,Value);
			pFIDI->Next();
			for(j=0;j<m_DataNum;j++)
				Value0[(iFilterH-1)*m_DataNum+j]=Value[j];
			
			for(j=iFilterCX;j<lWidth-iFilterW+iFilterCX+1;j++)
			{
				//初始化滤波器数组元素
				for (k=0;k<iFilterH;k++)
				{
					for (l=0;l<iFilterW;l++)
					{
						//指向第i-iTempCY+k行第j-iTempCX+l个像素的指针
						//保存到滤波器数组中
						lpbFilter[k*iFilterW+l]=Value0[k*m_DataNum+j-iFilterCX+l];
					}
				}
				//用滤波器中值代替原来中间像素值
				Value[j]=FindMedianValue(lpbFilter,iFilterH*iFilterW);
			}

			pFIDW->Write(3,m_DataNum,Value);
			pFIDW->Next();
			Num++;
			dep=m_StProDep+m_Rlev*Num;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton())
			{
				bUserBreak=TRUE;
				break;
			}
			for(k=0;k<iFilterH-1;k++)
				for(j=0;j<m_DataNum;j++)
					Value0[k*m_DataNum+j]=Value0[(k+1)*m_DataNum+j];
			
		}
		delete Value;delete Value0;delete lpbFilter;
	}
	else bUserBreak=TRUE;
	delete pFIDI;delete pFIDW;
	delete pGuage;	
	if(bUserBreak) return FALSE;
	else           return TRUE;	
}

/************************************************************************/
/*TraceEdge(CString InName,CString OutName)
该函数用于探测法边界跟踪
成功返回TRUE,否则返回FALSE                                              */
/************************************************************************/
BOOL CDiffrenceCp::TraceEdge(CString InName,CString OutName)
{
	CFIDIO* pFIDI = new CFIDIO(m_szBufPath+"."+InName);
	if(!pFIDI->Open())
	{
		delete pFIDI;return FALSE;
	}

	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetTitle(m_szDifName+"探测法边界跟踪...");
	pGuage->ShowWindow(SW_SHOW);
	float dep=m_StProDep;;
	BOOL bUserBreak=FALSE;

	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"PAD","none",REPR_SHORT,4,16,0,0.f,0.f,0.f},
							   {"DATAST","1/ohm",REPR_FLOAT,4,16,16,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;
	m_Content[1].nps=m_DataNum;                        
	m_Content[1].min=2.54f;                            
	m_Content[1].max=m_Bits*3.1416f;                   
	m_Content[1].rlev = m_Bits;                        

	m_Content[2].nps=m_DataNum;
	m_Content[2].npw=m_DataNum;                        
	m_Content[2].min=(float)m_PadNum;                  
	m_Content[2].max=(float)m_ButtonNum;               
	m_Content[2].rlev =0.f;                            
	strcpy(m_Content[2].name,OutName);

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;

	CFIDIO* pFIDW = NULL;
	int pos,i,j,k;
	pFIDW = new CFIDIO(m_szBufPath+"."+OutName,&m_Index);
	if(pFIDW->Open())
	{	
		int iWinH=100;	
		float *Value=NULL;	
		Value=new float[m_DataNum*iWinH];
		float *Value0=NULL;
		Value0=new float[m_DataNum*iWinH];

		//像素值
		float data,data0,Pixel,MaxPixel;
		//是否找到起始点及回到起始点
		BOOL bFindStartPoint;
		//是否扫描到一个边界点
		BOOL bFindPoint;
		//起始边界点和当前边界点
		CPoint StartPoint,CurrentPoint;
		//8个方向和起始扫描方向
		POINT Direction[8]={{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0}};
		int BeginDirect;
		
		CString str;
		int Num=0;
		dep=m_StProDep;
		int x,y;
	
		while(dep<m_EnProDep-iWinH*m_Rlev)
		{
			pFIDI->SeekToDepth(dep);
			pFIDI->Read(3,m_DataNum*iWinH,Value0);
			for(i=0;i<m_DataNum*iWinH;i++)
				Value[i]=Value0[i];


			//先找到最左上方的边界点
			bFindStartPoint=FALSE;
			//查找第1个边界点
			//注意，留出图像的最外边界，以防越界
			for(j=1;j<iWinH-1&&!bFindStartPoint;j++)
			{
				//每列
				for(i=1;i<m_DataNum-1&&!bFindStartPoint;i++)
				{
					//初始化
					data0=data=768.f;
					//指向原位图第j行第i列的位置
					pos=j*m_DataNum+i;
					Pixel=Value0[pos];
					if(Pixel<512) continue;
					//每个像素点的上1行和下1行的3个元素
					for(y=0;y<3;y++)
					{
						//每个像素点的左一列与右一列的3个像素点
						for(x=0;x<3;x++)
						{
							//计算像素值
							data=Value0[pos+m_DataNum*(y-1)+(x-1)];
							//求取与该点像素点相邻的8个像素的像素值的最大值
							if(data>data0)
								data0=data;
						}
					}
				
					//相邻8个像素点的最大像素值
					MaxPixel=data0;	
					//如果当前像素点是边界点
					if(MaxPixel!=Pixel)
					{
						//则找到了边界跟踪的起始点
						bFindStartPoint=TRUE;
						//记录起始点在图像中的位置
						StartPoint.y=j;
						StartPoint.x=i;
						//指向目标图像倒数第j行和第i列个像素的指针
						//把边界点涂成黑色
						Value[j*m_DataNum+i]=0.f;
					}
				}
			}

			if(bFindStartPoint)
			{
				//由于起始点是在左下方，故起始扫描沿左上方向
				BeginDirect=0;
				//跟踪边界
				bFindStartPoint=FALSE;
				//从初始点开始扫描
				CurrentPoint.y=StartPoint.y;
				CurrentPoint.x=StartPoint.x;

				//如果跟踪还没有回到原来的位置
				while(!bFindStartPoint)
				{
					//没有找到新的边界点
					bFindPoint=FALSE;
					//继续跟踪
					k=0;
					while(!bFindPoint)
					{
						//初始化
						data0=data=768.f;
						//沿扫描方向查看一个像素
						pos=m_DataNum*(CurrentPoint.y+Direction[BeginDirect].y)+
							(CurrentPoint.x+Direction[BeginDirect].x);
						Pixel=Value0[pos];
						//每个像素点的上1行和下1行的3个元素
						for(y=0;y<3;y++)
						{
							//每个像素点的左一列与右一列的3个像素点
							for(x=0;x<3;x++)
							{
								//计算像素值
								data=Value0[pos+m_DataNum*(y-1)+(x-1)];
								//求取与该点像素点相邻的8个像素的像素值的最大值
								if(data>data0)
									data0=data;
							}
						}
						//相邻8个像素点的最大像素值
						MaxPixel=data0;
						//找到了新的边界点
						if(MaxPixel!=Pixel)
						{
							//则找到了边界跟踪的起始点	
							bFindPoint=TRUE;
							//记录起始点在图像中的位置
							CurrentPoint.y+=Direction[BeginDirect].y;
							CurrentPoint.x+=Direction[BeginDirect].x;

							//如果新的边界点就是起始点，则跟踪完成
							if(CurrentPoint==StartPoint)
								bFindStartPoint=TRUE;
							if(CurrentPoint.y>iWinH-1||CurrentPoint.x>m_DataNum-1)
								bFindStartPoint=TRUE;
							//将边界点涂成黑色
							pos=m_DataNum*CurrentPoint.y+CurrentPoint.x;
							Value[pos]=0.f;

							//扫描的方向逆时针旋转两格
							BeginDirect--;
							if(BeginDirect<0)
								BeginDirect=7;
							BeginDirect--;
							if(BeginDirect<0)
								BeginDirect=7;
						}
						//每找到新的边界点
						else
						{
							//扫描方向顺时针转一格
							BeginDirect++;
							if(BeginDirect>7)
								BeginDirect=0;
						}
					}
				}
			}

			pFIDW->SeekToDepth(dep);
			pFIDW->Write(3,m_DataNum*iWinH,Value);
			Num+=iWinH;
			dep=m_StProDep+m_Rlev*Num;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton())
			{
				bUserBreak=TRUE;
				break;
			}
		}
		delete Value;delete Value0;
	}
	else bUserBreak=TRUE;
	delete pFIDI;delete pFIDW;
	delete pGuage;	
	if(bUserBreak) return FALSE;
	else           return TRUE;	
}

/************************************************************************/
/*KAveIncise(CString InName,CString OutName,int k)
int k;                  //分割的区域数    
成功返回TRUE,否则返回FALSE
该函数用于k-均值聚类法图像分割                                          */
/************************************************************************/
BOOL CDiffrenceCp::KAveIncise(CString InName,CString OutName,int k)
{
	CFIDIO* pFIDI = new CFIDIO(m_szBufPath+"."+InName);
	if(!pFIDI->Open())
	{
		delete pFIDI;return FALSE;
	}

	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetTitle(m_szDifName+" k-均值聚类法图像分割...");
	pGuage->ShowWindow(SW_SHOW);
	float dep=m_StProDep;;
	BOOL bUserBreak=FALSE;

	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"PAD","none",REPR_SHORT,4,16,0,0.f,0.f,0.f},
							   {"DATAST","1/ohm",REPR_FLOAT,4,16,16,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;
	m_Content[1].nps=m_DataNum;                        
	m_Content[1].min=2.54f;                            
	m_Content[1].max=m_Bits*3.1416f;                   
	m_Content[1].rlev = m_Bits;                        

	m_Content[2].nps=m_DataNum;
	m_Content[2].npw=m_DataNum;                        
	m_Content[2].min=(float)m_PadNum;                  
	m_Content[2].max=(float)m_ButtonNum;               
	m_Content[2].rlev =0.f;                            
	strcpy(m_Content[2].name,OutName);

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;

	CFIDIO* pFIDW = NULL;
	pFIDW = new CFIDIO(m_szBufPath+"."+OutName,&m_Index);
	if(pFIDW->Open())
	{		
		int iWinH=20;
		float *Value=NULL;	
		Value=new float[m_DataNum+10];
		pFIDW->SeekToDepth(m_StProDep+m_Rlev*iWinH/2);

		float *Value0=NULL;
		Value0=new float[m_DataNum*iWinH];
		pFIDI->SeekToDepth(m_StProDep);
		pFIDI->Read(3,m_DataNum*iWinH,Value0);
		pFIDI->SeekToDepth(m_StProDep+m_Rlev*iWinH);
		
		//每个区域的所有像素的平均值
		float *kAverage=new float[k];
		//每个区域的像素数目
		int *kAveNum=new int[k];
		//循环控制量
		BOOL bContinue;
		float Pixel;
		int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
		int Num,i,j,l,m,n;
		Num=iWinH;
		
		while(Num<RecordNum)
		{
			pFIDI->Read(3,m_DataNum,Value);
			pFIDI->Next();
			for(j=0;j<m_DataNum;j++)
				Value0[(iWinH-1)*m_DataNum+j]=Value[j];

			//初始化
			for(j=0;j<k;j++)
			{
				kAveNum[j]=1;
				kAverage[j]=j*1024.f/(k-1);
			}
			//初始化为真
			bContinue=TRUE;
			//循环,对图像进行分割
			while(bContinue)
			{
				//图像每行
				for(j=0;j<iWinH;j++)
				{
					//图像每列
					for(i=0;i<m_DataNum;i++)
					{
						//原图像的第j行第i列的像素
						Pixel=Value0[j*m_DataNum+i];
						//判断该像素属于哪个区域
						for(l=0;l<k;l++)
						{
							n=0;
							for(m=0;m<k;m++)
							{
								if(m==1)
									continue;
								if(pow(Pixel-kAverage[l],2.f)>pow(Pixel-kAverage[m],2.f))
									break;
								else
									n++;
							}
							//如果属于第l个区域
							if(n==(k-1))
							{
								//该区域像素数目加1
								kAveNum[l]++;
								//修改该像素的值目的是使各个区域好区分
								Value[i]=l*1024.f/(k-1);
								//继续判断下一个区域
								break;
							}
						}

						//如果第l个区域增加1个像素后，其像素平均值不变，则区域分割完毕
						if(fabs(kAverage[l]-(kAverage[l]*(kAveNum[l]-1)+Pixel)/kAveNum[l])<0.1f)
							bContinue=FALSE;
						//否则计算新的平均值，并取代原来的平均值
						else
							kAverage[l]=(kAverage[l]*(kAveNum[l]-1)+Pixel)/kAveNum[l];
					}
				}
			}

			pFIDW->Write(3,m_DataNum,Value);
			pFIDW->Next();
			Num++;
			dep=m_StProDep+m_Rlev*Num;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton())
			{
				bUserBreak=TRUE;
				break;
			}
			for(i=0;i<iWinH-1;i++)
				for(j=0;j<m_DataNum;j++)
					Value0[i*m_DataNum+j]=Value0[(i+1)*m_DataNum+j];
			
		}
		delete Value;delete Value0;
		delete kAveNum;delete kAverage;
	}
	else bUserBreak=TRUE;
	delete pFIDI;delete pFIDW;
	delete pGuage;	
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

// DIPCompute.cpp: implementation of the CDIPCompute class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FlowChart.h"
#include "FlowChartDoc.h"
#include "FlowNode.h"
#include "LinkWellData.h"
#include "DIPCompute.h"
#include "DIPComputeDlg.h"
#include "FIDio.h"
#include "LogProcess.h"
#include "DIPCalculate.h"
#include "Guage.h"
#include <process.h>
#include <math.h>
#include "Insert3D.h"
#include "DiffrenceDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CDIPCompute, CFlowNode, 0)

CDIPCompute::CDIPCompute()
{
	m_pDoc=NULL;
}

CDIPCompute::CDIPCompute(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
	m_WinLength=2.54f;

	m_StepLength=0.127f;
	m_SearchLength=0.508f;
	m_DangMax=70.f;
	m_RMin=0.5f;
	m_GradCons=100000.f;
	m_AzMax=50.f;
	m_NumMin=10.f;                //样本率
}

CDIPCompute::~CDIPCompute()
{

}

BOOL CDIPCompute::Run()
{	
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	if(m_szNodeName=="地层倾角计算")
	{
		m_bResult=TRUE;	
		if(!m_pDoc->m_bAunAll)
		{
			CDIPComputeDlg dlg;
			dlg.m_WinLength=m_WinLength;
			dlg.m_StepLength=m_StepLength;
			dlg.m_SearchLength=m_SearchLength;
			dlg.m_DangMax=m_DangMax;
			dlg.m_RMin=m_RMin;
			dlg.m_GradCons=m_GradCons;
			dlg.m_AzMax=m_AzMax;
			dlg.m_NumMin=m_NumMin;
			if(dlg.DoModal ()!=IDOK)
				return TRUE;
			else
			{
				m_WinLength=dlg.m_WinLength;
				m_StepLength=dlg.m_StepLength;
				m_SearchLength=dlg.m_SearchLength;
				m_DangMax=dlg.m_DangMax;
				m_RMin=dlg.m_RMin;
				m_GradCons=dlg.m_GradCons;
				m_AzMax=dlg.m_AzMax;
				m_NumMin=dlg.m_NumMin;
				m_bResult=DIPPro();
				OnWellRefresh();
				if(m_pDoc!=NULL)
					m_pDoc->SetModifiedFlag();
			}
		}	
		return m_bResult;
	}
	CString szImageType="静态图像";
	if(!m_pDoc->m_bAunAll)
	{
		CDiffrenceDlg dlg;
		dlg.m_szDifName=szImageType;
		dlg.m_DaltaDep=0.0762f;
		if(dlg.DoModal()!=IDOK)
			return FALSE;
		szImageType=dlg.m_szDifName;
	}

	CString InName,OutName;
	//原始数据;静态数据;动态数据
	if(szImageType=="静态图像")
		InName="STAMI";
	else
		InName="DYNMI";
	OutName=InName+"PIC";
	m_bResult=CreateFullImage(InName,OutName);
	if(m_bResult)
		OnWellRefresh();
	return m_bResult;
}

void CDIPCompute::Serialize(CArchive& ar)
{
	CFlowNode::Serialize(ar);
	if (ar.IsStoring())
	{
		ar<<m_WinLength;
		ar<<m_StepLength;
		ar<<m_SearchLength;
		ar<<m_DangMax;
		ar<<m_RMin;
		ar<<m_AzMax;
		ar<<m_NumMin;
	}
	else
	{
		ar>>m_WinLength;
		ar>>m_StepLength;
		ar>>m_SearchLength;
		ar>>m_DangMax;
		ar>>m_RMin;
		ar>>m_AzMax;
		ar>>m_NumMin;
		m_GradCons=100000.f;
	}
}
//井周反间距三维插值图像恢复
BOOL CDIPCompute::CreateFullImage(CString InName,CString OutName)
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
	pGuage->SetTitle(InName+" 全井壁图像恢复 "+OutName+" ...");
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
	strcpy(m_Content[2].name,OutName);

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;
	//初始化全井壁恢复图像
	CFIDIO* pFIDW = new CFIDIO(m_szWellName+"."+OutName,&m_Index);
	if(!pFIDW->Open())
	{	
		AfxMessageBox(OutName+" 初始化全井壁恢复图像失败!        ");
		delete pFIDW;
		delete pGuage;
		return FALSE;
	}
	pFIDW->Close(); 
	delete pFIDW;pFIDW=NULL;

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
	float *pImage=new float[DataNum*RecordNum];			//图像恢复数据
	//读取曲线所有数据
	fp.Seek (0L,CFile::begin);
	fp.Read (Value,DataNum*RecordNum*4);
	fp.Close();	

	//反间距三维插值
	//XY方向为井周平面XY值
	//Z方向取ndy*2个深度采样，深度点位上取ndy个采样，下取ndy个采样
	int ndy=5;
	double x,y,z;
	vector<Point3D> input;
	Insert3D* pInterpolater = new Insert3D(DataNum,6);
	int i,j,k=0;
	double Radius=DataNum/2/PI;
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
		//全井壁图像恢复
		for(i=0;i<DataNum;i++)
		{
			if(Value[DataNum*j+i]==-9999.f)
			{
				x=Radius*sin((i*2*PI)/DataNum);
				y=Radius*cos((i*2*PI)/DataNum);
				pImage[DataNum*j+i]=pInterpolater->GetInsertV(x,y,z,(Point3D*)&(*input.begin()),(Point3D*)&(*input.end()));
			}
			else pImage[DataNum*j+i]=Value[DataNum*j+i];
		}

		pGuage->SetDepth(m_StProDep+j*m_Rlev);
		if(pGuage->CheckCancelButton())
			break;
	}
	delete pInterpolater;
	//保存全井壁图像恢复数据
	strcpy(s,m_szWellName);
	strcat(s,".");strcat(s,OutName);
	CFIDIO::GetFileName(s,fn,'D');
	if(fp.Open(fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		fp.Seek (0L,CFile::begin);
		fp.Write (pImage,DataNum*RecordNum*4);
		fp.Close ();
	}
	delete Value;delete pImage;
	delete pGuage;
	return TRUE;
}

float CDIPCompute::KrigingCompute(int Num,float *X)
{
	int size=Num*3+2;
	int i,j,m;
	double tmp;

	double **K=new double *[Num*2+3];
	for(i=0;i<Num*2+3;i++)
		K[i]= new double[Num*2+3];

	double *D=new double[Num+3];
	double *DD=new double[Num+3];

	for(i=1;i<Num+1;i++)
	{
		for(j=1;j<Num+1;j++)
		{
			tmp=(X[3*i-3]-X[3*j-3])*(X[3*i-3]-X[3*j-3])+(X[3*i-2]-X[3*j-2])*(X[3*i-2]-X[3*j-2]);
		    tmp=sqrt(tmp);
			if(tmp==0)
				K[i][j]=3.202;
			else if(tmp>8.535)
				K[i][j]=0;
			else
				K[i][j]=3.202-(2.048+1.154*(1.5*tmp/8.535-0.5*(tmp*tmp*tmp)/(8.535*8.535*8.535)));
		}
		
		tmp=(X[3*i-3]-X[size-2])*(X[3*i-3]-X[size-2])+(X[3*i-2]-X[size-1])*(X[3*i-2]-X[size-1]);
		tmp=sqrt(tmp);
		if(tmp==0)
			D[i]=3.202;
		else if(tmp>8.535)
			D[i]=0;
		else
			D[i]=3.202-(2.048+1.154*(1.5*tmp/8.535-0.5*(tmp*tmp*tmp)/(8.535*8.535*8.535)));
	}
	D[Num+1]=1;
	for(j=1;j<Num+1;j++)
	{
		K[Num+1][j]=1;
		K[j][Num+1]=1;
	}
	K[Num+1][Num+1]=0;

	for(i=1;i<Num+2;i++)
	{
		for(j=(Num + 2);j<(2*Num+3);j++)
		{
			if(j-i==Num+1)
				K[i][j]=1;
			else
				K[i][j]=0;       //将一个与K相同行列的单位矩阵放到K的后面，将采取矩阵变换来求逆矩阵
		}
	}

	//开始逆矩阵的转化
	for(j=1;j<Num+2;j++)
	{
		for(m=Num+1;m>0;m--)
		{
			if(m>j)
			{
				if(K[m][j] != 0)
				{
					if(K[m-1][j] != 0)
					{
						tmp=K[m][j];
						for(i=0;i<2*Num+3;i++)
						{
							K[m][i]=K[m][i]*K[m-1][j]/tmp - K[m-1][i];
							K[m][j]=0;
						}
					}
					else
					{
						for(i=0;i<2*Num+3;i++)
						{
							tmp=K[m][i];
							K[m][i]=K[m-1][i];
							K[m-1][i]=tmp;
						}
					}
				}
			}
		}
	}//正变换结束

	//反变换开始
	for(j=Num+1;j>0;j--)
	{
		for(m=1;m<Num+2;m++)
		{
			if(j>m)
			{
				if(K[m][j]!= 0)
				{
					tmp=K[m][j];
					for(i=1;i<2*Num+3;i++)
					{
						K[m][i]=K[m][i]*K[m+1][j]/tmp -K[m+1][i];
						K[m][j]=0;
					}
				}
				else
				{
					for(i=1;i<2*Num+3;i++)
					{
						tmp=K[m][i];
					    K[m][i]=K[m-1][i];
					    K[m-1][i]=tmp;
					}
				}
			}
		}
	}

	for(m=1;m<Num+2;m++)
	{
		tmp=K[m][m];
		for(i=1;i<2*Num+3;i++)
			K[m][i]=K[m][i]/tmp;
	}

	/////////////////////计算两个矩阵相乘
	tmp=0;
	for(m=1;m<Num+2;m++)
	{
		for(i=Num+2;i<2*Num+3;i++)
           tmp=tmp+K[m][i] * D[i-Num-1]; 
		DD[m]=tmp;
		tmp=0;
	}

    tmp=0;
	for(i=1;i<Num+1;i++)
		tmp=tmp+X[3*i-1] * DD[i];

	for(i=0;i<Num*2+3;i++)
		delete K[i];
	delete K;
	delete D;delete DD;

	return (float)tmp;
}

//平面纹理合成图像恢复
BOOL CDIPCompute::TextureRecover(CString InName,CString OutName)
{
	int DataNum=0;
	CFIDIO* PadI=new CFIDIO(m_szWellName+"."+InName);
	if(PadI->Open())
	{
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
	int iWin0=15;
	float m_DaltaDep=0.0762f;
	int iWin13=(int)(m_DaltaDep/m_Rlev);
	int iWin=iWin13+iWin0*2;
	
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep-iWin13*m_Rlev);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetTitle(InName+" 全井壁图像恢复 "+OutName+" ...");
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
	strcpy(m_Content[2].name,OutName);

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;
	//初始化全井壁恢复图像
	CFIDIO* pFIDW = new CFIDIO(m_szWellName+"."+OutName,&m_Index);
	if(!pFIDW->Open())
	{	
		AfxMessageBox(OutName+"初始化全井壁恢复图像失败!        ");
		delete pFIDW;
		delete pGuage;
		return FALSE;
	}
	pFIDW->Close(); 
	delete pFIDW;pFIDW=NULL;

	char s[256],fn[256];
	strcpy(s,m_szWellName);
	strcat(s,".");strcat(s,InName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(!fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		AfxMessageBox(InName+"打开曲线创建失败!        ");
		delete pGuage;
		return FALSE;
	}

	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	float *Value=new float[DataNum*RecordNum];
	//读取曲线所有数据
	fp.Seek (0L,CFile::begin);
	fp.Read (Value,DataNum*RecordNum*4);
	fp.Close ();	

	int i,j,k,n,m,p;
	typedef struct{
					int	x[8];
					int	y[8];
					int count;
					float value[8];
				} TEMPLET;	

	TEMPLET TempToTop[4],TempToBot[4];
	TempToTop[0].count=8;
	TempToTop[1].count=7;
	TempToTop[2].count=6;
	TempToTop[3].count=5;

	for(i=0;i<4;i++)
	{
		TempToTop[i].x[0]=0;	TempToTop[i].y[0]=-6;
		TempToTop[i].x[1]=0;	TempToTop[i].y[1]=-3;
		TempToTop[i].x[2]=-1;	TempToTop[i].y[2]=-6;
		TempToTop[i].x[3]=-1;	TempToTop[i].y[3]=-3;
		TempToTop[i].x[4]=-1;	TempToTop[i].y[4]=0;
		TempToTop[i].x[5]=-1;	TempToTop[i].y[5]=3;
		TempToTop[i].x[6]=-1;	TempToTop[i].y[6]=6;
		TempToTop[i].x[7]=-1;	TempToTop[i].y[7]=9;
	}

	TempToBot[0].count=8;
	TempToBot[1].count=7;
	TempToBot[2].count=6;
	TempToBot[3].count=5;

	for(i=0;i<4;i++)
	{
		TempToBot[i].x[0]=1;	TempToBot[i].y[0]=-9;
		TempToBot[i].x[1]=1;	TempToBot[i].y[1]=-6;
		TempToBot[i].x[2]=1;	TempToBot[i].y[2]=-3;
		TempToBot[i].x[3]=1;	TempToBot[i].y[3]=0;
		TempToBot[i].x[4]=1;	TempToBot[i].y[4]=3;
		TempToBot[i].x[5]=1;	TempToBot[i].y[5]=6;
		TempToBot[i].x[6]=0;	TempToBot[i].y[6]=3;
		TempToBot[i].x[7]=0;	TempToBot[i].y[7]=6;
	}


	int iNum=0;		   //指示选用第iNum模板
	int minX,minY;
	double minVal,tmp;
	
	int addNum=DataNum/m_PadNum;
	int proNum=DataNum+2*addNum;
	float *pData=new float[proNum*iWin];
	float dep=m_StProDep;
	for(int id=0;id<RecordNum-iWin;id+=iWin13)
	{
		//准备用于处理的数据
		//处理的数据为前后增加一个极板的数据
		for(j=0;j<iWin;j++)
		{
			for(i=0;i<addNum;i++)
				pData[proNum*j+i]=Value[DataNum*(id+j)+DataNum-addNum+i];
			for(i=0;i<DataNum;i++)
				pData[proNum*j+addNum+i]=Value[DataNum*(id+j)+i];
			for(i=0;i<addNum;i++)
				pData[proNum*j+addNum+DataNum+i]=Value[DataNum*(id+j)+i];
		}
		//从左上角开始搜索
		for(n=2;n<iWin;n++) 
		{
			for(m=addNum;m<proNum-addNum;m++) 
			{
				if(pData[proNum*n+m]==-9999.f)   	//此像素点需要修复
				{
					/*iNum=m%4;*/
					for(k=0;k<TempToTop[iNum].count;k++)
					{
						p=proNum*(TempToTop[iNum].x[k]+n)+TempToTop[iNum].y[k]+m;
						TempToTop[iNum].value[k]=pData[p];
					}

					minX=0;minY=0;minVal=-1;
					for(i=2;i<iWin;i+=2)
					{
						for(j=addNum;j<proNum-addNum;j+=3)
						{
							if(m!=j && n!=i)
							{
								tmp=0;
								for(k=0;k<TempToTop[iNum].count;k++)
								{
									p=proNum*(iWin-(TempToBot[iNum].x[k]+i))+TempToBot[iNum].y[k]+j;
									tmp+=fabs(TempToTop[iNum].value[k]-pData[p]);
								}
								if(tmp<minVal||minVal==-1)
								{
									minVal=tmp;
									minY=i;
									minX=j;
								}
							}
						}
					}
					pData[proNum*n+m]=pData[proNum*(iWin-minY)+minX];
				}
			}
		}
		//从右下角开始搜索
		for(n=iWin-1;n>0;n--) 
		{
			for(m=proNum-addNum;m>addNum;m--) 
			{
				if(pData[proNum*n+m]==-9999.f)		//此像素点需要修复
				{
					/*iNum=m%4;*/
					for(k=0;k<TempToBot[iNum].count;k++)
					{
						p=proNum*(TempToBot[iNum].x[k]+n)+TempToBot[iNum].y[k]+m;
						TempToBot[iNum].value[k]=pData[p];
					}
					minX=0;minY=0;minVal=-1;
					for(i=2;i<iWin;i+=2)
					{
						for(j=addNum;j<proNum-addNum;j+=3)
						{
							if(m!=j && n!=i)
							{
								tmp=0;
								for(k=0;k<TempToBot[iNum].count;k++)
								{
									p=proNum*(iWin-(TempToBot[iNum].x[k]+i))+TempToBot[iNum].y[k]+j;
									tmp+=fabs(TempToBot[iNum].value[k]-pData[p]);
								}
								if(tmp<minVal||minVal<0)
								{
									minVal=tmp;
									minY=i;
									minX=j;
								}
							}
						}
					}
					pData[proNum*n+m]=pData[proNum*(iWin-minY)+minX];
				}
			}	
		}
		
		//将处理后的数据中间部份写到曲线数据中
		for(j=0;j<iWin13;j++)
			for(i=0;i<DataNum;i++)
				Value[(id+j+iWin0)*DataNum+i]=pData[(j+iWin0)*proNum+addNum+i];
	
		dep=m_StProDep+id*m_Rlev;
		pGuage->SetDepth(dep);
		if(pGuage->CheckCancelButton())
			break;
	}
	//保存曲线所有数据
	strcpy(s,m_szWellName);
	strcat(s,".");strcat(s,OutName);
	CFIDIO::GetFileName(s,fn,'D');
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		fp.Seek (0L,CFile::begin);
		fp.Write (Value,DataNum*RecordNum*4);
		fp.Close ();
	}

	delete pData;
	delete Value;
	delete pGuage;
	return TRUE;
}

//计算相关系数
int CDIPCompute::CORREL()
{
	int LOCAMX=0;
	float C=0;
    m_RMAX=-9999.f;
 	int i,j; 
	float SUMX=0.f;
    for(i=0;i<IWL;i++)
        SUMX+=X[i];	
    float XAVE=SUMX/((float)IWL);
	SUMX=0.f;
	for(i=0;i<IWL;i++)
	    SUMX+=(X[i]-XAVE)*(X[i]-XAVE);
	if(SUMX<0.00001f)
		return 0;
    for(j=0;j<2*ISL;j++)
	{
         float SUMY=0.f;
         for(i=0;i<IWL;i++)
           SUMY+=Y[i+j];
         float YAVE=SUMY/((float)IWL);       
         SUMY=0.f;
	     float SUMXY=0.f;
	     for(i=0;i<IWL;i++)
		 {
	       SUMY+=(Y[i+j]-YAVE)*(Y[i+j]-YAVE);
	       SUMXY+=(X[i]-XAVE)*(Y[i+j]-YAVE);
		 }
	     if(SUMY<0.00001)
	       C=0.f;
	     else
	       C=SUMXY/(float)(sqrt(SUMX*SUMY));
  	     if(m_RMAX<C)
		 {
			m_RMAX=C;
			LOCAMX=j;
		 }
	}
	return LOCAMX;
}

BOOL CDIPCompute::DIPPro()
{
	CFIDIO* pCal = new CFIDIO(m_szBufPath+".RADSGPT");
	if(!pCal->Open())
	{
		AfxMessageBox("打开阵列井径曲线失败        ");
		delete pCal;
		return FALSE;
	}
	BOOL bUserBreak=FALSE;
	CLogProcess *pLogIn=NULL;
	int PadNum=m_PadNum;
	if(m_szSeries.Find("FMI")>-1)
		PadNum=4;

	int i,NumIn=PadNum+4;
	pLogIn = new CLogProcess(m_szBufPath,NULL);
	INCURVE *PInCurve = new INCURVE[NumIn];
	strcpy(PInCurve[0].name,"DEVGPT");
	strcpy(PInCurve[1].name,"DAZGPT");
	strcpy(PInCurve[2].name,"P1AZGPT");
	strcpy(PInCurve[3].name,"RBGPT");
	CString str;
	for (i=0;i<PadNum;i++)
	{
		str.Format ("DIP%iGPT",i);
		strcpy(PInCurve[4+i].name,str);
	}

	float *XX=NULL;
	XX = new float[NumIn];
	pLogIn->InitIn(NumIn,PInCurve,(char *)XX);
	pLogIn->InitBuffer();
	if(m_StProDep< pLogIn->StartDep)
		m_StProDep= pLogIn->StartDep;
	if(m_EnProDep>pLogIn->EndDep)
		m_EnProDep=pLogIn->EndDep;
	float Rlev=pLogIn->DepLevel;
	if (Rlev <= 0.0001f)
	{
		AfxMessageBox("采样步长小于 0.0001",MB_OK|MB_ICONERROR);
		delete []XX;
		delete PInCurve;
		delete pLogIn;
		delete pCal;
		return FALSE;
	}
	IWL=(int)(m_WinLength/Rlev);
	float WinLength=Rlev*(float)IWL;
	ISL=(int)(m_SearchLength/Rlev);
	float SearchLength=Rlev*(float)ISL;
	ISTEP=(int)(m_StepLength/Rlev);
	float StepLength=Rlev*(float)ISTEP;
	int IWSL=IWL+ISL*2;	

	if(IWSL>4990)
	{
		AfxMessageBox("WL(窗长)+2*SL(探索长度)大于12.5米        ",MB_OK|MB_ICONERROR);
		delete []XX;
		delete PInCurve;
		delete pLogIn;
		delete pCal;
		return FALSE;
	}
	float ddd=m_WinLength/2.f+m_SearchLength;
	if(m_EnProDep+ddd<pLogIn->EndDep) ddd=0;
	
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep-ddd);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);	
	pGuage->SetTitle("倾角计算...");
	
	float *CalValue=NULL;
	CalValue=new float[PadNum];

	CLogProcess *pLogOut=NULL;
	pLogOut = new CLogProcess(m_szWellName,NULL);
	OUTCURVE *POutCurve = new OUTCURVE[3];
	strcpy(POutCurve[0].name,"DDIP");
	strcpy(POutCurve[1].name,"DDIR");
	strcpy(POutCurve[2].name,"GRAD");
	float *YY= new float[3];
	pLogOut->StartDep=m_StProDep;
	pLogOut->EndDep=m_EnProDep;
	pLogOut->DepLevel=StepLength;
	pLogOut->InitOut(3,POutCurve,(char *)YY);
	pLogOut->InitBuffer();

	int j,k,Num;	
	float Value[8][5000];
	int POS[50],Np;
	double ll[8],llR[50],temp;

	CDIPCalculate dipObj;	
	float   dep=m_StProDep;
	float	dip,dir,dip0,dir0;
	float	dev,daz,az1,rb;
	float 	sigma2;

	int nb=1,SetpNum=0;
	CString Filename=m_szWellName.Left (m_szWellName.ReverseFind ('\\'));
	Filename=Filename+"\\HandWork.List";
	FILE* fp=NULL;
	fp=fopen(Filename,"w");
	if(fp!=NULL)
	{
		str="序号,深度,原始角度,原始方位,类型,真角度,真方位\n";
		fprintf(fp,"%s",str);
		
		int iWin=(int)(m_WinLength/m_StepLength);
		float *pAz=NULL;
		pAz=new float[iWin+10];
		int kk=0,mm=0,nn;
		float RDAZ,RDAZ0;

		while(dep<m_EnProDep-ddd)
		{
			//读取窗长内的电导率曲线
			for(i=0;i<IWSL;i++)
			{
				pLogIn->Dep=dep+i*Rlev;
				pLogIn->DatIn();
				for(j=0;j<PadNum;j++)
					Value[j][i]=XX[4+j];
				//为计算倾角，取出窗长中间的井斜、方位、半径
				if(i==IWSL/2)
				{
					dev=XX[0];
					daz=XX[1];
					az1=XX[2];
					rb=XX[3];
					pCal->SeekToDepth(pLogIn->Dep);
					pCal->Read(3,PadNum,CalValue);
					for(j=0;j<PadNum;j++)
						vr[j]=CalValue[j]*254.f;
				}
			}

			//计算I极板到各极板的弧长
			ll[0]=0.;
			double rl=0.f;
			for(i=0; i<PadNum; i++)
				rl+=(double)vr[i];
			dipObj.ll=2.*PI*rl/PadNum;
			for(i=1; i<PadNum; i++)
				ll[i]=ll[i-1]+vr[i]*dipObj.ll/rl;

			Np=0;
			//进行两两相关对比
			for(i=0;i<PadNum;i++)
			{
				//标准数组赋值
				for(j=ISL;j<ISL+IWL;j++)
					X[j-ISL]=Value[i][j];
				for(k=i+1;k<PadNum;k++)
				{
					//目标数组赋值
					for(j=0;j<IWSL;j++)
						Y[j]=Value[k][j];
					POS[Np]=CORREL();
					if(m_RMAX>m_RMin)
					{
						llR[Np]=ll[k];
						Np++;
					}
				}
			}
			//对所有相关点以与I号极板远近排序
			Num=0;
			while(Num<Np)
			{
				k=0;temp=9999.f;
				for(i=0;i<Np;i++)
				{
					if(llR[i]<temp)
					{
						temp=llR[i];
						k=i;
					}
				}
				dipObj.x[Num]=llR[k];
				dipObj.y[Num]=((double)(POS[k]-ISL))*Rlev*10000.;
				llR[k]=99999.f;	
				Num++;
			}
			pAz[kk]=-999.25f;
			if(Np>2)
			{	
				dipObj.vnn=Np;
				dipObj.Compute();	
				dip=(float)dipObj.m_dip ;
				dir=(float)dipObj.m_dir+az1;
				while(dir>360.) 
					dir-=360.;
				while(dir<0.) 
					dir+=360.;
				if(dip<0.001f) 
					dip=0.001f;
				sigma2=m_GradCons/(float)dipObj.m_sigma2;
				if(dev>0.5f)
				{
					dipObj.m_dip =dip;
					dipObj.m_dir =dir;
					dipObj.m_dip1 =dev;
					dipObj.m_dir1 =daz;
					dipObj.ComputeDIPs ();
					dip0=(float)dipObj.m_dips;
					dir0=(float)dipObj.m_dirs;

					while(dir0>360.) 
						dir0-=360.;
					while(dir0<0.) 
						dir0+=360.;
				}
				else
				{
					dip0=dip;
					dir0=dir; 
				}
				if(dip0<m_DangMax)
				{
					pAz[kk]=dir0;
					//输出曲线
					YY[0]=dip0;YY[1]=dir0;YY[2]=sigma2;
				}	
			}
			kk++;
			if(mm>iWin-1)
			{
				//计算窗口内方位标准差
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
						YY[2]=100.f-RDAZ;
						pLogOut->Dep=dep;
						pLogOut->DatOut();

						str.Format("%d,%g,%g,%g,%s,%g,%g\n",
								nb,dep,dip,dir,"层理",dip0,dir0);
						fprintf(fp,"%s",str);
						nb++;
					}
				}
				for(i=1;i<kk;i++)
					pAz[i-1]=pAz[i];
				kk--;
			}
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton()) 
			{
				bUserBreak=TRUE;
				break;
			}
			mm++;
			SetpNum+=ISTEP;
			dep=m_StProDep+SetpNum*Rlev;
		}
		delete pAz;
		fclose(fp);
	}
	
	delete []XX;
	delete PInCurve;
	delete pLogIn;
		
	delete CalValue;
	delete pCal;
	delete pGuage;

	delete []YY;
	delete POutCurve;
	delete pLogOut;

	return TRUE;
}


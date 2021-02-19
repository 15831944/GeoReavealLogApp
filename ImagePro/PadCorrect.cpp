// PadCorrect.cpp: implementation of the CPadCorrect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "flowchart.h"
#include "PadCorrect.h"
#include "PADDepCorDlg.h"
#include "FlowChartDoc.h"
#include "FIDio.h"
#include "Guage.h"
#include "DataUnite.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CPadCorrect, CFlowNode, 0)
CPadCorrect::CPadCorrect()
{
	m_pDoc=NULL;
}

CPadCorrect::CPadCorrect(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
	m_ButDalta=-9999.f;
	m_bDipDepPro=FALSE;
	m_bRADSDepPro=FALSE;
	m_bACDepPro=FALSE;
	m_MinSemblance=99.65f;
}

CPadCorrect::~CPadCorrect()
{

}

void CPadCorrect::Serialize(CArchive& ar)
{
	m_Ver=1.5f;
	CFlowNode::Serialize(ar);	
	int i;
	if (ar.IsStoring())
	{
		ar<<m_ButDalta;
		for(i=0;i<10;i++)
			ar<<m_DepDalta[i];
		ar<<m_bDipDepPro;
		ar<<m_bRADSDepPro;
		ar<<m_bACDepPro;
		ar<<m_MinSemblance;
	}
	else
	{
		ar>>m_ButDalta;
		for(i=0;i<10;i++)
			ar>>m_DepDalta[i];
		ar>>m_bDipDepPro;
		ar>>m_bRADSDepPro;
		if(m_Ver>1.f)
			ar>>m_bACDepPro;
		if(m_Ver>1.2f)
			ar>>m_MinSemblance;
	}
}

BOOL CPadCorrect::Run()
{
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	if(m_ButDalta==-9999.f)
	{
		int i;
		//初始化各极板间距
		for(i=0;i<10;i++)
			m_DepDalta[i]=0.f;
		//初始化电扣间距
		m_ButDalta=0.00762f;                      //0.3英寸
		if(m_szSeries=="FMI_8PAD")
			for(i=1;i<10;i+=2)
				m_DepDalta[i]=0.14478f;           //5.7英寸
		else if(m_szSeries=="FMI_4PAD")
			for(i=1;i<10;i+=2)
					m_DepDalta[i]=0.0f;               
		else if(m_szSeries=="EMI")
		{
			for(i=1;i<10;i+=2)
				m_DepDalta[i]=0.062f;             //2.44英寸
		}
		else if(m_szSeries=="CMI")
		{
			for(i=1;i<10;i+=2)
				m_DepDalta[i]=0.f;				  //2.44英寸
			m_ButDalta=0.f;
			m_bDipDepPro=FALSE;
			m_bRADSDepPro=FALSE;
			m_bACDepPro=FALSE;
		}
		else
		{
			for(i=1;i<10;i+=2)
				m_DepDalta[i]=0.088392f;          //3.48英寸
			m_bDipDepPro=TRUE;
			m_bRADSDepPro=TRUE;
			m_bACDepPro=FALSE;
		}
	}

	if(!m_pDoc->m_bAunAll)
	{
		CPADDepCorDlg dlg(this);
		if(dlg.DoModal()!=IDOK)
			return FALSE;
	}
	m_bResult=TRUE;
	if(m_szSeries!="CMI")
	{
		//极板加速度校正  
		if(m_bACDepPro) AccelerationPro();

		//除Schlumberger公司的FMI仪器外全部作极板深度校正
		if(m_szSeries.Find("FMI")==-1)
			m_bResult=PadDepPro();

		//Halliburton电成像仪电扣深度校正
		if(m_szSeries=="EMI"||m_szSeries=="CMI")
			m_bResult=EMIButtonPro(m_ButDalta);
		//Baker Atlas电成像仪电扣深度校正
		else if(m_szSeries.Find("STAR")>-1)
			m_bResult=STARButtonPro(m_ButDalta);
		//倾角曲线深度校正
		if(m_bDipDepPro) DIPDepPro();
		//井眼半径曲线深度校正
		if(m_bRADSDepPro) RADSDepPro();
	}
	CDataUnite DataUnite;
	DataUnite.InitPara(m_pDoc);
	m_bResult=DataUnite.Run();
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	return m_bResult;
}

BOOL CPadCorrect::DIPDepPro()
{
	CString ss;
	CFIDIO* PadIO= NULL;
	int RecordNum,OffPoint;
	char s[256],fn[256];
	
	float *Value;
	Value=new float[m_ButtonNum+10];
	int Block=4;
	int i,k;
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);	
	BOOL bUserBreak=FALSE;
	float dep;

	for(i=0;i<m_PadNum;i++)
	{	
		ss.Format ("%d极板倾角曲线深度校正...",i+1);
		pGuage->SetTitle(ss);
		dep=m_StProDep;
		OffPoint=(int)(m_DepDalta[i]/m_Rlev);
		if(OffPoint<1) continue;	
		RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev)-OffPoint;
	
		strcpy(s,m_szBufPath);
		ss.Format (".DIP%iGPT",i);
		strcat(s,ss);
		CFIDIO::GetFileName(s,fn,'D');
		CFile fp;	
		if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
		{
			k=0;				
			while(k<RecordNum)
			{
				fp.Seek ((k+OffPoint)*Block,CFile::begin);
				fp.Read (Value,Block);
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
		else bUserBreak=TRUE;
		fp.Close ();
	}
	delete Value;
	delete pGuage;
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

BOOL CPadCorrect::RADSDepPro()
{
	CString ss;
	CFIDIO* PadIO= NULL;
	int RecordNum,OffPoint;
	char s[256],fn[256];
	
	float *Value;
	Value=new float[m_ButtonNum+10];
	int Block=4;
	int i,k;
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);	
	BOOL bUserBreak=FALSE;
	float dep;

	for(i=0;i<m_PadNum;i++)
	{	
		ss.Format ("%d井眼半径深度校正...",i+1);
		pGuage->SetTitle(ss);
		dep=m_StProDep;
		OffPoint=(int)(m_DepDalta[i]/m_Rlev);
		if(OffPoint<1) continue;	
		RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev)-OffPoint;
	
		strcpy(s,m_szBufPath);
		strcat(s,".RADSGPT");
		CFIDIO::GetFileName(s,fn,'D');
		CFile fp;	
		if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
		{
			k=0;				
			while(k<RecordNum)
			{
				fp.Seek ((k*m_PadNum+i+OffPoint)*Block,CFile::begin);
				fp.Read (Value,Block);
				fp.Seek ((k*m_PadNum+i)*Block,CFile::begin);
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
		else bUserBreak=TRUE;
		fp.Close ();
	}
	delete Value;
	delete pGuage;
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

/*
 *方法：极板深度校正
 */
BOOL CPadCorrect::PadDepPro()
{
	CString ss;
	CFIDIO* PadIO= NULL;
	int RecordNum,OffPoint;
	char s[256],fn[256];
	
	float *Value;
	Value=new float[m_ButtonNum+10];
	int Block=m_ButtonNum*4;
	int i,k;
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);	
	BOOL bUserBreak=FALSE;
	float dep;

	for(i=0;i<m_PadNum;i++)
	{	
		ss.Format ("%i号极板深度校正...",i+1);
		pGuage->SetTitle(ss);
		dep=m_StProDep;
		
		OffPoint=(int)(m_DepDalta[i]/m_Rlev);
		if(OffPoint<1) continue;	
		RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev)-OffPoint;
	
		strcpy(s,m_szBufPath);
		ss.Format (".P%iGPT",i);
		strcat(s,ss);
		CFIDIO::GetFileName(s,fn,'D');
		CFile fp;	
		if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
		{
			k=0;				
			while(k<RecordNum)
			{
				fp.Seek ((k+OffPoint)*Block,CFile::begin);
				fp.Read (Value,Block);
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
			fp.Close ();
		}
		else bUserBreak=TRUE;
		if(bUserBreak) break;
	}
	delete Value;
	delete pGuage;
	if(bUserBreak) return FALSE;
	else           return TRUE;
}


/*
 *Baker Atlas电成像仪电扣深度校正 
 */
BOOL CPadCorrect::STARButtonPro(float dalta)
{
	CString ss;
	CFIDIO* PadIO= NULL;
	float Value;
	int RecordNum,OffSize;
	char s[256],fn[256];
	OffSize=(int)(dalta/m_Rlev);
	if(OffSize<1)
		return TRUE;
	RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev)-OffSize;
	int Block=m_ButtonNum*4;
	OffSize*=Block;
	int i,j,k;
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);	
	BOOL bUserBreak=FALSE;
	float dep;
	for(i=0;i<m_PadNum;i++)
	{	
		ss.Format ("%i号极板电扣深度校正...",i+1);
		pGuage->SetTitle(ss);
		dep=m_StProDep;
		strcpy(s,m_szBufPath);
		ss.Format (".P%iGPT",i);
		strcat(s,ss);
		CFIDIO::GetFileName(s,fn,'D');
		CFile fp;
		if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
		{
			k=0;
			if(i%2==0)
			{
				while(k<RecordNum)
				{
					for(j=1;j<m_ButtonNum;j+=2)
					{				
						fp.Seek (j*4+k*Block+OffSize,CFile::begin);
						fp.Read (&Value,4);
						fp.Seek (j*4+k*Block,CFile::begin);
						fp.Write (&Value,4);	
					}
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
			else
			{
				while(k<RecordNum)
				{
					for(j=0;j<m_ButtonNum;j+=2)
					{				
						fp.Seek (j*4+k*Block+OffSize,CFile::begin);
						fp.Read (&Value,4);
						fp.Seek (j*4+k*Block,CFile::begin);
						fp.Write (&Value,4);	
					}
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
			fp.Close ();
		}
		else bUserBreak=TRUE;
		if(bUserBreak) break;
	}
	delete pGuage;
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

/*
 *Halliburton电成像仪电扣深度校正
 */
BOOL CPadCorrect::EMIButtonPro(float dalta)
{
	CString ss;	
	char s[256],fn[256];
	int OffPoint=(int)(dalta/m_Rlev);
	if(OffPoint<1)	return TRUE;
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev)-OffPoint;

	float *Value;
	Value=new float[m_ButtonNum];
	float *V;
	V=new float[m_ButtonNum];
	int Block=m_ButtonNum*4;
	float fdata;	
	int i,j,k;
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);	
	BOOL bUserBreak=FALSE;
	float dep;
	for(i=0;i<m_PadNum;i++)
	{	
		ss.Format ("%i号极板电扣深度校正...",i+1);
		pGuage->SetTitle(ss);
		dep=m_StProDep;
		strcpy(s,m_szBufPath);
		ss.Format (".P%iGPT",i);
		strcat(s,ss);
		CFIDIO::GetFileName(s,fn,'D');
		CFile fp;
		if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
		{
			k=0;
			if(i%2==0)
			{
				while(k<RecordNum)
				{
/*					for(j=1;j<m_ButtonMid+1;j+=2)
					{				
						fp.Seek (j*4+k*Block+OffPoint*Block,CFile::begin);
						fp.Read (&fdata,4);
						fp.Seek (j*4+k*Block,CFile::begin);
						fp.Write (&fdata,4);	
					}
					for(j=m_ButtonMid+1;j<m_ButtonNum;j+=2)
					{				
						fp.Seek (j*4+k*Block+OffPoint*Block,CFile::begin);
						fp.Read (&fdata,4);
						fp.Seek (j*4+k*Block,CFile::begin);
						fp.Write (&fdata,4);	
					}
*/
					//改写为一个for循环
					for(j=1;j<m_ButtonNum;j+=2)
					{				
						fp.Seek (j*4+k*Block+OffPoint*Block,CFile::begin);
						fp.Read (&fdata,4);
						fp.Seek (j*4+k*Block,CFile::begin);
						fp.Write (&fdata,4);	
					}
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
			else
			{
				while(k<RecordNum)
				{
/*					for(j=0;j<m_ButtonMid+2;j+=2)
					{				
						fp.Seek (j*4+k*Block+OffPoint*Block,CFile::begin);
						fp.Read (&fdata,4);
						fp.Seek (j*4+k*Block,CFile::begin);
						fp.Write (&fdata,4);	
					}
					for(j=m_ButtonMid+2;j<m_ButtonNum;j+=2)
					{				
						fp.Seek (j*4+k*Block+OffPoint*Block,CFile::begin);
						fp.Read (&fdata,4);
						fp.Seek (j*4+k*Block,CFile::begin);
						fp.Write (&fdata,4);	
					}
*/
					//改写为一个for循环
					for(j=0;j<m_ButtonNum;j+=2)
					{				
						fp.Seek (j*4+k*Block+OffPoint*Block,CFile::begin);
						fp.Read (&fdata,4);
						fp.Seek (j*4+k*Block,CFile::begin);
						fp.Write (&fdata,4);	
					}
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
			k=0;
			while(k<RecordNum)
			{
				fp.Seek (k*Block,CFile::begin);
				fp.Read (Value,Block);
/*				for(j=m_ButtonMid+1;j<m_ButtonNum;j++)
					V[j-m_ButtonMid-1]=Value[j];
				for(j=0;j<m_ButtonMid+1;j++)
					V[j+m_ButtonMid]=Value[j];
*/
				for(j=0;j<m_ButtonNum;j++)
					V[j]=Value[j];
				fp.Seek (k*Block,CFile::begin);
				fp.Write (V,Block);	
				k++;
				dep=m_StProDep+m_Rlev*k;
				pGuage->SetDepth(dep);
				if (pGuage->CheckCancelButton()) 
				{
					bUserBreak=TRUE;
					break;
				}
			}
			fp.Close ();
		}
		else bUserBreak=TRUE;
		if(bUserBreak) break;
	}
	delete Value;delete V;
	delete pGuage;
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

/*
 *功能：实现电成像测井资料各极板加速度校正
 *方法：采用各极板相邻两深度测井响应曲线相关对比
 *      识别井下仪器卡停位置
 */
/*
BOOL CPadCorrect::AccelerationPro()
{
	CString ss;
	char s[256],fn[256];
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	int Block=m_ButtonNum*4;
	int i,j,k,k1,m,n;
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);	
	BOOL bUserBreak=FALSE;
	float dep,Semblance;
	float XAve,YAve,Sum,XSum2,YSum2;

	float *Value=NULL;
	Value=new float[m_ButtonNum];
	float *Value1=NULL;
	Value1=new float[m_ButtonNum];

	BOOL bWrite;
	for(n=0;n<m_PadNum;n++)
	{	
		ss.Format ("%i号极板加速度校正...",n+1);
		pGuage->SetTitle(ss);
		strcpy(s,m_szBufPath);
		ss.Format (".P%iGPT",n);
		strcat(s,ss);
		CFIDIO::GetFileName(s,fn,'D');
		CFile fp;
		if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
		{
			fp.Read(Value,Block);
			Sum=0.f;
			for(i=0;i<m_ButtonNum;i++)
				Sum+=Value[i];
			XAve=Sum/m_ButtonNum;
			XSum2=0.f;
			for(i=0;i<m_ButtonNum;i++)
				XSum2+=(Value[i]-XAve)*(Value[i]-XAve);

			k=1;k1=1;
			while(k<RecordNum)
			{		
				dep=m_StProDep+m_Rlev*k;
				pGuage->SetDepth(dep);
				if(pGuage->CheckCancelButton()) 
				{
					bUserBreak=TRUE;
					break;
				}

				bWrite=TRUE;
				fp.Seek(k*Block,CFile::begin);
				fp.Read(Value1,Block);
				k++;
				//求相临深度点相关系数
				//相关系数大于设定值99.8时，仪器已卡停
				//卡停位置K1
				Sum=0.f;
				for(i=0;i<m_ButtonNum;i++)
					Sum+=Value1[i];
				YAve=Sum/m_ButtonNum;
				YSum2=0.f;
				for(i=0;i<m_ButtonNum;i++)
					YSum2+=(Value1[i]-YAve)*(Value1[i]-YAve);
				Semblance=sqrt(XSum2*YSum2);
				if(Semblance>0.f)
				{
					Sum=0.f;
					for(i=0;i<m_ButtonNum;i++)
						Sum+=(Value[i]-XAve)*(Value1[i]-YAve);
					Semblance=Sum/Semblance*100.f;
					if(Semblance>m_MinSemblance)
						bWrite=FALSE;
				}
				else bWrite=FALSE;
				if(bWrite)
				{
					m=k-k1;
					//m为仪器已卡停采样深度点数
					//读取卡停位置K1前m个深度点的数据进行伸长2m深度点处理,替换k1-m致k位置的数据
					if(m>1&&k1>=m)
					{
						float *pData=new float[m*m_ButtonNum];
						fp.Seek((k1-m)*Block,CFile::begin);
						fp.Read(pData,Block*m);
						for(j=0;j<m-1;j++)
						{
							for(i=0;i<m_ButtonNum;i++)
								Value[i]=pData[i+j*m_ButtonNum];
							fp.Seek((k1-m+j*2)*Block,CFile::begin);
							fp.Write(Value,Block);
							//线性插值处理
							for(i=0;i<m_ButtonNum;i++)
								Value[i]=(Value[i]+pData[i+(j+1)*m_ButtonNum])/2;
							fp.Seek((k1-m+j*2+1)*Block,CFile::begin);
							fp.Write(Value,Block);
						}
						//处理最后一点的线性插值
						for(i=0;i<m_ButtonNum;i++)
							Value[i]=pData[i+(m-1)*m_ButtonNum];
						fp.Seek((k-2)*Block,CFile::begin);
						fp.Write(Value,Block);

						for(i=0;i<m_ButtonNum;i++)
							Value[i]=(Value[i]+Value1[i])/2;
						fp.Seek((k-1)*Block,CFile::begin);
						fp.Write(Value,Block);
						delete pData;
					}
					k1=k;
				}	
				::memcpy(Value,Value1,Block);
				XAve=YAve;
				XSum2=YSum2;
			}
		}
		else bUserBreak=TRUE;
		fp.Close ();
		if(bUserBreak) break;
	}
	delete pGuage;delete Value;delete Value1;
	if(bUserBreak) return FALSE;
	else           return TRUE;
}*/

/*
 *功能：实现电成像测井资料各极板加速度校正
 *方法：dltaS=v0*dltaT+0.5*ACC*dltaT*dltaT
 *      v0=zv0+ACC*dltaT
 */
BOOL CPadCorrect::AccelerationPro()
{
	CString ss;
	int i,k,n;
	char s[256],fn[256],fn0[256];
	BOOL bUserBreak=FALSE;
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);

	//读取时间曲线到数组
	strcpy(s,m_szBufPath);
	strcat(s,".TIMEGPT");
	CFIDIO::GetFileName(s,fn,'D');
	CFile fpAC;
	float *timBuf=NULL;
	if(fpAC.Open(fn,CFile::modeRead|CFile::typeBinary))
	{
		timBuf=new float[RecordNum+1];
		fpAC.Read(timBuf,RecordNum*4);
		fpAC.Close ();
	}
	else return FALSE;
	//计算两深点时间差
	float *dt=new float[RecordNum+1];
	for(k=0;k<RecordNum-2;k++)
		dt[k]=timBuf[k]-timBuf[k+1];
	delete timBuf;
	dt[RecordNum-1]=dt[RecordNum-3];
	dt[RecordNum-2]=dt[RecordNum-3];
	for(k=1;k<RecordNum-1;k++)
		if(dt[k]<0.f)
			dt[k]=dt[k-1];
	//时间差曲线进行三点线性滤波
	for(i=0;i<30;i++)
		for(k=1;k<RecordNum-2;k++)
			dt[k]=(dt[k-1]+dt[k]+dt[k+1])/3.f;
	
	//读取加速度曲线到数组
	strcpy(s,m_szBufPath);
	strcat(s,".ZACCGPT");
	CFIDIO::GetFileName(s,fn,'D');
	float *accBuf=NULL;
	if(fpAC.Open(fn,CFile::modeRead|CFile::typeBinary))
	{
		accBuf=new float[RecordNum+1];
		fpAC.Read(accBuf,RecordNum*4);
		fpAC.Close ();
	}
	else
	{
		delete timBuf;delete dt;
		return FALSE;
	}
	//加速度曲线进行三点线性滤波
	for(i=0;i<30;i++)
		for(k=1;k<RecordNum-2;k++)
			accBuf[k]=(accBuf[k-1]+accBuf[k]+accBuf[k+1])/3.f;
	accBuf[0]=accBuf[2];
	accBuf[1]=accBuf[2];
	accBuf[RecordNum-1]=accBuf[RecordNum-3];
	accBuf[RecordNum-2]=accBuf[RecordNum-3];

	//读取井斜曲线到数组
	strcpy(s,m_szBufPath);
	strcat(s,".DEVGPT");
	CFIDIO::GetFileName(s,fn,'D');
	float *devBuf=new float[RecordNum+1];
	if(fpAC.Open(fn,CFile::modeRead|CFile::typeBinary))
	{
		fpAC.Read(devBuf,RecordNum*4);
		fpAC.Close ();
	}
	else
	{
		for(k=0;k<RecordNum;k++)
			devBuf[k]=0.f;
	}
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetTitle("计算实际深度...");
	pGuage->ShowWindow(SW_SHOW);

	double *depBuf=new double[RecordNum+1];
	float dep;
	double zdep,zs,zv0,zac;
	//计算实际深度
	zv0=m_Rlev*1000.f/dt[RecordNum-1];
	depBuf[RecordNum-1]=m_EnProDep;
	zdep=m_EnProDep-m_Rlev;
	depBuf[RecordNum-2]=zdep;
	k=3;
	while(k<=RecordNum)
	{
		dep=m_StProDep+m_Rlev*k;
		pGuage->SetDepth(dep);

		if(accBuf[RecordNum-k]>0.995f||accBuf[RecordNum-k]<0.965f)
			zac=accBuf[RecordNum-k]*9.8f-9.8*cos(devBuf[RecordNum-k]*3.14159/180.f);
		else
			zac=0.f;

		zs=zv0*dt[RecordNum-k]/1000.f+0.5f*zac*dt[RecordNum-k]/1000.f*dt[RecordNum-k]/1000.f;
		if(zs<0.f) zs=0.f;
		zdep-=zs;

		depBuf[RecordNum-k]=zdep;
		zv0=zv0+zac*dt[RecordNum-k]/1000.f;
		if(zv0<0.f) zv0=0.f;
		k++;
	}
	delete dt;delete accBuf;delete devBuf;
/////////////////////////////////////////////////////////////
	//将深度曲线保存并返回
	strcpy(s,m_szBufPath);
	strcat(s,".ZACCGPT");
	CFIDIO::GetFileName(s,fn,'D');
	if(fpAC.Open(fn,CFile::modeWrite|CFile::typeBinary))
	{
		fpAC.Write(depBuf,RecordNum*4);
		fpAC.Close ();
	}
	delete pGuage;delete depBuf; return TRUE;
/////////////////////////////////////////////////////////////
	//各极板加速校正
	int Block=m_ButtonNum*4;
	float *Value=new float[m_ButtonNum];
	for(n=0;n<m_PadNum;n++)
	{	
		strcpy(s,m_szBufPath);
		ss.Format (".P%iGPT",n);strcat(s,ss);
		CFIDIO::GetFileName(s,fn,'D');
		CFile fp;
		if(fp.Open(fn,CFile::modeReadWrite|CFile::typeBinary))
		{
			//数据文件备份
			k=0;
			CFile fp0;
			strcpy(fn0,fn);
			strcat(fn0,"00");
			if(fp0.Open(fn0,CFile::modeCreate|CFile::modeReadWrite|CFile::typeBinary))
			{
				ss.Format ("%i号极板数据备份...",n+1);
				pGuage->SetTitle(ss);
				while(k<RecordNum)
				{		
					fp.Read(Value,Block);
					fp0.Write(Value,Block);
					k++;
				}
				ss.Format ("%i号极板加速度校正...",n+1);
				pGuage->SetTitle(ss);

				k=0;
				zdep=m_StProDep;
				fp.Seek(0L,CFile::begin);
				while(k<RecordNum)
				{		
					dep=m_StProDep+m_Rlev*k;
					pGuage->SetDepth(dep);
					if(pGuage->CheckCancelButton()) 
					{
						bUserBreak=TRUE;
						break;
					}
					zdep=depBuf[k];
					
					fp0.Seek(k*Block,CFile::begin);
					fp0.Read(Value,Block);
					while(dep<zdep)
					{
						fp.Write(Value,Block);
						dep+=m_Rlev;
					}
					k++;
				}
				fp0.Close ();
			}
			else bUserBreak=TRUE;
			fp.Close ();
		}
		else bUserBreak=TRUE;
		if(bUserBreak) break;
	}
	delete pGuage;delete Value;delete depBuf;
	
	if(bUserBreak) return FALSE;
	else           return TRUE;
}
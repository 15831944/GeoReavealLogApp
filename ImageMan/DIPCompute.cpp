// DIPCompute.cpp: implementation of the CDIPCompute class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageMan.h"
#include "DIPCompute.h"
#include "FIDio.h"
#include "LogProcess.h"
#include "DIPCalculate.h"
#include "Guage.h"
#include <process.h>
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDIPCompute::CDIPCompute()
{
	m_WinLength=2.54f;
	m_StepLength=0.1016f;
	m_SearchLength=0.508f;
	m_DangMax=80.f;
	m_RMin=0.5f;
}

CDIPCompute::~CDIPCompute()
{

}

BOOL CDIPCompute:: Run()
{
	DIPPro();
	return TRUE;
}

//�������ϵ��
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

//��Ǽ���
BOOL CDIPCompute::DIPPro()
{
	CFIDIO* pCal = new CFIDIO(m_szBufPath+".RADSGPT");
	if(!pCal->Open())
	{
		AfxMessageBox("�����о�������ʧ��        ");
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
	if(Rlev<= 0.0001f)
	{
		AfxMessageBox("��������С�� 0.0001",MB_OK|MB_ICONERROR);
		delete []XX;
		delete PInCurve;
		delete pLogIn;
		delete pCal;
		return bUserBreak;
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
		AfxMessageBox("WL(����)+2*SL(̽������)����12.5��        ",MB_OK|MB_ICONERROR);
		delete []XX;
		delete PInCurve;
		delete pLogIn;
		delete pCal;
		return bUserBreak;
	}

	float *CalValue=NULL;
	CalValue=new float[PadNum];
	float ddd=m_WinLength/2.f+m_SearchLength;
	if(m_EnProDep+ddd<pLogIn->EndDep) ddd=0;
	//�������ָʾ��
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep-ddd);
	pGuage->Create(CWnd::GetActiveWindow());
	CRect rect;
	// get the current window size and position
	pGuage->GetWindowRect( rect );
	int h=rect.Height ();
	int w=rect.Width ();
	rect.left =(GetSystemMetrics(SM_CXSCREEN)-w)/2;
	rect.top =(GetSystemMetrics(SM_CYSCREEN)-h)/2;
	rect.bottom =rect.top+h;
	rect.right =rect.left+w;
	pGuage->MoveWindow(&rect);
	pGuage->ShowWindow(SW_SHOW);	
	pGuage->SetTitle("�Զ�ʶ���ʳ�������...");
	
	int j,k,Num;	
	float Value[8][5000];
	int POS[50],Np;
	double ll[8],llR[50],temp;
	
	CDIPCalculate dipObj;	
	float   dep=m_StProDep;
	float	dip,dir,dip0,dir0;
	float	dev,daz,az1,rb;
	float 	sigma2;

	int nb=1,StepNum=0;
	CString Filename=m_szWellName.Left (m_szWellName.ReverseFind ('\\'));
	Filename=Filename+"\\HandWork.List";
	if(DetectFileExist(Filename))
		str="";
	else
		str="���,���,ԭʼ�Ƕ�,ԭʼ��λ,����,��Ƕ�,�淽λ\n";
	FILE* fp=NULL;
	fp=fopen(Filename,"a");
	if(fp!=NULL)
	{
		if(str!="")
			fprintf(fp,"%s",str);

		//��ȡ�����ڵĵ絼������
		while(dep<m_EnProDep-ddd)
		{
			for(i=0;i<IWSL;i++)
			{
				pLogIn->Dep=dep+i*Rlev;
				pLogIn->DatIn();
				for(j=0;j<PadNum;j++)
					Value[j][i]=XX[4+j];
				//Ϊ������ǣ�ȡ�������м�ľ�б����λ���뾶
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
			//����I���嵽������Ļ���
			ll[0]=0.;
			double rl=0.f;
			for(i=0; i<PadNum; i++)
				rl+=(double)vr[i];
			dipObj.ll=2.*PI*rl/PadNum;
			for(i=1; i<PadNum; i++)
				ll[i]=ll[i-1]+vr[i]*dipObj.ll/rl;

			Np=0;
			//����������ضԱ�
			for(i=0;i<PadNum;i++)
			{
				//��׼���鸳ֵ
				for(j=ISL;j<ISL+IWL;j++)
					X[j-ISL]=Value[i][j];
				for(k=i+1;k<PadNum;k++)
				{
					//Ŀ�����鸳ֵ
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
			//��������ص�����I�ż���Զ������
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
				//����бУ��
				if(dev>0.5f)
				{
					if(XX[0]>85.f)
					{
						dip0=XX[0]-dip;
						dir0=dir;
					}
					else
					{
						dipObj.m_dip =dip;
						dipObj.m_dir =dir;
						dipObj.m_dip1 =dev;
						dipObj.m_dir1 =daz;
						dipObj.ComputeDIPs ();
						dip0=(float)dipObj.m_dips;
						dir0=(float)dipObj.m_dirs;
					}
				}
				else
				{
					dip0=dip;
					dir0=dir; 
				}
				while(dir0>360.f) 
					dir0-=360.f;
				while(dir0<0.f) 
					dir0+=360.f;
				if((dip<m_DangMax)&&(sigma2<m_GradMin))
				{
					str.Format("%d,%g,%g,%g,%s,%g,%g\n",
							  nb,dep,dip,dir,"����",dip0,dir0);
					fprintf(fp,"%s",str);
					nb++;
				}
			}
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton()) 
			{
				bUserBreak=TRUE;
				break;
			}
			StepNum+=ISTEP;
			dep=m_StProDep+StepNum*Rlev;
		}
		fclose(fp);
	}
	
	delete []XX;
	delete PInCurve;
	delete pLogIn;
		
	delete CalValue;
	delete pCal;
	delete pGuage;
	if(nb>1) return TRUE;
	else     return FALSE;
}


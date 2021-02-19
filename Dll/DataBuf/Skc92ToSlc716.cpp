// Skc92ToSlc716.cpp: implementation of the CSkc92ToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "Skc92ToSlc716.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkc92ToSlc716::CSkc92ToSlc716()
{

}

CSkc92ToSlc716::~CSkc92ToSlc716()
{

}
bool CSkc92ToSlc716::Skc92ToSlc716(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
{
	CString s;
	m_SourceFile=SourceFile; //源文件名   
	m_sObjectPath=sObjectPath;  //输出目录
	m_nCurvesRadio=nCurvesRadio;//过滤开关
	fp=fo;

	pGuage= new CProgressDlg;
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetRange(0,100);
	pGuage->SetStep(1);
	pGuage->OffsetPos(0);
	pGuage->ShowWindow(SW_SHOW);

	CurveIndex.RemoveAll();
	Skc92R=new CSkc92R;
	Slc716W=new CSlc716W;
	bool bResult=Transform();

	//输出转换结果信息
	if(bResult)
		Slc716W->PrintHead(fp);

	////////////////////////////////////
	CurveIndex.RemoveAll();
	delete Slc716W;	Slc716W=NULL;
	delete Skc92R; Skc92R=NULL;
	delete pGuage;  //删除进度条
	return bResult;
}
bool CSkc92ToSlc716::Transform()
{
	if(!IsSKC92(m_SourceFile))
	{
		fprintf(fp,"%s is not SKC92 Format\n\n",m_SourceFile);
		return false;
	}

	CString s;
	//根据输出目录及输入文件名确定输出文件名
	//由于SKC92文件的扩展名一般为一个字符，而主文件名可能一致，
	//所以输出文件名的主名应包括该扩展名
	CString sTempSource=MyGetFileName(m_SourceFile);
	if(MyGetFileExt(m_SourceFile).GetLength()==1)
	{
		sTempSource.Replace('.','_');
	}
	sOutFileName.Format ("%s\\%s.716",m_sObjectPath,sTempSource);
	ModiFileName(sOutFileName); //根据条件决定是否修改文件名
	
	//输出信息
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)sOutFileName);

	if(!Skc92R->Open(m_SourceFile)) 
	{
		fprintf(fp,"\n!!!!链接的数据文件打开失败\n\n");
		return false; 
	}
	Skc92R->ReadHead();
	if(Slc716W->Open(sOutFileName))
	{
		//头信息传递
		Skc92HeadToSlc716Head();
		Slc716W->NumPoint=Skc92R->NumPoint; //一条曲线的采样点数
		Slc716W->NumBlock=(Slc716W->NumPoint-1)/64+1; //数据记录数
		Slc716W->ClearHead(); //清输出文件头
		int nBufSize=Skc92R->NumPoint;

		//查找是否有CCL曲线要输出，有则应将采样间隔加密
		if(Skc92R->bCCLFour)
		{
			Slc716W->Slc716Head.Rlev*=0.25; //CBL测井且存在CCL
			nBufSize*=4; //如果有接箍曲线，采样数变为4倍
		}
		Slc716W->WriteHead(); //输出头信息

		nBufSize=((nBufSize-1)/64+1)*64;
		pGuage->SetRange(0,Slc716W->Slc716Head.NumLog);
		float *buf=new float[nBufSize]; //一条曲线的空间
		for(int i=0;i<nBufSize;i++)buf[i]=-999.25; 
		for(i=0;i<Slc716W->Slc716Head.NumLog;i++)
		{
			pGuage->SetPos(i+1);  //设置进度条当前值
			if (pGuage->CheckCancelButton())
			{
				sp_Break_Status=true;
				break;
			}

			Skc92R->ReadChannel(CurveIndex.GetAt(i),buf);
			if(Skc92R->bCCLFour)
			{
				if(Skc92R->CurveTypes[CurveIndex.GetAt(i)]==4)
				{
					//存在CCL曲线时，常规曲线进行插值
					for(int j=Skc92R->NumPoint-1;j>0;j--)
					{
						buf[4*j]=buf[j]; //将曲线值重新定位
					}
					//插值处理
					for(j=0;j<Skc92R->NumPoint-1;j++)
					{
						for(int k=1;k<4;k++)
						{
							buf[4*j+k]=buf[4*j]+k*0.25f*(buf[4*j+4]-buf[4*j]);
						}
					}
				}
			}
			Slc716W->WriteChannel(i,buf);
		}

		Slc716W->Close();
		delete []buf; buf=0;
	}
	Skc92R->Close(); //关闭输入文件

	return !sp_Break_Status;
}
void CSkc92ToSlc716::Skc92HeadToSlc716Head()
{
	CString s;
	//头信息传递
	sprintf(Slc716W->Slc716Head.WellName,"%-80.80s",Skc92R->Skc92Head.WellName);
	sprintf(Slc716W->Slc716Head.CompanyName,"%-80.80s",Skc92R->Skc92Head.CompanyName);
	if(Skc92R->Skc92Head.Stdep<Skc92R->Skc92Head.Endep)
	{
		Slc716W->Slc716Head.Stdep=Skc92R->Skc92Head.Stdep;
		Slc716W->Slc716Head.Endep=Skc92R->Skc92Head.Endep;
	}
	else
	{
		Slc716W->Slc716Head.Stdep=Skc92R->Skc92Head.Endep;
		Slc716W->Slc716Head.Endep=Skc92R->Skc92Head.Stdep;
	}
	Slc716W->Slc716Head.Rlev=float(fabs(Skc92R->Skc92Head.Rlev));
	Slc716W->Slc716Head.Spcpr=64;
	Slc716W->Slc716Head.Ecc=0;
	Slc716W->Slc716Head.NumLog=0; //设置输出曲线为0，然后从过滤表中查找
	CurveIndex.RemoveAll();	
	for(int i=0;i<Skc92R->Skc92Head.NumLog;i++)
	{
		s.Format ("%-4.4s",Skc92R->Skc92Head.CurveName[i]);
		s.TrimLeft();
		s.TrimRight();
		if(s.CompareNoCase("VDL")==0 || s.CompareNoCase("WAVE")==0)
		{
			continue; //SKC92中的波形曲线不能在SLC716中实现
		}
		bool bYes=false; //是否要该曲线
		if(m_nCurvesRadio==0)
		{
			//采用过滤文件选择曲线
			for(int j=0;j<sp_FilterCurves.GetSize();j++)
			{
				if(s.CompareNoCase(sp_FilterCurves.GetAt(j))==0)
				{
					bYes=true; 
					break;
				}
			}
		}
		else
		{
			bYes=true; //选择所有曲线或常规曲线
		}
		if(bYes)
		{
			CurveIndex.Add(i);
			sprintf(Slc716W->Slc716Head.CurveName[Slc716W->Slc716Head.NumLog],"%-4.4s",Skc92R->Skc92Head.CurveName[i]);
			Slc716W->Slc716Head.NumLog++;
		}
	}
}
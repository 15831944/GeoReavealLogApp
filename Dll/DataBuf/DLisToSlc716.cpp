// DLisToSlc716.cpp: implementation of the CDLisToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "_comext.h"
#include "DLisToSlc716.h"
#include "DlisCurDlg.h"
#include <stdlib.h>
#include "ModiConvDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDLisToSlc716::CDLisToSlc716()
{

}

CDLisToSlc716::~CDLisToSlc716()
{

}
bool CDLisToSlc716::DLisToSlc716(CString SourceFile,CString sObjectPath,int nCurvesRadio,int nConvEncode,float FixRlev,FILE *fo)
{
	CString s;
	m_SourceFile=SourceFile; //源文件名   
	m_sObjectPath=sObjectPath;  //输出目录
	m_nCurvesRadio=nCurvesRadio;//过滤开关
	m_nConvEncode=nConvEncode;
	m_FixRlev=FixRlev;
	fp=fo;
	pGuage= new CProgressDlg;
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetRange(0,100);
	pGuage->SetStep(1);
	pGuage->OffsetPos(0);
	pGuage->ShowWindow(SW_SHOW);
	
	DlisRead=new CDlisRead ;
	Slc716W=new CSlc716W ; //输出类
	bool bResult=Transform();
	delete pGuage;  //删除进度条
	delete Slc716W; Slc716W=NULL;
	delete DlisRead; DlisRead=NULL;
	
	NewCurveName.RemoveAll();

	return bResult;
}
bool CDLisToSlc716::Transform()
{
	CString s;
	if(!IsDLIS(m_SourceFile))
	{
		fprintf(fp,"%s is not DLIS Format\n\n",m_SourceFile);
		return false;
	}
	CString szWellName=MyGetFileTitle(m_SourceFile);
	if(!DlisRead->Open(m_SourceFile,fp)) 
	{
		fprintf(fp,"\n!!!!链接的数据文件打开失败\n\n");
	}
	else
	{
		//设置进度条为一小段，为了视觉
		pGuage->SetRange(0,100);
		pGuage->SetPos(6);
		//浏览整个DLIS文件，取得逻辑文件数及各逻辑文件的位置
		int NumLogFile=DlisRead->GetDlisLogicalFileNumber();
		for(int nf=0;nf<NumLogFile;nf++)
		{
			fprintf(fp,"\nDataBuff: %s\n",(const char*)m_SourceFile);
			fprintf(fp,"第 %d 逻辑文件的起始位置: %u Bytes\n",nf+1,DlisRead->LogFilePos.GetAt(nf));
			pGuage->SetRange(0,NumLogFile);
			pGuage->SetPos(nf+1); //只是为了让进度条不为0
			DlisRead->ScanDlisOneLogicalFile(nf);
			//DlisRead->Prt_LogFileIndex();
			/////////////////////////////////////////////////////////
			CString WellName;
			if(nf==0)
			{
				WellName=szWellName;
			}
			else
			{
				WellName.Format("%s_%d",szWellName,nf+1);
			}
			bool bResult=LogicalToSlc716(WellName,nf);
			if(sp_Break_Status) break; //中断
			//输出转换结果信息
			if(bResult)
			{
				Slc716W->PrintHead(fp);
			}
		}
		DlisRead->Close(); //关闭输入文件
	}
	return !sp_Break_Status;
}
bool CDLisToSlc716::DlisCurvesFilter()
{
	CString s;
	CStringArray sCurve;
	bool bFilter=false;
	int i;
	//根据选择开关进行过滤
	switch(m_nCurvesRadio)
	{
	case 0: //过滤曲线	
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				int k=DlisRead->Frame[i].ChannelID[j];
				CString sCurveName;
				bool bResult=false;
				sCurveName.Format ("%s",DlisRead->Channel[k].Name);
				for(int k1=0;k1<sp_FilterCurves.GetSize();k1++)
				{
					if(sCurveName.CompareNoCase(sp_FilterCurves.GetAt(k1))==0)
					{
						bResult=true;
						break;
					}
				}
				DlisRead->Frame[i].bDecode[j]=bResult;			
			}
		}
		break;
	case 2: //常规曲线
	case 1: //全部曲线
	case 3: //交互选择
	default:
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				int k=DlisRead->Frame[i].ChannelID[j];
				if(DlisRead->Channel[k].Sample==1)
				{
					DlisRead->Frame[i].bDecode[j]=true;
				}
				else
				{
					DlisRead->Frame[i].bDecode[j]=false;
				}
			}
		}
		break;
	}
	//如果无帧数据，则相应帧曲线设置为不解码
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		if(DlisRead->Frame[i].noFrame>0) continue;
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			DlisRead->Frame[i].bDecode[j]=false;
		}
	}
	//根据帧的DIRECTION属性判断是否是连续曲线，对于离散数据，暂时没有对其进行处理
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		//如果本帧曲线不是连续采样，则忽略(将该帧所有曲线设置为不解码)
		if(DlisRead->Frame[i].bDiscrete)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				DlisRead->Frame[i].bDecode[j]=false;
			}
		}
	}
	//设置索引曲线为不解码
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		DlisRead->Frame[i].bDecode[0]=false;
	}
	//只选择常规曲线
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			int k=DlisRead->Frame[i].ChannelID[j];
			if(DlisRead->Channel[k].Sample!=1)
			{
				DlisRead->Frame[i].bDecode[j]=false;
			}
		}
	}

	//设置是否进行交互操作
	switch(m_nCurvesRadio)
	{
	case 3: //交互操作
		bFilter=true;
		break;
	default: //根据曲线名长度判断是否要进行改名等操作
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				if(DlisRead->Frame[i].bDecode[j])
				{
					int k=DlisRead->Frame[i].ChannelID[j];
					//判断长度是否符合要求
					if(strlen(DlisRead->Channel[k].Name)>4)
					{
						bFilter=true;
						break;
					}
					//判断曲线名是否已经存在
					for(int k1=0;k1<sCurve.GetSize();k1++)
					{
						if(sCurve.GetAt(k1).Compare(DlisRead->Channel[k].Name)==0)
						{
							bFilter=true;
							break;				
						}
					}
					//前面曲线暂时保存
					sCurve.Add(DlisRead->Channel[k].Name);
				}
			}
			if(bFilter) break;
		}
		break;
	}
	sCurve.RemoveAll();
	return bFilter;
}
void CDLisToSlc716::SetStartStopRlev()
{
	int nDep;
	CModiConvDlg ModiConvDlg;
	switch(m_nConvEncode)
	{
	case 1: //根据深度计算适当的起止深度及采样间隔
		nDep=int(Slc716W->Slc716Head.Stdep/m_FixRlev+0.95);
		Slc716W->Slc716Head.Stdep=nDep*m_FixRlev;
		nDep=int(Slc716W->Slc716Head.Endep/m_FixRlev);
		Slc716W->Slc716Head.Endep=nDep*m_FixRlev;
		Slc716W->Slc716Head.Rlev=m_FixRlev;
		break;
	case 2: //人工指定起止深度及采样间隔
		ModiConvDlg.m_OldStart=Slc716W->Slc716Head.Stdep;
		ModiConvDlg.m_OldStop=Slc716W->Slc716Head.Endep;
		ModiConvDlg.m_OldRlev=Slc716W->Slc716Head.Rlev;
		if(ModiConvDlg.DoModal()==IDOK)
		{
			Slc716W->Slc716Head.Stdep=ModiConvDlg.m_NewStart;
			Slc716W->Slc716Head.Endep=ModiConvDlg.m_NewStop;
			Slc716W->Slc716Head.Rlev=ModiConvDlg.m_NewRlev;
		}
		break;
	default://不必重新指定：以最小采样间隔及最大井段
		break;
	}
}

bool CDLisToSlc716::LogicalToSlc716(CString WellName,int nf)
{
	CString s;
	//根据输出目录及输入文件名确定输出文件名
	sOutFileName.Format ("%s\\%s.716",m_sObjectPath,WellName);
	ModiFileName(sOutFileName); //根据条件决定是否修改文件名

	SetSlc716HeadFromDlisHead();
	if(NewCurveName.GetSize()==0) 
	{
		fprintf(fp,"\n!!!!没有要处理的曲线\n");
		return false;
	}
	//输出信息
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)sOutFileName);
	SetStartStopRlev(); //根据设置指定深度及采样间隔
	if(!Slc716W->Open(sOutFileName)) return false;
	Slc716W->NumPoint=int((Slc716W->Slc716Head.Endep-Slc716W->Slc716Head.Stdep)/Slc716W->Slc716Head.Rlev+0.5)+1;
	Slc716W->NumBlock=(Slc716W->NumPoint-1)/64+1;
	Slc716W->ClearHead(); //清输出文件头
	Slc716W->WriteHead(); //输出头信息
	DlisRead->ReadLogicalFile(nf); //DLIS数据读到以帧为单位的文件
	//设置进度条范围为解码曲线数
	pGuage->SetRange(0,NewCurveName.GetSize());
	pGuage->SetPos(1);
	int NumLog=-1; //输出曲线序号
	for(int i=0;i<DlisRead->NumFrame;i++)
	{
		if(sp_Break_Status) break;
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			pGuage->SetPos(NumLog+2);
			if (pGuage->CheckCancelButton())
			{
				sp_Break_Status=true;
				break;
			}
			if(!DlisRead->Frame[i].bDecode[j]) continue;
			NumLog++;
			int index=DlisRead->Frame[i].ChannelID[j];
			int Num1=DlisRead->Frame[i].noFrame;
			float *buf1=new float[Num1];
			float *buf2=new float[Slc716W->NumBlock*64];
			for(int k=0;k<Slc716W->NumBlock*64;k++)
			{
				buf2[k]=-999.25;
			}
			DlisRead->ReadCurve(i,j,Num1,buf1);
			float Start1=__min(DlisRead->Frame[i].Index_Start,DlisRead->Frame[i].Index_Stop);
			float Stop1=__max(DlisRead->Frame[i].Index_Start,DlisRead->Frame[i].Index_Stop);
			float Rlev1=float(fabs(DlisRead->Frame[i].Index_Step));
			FloatResample(Start1,Stop1,Rlev1,buf1,Slc716W->Slc716Head.Stdep,Slc716W->Slc716Head.Endep,Slc716W->Slc716Head.Rlev,buf2);
			Slc716W->WriteChannel(NumLog,buf2);
			delete []buf1;	buf1=NULL;
			delete []buf2;	buf2=NULL;
		}
	}
	Slc716W->Close();
	return true;
}
void CDLisToSlc716::SelectChannelFromWin()
{
	CString s;
	CDlisCurDlg DlisCurDlg;
	NewCurveName.RemoveAll();
	int NumLog=0;
	for(int i=0;i<DlisRead->NumFrame;i++)
	{
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			if(DlisRead->Frame[i].bDecode[j])
			{
				NumLog++;
				int k=DlisRead->Frame[i].ChannelID[j];
				CString sNewName=DlisRead->Channel[k].Name; //新名称
				DlisRead->GetNewName(sNewName,i,j,DlisRead->Channel[k].CopyNum,
					DlisRead->Channel[k].Origin,sp_bAddFrameNo,
					sp_bAddOrigin,sp_bAddCopyNum);
				DlisCurDlg.m_ObjectName.Add(sNewName); //输出曲线名;
				DlisCurDlg.m_SourceName.Add(DlisRead->Channel[k].Name); ////输入曲线名
				s.Format ("%u",DlisRead->Channel[k].Origin);
				DlisCurDlg.m_Origin.Add(s); //通道中源
				s.Format ("%u",DlisRead->Channel[k].CopyNum);
				DlisCurDlg.m_CopyNum.Add(s); //通道中拷贝数
				s.Format ("%.1f-%.1f",DlisRead->Frame[i].Index_Start,DlisRead->Frame[i].Index_Stop);
				DlisCurDlg.m_IndexInfo.Add(s); //起止索引
				s.Format ("%.5f",DlisRead->Frame[i].Index_Step);
				DlisCurDlg.m_StepInfo.Add(s); //采样间隔
				s.Format ("%d",i);
				DlisCurDlg.m_FrameNum.Add(s); //帧名
				CString sValue="[";
				for(int k1=0;k1<DlisRead->Channel[k].NumDim;k1++)
				{
					if(k1>0)
					{
						sValue+=",";
					}
					s.Format ("%d",DlisRead->Channel[k].Dimension[k1]);
					sValue+=s;
				}
				sValue+="]";
				DlisCurDlg.m_Dimension.Add(sValue); //维定义
			}
		}
	}
	if(NumLog==0) return ; //没有曲线
	DlisCurDlg.m_nMaxLen=4;
	DlisCurDlg.m_nMaxLog=128;
	DlisCurDlg.m_NumLog=NumLog;
	DlisCurDlg.nTransMode=m_nCurvesRadio;
	//////////////////////////////////////////////////////////////////
	DlisCurDlg.DoModal();
	//////////////////////////////////////////////////////////////////
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			DlisRead->Frame[i].bDecode[j]=false;
		}
	}
	for(i=0;i<DlisCurDlg.m_ObjectName.GetSize();i++)
	{
		CString sFrame=DlisCurDlg.m_FrameNum.GetAt(i);
	
		int iFrame=atol((const char*)sFrame);
		DWORD Origin=atol(DlisCurDlg.m_Origin.GetAt(i));
		BYTE CopyNum=BYTE(atol(DlisCurDlg.m_CopyNum.GetAt(i)));
		CString sCurveName=	DlisCurDlg.m_SourceName.GetAt(i);

		for(int j=0;j<DlisRead->Frame[iFrame].NumCurve;j++)
		{
			int k=DlisRead->Frame[iFrame].ChannelID[j];
			if(DlisRead->Channel[k].Origin != Origin) continue;
			if(DlisRead->Channel[k].CopyNum != CopyNum) continue;
			if(sCurveName.Compare(DlisRead->Channel[k].Name)==0)
			{
				DlisRead->Frame[iFrame].bDecode[j]=true;
				NewCurveName.Add(DlisCurDlg.m_ObjectName.GetAt(i));
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////
	DlisCurDlg.m_ObjectName.RemoveAll(); //输出曲线名
	DlisCurDlg.m_SourceName.RemoveAll(); //输入曲线名
	DlisCurDlg.m_IndexInfo.RemoveAll(); //起止索引
	DlisCurDlg.m_StepInfo.RemoveAll(); //采样间隔
	DlisCurDlg.m_FrameNum.RemoveAll(); //帧名（用帧对象号代替）
	DlisCurDlg.m_Dimension.RemoveAll(); //维定义
	DlisCurDlg.m_Origin.RemoveAll(); //通道中源
	DlisCurDlg.m_CopyNum.RemoveAll(); //通道中拷贝数
}

void CDLisToSlc716::SetSlc716HeadFromDlisHead()
{
	CString s;
	char str[128];
	Slc716W->ResetHead(); //头信息清除
	s.Format ("%-.80s",DlisRead->Origin.WellName);
	s.TrimLeft();
	s.TrimRight();
	sprintf(str,"%-80.80s",s);
	memcpy(Slc716W->Slc716Head.WellName,str,80);
	s.Format ("%-.80s",DlisRead->Origin.Company);
	s.TrimLeft();
	s.TrimRight();
	sprintf(str,"%-80.80s",s);
	memcpy(Slc716W->Slc716Head.CompanyName,str,80);
	Slc716W->Slc716Head.NumLog=0;
	CString sCurveName;
	//////////////////////////////////////////////////////////////
	bool bResult=DlisCurvesFilter();//根据曲线选择开关(m_nCurvesRadio)对曲线进行过滤
	if(bResult)
	{
		//要进行交互操作
		SelectChannelFromWin();
	}
	else
	{//不必进行交互操作
		NewCurveName.RemoveAll();
		for(int i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				if(DlisRead->Frame[i].bDecode[j])
				{
					int k=DlisRead->Frame[i].ChannelID[j];
					NewCurveName.Add(DlisRead->Channel[k].Name);
				}
			}
		}
	}
	////////////////////////////////////////////////////////////////////
	if(NewCurveName.GetSize()==0) return ;
	float MinIndex=-999.25;
	float MaxIndex=-999.25;
	float Step=-999.25;
	bool bFirst=true;
	for(int i=0;i<DlisRead->NumFrame;i++)
	{
		if(DlisRead->Frame[i].noFrame<2) continue;
		if(bFirst)
		{
			MinIndex=__min(DlisRead->Frame[i].Index_Start,DlisRead->Frame[i].Index_Stop);
			MaxIndex=__max(DlisRead->Frame[i].Index_Start,DlisRead->Frame[i].Index_Stop);
			Step=float(fabs(DlisRead->Frame[i].Index_Step));
			bFirst=false;
		}
		else
		{
			MinIndex=__min(MinIndex,__min(DlisRead->Frame[i].Index_Start,DlisRead->Frame[i].Index_Stop));
			MaxIndex=__max( MinIndex,__max(DlisRead->Frame[i].Index_Start,DlisRead->Frame[i].Index_Stop));
			Step=__min(Step,float(fabs(DlisRead->Frame[i].Index_Step)));
		}
	}
	Slc716W->Slc716Head.Stdep=MinIndex;
	Slc716W->Slc716Head.Endep=MaxIndex;
	Slc716W->Slc716Head.Rlev=Step;
	//曲线名
	Slc716W->Slc716Head.NumLog=NewCurveName.GetSize();
	int NumLog=-1; //实际曲线数序号
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			if(DlisRead->Frame[i].bDecode[j])
			{
				NumLog++;
				int index=DlisRead->Frame[i].ChannelID[j]; //曲线在DLIS通道中的索引号
				char str[5];
				sprintf(str,"%-4.4s",(const char*)(NewCurveName.GetAt(NumLog)));
				memcpy(Slc716W->Slc716Head.CurveName[NumLog],str,4);
			}
		}
	}
}

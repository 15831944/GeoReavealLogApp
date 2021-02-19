// LisToFid.cpp: implementation of the CLisToFid class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "LisToFid.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLisToFid::CLisToFid()
{
	
}

CLisToFid::~CLisToFid()
{
	
}
bool CLisToFid::LisToFid(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
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
	
	LisRead=new CLisRead ;
	FidWrite=new CFidWrite ; //输出类
	CurveIndex.RemoveAll();
	bool bResult=Transform();
	delete pGuage;  //删除进度条
	delete FidWrite; FidWrite=NULL;
	delete LisRead;	LisRead=NULL;
	
	CurveIndex.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	return bResult;
}
bool CLisToFid::Transform()
{
	CString s;
	if(!IsLIS(m_SourceFile))
	{
		fprintf(fp,"%s is not LIS Format\n\n",m_SourceFile);
		return false;
	}
	
	CString szWellName=MyGetFileTitle(m_SourceFile);
	if(!LisRead->Open(m_SourceFile)) 
	{
		fprintf(fp,"\n!!!!链接的数据文件打开失败\n\n");
	}
	else
	{
		LisRead->GetTapeProducer(sProducerName,OriginOfdata);
		int NumFile=0;
		while(1)
		{
			fprintf(fp,"\nSource: %s\n",m_SourceFile);
			if(!LisRead->ScanLisOneLogicalFile(fp))break;
			fprintf(fp,"\nLisRead->NumLog=%d\n\n",LisRead->NumLog);
			fflush(fp);
			if(LisRead->NumLog>0)
			{
				NumFile++;
				LisRead->SetLisPos(LisRead->LisLogicalFileStartPos);
				CString WellName;
				if(NumFile==1)
				{
					WellName=szWellName;
				}
				else
				{
					WellName.Format("%s_%d",szWellName,NumFile);
				}
				bool bResult=LogicalToFid(WellName);
				if(sp_Break_Status) break;
				//输出转换结果信息
				if(bResult)
				{
					FidWrite->PrintHead(fp);
				}
				
				//输出信息
				fprintf(fp,"Object: %s\n",(const char*)FidWrite->m_sFidFile);
			}
			//LIS盘文件下一个逻辑文件位置
			LisRead->SetLisPos(LisRead->LisLogicalFileStopPos);
		}
		LisRead->Close(); //关闭输入文件
	}
	return !sp_Break_Status;
}
bool CLisToFid::LogicalToFid(CString WellName)
{
	CString s;
	LisRead->ReadLogicalFile(); //LIS数据读到一个临时文件
	/////////////////////////////////////
	//根据输出目录及输入文件名确定井名和输出路径
	CString sFileTitle=WellName;
	CString sOutWellPath=m_sObjectPath+"\\#"+sFileTitle;
	ModiFileName(sOutWellPath); //根据条件决定是否修改文件名
	sFileTitle=MyGetFileTitle(sOutWellPath);
	CString sWellName=sFileTitle.Mid(1); //去掉“#”
	FidWrite->SetFidFileName(m_sObjectPath,sWellName);

	SetFidHeadFromLisHead();
	if(CurveIndex.GetSize()==0) return false;
	FidWrite->WriteFidInfo();
	pGuage->SetRange(0,CurveIndex.GetSize());
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		pGuage->SetPos(i+1);
		if (pGuage->CheckCancelButton())
		{
			sp_Break_Status=true;
			break;
		}

		int index=CurveIndex.GetAt(i); 
		if(LisRead->Curve[index].NumOfDimension==1)
		{
			DWORD Num=LisRead->NumPoint*LisRead->Curve[index].Samples;
			float *buffer=new float[Num];
			LisRead->ReadCurve(index,Num,buffer);
			FidWrite->WriteGenData(i,Num,buffer);
			delete []buffer;
			buffer=NULL;
		}
		else if(LisRead->Curve[index].NumOfDimension==2)
		{
			if(FidWrite->OpenWave(i))
			{
				DWORD BufSize=FidWrite->FidChannel[i].DimInfo[2].Npw*LisRead->Curve[index].Samples;
				float *buffer=new float[BufSize];
				for(int j=0;j<FidWrite->FidChannel[i].DimInfo[2].Npw;j++)
				{
					buffer[j]=0;
				}
				int Num=LisRead->NumPoint;
				int nUpDown=1; //上测或下测开关
				if(LisRead->StDep>LisRead->EnDep)
				{
					nUpDown=-1;
				}
				for(j=0;j<Num;j++)
				{
					float DepFram=LisRead->StDep+j*LisRead->Rlev;
					LisRead->ReadWave(index,DepFram,buffer);
					for(int k=0;k<LisRead->Curve[index].Samples;k++)
					{
						float dept=DepFram+nUpDown*k*FidWrite->FidChannel[i].DimInfo[0].Rlev;
						int m=k*FidWrite->FidChannel[i].DimInfo[2].Npw;
						FidWrite->WriteWave(dept,&buffer[m]);
					}
				}
				delete []buffer;
				buffer=NULL;
				FidWrite->CloseWave();
			}
		}
	}
	return true;
}

void CLisToFid::SetFidHeadFromLisHead()
{
	CString s;
	sprintf(FidWrite->FidHead.Oil_Field,"%-80.80s","");
	sprintf(FidWrite->FidHead.Region,"%-80.80s","");
	sprintf(FidWrite->FidHead.Company,"%-80.80s","");
	sprintf(FidWrite->FidHead.Well,"%-80.80s","");
	sprintf(FidWrite->FidHead.X_Coordinate,"%-40.40s","");
	sprintf(FidWrite->FidHead.Y_Coordinate,"%-40.40s","");
	sprintf(FidWrite->FidHead.Exformation,"%-80.80s","");
	FidWrite->FidHead.NumLog=0;
	CurveIndex.RemoveAll();
	CString sCurveName;
	if(m_nCurvesRadio==3)
	{	//交互选择
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=15; //曲线名最大长度
		ReCurNamDlg.m_nMaxLog=512; //FID文件允许最多曲线数
		ReCurNamDlg.m_NumLog=LisRead->NumLog; //实际曲线数
		for(int i=0;i<LisRead->NumLog;i++)
		{
			sCurveName.Format ("%-.4s",LisRead->Curve[i].Name);
			sCurveName.TrimLeft();
			sCurveName.TrimRight();
			ReCurNamDlg.m_SourceName.Add(sCurveName);
			ReCurNamDlg.m_ObjectName.Add(sCurveName);
			s.Format ("%-4.4s",LisRead->Curve[i].Units);
			ReCurNamDlg.m_Units.Add(s);
			ReCurNamDlg.m_NumDim.Add(LisRead->Curve[i].NumOfDimension);
		}
		int Result=ReCurNamDlg.DoModal();
		SelCurveName.RemoveAll();
		NewCurveName.RemoveAll();
		NewUnits.RemoveAll();
		for(i=0;i<ReCurNamDlg.m_SourceName.GetSize();i++)
		{
			SelCurveName.Add(ReCurNamDlg.m_SourceName.GetAt(i));
			NewCurveName.Add(ReCurNamDlg.m_ObjectName.GetAt(i));
			NewUnits.Add(ReCurNamDlg.m_Units.GetAt(i));
		}
		ReCurNamDlg.m_SourceName.RemoveAll();
		ReCurNamDlg.m_ObjectName.RemoveAll();
		ReCurNamDlg.m_Units.RemoveAll();
		for(i=0;i<SelCurveName.GetSize();i++)
		{
			for(int j=0;j<LisRead->NumLog;j++)
			{
				sCurveName.Format ("%-.4s",LisRead->Curve[j].Name);
				sCurveName.TrimLeft();
				sCurveName.TrimRight();
				if(sCurveName.CompareNoCase(SelCurveName.GetAt(i))==0)
				{
					CurveIndex.Add(j);
					break;
				}
			}
		}
	}
	else
	{
		NewCurveName.RemoveAll();
		NewUnits.RemoveAll();
		for(int i=0;i<LisRead->NumLog;i++)
		{
			bool bOk=false;
			sCurveName.Format ("%-.4s",LisRead->Curve[i].Name);
			sCurveName.TrimLeft();
			sCurveName.TrimRight();
			if(m_nCurvesRadio==0)
			{
				for(int j=0;j<sp_FilterCurves.GetSize();j++)
				{
					if(sCurveName.CompareNoCase(sp_FilterCurves.GetAt(j))==0)
					{
						bOk=true;
						break;
					}
				}
			}
			else if(m_nCurvesRadio==2)
			{
				if(LisRead->Curve[i].NumX==1)
				{
					bOk=true;
				}
			}
			else if(m_nCurvesRadio==1)
			{
				bOk=true;
			}
			//////////////////////////////
			if(bOk)
			{
				CurveIndex.Add(i);
				s.Format("%-.4s",LisRead->Curve[i].Name);
				NewCurveName.Add(s);
				NewUnits.Add(LisRead->Curve[i].Units);
			}
		}
	}
	FidWrite->FidHead.NumLog=CurveIndex.GetSize();
	
	//根据曲线数(FidWrite->FidHead.NumLog)开辟空间
	FidWrite->InitFidChannel(); 
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		int k=CurveIndex.GetAt(i);
		s.Format("%-.15s",NewCurveName.GetAt(i));
		s.TrimRight();
		sprintf(FidWrite->FidChannel[i].CurveName,"%-.32s",(const char*)s);
		
		FidWrite->FidChannel[i].NumOfDimension=LisRead->Curve[k].NumOfDimension+1;
		
		//设置第一维数据
		strcpy(FidWrite->FidChannel[i].DimInfo[0].Name,"DEP");
		strcpy(FidWrite->FidChannel[i].DimInfo[0].Unit,"m");
		FidWrite->FidChannel[i].DimInfo[0].RepCode=4;
		FidWrite->FidChannel[i].DimInfo[0].CodeLen=4;
		FidWrite->FidChannel[i].DimInfo[0].Nps=1;
		FidWrite->FidChannel[i].DimInfo[0].Npw=0;
		FidWrite->FidChannel[i].DimInfo[0].Rlev=float(fabs(LisRead->Rlev/LisRead->Curve[i].Samples));
		if(LisRead->StDep<LisRead->EnDep)
		{
			FidWrite->FidChannel[i].DimInfo[0].Start=LisRead->StDep;
			FidWrite->FidChannel[i].DimInfo[0].Stop=LisRead->EnDep+(LisRead->Curve[i].Samples-1)*FidWrite->FidChannel[i].DimInfo[0].Rlev;
		}
		else
		{
			FidWrite->FidChannel[i].DimInfo[0].Start=LisRead->EnDep-(LisRead->Curve[i].Samples-1)*FidWrite->FidChannel[i].DimInfo[0].Rlev;;
			FidWrite->FidChannel[i].DimInfo[0].Stop=LisRead->StDep;
		}
		if(LisRead->Curve[k].NumOfDimension==1)
		{
			//设置第二维数据
			sprintf(FidWrite->FidChannel[i].DimInfo[1].Name,"%-.4s",FidWrite->FidChannel[i].CurveName);
			//sprintf(FidWrite->FidChannel[i].DimInfo[1].Unit,"%-4.4s",LisRead->Curve[k].Units);
			sprintf(FidWrite->FidChannel[i].DimInfo[1].Unit,"%-4.4s",NewUnits.GetAt(i));

			if(LisRead->Curve[k].iCodLen==4)
			{
				FidWrite->FidChannel[i].DimInfo[1].RepCode=FID_REPR_FLOAT;
			}
			else
			{
				FidWrite->FidChannel[i].DimInfo[1].RepCode=LisRead->Curve[k].oRepC;
			}
			FidWrite->FidChannel[i].DimInfo[1].CodeLen=WisCodeLen(FidWrite->FidChannel[i].DimInfo[1].RepCode);
			FidWrite->FidChannel[i].DimInfo[1].Nps=1;
			FidWrite->FidChannel[i].DimInfo[1].Npw=1;
			FidWrite->FidChannel[i].DimInfo[1].Start=0;
			FidWrite->FidChannel[i].DimInfo[1].Stop=100;
			FidWrite->FidChannel[i].DimInfo[1].Rlev=0;
		}
		else
		{
			//设置第二维数据
			sprintf(FidWrite->FidChannel[i].DimInfo[1].Name,"%-.4s","T");
			sprintf(FidWrite->FidChannel[i].DimInfo[1].Unit,"%-.4s","us");
			FidWrite->FidChannel[i].DimInfo[1].RepCode=WIS_REPR_FLOAT;
			FidWrite->FidChannel[i].DimInfo[1].CodeLen=WisCodeLen(FidWrite->FidChannel[i].DimInfo[1].RepCode);
			FidWrite->FidChannel[i].DimInfo[1].Nps=LisRead->Curve[k].NumX;
			FidWrite->FidChannel[i].DimInfo[1].Npw=0;
			s.Format("%-4.4s",LisRead->Curve[k].Name);
			s.TrimRight();
			s.MakeUpper();
			if(s.Left(2)==_T("WF"))
			{
				if(sProducerName=="SCHLUMBERGER")
				{
					FidWrite->FidChannel[i].DimInfo[1].Start=200;
					FidWrite->FidChannel[i].DimInfo[1].Rlev=5;
				}
				else if(sProducerName=="HALLIBURTON")
				{
					FidWrite->FidChannel[i].DimInfo[1].Start=0;
					FidWrite->FidChannel[i].DimInfo[1].Rlev=4;
				}
				else
				{
					FidWrite->FidChannel[i].DimInfo[1].Start=0;
					FidWrite->FidChannel[i].DimInfo[1].Rlev=1;
				}
			}
			else
			{
				FidWrite->FidChannel[i].DimInfo[1].Start=1;
				FidWrite->FidChannel[i].DimInfo[1].Rlev=1;
			}
			FidWrite->FidChannel[i].DimInfo[1].Stop=FidWrite->FidChannel[i].DimInfo[1].Start+(FidWrite->FidChannel[i].DimInfo[1].Nps-1)*FidWrite->FidChannel[i].DimInfo[1].Rlev;
			//三维数据
			sprintf(FidWrite->FidChannel[i].DimInfo[2].Name,"%-.4s",FidWrite->FidChannel[i].CurveName);
			//sprintf(FidWrite->FidChannel[i].DimInfo[2].Unit,"%-.4s",LisRead->Curve[k].Units);
			sprintf(FidWrite->FidChannel[i].DimInfo[2].Unit,"%-.4s",NewUnits.GetAt(i));
			FidWrite->FidChannel[i].DimInfo[2].RepCode=LisRead->Curve[k].oRepC; //LisRepCToFid(LisRead->Curve[k].iRepC);
			FidWrite->FidChannel[i].DimInfo[2].CodeLen=WisCodeLen(FidWrite->FidChannel[i].DimInfo[2].RepCode);
			FidWrite->FidChannel[i].DimInfo[2].Nps=LisRead->Curve[k].NumX;
			FidWrite->FidChannel[i].DimInfo[2].Npw=LisRead->Curve[k].NumX;
			FidWrite->FidChannel[i].DimInfo[2].Start=0;
			FidWrite->FidChannel[i].DimInfo[2].Stop=100;
			FidWrite->FidChannel[i].DimInfo[2].Rlev=FidWrite->FidChannel[i].DimInfo[1].Rlev;
		}
	}
}

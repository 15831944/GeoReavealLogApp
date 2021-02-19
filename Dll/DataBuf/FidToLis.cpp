// FidToLis.cpp: implementation of the CFidToLis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "FidToLis.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFidToLis::CFidToLis()
{

}

CFidToLis::~CFidToLis()
{
	CurveItem.RemoveAll();
}
bool CFidToLis::FidToLis(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
{
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

	FidRead=new CFidRead ;
	LisWrite=new CLisWrite ; 
	CurveIndex.RemoveAll();
	bool bResult=Transform();
	delete pGuage;  //删除进度条
	delete FidRead; FidRead=NULL;
	delete LisWrite; LisWrite=NULL;

	return bResult;
}
bool CFidToLis::Transform()
{
	if(!IsFID(m_SourceFile))
	{
		fprintf(fp,"%s is not FID Format\n\n",m_SourceFile);
		return false;
	}
	CString s;
	//根据输出目录及输入文件名确定输出文件名
	LisFileName.Format ("%s\\%s.lis",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	ModiFileName(LisFileName); //根据条件决定是否修改文件名	
	//输出信息
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)LisFileName);

	bool bResult=FidRead->ReadFidInfo(m_SourceFile);
	if(!bResult)
	{
		fprintf(fp,"\n!!!!输入文件打开有错!\n\n");
		return false;
	}
	if(FidRead->FidHead.NumLog<=0)
	{
		fprintf(fp,"\n!!!!曲线没有数据实体\n\n");
		return false;
	}
	/////////////////////////////////////////////
	SelectCurves();  //选择曲线或对曲线改名
	if(FidRead->FidHead.NumLog<=0)
	{
		fprintf(fp,"\n!!!!曲线没有数据实体\n\n");
		return false;
	}
	////////////////////////////////////////////
	CString sInWellPath=MyGetPathName(m_SourceFile);//FID曲线数据所在目录
	CString sInWellName=MyGetFileTitle(m_SourceFile);//FID曲线数据对应井名
	LisWrite->Open(LisFileName); //打开输出LIS文件
	GetLisFileIndexFromFid(); //得到曲线分组信息

	//Prt_CurveItemIndex(); //打印曲线分组信息

	///////////////////////////////////////////////
	SetReelHeader(); //设置卷头记录信息
	LisWrite->WriteRTHT(132); //卷头
	LisWrite->WriteRTHT(130); //带头

	//////////////////////////////////////////////
	int NumFile=CurveItem.GetAt(0);
	for(int nf=0;nf<NumFile;nf++)
	{
		SetFileHeader(nf+1); //设置文件头参数
		LisWrite->WriteFHR(128); //File Header Record
		SetEB(nf);  //设置格式说明块
		Prt_FileInfo(nf); //输出文件信息
		LisWrite->WriteEB(); //写格式说明记录
		WriteDataToLis(nf); //输出数据到LIS文件
		if(sp_Break_Status) break;
		LisWrite->WriteFHR(129); //File Trailer Record
	}
	fprintf(fp,"----共分成 %d 个逻辑文件----\n\n",NumFile);


	//////////////////////////////////////
	LisWrite->WriteRTHT(131); //带尾
	LisWrite->WriteRTHT(133); //卷尾

	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	LisWrite->Close();
	return !sp_Break_Status;
}
void CFidToLis::GetLisFileIndexFromFid()
{
	CString s,s1;
	const double er=0.00001;
	CWordArray FraSize;
	int *CurveID=new int[FidRead->FidHead.NumLog];
	CWordArray *CurveArray=new CWordArray[FidRead->FidHead.NumLog];
	WORD NumItem=0; //组数
	for(int i=0;i<FidRead->FidHead.NumLog;i++)
	{
		CurveID[i]=1;
	}
	for(i=0;i<FidRead->FidHead.NumLog;i++)
	{
		int nDim=FidRead->FidChannel[i].NumOfDimension-1;
		if(nDim<2) continue;
		float sdep=FidRead->FidChannel[i].DimInfo[0].Start;
		float rlev=FidRead->FidChannel[i].DimInfo[0].Rlev;
		WORD CurveSize=FidRead->FidChannel[i].DimInfo[nDim].Npw*FidRead->FidChannel[i].DimInfo[nDim].CodeLen;
		for(int j=0;j<NumItem;j++)
		{
			if((FraSize.GetAt(j)+CurveSize)>32767) continue;
			int k=CurveArray[j].GetAt(0);
			float sdep1=FidRead->FidChannel[k].DimInfo[0].Start;
			float rlev1=FidRead->FidChannel[k].DimInfo[0].Rlev;
			if(fabs(sdep-sdep1)<er && fabs(rlev-rlev1)<er)
			{
				CurveArray[j].Add(i);
				CurveID[i]=0;
				FraSize.SetAt(j,FraSize.GetAt(j)+CurveSize);
				break;
			}
		}
		if(CurveID[i]==1)
		{
			CurveArray[NumItem].Add(i);
			FraSize.Add(CurveSize);
			CurveID[i]=0;
			NumItem++;
		}
	}
	//将常规曲线按起始深度和采样间隔归到波形曲线中
	for(i=0;i<NumItem;i++)
	{
		int k=CurveArray[i].GetAt(0);
		float sdep=FidRead->FidChannel[k].DimInfo[0].Start;
		float rlev=FidRead->FidChannel[k].DimInfo[0].Rlev;
		for(int j=0;j<FidRead->FidHead.NumLog;j++)
		{
			if(CurveID[j]==0) continue; //该曲线已归类
			float sdep1=FidRead->FidChannel[j].DimInfo[0].Start;
			float rlev1=FidRead->FidChannel[j].DimInfo[0].Rlev;
			WORD CurveSize=FidRead->FidChannel[i].DimInfo[1].Npw*FidRead->FidChannel[i].DimInfo[1].CodeLen;
			if((FraSize.GetAt(i)+CurveSize)>32767) continue; //该逻辑文件帧长度已足够大
			if(fabs(sdep-sdep1)<er && fabs(rlev-rlev1)<er)
			{
				FraSize.SetAt(i,FraSize.GetAt(i)+CurveSize);
				CurveArray[i].Add(j);
				CurveID[j]=0;
			}
		}
	}
	//将没有放入波形组的常规曲线单独存放为一个逻辑文件
	bool bHasConv=false;
	for(i=0;i<FidRead->FidHead.NumLog;i++)
	{
		if(CurveID[i]==1)
		{
			bHasConv=true;
			break;
		}
	}
	if(bHasConv)
	{
		for(i=0;i<FidRead->FidHead.NumLog;i++)
		{
			if(CurveID[i]==1)
			{
				CurveArray[NumItem].Add(i);
				CurveID[i]=0;
			}
		}
		NumItem++;
	}
	//将分组结果放入结果变量中
	CurveItem.RemoveAll();
	CurveItem.Add(NumItem);
	for(i=0;i<NumItem;i++)
	{
		int n=CurveArray[i].GetSize();
		CurveItem.Add(n);
		for(int j=0;j<n;j++)
		{
			CurveItem.Add(CurveArray[i].GetAt(j));
		}
	}
	///////////////////////////////////////////////////////
	delete []CurveID; CurveID=NULL;
	FraSize.RemoveAll();
	for(i=0;i<FidRead->FidHead.NumLog;i++)
	{
		CurveArray[i].RemoveAll();
	}
	delete []CurveArray; CurveArray=NULL;
}
void CFidToLis::Prt_CurveItemIndex()
{
	if(CurveItem.GetSize()==0) return;
	int NumFile=CurveItem.GetAt(0);
	int pos=1;
	for(int i=0;i<NumFile;i++)
	{
		fprintf(fp,"\n    --- No. %d Logical File : %d Curves---\n",i+1,CurveItem.GetAt(pos));
		fprintf(fp,"%-3s %-12s %-3s %-9s %-9s %-9s %-8s %-8s\n",
			"No.","Name","Dim","StartDep","EndDep","Rlev","DepUnits","Units");
		for(int j=pos+1;j<pos+1+CurveItem.GetAt(pos);j++)
		{
			int index=CurveItem.GetAt(j);
			int n=j-pos;
			int kDim=FidRead->FidChannel[index].NumOfDimension-1;
			fprintf(fp,"%-3d %-12.32s %-3d %-9G %-9G %-9G %-8.8s %-8.8s\n",
				n,FidRead->FidChannel[index].CurveName,
				kDim,
				FidRead->FidChannel[index].DimInfo[0].Start,
				FidRead->FidChannel[index].DimInfo[0].Stop,
				FidRead->FidChannel[index].DimInfo[0].Rlev,
				FidRead->FidChannel[index].DimInfo[0].Unit,
				FidRead->FidChannel[index].DimInfo[kDim].Unit );
		}
		pos+=CurveItem.GetAt(pos)+1;
	}
	fprintf(fp,"    ----共分成%d个逻辑文件----\n",NumFile);
}

void CFidToLis::SetReelHeader()
{
	sprintf(LisWrite->TapeHead.SerName,"%-6.6s","LIDT");
	sprintf(LisWrite->TapeHead.Nul1,"%-6.6s"," ");
	struct tm *newtime;
	long ltime;
	time( &ltime );
	newtime = gmtime( &ltime );
	sprintf(LisWrite->TapeHead.Date,"%02d/%02d/%02d",newtime->tm_mon+1,newtime->tm_mday,newtime->tm_year%100);
	sprintf(LisWrite->TapeHead.Nul2,"%-2.2s"," ");
	sprintf(LisWrite->TapeHead.OriginData,"%-4.4s","FID");	
	sprintf(LisWrite->TapeHead.Nul3,"%-2.2s"," ");
	sprintf(LisWrite->TapeHead.Name,"%-8.8s"," ");
	sprintf(LisWrite->TapeHead.Nul4,"%-2.2s"," ");
	sprintf(LisWrite->TapeHead.ConnectNum,"%-2.2s","01");	
	sprintf(LisWrite->TapeHead.Nul5,"%-2.2s"," ");
	sprintf(LisWrite->TapeHead.PreConName,"%-8.8s"," ");
	sprintf(LisWrite->TapeHead.Nul6,"%-2.2s"," ");
	sprintf(LisWrite->TapeHead.Comments,"%-74s","SICHUAN LOGGING SERVICES , LIS DATA");
}
void CFidToLis::SetFileHeader(int FileNo)
{
	sprintf(LisWrite->FileHead.Name,"%-6s.%03d","LIDT",FileNo);
	sprintf(LisWrite->FileHead.Nul1,"%-2.2s"," ");
	sprintf(LisWrite->FileHead.SubSerName,"%-6s","LIDT");
	sprintf(LisWrite->FileHead.Version,"%-8s","V 1.00");


	struct tm *newtime;
	long ltime;
	_tzset();
	time( &ltime );
	newtime = localtime( &ltime );
	sprintf(LisWrite->FileHead.Date,"%02d/%02d/%02d",newtime->tm_mon+1,newtime->tm_mday,newtime->tm_year%100);
	LisWrite->FileHead.Nul2=32;
	sprintf(LisWrite->FileHead.MaxPRL,"%5d",MAX_PHYRECORD_LENGTH);
	sprintf(LisWrite->FileHead.Nul3,"%-2s"," ");
	sprintf(LisWrite->FileHead.FileType,"%-2s","LO");
	sprintf(LisWrite->FileHead.Nul4,"%-2s","  ");
	sprintf(LisWrite->FileHead.PreConName,"%-10s"," ");
}

int CFidToLis::GetLogicalCurveIndex(int nf)
{
	if(CurveItem.GetSize()==0) return -1;
	int NumFile=CurveItem.GetAt(0);
	if(NumFile==0) return 0;
	if(nf>=NumFile) return 0;
	int StartPos=1;
	for(int i=0;i<nf;i++)
	{
		StartPos+=CurveItem.GetAt(StartPos)+1;
	}
	int NumLog=CurveItem.GetAt(StartPos); //本逻辑文件的曲线数
	//逻辑文件曲线索引
	CurveIndex.RemoveAll();
	for(i=0;i<NumLog;i++)
	{
		CurveIndex.Add(CurveItem.GetAt(StartPos+i+1));
	}
	return NumLog;
}
unsigned char CFidToLis::FidRepCodeToLis(unsigned char FidRepCode)
{
	unsigned char LisRepCode;
	switch(FidRepCode)
	{
	case FID_REPR_INT:
	case FID_REPR_LONG:
		LisRepCode=73;
		break;
	case FID_REPR_SHORT:
		LisRepCode=79;
		break;
	case FID_REPR_FLOAT:
	case FID_REPR_DOUBLE:
		LisRepCode=68;
		break;
	case FID_REPR_CHAR:
		LisRepCode=56;
		break;
	default:
		LisRepCode=56;
		break;
	}
	return LisRepCode;
}
void CFidToLis::SetEB(int nf)
{ 
	CString s;
	if(LisWrite->Dsbr)
	{
		delete []LisWrite->Dsbr; LisWrite->Dsbr=NULL;
	}
	//找出本逻辑文件的索引起始位置
	LisWrite->NumLog=GetLogicalCurveIndex(nf);
	//找出最小和最大采样间隔
	float MinRlev=float(fabs(FidRead->FidChannel[CurveIndex.GetAt(0)].DimInfo[0].Rlev));
	float MaxRlev=MinRlev;
	char str[255];
	for(int i=1;i<LisWrite->NumLog;i++)
	{
		int k=CurveIndex.GetAt(i);
		if(MinRlev>fabs(FidRead->FidChannel[k].DimInfo[0].Rlev))
		{
			MinRlev=float(fabs(FidRead->FidChannel[k].DimInfo[0].Rlev));
		}
		if(MaxRlev<fabs(FidRead->FidChannel[k].DimInfo[0].Rlev))
		{
			MaxRlev=float(fabs(FidRead->FidChannel[k].DimInfo[0].Rlev));
		}
	}
	//确定帧采样间隔
	int num=int(MaxRlev/MinRlev);
	if(num>127) num=127; //采样数只能占一个字节,最大为255
	LisWrite->Rlev=num*MinRlev;
	LisWrite->Dsbr=new DataSpeBlockRec[LisWrite->NumLog];
	LisWrite->EnVa.DaFraSiz=0;
	for(i=0;i<LisWrite->NumLog;i++)
	{
		int k=CurveIndex.GetAt(i);
		sprintf(str,"%-4.4s",(const char*)(NewCurveName.GetAt(k)));
		memcpy(LisWrite->Dsbr[i].Mnemon,str,4); //曲线名
		memcpy(LisWrite->Dsbr[i].ServID,"SNAM  ",6); //仪器名
		memcpy(LisWrite->Dsbr[i].ServOrd,"        ",8);  //服务号
		int kDim=FidRead->FidChannel[k].NumOfDimension-1;
		
		//sprintf(str,"%-4.4s",FidRead->FidChannel[k].DimInfo[kDim].Unit);
		sprintf(str,"%-4.4s",NewUnits.GetAt(k));
		s=str;
		if(s.CompareNoCase("none")==0)
		{
			memcpy(LisWrite->Dsbr[i].Units,"    ",4); //测量单位
		}
		else
		{
			memcpy(LisWrite->Dsbr[i].Units,str,4); //测量单位
		}
		
		LisWrite->Dsbr[i].APICode=0; //API代码,对于0型可将其分为4部分:测井类型、曲线类型、曲线等级、曲线修饰
		LisWrite->Dsbr[i].FileNumber=nf+1; //文件号,指首次获得数据时的文件号
		LisWrite->Dsbr[i].Nul=0; //两字节，值为0
		LisWrite->Dsbr[i].ProLvl=0; //处理水平(类型0)，对于类型1没用
		LisWrite->Dsbr[i].Samples=int((LisWrite->Rlev+0.5*FidRead->FidChannel[k].DimInfo[0].Rlev)/FidRead->FidChannel[k].DimInfo[0].Rlev); //采样点数
		if(LisWrite->Dsbr[i].Samples<1)
		{
			LisWrite->Dsbr[i].Samples=1;
		}
		LisWrite->Dsbr[i].RepC=FidRepCodeToLis(FidRead->FidChannel[k].DimInfo[kDim].RepCode); //数据代码
		LisWrite->Dsbr[i].Size=FidRead->FidChannel[k].DimInfo[kDim].Npw*LisWrite->Dsbr[i].Samples*LisWrite->GetCodeLength(LisWrite->Dsbr[i].RepC); //样点总长度，指一帧内所占的字节数
		for(int j=0;j<5;j++)
		{
			LisWrite->Dsbr[i].ProInd[j]=0; //处理水平指示器(类型1),对于类型0没用
		}
	}
	/////////////////////////////////////////////////////////////
	LisWrite->EnVa.DaRecTyp=0; //1:数据记录的逻辑类型
	LisWrite->EnVa.DSBTyp=0; //2:数据说明块类型
	//3:数据记录中每帧长度（字节）
	LisWrite->EnVa.DaFraSiz=0;
	for(i=0;i<LisWrite->NumLog;i++)
	{
		LisWrite->EnVa.DaFraSiz+=LisWrite->Dsbr[i].Size;
	}
	LisWrite->EnVa.UDFlag=255; //4:上测/下测标志,1为上测 
	LisWrite->EnVa.DepUnitFlag=255; //5:深度单位标志,1为英尺，255为米,0为时间
	LisWrite->EnVa.DaRefP=0; //6:数据参考点到测量点间的相对距离
	memcpy(LisWrite->EnVa.DaRefUnt,"M   ",4); //7:数据参考点到测量点间的相对距离单位
	LisWrite->EnVa.FraSpace=LisWrite->Rlev; //8:帧之间的采样间距
	memcpy(LisWrite->EnVa.FraUnt,"M   ",4); //9:采样间距的单位
	LisWrite->EnVa.MaxFraRec=1; //一个逻辑记录所包含的帧数
	for(i=0;i<127;i++)
	{	//最多127帧
		if(LisWrite->EnVa.MaxFraRec>=127) break;
		if((LisWrite->EnVa.MaxFraRec+1)*LisWrite->EnVa.DaFraSiz > 1014)break;
		LisWrite->EnVa.MaxFraRec++;
	}
	LisWrite->EnVa.AbsVal=-999.25;	//12:缺省值
	LisWrite->EnVa.DepMod=1; //13:深度记录方式标志
	memcpy(LisWrite->EnVa.DepUnt,"M   ",4);	//14:深度值计数单位
	LisWrite->EnVa.DepRpc=68;	//15:深度值信息代码(深度方式为1时)
	LisWrite->EnVa.DSBSTyp=1;	//16:数据说明块子类型号
}
void CFidToLis::WriteDataToLis(int nf)
{
	LisWrite->NumLog=CurveIndex.GetSize();//逻辑文件曲线数
	for(int i=0;i<LisWrite->NumLog;i++)
	{
		int k=CurveIndex.GetAt(i);
		float sdep=FidRead->FidChannel[k].DimInfo[0].Start;
		float edep=FidRead->FidChannel[k].DimInfo[0].Stop;
		if(i==0)
		{
			LisWrite->StDep=sdep; //逻辑文件起始深度
			LisWrite->EnDep=edep; //逻辑文件结束深度
		}
		else
		{
			if(LisWrite->StDep>sdep)LisWrite->StDep=sdep;
			if(LisWrite->EnDep<edep)LisWrite->EnDep=edep;
		}
	}
	CString sCurveName;
	LisWrite->StartWriteData();

	pGuage->SetRange(0,LisWrite->NumLog);
	for(i=0;i<LisWrite->NumLog;i++)
	{
		pGuage->SetPos(i+1);
		if (pGuage->CheckCancelButton())
		{
			sp_Break_Status=true;
			break;
		}
		int k=CurveIndex.GetAt(i); //FID曲线索
		sCurveName.Format("%-.32s",FidRead->FidChannel[k].CurveName);
		float sdep=FidRead->FidChannel[k].DimInfo[0].Start;
		float edep=FidRead->FidChannel[k].DimInfo[0].Stop;
		float rlev=FidRead->FidChannel[k].DimInfo[0].Rlev;
		int NumR=int((edep-sdep)/rlev+0.5)+1;
		if(FidRead->FidChannel[k].NumOfDimension==2)
		{
			float *buf=new float[NumR];
			FidRead->ReadGenData(sCurveName,NumR,buf);
			LisWrite->WriteCurveData(i,sdep,edep,rlev,NumR,buf);
			delete []buf; buf=NULL;
		}
		else if(FidRead->FidChannel[k].NumOfDimension==3)
		{
			if(FidRead->OpenWave(sCurveName))
			{
				float *buf=new float[FidRead->FidChannel[k].DimInfo[2].Npw];
				for(int j=0;j<NumR;j++)
				{
					float depth=FidRead->FidChannel[k].DimInfo[0].Start+j*FidRead->FidChannel[k].DimInfo[0].Rlev;
					if(!FidRead->ReadWave(depth,buf))
					{
						for(int k1=0;k1<FidRead->FidChannel[k].DimInfo[2].Npw;k1++)
						{
							buf[k1]=0;
						}
					}
					LisWrite->WriteWaveData(i,depth,FidRead->FidChannel[k].DimInfo[2].Npw,buf);
				}
				FidRead->CloseWave();
				delete []buf;
				buf=NULL;
			}
		}
	}
	LisWrite->StopWriteData();
}

void CFidToLis::SelectCurves()
{
	CString s;
	bool *CurveID=new bool[FidRead->FidHead.NumLog];
	for(int i=0;i<FidRead->FidHead.NumLog;i++)
		CurveID[i]=false;
	CString sCurveName;
	switch(m_nCurvesRadio)
	{
	case 0://曲线过滤
		for(i=0;i<FidRead->FidHead.NumLog;i++)
		{
			bool bOk=false;
			sCurveName.Format ("%-.32s",FidRead->FidChannel[i].CurveName);
			sCurveName.TrimRight();
			for(int j=0;j<sp_FilterCurves.GetSize();j++)
			{
				if(sCurveName.CompareNoCase(sp_FilterCurves.GetAt(j))==0)
				{
					bOk=true;
					break;
				}
			}
			if(bOk)
			{
				CurveID[i]=true;
			}
		}
		break;
	case 2://常规曲线
		for(i=0;i<FidRead->FidHead.NumLog;i++)
		{
			if(FidRead->FidChannel[i].NumOfDimension==2)
			{
				CurveID[i]=true;
			}
		}
		break;
	default://交互选择或全部曲线
		for(i=0;i<FidRead->FidHead.NumLog;i++)
		{
			CurveID[i]=true;
		}
		break;
	}
	for(i=FidRead->FidHead.NumLog-1;i>=0;i--)
	{
		if(!CurveID[i])
		{
			for(int j=i;j<FidRead->FidHead.NumLog-1;j++)
			{
				FidRead->FidChannel[j]=FidRead->FidChannel[j+1];
			}
			FidRead->FidHead.NumLog--;
		}
	}
	delete []CurveID; CurveID=NULL;
	///////////////////////////////////////////
	//根据开关设置及曲线名长度决定是否进行交互选择
	bool bRename=false;
	if(m_nCurvesRadio==3)
	{//交互选择
		bRename=true;
	}
	else
	{//判断曲线长度是否超过LIS要求
		for(i=0;i<FidRead->FidHead.NumLog;i++)
		{
			sCurveName.Format("%-.32s",FidRead->FidChannel[i].CurveName);
			sCurveName.TrimRight();
			if(sCurveName.GetLength()>4)
			{
				bRename=true;
				break;
			}
		}
	}
	if(bRename)
	{
		SelCurveName.RemoveAll();
		NewCurveName.RemoveAll();
		NewUnits.RemoveAll();
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=4;
		ReCurNamDlg.m_nMaxLog=1024;
		for(int i=0;i<FidRead->FidHead.NumLog;i++)
		{
			int NumDim=FidRead->FidChannel[i].NumOfDimension-1;
			sCurveName.Format ("%-.32s",FidRead->FidChannel[i].CurveName);
			sCurveName.TrimLeft();
			sCurveName.TrimRight();
			ReCurNamDlg.m_SourceName.Add(sCurveName);
			ReCurNamDlg.m_ObjectName.Add(sCurveName);
			ReCurNamDlg.m_Units.Add(FidRead->FidChannel[i].DimInfo[NumDim].Unit);
			ReCurNamDlg.m_NumDim.Add(NumDim);
		}
		int Result=ReCurNamDlg.DoModal();
		for(i=0;i<ReCurNamDlg.m_SourceName.GetSize();i++)
		{
			SelCurveName.Add(ReCurNamDlg.m_SourceName.GetAt(i));
			NewCurveName.Add(ReCurNamDlg.m_ObjectName.GetAt(i));
			NewUnits.Add(ReCurNamDlg.m_Units.GetAt(i));
		}
		ReCurNamDlg.m_SourceName.RemoveAll();
		ReCurNamDlg.m_ObjectName.RemoveAll();
		ReCurNamDlg.m_Units.RemoveAll();

		CurveID=new bool[FidRead->FidHead.NumLog];
		for(i=0;i<FidRead->FidHead.NumLog;i++)
		{
			CurveID[i]=false;
		}
		for(i=0;i<FidRead->FidHead.NumLog;i++)
		{
			sCurveName.Format ("%-.32s",FidRead->FidChannel[i].CurveName);
			sCurveName.TrimLeft();
			sCurveName.TrimRight();
			for(int j=0;j<SelCurveName.GetSize();j++)
			{
				if(sCurveName.CompareNoCase(SelCurveName.GetAt(j))==0)
				{
					CurveID[i]=true;
				}
			}
		}
		for(i=FidRead->FidHead.NumLog-1;i>=0;i--)
		{
			if(!CurveID[i])
			{
				for(int j=i;j<FidRead->FidHead.NumLog-1;j++)
				{
					FidRead->FidChannel[j]=FidRead->FidChannel[j+1];
				}
				FidRead->FidHead.NumLog--;
			}
		}
		delete []CurveID; CurveID=NULL;
		SelCurveName.RemoveAll();
	}
	else 
	{
		NewCurveName.RemoveAll();
		NewUnits.RemoveAll();
		for(i=0;i<FidRead->FidHead.NumLog;i++)
		{
			int NumDim=FidRead->FidChannel[i].NumOfDimension-1;
			sCurveName.Format("%-.4s",FidRead->FidChannel[i].CurveName);
			NewCurveName.Add(sCurveName);
			NewUnits.Add(FidRead->FidChannel[i].DimInfo[NumDim].Unit);
		}
	}
}
void CFidToLis::Prt_FileInfo(int nf)
{
	fprintf(fp,"    --- 第 %d 个 逻辑文件,共有 %d 条曲线 ---\n",nf+1,LisWrite->NumLog);
	fprintf(fp,"%-3s %-12s %-4s %-3s %-9s %-9s %-9s %-3s %-4s %-4s %-5s %-8s\n",
			"No.","FID","LIS","Dim","StartDep","EndDep","Rlev",
			"Sam","Size","Code","Index","Units");
	for(int i=0;i<LisWrite->NumLog;i++)
	{
		int k=CurveIndex.GetAt(i);
		int nDim=FidRead->FidChannel[k].NumOfDimension-1;
		fprintf(fp,"%-3d",i+1);
		fprintf(fp," %-12.32s",FidRead->FidChannel[k].CurveName);
		fprintf(fp," %-4.4s",LisWrite->Dsbr[i].Mnemon);
		fprintf(fp," %-3d",nDim);
		fprintf(fp," %-9G",FidRead->FidChannel[k].DimInfo[0].Start);
		fprintf(fp," %-9G",FidRead->FidChannel[k].DimInfo[0].Stop);
		fprintf(fp," %-9G",FidRead->FidChannel[k].DimInfo[0].Rlev);
		fprintf(fp," %-3d",LisWrite->Dsbr[i].Samples);
		fprintf(fp," %-4u",LisWrite->Dsbr[i].Size);
		fprintf(fp," %-4u",LisWrite->Dsbr[i].RepC);
		fprintf(fp," %-5s",FidRead->FidChannel[k].DimInfo[0].Unit);
		fprintf(fp," %-8s",NewUnits.GetAt(i));
		fprintf(fp,"\n");
	}
}

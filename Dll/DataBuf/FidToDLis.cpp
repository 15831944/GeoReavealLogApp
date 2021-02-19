// FidToDLis.cpp: implementation of the CFidToDLis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "FidToDLis.h"
#include <math.h>
#include <time.h>
#include "SetDlisParaDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFidToDLis::CFidToDLis()
{
	ItemNum=NULL;
}

CFidToDLis::~CFidToDLis()
{
	if(ItemNum)
	{
		delete []ItemNum; ItemNum=NULL;
	}
}
BYTE CFidToDLis::FidRepCodeToDlis(int RepCode)
{
	BYTE DlisRepC=DLIS_REPC_FSINGL;
	switch(RepCode)
	{
	case FID_REPR_INT:
	case FID_REPR_LONG:
		DlisRepC=DLIS_REPC_SLONG;
		break;
	case FID_REPR_SHORT:
		DlisRepC=DLIS_REPC_SNORM;
		break;
	case FID_REPR_FLOAT:
		DlisRepC=DLIS_REPC_FSINGL;
		break;
	case FID_REPR_DOUBLE:
		DlisRepC=DLIS_REPC_FDOUBL;
		break;
	case FID_REPR_CHAR:
		DlisRepC=DLIS_REPC_SSHORT;
		break;
	default:
		break;
	}
	return DlisRepC;
}
bool CFidToDLis::FidToDlis(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sProducer,FILE *fo)
{
	m_SourceFile=SourceFile; //源文件名   
	m_sObjectPath=sObjectPath;  //输出目录
	m_nCurvesRadio=nCurvesRadio;//过滤开关
	m_sProducer=sProducer;
	fp=fo;
	pGuage= new CProgressDlg;
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetRange(0,100);
	pGuage->SetStep(1);
	pGuage->OffsetPos(0);
	pGuage->ShowWindow(SW_SHOW);

	FidRead=new CFidRead ;
	DlisWrite=new CDlisWrite ; 
	bool bResult=Transform();
	delete pGuage;  //删除进度条
	delete FidRead; 
	FidRead=NULL;
	delete DlisWrite; 
	DlisWrite=NULL;

	return bResult;
}


bool CFidToDLis::Transform()
{
	if(!IsFID(m_SourceFile))
	{
		fprintf(fp,"%s is not FID Format\n\n",m_SourceFile);
		return false;
	}
	CString s;
	//根据输出目录及输入文件名确定输出文件名
	DlisFileName.Format ("%s\\%s.dlis",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	DlisFileName.MakeLower();
	ModiFileName(DlisFileName); //根据条件决定是否修改文件名

	bool bResult=FidRead->ReadFidInfo(m_SourceFile);
	if(!bResult)
	{
		fprintf(fp,"\n!!!!链接的数据文件打开失败\n\n");
		return false;
	}
	if(FidRead->FidHead.NumLog<=0)
	{
		fprintf(fp,"\n!!!!曲线没有数据实体\n\n");
		return false;
	}
	//输出信息
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)DlisFileName);
	/////////////////////////////////////////////
	SelectCurves();  //选择曲线或对曲线改名
	if(FidRead->FidHead.NumLog<=0)
	{
		fprintf(fp,"\n!!!!没有曲线输出!!!\n\n");
		return false;
	}
	////////////////////////////////////////////
	sInWellPath=MyGetPathName(m_SourceFile); //输入井路径
	sInWellName=MyGetFileTitle(m_SourceFile); //得到井名
	sInField=MyGetPathName(sInWellPath); 
	sInField=MyGetFileTitle(sInField); //得到油田名
	GetDlisFDataIndexFromFid(); //通道信息进行分组
	SetDlisChannelsFromFid(); //设置通道信息
	SetDlisFrameFromFid(); //设置DLIS帧对象属性
	SetAxisFromFid(); //设置DLIS轴对象属性
	Prt_FdataItemInfo(); //打印帧分组信息
	SetStoUnitLab(); //设置DLIS存储标志
	SetFileHeader(); //设置文件头信息
	SetOrigin(); //设置源信息
	SetDlisParameter(); //修改或补充参数
	DlisWrite->Open(DlisFileName,fp); //打开DLIS文件
	DlisWrite->SetTrailLengthFlag(); //记录尾长度设置
	DlisWrite->WriteStoUnitLab(); //写DLIS存储标志
	DlisWrite->WriteFileHeader(); //写文件头信息记录
	DlisWrite->WriteOrigin(); //写源信息记录
	DlisWrite->WriteParameter(); //写静态参数记录
	DlisWrite->WriteChannel(); //写通道信息记录
	DlisWrite->WriteFrame(); //写帧信息记录
	DlisWrite->WriteAxis(); //写轴信息记录
	FidDataToDlis(); //输出数据
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	DlisWrite->Close(); //关闭DLIS文件
	DlisWrite->Prt_Header();
	return !sp_Break_Status;
}

void CFidToDLis::SelectCurves()
{
	CString s;
	bool *CurveID=new bool[FidRead->FidHead.NumLog];
	for(int i=0;i<FidRead->FidHead.NumLog;i++)
	{
		CurveID[i]=false;
	}
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
	//根据开关设置决定是否进行交互选择
	if(m_nCurvesRadio==3)
	{
		SelCurveName.RemoveAll();
		NewCurveName.RemoveAll();
		NewUnits.RemoveAll();
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=255; //最大曲线长度
		ReCurNamDlg.m_nMaxLog=99999; //最多曲线数
		for(int i=0;i<FidRead->FidHead.NumLog;i++)
		{
			int NumDim=FidRead->FidChannel[i].NumOfDimension-1;
			sCurveName.Format ("%-.32s",FidRead->FidChannel[i].CurveName);
			ReCurNamDlg.m_SourceName.Add(sCurveName);
			ReCurNamDlg.m_Units.Add(FidRead->FidChannel[i].DimInfo[NumDim].Unit);
			sCurveName.TrimLeft();
			sCurveName.TrimRight();
			sCurveName.Remove(' ');
			ReCurNamDlg.m_ObjectName.Add(sCurveName);//预置新曲线名
			ReCurNamDlg.m_NumDim.Add(NumDim);
		}
		int Result=ReCurNamDlg.DoModal();
		for(i=0;i<ReCurNamDlg.m_SourceName.GetSize();i++)
		{
			//原名称
			SelCurveName.Add(ReCurNamDlg.m_SourceName.GetAt(i));
			//新名称
			s=ReCurNamDlg.m_ObjectName.GetAt(i);
			s.MakeUpper();
			NewCurveName.Add(s);
			//单位
			NewUnits.Add(ReCurNamDlg.m_Units.GetAt(i));

		}
		ReCurNamDlg.m_SourceName.RemoveAll();
		ReCurNamDlg.m_ObjectName.RemoveAll();
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
		//没有交互操作
		NewCurveName.RemoveAll();
		NewUnits.RemoveAll();
		for(i=0;i<FidRead->FidHead.NumLog;i++)
		{
			sCurveName.Format("%-.32s",FidRead->FidChannel[i].CurveName);
			sCurveName.MakeUpper();
			NewCurveName.Add(sCurveName);
			int NumDim=FidRead->FidChannel[i].NumOfDimension-1;
			NewUnits.Add(FidRead->FidChannel[i].DimInfo[NumDim].Unit);
		}
	}
}

void CFidToDLis::GetDlisFDataIndexFromFid()
{
	CString s;
	typedef struct tagCurve
	{
		float Start;
		float Stop;
		float Rlev;
	}_Curve;
	const double er=0.00001;
	NumItem=0; //组数
	if(FidRead->FidHead.NumLog<1) return;
	_Curve *Curve=new _Curve[FidRead->FidHead.NumLog];
	if(ItemNum)
	{
		delete []ItemNum; ItemNum=NULL;
	}
	ItemNum=new int[FidRead->FidHead.NumLog];
	for(int i=0;i<FidRead->FidHead.NumLog;i++)
	{
		float Start=FidRead->FidChannel[i].DimInfo[0].Start;
		float Stop=FidRead->FidChannel[i].DimInfo[0].Stop;
		float Rlev=FidRead->FidChannel[i].DimInfo[0].Rlev;
		bool bFind=false;
		for(int j=0;j<NumItem;j++)
		{
			if(fabs(Start-Curve[j].Start)<er && fabs(Stop-Curve[j].Stop)<er	&&  fabs(Rlev-Curve[j].Rlev)<er)
			{
				bFind=true;
				ItemNum[i]=j;
				break;
			}
		}
		if(!bFind)
		{
			Curve[NumItem].Start=Start;
			Curve[NumItem].Stop=Stop;
			Curve[NumItem].Rlev=Rlev;
			ItemNum[i]=NumItem;
			NumItem++;
		}
	}
	delete []Curve; Curve=NULL;
}
void CFidToDLis::Prt_FdataItemInfo()
{
	for(int i=0;i<NumItem;i++)
	{
		fprintf(fp,"第%2d组: ",i+1);
		int nc=0;
		for(int j=0;j<FidRead->FidHead.NumLog;j++)
		{
			if(ItemNum[j]==i)
			{
				fprintf(fp," %-4.32s",FidRead->FidChannel[j].CurveName);
				nc++;
				if(nc%10==0)
				{
					fprintf(fp,"\n%-8s","");
				}
			}
		}
		fprintf(fp,"\n");
	}
}
void CFidToDLis::SetStoUnitLab()
{
	char str[256];
//	DlisWrite->MaxPhyLen=58;//最大记录长度
	memcpy(DlisWrite->StoUnitLab.SeqNum,"   1",4); //储存序号
	memcpy(DlisWrite->StoUnitLab.DLISVer,"V1.00",5); //版本号
	memcpy(DlisWrite->StoUnitLab.Stru,"RECORD",6); //存储单结构
	sprintf(str,"%5d",DlisWrite->MaxPhyLen); 
	memcpy(DlisWrite->StoUnitLab.MaxRecLen,str,5); 
	sprintf(str,"%-60.60s","FID to DLIS Conversion"); //存储单标志
	memcpy(DlisWrite->StoUnitLab.StoSetIden,str,60);
}
void CFidToDLis::SetFileHeader()
{
	sprintf(DlisWrite->FileHeader.SequenceNumber,"%10.10s","1");//序列号
	sprintf(DlisWrite->FileHeader.ID,"%-65.65s","FID.001");//ID号
}
void CFidToDLis::SetOrigin()
{
	CString s;
	strcpy(DlisWrite->Origin.FileID,"FID.001");//FILE-ID
	strcpy(DlisWrite->Origin.FileSetName,"FID-TO-DLIS-CONVERSION");//FILE-SET-NAME
	strcpy(DlisWrite->Origin.FileSetNumber,"1");//FILE-SET-NUMBER
	strcpy(DlisWrite->Origin.FileNumber,"1"); //FILE-NUMBER
	strcpy(DlisWrite->Origin.FileType,"RE-FMT"); //FILE-TYPE
	strcpy(DlisWrite->Origin.Product,"GeoReveal "); //PRODUCT
	strcpy(DlisWrite->Origin.Version,"1.0"); //VERSION
	strcpy(DlisWrite->Origin.Programs,"FID-TO-DLIS"); //PROGRAMS
	CTime t=CTime::GetCurrentTime(); //取当前系统日期和时间
	sprintf(DlisWrite->Origin.CreationTime,"YEAR=%d MONTH=%d DAY=%d HOUR=%d MINUTE=%d SECOND=%d",
		t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(),t.GetMinute(),
		t.GetSecond()); //CREATION-TIME
	strcpy(DlisWrite->Origin.OrderNumber,""); //ORDER-NUMBER
	strcpy(DlisWrite->Origin.DescentNumber,""); //DESCENT-NUMBER
	strcpy(DlisWrite->Origin.RunNumber,""); //RUN-NUMBER
	strcpy(DlisWrite->Origin.WellID,""); //WELL-ID
	strcpy(DlisWrite->Origin.WellName,FidRead->FidHead.Well); //WELL-NAME
	strcpy(DlisWrite->Origin.FieldName,FidRead->FidHead.Oil_Field); //FIELD-NAME
	s=m_sProducer;
	s.TrimLeft();
	s.TrimRight();
	int sl=(m_sProducer).GetLength();
	if(sl>6)
	{
		strcpy(DlisWrite->Origin.ProducerName,s.Left(sl-6));//PRODUCER-NAME
		strcpy(DlisWrite->Origin.ProducerCode,s.Mid(sl-4,3));//PRODUCER-CODE
		strcpy(DlisWrite->Origin.NameSpaceName,""); //NAME-SPACE-NAME
	}
	else
	{
		strcpy(DlisWrite->Origin.ProducerName,"Schlumberger"); 
		strcpy(DlisWrite->Origin.ProducerCode,"440"); 
		strcpy(DlisWrite->Origin.NameSpaceName,"SLB"); //NAME-SPACE-NAME
	}
	strcpy(DlisWrite->Origin.Company,"SiChuan Petroleum Administration"); //COMPANY
	strcpy(DlisWrite->Origin.NameSpaceVersion,"0"); //NAME-SPACE-VERSION
}

void CFidToDLis::SetDlisParameter()
{
	sInWellName=MyGetFileTitle(m_SourceFile); //得到的井名
	sInField=MyGetPathName(m_SourceFile); 
	sInField=MyGetFileTitle(sInField); //得到的油田名
	CString s;
	BOOL b1=IsAsciiString(DlisWrite->Origin.Company);
	if(!b1)
	{
		strcpy(DlisWrite->Origin.Company,"SiChuan Petroleum Administration"); //COMPANY
	}
	BOOL b2=IsAsciiString(DlisWrite->Origin.FieldName);	
	if(!b2)
	{
		strcpy(DlisWrite->Origin.FieldName,sInField); //得到油田名
	}
	BOOL b3=IsAsciiString(DlisWrite->Origin.WellName);
	if(!b3)
	{
		strcpy(DlisWrite->Origin.WellName,sInWellName); //得到井名
	}
	if(sp_bAllIgnore) return; //忽略修改

/*	BOOL bModify=b1&b2&b3;
	if(!bModify)
	{
		CSetDlisParaDlg SetDlisParaDlg;
		SetDlisParaDlg.m_FileName=m_SourceFile;
		SetDlisParaDlg.m_Company=DlisWrite->Origin.Company;
		SetDlisParaDlg.m_Field=DlisWrite->Origin.FieldName;
		SetDlisParaDlg.m_Well=DlisWrite->Origin.WellName;
		SetDlisParaDlg.m_Field.Remove('[');
		SetDlisParaDlg.m_Field.Remove(']');
		SetDlisParaDlg.m_Well.Remove('[');
		SetDlisParaDlg.m_Well.Remove(']');
		SetDlisParaDlg.m_DefaultVal=DlisWrite->Origin.DefaultVal;
		SetDlisParaDlg.DoModal();
		//不论是确定还是取消均重新取值,确定为人工修正，取消则修正为缺省值
		strcpy(DlisWrite->Origin.Company,(const char*)SetDlisParaDlg.m_Company);
		strcpy(DlisWrite->Origin.FieldName,(const char*)SetDlisParaDlg.m_Field);
		strcpy(DlisWrite->Origin.WellName,(const char*)SetDlisParaDlg.m_Well);
		DlisWrite->Origin.DefaultVal=SetDlisParaDlg.m_DefaultVal;
		sp_bAllIgnore=SetDlisParaDlg.m_bAllIgnore;
	}*/
}
void CFidToDLis::SetDlisChannelsFromFid()
{
	CString s;
	DlisWrite->SetFrameNumber(NumItem);
	DlisWrite->SetChannelNumber(FidRead->FidHead.NumLog);
	for(int i=0;i<FidRead->FidHead.NumLog;i++)
	{
		DlisWrite->Channel[i].Origin=1;  //对象O=1,C=0
		DlisWrite->Channel[i].CopyNum=0; //对象O=1,C=0
		int NumDim=FidRead->FidChannel[i].NumOfDimension-1;
		strcpy(DlisWrite->Channel[i].Name,NewCurveName.GetAt(i));

		strcpy(DlisWrite->Channel[i].LongName,"");
		strcpy(DlisWrite->Channel[i].Properties,"");
		strcpy(DlisWrite->Channel[i].Units,NewUnits.GetAt(i));
		if(stricmp(DlisWrite->Channel[i].Units,"none")==0)
		{
			strcpy(DlisWrite->Channel[i].Units,"");
		}
		DlisWrite->Channel[i].RepCode=FidRepCodeToDlis(FidRead->FidChannel[i].DimInfo[NumDim].RepCode);
		if(NumDim==1)
		{
			DlisWrite->Channel[i].NumDim=1; //维数
			DlisWrite->Channel[i].Dimension[0]=1; //维元素
		}
		else if(NumDim==2)
		{
			DlisWrite->Channel[i].NumDim=1; //维数
			DlisWrite->Channel[i].Dimension[0]=FidRead->FidChannel[i].DimInfo[1].Nps; //维元素
		}
		else if(NumDim==3)
		{
			DlisWrite->Channel[i].NumDim=2; //维数
			DlisWrite->Channel[i].Dimension[0]=FidRead->FidChannel[i].DimInfo[1].Nps; //维元素
			DlisWrite->Channel[i].Dimension[1]=FidRead->FidChannel[i].DimInfo[2].Nps; //维元素
		}
	}
}
void CFidToDLis::SetDlisFrameFromFid()
{
	CString s;
	for(int i=0;i<NumItem;i++)
	{
		DlisWrite->Frame[i].Origin=1;//源(对象)
		DlisWrite->Frame[i].CopyNum=0;//拷贝数(对象)
		sprintf(DlisWrite->Frame[i].Name,"B%d",i+1);//帧名(对象名)
		strcpy(DlisWrite->Frame[i].Index_Type,"BOREHOLE-DEPTH"); //索引类型
		strcpy(DlisWrite->Frame[i].Direction,"INCREASING"); //测井方向
		strcpy(DlisWrite->Frame[i].IndexUnits,FidRead->FidChannel[i].DimInfo[0].Unit); //索引单位 
		int NumCurve=0; //本帧曲线数
		for(int k=0;k<FidRead->FidHead.NumLog;k++)
		{
			if(ItemNum[k]==i)
			{
				DlisWrite->Frame[i].Index_Start=FidRead->FidChannel[k].DimInfo[0].Start; //索引最小值
				DlisWrite->Frame[i].Index_Stop=FidRead->FidChannel[k].DimInfo[0].Stop; //索引最大值
				DlisWrite->Frame[i].Index_Step=FidRead->FidChannel[k].DimInfo[0].Rlev; //步长
				DlisWrite->Frame[i].ChannelID[NumCurve]=k;
				NumCurve++;
			}
		}
		DlisWrite->Frame[i].NumCurve=NumCurve; //本帧曲线数
	}
}
void CFidToDLis::SetAxisFromFid()
{
	CString s;
	int NumAxis=0;
	for(int i=0;i<FidRead->FidHead.NumLog;i++)
	{
		int NumDim=FidRead->FidChannel[i].NumOfDimension-1;
		if(NumDim<2) continue;
		for(int j=1;j<NumDim;j++)
		{
			if(fabs(FidRead->FidChannel[i].DimInfo[j].Rlev)>0)
			{
				NumAxis++;
			}
		}
	}
	if(NumAxis==0) return;
	DlisWrite->SetDlisAxisNumber(NumAxis);
	int nAx=0;
	for(i=0;i<FidRead->FidHead.NumLog;i++)
	{
		int NumDim=FidRead->FidChannel[i].NumOfDimension-1;
		DlisWrite->Channel[i].nAxis=NumDim-1; //曲线轴维数
		if(NumDim<2) continue;
		DlisWrite->Axis[nAx].NumC=0; //坐标数
		for(int j=1;j<NumDim;j++)
		{
			if(fabs(FidRead->FidChannel[i].DimInfo[j].Rlev)>0)
			{
				DlisWrite->Axis[nAx].Origin=1; //源
				DlisWrite->Axis[nAx].CopyNum=0; //拷贝数
				sprintf(DlisWrite->Axis[nAx].Name,"%d",1000+nAx); //轴名，对应于对象名
				s.Format("%s",FidRead->FidChannel[i].DimInfo[j].Name);
				if(s.CompareNoCase("T")==0 || s.CompareNoCase("TIME")==0)
				{
					strcpy(DlisWrite->Axis[nAx]. AxisID,"MICRO_TIME");//轴ID号:C=1,R=IDENT
				}
				else
				{
					strcpy(DlisWrite->Axis[nAx].AxisID,"SENSOR_OFFSET");//轴ID号:C=1,R=IDENT
				}
				DlisWrite->Axis[nAx].NumC=1; //坐标数
				DlisWrite->Axis[nAx].Coordinates[0]=FidRead->FidChannel[i].DimInfo[j].Start; //坐标
				DlisWrite->Axis[nAx].Spacing=FidRead->FidChannel[i].DimInfo[j].Rlev; //间隔
				strcpy(DlisWrite->Axis[nAx].Units,FidRead->FidChannel[i].DimInfo[j].Unit); //坐标单位
				DlisWrite->Channel[i].Axis[j-1].Origin=1; //轴：R=OBNAME	
				DlisWrite->Channel[i].Axis[j-1].CopyNum=0; //轴：R=OBNAME	
				strcpy(DlisWrite->Channel[i].Axis[j-1].Name,DlisWrite->Axis[nAx].Name); //轴：R=OBNAME	
				nAx++;
			}
			else
			{
				//由于增量为0,设置该轴名为空
				DlisWrite->Channel[i].Axis[j-1].Origin=1; //轴：R=OBNAME
				DlisWrite->Channel[i].Axis[j-1].CopyNum=0; //轴：R=OBNAME
				strcpy(DlisWrite->Channel[i].Axis[j-1].Name,""); //轴：R=OBNAME
			}
		}
	}
}
void CFidToDLis::FidDataToDlis()
{
	CString s;
	for(int nItem=0;nItem<NumItem;nItem++)
	{
		if(sp_Break_Status) break;
		int NumCurve=DlisWrite->Frame[nItem].NumCurve;
		float Start=DlisWrite->Frame[nItem].Index_Start;
		float Stop=DlisWrite->Frame[nItem].Index_Stop;
		float Step=DlisWrite->Frame[nItem].Index_Step;
		int NumR=int((Stop-Start)/Step+0.5)+1;
		DlisWrite->StartWriteData(nItem);
		pGuage->SetRange(0,NumCurve);
		for(int i=0;i<NumCurve;i++)
		{
			pGuage->SetPos(i+1);
			if (pGuage->CheckCancelButton())
			{
				sp_Break_Status=true;
				break;
			}

			int index=DlisWrite->Frame[nItem].ChannelID[i];
			CString sCurveName;
			sCurveName.Format("%-.32s",FidRead->FidChannel[index].CurveName);
			if(FidRead->FidChannel[index].NumOfDimension==2)
			{
				float *buf=new float[NumR];
				FidRead->ReadGenData(sCurveName,NumR,buf);
				for(int k=0;k<NumR;k++)
				{
					if(fabs(buf[k]+9999.0)<0.01) buf[k]=DlisWrite->Origin.DefaultVal;
					if(fabs(buf[k]+999.25)<0.01) buf[k]=DlisWrite->Origin.DefaultVal;
				}
				DlisWrite->WriteCurveData(nItem,i,NumR,buf);
				delete []buf; buf=NULL;
			}
			else
			{
				FidRead->OpenArray(sCurveName);
				int Count=DlisWrite->Channel[index].Sample;
				float *vFloat=new float[Count];
				for(int j=0;j<NumR;j++)
				{
					float depth=Start+j*Step;
					if(!FidRead->ReadArray(depth,vFloat)) break;
					DlisWrite->WriteArrayData(nItem,i,j,Count,vFloat);
				}
				FidRead->CloseArray();
				delete []vFloat ; vFloat=NULL;
			}
		}
		DlisWrite->StopWriteData(nItem);
	}
}

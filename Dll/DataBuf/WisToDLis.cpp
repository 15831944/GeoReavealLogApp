// WisToDLis.cpp: implementation of the CWisToDLis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "WisToDLis.h"
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

CWisToDLis::CWisToDLis()
{
	ItemNum=NULL;
}

CWisToDLis::~CWisToDLis()
{
	if(ItemNum)
	{
		delete []ItemNum; ItemNum=NULL;
	}
}

bool CWisToDLis::WisToDLis(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sProducer,FILE *fo)
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

	WisRead=new CWisRead ;
	DlisWrite=new CDlisWrite ; 
	bool bResult=Transform();
	delete pGuage;  //删除进度条
	delete WisRead; WisRead=NULL;
	delete DlisWrite; DlisWrite=NULL;

	return bResult;
}


bool CWisToDLis::Transform()
{
	if(!IsWIS(m_SourceFile))
	{
		fprintf(fp,"%s is not WIS Format\n\n",m_SourceFile);
		return false;
	}
	CString s;
	//根据输出目录及输入文件名确定输出文件名
	DlisFileName.Format ("%s\\%s.dlis",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	DlisFileName.MakeLower();
	ModiFileName(DlisFileName); //根据条件决定是否修改文件名

	if(!WisRead->Open(m_SourceFile))
	{
		return false;
	}
	WisRead->Open(m_SourceFile);
	WisRead->GetHead();
	WisRead->GetObjectEntry();
	if(WisRead->WisHead.ObjectNumber<1)
	{
		return false;
	}

	SelectCurves();//选择曲线或对曲线改名
	if(CurveIndex.GetSize()==0)
	{
		WisRead->Close();
		fprintf(fp,"\n!!!!没有曲线输出!!!\n\n");
		return false;
	}
	//输出信息
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)DlisFileName);
	////////////////////////////////////////////
	GetDlisFDataIndexFromWis(); //对WIS通道信息进行分组
	Prt_FdataItemInfo(); //打印帧分组信息
	SetDlisChannelsFromWis(); //根据WIS信息设置通道信息
	SetDlisFrameFromWis(); //设置DLIS帧对象属性
	SetAxisFromWis(); //设置DLIS轴对象属性
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
	WisDataToDlis(); //输出数据
	DlisWrite->Close(); //关闭DLIS文件
	DlisWrite->Prt_Header();
	CurveIndex.RemoveAll(); //源曲线索引号
	SelCurveName.RemoveAll(); //选择曲线名表
	NewCurveName.RemoveAll(); //新曲线名表
	NewUnits.RemoveAll(); ////新单位名表
	return !sp_Break_Status;
}
BYTE CWisToDLis::WisRepCodeToDlis(int RepCode)
{
	BYTE DlisRepC=DLIS_REPC_FSINGL;
	switch(RepCode)
	{
	case WIS_REPR_INT://4字节有符号整型
	case WIS_REPR_LONG:   //	4字节有符号长整型数
		DlisRepC=DLIS_REPC_SLONG;
		break;
	case WIS_REPR_SHORT:  //	2字节有符号短整型数
		DlisRepC=DLIS_REPC_SNORM;
		break;
	case WIS_REPR_FLOAT:  //	4字节IEEE单精度浮点数
		DlisRepC=DLIS_REPC_FSINGL;
		break;
	case WIS_REPR_DOUBLE: //	8字节IEE双精度浮点数
		DlisRepC=DLIS_REPC_FDOUBL;
		break;
	case WIS_REPR_CHAR:   //	1字节有符号字节数
		DlisRepC=DLIS_REPC_SSHORT;
		break;
	case WIS_REPR_UCHAR:  //	1字节无符号字节数
		DlisRepC=DLIS_REPC_USHORT;
		break;
	case WIS_REPR_USHORT: //	2字节无符号短整型数
		DlisRepC=DLIS_REPC_UNORM;
		break;
	case WIS_REPR_UINT:   //	4字节无符号整型
	case WIS_REPR_ULONG:  //	4字节无符号长整型数
		DlisRepC=DLIS_REPC_ULONG;
		break;
	default:
		break;
	}
	return DlisRepC;
}
void CWisToDLis::SelectCurves()
{
	CString s;
	if(WisRead->WisHead.ObjectNumber<1)
		return;
	bool *CurveID=new bool[WisRead->WisHead.ObjectNumber];
	for(int i=0;i<WisRead->WisHead.ObjectNumber;i++)
	{
		CurveID[i]=false;
	}
	CString sCurveName;
	switch(m_nCurvesRadio)
	{
	case 0://曲线过滤	
		for(i=0;i<WisRead->WisHead.ObjectNumber;i++)
		{
			if(WisRead->GetCurveDim(i)<1) continue; //不是连续通道
			bool bOk=false;
			sCurveName.Format ("%-.15s",WisRead->m_ObjectEntry[i].Name);
			sCurveName.TrimRight();
			for(int j=0;j<sp_FilterCurves.GetSize();j++)
			{
				if(sCurveName.CompareNoCase(sp_FilterCurves.GetAt(j))==0)
				{
					bOk=true;
					break;
				}
			}
			CurveID[i]=bOk;
		}
		break;
	case 2://常规曲线
		for(i=0;i<WisRead->WisHead.ObjectNumber;i++)
		{
			if(WisRead->GetCurveDim(i)==1)
			{
				CurveID[i]=true;
			}
		}
		break;
	default://交互选择或全部曲线
		for(i=0;i<WisRead->WisHead.ObjectNumber;i++)
		{
			if(WisRead->GetCurveDim(i)>0)
			{
				CurveID[i]=true;
			}
		}
		break;
	}
	//////////////////////////////////////////////
	SelCurveName.RemoveAll(); //选择曲线名表
	NewCurveName.RemoveAll(); //新曲线名表
	NewUnits.RemoveAll(); ////新单位名表
	CurveIndex.RemoveAll(); //源曲线索引号
	for(i=0;i<WisRead->WisHead.ObjectNumber;i++)
	{
		if(!CurveID[i]) continue;
		s.Format("%-.15s",WisRead->m_ObjectEntry[i].Name);
		SelCurveName.Add(s);
		s.TrimRight();
		s.TrimLeft();
		NewCurveName.Add(s);
		WisRead->OpenChannel(WisRead->m_ObjectEntry[i].Name);
		s.Format ("%-.7s",WisRead->WisChannel.Unit);
		s.TrimRight();
		if(s.CompareNoCase("none")==0)
		{
			NewUnits.Add("");
		}
		else
		{
			NewUnits.Add(s);
		}
		CurveIndex.Add(i);
	}
	delete []CurveID; CurveID=NULL;
	///////////////////////////////////////////

	//根据开关设置决定是否进行交互选择
	if(m_nCurvesRadio==3)
	{
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=255; //最大曲线长度
		ReCurNamDlg.m_nMaxLog=99999; //最多曲线数
		for(int i=0;i<CurveIndex.GetSize();i++)
		{
			int index=CurveIndex.GetAt(i);
			WisRead->OpenChannel(WisRead->m_ObjectEntry[index].Name);
			int NumDim=WisRead->WisChannel.NumOfDimension;
			ReCurNamDlg.m_SourceName.Add(SelCurveName.GetAt(i));
			ReCurNamDlg.m_Units.Add(NewUnits.GetAt(i));
			ReCurNamDlg.m_ObjectName.Add(NewCurveName.GetAt(i));//预置新曲线名
			ReCurNamDlg.m_NumDim.Add(NumDim);
		}
		SelCurveName.RemoveAll();
		NewCurveName.RemoveAll();
		NewUnits.RemoveAll();
		int Result=ReCurNamDlg.DoModal();
		for(i=0;i<ReCurNamDlg.m_SourceName.GetSize();i++)
		{
			SelCurveName.Add(ReCurNamDlg.m_SourceName.GetAt(i));//原名称
			s=ReCurNamDlg.m_ObjectName.GetAt(i);//新名称
			s.MakeUpper();
			NewCurveName.Add(s);
			NewUnits.Add(ReCurNamDlg.m_Units.GetAt(i));//单位
		}
		ReCurNamDlg.m_SourceName.RemoveAll();
		ReCurNamDlg.m_ObjectName.RemoveAll();
		ReCurNamDlg.m_NumDim.RemoveAll();
		ReCurNamDlg.m_Units.RemoveAll();
		//重新索引
		CWordArray NewIndex;
		for(i=0;i<SelCurveName.GetSize();i++)
		{
			for(int j=0;j<CurveIndex.GetSize();j++)
			{
				int index=CurveIndex.GetAt(j);
				s.Format ("%-.15s",WisRead->m_ObjectEntry[index].Name);
				if(s.CompareNoCase(SelCurveName.GetAt(i))==0)
				{
					NewIndex.Add(index);
					break;
				}
			}
		}
		//将索引值换回
		CurveIndex.RemoveAll();
		for(i=0;i<NewIndex.GetSize();i++)
		{
			CurveIndex.Add(NewIndex.GetAt(i));
		}
		NewIndex.RemoveAll();
	}
}

void CWisToDLis::GetDlisFDataIndexFromWis()
{
	CString s;
	typedef struct tagCurve
	{
		float Start;
		float Stop;
		float Rlev;
	}_Curve;
	const double er=0.001;
	NumItem=0; //组数
	if(CurveIndex.GetSize()==0) return;
	_Curve *Curve=new _Curve[CurveIndex.GetSize()];
	if(ItemNum)
	{
		delete []ItemNum; ItemNum=NULL;
	}
	ItemNum=new int[CurveIndex.GetSize()];
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		int index=CurveIndex.GetAt(i);
		WisRead->OpenChannel(WisRead->m_ObjectEntry[index].Name);
		float Start=WisRead->WisChannel.DimInfo[0].StartVal;
		float Rlev=WisRead->WisChannel.DimInfo[0].Delta;
		float Stop=Start+(WisRead->WisChannel.DimInfo[0].Samples-1)*Rlev;
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
void CWisToDLis::Prt_FdataItemInfo()
{
	for(int i=0;i<NumItem;i++)
	{
		fprintf(fp,"第%2d组: ",i+1);
		int nc=0;
		for(int j=0;j<CurveIndex.GetSize();j++)
		{
			if(ItemNum[j]==i)
			{
				fprintf(fp," %-8.8s",NewCurveName.GetAt(j));
				nc++;
				if(nc%5==0)
				{
					fprintf(fp,"\n%-8s","");
				}
			}
		}
		fprintf(fp,"\n");
	}
}
void CWisToDLis::SetDlisChannelsFromWis()
{
	CString s;
	DlisWrite->SetFrameNumber(NumItem);
	DlisWrite->SetChannelNumber(CurveIndex.GetSize());
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		int index=CurveIndex.GetAt(i);
		WisRead->OpenChannel(WisRead->m_ObjectEntry[index].Name);
		DlisWrite->Channel[i].Origin=1;  //对象O=1,C=0
		DlisWrite->Channel[i].CopyNum=0; //对象O=1,C=0
		strcpy(DlisWrite->Channel[i].Name,NewCurveName.GetAt(i));

		strcpy(DlisWrite->Channel[i].LongName,"");
		strcpy(DlisWrite->Channel[i].Properties,"");
		strcpy(DlisWrite->Channel[i].Units,NewUnits.GetAt(i));
		if(stricmp(DlisWrite->Channel[i].Units,"none")==0)
		{
			strcpy(DlisWrite->Channel[i].Units,"");
		}
		DlisWrite->Channel[i].RepCode=WisRepCodeToDlis(WisRead->WisChannel.RepCode);
		if(WisRead->WisChannel.NumOfDimension==1)
		{
			DlisWrite->Channel[i].NumDim=1; //常规或波形
			DlisWrite->Channel[i].Dimension[0]=1; //维元素
			DlisWrite->Channel[i].Dimension[1]=0; //维元素
			DlisWrite->Channel[i].Dimension[2]=0; //维元素
		}
		else 
		{
			DlisWrite->Channel[i].NumDim=WisRead->WisChannel.NumOfDimension-1; //阵列
			for(int j=0;j<DlisWrite->Channel[i].NumDim;j++)
			{
				int k=DlisWrite->Channel[i].NumDim-j;
				DlisWrite->Channel[i].Dimension[j]=WisRead->WisChannel.DimInfo[k].Samples;
			}
		}
	}
}
void CWisToDLis::SetDlisFrameFromWis()
{
	CString s;
	CString IndexUnits;
	int index=CurveIndex.GetAt(0); //第一条输出曲线的索引
	WisRead->OpenChannel(WisRead->m_ObjectEntry[index].Name);

	IndexUnits.Format ("%-.7s",WisRead->WisChannel.DimInfo[0].Unit);
	IndexUnits.TrimRight();
	if(IndexUnits.CompareNoCase("feet")==0 || IndexUnits.CompareNoCase("ft")==0)
	{
		IndexUnits="ft";
	}
	else
	{
		IndexUnits="m";
	}
	for(int i=0;i<NumItem;i++)
	{
		DlisWrite->Frame[i].Origin=1;//源(对象)
		DlisWrite->Frame[i].CopyNum=0;//拷贝数(对象)
		sprintf(DlisWrite->Frame[i].Name,"B%d",i+1);//帧名(对象名)
		strcpy(DlisWrite->Frame[i].Index_Type,"BOREHOLE-DEPTH"); //索引类型
		strcpy(DlisWrite->Frame[i].Direction,"INCREASING"); //测井方向
		strcpy(DlisWrite->Frame[i].IndexUnits,IndexUnits); //索引单位 
		int NumCurve=0; //本帧曲线数
		for(int k=0;k<CurveIndex.GetSize();k++)
		{
			if(ItemNum[k]==i)
			{
				int index=CurveIndex.GetAt(k);
				WisRead->OpenChannel(WisRead->m_ObjectEntry[index].Name);
				DlisWrite->Frame[i].Index_Start=WisRead->WisChannel.DimInfo[0].StartVal; //索引最小值
				DlisWrite->Frame[i].Index_Stop=WisRead->WisChannel.DimInfo[0].StartVal+(WisRead->WisChannel.DimInfo[0].Samples-1)*WisRead->WisChannel.DimInfo[0].Delta; //索引最大值
				DlisWrite->Frame[i].Index_Step=WisRead->WisChannel.DimInfo[0].Delta; //步长
				DlisWrite->Frame[i].ChannelID[NumCurve]=k;
				NumCurve++;
			}
		}
		DlisWrite->Frame[i].NumCurve=NumCurve; //本帧曲线数
	}
}
void CWisToDLis::SetAxisFromWis()
{
	CString s;
	int NumAxis=0;
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		int index=CurveIndex.GetAt(i);
		WisRead->OpenChannel(WisRead->m_ObjectEntry[index].Name);
		int NumDim=WisRead->WisChannel.NumOfDimension;
		NumAxis+=(NumDim-1);
	}
	fprintf(fp,"====NumAxis=%d\n",NumAxis);
	fflush(fp);
	if(NumAxis==0) return;
	DlisWrite->SetDlisAxisNumber(NumAxis);
	int nAx=0;
	for(i=0;i<CurveIndex.GetSize();i++)
	{
		int index=CurveIndex.GetAt(i); //WIS曲线索引号
		WisRead->OpenChannel(WisRead->m_ObjectEntry[index].Name);
		int NumDim=WisRead->WisChannel.NumOfDimension;
		DlisWrite->Channel[i].nAxis=NumDim-1; //曲线轴维数
		if(DlisWrite->Channel[i].nAxis==0) continue;
		DlisWrite->Axis[nAx].NumC=0; //坐标数
		//第一维
		int kDim=NumDim-1;
		DlisWrite->Axis[nAx].Origin=1; //源
		DlisWrite->Axis[nAx].CopyNum=0; //拷贝数
		DlisWrite->Axis[nAx].NumC=1; //坐标数
		DlisWrite->Channel[i].Axis[0].Origin=1; //轴：R=OBNAME	
		DlisWrite->Channel[i].Axis[0].CopyNum=0; //轴：R=OBNAME	
		sprintf(DlisWrite->Axis[nAx].Name,"%d",1000+nAx); //轴名，对应于对象名
		strcpy(DlisWrite->Channel[i].Axis[0].Name,DlisWrite->Axis[nAx].Name); //轴：R=OBNAME
		if(WisRead->WisChannel.DimInfo[kDim].Delta>0)
		{
			strcpy(DlisWrite->Axis[nAx]. AxisID,"MICRO_TIME");//轴ID号:C=1,R=IDENT
		}
		else
		{
			strcpy(DlisWrite->Axis[nAx]. AxisID,"SENSOR");//轴ID号:C=1,R=IDENT
		}
		DlisWrite->Axis[nAx].Coordinates[0]=WisRead->WisChannel.DimInfo[kDim].StartVal; //坐标
		DlisWrite->Axis[nAx].Spacing=WisRead->WisChannel.DimInfo[kDim].Delta; //间隔
		s.Format ("%-.7s",WisRead->WisChannel.DimInfo[kDim].Unit);
		s.TrimRight();
		strcpy(DlisWrite->Axis[nAx].Units,s); //坐标单位
		nAx++;
		//第二维
		if(DlisWrite->Channel[i].nAxis==2)
		{
			DlisWrite->Axis[nAx].Origin=1; //源
			DlisWrite->Axis[nAx].CopyNum=0; //拷贝数
			DlisWrite->Axis[nAx].NumC=1; //坐标数
			DlisWrite->Channel[i].Axis[1].Origin=1; //轴：R=OBNAME	
			DlisWrite->Channel[i].Axis[1].CopyNum=0; //轴：R=OBNAME	
			sprintf(DlisWrite->Axis[nAx].Name,"%d",1000+nAx); //轴名，对应于对象名
			strcpy(DlisWrite->Channel[i].Axis[1].Name,DlisWrite->Axis[nAx].Name); //轴：R=OBNAME	
			strcpy(DlisWrite->Axis[nAx]. AxisID,"SENSOR");//轴ID号:C=1,R=IDENT
			DlisWrite->Axis[nAx].Coordinates[0]=WisRead->WisChannel.DimInfo[1].StartVal; //坐标
			DlisWrite->Axis[nAx].Spacing=WisRead->WisChannel.DimInfo[1].Delta; //间隔
			s.Format ("%-.7s",WisRead->WisChannel.DimInfo[1].Unit);
			s.TrimRight();
			strcpy(DlisWrite->Axis[nAx].Units,s); //坐标单位
			nAx++;
		}
	}
}
void CWisToDLis::SetStoUnitLab()
{
	char str[256];
	memcpy(DlisWrite->StoUnitLab.SeqNum,"   1",4); //储存序号
	memcpy(DlisWrite->StoUnitLab.DLISVer,"V1.00",5); //版本号
	memcpy(DlisWrite->StoUnitLab.Stru,"RECORD",6); //存储单结构
	sprintf(str,"%5d",DlisWrite->MaxPhyLen); 
	memcpy(DlisWrite->StoUnitLab.MaxRecLen,str,5); 
	sprintf(str,"%-60.60s","WIS to DLIS Conversion"); //存储单标志
	memcpy(DlisWrite->StoUnitLab.StoSetIden,str,60);
}
void CWisToDLis::SetFileHeader()
{
	sprintf(DlisWrite->FileHeader.SequenceNumber,"%10.10s","1");//序列号
	sprintf(DlisWrite->FileHeader.ID,"%-65.65s","WIS.001");//ID号
}
void CWisToDLis::SetOrigin()
{
	CString s;
	strcpy(DlisWrite->Origin.FileID,"WIS.001");//FILE-ID
	strcpy(DlisWrite->Origin.FileSetName,"WIS-TO-DLIS-CONVERSION");//FILE-SET-NAME
	strcpy(DlisWrite->Origin.FileSetNumber,"1");//FILE-SET-NUMBER
	strcpy(DlisWrite->Origin.FileNumber,"1"); //FILE-NUMBER
	strcpy(DlisWrite->Origin.FileType,"RE-FMT"); //FILE-TYPE
	strcpy(DlisWrite->Origin.Product,"GeoReveal"); //PRODUCT
	strcpy(DlisWrite->Origin.Version,"1.0"); //VERSION
	strcpy(DlisWrite->Origin.Programs,"WIS-TO-DLIS"); //PROGRAMS
	CTime t=CTime::GetCurrentTime(); //取当前系统日期和时间
	sprintf(DlisWrite->Origin.CreationTime,"YEAR=%d MONTH=%d DAY=%d HOUR=%d MINUTE=%d SECOND=%d",
		t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(),t.GetMinute(),
		t.GetSecond()); //CREATION-TIME
	strcpy(DlisWrite->Origin.OrderNumber,""); //ORDER-NUMBER
	strcpy(DlisWrite->Origin.DescentNumber,""); //DESCENT-NUMBER
	strcpy(DlisWrite->Origin.RunNumber,""); //RUN-NUMBER
	strcpy(DlisWrite->Origin.WellID,""); //WELL-ID
	strcpy(DlisWrite->Origin.WellName,""); //WELL-NAME
	strcpy(DlisWrite->Origin.FieldName,""); //FIELD-NAME
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

void CWisToDLis::SetDlisParameter()
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

void CWisToDLis::WisDataToDlis()
{
	CString s;
	int NumLog=-1; //已输出的曲线数
	pGuage->m_nTextMode=1;
	for(int nItem=0;nItem<NumItem;nItem++)
	{
		if(sp_Break_Status) break;
		int NumCurve=DlisWrite->Frame[nItem].NumCurve;
		float Start=DlisWrite->Frame[nItem].Index_Start;
		float Stop=DlisWrite->Frame[nItem].Index_Stop;
		float Step=DlisWrite->Frame[nItem].Index_Step;
		int NumPoint=int((Stop-Start)/Step+0.5)+1;
		DlisWrite->StartWriteData(nItem);
		pGuage->SetRange(0,100);
		for(int i=0;i<NumCurve;i++)
		{
			if(sp_Break_Status) break;
			NumLog++; //已输出曲线数
			//index曲线在DLIS通道中索引
			int index=DlisWrite->Frame[nItem].ChannelID[i]; 
			int IndexWis=CurveIndex.GetAt(index); //曲线在WIS中索引
			WisRead->OpenChannel(WisRead->m_ObjectEntry[IndexWis].Name);
			s=SelCurveName.GetAt(index)+"("+NewCurveName.GetAt(index)+")";
			s.Remove(' ');
			fprintf(fp,"====Write: %-4d %-20s %5d\n",NumLog,s,DlisWrite->Channel[index].Sample);
			pGuage->m_sTextInfo=SelCurveName.GetAt(index);
			pGuage->SetPos((i+1)*100/NumCurve);
			if (pGuage->CheckCancelButton())
			{
				sp_Break_Status=true;
				break;
			}
			/////////////////////////////////////////////////////////////////
			if(WisRead->WisChannel.NumOfDimension==1)
			{
				float depth=WisRead->WisChannel.DimInfo[0].StartVal;
				float *buf=new float[NumPoint];
				WisRead->ReadCurve(IndexWis,depth,NumPoint,buf);
				DlisWrite->WriteCurveData(nItem,i,NumPoint,buf);
				delete []buf; buf=NULL;
			}
			else
			{
				int Count=DlisWrite->Channel[index].Sample;
				float *buf=new float[Count];
				for(int j=0;j<Count;j++)
				{
					buf[j]=0;
				}
				pGuage->SetRange(0,100);
				for(j=0;j<NumPoint;j++)
				{
					pGuage->SetPos(((j+1)*100)/NumPoint);
					if (pGuage->CheckCancelButton())
					{
						sp_Break_Status=true;
						break;
					}
					float depth=Start+j*Step;
					WisRead->ReadWaveArray(IndexWis,depth,buf);
					DlisWrite->WriteArrayData(nItem,i,j,Count,buf);
				}
				delete []buf ; buf=NULL;
			}
		}
		pGuage->m_sTextInfo="将临时文件写到DLIS文件";
		pGuage->SetPos(75);
		DlisWrite->StopWriteData(nItem);
	}
}

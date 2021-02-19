// Slc716ToDLis.cpp: implementation of the CSlc716ToDLis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "Slc716ToDLis.h"
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

CSlc716ToDLis::CSlc716ToDLis()
{
}

CSlc716ToDLis::~CSlc716ToDLis()
{
}
bool CSlc716ToDLis::Slc716ToDlis(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sProducer,FILE *fo)
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
	Slc716R=new CSlc716R ;
	DlisWrite=new CDlisWrite ; 
	bool bResult=Transform();
	delete pGuage;  //删除进度条
	delete Slc716R; Slc716R=NULL;
	delete DlisWrite; DlisWrite=NULL;
	return bResult;
}

bool CSlc716ToDLis::Transform()
{
	if(!IsSLC716(m_SourceFile))
	{
		fprintf(fp,"%s is not SLC716 Format\n\n",m_SourceFile);
		return false;
	}
	CString s;
	//根据输出目录及输入文件名确定输出文件名
	DlisFileName.Format ("%s\\%s.dlis",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	DlisFileName.MakeLower();
	ModiFileName(DlisFileName); //根据条件决定是否修改文件名

	if(!Slc716R->Open(m_SourceFile))
	{
		return false;
	}
	Slc716R->ReadHead(); //读SLC716头
	SelectCurves();//选择曲线或对曲线改名
	if(Slc716R->Slc716Head.NumLog<1)
	{
		Slc716R->Close();
		fprintf(fp,"\n!!!!没有曲线输出!!!\n\n");
		return false;
	}
	//输出信息
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)DlisFileName);
	////////////////////////////////////////////
	SetDlisChannelsFromSlc716(); //根据SLC716信息设置通道信息
	SetDlisFrameFromSlc716(); //设置DLIS帧对象属性
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
	Slc716DataToDlis(); //输出数据
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	CurveIndex.RemoveAll();
	DlisWrite->Close(); //关闭DLIS文件
	DlisWrite->Prt_Header();
	return !sp_Break_Status;
}

void CSlc716ToDLis::SelectCurves()
{
	CString s;
	bool *CurveID=new bool[Slc716R->Slc716Head.NumLog];
	for(int i=0;i<Slc716R->Slc716Head.NumLog;i++)
	{
		CurveID[i]=false;
	}
	CString sCurveName;
	switch(m_nCurvesRadio)
	{
	case 0://曲线过滤
		for(i=0;i<Slc716R->Slc716Head.NumLog;i++)
		{
			bool bOk=false;
			sCurveName.Format ("%-.4s",Slc716R->Slc716Head.CurveName[i]);
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
	case 1: //全部曲线
	case 2: //常规曲线
	case 3: //交互选择
		for(i=0;i<Slc716R->Slc716Head.NumLog;i++)
		{
			CurveID[i]=true;
		}
		break;
	}
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	//根据开关设置决定是否进行交互选择
	if(m_nCurvesRadio !=3 )
	{	//没有交互操作
		for(i=0;i<Slc716R->Slc716Head.NumLog;i++)
		{
			if(!CurveID[i]) continue;
			sCurveName.Format("%-.4s",Slc716R->Slc716Head.CurveName[i]);
			SelCurveName.Add(sCurveName);
			sCurveName.MakeUpper();
			sCurveName.TrimRight();
			NewCurveName.Add(sCurveName);
			NewUnits.Add("");
		}
	}
	else
	{	//交互操作
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=255; //最大曲线长度
		ReCurNamDlg.m_nMaxLog=99999; //最多曲线数
		for(int i=0;i<Slc716R->Slc716Head.NumLog;i++)
		{
			if(!CurveID[i]) continue;
			sCurveName.Format ("%-.4s",Slc716R->Slc716Head.CurveName[i]);
			ReCurNamDlg.m_SourceName.Add(sCurveName);
			sCurveName.TrimLeft();
			sCurveName.TrimRight();
			sCurveName.Remove(' ');
			ReCurNamDlg.m_ObjectName.Add(sCurveName);//预置新曲线名
			ReCurNamDlg.m_Units.Add("");
			ReCurNamDlg.m_NumDim.Add(1);
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
		ReCurNamDlg.m_NumDim.RemoveAll();
		ReCurNamDlg.m_Units.RemoveAll();
	}
	///////////////////////////////////////////////////
	delete []CurveID; CurveID=NULL;
	CurveIndex.RemoveAll();
	for(i=0;i<SelCurveName.GetSize();i++)
	{
		for(int j=0;j<Slc716R->Slc716Head.NumLog;j++)
		{
			s.Format ("%-4.4s",Slc716R->Slc716Head.CurveName[j]);
			if(s.CompareNoCase(SelCurveName.GetAt(i))==0)
			{
				CurveIndex.Add(j);
				break;
			}
		}
	}
}
void CSlc716ToDLis::SetDlisChannelsFromSlc716()
{
	CString s;
	DlisWrite->SetFrameNumber(1); //帧数
	DlisWrite->SetChannelNumber(CurveIndex.GetSize()); //曲线数
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
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
		DlisWrite->Channel[i].RepCode=DLIS_REPC_FSINGL;
		DlisWrite->Channel[i].NumDim=1; //维数
		DlisWrite->Channel[i].Dimension[0]=1; //维元素
		DlisWrite->Channel[i].Sample=1;//采样数据个数
	}
}
void CSlc716ToDLis::SetDlisFrameFromSlc716()
{
	CString s;
	DlisWrite->Frame[0].Origin=1;//源(对象)
	DlisWrite->Frame[0].CopyNum=0;//拷贝数(对象)
	sprintf(DlisWrite->Frame[0].Name,"B1");//帧名(对象名)
	strcpy(DlisWrite->Frame[0].Index_Type,"BOREHOLE-DEPTH"); //索引类型
	strcpy(DlisWrite->Frame[0].Direction,"INCREASING"); //测井方向
	strcpy(DlisWrite->Frame[0].IndexUnits,"m"); //索引单位 
	DlisWrite->Frame[0].Index_Start=Slc716R->Slc716Head.Stdep; //索引最小值
	DlisWrite->Frame[0].Index_Stop=Slc716R->Slc716Head.Endep; //索引最大值
	DlisWrite->Frame[0].Index_Step=Slc716R->Slc716Head.Rlev; //步长
	DlisWrite->Frame[0].NumCurve=CurveIndex.GetSize(); //本帧曲线数
	for(int i=0;i<DlisWrite->Frame[0].NumCurve;i++)
	{
		DlisWrite->Frame[0].ChannelID[i]=i; //帧曲线在通道中索引号
	}
}
void CSlc716ToDLis::SetStoUnitLab()
{
	char str[256];
	memcpy(DlisWrite->StoUnitLab.SeqNum,"   1",4); //储存序号
	memcpy(DlisWrite->StoUnitLab.DLISVer,"V1.00",5); //版本号
	memcpy(DlisWrite->StoUnitLab.Stru,"RECORD",6); //存储单结构
	sprintf(str,"%5d",DlisWrite->MaxPhyLen); 
	memcpy(DlisWrite->StoUnitLab.MaxRecLen,str,5); 
	sprintf(str,"%-60.60s","SLC716 to DLIS Conversion"); //存储单标志
	memcpy(DlisWrite->StoUnitLab.StoSetIden,str,60);
}
void CSlc716ToDLis::SetFileHeader()
{
	sprintf(DlisWrite->FileHeader.SequenceNumber,"%10.10s","1");//序列号
	sprintf(DlisWrite->FileHeader.ID,"%-65.65s","SLC716.001");//ID号
}
void CSlc716ToDLis::SetOrigin()
{
	CString s;
	strcpy(DlisWrite->Origin.FileID,"SLC716.001");//FILE-ID
	strcpy(DlisWrite->Origin.FileSetName,"SLC716-TO-DLIS-CONVERSION");//FILE-SET-NAME
	strcpy(DlisWrite->Origin.FileSetNumber,"1");//FILE-SET-NUMBER
	strcpy(DlisWrite->Origin.FileNumber,"1"); //FILE-NUMBER
	strcpy(DlisWrite->Origin.FileType,"RE-FMT"); //FILE-TYPE
	strcpy(DlisWrite->Origin.Product,"GeoReveal "); //PRODUCT
	strcpy(DlisWrite->Origin.Version,"1.0"); //VERSION
	strcpy(DlisWrite->Origin.Programs,"SLC716-TO-DLIS"); //PROGRAMS
	CTime t=CTime::GetCurrentTime(); //取当前系统日期和时间
	sprintf(DlisWrite->Origin.CreationTime,"YEAR=%d MONTH=%d DAY=%d HOUR=%d MINUTE=%d SECOND=%d",
		t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(),t.GetMinute(),
		t.GetSecond()); //CREATION-TIME
	strcpy(DlisWrite->Origin.OrderNumber,""); //ORDER-NUMBER
	strcpy(DlisWrite->Origin.DescentNumber,""); //DESCENT-NUMBER
	strcpy(DlisWrite->Origin.RunNumber,""); //RUN-NUMBER
	strcpy(DlisWrite->Origin.WellID,""); //WELL-ID
	s.Format ("%-80.80s",Slc716R->Slc716Head.WellName);
	s.TrimRight();
	s.TrimLeft();
	if(s.IsEmpty())
	{
		s=MyGetFileTitle(m_SourceFile);
	}
	strcpy(DlisWrite->Origin.WellName,s); //WELL-NAME
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
	strcpy(DlisWrite->Origin.Company,Slc716R->Slc716Head.CompanyName); //COMPANY
	strcpy(DlisWrite->Origin.NameSpaceVersion,"0"); //NAME-SPACE-VERSION
}

void CSlc716ToDLis::SetDlisParameter()
{
	CString sInWellName=MyGetFileTitle(m_SourceFile); //得到的井名
	CString sInField=MyGetPathName(m_SourceFile); 
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

void CSlc716ToDLis::Slc716DataToDlis()
{
	CString s;
	DlisWrite->StartWriteData(0);
	int nBufLen=Slc716R->NumBlock*64;
	int NumPoint=Slc716R->NumPoint;
	float *buf=new float[nBufLen];
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
		Slc716R->ReadChannel(index,buf);
		for(int k=0;k<NumPoint;k++)
		{
			if(fabs(buf[k]+9999.0)<0.01) buf[k]=DlisWrite->Origin.DefaultVal;
			if(fabs(buf[k]+999.25)<0.01) buf[k]=DlisWrite->Origin.DefaultVal;
		}
		DlisWrite->WriteCurveData(0,i,NumPoint,buf);
	}
	DlisWrite->StopWriteData(0);
	delete []buf; buf=NULL;
}

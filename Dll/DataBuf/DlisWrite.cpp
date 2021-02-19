// DlisWrite.cpp: implementation of the CDlisWrite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DlisWrite.h"
#include <math.h>
#include <string.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDlisWrite::CDlisWrite()
{
	MaxPhyLen=8192; //物理记录长度
	DiskBufLen=0; //磁盘缓冲区当前长度
	DiskBuf=NULL;
	LogBufLen=MAX_DLIS_BLOCKLEN;
	LogBuf=new BYTE[LogBufLen];
	bTrailLengthFlag=false;
	IORIGIN=new DWORD[MAX_OBJECT_NUM];//数据源
	ICOPYNUM=new BYTE[MAX_OBJECT_NUM];//数据拷贝数
	IVALUE=new int[MAX_DATAITEM_NUM]; //整数值
	VALUE=new float[MAX_DATAITEM_NUM]; //浮点值
	CVALUE=new char[MAX_DATAITEM_NUM*64]; //子串集,假定一个子串平均长度为64字节
	Axis=NULL;
	Channel=NULL;
	Frame=NULL;
	VariableReset(); //变量初始化
}

CDlisWrite::~CDlisWrite()
{
	if(DiskBuf) { delete []DiskBuf; DiskBuf=NULL; }
	if(LogBuf){ delete []LogBuf; LogBuf=NULL; }
	delete []IORIGIN; IORIGIN=NULL;
	delete []ICOPYNUM; ICOPYNUM=NULL;
	delete []IVALUE; IVALUE=NULL;
	delete []VALUE; VALUE=NULL;
	delete []CVALUE; CVALUE=NULL;
	if(Axis) { delete []Axis; Axis=NULL; }
	if(Channel) {delete []Channel; Channel=NULL; }
	if(Frame) {delete []Frame; Frame=NULL; }
}
void CDlisWrite::VariableReset()
{
	NumChannel=0;
	NumAxis=0;
	NumFrame=0;
	memset(FileHeader.ID,'\0',66);
	memset(FileHeader.SequenceNumber,'\0',11);
	//源
	memset(Origin.FileID,'\0',MAX_ORIGIN_LEN+1); //FILE-ID
	memset(Origin.FileSetName,'\0',MAX_ORIGIN_LEN+1); //FILE-SET-NAME
	memset(Origin.FileSetNumber,'\0',MAX_ORIGIN_LEN+1); //FILE-SET-NUMBER
	memset(Origin.FileNumber,'\0',MAX_ORIGIN_LEN+1); //FILE-NUMBER
	memset(Origin.FileType,'\0',MAX_ORIGIN_LEN+1); //FILE-TYPE
	memset(Origin.Product,'\0',MAX_ORIGIN_LEN+1); //PRODUCT
	memset(Origin.Version,'\0',MAX_ORIGIN_LEN+1); //VERSION
	memset(Origin.Programs,'\0',MAX_ORIGIN_LEN+1); //PROGRAMS
	memset(Origin.CreationTime,'\0',MAX_ORIGIN_LEN+1); //CREATION-TIME
	memset(Origin.OrderNumber,'\0',MAX_ORIGIN_LEN+1); //ORDER-NUMBER
	memset(Origin.DescentNumber,'\0',MAX_ORIGIN_LEN+1); //DESCENT-NUMBER
	memset(Origin.RunNumber,'\0',MAX_ORIGIN_LEN+1); //RUN-NUMBER
	memset(Origin.WellID,'\0',MAX_ORIGIN_LEN+1); //WELL-ID
	memset(Origin.WellName,'\0',MAX_ORIGIN_LEN+1); //WELL-NAME
	memset(Origin.FieldName,'\0',MAX_ORIGIN_LEN+1); //FIELD-NAME
	memset(Origin.ProducerCode,'\0',MAX_ORIGIN_LEN+1); //PRODUCER-CODE
	memset(Origin.ProducerName,'\0',MAX_ORIGIN_LEN+1); //PRODUCER-NAME
	memset(Origin.Company,'\0',MAX_ORIGIN_LEN+1); //COMPANY
	memset(Origin.NameSpaceName,'\0',MAX_ORIGIN_LEN+1); //NAME-SPACE-NAME
	memset(Origin.NameSpaceVersion,'\0',MAX_ORIGIN_LEN+1); //NAME-SPACE-VERSION
	Origin.DefaultVal=-999.25; //缺省值

}
bool CDlisWrite::Open(CString sDataFile,FILE *fo)
{
	m_sDataFile=sDataFile;
	fp=fo;
	if(fDlis.Open(sDataFile,CFile::modeCreate|CFile::modeReadWrite)==FALSE)
	{
		return false;
	}
	return true;
}
void CDlisWrite::Close()
{
	WriteDlisBufDToisk();
	fDlis.Close();
	fprintf(fp,"====DLIS File Size: %d Bytes\n",GetFileLength(m_sDataFile));
}
void CDlisWrite::WriteStoUnitLab()
{
	fDlis.SeekToBegin();
	fDlis.Write(&StoUnitLab,sizeof(StoUnitLab));
	if(DiskBuf)
	{
		delete []DiskBuf; DiskBuf=NULL;
	}
	DiskBuf=new BYTE[MaxPhyLen];
}

void CDlisWrite::WriteDlisBufDToisk()
{
	if(DiskBufLen<=4) return;
	DiskBuf[0]=(DiskBufLen>>8)&0XFF;
	DiskBuf[1]=DiskBufLen&0XFF;
	DiskBuf[2]=0XFF;
	DiskBuf[3]=0X01;
	fDlis.Write(DiskBuf,DiskBufLen);
	DiskBufLen=4;
}

void CDlisWrite::SetTrailLengthFlag(bool bFlag)
{
	bTrailLengthFlag=bFlag;
}

void CDlisWrite::WriteLogBufToDlisBuf(BYTE RecFlag,BYTE RecType,int Buflen,BYTE *buf,bool bWFlag)
{
	CString s;
	if(DiskBufLen==0)
	{
		DiskBuf[0]=0;
		DiskBuf[1]=4;
		DiskBuf[2]=0XFF;
		DiskBuf[3]=0X01;
		DiskBufLen=4;
	}
	int nBuflen=Buflen; //要送入缓冲区的字节数
	int TrailByte=0; //尾记录长度所占字节
	int IpntLenC=0; //已送入缓冲区字节
	if(bTrailLengthFlag)
	{
		TrailByte=2;
	}
	//如果现长度与原长度的和大于物理记录长度，则将原内容先写入盘
	//表达式中１６为最小逻辑记录长度，如果没有给定该值将导致错误
	if((DiskBufLen+4+TrailByte+Buflen+MIN_LOG_LENGTH)>MaxPhyLen)
	{
		WriteDlisBufDToisk();
	}
	//写当前缓冲区数据
	while(1)
	{
		if(nBuflen<1) break;
		BYTE AttByte=0X00; //属性初始值
		if(RecFlag) AttByte|=0X80; //是否为直接格式记录
		if(IpntLenC>0) AttByte|=0X40; //是否为前继记录
		if(bTrailLengthFlag) AttByte|=0X02; //有尾长度
		if((DiskBufLen+4+TrailByte+nBuflen+MIN_LOG_LENGTH)>MaxPhyLen)
		{
			int nPackLen=MaxPhyLen-DiskBufLen-4-TrailByte;
			AttByte|=0X20; //后续有记录
			DiskBuf[DiskBufLen]=((nPackLen+4+TrailByte)>>8)&0XFF;
			DiskBuf[DiskBufLen+1]=(nPackLen+4+TrailByte)&0XFF;
			DiskBuf[DiskBufLen+2]=AttByte;
			DiskBuf[DiskBufLen+3]=RecType;
			memcpy(&DiskBuf[DiskBufLen+4],&buf[IpntLenC],nPackLen);
			if(bTrailLengthFlag)
			{
				DiskBuf[DiskBufLen+4+nPackLen]=DiskBuf[DiskBufLen];
				DiskBuf[DiskBufLen+4+nPackLen+1]=DiskBuf[DiskBufLen+1];
			}
			DiskBufLen+=4+nPackLen+TrailByte;
			IpntLenC+=nPackLen;
			nBuflen-=nPackLen;
			//输出（设置物理记录头信息）
			WriteDlisBufDToisk();
			DiskBufLen=4;
		}
		else
		{
			int nPack=nBuflen+TrailByte; //含尾长度的字节数
			int nPad=0; //尾填充字节数
			if(nPack<MIN_LOG_LENGTH || nPack%4!=0)
			{ 
				AttByte|=0X01; //尾填充
				if(nPack<MIN_LOG_LENGTH)
				{
					nPad=MIN_LOG_LENGTH-nPack;
				}
				else
				{
					nPad=4-nPack%4; //补充为4的倍数
				}
				nPack+=nPad;
			}
			nPack+=4; //逻辑记录头长度为4字节

			DiskBuf[DiskBufLen]=(nPack>>8)&0XFF;
			DiskBuf[DiskBufLen+1]=nPack&0XFF;
			DiskBuf[DiskBufLen+2]=AttByte;
			DiskBuf[DiskBufLen+3]=RecType;
			memcpy(&DiskBuf[DiskBufLen+4],&buf[IpntLenC],nBuflen);
			if(nPad>0)
			{
				for(int k=0;k<(nPad-1);k++)
				{
					DiskBuf[DiskBufLen+4+nBuflen+k]='\0';
				}
				DiskBuf[DiskBufLen+4+nBuflen+nPad-1]=nPad;
			}
			if(bTrailLengthFlag)
			{
				DiskBuf[DiskBufLen+4+nBuflen+nPad]=DiskBuf[DiskBufLen];
				DiskBuf[DiskBufLen+4+nBuflen+nPad+1]=DiskBuf[DiskBufLen+1];
			}
			DiskBufLen+=nPack;
			IpntLenC+=nBuflen;
			nBuflen=0;
			//根据条件决定是否输出
			if(bWFlag || DiskBufLen>=(MaxPhyLen-6))
			{
				WriteDlisBufDToisk();
				DiskBufLen=4;
			}
		}
	}
}
void CDlisWrite::PutCompSET(const char *Type,const char *Name,int &IP,BYTE *buf)
{
	int sl=strlen(Type);
	int IpntC=0;
	buf[0]=0XE0;
	if(sl<1)
	{
		TRACE("\n警告:Call PutComponentSET,but Type is NULL\n\n");
		return;
	}
	if(sl>255)
	{
		TRACE("\n警告:Call PutComponentSET,but Type len > 255\n\n");
		return;
	}
	if(strlen(Name)>255)
	{
		TRACE("\n警告:Call PutComponentSET,but name Length > 255\n\n");
		return;
	}

	buf[0]|=0X10; //设置类型位
	buf[1]=sl;
	memcpy(&buf[2],Type,sl);
	IpntC=2+buf[1];
	sl=strlen(Name);
	if(sl>0)
	{
		buf[0]|=0X08; //设置名称位
		buf[IpntC]=sl;
		memcpy(&buf[IpntC+1],Name,sl);
		IpntC+=1+sl;
	}
	IP+=IpntC;
}
void CDlisWrite::PutCompATTRIB(const char *Label,const DWORD Count,const BYTE RepCode,const char *Units,int &IP,BYTE *buf)
{
	int sl=strlen(Label);
	int IpntC=0;
	buf[0]=0X20;
	if(sl<1)
	{
		TRACE("\n警告:Call PutComponentATTRIB,but Label is NULL\n\n");
		return;
	}
	if(sl>255)
	{
		TRACE("\n警告:Call PutComponentATTRIB,but Label len > 255\n\n");
		return;
	}
	int NBYTE;
	//Label
	buf[0]|=0X10; 
	buf[1]=sl;
	memcpy(&buf[2],Label,sl);
	IpntC=2+sl;
	//Count
	if(Count>1)
	{
		buf[0]|=0X08; 
		DlisOut.DLIS_UVARI(Count,NBYTE,&buf[IpntC]);
		IpntC+=NBYTE;
	}
	//RepCode
	if(RepCode>0)
	{
		buf[0]|=0X04;
		buf[IpntC]=RepCode;
		IpntC++;
	}
	//Units
	sl=strlen(Units);
	if(sl>0)
	{
		buf[0]|=0X02;
		buf[IpntC]=sl;
		IpntC++;
		memcpy(&buf[IpntC],Units,sl);
		IpntC+=sl;
	}
	IP+=IpntC;
}
void CDlisWrite::PutCompOBJECT(const int Origin,const BYTE CopyNum,const char *Name,int &IP,BYTE *buf)
{
	int sl=strlen(Name);
	if(sl<1)
	{
		TRACE("\n警告:Call PutCompOBJECT,but Name is NULL\n\n");
		return;
	}
	if(sl>255)
	{
		TRACE("\n警告:Call PutCompOBJECT,but Name len > 255\n\n");
		return;
	}

	int IpntC=0;
	int NBYTE=0;
	buf[0]=0X70;
	IpntC++;
	DlisOut.DLIS_OBNAME(Origin,CopyNum,Name,NBYTE,&buf[IpntC]);
	IpntC+=NBYTE;
	IP+=IpntC;
}

void CDlisWrite::WriteFileHeader()
{
	int IpntLenC=0,NBYTE=0;
	PutCompSET("FILE-HEADER","",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("SEQUENCE-NUMBER",1,DLIS_REPC_ASCII,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("ID",1,DLIS_REPC_ASCII,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompOBJECT(1,0,"1",IpntLenC,&LogBuf[IpntLenC]); //对象O=1,C=0,N="1"
	DlisOut.Encode_ASCII(IpntLenC,&LogBuf[IpntLenC],FileHeader.SequenceNumber);
	DlisOut.Encode_ASCII(IpntLenC,&LogBuf[IpntLenC],FileHeader.ID);
	WriteLogBufToDlisBuf(1,0,IpntLenC,LogBuf,false);
//	WriteDlisBufDisk();
}
void CDlisWrite::WriteOrigin()
{
	CString s;
	int IpntLenC=0,NBYTE=0;
	PutCompSET("ORIGIN","51",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("FILE-ID",        1, DLIS_REPC_ASCII,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("FILE-SET-NAME",  1, DLIS_REPC_IDENT,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("FILE-SET-NUMBER",1, DLIS_REPC_UVARI,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("FILE-NUMBER",    1, DLIS_REPC_UVARI,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("FILE-TYPE",      1, DLIS_REPC_IDENT,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("PRODUCT",        1, DLIS_REPC_ASCII,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("VERSION",        1, DLIS_REPC_ASCII,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("PROGRAMS",       1, DLIS_REPC_ASCII,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("CREATION-TIME",  1, DLIS_REPC_DTIME,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("ORDER-NUMBER",   1, DLIS_REPC_ASCII,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("DESCENT-NUMBER", 1, DLIS_REPC_ASCII,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("RUN-NUMBER",     1, DLIS_REPC_ASCII,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("WELL-ID",        1, DLIS_REPC_ASCII,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("WELL-NAME",      1, DLIS_REPC_ASCII,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("FIELD-NAME",     1, DLIS_REPC_ASCII,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("PRODUCER-CODE",  1, DLIS_REPC_UNORM,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("PRODUCER-NAME",  1, DLIS_REPC_ASCII,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("COMPANY",        1, DLIS_REPC_ASCII,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("NAME-SPACE-NAME",1, DLIS_REPC_IDENT,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("NAME-SPACE-VERSION",1,DLIS_REPC_UVARI,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompOBJECT(1,0,"2",IpntLenC,&LogBuf[IpntLenC]); //对象O=1,C=0,N="2"
	DlisOut.Encode_ASCII(IpntLenC,&LogBuf[IpntLenC],Origin.FileID);
	DlisOut.Encode_IDENT(IpntLenC,&LogBuf[IpntLenC],Origin.FileSetName);
	int NVALUE=atoi(Origin.FileSetNumber);
	DlisOut.Encode_UVARI(IpntLenC,&LogBuf[IpntLenC],&NVALUE);
	NVALUE=atoi(Origin.FileNumber);
	DlisOut.Encode_UVARI(IpntLenC,&LogBuf[IpntLenC],&NVALUE);
	DlisOut.Encode_IDENT(IpntLenC,&LogBuf[IpntLenC],Origin.FileType);
	DlisOut.Encode_ASCII(IpntLenC,&LogBuf[IpntLenC],Origin.Product);
	DlisOut.Encode_ASCII(IpntLenC,&LogBuf[IpntLenC],Origin.Version);
	DlisOut.Encode_ASCII(IpntLenC,&LogBuf[IpntLenC],Origin.Programs);
	WORD Year=2004;//年
	BYTE Month=11,Day=18,Hour=16,Minute=51,Second=1; //月日时分秒
	if(strlen(Origin.CreationTime)>0)
	{
		int n=s.Find("YEAR="); if(n>=0)	{Year=atoi((const char*)(s.Mid(n+5))); }
		n=s.Find("MONTH=");	if(n>=0) {Month=atoi((const char*)(s.Mid(n+6))); }
		n=s.Find("DAY="); if(n>=0) { Day=atoi((const char*)(s.Mid(n+4))); }
		n=s.Find("HOUR="); if(n>=0) { Hour=atoi((const char*)(s.Mid(n+5))); }
		n=s.Find("MINUTE="); if(n>=0) { Minute=atoi((const char*)(s.Mid(n+7))); }
		n=s.Find("SECOND="); if(n>=0) { Second=atoi((const char*)(s.Mid(n+7))); }
	}
	else
	{
		CTime t=CTime::GetCurrentTime(); //取当前系统日期和时间
		Year=t.GetYear();
		Month=t.GetMonth();
		Day=t.GetDay();
		Hour=t.GetHour();
		Minute=t.GetMinute();
		Second=t.GetSecond();
	}
	DlisOut.Encode_DTIME(Year,Month,Day,Hour,Minute,Second,IpntLenC,&LogBuf[IpntLenC]);
	DlisOut.Encode_ASCII(IpntLenC,&LogBuf[IpntLenC],Origin.OrderNumber);
	DlisOut.Encode_ASCII(IpntLenC,&LogBuf[IpntLenC],Origin.DescentNumber);
	DlisOut.Encode_ASCII(IpntLenC,&LogBuf[IpntLenC],Origin.RunNumber);
	DlisOut.Encode_ASCII(IpntLenC,&LogBuf[IpntLenC],Origin.WellID);
	DlisOut.Encode_ASCII(IpntLenC,&LogBuf[IpntLenC],Origin.WellName);
	DlisOut.Encode_ASCII(IpntLenC,&LogBuf[IpntLenC],Origin.FieldName);
	NVALUE=atoi(Origin.ProducerCode);
	DlisOut.Encode_UNORM(IpntLenC,&LogBuf[IpntLenC],&NVALUE);
	DlisOut.Encode_ASCII(IpntLenC,&LogBuf[IpntLenC],Origin.ProducerName);
	DlisOut.Encode_ASCII(IpntLenC,&LogBuf[IpntLenC],Origin.Company);
	DlisOut.Encode_IDENT(IpntLenC,&LogBuf[IpntLenC],Origin.NameSpaceName);
	NVALUE=atoi(Origin.NameSpaceVersion);
	DlisOut.Encode_UVARI(IpntLenC,&LogBuf[IpntLenC],&NVALUE);
	WriteLogBufToDlisBuf(1,1,IpntLenC,LogBuf,false);
//	WriteDlisBufDisk();
}
void CDlisWrite::WriteParameter()
{
	CString s;
	int IpntLenC=0,NBYTE=0;
	int NumDim=1;
	PutCompSET("PARAMETER","52",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("LONG-NAME", 1, DLIS_REPC_ASCII,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("DIMENSION", 1, DLIS_REPC_UVARI,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("VALUES",    1, DLIS_REPC_FSINGL,"",IpntLenC,&LogBuf[IpntLenC]);
	//公司名
	if(strlen(Origin.Company)>0)
	{
		PutCompOBJECT(1,0,"CN",IpntLenC,&LogBuf[IpntLenC]); //对象O=1,C=0
		DlisOut.Encode_DEFAULT(IpntLenC,&LogBuf[IpntLenC]);//无长名
		DlisOut.Encode_UVARI(IpntLenC,&LogBuf[IpntLenC],&NumDim);
		DlisOut.Encode_ASCII(IpntLenC,&LogBuf[IpntLenC],Origin.Company,"",DLIS_REPC_ASCII);
	}
	//油田名称
	if(strlen(Origin.FieldName)>0)
	{
		PutCompOBJECT(1,0,"FN",IpntLenC,&LogBuf[IpntLenC]); //对象O=1,C=0
		DlisOut.Encode_DEFAULT(IpntLenC,&LogBuf[IpntLenC]);//无长名
		DlisOut.Encode_UVARI(IpntLenC,&LogBuf[IpntLenC],&NumDim);
		DlisOut.Encode_ASCII(IpntLenC,&LogBuf[IpntLenC],Origin.FieldName,"",DLIS_REPC_ASCII);
	}
	//井名
	if(strlen(Origin.WellName)>0)
	{
		PutCompOBJECT(1,0,"WN",IpntLenC,&LogBuf[IpntLenC]); //对象O=1,C=0
		DlisOut.Encode_DEFAULT(IpntLenC,&LogBuf[IpntLenC]);//无长名
		DlisOut.Encode_UVARI(IpntLenC,&LogBuf[IpntLenC],&NumDim);
		DlisOut.Encode_ASCII(IpntLenC,&LogBuf[IpntLenC],Origin.WellName,"",DLIS_REPC_ASCII);
	}
	//空值
	PutCompOBJECT(1,0,"NULL",IpntLenC,&LogBuf[IpntLenC]); //对象O=1,C=0
	DlisOut.Encode_DEFAULT(IpntLenC,&LogBuf[IpntLenC]);//无长名
	DlisOut.Encode_UVARI(IpntLenC,&LogBuf[IpntLenC],&NumDim);
	DlisOut.Encode_FSINGL(IpntLenC,&LogBuf[IpntLenC],&Origin.DefaultVal,"",DLIS_REPC_FSINGL);
	if(NumFrame==1)
	{	//只有一帧，可写起止深度及间隔
		//STRT
		PutCompOBJECT(1,0,"STRT",IpntLenC,&LogBuf[IpntLenC]); //对象O=1,C=0
		DlisOut.Encode_DEFAULT(IpntLenC,&LogBuf[IpntLenC]);//无长名
		DlisOut.Encode_UVARI(IpntLenC,&LogBuf[IpntLenC],&NumDim);
		DlisOut.Encode_FSINGL(IpntLenC,&LogBuf[IpntLenC],&Frame[0].Index_Start,"",DLIS_REPC_FSINGL);
		//STOP
		PutCompOBJECT(1,0,"STOP",IpntLenC,&LogBuf[IpntLenC]); //对象O=1,C=0
		DlisOut.Encode_DEFAULT(IpntLenC,&LogBuf[IpntLenC]);//无长名
		DlisOut.Encode_UVARI(IpntLenC,&LogBuf[IpntLenC],&NumDim);
		DlisOut.Encode_FSINGL(IpntLenC,&LogBuf[IpntLenC],&Frame[0].Index_Stop,"",DLIS_REPC_FSINGL);
		//STEP
		PutCompOBJECT(1,0,"STEP",IpntLenC,&LogBuf[IpntLenC]); //对象O=1,C=0
		DlisOut.Encode_DEFAULT(IpntLenC,&LogBuf[IpntLenC]);//无长名
		DlisOut.Encode_UVARI(IpntLenC,&LogBuf[IpntLenC],&NumDim);
		DlisOut.Encode_FSINGL(IpntLenC,&LogBuf[IpntLenC],&Frame[0].Index_Step,"",DLIS_REPC_FSINGL);

	}

	//送入磁盘缓冲区
	WriteLogBufToDlisBuf(1,5,IpntLenC,LogBuf,false);
}

void CDlisWrite::SetIndexCurveInfo()
{
	//增加索引曲线,索引数与帧数相同
	for(int nFrame=0;nFrame<NumFrame;nFrame++)
	{
		int Index=NumChannel-NumFrame+nFrame;
		Channel[Index].Origin=1;  //对象O=1,C=0
		Channel[Index].CopyNum=0; //对象O=1,C=0
		CString sIndexCurve;
		for(int i=nFrame;;i++)
		{
			sIndexCurve.Format ("U-%d",1000+i);
			bool bExist=false;
			for(int j=0;j<Index;j++)
			{
				if(sIndexCurve.CompareNoCase(Channel[j].Name)==0)
				{
					bExist=true;
					break;
				}
			}
			if(!bExist) break;
		}
		strcpy(Channel[Index].Name,sIndexCurve);
		strcpy(Channel[Index].LongName,"");
		strcpy(Channel[Index].Properties,"");
		strcpy(Channel[Index].Units,Frame[0].IndexUnits);
		Channel[Index].RepCode=DLIS_REPC_FSINGL;
		Channel[Index].NumDim=1; //维数
		Channel[Index].Dimension[0]=1; //维元素
		Channel[Index].Sample=1;//采样数据个数
	}
}

void CDlisWrite::WriteChannel()
{
	CString s;
	int IpntLenC=0,NBYTE=0;
	SetIndexCurveInfo(); //设置索引曲线信息
	for(int i=0;i<NumChannel;i++)
	{
		Channel[i].Sample=1;	
		Channel[i].NumEle=Channel[i].NumDim;
		for(int j=0;j<Channel[i].NumDim;j++)
		{
			Channel[i].ElementLimt[j]=Channel[i].Dimension[j];
			Channel[i].Sample*=Channel[i].Dimension[j];
		}
	}

	PutCompSET("CHANNEL","53",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("LONG-NAME",    1, DLIS_REPC_ASCII,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("REPRESENTATION-CODE", 1, DLIS_REPC_USHORT,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("UNITS",        1, DLIS_REPC_UNITS,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("DIMENSION",    1, DLIS_REPC_UVARI,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("ELEMENT-LIMIT",1, DLIS_REPC_UVARI,"",IpntLenC,&LogBuf[IpntLenC]);
	if(NumAxis>0)
	{
		PutCompATTRIB("AXIS",         1, DLIS_REPC_OBNAME,"",IpntLenC,&LogBuf[IpntLenC]);
	}
	PutCompATTRIB("PROPERTIES",   1, DLIS_REPC_IDENT,"",IpntLenC,&LogBuf[IpntLenC]);

	fprintf(fp,"====Channel Number: %d\n",NumChannel);
	for(i=0;i<NumChannel;i++)
	{
		PutCompOBJECT(Channel[i].Origin,Channel[i].CopyNum,Channel[i].Name,IpntLenC,&LogBuf[IpntLenC]); //对象:曲线名
		if(strlen(Channel[i].LongName)>0)
		{
			DlisOut.Encode_ASCII(IpntLenC,&LogBuf[IpntLenC],Channel[i].LongName);	
		}
		else
		{
			DlisOut.Encode_NULL(IpntLenC,&LogBuf[IpntLenC]); //LONG-NAME
		}
		DlisOut.Encode_USHORT(IpntLenC,&LogBuf[IpntLenC],&Channel[i].RepCode);//REPRESENTATION-CODE
		DlisOut.Encode_UNITS(IpntLenC,&LogBuf[IpntLenC],Channel[i].Units);//UNITS
		DlisOut.Encode_UVARI(IpntLenC,&LogBuf[IpntLenC],Channel[i].Dimension,"",0,Channel[i].NumDim);//DIMENSION
		DlisOut.Encode_UVARI(IpntLenC,&LogBuf[IpntLenC],Channel[i].ElementLimt,"",0,Channel[i].NumEle);//ELEMENT-LIMIT
		if(NumAxis>0)
		{
			if(Channel[i].nAxis>0)
			{
				LogBuf[IpntLenC]=0X29;
				IpntLenC++;
				DlisOut.DLIS_UVARI(Channel[i].nAxis, NBYTE,&LogBuf[IpntLenC]);
				IpntLenC+=NBYTE;
				for(int j=0;j<Channel[i].nAxis;j++)
				{
					DlisOut.DLIS_OBNAME(Channel[i].Axis[j].Origin,Channel[i].Axis[j].CopyNum,Channel[i].Axis[j].Name,NBYTE,&LogBuf[IpntLenC]);
					IpntLenC+=NBYTE;
				}
			}
			else
			{
				DlisOut.Encode_NULL(IpntLenC,&LogBuf[IpntLenC]); //无轴属性
			}
		}
		if(strlen(Channel[i].Properties)>0)
		{
			DlisOut.Encode_IDENT(IpntLenC,&LogBuf[IpntLenC],Channel[i].Properties); //PROPERTIES
		}
		else
		{
			DlisOut.Encode_DEFAULT(IpntLenC,&LogBuf[IpntLenC]); //属性为空
		}
	}

	//送入磁盘缓冲区
	WriteLogBufToDlisBuf(1,3,IpntLenC,LogBuf,false);
}
void CDlisWrite::SetChannelNumber(int Num)
{
	NumChannel=Num+NumFrame; //其中有NumFrame条为索引曲线
	if(Channel)
	{
		delete []Channel; Channel=NULL;
	}
	Channel=new _Channel[NumChannel];
	//清空
	memset(Channel[0].Name,'\0',256);
	memset(Channel[0].LongName,'\0',256);
	memset(Channel[0].Properties,'\0',256);
	memset(Channel[0].Units,'\0',256);
	memset(Channel[0].Axis,'\0',256);
	memset(Channel[0].Source,'\0',256);
	for(int i=0;i<MAX_DIM_NUM;i++)
	{
		Channel[0].Axis[i].Origin=0;
		Channel[0].Axis[i].CopyNum=0;
		memset(Channel[0].Axis[i].Name,'\0',256);
		Channel[0].Dimension[i]=0;
		Channel[0].ElementLimt[i]=0;
		memset(Channel[0].AxisID[i],'\0',256);
		Channel[0].AxisStart[i]=0;
		Channel[0].AxisStep[i]=1;
		memset(Channel[0].AxisUnits[i],'\0',16);
	}
	Channel[0].nAxis=0;
	Channel[0].Origin=0;
	Channel[0].CopyNum=0;
	Channel[0].RepCode=0;
	Channel[0].NumDim=0;
	Channel[0].NumEle=0;
	memset(Channel[0].Source,'\0',256);
	Channel[0].Sample=1;
	Channel[0].RepCode=2;
	Channel[0].PcCode=1;
	for(i=1;i<NumChannel;i++)
	{
		Channel[i]=Channel[i-1];
	}
}
void CDlisWrite::SetFrameNumber(int Num)
{
	NumFrame=Num;
	if(Frame)
	{
		delete []Frame; Frame=NULL;
	}
	Frame=new _Frame[NumFrame];
	memset(Frame[0].Name,'\0',256);
	memset(Frame[0].Description,'\0',256);
	memset(Frame[0].Index_Type,'\0',256);
	memset(Frame[0].Direction,'\0',256);
	memset(Frame[0].IndexUnits,'\0',256);
	
	Frame[0].CopyNum=0;
	Frame[0].Encrypted=0;
	Frame[0].Origin=0;
	Frame[0].noFrame=0;
	Frame[0].NumCurve=0;
	Frame[0].IndexNum=-1;
	for(int j=0;j<MAX_FRAME_CHANNEL_NUM;j++)
	{
		Frame[0].ChannelID[j]=-1;
		Frame[0].bDecode[j]=true;
	}
	Frame[0].Index_Step=-999.25;
	Frame[0].Index_Start=-999.25;
	Frame[0].Index_Stop=-999.25;
	Frame[0].IndexFactor=1.0;
	Frame[0].bDiscrete=true;
	for(int i=1;i<NumFrame;i++)
	{
		Frame[i]=Frame[i-1];
	}
}
void CDlisWrite::SetDlisAxisNumber(int Num)
{
	NumAxis=Num;
	if(Axis)
	{
		delete []Axis; Axis=NULL;
	}
	if(NumAxis==0) return;
	Axis=new _Axis[NumAxis];
	memset(Axis[0].Name,'\0',256); //轴名，对应于对象名
	Axis[0].Origin=0; //源
	Axis[0].CopyNum=0; //拷贝数
	memset(Axis[0].AxisID,'\0',256); //轴ID号:C=1,R=IDENT
	Axis[0].NumC=0; //坐标数
	for(int i=0;i<MAX_DIM_NUM;i++)Axis[0].Coordinates[i]=0; //坐标
	memset(Axis[0].Units,'\0',16); //坐标单位
	Axis[0].Spacing=0; //间隔
	for(i=1;i<NumAxis;i++)
	{
		Axis[i]=Axis[i-1];
	}
}

void CDlisWrite::WriteFrame()
{
	CString s;
	int IpntLenC=0,NBYTE=0;
	FrameDataSize(); //计算各帧的长度
	CheckLogBufLen(); //检查并重新分配缓冲区长度
	PutCompSET("FRAME","54",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("CHANNELS",   1, DLIS_REPC_OBNAME,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("INDEX-TYPE", 1, DLIS_REPC_IDENT,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("DIRECTION",  1, DLIS_REPC_IDENT,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("SPACING",    1, DLIS_REPC_FSINGL,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("INDEX-MIN",  1, DLIS_REPC_FSINGL,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("INDEX-MAX",  1, DLIS_REPC_FSINGL,"",IpntLenC,&LogBuf[IpntLenC]);
	fprintf(fp,"====Frame Number: %d\n",NumFrame);
	for(int i=0;i<NumFrame;i++)
	{
		PutCompOBJECT(Frame[i].Origin,Frame[i].CopyNum,Frame[i].Name,IpntLenC,&LogBuf[IpntLenC]); //帧对象名
		//曲线名
		LogBuf[IpntLenC]=0X29;
		IpntLenC++;
		DlisOut.DLIS_UVARI(Frame[i].NumCurve+1, NBYTE,&LogBuf[IpntLenC]);
		IpntLenC+=NBYTE;
		//索引
		int index=NumChannel-NumFrame+i;
		DlisOut.DLIS_OBNAME(Channel[index].Origin,Channel[index].CopyNum,Channel[index].Name,NBYTE,&LogBuf[IpntLenC]);
		IpntLenC+=NBYTE;
		//其它曲线
		for(int j=0;j<Frame[i].NumCurve;j++)
		{
			index=Frame[i].ChannelID[j];
			DlisOut.DLIS_OBNAME(Channel[index].Origin,Channel[index].CopyNum,Channel[index].Name,NBYTE,&LogBuf[IpntLenC]);
			IpntLenC+=NBYTE;
		}
		//索引类型
		DlisOut.Encode_IDENT(IpntLenC,&LogBuf[IpntLenC],"BOREHOLE-DEPTH");
		//增量方向
		DlisOut.Encode_IDENT(IpntLenC,&LogBuf[IpntLenC],"INCREASING");
		//采样间隔
		DlisOut.Encode_FSINGL(IpntLenC,&LogBuf[IpntLenC],&Frame[i].Index_Step,Frame[i].IndexUnits);
		//INDEX-MIN
		DlisOut.Encode_FSINGL(IpntLenC,&LogBuf[IpntLenC],&Frame[i].Index_Start,Frame[i].IndexUnits);
		//INDEX-MAX
		DlisOut.Encode_FSINGL(IpntLenC,&LogBuf[IpntLenC],&Frame[i].Index_Stop,Frame[i].IndexUnits);
	}
	//送入磁盘缓冲区
	WriteLogBufToDlisBuf(1,4,IpntLenC,LogBuf,false);
}
void CDlisWrite::WriteAxis()
{
	CString s;
	int IpntLenC=0;
	if(NumAxis==0) return; //没有轴对象
	fprintf(fp,"====Axis Number: %d\n",NumAxis);
	PutCompSET("AXIS","55",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("AXIS-ID",    1, 0,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("COORDINATES",1, 0,"",IpntLenC,&LogBuf[IpntLenC]);
	PutCompATTRIB("SPACING",    1, 0,"",IpntLenC,&LogBuf[IpntLenC]);
	for(int i=0;i<NumAxis;i++)
	{
		PutCompOBJECT(Axis[i].Origin,Axis[i].CopyNum,Axis[i].Name,IpntLenC,&LogBuf[IpntLenC]);
		DlisOut.Encode_IDENT(IpntLenC,&LogBuf[IpntLenC],Axis[i].AxisID);
		DlisOut.Encode_FSINGL(IpntLenC,&LogBuf[IpntLenC],Axis[i].Coordinates,Axis[i].Units,DLIS_REPC_FSINGL,Axis[i].NumC);
		DlisOut.Encode_FSINGL(IpntLenC,&LogBuf[IpntLenC],&(Axis[i].Spacing),Axis[i].Units,DLIS_REPC_FSINGL,1);
	}
	WriteLogBufToDlisBuf(1,2,IpntLenC,LogBuf,false);
}
void CDlisWrite::FrameDataSize()
{
	fprintf(fp,"====Frame data Number: %d\n",NumFrame);
	for(int nItem=0;nItem<NumFrame;nItem++)
	{
		Frame[nItem].FdataSize=0;
		for(int i=0;i<Frame[nItem].NumCurve;i++)
		{
			int index=Frame[nItem].ChannelID[i];
			int CodeLen=DlisCodeLen(Channel[index].RepCode);
			Frame[nItem].OffSet[i]=Frame[nItem].FdataSize;
			Frame[nItem].FdataSize+=Channel[index].Sample*CodeLen;
		}
		fprintf(fp,"====Frame: %d , Curve Number: %d ,Data Size: %d\n",
			nItem+1,Frame[nItem].NumCurve,Frame[nItem].FdataSize);
		fflush(fp);
	}
}
int CDlisWrite::DlisCodeLen(BYTE RepCode)
{
	CString s;
	int CodeLen=1;
	switch(RepCode)
	{
	case  DLIS_REPC_FSHORT: //1:
		CodeLen=2;
		break;
	case  DLIS_REPC_FSINGL: //2:
		CodeLen=4;
		break;
	case  DLIS_REPC_FSING1: //3:
		CodeLen=8;
		break;
	case  DLIS_REPC_FSING2: //4:
		CodeLen=12;
		break;
	case  DLIS_REPC_ISINGL: //5:
		CodeLen=4;
		break;
	case  DLIS_REPC_VSINGL: //6:
		CodeLen=4;
		break;
	case  DLIS_REPC_FDOUBL: //7:
		CodeLen=8;
		break;
	case  DLIS_REPC_FDOUB1: //8:
		CodeLen=16;
		break;
	case  DLIS_REPC_FDOUB2: //9:
		CodeLen=24;
		break;
	case  DLIS_REPC_CSINGL: //10:
		CodeLen=8;
		break;
	case  DLIS_REPC_CDOUBL: //11:
		CodeLen=16;
		break;
	case  DLIS_REPC_SSHORT: //12:
		CodeLen=1;
		break;
	case  DLIS_REPC_SNORM: //13:
		CodeLen=2;
		break;
	case  DLIS_REPC_SLONG: //14:
		CodeLen=4;
		break;
	case  DLIS_REPC_USHORT: //15:
		CodeLen=1;
		break;
	case  DLIS_REPC_UNORM: //16:
		CodeLen=2;
		break;
	case  DLIS_REPC_ULONG: //17:
		CodeLen=4;
		break;
	case  DLIS_REPC_ATATUS: //26:
		CodeLen=1;
		break;
	case  DLIS_REPC_DTIME: //21:
		CodeLen=8;
		break;
	case  DLIS_REPC_UVARI: //18:
	case  DLIS_REPC_IDENT: //19:
	case  DLIS_REPC_ASCII: //20:
	case  DLIS_REPC_ORIGIN: //22:
	case  DLIS_REPC_OBNAME: //23:
	case  DLIS_REPC_OBJREF: //24:
	case  DLIS_REPC_ATTREF: //25:
	case  DLIS_REPC_UNITS: //27:
	default:
		CodeLen=0;
		s.Format("表示码:%u 数据长度没有定义,被设置为0",RepCode);
		AfxMessageBox(s);
		break;
	}
	return CodeLen;
}
void CDlisWrite::WriteCurveData(int nItem,int nCurve,int NumR,float *vf)
{
	BYTE buf[24];
	int index=Frame[nItem].ChannelID[nCurve];
	BYTE RepCode=Channel[index].RepCode;
	int NBYTE;
	for(int i=0;i<NumR;i++)
	{
		FloatToLogBuf(vf[i],RepCode,NBYTE,buf);
		int pos=i*Frame[nItem].FdataSize+Frame[nItem].OffSet[nCurve];
		fTmp.Seek(pos,CFile::begin);
		fTmp.Write(buf,NBYTE);
	}
}

void CDlisWrite::StartWriteData(int noFrame)
{
	WriteDlisBufDToisk(); //先将缓冲区数据写进磁盘
	CString s;
	if(IsFileName(sMidFile))
	{
		remove((const char*)sMidFile);
	}
	TCHAR szTempPath  [MAX_PATH];
	DWORD dwResult=:: GetTempPath (MAX_PATH, szTempPath);
	ASSERT (dwResult);
	sMidFile.Format("%s~DlisData.tmp",szTempPath);
	if(fTmp.Open(sMidFile,CFile::modeCreate|CFile::modeWrite)==FALSE) 
	{
		s.Format("创建临时文件 %s 出错!",sMidFile);
		AfxMessageBox(s,MB_ICONSTOP);
		return;
	}
	int NumPoint=int(fabs((Frame[noFrame].Index_Stop-Frame[noFrame].Index_Start)/Frame[noFrame].Index_Step)+0.5)+1;
	int fs=NumPoint*Frame[noFrame].FdataSize;
	fTmp.SetLength(fs);
	fTmp.SeekToBegin();
	fprintf(fp,"====Set Temporary File Length: %d\n",fs);
	fflush(fp);
}
void CDlisWrite::StopWriteData(int noFrame)
{
	CString s;
	fTmp.Close();
	if(Frame[noFrame].FdataSize==0)
	{
		s.Format ("CDlisWrite::StopWriteData出错:FdataSize=%d",Frame[noFrame].FdataSize);
		AfxMessageBox(s);
		return;
	}
	if(fTmp.Open(sMidFile,CFile::modeRead|CFile::shareDenyNone)==FALSE) 
	{
		return;
	}
	int fs=fTmp.GetLength();
	int NumFra=fs/Frame[noFrame].FdataSize;
	//从临时文件中读取数据到DLIS帧中
	int IpntLenC=0,NBYTE=0,ObjLen=0;
	//帧对象名
	DlisOut.DLIS_OBNAME(Frame[noFrame].Origin,Frame[noFrame].CopyNum,Frame[noFrame].Name,ObjLen,&LogBuf[0]);
	for(int i=0;i<NumFra;i++)
	{
		//帧序号
		DlisOut.DLIS_UVARI(i+1,NBYTE,&LogBuf[ObjLen]);
		IpntLenC=ObjLen+NBYTE;
		//帧深度
		float TDEP=Frame[noFrame].Index_Start+i*Frame[noFrame].Index_Step;
		DlisOut.DLIS_FSINGL(TDEP,NBYTE,&LogBuf[IpntLenC]);
		//帧数据体
		IpntLenC+=NBYTE;
		fTmp.Read(&LogBuf[IpntLenC],Frame[noFrame].FdataSize);
		IpntLenC+=Frame[noFrame].FdataSize;
		//输出
		WriteLogBufToDlisBuf(0,0,IpntLenC,LogBuf,false);
	}
	fTmp.Close();
	//输出帧数据结束记录
	WriteLogBufToDlisBuf(0,127,ObjLen,LogBuf,true);
	remove(sMidFile);
}
void CDlisWrite::CheckLogBufLen()
{
	for(int nItem=0;nItem<NumFrame;nItem++)
	{
		int Size=Frame[nItem].FdataSize+128+strlen(Frame[nItem].Name);
		if(Size>LogBufLen)
		{
			if(Size>1024*1024*4)
			{	//如果超过4兆,说明程序运行出现问题
				CString s;
				s.Format ("问题:Size=%d LogBufLen=%d Frame[%d].FdataSize=%d",
					Size,LogBufLen,nItem,Frame[nItem].FdataSize);
				AfxMessageBox(s);
			}
			delete []LogBuf; LogBuf=NULL;
			LogBufLen=((Size-1)/1024+1)*1024;
			LogBuf=new BYTE[LogBufLen];
			////////////////////////////////////////////////////////////////////////////////
			fprintf(fp,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
			fprintf(fp,"%40s: %u\n","Buffer Length",LogBufLen);
			fprintf(fp,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
			fflush(fp);
		}
	}
}
void CDlisWrite::FloatToLogBuf(float vf,BYTE RepCode,int &NBYTE,BYTE *buf)
{
	CString s;
	short vShort;
	char vChar;
	long vLong;
	double vDouble;
	BYTE vByte;
	WORD vWord;
	DWORD vDword;
	switch(RepCode)
	{
	case DLIS_REPC_FSINGL:
		DlisOut.DLIS_FSINGL(vf,NBYTE,buf);
		break;
	case DLIS_REPC_SNORM:
		vShort=short(vf);
		DlisOut.DLIS_SNORM(vShort,NBYTE,buf);
		break;
	case DLIS_REPC_SSHORT:
		vChar=char(vf);
		buf[0]=vChar;
		NBYTE=1;
		break;
	case DLIS_REPC_SLONG:
		vLong=long(vf);
		DlisOut.DLIS_SLONG(vLong,NBYTE,buf);
		break;
	case DLIS_REPC_FDOUBL:
		vDouble=double(vf);
		DlisOut.DLIS_FDOUBL(vDouble,NBYTE,buf);
		break;
	case DLIS_REPC_USHORT:
		vByte=BYTE(vf);
		buf[0]=vByte;
		NBYTE=1;
		break;
	case DLIS_REPC_UNORM:
		vWord=WORD(vf);
		DlisOut.DLIS_UNORM(vWord,NBYTE,buf);
		break;
	case DLIS_REPC_ULONG:
		vDword=DWORD(vf);
		DlisOut.DLIS_ULONG(vDword,NBYTE,buf);
		break;
	default:
		s.Format ("表示码: %u 不对",RepCode);
		AfxMessageBox(s);
		break;
	}
}

void CDlisWrite::WriteArrayData(int nItem,int nCurve,int noFra,int Count,float *vFloat)
{
	CString s;
	int IpntLenC=0,NBYTE=0;
	int index=Frame[nItem].ChannelID[nCurve];
	BYTE RepCode=Channel[index].RepCode;

	for(int i=0;i<Count;i++)
	{
		FloatToLogBuf(vFloat[i],RepCode,NBYTE,&LogBuf[IpntLenC]);
		IpntLenC+=NBYTE;
	}
	int pos=noFra*Frame[nItem].FdataSize+Frame[nItem].OffSet[nCurve];
	fTmp.Seek(pos,CFile::begin);
	fTmp.Write(LogBuf,IpntLenC);
}
void CDlisWrite::WriteArrayData(int nItem,int nCurve,int noFra,int Count,char *vChar)
{
	CString s;
	int index=Frame[nItem].ChannelID[nCurve];
	BYTE RepCode=Channel[index].RepCode;
	if(RepCode!=DLIS_REPC_SSHORT)
	{
		AfxMessageBox("错误调用:WriteArrayData() 输入数据类型不是Char");
		return;
	}
	int pos=noFra*Frame[nItem].FdataSize+Frame[nItem].OffSet[nCurve];
	fTmp.Seek(pos,CFile::begin);
	fTmp.Write(vChar,Count);
}
void CDlisWrite::WriteArrayData(int nItem,int nCurve,int noFra,int Count,short *vShort)
{
	CString s;
	int IpntLenC=0;
	int index=Frame[nItem].ChannelID[nCurve];
	BYTE RepCode=Channel[index].RepCode;
	if(RepCode!=DLIS_REPC_SNORM)
	{
		AfxMessageBox("错误调用:WriteArrayData() 输入数据类型不是Short");
		return;
	}
	for(int i=0;i<Count;i++)
	{
		LogBuf[IpntLenC]=(vShort[i]>>8)&0XFF;
		LogBuf[IpntLenC+1]=vShort[i]&0XFF;
		IpntLenC+=2;
	}
	int pos=noFra*Frame[nItem].FdataSize+Frame[nItem].OffSet[nCurve];
	fTmp.Seek(pos,CFile::begin);
	fTmp.Write(LogBuf,IpntLenC);
}

void CDlisWrite::Prt_Header()
{
	fprintf(fp,"***************************************************************************\n");
	fprintf(fp,"*%30s: %s[%s]\n","Producer",Origin.ProducerName,Origin.ProducerCode);
	fprintf(fp,"*%30s: %s\n","Company Name",Origin.Company);
	fprintf(fp,"*%30s: %s\n","Well Name",Origin.WellName);
	fprintf(fp,"*%30s: %s\n","Field Name",Origin.FieldName);
	fprintf(fp,"*%30s: %G\n","Default Value",Origin.DefaultVal);
	fprintf(fp,"***************************************************************************\n");
	for(int i=0;i<NumFrame;i++)
	{
		fprintf(fp,"%20s: Origin=%u Copy=%u Name=%s  \n",
			"Frame",Frame[i].Origin,Frame[i].CopyNum,Frame[i].Name); //帧对象名
		//STEP
		fprintf(fp,"%14s: %f %s\n","INDEX-STEP",Frame[i].Index_Step,Frame[i].IndexUnits);
		//INDEX-MIN
		fprintf(fp,"%14s: %f %s\n","INDEX-MIN",Frame[i].Index_Start,Frame[i].IndexUnits);
		//INDEX-MAX
		fprintf(fp,"%14s: %f %s\n","INDEX-MAX",Frame[i].Index_Stop,Frame[i].IndexUnits);
		//曲线名
		fprintf(fp,"%9s:","Curves");
		for(int j=0;j<Frame[i].NumCurve;j++)
		{
			int index=Frame[i].ChannelID[j];
			fprintf(fp," %-12.255s",Channel[index].Name);
			if((j+1)%5==0)
			{
				fprintf(fp,"\n%10s"," ");
			}
		}
		fprintf(fp,"\n");
	}
	fprintf(fp,"***************************************************************************\n");
}

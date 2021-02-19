// WisWrite.cpp: implementation of the CWisWrite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WisWrite.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWisWrite::CWisWrite()
{

}

CWisWrite::~CWisWrite()
{

}
bool CWisWrite::Open(CString sDataFile)
{
	if(fWis.Open(sDataFile,CFile::modeCreate|CFile::modeReadWrite)==FALSE)
	{
		return false;
	}
	char *Wis_Label="WIS 1.0   ";
	fWis.Write(Wis_Label,10);
	return true;
}
void CWisWrite::Close()
{
	fWis.Close();
}
void CWisWrite::InitWisHead(int NumObject)
{
	WisHead.MachineType=0;  // 机器类型：0-PC  1-SUN   2-IBM  3-HP
	WisHead.MaxObjectNumber=((NumObject-1)/512+1)*512;
	WisHead.ObjectNumber=0;
	WisHead.BlockLen=1024;
	WisHead.EntryOffset=10+sizeof(WisHead);
	DWORD n=WisHead.EntryOffset+WisHead.MaxObjectNumber*sizeof(WIS_OBJECT_ENTRY);
	WisHead.DataOffset=((n-1)/WisHead.BlockLen+1)*WisHead.BlockLen;
	WisHead.FileSize=WisHead.DataOffset;
	time(&WisHead.TimeCreate);  //时间，单位为秒
	for(int i=0;i<32;i++)WisHead.Reserved[i]=0X0; //保留值
	WriteWisHead();
}
void CWisWrite::WriteWisHead()
{
	fWis.Seek(10,CFile::begin);
	fWis.Write(&WisHead,sizeof(WisHead));
	fWis.SetLength(WisHead.FileSize);
}

DWORD CWisWrite::GetWisLength()
{
	WIS_OBJECT_ENTRY ObjectEntry;
	CString s;
	DWORD fs=WisHead.DataOffset;
	for(int i=0;i<WisHead.ObjectNumber;i++)
	{
		DWORD pos=WisHead.EntryOffset+i*sizeof(WIS_OBJECT_ENTRY);
		fWis.Seek(pos,CFile::begin);
		fWis.Read(&ObjectEntry,sizeof(WIS_OBJECT_ENTRY));
		DWORD n=ObjectEntry.Position+ObjectEntry.BlockNum*WisHead.BlockLen;
		if(fs<n) fs=n;
	}
	return fs;
}
void CWisWrite::WriteChannel()
{
	m_ObjectEntry->Status=0;
	time(&m_ObjectEntry->TimeCreate);
	time(&m_ObjectEntry->TimeWrite);
	memset(m_ObjectEntry->Reserved,'\0',32);
	m_ObjectEntry->Position=GetWisLength();
	//数据占用块数(含一块通道信息块)
	m_ObjectEntry->BlockNum=(WisChannel->DimInfo[0].Size*WisChannel->DimInfo[0].Samples-1)/WisHead.BlockLen+2;

	/////////////////////////////////
	WORD IndexObject=WisHead.ObjectNumber; //当前对象索引号
	
	//写对象
	DWORD pos=WisHead.EntryOffset+IndexObject*sizeof(WIS_OBJECT_ENTRY);
	fWis.Seek(pos,CFile::begin);
	fWis.Write(m_ObjectEntry,sizeof(WIS_OBJECT_ENTRY));

	//写通道
	fWis.Seek(m_ObjectEntry->Position,CFile::begin);
	fWis.Write(WisChannel,sizeof(WIS_CHANNEL));

	//修改对象数及文件长度、写文件头
	WisHead.ObjectNumber++;
	WisHead.FileSize=m_ObjectEntry->Position+m_ObjectEntry->BlockNum*WisHead.BlockLen;
	WriteWisHead();
}
void CWisWrite::WriteGendata(int count,float *buffer)
{
	DWORD pos=m_ObjectEntry->Position+WisHead.BlockLen;
	WriteDataToWis(pos,WisChannel->RepCode,count,buffer);
}
void CWisWrite::WriteWave(float depth,float *buffer)
{
	CString s;
	if(WisChannel->NumOfDimension != 2) 
	{
		return; //必须是二维曲线
	}
	if(depth<WisChannel->DimInfo[0].StartVal)
	{
		return; //深度不对
	}
	DWORD np=DWORD((depth-WisChannel->DimInfo[0].StartVal)/WisChannel->DimInfo[0].Delta+0.5);
	DWORD count=WisChannel->DimInfo[1].Samples;
	DWORD BlockSize=WisChannel->DimInfo[0].Size;
	DWORD pos=m_ObjectEntry->Position+WisHead.BlockLen+np*BlockSize;
	WriteDataToWis(pos,WisChannel->RepCode,count,buffer);
}
void CWisWrite::WriteWaveArray(float depth,float *buffer)
{
	CString s;
	if(WisChannel->NumOfDimension < 2) 
	{
		return; //必须是二维或三维曲线
	}
	if(depth<WisChannel->DimInfo[0].StartVal)
	{
		return; //深度不对
	}
	DWORD np=DWORD((depth-WisChannel->DimInfo[0].StartVal)/WisChannel->DimInfo[0].Delta+0.5);
	DWORD count=1;
	for(int i=1;i<WisChannel->NumOfDimension;i++)
	{
		count*=WisChannel->DimInfo[i].Samples;
	}
	DWORD BlockSize=count*WisChannel->CodeLen;
	DWORD pos=m_ObjectEntry->Position+WisHead.BlockLen+np*BlockSize;
	WriteDataToWis(pos,WisChannel->RepCode,count,buffer);
}
void CWisWrite::WriteDataToWis(DWORD pos,WORD nRepCode,DWORD count,float *buffer)
{
	CString s;
	int *iBuf=NULL;
	short *hBuf=NULL;
	long *lBuf=NULL;
	double *dfBuf=NULL;
	char *cBuf=NULL;
	BYTE *ucBuf=NULL;
	unsigned short *uhBuf=NULL;
	unsigned int *uiBuf=NULL;
	unsigned long *ulBuf=NULL;
	DWORD i;
	fWis.Seek(pos,CFile::begin);
	switch(nRepCode)
	{
	case WIS_REPR_INT: //有符号整型
		iBuf=new int[count];
		for(i=0;i<count;i++) iBuf[i]=int(buffer[i]);
		fWis.Write(iBuf,count*sizeof(int));
		delete []iBuf; iBuf=NULL;
		break;
	case WIS_REPR_SHORT: //有符号短整型数
		hBuf=new short[count];
		for(i=0;i<count;i++) hBuf[i]=short(buffer[i]);
		fWis.Write(hBuf,count*sizeof(short));
		delete []hBuf; hBuf=NULL;
		break;
	case WIS_REPR_LONG: //有符号长整型数
		lBuf=new long[count];
		for(i=0;i<count;i++) lBuf[i]=long(buffer[i]);
		fWis.Write(lBuf,count*sizeof(long));
		delete []lBuf; lBuf=NULL;
		break;
	case WIS_REPR_FLOAT: //IEEE单精度浮点数	
		fWis.Write(buffer,count*sizeof(float));
		break;
	case WIS_REPR_DOUBLE: //IEE双精度浮点数
		dfBuf=new double[count];
		for(i=0;i<count;i++) dfBuf[i]=double(buffer[i]);
		fWis.Write(dfBuf,count*sizeof(double));
		delete []dfBuf; dfBuf=NULL;
		break;
	case WIS_REPR_STRING: //字符串
		//无法转换
		break;
	case WIS_REPR_CHAR: //有符号字节数
		cBuf=new char[count];
		for(i=0;i<count;i++) 
		{
			cBuf[i]=char(buffer[i]);
		}
		fWis.Write(cBuf,count*sizeof(char));
		delete []cBuf; cBuf=NULL;
		break;
	case WIS_REPR_UCHAR: //无符号字节数
		ucBuf=new unsigned char[count];
		for(i=0;i<count;i++) ucBuf[i]=unsigned char(buffer[i]);
		fWis.Write(ucBuf,count*sizeof(unsigned char));
		delete []ucBuf; ucBuf=NULL;
		break;
	case WIS_REPR_USHORT: //无符号短整型数
		uhBuf=new unsigned short[count];
		for(i=0;i<count;i++) uhBuf[i]=unsigned short(buffer[i]);
		fWis.Write(uhBuf,count*sizeof(unsigned short));
		delete []uhBuf; uhBuf=NULL;
		break;
	case WIS_REPR_UINT: //无符号整型
		uiBuf=new unsigned int[count];
		for(i=0;i<count;i++) uiBuf[i]=unsigned int(buffer[i]);
		fWis.Write(uiBuf,count*sizeof(unsigned int));
		delete []uiBuf; uiBuf=NULL;
		break;
	case WIS_REPR_ULONG: //无符号长整型数
		ulBuf=new unsigned long[count];
		for(i=0;i<count;i++) ulBuf[i]=unsigned long(buffer[i]);
		fWis.Write(ulBuf,count*sizeof(unsigned long));
		delete []ulBuf; ulBuf=NULL;
		break;
	}
}
void CWisWrite::PrintHead(FILE *fp)
{
	fprintf(fp,"------- WIS FILE HEADER ---------\n");
	fprintf(fp,"%+16s %d\n","MachineType:",WisHead.MachineType);
	fprintf(fp,"%+16s %d\n","MaxObjectNumber:",WisHead.MaxObjectNumber);
	fprintf(fp,"%+16s %d\n","ObjectNumber:",WisHead.ObjectNumber);
	fprintf(fp,"%+16s %d\n","BlockLen:",WisHead.BlockLen);
	fprintf(fp,"%+16s %d\n","EntryOffset:",WisHead.EntryOffset);
	fprintf(fp,"%+16s %d\n","DataOffset:",WisHead.DataOffset);
	fprintf(fp,"%+16s %d\n","FileSize:",WisHead.FileSize);
	fprintf(fp,"%+16s %s\n","TimeCreate:",asctime(localtime(&WisHead.TimeCreate)));
}
void CWisWrite::InitWisObjectEntry()
{
	memset(m_ObjectEntry->Name,'\0',16);
	m_ObjectEntry->Status=0;
	m_ObjectEntry->Attribute=0;
	m_ObjectEntry->SubAttribute=0;
	m_ObjectEntry->Position=0;
	m_ObjectEntry->BlockNum=0;
	time(&m_ObjectEntry->TimeCreate);
	time(&m_ObjectEntry->TimeWrite);
	memset(m_ObjectEntry->Reserved,'\0',32);
}
void CWisWrite::InitWisChannel()
{
	memset(WisChannel->Unit,'\0',8);
	memset(WisChannel->AliasName,'\0',16);
	memset(WisChannel->AliasUnit,'\0',16);
	WisChannel->RepCode=0;
	WisChannel->CodeLen=0;
	WisChannel->MinVal=0;
	WisChannel->MaxVal=0;
	WisChannel->Reserved=0;
	WisChannel->NumOfDimension=0;
	for(int i=0;i<4;i++)
	{
		memset(WisChannel->DimInfo[i].Name,'\0',8);
		memset(WisChannel->DimInfo[i].Unit,'\0',8);
		memset(WisChannel->DimInfo[i].AliasName,'\0',16);
		WisChannel->DimInfo[i].StartVal=0;
		WisChannel->DimInfo[i].Delta=0;
		WisChannel->DimInfo[i].Samples=0;
		WisChannel->DimInfo[i].MaxSamples=0;
		WisChannel->DimInfo[i].Size=0;
		WisChannel->DimInfo[i].RepCode=0;
		WisChannel->DimInfo[i].Reserved=0;
	}
}
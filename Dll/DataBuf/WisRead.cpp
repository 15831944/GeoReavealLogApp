// WisRead.cpp: implementation of the CWisRead class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WisRead.h"
#include <ctype.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWisRead::CWisRead()
{
	m_ObjectEntry=NULL;
}

CWisRead::~CWisRead()
{
	if(m_ObjectEntry)
	{
		delete []m_ObjectEntry; 
		m_ObjectEntry=NULL;
	}

}
bool _declspec(dllexport) IsWIS(CString sDataFile)
{
	CString s;
	CFile f;
	char Wis_Label[10];
	char str[11];
	if(f.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE) return false;
	f.Read(Wis_Label,10);
	f.Close();
	sprintf(str,"%-7.7s",Wis_Label);
	if(strcmp(str,"WIS 1.0")!=0)
	{
		return false;
	}
	return true;
}
bool CWisRead::Open(CString sDataFile)
{
	if(fWis.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE)
	{
		return false;
	}
	WORD MachineType;
	fWis.Seek(10,CFile::begin);
	fWis.Read(&MachineType,sizeof(WORD));
	if(MachineType != 0 )
	{
		//0-PC  1-SUN   2-IBM	3-HP
		fWis.Close(); //不是PC机的WIS文件，目前不能识别
		return false;
	}
	FileSize=fWis.GetLength(); //文件长度
	return true;
}
bool CWisRead::GetHead()
{
	fWis.Seek(10,CFile::begin);
	fWis.Read(&WisHead,sizeof(WisHead));
	return true;
}
bool CWisRead::GetObjectEntry()
{
	CString s;
	m_ObjectEntry=new WIS_OBJECT_ENTRY[WisHead.ObjectNumber];
	for(int i=0;i<WisHead.ObjectNumber;i++)
	{
		DWORD pos=WisHead.EntryOffset+i*sizeof(m_ObjectEntry[i]);
		fWis.Seek(pos,CFile::begin);
		fWis.Read(&m_ObjectEntry[i],sizeof(m_ObjectEntry[i]));
	}
	return true;
}
void CWisRead::Close()
{
	fWis.Close();
}
int CWisRead::OpenChannel(const char *name)
{
	int index=-1;
	CString s,SearChannelName;
	SearChannelName.Format("%s",name);
	SearChannelName.TrimRight();
	for(int i=0;i<WisHead.ObjectNumber;i++)
	{
		s.Format("%s",m_ObjectEntry[i].Name);
		s.TrimRight();
		if(SearChannelName.CompareNoCase(s)==0)
		{
			index=i;
			break;
		}
	}
	if(index<0)
	{
		return -1;
	}
	fWis.Seek(m_ObjectEntry[index].Position,CFile::begin);
	fWis.Read(&WisChannel,sizeof(WisChannel));

	s.Format("%-.7s",WisChannel.Unit);
	s.TrimRight();
	if(s.CompareNoCase("(null)")==0 || s.CompareNoCase("null")==0
		|| s.CompareNoCase("none")==0 || s.CompareNoCase("----")==0)
	{
		s.Empty();
	}
	if(s.IsEmpty())
	{
		sprintf(WisChannel.Unit,"%-.8s","none");
	}
	s.Format("%-.15s",WisChannel.AliasName);
	s.TrimRight();
	if(s.IsEmpty())
	{
		sprintf(WisChannel.AliasName,"%-.15s","none");
	}
	s.Format("%-.15s",WisChannel.AliasUnit);
	s.TrimRight();
	if(s.IsEmpty())
	{
		sprintf(WisChannel.AliasUnit,"%-.15s","none");
	}
	if(WisChannel.NumOfDimension<1 || WisChannel.NumOfDimension>3)
	{
		return -1;
	}
	for(i=0;i<WisChannel.NumOfDimension;i++)
	{
		s.Format("%-.8s",WisChannel.DimInfo[i].Name);
		s.TrimRight();
		if(s.IsEmpty())
		{
			sprintf(WisChannel.DimInfo[i].Name,"%-.7s","none");
		}
		s.Format("%-.8s",WisChannel.DimInfo[i].Unit);
		s.TrimRight();
		if(s.IsEmpty())
		{
			sprintf(WisChannel.DimInfo[i].Unit,"%-.7s","none");
		}
		s.Format("%-.16s",WisChannel.DimInfo[i].AliasName);
		s.TrimRight();
		if(s.IsEmpty())
		{
			sprintf(WisChannel.DimInfo[i].Unit,"%-.15s","none");
		}

	}
	return index;
}

DWORD CWisRead::ReadData(DWORD pos,int nRepCode,DWORD count,float *buffer)
{
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
		fWis.Read(iBuf,count*sizeof(int));
		for(i=0;i<count;i++) buffer[i]=float(iBuf[i]);
		delete []iBuf; iBuf=NULL;
		break;
	case WIS_REPR_SHORT: //有符号短整型数
		hBuf=new short[count];
		fWis.Read(hBuf,count*sizeof(short));
		for(i=0;i<count;i++) buffer[i]=float(hBuf[i]);
		delete []hBuf; hBuf=NULL;
		break;
	case WIS_REPR_LONG: //有符号长整型数
		lBuf=new long[count];
		fWis.Read(lBuf,count*sizeof(long));
		for(i=0;i<count;i++) buffer[i]=float(lBuf[i]);
		delete []lBuf; lBuf=NULL;
		break;
	case WIS_REPR_FLOAT: //IEEE单精度浮点数	
		fWis.Read(buffer,count*sizeof(float));
		break;
	case WIS_REPR_DOUBLE: //IEE双精度浮点数
		dfBuf=new double[count];
		fWis.Read(dfBuf,count*sizeof(double));
		for(i=0;i<count;i++) buffer[i]=float(dfBuf[i]);
		delete []dfBuf; dfBuf=NULL;
		break;
	case WIS_REPR_STRING: //字符串
		//无法转换
		break;
	case WIS_REPR_CHAR: //有符号字节数
		cBuf=new char[count];
		fWis.Read(cBuf,count*sizeof(char));
		for(i=0;i<count;i++) buffer[i]=float(cBuf[i]);
		delete []cBuf; cBuf=NULL;
		break;
	case WIS_REPR_UCHAR: //无符号字节数
		ucBuf=new unsigned char[count];
		fWis.Read(ucBuf,count*sizeof(unsigned char));
		for(i=0;i<count;i++) buffer[i]=float(ucBuf[i]);
		delete []ucBuf; ucBuf=NULL;
		break;
	case WIS_REPR_USHORT: //无符号短整型数
		uhBuf=new unsigned short[count];
		fWis.Read(uhBuf,count*sizeof(unsigned short));
		for(i=0;i<count;i++) buffer[i]=float(uhBuf[i]);
		delete []uhBuf; uhBuf=NULL;
		break;
	case WIS_REPR_UINT: //无符号整型
		uiBuf=new unsigned int[count];
		fWis.Read(uiBuf,count*sizeof(unsigned int));
		for(i=0;i<count;i++) buffer[i]=float(uiBuf[i]);
		delete []uiBuf; uiBuf=NULL;
		break;
	case WIS_REPR_ULONG: //无符号长整型数
		ulBuf=new unsigned long[count];
		fWis.Read(ulBuf,count*sizeof(unsigned long));
		for(i=0;i<count;i++) buffer[i]=float(ulBuf[i]);
		delete []ulBuf; ulBuf=NULL;
		break;
	}	
	return count;
}
DWORD CWisRead::ReadCurve(int index, float depth, DWORD count, float* buffer)
{
	if(WisChannel.NumOfDimension>1) 
	{
		return 0; //非常规数据
	}
	DWORD pos=m_ObjectEntry[index].Position+WisHead.BlockLen;
	ReadData(pos,WisChannel.RepCode,count,buffer);
	return count;
}

bool CWisRead::ReadWave(int index,float depth,float *buffer)
{
	CString s;
	if(WisChannel.NumOfDimension != 2) 
	{
		return false; //必须是二维曲线
	}
	if(depth<WisChannel.DimInfo[0].StartVal)
	{
		return false; //深度不对
	}
	DWORD np=DWORD((depth-WisChannel.DimInfo[0].StartVal)/WisChannel.DimInfo[0].Delta+0.5);
	DWORD count=WisChannel.DimInfo[1].Samples;
	DWORD BlockSize=WisChannel.DimInfo[0].Size;
	DWORD pos=m_ObjectEntry[index].Position+WisHead.BlockLen+np*BlockSize;
	ReadData(pos,WisChannel.RepCode,count,buffer);
	return true;
}
bool CWisRead::ReadWaveArray(int index,float depth,float *buffer)
{
	CString s;
	if(WisChannel.NumOfDimension < 2) 
	{
		return false; //必须是二维或三维曲线
	}
	if(depth<WisChannel.DimInfo[0].StartVal)
	{
		return false; //深度不对
	}
	DWORD np=DWORD((depth-WisChannel.DimInfo[0].StartVal)/WisChannel.DimInfo[0].Delta+0.5);
	DWORD count=1;
	for(int i=1;i<WisChannel.NumOfDimension;i++)
	{
		count*=WisChannel.DimInfo[i].Samples;
	}
	DWORD BlockSize=count*WisChannel.CodeLen;
	DWORD pos=m_ObjectEntry[index].Position+WisHead.BlockLen+np*BlockSize;
	ReadData(pos,WisChannel.RepCode,count,buffer);
	return true;
}
int CWisRead::GetCurveDim(int index)
{
	if(m_ObjectEntry[index].Status != 0) return -1; //不是正常对象
	if(m_ObjectEntry[index].Attribute != 1)return -1; //不是通道对象
	if(m_ObjectEntry[index].BlockNum<1) return -1; //没有数据
	if(!isalnum(m_ObjectEntry[index].Name[0])) return -1; //对象名不符合曲线名要求
	//读通道信息
	OpenChannel(m_ObjectEntry[index].Name);
	//判断是否为连续采样通道
	bool bIsContinue=true;
	for(int i=0;i<WisChannel.NumOfDimension;i++)
	{
		if(WisChannel.DimInfo[i].Delta==0)
		{
			bIsContinue=false;
			break;
		}
	}
	if(!bIsContinue)return -1; //不是连续采样通道,如FMT等数据
	return WisChannel.NumOfDimension;
}
void CWisRead::Prt_HeaderInfo(FILE *fp)
{
	fprintf(fp,"---------------------Header information---------------------\n");
	if(WisHead.MachineType == 1)
		fprintf(fp,"%+16s %s\n","MachineType:","SUN");
	else if(WisHead.MachineType == 2) 
		fprintf(fp,"%+16s %s\n","MachineType:","IBM");
	else if(WisHead.MachineType == 3)
		fprintf(fp,"%+16s %s\n","MachineType:","HP"); 
	else 
		fprintf(fp,"%+16s %s\n","MachineType:","PC"); 
	fprintf(fp,"%+16s %d\n","MaxObjectNumber:",WisHead.MaxObjectNumber);
	fprintf(fp,"%+16s %d\n","ObjectNumber:",WisHead.ObjectNumber);
	fprintf(fp,"%+16s %d\n","BlockLen:",WisHead.BlockLen);
	fprintf(fp,"%+16s %d\n","EntryOffset:",WisHead.EntryOffset);
	fprintf(fp,"%+16s %d\n","DataOffset:",WisHead.DataOffset);
	fprintf(fp,"%+16s %d\n","FileSize:",WisHead.FileSize);
	fprintf(fp,"%+16s %s\n\n\n","TimeCreate:",asctime(localtime(&WisHead.TimeCreate)));
}

void CWisRead::Prt_ObjectInfo(FILE *fp)
{
	int NumOk=0;
	int NumDiscard=0;
	int NumDeleted=0;
	int NumTable=0;
	int NumStream=0;
	fprintf(fp,"-------------------All object information-------------------\n");
	for(int i=0;i<WisHead.ObjectNumber;i++)
	{
		if(m_ObjectEntry[i].Status==0)
			NumOk++;
		else if(m_ObjectEntry[i].Status==1)
			NumDiscard++;
		else if(m_ObjectEntry[i].Status==2)
			NumDeleted++;
		else
			continue;
	}
	fprintf(fp,"√Available:%d\n",NumOk);
	fprintf(fp,"≮  Ignored:%d\n",NumDiscard);
	fprintf(fp,"× Delected:%d\n\n",NumDeleted);

	fprintf(fp,"No. %-10s %-4s %-6s %-6s %-8s %-8s\n",
		"Name","Stat","Attrib","SubAtt","Position","BlockNum");
	for(i=0;i<WisHead.ObjectNumber;i++)
	{
		fprintf(fp,"%-3d",i);
		fprintf(fp," %-10.16s",m_ObjectEntry[i].Name);
		if(m_ObjectEntry[i].Status==0)
			fprintf(fp," %-4s","√");
		else if(m_ObjectEntry[i].Status==1)
			fprintf(fp," %-4s","≮");
		else if(m_ObjectEntry[i].Status==2)
			fprintf(fp," %-4s","×");
		else
		{
			fprintf(fp,"ERROR\n");
			continue;
		}
		
		if(m_ObjectEntry[i].Attribute==1)
			fprintf(fp," %-8s","CHANNEL");
		else if(m_ObjectEntry[i].Attribute==2)
		{
			fprintf(fp," %-6s","TABLE");
			if(m_ObjectEntry[i].Status==0)
				NumTable++;
		}
		else if(m_ObjectEntry[i].Attribute==3)
		{
			fprintf(fp," %-7s","STREAM");
			if(m_ObjectEntry[i].Status==0)
				NumStream++;
		}
		fprintf(fp," %-6d",m_ObjectEntry[i].SubAttribute);
		fprintf(fp," %-8d",m_ObjectEntry[i].Position);
		fprintf(fp," %-8d",m_ObjectEntry[i].BlockNum);
		fprintf(fp,"\n");
	}
	fprintf(fp,"\n");
	if(NumTable>0)
		Prt_TableInfo(fp); 
	if(NumStream>0)
		Prt_StreamInfo(fp); 
}
void CWisRead::Prt_ChannelInfo(FILE *fp)
{
	fprintf(fp,"\n---------------Available curve information----------------\n");
	fprintf(fp,"%-8s %-8s %4s %3s","CHANNELS","UNITS","Code","Dim");
	fprintf(fp," %-5s %4s %8s %8s %7s","Index","Unit","Start","Delta","Samples");
	fprintf(fp," %-5s %4s %8s %8s %7s","Index","Unit","Start","Delta","Samples");
	fprintf(fp,"\n");
	for(int i=0;i<WisHead.ObjectNumber;i++)
	{
		if(m_ObjectEntry[i].Status!=0) continue; //不是正常对象
		if(m_ObjectEntry[i].Attribute!=1) continue; //不是通道对象
		if(OpenChannel(m_ObjectEntry[i].Name)<0) continue; //通道对象有问题
		fprintf(fp,"%-8.15s",m_ObjectEntry[i].Name);
		fprintf(fp," %-8.7s",WisChannel.Unit);
		fprintf(fp," %4d",WisChannel.RepCode);
		fprintf(fp," %3d",WisChannel.NumOfDimension);
		for(int j=0;j<WisChannel.NumOfDimension;j++)
		{
			fprintf(fp," %-5.7s",WisChannel.DimInfo[j].Name);
			fprintf(fp," %-4.7s",WisChannel.DimInfo[j].Unit);
			fprintf(fp," %8G",WisChannel.DimInfo[j].StartVal);
			fprintf(fp," %8G",WisChannel.DimInfo[j].Delta);
			fprintf(fp," %4u",WisChannel.DimInfo[j].Samples);
		}
		fprintf(fp,"\n");
	}
	fprintf(fp,"\n");
}

void  CWisRead::Prt_TableInfo(FILE *fp) //打印表信息
{
	WIS_TABLE_FIELD *pField;
	long NumRecord,NumField;
	int i,j,k;
	fprintf(fp,"\n---------------Available table information----------------\n");
	for(i=0;i<WisHead.ObjectNumber;i++)
	{
		if(m_ObjectEntry[i].Status!=0) continue; //不是正常对象
		if(m_ObjectEntry[i].Attribute!=2) continue; //不是通道对象
		if(OpenChannel(m_ObjectEntry[i].Name)<0) continue; //通道对象有问题
		fprintf(fp,"Name:%s\n\n",m_ObjectEntry[i].Name);
		fWis.Seek(m_ObjectEntry[i].Position,CFile::begin);
		fWis.Read(&NumRecord,4);
		fWis.Read(&NumField,4);
		pField=new WIS_TABLE_FIELD[NumField];
		fWis.Read(pField,NumField*sizeof(WIS_TABLE_FIELD));
		for(j=0;j<NumField;j++)
			fprintf(fp,"%-32s",pField[j].Name);			
		fprintf(fp,"\n");
		CString str;
		for(k=0;k<NumRecord;k++)
		{
			for(j=0;j<NumField;j++)
			{		
				switch(pField[j].RepCode)
				{
				case 2:
					short d2;
					fWis.Read(&d2,pField[j].Length);
					str.Format ("%d",d2);
					break;
				case 3:
					long d3;
					fWis.Read(&d3,pField[j].Length);
					str.Format ("%d",d3);
					break;
				case 4:
					float d4;
					fWis.Read(&d4,pField[j].Length);
					str.Format ("%.4f",d4);
					break;
				case 5:
					double d5;
					fWis.Read(&d5,pField[j].Length);
					str.Format ("%.4f",d5);
					break;
				case 7:
					unsigned short d7;
					fWis.Read(&d7,pField[j].Length);
					str.Format ("%d",d7);
					break;
				default:
					unsigned long d8;
					fWis.Read(&d8,pField[j].Length);
					str.Format ("%d",d8);
					break;
				}
				fprintf(fp,"%-32s ",str);
			}
		}		
		delete pField;
		fprintf(fp,"\n\n");
	}
}

void  CWisRead::Prt_StreamInfo(FILE *fp) //打印流信息
{
	fprintf(fp,"\n---------------Available Stream information---------------\n");
	char* buff=NULL;
	unsigned long StreamLength;
	for(int i=0;i<WisHead.ObjectNumber;i++)
	{
		if(m_ObjectEntry[i].Status!=0) continue; //不是正常对象
		if(m_ObjectEntry[i].Attribute!=3) continue; //不是通道对象
		if(OpenChannel(m_ObjectEntry[i].Name)<0) continue; //通道对象有问题
		fprintf(fp,"Name:%s\n\n",m_ObjectEntry[i].Name );
		fWis.Seek(m_ObjectEntry[i].Position,CFile::begin);
		fWis.Read(&StreamLength,4);
		buff=new char[StreamLength];
		fWis.Read(buff,StreamLength);
		fprintf(fp,"%s\n\n",buff);
		delete buff;
		buff=NULL;
	}
    fprintf(fp,"\n\n");
}
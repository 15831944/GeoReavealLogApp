// LisRead.cpp: implementation of the CLisRead class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LisRead.h"
#include <math.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLisRead::CLisRead()
{
	MaxPhyLen=8192;
	Curve=NULL;
}

CLisRead::~CLisRead()
{
	if(IsFileName(sMidFile))
	{
		remove((const char*)sMidFile);
	}

	if(Curve)
	{
		delete Curve;
		Curve=NULL;
	}
	
}
bool _declspec(dllexport) IsLIS(CString sDataFile)
{
	CString s;
	bool bLis=false; //�Ƿ�ΪLIS�ļ�
	CFile f;
	if(f.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE) return false;
	DWORD Pos=0;
	WORD TapRecLen;
	int NumCheckRec=0;
	BYTE Buf[6],BRecType;
	for(int i=0;;i++)
	{
		if(Pos>=f.GetLength()) break;
		NumCheckRec++;
		if(NumCheckRec>3) 
		{
			//����Ѽ�����㹻�ļ�¼������Ϊ�Ƿ���Ҫ���ļ�
			bLis=true;
			break;
		}
		f.Seek(Pos,CFile::begin);
		f.Read(Buf,6);
		TapRecLen=256*Buf[0]+Buf[1];
		if(i==0)
		{
			if(Buf[4]!=132 && Buf[4]!=130 && Buf[4]!=128)
			{
				bLis=false;
				break;
			}
		}
		if((Buf[2]&0x40)!=0)
		{
			//�����¼���ͱ�־������Ϊ0
			bLis=false;
			break;
		}
		if((Buf[3]&0x02)==0)
		{
			BRecType=Buf[4];	//��ǰ����,�õ��߼���¼����
		}
		
		Pos+=TapRecLen;
		if(Pos==f.GetLength()) 
		{
			bLis=true; //�����ļ�β
			break;
		}
		if(Pos>f.GetLength())
		{
			if(BRecType==129 || BRecType==131 || BRecType==133)
			{
				bLis=true; //û���ļ�β������һ��¼Ϊ�߼��ļ�β���β���β����Ϊ����������Ƕ�����ֽ�
			}
			else
			{
				bLis=false;
			}
			break; //һ�������¼����СӦ����4�ֽ�
		}
	}
	f.Close();
	return bLis;
}
bool CLisRead::Open(CString sDataFile)
{
	if(fLis.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE)
	{
		return false;
	}
	m_sDataFile=sDataFile;
	FileSize=fLis.GetLength(); //�ļ�����
	LisPos=0;
	LisRecPos=0;
	ResetTapeHeader();
	ReadTapeHeader();
	LisPos=0;
	LisRecPos=0;
	return true;
}
void CLisRead::ResetTapeHeader()
{
	memset(TapeHead.SerName,' ',6); //�����
	memset(TapeHead.Nul1,' ',6); //Undefined
	memset(TapeHead.Date,' ',8); //�γ��ļ�����
	memset(TapeHead.Nul2,' ',2); //Undefined
	memset(TapeHead.OriginData,' ',4);//������Դ
	memset(TapeHead.Nul3,' ',2);//Undefined
	memset(TapeHead.Name,' ',8);//����
	memset(TapeHead.Nul4,' ',2); //Undefined
	memset(TapeHead.ConnectNum,' ',2);//����߼����Ӻ�
	memset(TapeHead.Nul5,' ',2); //Undefined
	memset(TapeHead.PreConName,' ',8);//ǰ�ھ���
	memset(TapeHead.Nul6,' ',2); //Undefined
	memset(TapeHead.Comments,' ',74); //ע��
	ReelHead=TapeHead;
}

void CLisRead::ReadTapeHeader()
{
	DWORD nCount=0; //�߼���¼����
	for(DWORD rec=0;rec<2;rec++)
	{
		if(GetLogicalRecord(nCount)<1) 
		{
			break;
		}
		switch(RecHead.type)
		{
		case 132:  //Reel Header
			LisBufToReelHeader();
			break;
		case 130:  //Tape Header
			LisBufToTapeHeader();
			break;
		}
	}
}
void CLisRead::Close()
{
	fLis.Close();
}
DWORD CLisRead::GetLogicalRecPos()
{
	return LisRecPos;
}
void CLisRead::SetLisPos(DWORD pos)
{
	LisPos=pos;
	LisRecPos=pos;
}
DWORD CLisRead::GetLogicalFilePos()
{
	return LisLogicalFileStartPos;
}
void CLisRead::ScanLisFile(int NumPrtFdata,int NumPrtSample,BOOL bPrtBaseInfo,FILE *fp)
{
	m_NumPrtFdata=NumPrtFdata; //��ʾ֡������
	m_NumPrtSample=NumPrtSample; //��ʾ������(һ��)
	m_bPrtBaseInfo=bPrtBaseInfo;
	CString s;
	DWORD nCount=0; //�߼���¼����
	DWORD NumInfoBlock=0; //һ���߼��ļ��е���Ϣ��¼��
	DWORD NumDFIR=0; //һ���߼��ļ��еĸ�ʽ˵����Ϣ��¼��
	DWORD NumLogFile=0; //һ�̴����߼��ļ���
	DWORD NumDataRecord=0; //���ݼ�¼
	DWORD NumPrtFra=0; //��ӡ֡��
	bool bLogicalHead=false;
	LisPos=0;
	for(DWORD rec=0;;rec++)
	{
		if(GetLogicalRecord(nCount)<1) break;
		switch(RecHead.type)
		{
		case 132:  //Reel Header
			LisBufToReelHeader();
			Prt_LisReelHeader(fp);
			break;
		case 133:  //Reel Trailer
			LisBufToReelTrailer();
			Prt_LisReelTrailer(fp);
			break;
		case 130:  //Tape Header
			LisBufToTapeHeader();
			Prt_LisTapeHeader(fp);
			break;
		case 131:  //Tape Trailer
			LisBufToTapeTrailer();
			Prt_LisTapeTrailer(fp);
			break;
		case 128:  //File Header
			if(bLogicalHead)
			{
				fprintf(fp,"\n            *** %s �е� %d ���߼��ļ����ļ�������¼ ***\n\n",m_sDataFile,NumLogFile);
			}
			bLogicalHead=true;
			LisLogicalFileStartPos=LisRecPos; //�߼��ļ���ʼλ��
			LisBufToFileHeader();
			NumLogFile++;
			NumInfoBlock=0;
			NumDFIR=0;
			NumDataRecord=0;
			NumPrtFra=0;
			NumPoint=0;
			StDep=-999.25;
			EnDep=-999.25;
			Rlev=-999.25;
			NumLog=0;
			if(NumLogFile==1)
			{
				fprintf(fp,"-------------------------------------------------------------\n");
			}
			fprintf(fp,"\n            *** No. %d Logical File ***\n",NumLogFile);
			Prt_LisFileHeader(fp);
			break;
		case 129:  //File Trailer
			bLogicalHead=false;
			LisLogicalFileStopPos=LisPos; //�߼��ļ�����λ��
			LisBufToFileTrailer();
			Prt_WellInfo(fp);
			Prt_LisFileTrailer(fp);
			fprintf(fp,"-------------------------------------------------------------\n\n");
			break;
		case 34:  //Information
			NumInfoBlock++;
			if(bPrtBaseInfo)
			{
				fprintf(fp,"--------------- No. %d Information Blocks Start ---------------\n",NumInfoBlock);
				LisBufToComponentBlocks(nCount,fp);
				fprintf(fp,"--------------- No. %d Information Blocks End ---------------\n\n",NumInfoBlock);
			}
			break;
		case 64:  //Data format specify
			NumDFIR++;
			if(NumDFIR==1)
			{
				ResetEntryValue();
				LisBufToEntryValue(nCount,fp);
				fprintf(fp,"\n      *** Data Format Specification Record Start ***\n");
				Prt_EntryBlock(fp);
				//fprintf(fp,"            *** Data Format Specification Record End ***\n");
			}
			break;
		case 0: //Data Record
			LisBufToDepth(nCount);
			if(NumPrtFra<DWORD(m_NumPrtFdata))
			{
				Prt_LisData(nCount,NumPrtFra,fp);
			}
			break;
		default:
			//s.Format ("��¼����:%u\n",RecHead.type);
			//AfxMessageBox(s);
			break;
		}
	}
	fprintf(fp,"Logical File Number : %u\n",NumLogFile);
	if(LisPos>FileSize)
	{
		fprintf(fp,"�ļ���β���쳣\n");
	}
	else if(LisPos<FileSize)
	{
		fprintf(fp,"�ļ�û�ж���\n");
	}
	LisPos=0;
}
bool CLisRead::ScanLisOneLogicalFile(FILE *fp)
{
	CString s;
	DWORD nCount=0; //�߼���¼����
	DWORD NumDFIR=0; //һ���߼��ļ��еĸ�ʽ˵����Ϣ��¼��
	bool LogicalFileEnd=false;
	bool HasLogicalFile=false;
	DWORD PreLisRecPos=0;
	DWORD PreLisPos=0;
	ResetHeader();
	for(DWORD rec=0;;rec++)
	{
		PreLisRecPos=LisRecPos; //��һ�߼���¼������ʼλ��
		PreLisPos=LisPos; //LIS�ļ���һ��¼λ��
		if(GetLogicalRecord(nCount)<1)
		{
			break;
		}
		switch(RecHead.type)
		{
		case 128:  //File Header
			if(HasLogicalFile)
			{
				fprintf(fp,"\n            *** %s �е�ǰ�߼��ļ����ļ�������¼ ***\n\n",m_sDataFile);
				LogicalFileEnd=true;
				LisRecPos=PreLisRecPos; //��һ�߼���¼������ʼλ��
				LisPos=PreLisPos; //LIS�ļ���һ��¼λ��
				break;
			}
			LisBufToFileHeader();
			LisLogicalFileStartPos=LisRecPos;
			StDep=-999.25;
			EnDep=-999.25;
			Rlev=-999.25;
			NumPoint=0;
			HasLogicalFile=true;
			NumDatRec=0;
			//fprintf(fp,"\n--------------- No. %d Logical File ---------------\n",NumLogFile);
			//Prt_LisFileHeader(fp);
			break;
		case 129:  //File Trailer
			LogicalFileEnd=true;
			LisPos=LisPos;
			break;
		case 64:  //Data format specify
			NumDFIR++;
			if(NumDFIR==1)
			{
				ResetEntryValue();
				LisBufToEntryValue(nCount,fp);
				fprintf(fp,"\n--------------- Data Format Specification Record Start ---------------\n");
				Prt_EntryBlock(fp);
				fprintf(fp,"--------------- Data Format Specification Record End ---------------\n");
			}
			break;
		case 0: //Data Record
			NumDatRec++;
			if(NumDatRec==1)
			{
				LisDatStartPos=LisRecPos;
			}
			LisBufToDepth(nCount);
			break;
		default:
			//fprintf(fp,"Record Type=%u\n",LisBuf[0]);
			break;
		}
		if(LogicalFileEnd) break;
	}
	LisLogicalFileStopPos=LisPos;
	LisRecPos=LisLogicalFileStartPos;
	if(EnVa.DepMod==0)
	{
		Rlev=(EnDep-StDep)/(NumPoint-1);
	}
	if(NumDFIR>0)
	{
		fprintf(fp,"Data Depth form StartDep = %G to EndDep = %G\n",StDep,EnDep);
		fprintf(fp,"Level Spacing = %G    FramNumber = %u\n",Rlev,NumPoint);
		fprintf(fp,"NumLog = %d\n",NumLog);
	}
	else
	{
		if(HasLogicalFile)
		{
			fprintf(fp,"This Logical No Data\n");
		}
	}
	fflush(fp);
	return HasLogicalFile;
}
int CLisRead::GetLogicalRecord(DWORD &Count)
{
	CString s;
	Count=0;
	while(1)
	{
		if(LisPos>=FileSize)
		{
			return 0; //�ѵ��ļ�β
		}
		BYTE PhyBuf[4]; //����ͷ��¼��Ϣ
		LisRecPos=LisPos;
		fLis.Seek(LisPos,CFile::begin);
		fLis.Read(PhyBuf,4); //������ͷ
		LisGetPhyHead(PhyBuf); //�ֽ�����ͷ����
		if(PhyHead.Length>MaxPhyLen)
		{
			return -1; //�����¼���ȴ��ڶ���
		}
		if(PhyHead.Length<4)
		{	
			return -2; //�ļ�β�����쳣
		}
		LisPos+=PhyHead.Length;
		int NumTrail=LisGetTrailLen(); //�����¼β����(�ֽ���)
		DWORD BlockSize=PhyHead.Length-NumTrail-4;//һ�������¼��ʵ�����ݳ���
		if(!PhyHead.Predecessor_Continuation_Bit)
		{	
			//��ǰ��λ��һ���߼���¼��ʼ
			Count=0;
		}
		if((Count+BlockSize)>MAX_LIS_BLOCKLEN)
		{
			return -3; //�߼���¼���ȴ��ڶ���
		}
		fLis.Read(&LisBuf[Count],BlockSize);
		Count+=BlockSize; //�ۼ��߼���¼����
		if(!PhyHead.Predecessor_Continuation_Bit)
		{
			RecHead.type=LisBuf[0];
		}
		if(!PhyHead.Successor_Continuation_Bit) break;
	}
	return 1;
}
void CLisRead::LisGetPhyHead(BYTE *PhyBuf)
{
	PhyHead.Length=256L*PhyBuf[0]+PhyBuf[1];
	PhyHead.Physical_Record_Type=0x01&(PhyBuf[2]>>6);
	PhyHead.Checksum_Type_Bit1=0x01&(PhyBuf[2]>>5);
	PhyHead.Checksum_Type_Bit2=0x01&(PhyBuf[2]>>4);
	PhyHead.File_Number_Presence=0x01&(PhyBuf[2]>>2);
	PhyHead.Record_Number_Presence=0x01&(PhyBuf[2]>>1);
	PhyHead.Parity_Error=0x01&(PhyBuf[3]>>6);
	PhyHead.Checksum_Error=0x01&(PhyBuf[3]>>5);
	PhyHead.Predecessor_Continuation_Bit=0x01&(PhyBuf[3]>>1);
	PhyHead.Successor_Continuation_Bit=0x01&PhyBuf[3];
}
int CLisRead::LisGetTrailLen()
{
	int NumTrail=0;
	if(PhyHead.Checksum_Type_Bit2)    NumTrail +=2;
	if(PhyHead.File_Number_Presence)  NumTrail +=2;
	if(PhyHead.Record_Number_Presence) NumTrail +=2;
	if(NumTrail>6) NumTrail=6;
	return NumTrail;
}
void CLisRead::LisBufToReelHeader()
{
	memcpy(ReelHead.SerName,&LisBuf[2],6);
	memcpy(ReelHead.Nul1,&LisBuf[8],6);
	memcpy(ReelHead.Date,&LisBuf[14],8);
	memcpy(ReelHead.Nul2,&LisBuf[22],2);
	memcpy(ReelHead.OriginData,&LisBuf[24],4);
	memcpy(ReelHead.Nul3,&LisBuf[28],2);
	memcpy(ReelHead.Name,&LisBuf[30],8);
	memcpy(ReelHead.Nul4,&LisBuf[38],2);
	memcpy(ReelHead.ConnectNum,&LisBuf[40],2);
	memcpy(ReelHead.Nul5,&LisBuf[42],2);
	memcpy(ReelHead.PreConName,&LisBuf[44],8);
	memcpy(ReelHead.Nul6,&LisBuf[52],2);
	memcpy(ReelHead.Comments,&LisBuf[54],74);
}
void CLisRead::LisBufToReelTrailer()
{
	memcpy(ReelTrail.SerName,&LisBuf[2],6);
	memcpy(ReelTrail.Nul1,&LisBuf[8],6);
	memcpy(ReelTrail.Date,&LisBuf[14],8);
	memcpy(ReelTrail.Nul2,&LisBuf[22],2);
	memcpy(ReelTrail.OriginData,&LisBuf[24],4);
	memcpy(ReelTrail.Nul3,&LisBuf[28],2);
	memcpy(ReelTrail.Name,&LisBuf[30],8);
	memcpy(ReelTrail.Nul4,&LisBuf[38],2);
	memcpy(ReelTrail.ConnectNum,&LisBuf[40],2);
	memcpy(ReelTrail.Nul5,&LisBuf[42],2);
	memcpy(ReelTrail.PreConName,&LisBuf[44],8);
	memcpy(ReelTrail.Nul6,&LisBuf[52],2);
	memcpy(ReelTrail.Comments,&LisBuf[54],74);
}

void CLisRead::LisBufToTapeHeader()
{
	memcpy(TapeHead.SerName,&LisBuf[2],6);
	memcpy(TapeHead.Nul1,&LisBuf[8],6);
	memcpy(TapeHead.Date,&LisBuf[14],8);
	memcpy(TapeHead.Nul2,&LisBuf[22],2);
	memcpy(TapeHead.OriginData,&LisBuf[24],4);
	memcpy(TapeHead.Nul3,&LisBuf[28],2);
	memcpy(TapeHead.Name,&LisBuf[30],8);
	memcpy(TapeHead.Nul4,&LisBuf[38],2);
	memcpy(TapeHead.ConnectNum,&LisBuf[40],2);
	memcpy(TapeHead.Nul5,&LisBuf[42],2);
	memcpy(TapeHead.PreConName,&LisBuf[44],8);
	memcpy(TapeHead.Nul6,&LisBuf[52],2);
	memcpy(TapeHead.Comments,&LisBuf[54],74);
}
void CLisRead::LisBufToTapeTrailer()
{
	memcpy(TapeTrail.SerName,&LisBuf[2],6);
	memcpy(TapeTrail.Nul1,&LisBuf[8],6);
	memcpy(TapeTrail.Date,&LisBuf[14],8);
	memcpy(TapeTrail.Nul2,&LisBuf[22],2);
	memcpy(TapeTrail.OriginData,&LisBuf[24],4);
	memcpy(TapeTrail.Nul3,&LisBuf[28],2);
	memcpy(TapeTrail.Name,&LisBuf[30],8);
	memcpy(TapeTrail.Nul4,&LisBuf[38],2);
	memcpy(TapeTrail.ConnectNum,&LisBuf[40],2);
	memcpy(TapeTrail.Nul5,&LisBuf[42],2);
	memcpy(TapeTrail.PreConName,&LisBuf[44],8);
	memcpy(TapeTrail.Nul6,&LisBuf[52],2);
	memcpy(TapeTrail.Comments,&LisBuf[54],74);
}
void CLisRead::LisBufToFileHeader()
{
	memcpy(FileHead.Name,&LisBuf[2],10);  //Filename
	memcpy(FileHead.Nul1,&LisBuf[12],2);  //Undefined
	memcpy(FileHead.SubSerName,&LisBuf[14],6); //Sub-Service Name
	memcpy(FileHead.Version,&LisBuf[20],8);  //Version
	memcpy(FileHead.Date,&LisBuf[28],8); //Date
	FileHead.Nul2=LisBuf[36];  //Undefined
	memcpy(FileHead.MaxPRL,&LisBuf[37],5); //Maximum Physical Record Length
	memcpy(FileHead.Nul3,&LisBuf[42],2); //Undefined
	memcpy(FileHead.FileType,&LisBuf[44],2); //File Type
	memcpy(FileHead.Nul4,&LisBuf[46],2); //Undefined
	memcpy(FileHead.PreConName,&LisBuf[48],10); //Predecessor Continuation Filename
}
void CLisRead::LisBufToFileTrailer()
{
	memcpy(FileTrail.Name,&LisBuf[2],10);  //Filename
	memcpy(FileTrail.Nul1,&LisBuf[12],2);  //Undefined
	memcpy(FileTrail.SubSerName,&LisBuf[14],6); //Sub-Service Name
	memcpy(FileTrail.Version,&LisBuf[20],8);  //Version
	memcpy(FileTrail.Date,&LisBuf[28],8); //Date
	FileTrail.Nul2=LisBuf[36];  //Undefined
	memcpy(FileTrail.MaxPRL,&LisBuf[37],5); //Maximum Physical Record Length
	memcpy(FileTrail.Nul3,&LisBuf[42],2); //Undefined
	memcpy(FileTrail.FileType,&LisBuf[44],2); //File Type
	memcpy(FileTrail.Nul4,&LisBuf[46],2); //Undefined
	memcpy(FileTrail.PreConName,&LisBuf[48],10); //Predecessor Continuation Filename
}
void CLisRead::LisBufToComponentBlocks(DWORD nCount,FILE *fp)
{
	CString s;
	DWORD pos=2;
	for(int NumInfoBlock=0;;NumInfoBlock++)
	{
		if(pos>=(nCount-12))
		{
			break; //һ���ɷݿ�ĳ�����С��12�ֽ�
		}
		CompBlock.Type=LisBuf[pos];
		CompBlock.Code=LisBuf[pos+1];
		CompBlock.Length=LisBuf[pos+2];
		CompBlock.Category=LisBuf[pos+3];
		memcpy(CompBlock.Name,&LisBuf[pos+4],4);
		memcpy(CompBlock.Units,&LisBuf[pos+8],4);
		BYTE buf[256];
		char szFormat[256];
		if(CompBlock.Length>0)
		{
			memcpy(buf,&LisBuf[pos+12],CompBlock.Length);
		}
		switch(CompBlock.Type)
		{
		case 73:
            fprintf(fp,"%.4s:%.4s",CompBlock.Name,(const char*)buf);
            break;
		case 0:
			fprintf(fp,"\n");
		case 69:
			switch(CompBlock.Code)
			{
			case 49:  //16 bits float
				fprintf(fp," %.4s=%-8G",CompBlock.Name,LisFloat16(buf));
				break;
			case 50:  //32 bits low identify float
				fprintf(fp," %.4s=%-8G",CompBlock.Name,LisFloat32Low(buf));
				break;
			case 56:  //8 bits char
				fprintf(fp," %.4s=%-8d",CompBlock.Name,Lis8Integer(buf));
				break;
			case 65:  //ASCII chain
				sprintf(szFormat," %%.4s=%%-%d.%ds",8,CompBlock.Length);
				fprintf(fp,szFormat,CompBlock.Name,buf);
				break;
			case 66:  //8 bits unsigned char
				fprintf(fp," %.4s=%-8d",CompBlock.Name,Lis8Unsigned(buf));
				break;
			case 68: //32 bits float
				fprintf(fp," %.4s=%-8G",CompBlock.Name,LisFloat32(buf));
				break;
			case 70:  //32 bits fixed-point
				fprintf(fp," %.4s=%-8G",CompBlock.Name,LisFloat32Fix(buf));
				break;
			case 73:  //32 bits Integer
				fprintf(fp," %.4s=%-8d",CompBlock.Name,Lis32Integer(buf));
				break;
			case 77:  //screen ruler
				sprintf(szFormat," %%.4s=%%-%d.%ds",8,CompBlock.Length);
				fprintf(fp,szFormat,CompBlock.Name,buf);
				break;
			case 79:  //16 bits integer
				fprintf(fp," %.4s=%-8d",CompBlock.Name,Lis16Integer(buf));
				break;
			case 128:  //rough block
				sprintf(szFormat," %%.4s=%%-%d.%ds",8,CompBlock.Length);
				fprintf(fp,szFormat,CompBlock.Name,buf);
				break;
			default:
				fprintf(fp," %.4s=%-8s",CompBlock.Name,"Undefined");
				break;
			}
		}
		pos=pos+12+CompBlock.Length;
	}
	fprintf(fp,"\n");
}
void CLisRead::LisBufToEntryValue(DWORD nCount,FILE *fp)
{
	CString s;
	DWORD pos=2;
	//���ݸ�ʽ˵����¼��ǰ���֣���Ŀ��
	while(1)
	{
		if(pos>=nCount) break;
		Entry.Type=LisBuf[pos];
		Entry.Length=LisBuf[pos+1];
		Entry.Code=LisBuf[pos+2];
		BYTE buf[256];
		float fValue=0;
		if(Entry.Type==0) 
		{
			pos+=Entry.Length+3;
			break; //��Ŀ�����
		}
		memcpy(buf,&LisBuf[pos+3],Entry.Length);
		fValue=LisBufToFloat(Entry.Code,buf);
		switch(Entry.Type)
		{
		case 1:
            EnVa.DaRecTyp=unsigned char(fValue);
            break;
		case 2:
            EnVa.DSBTyp=unsigned char(fValue);
            break;
		case 3:
            EnVa.DaFraSiz = WORD(fValue);
            break;
		case 4:
            EnVa.UDFlag = unsigned char(fValue);
            break;
		case 5:
            EnVa.DepUnitFlag=unsigned char(fValue);
            break;
		case 6:
            EnVa.DaRefP = int(fValue);
            break;
		case 7:
			memcpy(EnVa.DaRefUnt,buf,4);
            break;
		case 8:
            EnVa.FraSpace = float(fValue);
            break;
		case 9:
			memcpy(EnVa.FraUnt,buf,4);
            break;
		case 10:
            break;
		case 11:
            EnVa.MaxFraRec = unsigned char(fValue);
            break;
		case 12:
            EnVa.AbsVal = fValue;
            break;
		case 13:
            EnVa.DepMod = unsigned char(fValue);
            break;
		case 14:
			memcpy(EnVa.DepUnt,buf,4);
            break;
		case 15:
            EnVa.DepRpc = unsigned char(fValue);
            break;
		case 16:
            EnVa.DSBSTyp = unsigned char(fValue);
            break;
		}
		pos+=Entry.Length+3;
    }
	//Get Depth Transform coefficient & Rlev
	DepScale=GetDepScaleFromLisUnits(EnVa.DepUnt);
	NumLog=(nCount-pos)/40;//���ݸ�ʽ˵����¼���ȼ���������
	if(Curve)
	{
		delete Curve;
		Curve=NULL;
	}
	Curve=new CurveInfo[NumLog];
	for(int i=0;i<NumLog;i++)
	{
		memcpy(Dsbr.Mnemon,&LisBuf[pos],4);
		memcpy(Dsbr.ServID,&LisBuf[pos+4],6);
		memcpy(Dsbr.ServOrd,&LisBuf[pos+10],8);
		memcpy(Dsbr.Units,&LisBuf[pos+18],4);
		Dsbr.APICode=Lis32Integer(&LisBuf[pos+22]);
		Dsbr.FileNumber=Lis16Integer(&LisBuf[pos+26]);
		Dsbr.Size=Lis16Integer(&LisBuf[pos+28]);
		Dsbr.Nul=Lis16Integer(&LisBuf[pos+30]);
		Dsbr.ProLvl=LisBuf[pos+32];
		Dsbr.Samples=LisBuf[pos+33];
		Dsbr.RepC=LisBuf[pos+34];
		memcpy(Dsbr.ProInd,&LisBuf[pos+35],5);
		//��˵������Ϣ����������Ϣ�б���
		memcpy(Curve[i].Name,Dsbr.Mnemon,4);
		memcpy(Curve[i].Units,Dsbr.Units,4);
		if(Curve[i].Units[0]==' ')
		{
			memcpy(Curve[i].Units,"none",4);
		}
		memcpy(Curve[i].ServID,Dsbr.ServID,6);
		memcpy(Curve[i].ServOrd,Dsbr.ServOrd,8);
		Curve[i].APICode=Dsbr.APICode;
		Curve[i].FileNumber=Dsbr.FileNumber;
		memcpy(Curve[i].ProInd,Dsbr.ProInd,5);
		Curve[i].Samples=Dsbr.Samples;
		Curve[i].Size=Dsbr.Size;
		Curve[i].ProLvl=Dsbr.ProLvl;
		Curve[i].iRepC=Dsbr.RepC;
		Curve[i].iCodLen=GetCodeLength(Dsbr.RepC);
		Curve[i].oRepC=LisRepCToPcRepC(Dsbr.RepC);
		Curve[i].oCodLen=GetObjCodeLen(Curve[i].oRepC);
		Curve[i].NumX=Curve[i].Size/(Curve[i].iCodLen*Curve[i].Samples);
		if(Curve[i].NumX>1)
		{
			Curve[i].NumOfDimension=2;
		}
		else
		{
			Curve[i].NumOfDimension=1;
		}
		if(i==0)
		{
			Curve[i].OffSet=0;
		}
		else
		{
			Curve[i].OffSet=Curve[i-1].OffSet+Curve[i-1].Size;
		}
		//////////////////////////////////////
		pos+=40; //��һ��˵�����λ��
	}
	EnVa.DaFraSiz=0;
	for(i=0;i<NumLog;i++)
	{
		EnVa.DaFraSiz+=Curve[i].Size;
	}

	if(EnVa.DepMod==0)
	{
		DepScale=GetDepScaleFromLisUnits(Curve[0].Units);
	}
	memcpy(EnVa.DepUnt,"M   ",4);
	Rlev=float(EnVa.FraSpace*DepScale);
	if(EnVa.UDFlag==1) Rlev=-Rlev;
	
}
void CLisRead::ResetHeader()
{
	ResetEntryValue();
	DepScale=1; //���ת��ϵ��
	Rlev=-999.25; //�������
	StDep=-999.25; //��ʼ���
	EnDep=-999.25; //��ֹ���
	NumLog=0; //������
	NumPoint=0; //��������
}
void CLisRead::ResetEntryValue()
{
	EnVa.DaRecTyp=0; //1:���ݼ�¼���߼�����
	EnVa.DSBTyp=0; //2:����˵��������
	EnVa.DaFraSiz=0; //3:���ݼ�¼��ÿ֡���ȣ��ֽڣ�
	EnVa.UDFlag=1; //4:�ϲ�/�²��־,1Ϊ�ϲ�
	EnVa.DepUnitFlag=1; //5:��ȵ�λ��־,1ΪӢ��,255Ϊ��,0Ϊʱ��
	EnVa.DaRefP=0; //6:���ݲο��㵽����������Ծ���
	sprintf(EnVa.DaRefUnt,".1IN"); //7:���ݲο��㵽����������Ծ��뵥λ,����Ϊ0.1in(25.4mm)
	EnVa.FraSpace=0; //8:֮֡��Ĳ�����ࡣ����,�ɸ��������ȼ���
	sprintf(EnVa.FraUnt,".1IN");//9:�������ĵ�λ������Ϊ0.1in(25.4mm)
	EnVa.MaxFraRec=0;//11:ÿ����¼�Ĵ�֡�����ɸ���ÿ֡��ʵ�ʳ��Ⱥ�ʵ�ʼ�¼���ȼ���
	EnVa.AbsVal=-999.25;//12:ȱʡֵ,һ��Ϊ-999.25
	EnVa.DepMod=0; //13:��ȼ�¼��ʽ��־��1��ʾ��ȼ�¼��ÿ֡ǰ(��λ����14��15��)
	sprintf(EnVa.DepUnt,".1IN"); //14:���ֵ������λ����ȷ�ʽΪ1ʱ,����Ϊ0.1in(25.4mm)
	EnVa.DepRpc=73; //15:���ֵ��Ϣ����(��ȷ�ʽΪ1ʱ)
	EnVa.DSBSTyp=0; //16:����˵���������ͺ�
	NumLog=0;
}

void CLisRead::Prt_LisReelHeader(FILE *fp)
{
	fprintf(fp,"\n           *** Reel Header Record ***\n");
	fprintf(fp,"            Service Name  =  %.6s\n",ReelHead.SerName);
	fprintf(fp,"                    Date  =  %.8s\n",ReelHead.Date);
	fprintf(fp,"          Origin of Data  =  %.4s\n",ReelHead.OriginData);
	fprintf(fp,"               Reel Name  =  %.8s\n", ReelHead.Name);
	fprintf(fp,"Reel Continuation Number  =  %.2s\n",ReelHead.ConnectNum);
	fprintf(fp,"      Previous Reel Name  =  %.8s\n",ReelHead.PreConName);
	fprintf(fp,"Comments: %.74s\n",ReelHead.Comments);
}
void CLisRead::Prt_LisReelTrailer(FILE *fp)
{
	fprintf(fp,"\n           *** Reel Trailer Record ***\n");
	fprintf(fp,"            Service Name  =  %.6s\n",ReelTrail.SerName);
	fprintf(fp,"                    Date  =  %.8s\n",ReelTrail.Date);
	fprintf(fp,"          Origin of Data  =  %.4s\n",ReelTrail.OriginData);
	fprintf(fp,"               Reel Name  =  %.8s\n", ReelTrail.Name);
	fprintf(fp,"Reel Continuation Number  =  %.2s\n",ReelTrail.ConnectNum);
	fprintf(fp,"      Previous Reel Name  =  %.8s\n",ReelTrail.PreConName);
	fprintf(fp,"%.74s\n",ReelTrail.Comments);
}

void CLisRead::Prt_LisTapeHeader(FILE *fp)
{
	fprintf(fp,"\n           *** Tape Header Record ***\n");
	fprintf(fp,"            Service Name  =  %.6s\n",TapeHead.SerName);
	fprintf(fp,"                    Date  =  %.8s\n",TapeHead.Date);
	fprintf(fp,"          Origin of Data  =  %.4s\n",TapeHead.OriginData);
	fprintf(fp,"               Tape Name  =  %.8s\n",TapeHead.Name);
	fprintf(fp,"Tape Continuation Number  =  %.2s\n",TapeHead.ConnectNum);
	fprintf(fp,"      Previous Tape Name  =  %.8s\n",TapeHead.PreConName);
	fprintf(fp,"Comments:%.74s\n",TapeHead.Comments);
}
void CLisRead::Prt_LisTapeTrailer(FILE *fp)
{
	fprintf(fp,"\n           *** Tape Trailer Record ***\n");
	fprintf(fp,"            Service Name  =  %.6s\n",TapeTrail.SerName);
	fprintf(fp,"                    Date  =  %.8s\n",TapeTrail.Date);
	fprintf(fp,"          Origin of Data  =  %.4s\n",TapeTrail.OriginData);
	fprintf(fp,"               Tape Name  =  %.8s\n",TapeTrail.Name);
	fprintf(fp,"Tape Continuation Number  =  %.2s\n",TapeTrail.ConnectNum);
	fprintf(fp,"      Previous Tape Name  =  %.8s\n",TapeTrail.PreConName);
	fprintf(fp,"%.74s\n",TapeTrail.Comments);
}
void CLisRead::Prt_LisFileHeader(FILE *fp)
{
	fprintf(fp,"\n-------------- File Header Record (StartPos: %u Bytes)------------\n",LisLogicalFileStartPos);
	fprintf(fp,"                     File Name  =  %.10s\n",FileHead.Name);
	fprintf(fp,"        Service Sub Level Name  =  %.6s\n",FileHead.SubSerName);
	fprintf(fp,"                     Version #  =  %.8s\n",FileHead.Version);
	fprintf(fp,"            Date of Generation  =  %.8s\n",FileHead.Date);
	fprintf(fp,"Maximum Physical Record Length  =  %.5s\n",FileHead.MaxPRL);
	fprintf(fp,"                     File Type  =  %.2s\n",FileHead.FileType);
	fprintf(fp,"   Optional Previous File Name  =  %.10s\n",FileHead.PreConName);
}

void CLisRead::Prt_LisFileTrailer(FILE *fp)
{
	fprintf(fp,"\n           *** File Trailer Record (StopPos: %u Bytes) ***\n",LisLogicalFileStopPos-1);
	fprintf(fp,"                     File Name  =  %.10s\n",FileTrail.Name);
	fprintf(fp,"        Service Sub Level Name  =  %.6s\n",FileTrail.SubSerName);
	fprintf(fp,"                     Version #  =  %.8s\n",FileTrail.Version);
	fprintf(fp,"            Date of Generation  =  %.8s\n",FileTrail.Date);
	fprintf(fp,"Maximum Physical Record Length  =  %.5s\n",FileTrail.MaxPRL);
	fprintf(fp,"                     File Type  =  %.2s\n",FileTrail.FileType);
	fprintf(fp,"   Optional Previous File Name  =  %.10s\n",FileTrail.PreConName);
}
void CLisRead::Prt_EntryBlock(FILE *fp)
{
	CString s;
	//Output Entry Block information
	fprintf(fp,"                  Data Record Type  = %u\n",EnVa.DaRecTyp);
	fprintf(fp,"    Datum SpeFIDication Block Type  = %u\n",EnVa.DSBTyp);
	fprintf(fp,"                   Data Frame Size  = %hd\n",EnVa.DaFraSiz);
	fprintf(fp,"                      UP/DOWN Flag  = %u\n",EnVa.UDFlag);
	fprintf(fp,"     Optical Log Depth Scale Units  = %u\n",EnVa.DepUnitFlag);
	fprintf(fp,"              Data Reference Point  = %d\n",EnVa.DaRefP);
	fprintf(fp,"              Units of Frame Space  = %.4s\n",EnVa.DaRefUnt);
	fprintf(fp,"                     Frame Spacing  = %G\n",EnVa.FraSpace);
	fprintf(fp,"              Units of Frame Space  = %4.4s\n",EnVa.FraUnt);
	fprintf(fp,"             Maximum Frames/Record  = %d\n",EnVa.MaxFraRec);
	fprintf(fp,"                      Absent Value  = %G\n",EnVa.AbsVal);
	fprintf(fp,"                 Output Depth Flag  = %d\n",EnVa.DepMod);
	fprintf(fp,"    Units of Depth in Data Records  = %.4s\n",EnVa.DepUnt);
	fprintf(fp,"            Code for Depth Message  = %d\n",EnVa.DepRpc);
	fprintf(fp," Data Specification Block Sub-type  = %d\n",EnVa.DSBSTyp);
	
	//���ݸ�ʽ˵����¼�ĺ󲿷֣�����˵����
	if(EnVa.DSBSTyp==1)
	{
		fprintf(fp,"%-2s %-4s %-6s %-8s %-4s","ID","Mnem","Tool","ServOrd","Unit");
		fprintf(fp," %-8s","APICode");
		fprintf(fp," %-5s %-4s %-4s %-4s", "File#","Size","Samp","RepC");
		fprintf(fp," %-32s","Process_Indicators");
	}
	else
	{
		fprintf(fp,"%-2s %-4s %-6s %-8s %-4s","ID","Mnem","Tool","ServOrd","Unit");
		fprintf(fp," %-4s %-4s %-4s %-4s","APIL","APIT","APIC","APIM");
		fprintf(fp," %-5s %-4s %-4s %-4s", "File#","Size","Samp","RepC");
		fprintf(fp," %-10s","Proc_Lev");
	}
	fprintf(fp,"\n");
	for(int i=0;i<NumLog;i++)
	{
		fprintf(fp,"%-2u",i+1);
		fprintf(fp," %-4.4s",Curve[i].Name);
		fprintf(fp," %-6.6s",Curve[i].ServID);
		fprintf(fp," %-8.8s",Curve[i].ServOrd);
		fprintf(fp," %-4.4s",Curve[i].Units);
		if(EnVa.DSBSTyp==1)
		{
			fprintf(fp," %-08d",Curve[i].APICode);
		}
		else
		{
			for(int k=0;k<4;k++)
			{
				fprintf(fp," %-4u",(Curve[i].APICode>>(3-k))&0xFF);
			}
		}
		fprintf(fp," %-5d %-4d %-4d %-4d",
			Curve[i].FileNumber,Curve[i].Size,Curve[i].Samples,Curve[i].iRepC);
		fprintf(fp," ");
		if(EnVa.DSBSTyp==1)
		{
			for(int k=0;k<40;k++)
			{
				int k1=k/8;
				int k2=k%8;
				int n=((Curve[i].ProInd[k1])<<k2)&0X80;
				fprintf(fp,"%c",n==0x80? '1':'0');
			}
		}
		else
		{
			for(int k=0;k<8;k++)
			{
				int n=(Curve[i].ProLvl<<k)&0X80;
				fprintf(fp,"%c",n==0x80? '1':'0');
			}
		}
		fprintf(fp,"\n");
	}
	if(EnVa.DepMod==0)
	{
		DepScale=GetDepScaleFromLisUnits(Curve[0].Units);
	}
	memcpy(EnVa.DepUnt,"M   ",4);
	Rlev=float(EnVa.FraSpace*DepScale);
	if(EnVa.UDFlag==1) Rlev=-Rlev;
}
void CLisRead::Prt_WellInfo(FILE *fp)
{
	if(EnVa.DepMod==0)
	{
		Rlev=(EnDep-StDep)/(NumPoint-1);
	}

	fprintf(fp,"\n");
	fprintf(fp,"Data Depth form StartDep = %G to EndDep = %G\n",StDep,EnDep);
	fprintf(fp,"Level Spacing = %G  Sample Number = %u  NumLog=%d\n",Rlev,NumPoint,NumLog);
}

//16 bit float,code=49
float CLisRead::LisFloat16(unsigned char *buf)
{
	float r,s;
	long ie,im;
	ie=0x0f&buf[1];
	im=((0x7f&buf[0])<<4)+(buf[1]>>4);
	s=1.0;
	if((buf[0]>>7)&0x01)
	{
		s=-1.0;
		im=((~im)+1)&0x7ff;
	}
	r=(float)(s*im/2048.0*pow(2.0,1.0*ie));
	return r;
}
//32 bit float,code=50
float CLisRead::LisFloat32Low(unsigned char *buf)
{
	float r;
	long ie,im;
	ie=(buf[0]<<8)|buf[1];
	im=(buf[2]<<8)|buf[3];
	if((buf[0]>>7)&0x01)
	{
		ie=((~ie)+1)&0x7fff;
		ie=-ie;
	}
	if((buf[2]>>7)&0x01)
	{
		im=((~im)+1)&0x7fff;
		im=-im;
	}
	r=(float)(im/32768.0*pow(2.0,1.0*ie));
	return r;
}
//8 bit complement integer,code=56
short CLisRead::Lis8Integer(unsigned char *buf)
{
	char c;
	c=buf[0];
	return (short)c;
}
//8 bit unsigned integer,code=66
short CLisRead::Lis8Unsigned(unsigned char *buf)
{
	return (short)(0xff&buf[0]);
}
//32 bit float,code=68
float CLisRead::LisFloat32(unsigned char *buf)
{
	float r,s;
	long ie,im;
	ie=((buf[0]<<1)|(buf[1]>>7))&0xff;
	if((buf[0]>>7)&0x01)
	{
		s=-1.0;
		ie=127-ie;
		im=65536L*(0x7f&buf[1])+256L*buf[2]+buf[3];
		im=((~im)+1)&0x7fffff;
	}
	else
	{
		s=1.0;
		ie=ie-128;
		im=65536L*(0x7f&buf[1])+256L*buf[2]+buf[3];
	}
	if(im==0 || ie<=-128)
	{
		r=(float)(s*0.0); //avoid underflow
	}
	else
	{
		r=(float)(s*im/8388608.0*pow(2.0,(double)ie));
	}
	return r;
}
//32 bit fixed-point,code=70
float CLisRead::LisFloat32Fix(unsigned char *buf)
{
	float r;
	long n;
	n=16777216L*buf[0]+65536L*buf[1]+256L*buf[2]+buf[3];
	if((buf[0]>>7)&0x01)
	{
		n=(~n)+1L;
		r=(float)(-n/65536.0);
	}
	else
	{
		r=(float)(n/65536.0);
	}
	return r;
}
//32 bit Integer,code=73
int CLisRead::Lis32Integer(unsigned char *buf)
{
	int n;
	n=16777216L*buf[0]+65536L*buf[1]+256L*buf[2]+buf[3];
	return n;
}
//16 bit Integer,code=79
short CLisRead::Lis16Integer(unsigned char *buf)
{
	short n;
	n=256L*buf[0]+buf[1];
	return n;
}
float CLisRead::GetDepScaleFromLisUnits(char *Units)
{
	int nx=0;
	double f1=1.0;
	double f2=1.0;
	char str[5];
	for(int i=0;i<4;i++)
	{
		if(isalpha(Units[i]))
		{
			nx=i;
			break;
		}
	}
	if(nx>0)
	{
		memcpy(str,Units,nx);
		str[nx]=0;
		f1=atof(str);
		memcpy(str,&Units[nx],4-nx);
		str[4-nx]=0;
	}
	else
	{
		f1=1.0;
		memcpy(str,Units,4);
	}
	if(strnicmp(str,"IN",2)==0)
	{
		f2=0.0254;
	}
	else if(strnicmp(str,"F",1)==0)
	{
		f2=0.3048;
	}
	else if(strnicmp(str,"MM",2)==0)
	{
		f2=0.001;
	}
	else if(strnicmp(str,"CM",2)==0)
	{
		f2=0.01;
	}
	else if(strnicmp(str,"DM",2)==0)
	{
		f2=0.1;
	}
	else
	{
		f2=1.0; //������λ
	}
	return float(f1*f2);
}
int CLisRead::GetCodeLength(unsigned char RepCode)
{
	int n;
	switch(RepCode)
	{
    case 49: n=2; break; //16λ����
    case 50: n=4; break; //32λ�ͷֱ��ʸ���
    case 56: n=1; break; //8λ��������
    case 66: n=1; break; //8λ�޷�������
    case 68: n=4; break; //32λ����
    case 70: n=4; break; //32λ�̶�����
    case 73: n=4; break; //32λ����
    case 77: n=0; break; //�����γߡ�
    case 79: n=2; break; //16λ����
    case 128: n=1; break; //��ë�ߡ����ݿ�
    case 234: n=1; break; //
    default: n=1; break;
	}
	return n;
}
int CLisRead::GetObjCodeLen(unsigned char RepCode)
{
	int n=1;
	switch(RepCode)
	{
	case FID_REPR_CHAR:
		n=1;
		break;
	case FID_REPR_SHORT:
		n=2;
		break;
	case FID_REPR_INT:
	case FID_REPR_LONG:
	case FID_REPR_FLOAT:
		n=4;
		break;
	default:
		n=1;
		break;
	}
	return n;
}
float CLisRead::LisBufToFloat(BYTE Code,BYTE *buf)
{
	if(Code==49)
	{
		return float(LisFloat16(buf));
	}
	else if(Code==50)
	{
		return float(LisFloat32Low(buf));
	}
	else if(Code==56)
	{
		return float(Lis8Integer(buf));
	}
	else if(Code==66)
	{
		return float(Lis8Unsigned(buf));
	}
	else if(Code==68)
	{
		return LisFloat32(buf);
	}
	else if(Code==70)
	{
		return LisFloat32Fix(buf);
	}
	else if(Code==73)
	{
		return float(Lis32Integer(buf));
	}
	else if(Code==79)
	{
		return float(Lis16Integer(buf));
	}
	else 
	{
		return float(BYTE(buf[0]));
	}
}
int CLisRead::LisBufToInt(BYTE Code,BYTE *buf)
{
	if(Code==49)
	{
		return int(LisFloat16(buf));
	}
	else if(Code==50)
	{
		return int(LisFloat32Low(buf));
	}
	else if(Code==56)
	{
		return int(Lis8Integer(buf));
	}
	else if(Code==66)
	{
		return int(Lis8Unsigned(buf));
	}
	else if(Code==68)
	{
		return int(LisFloat32(buf));
	}
	else if(Code==70)
	{
		return int(LisFloat32Fix(buf));
	}
	else if(Code==73)
	{
		return int(Lis32Integer(buf));
	}
	else if(Code==79)
	{
		return int(Lis16Integer(buf));
	}
	else 
	{
		return int(BYTE(buf[0]));
	}
}
short CLisRead::LisBufToShort(BYTE Code,BYTE *buf)
{
	if(Code==49)
	{
		return short(LisFloat16(buf));
	}
	else if(Code==50)
	{
		return short(LisFloat32Low(buf));
	}
	else if(Code==56)
	{
		return short(Lis8Integer(buf));
	}
	else if(Code==66)
	{
		return short(Lis8Unsigned(buf));
	}
	else if(Code==68)
	{
		return short(LisFloat32(buf));
	}
	else if(Code==70)
	{
		return short(LisFloat32Fix(buf));
	}
	else if(Code==73)
	{
		return short(Lis32Integer(buf));
	}
	else if(Code==79)
	{
		return short(Lis16Integer(buf));
	}
	else 
	{
		return short(BYTE(buf[0]));
	}
}
BYTE CLisRead::LisBufToByte(BYTE Code,BYTE *buf)
{
	if(Code==49)
	{
		return BYTE(LisFloat16(buf));
	}
	else if(Code==50)
	{
		return BYTE(LisFloat32Low(buf));
	}
	else if(Code==56)
	{
		return BYTE(Lis8Integer(buf));
	}
	else if(Code==66)
	{
		return BYTE(Lis8Unsigned(buf));
	}
	else if(Code==68)
	{
		return BYTE(LisFloat32(buf));
	}
	else if(Code==70)
	{
		return BYTE(LisFloat32Fix(buf));
	}
	else if(Code==73)
	{
		return BYTE(Lis32Integer(buf));
	}
	else if(Code==79)
	{
		return BYTE(Lis16Integer(buf));
	}
	else 
	{
		return BYTE(BYTE(buf[0]));
	}
}
char CLisRead::LisBufToChar(BYTE Code,BYTE *buf)
{
	if(Code==49)
	{
		return char(LisFloat16(buf));
	}
	else if(Code==50)
	{
		return char(LisFloat32Low(buf));
	}
	else if(Code==56)
	{
		return char(Lis8Integer(buf));
	}
	else if(Code==66)
	{
		return char(Lis8Unsigned(buf));
	}
	else if(Code==68)
	{
		return char(LisFloat32(buf));
	}
	else if(Code==70)
	{
		return char(LisFloat32Fix(buf));
	}
	else if(Code==73)
	{
		return char(Lis32Integer(buf));
	}
	else if(Code==79)
	{
		return char(Lis16Integer(buf));
	}
	else 
	{
		return char(BYTE(buf[0]));
	}
}
BYTE CLisRead::LisRepCToPcRepC(BYTE Code)
{
	BYTE nType=FID_REPR_FLOAT;
	switch(Code)
	{
	case 49: //16λ����
	case 50: //32λ�ͷֱ��ʸ���
	case 68: //32λ����
	case 70: //32λ�̶�����
		nType=FID_REPR_FLOAT;
		break;
	case 56: //8λ��������(char)
	case 65: //�ַ�����7λASCII��
	case 66: //8λ�޷�������
		nType=FID_REPR_CHAR;
		break;
	case 73: //32λ����
		nType=FID_REPR_LONG;
		break;
	case 79: //16λ����
		nType=FID_REPR_SHORT;
		break;
	default:
		nType=FID_REPR_CHAR;
		break;
	}
	return nType;
}
void CLisRead::Prt_LisData(DWORD nCount,DWORD &NumPrtFra,FILE *fp)
{
	DWORD pos=2;
	float FraDepth,Depth;
    if(EnVa.DepMod==1)
    {
		//�õ���Ȳ�ת���ɡ��ס�
		FraDepth=DepScale*LisBufToFloat(EnVa.DepRpc,&LisBuf[pos]);
		pos+=GetCodeLength(EnVa.DepRpc);
	}
	else
	{
		//�õ���Ȳ�ת���ɡ��ס�
		FraDepth=DepScale*LisBufToFloat(Curve[0].iRepC,&LisBuf[pos]);
	}
	int nFrameRec=(nCount-pos)/EnVa.DaFraSiz;
	for(int i=0;i<nFrameRec;i++)
	{
		NumPrtFra++;
		if(NumPrtFra>DWORD(m_NumPrtFdata)) break;
		if(EnVa.DepMod==1)
		{
			Depth=FraDepth+i*Rlev;
			fprintf(fp,"\nDepth = %G %-4.4s\n",Depth,EnVa.DepUnt);
		}
		else
		{
			fprintf(fp,"\n");
		}
		DWORD CurPos=pos;
		for(int k=0;k<NumLog;k++)
		{
			if(k>0 && k%5==0)
			{
				fprintf(fp,"\n");
			}
			int NumItem=Curve[k].Size/Curve[k].iCodLen;
			if(NumItem>1)
			{
				fprintf(fp,"\n");
			}
			int NumPrt=NumItem;
			if(NumPrt>m_NumPrtSample)
			{
				NumPrt=m_NumPrtSample;	//������ʾ����
				if(NumPrt==(NumItem-1))NumPrt=NumItem; //ֻ��һ�����
			}

			fprintf(fp,"%-4.4s=",Curve[k].Name);
			if(Curve[k].oRepC==FID_REPR_FLOAT)
			{
				if(Curve[k].Size==Curve[k].iCodLen)
				{
					float vi=LisBufToFloat(Curve[k].iRepC,&LisBuf[CurPos]);
					fprintf(fp,"%10.2f  ",vi);
				}
				else
				{
					for(int k1=0;k1<NumPrt;k1++)
					{
						if(k1>0 && k1%24==0)
						{
							fprintf(fp,"\n     ");
						}
						float vi=LisBufToFloat(Curve[k].iRepC,&LisBuf[CurPos+k1*Curve[k].iCodLen]);
						fprintf(fp," %8.2f",vi);
					}
				}
			}
			else
			{
				if(Curve[k].Size==Curve[k].iCodLen)
				{
					int vi=LisBufToInt(Curve[k].iRepC,&LisBuf[CurPos]);
					fprintf(fp,"%10d  ",vi);
				}
				else
				{
					for(int k1=0;k1<NumPrt;k1++)
					{
						if(k1>0 && k1%24==0)
						{
							fprintf(fp,"\n     ");
						}
						int vi=LisBufToInt(Curve[k].iRepC,&LisBuf[CurPos+k1*Curve[k].iCodLen]);
						fprintf(fp," %3d",vi);
					}
				}
			}
			if(NumItem>1 && NumPrt<NumItem)
			{
				fprintf(fp," ...... [%d]\n",NumItem);
			}
			CurPos+=Curve[k].Size;
		}
		fprintf(fp,"\n");
		pos+=EnVa.DaFraSiz;
	}	
}
void CLisRead::LisBufToDepth(DWORD nCount)
{
	DWORD pos=2;
	float DeptRec,Depth;
    if(EnVa.DepMod==1)
    {
		//�õ���Ȳ�ת���ɡ��ס�
		DeptRec=DepScale*LisBufToFloat(EnVa.DepRpc,&LisBuf[pos]);
		pos+=GetCodeLength(EnVa.DepRpc);
	}
	else
	{
		//�õ���Ȳ�ת���ɡ��ס�
		DeptRec=DepScale*LisBufToFloat(Curve[0].iRepC,&LisBuf[pos]);
	}
	EnVa.DaFraSiz=0;
	for(int i=0;i<NumLog;i++)
	{
		EnVa.DaFraSiz+=Curve[i].Size;
	}
	int nFrameRec=(nCount-pos)/EnVa.DaFraSiz;
	NumPoint+=nFrameRec; //��������

	for(i=0;i<nFrameRec;i++)
	{
		if(EnVa.DepMod==1)
		{
			Depth=DeptRec+i*Rlev;
		}
		else
		{
			Depth=LisBufToFloat(Curve[0].iRepC,&LisBuf[pos]);
			pos+=EnVa.DaFraSiz;
		}
		if(fabs(StDep+999.25)<0.1)
		{
			StDep=Depth; //��ʼ���
		}
		EnDep=Depth; //��ֹ���
	}
}
void CLisRead::LisBufToData(BYTE *buf,BYTE RepC,short nItem,float *vf)
{
	int CodeLen=GetCodeLength(RepC);
	for(short i=0;i<nItem;i++)
	{
		int pos=i*CodeLen;
		vf[i]=LisBufToFloat(RepC,&buf[pos]);
	}
}
void CLisRead::LisBufToData(BYTE *buf,BYTE RepC,short nItem,int *vi)
{
	int CodeLen=GetCodeLength(RepC);
	for(short i=0;i<nItem;i++)
	{
		int pos=i*CodeLen;
		vi[i]=LisBufToInt(RepC,&buf[pos]);
	}
}
void CLisRead::LisBufToData(BYTE *buf,BYTE RepC,short nItem,short *vh)
{
	int CodeLen=GetCodeLength(RepC);
	for(short i=0;i<nItem;i++)
	{
		int pos=i*CodeLen;
		vh[i]=LisBufToShort(RepC,&buf[pos]);
	}
}
void CLisRead::LisBufToData(BYTE *buf,BYTE RepC,short nItem,BYTE *vb)
{
	int CodeLen=GetCodeLength(RepC);
	for(short i=0;i<nItem;i++)
	{
		int pos=i*CodeLen;
		vb[i]=LisBufToByte(RepC,&buf[pos]);
	}
}
void CLisRead::LisBufToData(BYTE *buf,BYTE RepC,short nItem,char *vc)
{
	int CodeLen=GetCodeLength(RepC);
	for(short i=0;i<nItem;i++)
	{
		int pos=i*CodeLen;
		vc[i]=LisBufToChar(RepC,&buf[pos]);
	}
}


bool CLisRead::ReadCurve(int index,int Num,float *buffer)
{
	if(fTemp.Open(sMidFile,CFile::modeRead|CFile::shareDenyNone)==FALSE) return false;
	DWORD pos=0;
	int OffSet=0;
	for(int i=0;i<index;i++)
	{
		OffSet+=Curve[i].oCodLen*Curve[i].NumX*Curve[i].Samples;
	}
	DWORD count=Curve[index].NumX*Curve[index].Samples;
	for(i=0;i<NumPoint;i++)
	{
		pos=i*nTempBlockSize+OffSet;
		ReadData(pos,Curve[index].oRepC,Curve[index].oCodLen,count,&buffer[i*count]);
	}
	fTemp.Close();

	//���Ϊ�ϲ⣬��������������
	if(StDep>EnDep)
	{
		for(i=0;i<Num/2;i++)
		{
			float fTemp=buffer[i];
			buffer[i]=buffer[Num-i-1];
			buffer[Num-i-1]=fTemp;
		}
	}
	return true;
}
bool CLisRead::ReadData(DWORD pos,BYTE RepCode,BYTE CodeLen,DWORD count,float *buf)
{
	fTemp.Seek(pos,CFile::begin);
	int *vi;
	short *vh;
	char *vc;
	DWORD i;
	switch(RepCode)
	{
	case FID_REPR_FLOAT:
		fTemp.Read(buf,count*CodeLen);
		break;
	case FID_REPR_INT:
	case FID_REPR_LONG:
		vi=new int[count];
		fTemp.Read(vi,count*CodeLen);
		for(i=0;i<count;i++)
		{
			buf[i]=float(vi[i]);
		}
		delete []vi; vi=NULL;
		break;
	case FID_REPR_SHORT:
		vh=new short[count];
		fTemp.Read(vh,count*CodeLen);
		for(i=0;i<count;i++)
		{
			buf[i]=float(vh[i]);
		}
		delete []vh; vh=NULL;
		break;
	case FID_REPR_CHAR:
		vc=new char[count];
		fTemp.Read(vc,count*CodeLen);
		for(i=0;i<count;i++)
		{
			buf[i]=float(vc[i]);
		}
		delete []vc; vc=NULL;
		break;
	default:
		break;
	}
	return true;
}
bool CLisRead::ReadWave(int index,float depth,float *buffer)
{
	if(fTemp.Open(sMidFile,CFile::modeRead|CFile::shareDenyNone)==FALSE) return false;
	int OffSet=0;
	for(int i=0;i<index;i++)
	{
		OffSet+=Curve[i].oCodLen*Curve[i].NumX*Curve[i].Samples;
	}
	DWORD count=Curve[index].NumX*Curve[index].Samples;
	DWORD np=DWORD((depth-StDep)/Rlev+0.5);
	DWORD pos=np*nTempBlockSize+OffSet;
	ReadData(pos,Curve[index].oRepC,Curve[index].oCodLen,count,buffer);
	fTemp.Close();
	//�ϲ���²���������ڵ��ú����Լ����
	return true;
}
void CLisRead::ReadLogicalFile()
{
	CString s;
	if(IsFileName(sMidFile))
	{
		remove((const char*)sMidFile);
	}
	
	TCHAR szTempPath  [MAX_PATH];
	DWORD dwResult=:: GetTempPath (MAX_PATH, szTempPath);
	ASSERT (dwResult);
	sMidFile.Format("%s~LisData.tmp",szTempPath);
	CFile f;
	if(f.Open(sMidFile,CFile::modeCreate|CFile::modeWrite)==FALSE) 
	{
		s.Format("������ʱ�ļ� %s ����!",sMidFile);
		AfxMessageBox(s,MB_ICONSTOP);
		return;
	}
	nTempBlockSize=0;
	DWORD  nItem=0; //���������ֵ
	for(int i=0;i<NumLog;i++)
	{
		DWORD n=Curve[i].Size/Curve[i].iCodLen;
		if(nItem<n) nItem=n;
		nTempBlockSize+=Curve[i].oCodLen*n;
	}
	DWORD fs=NumPoint*nTempBlockSize;
	f.SetLength(fs);
	f.SeekToBegin();
	/////////////////////////////
	SetLisPos(LisDatStartPos);
	DWORD Count;
	DWORD DatOffset=2+EnVa.DepMod*GetCodeLength(EnVa.DepRpc);
	float *vf=new float[nItem];
	int *vi=new int[nItem];
	short *vh=new short[nItem];
	BYTE *vb=new BYTE[nItem];
	char *vc=new char[nItem];
	DWORD Number=0;
	DWORD rec=0;
	while(rec<NumDatRec)
	{
		if(GetLogicalRecord(Count)<1) break; //�ѵ��ļ�β
		if(LisBuf[0]==129 || LisBuf[0]==128) break; //�Ѷ����ļ�β��¼����һ���ļ�ͷ
		if(LisBuf[0]!=0) continue; //�������ݼ�¼(��ЩLIS�ļ��������в�����������¼)
		rec++;
		int NumFrame=(Count-DatOffset)/EnVa.DaFraSiz;
		for(int i=0;i<NumFrame;i++)
		{
			Number++;
			for(int j=0;j<NumLog;j++)
			{
				DWORD pos=DatOffset+i*EnVa.DaFraSiz+Curve[j].OffSet;
				unsigned char RepC=Curve[j].iRepC;
				short nItem=Curve[j].Size/Curve[j].iCodLen;
				switch(Curve[j].oRepC)
				{
				case FID_REPR_FLOAT:
					LisBufToData(&LisBuf[pos],RepC,nItem,vf);
					f.Write(vf,nItem*sizeof(float));
					break;
				case FID_REPR_INT:
				case FID_REPR_LONG:
					LisBufToData(&LisBuf[pos],RepC,nItem,vi);
					f.Write(vi,nItem*sizeof(int));
					break;
				case FID_REPR_SHORT:
					LisBufToData(&LisBuf[pos],RepC,nItem,vh);
					f.Write(vh,nItem*sizeof(short));
					break;
				default:
					LisBufToData(&LisBuf[pos],RepC,nItem,vc);
					f.Write(vc,nItem);
					break;
				}
			}
		}
	}
	f.Close();
	delete []vf; vf=NULL;
	delete []vi; vi=NULL;
	delete []vh; vh=NULL;
	delete []vb; vb=NULL;
	delete []vc; vc=NULL;
}
void CLisRead::GetTapeProducer(CString &sProducer,CString &OriginOfdata)
{
	CString s;
	//�Ӵ�ͷ�õ���˾����������Ϣ
	sProducer.Empty();
	OriginOfdata.Empty();
	//�豸��
	OriginOfdata.Format ("%-.4s",TapeHead.OriginData);
	OriginOfdata.TrimRight();
	OriginOfdata.MakeUpper();
	
	//��˾��
	s.Format ("%.74s\n",TapeHead.Comments);
	s.TrimRight();
	s.MakeUpper();
	if(sProducer.IsEmpty())
	{
		if(s.Find("SCHLUMBERGER")>=0)
		{
			sProducer="SCHLUMBERGER";
		}
	}
	if(sProducer.IsEmpty())
	{
		if(s.Find("HALLIBURTON")>=0)
		{
			sProducer="HALLIBURTON";
		}
	}
	if(sProducer.IsEmpty())
	{
		if(s.Find("SICHUAN")>=0)
		{
			sProducer="SICHUAN";
		}
	}

}

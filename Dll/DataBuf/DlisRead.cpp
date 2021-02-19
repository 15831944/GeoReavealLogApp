// DlisRead.cpp: implementation of the CDlisRead class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DlisRead.h"
#include <math.h>
#include <io.h>
#include <sys/stat.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDlisRead::CDlisRead()
{
	InitVariable();
	BufLen=MAX_DLIS_BLOCKLEN;
	DlisBuf=new BYTE[BufLen];
	Channel=new _Channel[MAX_FILE_CHANNEL_NUM];
	Axis=new _Axis[MAX_FILE_CHANNEL_NUM];
	Frame=new _Frame[MAX_FRAME_OBJ_NUM];
	IORIGIN=new DWORD[MAX_OBJECT_NUM];//����Դ
	ICOPYNUM=new BYTE[MAX_OBJECT_NUM];//���ݿ�����
	IVALUE=new int[MAX_DATAITEM_NUM]; //����ֵ
	VALUE=new float[MAX_DATAITEM_NUM]; //����ֵ
	LCVALUE=new int[MAX_DATAITEM_NUM]; //�Ӵ�����
	CVALUE=new char[MAX_DATAITEM_NUM*64]; //�Ӵ���,�ٶ�һ���Ӵ�ƽ������Ϊ64�ֽ�
	VariableReset();
	bTrans=false;
	fMid=NULL;
	NumPrtFdata=1; //��ʾ֡������
	NumPrtSample=5; //��ʾ������
	bPrtBaseInfo=false; //�Ƿ���ʾ������Ϣ

}

CDlisRead::~CDlisRead()
{
	delete []DlisBuf; DlisBuf=NULL;
	delete []Channel; Channel=NULL;
	delete []Axis; Axis=NULL;
	delete []Frame; Frame=NULL;
	delete []IORIGIN; IORIGIN=NULL;
	delete []ICOPYNUM; ICOPYNUM=NULL;
	delete []IVALUE; IVALUE=NULL;
	delete []VALUE; VALUE=NULL;
	delete []LCVALUE; LCVALUE=NULL;
	delete []CVALUE; CVALUE=NULL;


	TempAttribLabel.RemoveAll();
	TempAttribCount.RemoveAll();
	TempAttribRepCd.RemoveAll();
	TempAttribUnits.RemoveAll();
	LogFilePos.RemoveAll();
	for(int i=0;i<sMidFile.GetSize();i++)
	{
		remove(sMidFile.GetAt(i));
	}
	sMidFile.RemoveAll();
}
void CDlisRead::InitVariable()
{
	NumLogFile=0;
	NumPhyRec=0;
	nLogLen=0;
	LogRecBytes=0;
	NpntLenC=0;
	NumLogRec=0;
	nLogPos=0;
	NumEncry=0;
	bRpr=true;
}

bool _declspec(dllexport) IsDLIS(CString sDataFile)
{
	CFile f;
	char Dlis_Label[6];
	char str[7];
	if(f.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE) return false;
	f.Seek(9,CFile::begin);
	f.Read(Dlis_Label,6);
	f.Close();
	sprintf(str,"%-6.6s",Dlis_Label);
	if(strcmp(str,"RECORD")!=0)
	{
		return false;
	}
	return true;
}
void CDlisRead::VariableReset()
{
	NumChannel=0;
	NumAxis=0;
	NumFrame=0;
	NumOrigin=0;
	memset(FileHeader.ID,'\0',66);
	memset(FileHeader.SequenceNumber,'\0',11);
	/////////////////////////////////////////////////////////////
	if(Channel)
	{
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

		for(i=1;i<MAX_FILE_CHANNEL_NUM;i++)
		{
			Channel[i]=Channel[i-1];
		}
	}
	////////////////////////////////////////////////////////////////////
	if(Axis)
	{
		memset(Axis[0].Name,'\0',256); //��������Ӧ�ڶ�����
		Axis[0].Origin=0; //Դ
		Axis[0].CopyNum=0; //������
		memset(Axis[0].AxisID,'\0',256); //��ID��:C=1,R=IDENT
		Axis[0].NumC=0; //������
		for(int i=0;i<MAX_DIM_NUM;i++)Axis[0].Coordinates[i]=0; //����
		memset(Axis[0].Units,'\0',16); //���굥λ
		Axis[0].Spacing=0; //���
		for(i=1;i<MAX_AXIS_NUM;i++)
		{
			Axis[i]=Axis[i-1];
		}
	}
	//////////////////////////////////////////////////////////////
	if(Frame)
	{
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
		for(int i=1;i<MAX_FRAME_OBJ_NUM;i++)
		{
			Frame[i]=Frame[i-1];
		}
	}
	//////////////////////////////////////////////////
	//Դ
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

}
bool CDlisRead::Open(CString sDataFile,FILE *fo)
{
	if(fDlis.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE)
	{
		return false;
	}
	m_sDataFile=sDataFile;
	FileSize=fDlis.GetLength(); //�ļ�����
	fp=fo;
	return true;
}
void CDlisRead::Close()
{
	fDlis.Close();
}
void CDlisRead::ReadStoUnitLab()
{
	fDlis.SeekToBegin();
	fDlis.Read(&StoUnitLab,sizeof(StoUnitLab));
	FilePos=sizeof(StoUnitLab);
}

void CDlisRead::Prt_StoUnitLab()
{
	fprintf(fp,"******************************** Storage Unit Label **************************\n");
	fprintf(fp,"             Sequence Number: %.4s\n",StoUnitLab.SeqNum);
	fprintf(fp,"                DLIS Version: %.5s\n",StoUnitLab.DLISVer);
	fprintf(fp,"                   Structure: %.6s\n",StoUnitLab.Stru);
	fprintf(fp,"       Maximum Record Length: %.5s\n",StoUnitLab.MaxRecLen);
	fprintf(fp,"      Storage Set Identifier: %.60s\n",StoUnitLab.StoSetIden);
	fprintf(fp,"******************************************************************************\n");
}

bool CDlisRead::ReadPhyHeader()
{
	if(FilePos>=FileSize)
	{
		return false;
	}
	fDlis.Seek(FilePos,CFile::begin);
	fDlis.Read(&PhyRec,sizeof(PhyRec));
	SunI2TOPC(PhyRec.VisRecLen); 
	NumPhyRec++;
	FilePos+=sizeof(PhyRec);
	return true;
}
int CDlisRead::GetLogicalRecord()
{
	CString s;
	nLogLen=0;
	LogRecBytes=0;
	while(1)
	{
		if(FilePos>=FileSize) break;
		if(bRpr) 
		{
			ReadPhyHeader(); //physical header
			bRpr=false;
			NpntLenC=4;
		}
		fDlis.Seek(FilePos,CFile::begin);
		fDlis.Read(&LogRecHead,sizeof(LogRecHead));  //Read Logical Header
		SunI2TOPC(LogRecHead.Len);
		LogRecAttrib.Stru = ((LogRecHead.Attrib>>7)&1);
		LogRecAttrib.Pred = ((LogRecHead.Attrib>>6)&1);
		LogRecAttrib.Succ = ((LogRecHead.Attrib>>5)&1);
		LogRecAttrib.Encry = ((LogRecHead.Attrib>>4)&1);
		LogRecAttrib.EncryPack = ((LogRecHead.Attrib>>3)&1);
		LogRecAttrib.Checksum = ((LogRecHead.Attrib>>2)&1);
		LogRecAttrib.TraiLen = ((LogRecHead.Attrib>>1)&1);
		LogRecAttrib.Padd = ((LogRecHead.Attrib>>0)&1);
		NpntLenC+=LogRecHead.Len;
		if(NpntLenC>=PhyRec.VisRecLen)
		{
			bRpr=true; //���߼���¼��Ϊ��һ�������¼
		}
		if(LogRecAttrib.Encry==1 )
		{
			NumEncry++;//ignore Encryption Logical Record
		}
		if(LogRecAttrib.Pred==0)
		{
			LogRecBytes=0;
			nLogLen=0; //This is the first Segment of the Logical Record
			nLogPos=FilePos;
		}
		/////////////////////////////
		if((nLogLen+LogRecHead.Len-4)>BufLen)
		{	//������̫С�����·���
			BYTE *NewBuf=new BYTE[BufLen+MAX_DLIS_BLOCKLEN];
			memcpy(NewBuf,DlisBuf,BufLen);
			delete []DlisBuf; DlisBuf=NULL;
			DlisBuf=new BYTE[BufLen+MAX_DLIS_BLOCKLEN];
			memcpy(DlisBuf,NewBuf,BufLen);
			delete []NewBuf; NewBuf=NULL;
			BufLen+=MAX_DLIS_BLOCKLEN;
		}
		fDlis.Read(&DlisBuf[nLogLen],LogRecHead.Len-4);//Read Logical Record
		LogRecBytes+=(LogRecHead.Len-4);
		nLogLen+=(LogRecHead.Len-4); //�ۼ��߼���¼����
		FilePos+=LogRecHead.Len;
		if(LogRecAttrib.Checksum)nLogLen-=2;  //A Checksum is present in the LRST
		if(LogRecAttrib.TraiLen)nLogLen-=2;  //A Copy of the LRS length is present in the LRST
		if(LogRecAttrib.Padd==1)  //Pad Bytes are present in LRST
		{
			//Buffer[nLogLen-1]; //������䳤��
			//������ЩDLIS�ļ������������������Բ���ʹ����䳤��
			nLogLen--;
		}
		if(!LogRecAttrib.Succ) break;//û�к��λ,���߼���¼�Ѷ���
	}
	return nLogLen;
}

bool CDlisRead::DlisBufToEFLR()
{
	CString s;
	unsigned char cAttribute; //Component Format
	int IdefC; //Type of Component
	char SetSymTable[5+1] = "TN   "; //SET,REDUNDANT SET,REPLACEMENT SET COMPONENTS
	char ObjSymTable[5+1] = "N    "; //OBJECT COMPONENTS
	char AttSymTable[5+1] = "LCRUV"; //ATTRIBUTE,INVARIANT ATTRIBUTE COMPONENTS
	int  NumCompForm = 0; //��ָ���
	char CompForm[5+1] = "     "; //ʵ���������
	int NBYTE; //��ǰ�����ֽ�
	bool bOkObject=false; //�ǲ��ѳ��ֶ������
	
	char ObjName[255+1]; //Object Name
	char AttribLabel[255+1];  //Current Attribute Label
	char AttribUnits[255+1];  //Current Attribute Units 
	BYTE  IRepCd;  //Current Attribute Reprentation Code
	int  ICount;  //Current Attribute Count
	DWORD IpntLenC=0; //��ǰ����λ��
	
	//////////////////////////////////////////////////////////
	TempAttribLabel.RemoveAll(); //ģ�����Ա�ʶ��
	TempAttribCount.RemoveAll(); //ģ�����Լ�����
	TempAttribRepCd.RemoveAll(); //ģ�����Ա�ʾ��
	TempAttribUnits.RemoveAll(); //ģ�����Ե�λ
	//////////////////////////////////////////////////////////
	NumSaveAttrib=0;
	NumAttrib=0; //������
	kAttrib=-1; //��ǰ�������
	memset(SetType,'\0',256);//Set Type
	memset(SetName,'\0',256);//Set Name
	memset(ObjName,'\0',256);//������
	memset(AttribLabel,'\0',256);//���Ա��
	memset(AttribUnits,'\0',256);//���Ե�λ
	bool bResult=true;
	for (;;)	// Get Information
	{
		if (IpntLenC>=nLogLen) break;
		///////////////////////////////////////
		cAttribute = DlisBuf[IpntLenC];
		IpntLenC++;
		IdefC = (cAttribute>>5);  //Get Type of Component
		switch (IdefC)
		{
		case 0:	// Absent Attribute
		case 1:	// Attribute
		case 2:	// Invariant Attribute
			strcpy(CompForm,"LCRUV");
			NumCompForm=5;
            break;
		case 3:		// Object
			strcpy(CompForm,"N    ");
			NumCompForm=1;
			break;
		case 4:		// reserved
			strcpy(CompForm,"     ");
			NumCompForm=0;
			break;
		case 5:		// Redundant Set
		case 6:		// Replacement Set
		case 7:		// Set
			strcpy(CompForm,"TN   ");
			NumCompForm=2;
			break;
		}
		
		int IMEX = 0x10;
		for (int i=0;i<5; i++, IMEX/=2)
		{
			if ((cAttribute&IMEX)==0)
			{
				CompForm[i] = ' ';
			}
		}
		
		switch (IdefC)
		{
		case 5:		// Redundant Set
		case 6:		// Replacement Set
		case 7:		// Set
			if(LogRecHead.Type==0 && IdefC==7)
			{
				//DLIS�ļ���FILE-HEADER��¼Ϊ�̶���ʽ��Ӧ����SET����е�"N"
				NumCompForm=1;
			}
			strcpy(SetName,"");
			for (i=0; i<NumCompForm; i++)
			{
                switch (CompForm[i])
				{
                case 'T':	// Type of Object in Set
					NBYTE = DlisBuf[IpntLenC]; //USHORT
					IpntLenC ++;
					memcpy(SetType, &DlisBuf[IpntLenC], NBYTE);
					SetType[NBYTE] = '\0';
					IpntLenC += NBYTE;
					break;
                case 'N':	// Name of Set
					NBYTE = DlisBuf[IpntLenC];
					IpntLenC ++;
					memcpy(SetName, &DlisBuf[IpntLenC], NBYTE);
					SetName[NBYTE] = '\0';
					IpntLenC += NBYTE;
					break;
				default:
					break;
				}
			}
			if(bPrtBaseInfo && bPrintEFLR)
			{
				fprintf(fp,"*%30s: %s\n","Set.Type",SetType);
				if(strlen(SetName)>0)
				{
					fprintf(fp,"*%30s: %s\n","Set.Name",SetName);
				}
			}
			break;
		case 3:	// Object
			for (i=0; i<NumCompForm; i++)
			{
                int IVORG; //�����Դ
				int IVCOPY; //����Ŀ�����
				int ObjNameLen; // ����ı�ʶ������
                switch (CompForm[i])
				{
				case 'N':
					bOkObject=true;
					kAttrib=-1;
					DlisIn.DLIS_OBNAME(&DlisBuf[IpntLenC], IVORG, IVCOPY, NBYTE, ObjNameLen);
                    IpntLenC += NBYTE;
                    memcpy(ObjName, &DlisBuf[IpntLenC], ObjNameLen);
                    ObjName[ObjNameLen] = '\0';
                    IpntLenC += ObjNameLen;
					bResult=InitObject(IVORG,IVCOPY,ObjName);
					if(!bResult) return false;
					if(bPrtBaseInfo & bPrintEFLR)
					{
						fprintf(fp,"*\n");
						fprintf(fp,"*%30s: %u&%u&%u'%s\n","Object.Name",IVORG,IVCOPY,strlen(ObjName),ObjName);
					}
					break;
				default:
					break;
				}
			}
			break;
			
		case 4:// reserved
			break;
		case 0:	// Absent Attribute
			if (!bOkObject)
			{
				s.Format ("SetType:%s\n ����֮ǰ������ȱʡ���Կ�...",SetType);
				AfxMessageBox(s);
				return false;
			}
			//����ȱʡ���ԣ�����ֵ��ģ�忽��
		case 1:	// Attribute
		case 2: // Invariant Attribute
			if(!bOkObject)
			{
				NumAttrib++;
				TempAttribLabel.Add(""); //Label
				TempAttribCount.Add(1); //Conut
				TempAttribUnits.Add(""); //Units
				TempAttribRepCd.Add(19); //Representation Code,Default is IDENT
				kAttrib=NumAttrib-1;
			}
			else
			{
				if(kAttrib>=(NumAttrib-1)) break; //�����������ֵ��ȡ��
				kAttrib++;
				strcpy(AttribLabel,(const char*)(TempAttribLabel.GetAt(kAttrib))); 
				ICount=TempAttribCount[kAttrib];
                strcpy(AttribUnits,(const char*)(TempAttribUnits.GetAt(kAttrib)));
				IRepCd=TempAttribRepCd.GetAt(kAttrib);
			}
			if(CompForm[0]=='L')
			{
				NBYTE = DlisBuf[IpntLenC];  //USHORT
				IpntLenC ++;
				memcpy(AttribLabel, &DlisBuf[IpntLenC], NBYTE);
				AttribLabel[NBYTE] = '\0';
				IpntLenC += NBYTE;
				if(!bOkObject)
				{
					TempAttribLabel.SetAt(kAttrib,AttribLabel);
				}
			}
            if(CompForm[1]=='C')
			{
				DlisIn.DLIS_UVARI(&DlisBuf[IpntLenC], ICount, NBYTE);
				IpntLenC += NBYTE;
				if(!bOkObject)
				{
					TempAttribCount.SetAt(kAttrib,ICount);
				}
				if(ICount>MAX_DATAITEM_NUM)
				{
					s.Format ("ICount=%u > MAX_DATAITEM_NUM=%u",ICount,MAX_DATAITEM_NUM);
					AfxMessageBox(s);
					return false;
				}
			}
            if(CompForm[2]=='R')
			{
				IRepCd = DlisBuf[IpntLenC];
				IpntLenC ++;
				if(!bOkObject)
				{
					TempAttribRepCd.SetAt(kAttrib,IRepCd);
				}
			}
			if(CompForm[3]=='U')
			{
				NBYTE = DlisBuf[IpntLenC];
				IpntLenC ++;
				memcpy(AttribUnits, &DlisBuf[IpntLenC], NBYTE);
				AttribUnits[NBYTE] = '\0';
				IpntLenC += NBYTE;
				if(!bOkObject)
				{
					TempAttribUnits.SetAt(kAttrib,AttribUnits);
				}
			}
			if(CompForm[4]=='V')
			{
				if(IRepCd<1 || IRepCd>27)
				{
					s.Format ("��:NumLogRec=%d Type=%u Len=%u IpntLenC=%d IRepCd=%u",
						NumLogRec,LogRecHead.Type,nLogLen,IpntLenC,IRepCd);
					AfxMessageBox(s);
					return false;//�����д�
				}
				bResult=DlisIn.Decode(&DlisBuf[IpntLenC],IpntLenC,ICount,IRepCd,
					IVALUE,VALUE,CVALUE,LCVALUE,IORIGIN,ICOPYNUM);
				if(!bResult) return false;//�����д�
				if(!bOkObject)
				{
					//��û�г��ֶ��󣬸�ֵΪģ������,�����������ʱ�ļ�
					bResult=SaveAttrib(ICount,IRepCd);
					if(!bResult) return false;//�����д�
				}
			}
			else
			{
				///////////////////////////////////
				if(IdefC == 0) 
				{   
					//ȱʡ���ԣ���ֵ
					SetZeroVALUE();
					break;
				}
				else
				{
					//������֮ģ��,ֻ�е��ѳ��ֶ������ܶ�ȡ
					if(bOkObject)
					{
						LoadAttrib();  //��ģ���м�������
					}
				}
			}
			if(bOkObject)
			{
				//����ֵ���й�����ӡ���
				bResult=ValueAnalyser(ObjName,AttribLabel,AttribUnits,ICount,IRepCd);
				if(!bResult) return false;
			}
		}
	}
	remove(sTempFile);
	return true;
}
void CDlisRead::SetZeroVALUE()
{
	CVALUE[0]=0;
	VALUE[0]=0;
	IVALUE[0]=0;
	LCVALUE[0]=0;
}
void CDlisRead::LoadAttrib()
{
	SetZeroVALUE();
	struct _stat FileInf;
	if(_stat(sTempFile,&FileInf)!=0) return;
	if(FileInf.st_size<=MAX_ATTRIB_NUM*8) return;
	int fs=FileInf.st_size;
	CString s;
	CFile f;
	int BlockPos,BlockLen;
	f.Open(sTempFile,CFile::modeRead|CFile::shareDenyNone);
	f.Seek(kAttrib*8,CFile::begin); //��λ����λ��
	f.Read(&BlockPos,4);
	f.Read(&BlockLen,4);
	if(BlockPos<(MAX_ATTRIB_NUM*8) || (BlockPos+BlockLen)>fs)
	{
		f.Close(); 
		return; //û�ж�����д�
	}
	BYTE DataType=DlisCodeOutType(TempAttribRepCd.GetAt(kAttrib)); //ȡ�����ݽ��������
	f.Seek(BlockPos,CFile::begin); //��λ��λ��
	CString sValue; //����ֵ
	switch(DataType)
	{
	case 1: //����(float)
		f.Read(&VALUE[0],BlockLen);
		sValue.Format ("%f",VALUE[0]);
		break;
	case 2: //����(int)
		f.Read(&IVALUE[0],BlockLen);
		sValue.Format ("%d",IVALUE[0]);
		break;
	case 3: //�ַ�����
		f.Read(&CVALUE[0],BlockLen);
		sValue.Format ("%s",&CVALUE[0]);
		break;
	}
	f.Close();
}
bool CDlisRead::SaveAttrib(DWORD ICount,BYTE IRepCd)
{
	CString s;
	if((kAttrib+1)>MAX_ATTRIB_NUM)
	{
		s.Format ("���Ա���ʱ����:\nkAttrib=%u ����С�� MAX_ATTRIB_NUM=%u",kAttrib,MAX_ATTRIB_NUM);
		AfxMessageBox(s);
		return false;
	}
	CFile f;
	int fs;
	NumSaveAttrib++;
	if(NumSaveAttrib==1)
	{
		TCHAR szTempPath  [MAX_PATH];
		DWORD dwResult=:: GetTempPath (MAX_PATH, szTempPath);
		ASSERT (dwResult);
		sTempFile.Format("%s~DlisAtt.tmp",szTempPath);
		fs=MAX_ATTRIB_NUM*8; //�����ļ���С����
		f.Open(sTempFile,CFile::modeCreate|CFile::modeWrite);
		f.SetLength(fs);
		f.SeekToBegin();
		char *buf=new char[fs];
		memset(buf,'\0',fs);
		f.Write(buf,fs);
		f.Close();
		delete []buf;
		buf=NULL;
	}	
	f.Open(sTempFile,CFile::modeReadWrite);
	fs=f.GetLength();
	int BlockLen=0; //�鳤��
	BYTE DataType=DlisCodeOutType(IRepCd); //ȡ�����ݽ��������
	switch(DataType)
	{
	case 1: //����(float)
	case 2: //����(int)
		BlockLen=ICount*4; //ȡ�ÿ鳤��
		break;
	case 3: //�ַ�����
		for(DWORD k=0;k<ICount;k++)
		{
			BlockLen+=LCVALUE[k]+1; //ȡ�ÿ鳤��
		}
		break;
	}
	f.Seek(kAttrib*8,CFile::begin); //��λ�������
	f.Write(&fs,sizeof(int)); //д��λ��
	f.Write(&BlockLen,sizeof(int)); //����鳤��
	f.Seek(fs,CFile::begin); //��λ�����λ��
	switch(DataType)
	{
	case 1:
		f.Write(&VALUE[0],BlockLen);
		break;
	case 2:
		f.Write(&IVALUE[0],BlockLen);
		break;
	case 3:
		f.Write(&CVALUE[0],BlockLen);
		break;
	}
	fs=f.GetLength();
	f.Close();
	return true;
}
bool CDlisRead::ValueAnalyser(char *ObjName,char *AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd)
{
	bool bResult=true;
	switch(LogRecHead.Type)
	{
	case 0: //FHLR
		bResult=ValuesToFHLR(ObjName,AttribLabel,AttribUnits,ICount,IRepCd);
		break;
	case 1:	// OLR
		if(NumOrigin<2)
		{
			bResult=ValuesToOLR(ObjName,AttribLabel,AttribUnits,ICount,IRepCd);
		}
		break;
	case 3: //CHANNL
		bResult=ValuesToCHANNL(ObjName,AttribLabel,AttribUnits,ICount,IRepCd);
		if(!bResult) return false;
		break;
	case 4: //FRAME
		bResult=ValuesToFRAME(ObjName,AttribLabel,AttribUnits,ICount,IRepCd);
		if(!bResult) return false;
		break;
	case 2: // AXIS
		bResult=ValuesToAXIS(ObjName,AttribLabel,AttribUnits,ICount,IRepCd);
		if(!bResult) return false;
		break;

	case 5: // STATIC
	case 6: // SCRIPT
	case 7: // UPDATE
	case 8: // UDI
	case 9: // LNAME
	case 10: // SPEC
	case 11: // DICT
	default:
		break;
	}
	//�����ɨ�裬����ʾ��������ֵ
	if(bPrtBaseInfo && bPrintEFLR)
	{
		Prt_EflrPar(ObjName,AttribLabel,AttribUnits,ICount,IRepCd);
	}
	return true;
}
void CDlisRead::Prt_EflrPar(char *ObjName,char *AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd)
{
	if(IRepCd>=1 && IRepCd<=11)
	{
		for(WORD i=0;i<ICount;i++)
		{
			if(i==0)
			{
				fprintf(fp,"*%30s: %G %s\n",AttribLabel,VALUE[i],AttribUnits);
			}
			else
			{
				fprintf(fp,"*%30s: %G %s\n","",VALUE[i],AttribUnits);
			}
		}
	}
	else if((IRepCd>=12 && IRepCd<=18) ||(IRepCd==22))
	{
		for(WORD i=0;i<ICount;i++)
		{
			if(i==0)
			{
				fprintf(fp,"*%30s: %d %s\n",AttribLabel,IVALUE[i],AttribUnits);
			}
			else
			{
				fprintf(fp,"*%30s: %d %s\n","",IVALUE[i],AttribUnits);
			}
		}
	}
	else
	{
		int ip=0;
		for(WORD i=0;i<ICount;i++)
		{
			if(i==0)
			{
				fprintf(fp,"*%30s: %s\n",AttribLabel,&CVALUE[ip]);
			}
			else
			{
				fprintf(fp,"*%30s: %s\n","",&CVALUE[ip]);
			}
			ip+=LCVALUE[i]+1;
		}
	}
}
void CDlisRead::Prt_RecordHeader()
{
	CString s=RecordTypeToStr(LogRecHead.Type,LogRecAttrib.Stru);
	fprintf(fp,"\n");
	fprintf(fp,"******************************* Record %5d ***********************************\n",NumLogRec);
	fprintf(fp,"*%30s: %s (%u)\n","TYPE",s,LogRecHead.Type);
	fprintf(fp,"*%30s: %u(%u) Bytes\n","Length",LogRecBytes,nLogLen);
	fprintf(fp,"*%30s: %u Bytes\n","Logical Record Position",nLogPos);
	fprintf(fp,"********************************************************************************\n");
}
CString CDlisRead::RecordTypeToStr(BYTE RecType,BYTE Stru)
{
	if(Stru==1)
	{
		switch(RecType)
		{
		case 0: return _T("FHLR"); break;
		case 1:	return _T("OLR");  break;
		case 2:	return _T("AXIS"); break;
		case 3:	return _T("CHANNL"); break;
		case 4:	return _T("FRAME"); break;
		case 5:	return _T("STATIC"); break;
		case 6:	return _T("SCRIPT"); break;
		case 7:	return _T("UPDATE");break;
		case 8:	return _T("UDI"); break;
		case 9: return _T("LNAME"); break;
		case 10: return _T("SPEC"); break;
		case 11: return _T("DICT");break;
		default: break;
		}
	}
	else
	{
		switch(RecType)
		{
		case 0: return _T("FDATA"); break;
		case 1:	return _T("NOFORM");  break;
		case 2:	return _T("AXIS"); break;
		case 127: return _T("EOD");break;
		default: break;
		}
	}
	return _T("");
}
CString CDlisRead::DlisCodeToStr(BYTE IRepCode)
{
	switch(IRepCode)
	{
	case 1: return _T("FSHORT");
	case 2: return _T("FSINGL");
	case 3: return _T("FSING1");
	case 4: return _T("FSING2");
	case 5: return _T("ISINGL");
	case 6: return _T("VSINGL");
	case 7: return _T("FDOUBL"); 
	case 8: return _T("FDOUB1");
	case 9: return _T("FDOUB2");
	case 10: return _T("CSINGL");
	case 11: return _T("CDOUBL"); 
	case 12: return _T("SSHORT");
	case 13: return _T("SNORM"); 
	case 14: return _T("SLONG");  
	case 15: return _T("USHORT");
	case 16: return _T("UNORM"); 
	case 17: return _T("ULONG"); 
	case 18: return _T("UVARI");
	case 19: return _T("IDENT"); 
	case 20: return _T("ASCII");
	case 21: return _T("DTIME"); 
	case 22: return _T("ORIGIN"); 
	case 23: return _T("OBNAME"); 
	case 24: return _T("OBJREF");
	case 25: return _T("ATTREF");
	case 26: return _T("ATATUS");
	case 27: return _T("UNITS");
	};
	return _T("");
}
BYTE CDlisRead::DlisCodeOutType(BYTE IRepCd)
{
	if(IRepCd>=1 && IRepCd<=11)
	{
		return 1; //����
	}
	else if((IRepCd>=12 && IRepCd<=18) ||(IRepCd==22))
	{
		return 2; //����
	}
	else
	{
		return 3; //�ַ���
	}
}
BYTE CDlisRead::DlisCodeToFid(BYTE IRepCd)
{
	switch(IRepCd)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
		return PC_REPR_FLOAT; //4�ֽڸ���
		break;
	case 14:
	case 16:
	case 17:
	case 18:
		return PC_REPR_LONG; //4�ֽ�����
		break;
	case 13:
	case 15:
		return PC_REPR_SHORT; //2�ֽڲ�������
		break;
	case 12:
		return PC_REPR_CHAR; //8λ��������
		break;
	default:
		break;
	}
	return PC_REPR_CHAR;
}

void CDlisRead::Prt_FileHeader()
{
	fprintf(fp,"*\n");
	fprintf(fp,"*%30s: %s\n","SEQUENCE-NUMBER",FileHeader.SequenceNumber);
	fprintf(fp,"*%30s: %s\n","ID",FileHeader.ID);
	fprintf(fp,"*\n");
	fflush(fp); //��������������������ļ�����ջ�����
}
void CDlisRead::Prt_Axis()
{
	if(NumAxis<1) return; //�����¼
	CString s;
	fprintf(fp,"*%30s: %u\n","Axis Number",NumAxis);
	fprintf(fp,"%-4s %-6s %-4s %-10s %-24s %-14s %-10s %-8s\n",
		"Num.","Origin","Copy","Name","Axis-ID","Coordinates","Spacing","Units");
	for(int i=0;i<NumAxis;i++)
	{
		fprintf(fp,"%-4d",i+1);
		fprintf(fp," %-6u",Axis[i].Origin);
		fprintf(fp," %-4u",Axis[i].CopyNum);
		fprintf(fp," %-10.255s",Axis[i].Name);
		fprintf(fp," %-24.255s",Axis[i].AxisID);

		CString sVal="";
		for(int j=0;j<Axis[i].NumC;j++)
		{
			s.Format("%G",Axis[i].Coordinates[j]);
			if(j>0)
			{
				sVal+=",";
			}
			sVal+=s;
		}
		fprintf(fp," %-14s",sVal);
		fprintf(fp," %-10G",Axis[i].Spacing);
		fprintf(fp," %-8s",Axis[i].Units);
		fprintf(fp,"\n");
	}
	fprintf(fp,"*\n");
}

void CDlisRead::Prt_Channel()
{
	CString s;
//	Prt_RecordHeader();
	fprintf(fp,"*%30s: %u\n","Channel Number",NumChannel);
	fprintf(fp,"%-4s %-3s %-2s %-10s %-10s %-14s %-6s",
		"Num.","O","C","Name","RepCode","Dimension","UNITS");
	if(NumAxis>0)
	{
		fprintf(fp," %-16s","AXIS");
	}
	fprintf(fp," %s\n","LONG-NAME");
	for(int i=0;i<NumChannel;i++)
	{
		fprintf(fp,"%-4d",i+1);
		fprintf(fp," %-3u",Channel[i].Origin);
		fprintf(fp," %-2u",Channel[i].CopyNum);
		fprintf(fp," %-10.255s",Channel[i].Name);
		s.Format ("%s(%u)",DlisCodeToStr(Channel[i].RepCode),Channel[i].RepCode);
		fprintf(fp," %-10s",(const char*)s);
		CString sVal="[";
		for(int j=0;j<Channel[i].NumDim;j++)
		{
			s.Format("%u",Channel[i].Dimension[j]);
			if(j>0)
			{
				sVal+=",";
			}
			sVal+=s;
		}
		sVal+="]";
		fprintf(fp," %-14s",sVal);
		fprintf(fp," %-6s",Channel[i].Units);
		if(NumAxis>0)
		{
			if(Channel[i].nAxis>0)
			{
				sVal="[";
				for(j=0;j<Channel[i].nAxis;j++)
				{
					if(j>0)
					{
						sVal+=",";
					}
					sVal+=Channel[i].AxisID[j]; //��ID��
				}
				sVal+="]";
			}
			else
			{
				sVal="";
			}
			fprintf(fp," %-16s",sVal);
		}
		fprintf(fp," %s",Channel[i].LongName); //LONG-NAME
		fprintf(fp,"\n");
	}
	fprintf(fp,"*\n");
}

void CDlisRead::Prt_Frame()
{
	CString s;
	fprintf(fp,"*%30s: %u\n","Frame Number",NumFrame);
	for(WORD i=0;i<NumFrame;i++)
	{
		fprintf(fp,"*\n");
		fprintf(fp,"*%30s: %u\n","Frame No.",i+1);
		fprintf(fp,"*%30s: %u&%u&%u'%s\n","Frame_Name",Frame[i].Origin,Frame[i].CopyNum,strlen(Frame[i].Name),Frame[i].Name);
		fprintf(fp,"*%30s: %d\n","Frame Number",Frame[i].noFrame);
		fprintf(fp,"*%30s: %s\n","INDEX-TYPE",Frame[i].Index_Type);
		fprintf(fp,"*%30s: %s\n","DIRECTION",Frame[i].Direction);
		fprintf(fp,"*%30s: %G\n","INDEX-START",Frame[i].Index_Start);
		fprintf(fp,"*%30s: %G\n","INDEX-STOP",Frame[i].Index_Stop);
		fprintf(fp,"*%30s: %G\n","INDEX-STEP",Frame[i].Index_Step);
		fprintf(fp,"*%30s: %s\n","INDEX-UNITS",Frame[i].IndexUnits);
		fprintf(fp,"*%30s: %G\n","INDEX-FACTOR",Frame[NumFrame-1].IndexFactor);
		fprintf(fp,"*%30s: %u\n","Channel Number",Frame[i].NumCurve);
		fprintf(fp,"*  CHANNELS =");
		for(WORD j=0;j<Frame[i].NumCurve;j++)
		{
			WORD k=Frame[i].ChannelID[j];
			if(j%10==0 && j>0)
			{
				fprintf(fp,"*   %4u    =",(j/10)*10);
			}
			fprintf(fp," %-7s",Channel[k].Name);
			if((j+1)%10==0)
			{
				fprintf(fp,"\n");
			}
		}
		if(Frame[i].NumCurve%10!=0)
		{
			fprintf(fp,"\n");
		}
	}
	fprintf(fp,"\n");
}
void CDlisRead::Prt_Origin()
{
	fprintf(fp,"*%30s: %s\n","CREATION-TIME",Origin.CreationTime);
	fprintf(fp,"*%30s: %s\n","WELL-NAME",Origin.WellName);
	fprintf(fp,"*%30s: %s\n","FIELD-NAME",Origin.FieldName);
	fprintf(fp,"*%30s: %s\n","PRODUCER-CODE",Origin.ProducerCode);
	fprintf(fp,"*%30s: %s\n","PRODUCER-NAME",Origin.ProducerName);
	fprintf(fp,"*%30s: %s\n","COMPANY",Origin.Company);
	fprintf(fp,"*\n");
}
bool CDlisRead::ValuesToFHLR(char *ObjName,char *AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd)
{
	if(strcmp(TempAttribLabel.GetAt(kAttrib),"SEQUENCE-NUMBER")==0)
	{
		sprintf(FileHeader.SequenceNumber,"%-.10s",&CVALUE[0]);
		StrTrimLeftRight(FileHeader.SequenceNumber);
	}
	else if(strcmp(TempAttribLabel.GetAt(kAttrib),"ID")==0)
	{
		sprintf(FileHeader.ID,"%-.65s",&CVALUE[0]);
		StrTrimLeftRight(FileHeader.ID);
	}
	return true;
}
bool CDlisRead::ValuesToFRAME(char *ObjName,char *AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd)
{
	CString s;
	//����FRAME
	if(strcmp(SetType,"FRAME")==0)
	{
		if(strcmp(TempAttribLabel.GetAt(kAttrib),"DESCRIPTION")==0)
		{
			sprintf(Frame[NumFrame-1].Description,"%-.255s",&CVALUE[0]);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"CHANNELS")==0)
		{
			if(ICount>MAX_FRAME_CHANNEL_NUM)
			{
				s.Format ("֡ͨ��������Ϊ:%u ���ڳ�����:%u",ICount,MAX_FRAME_CHANNEL_NUM);
				AfxMessageBox(s);
				return false;
			}
			else
			{
				Frame[NumFrame-1].NumCurve=WORD(ICount);
				int ip=0;
				for(WORD i=0;i<ICount;i++)
				{
					Frame[NumFrame-1].ChannelID[i]=-1;
					for(WORD j=0;j<NumChannel;j++)
					{
						if(IORIGIN[i] != Channel[j].Origin) continue;
						if(ICOPYNUM[i] != Channel[j].CopyNum) continue;
						if(strcmp(&CVALUE[ip],Channel[j].Name)!=0) continue;
						Frame[NumFrame-1].ChannelID[i]=j;
						break;
					}
					if(Frame[NumFrame-1].ChannelID[i]==-1)
					{	//��������£�Ӧ��������������
						s.Format ("����%sû���ҵ�ͨ��,�޷������������!",&CVALUE[ip]);
						AfxMessageBox(s);
						return false;
					}
					ip+=LCVALUE[i]+1;
				}
			}
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"INDEX-TYPE")==0)
		{
			//When a Frame has an Index, then it must be the first Channel
			//in the Frame,and it must be scalar.
			sprintf(Frame[NumFrame-1].Index_Type,"%-.255s",&CVALUE[0]);
			Frame[NumFrame-1].IndexNum = 0;
			char str[256];
			strcpy(str,Channel[Frame[NumFrame-1].ChannelID[0]].Units);
			DlisIn.UnitsTrans(str,Frame[NumFrame-1].IndexFactor);
			strcpy(Frame[NumFrame-1].IndexUnits,str);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"DIRECTION")==0)
		{
			sprintf(Frame[NumFrame-1].Direction,"%-.255s",&CVALUE[0]);
			Frame[NumFrame-1].bDiscrete=false;
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"ENCRYPTED")==0)
		{
			Frame[NumFrame-1].Encrypted=1; //�����Ƿ����ֻ������ѡ�񣬵�����ʱ��1��ʾ
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"SPACING")==0)
		{
			float tempf;
			char str[256];
			strcpy(str,AttribUnits);
			DlisIn.UnitsTrans(str,tempf);
			Frame[NumFrame-1].Index_Step=VALUE[0]*tempf;
			Frame[NumFrame-1].bDiscrete=false;
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"INDEX-MIN")==0)
		{
			float tempf;
			char str[256];
			strcpy(str,AttribUnits);
			DlisIn.UnitsTrans(str,tempf);
			Frame[NumFrame-1].Index_Start=VALUE[0]*tempf;
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"INDEX-MAX")==0)
		{
			float tempf;
			char str[256];
			strcpy(str,AttribUnits);
			DlisIn.UnitsTrans(str,tempf);
			Frame[NumFrame-1].Index_Stop=VALUE[0]*tempf;
		}
	}
	else if(strcmp(SetType,"PATH")==0)
	{	//����PATH
	}
	return true;
}
bool CDlisRead::ValuesToAXIS(char *ObjName,char *AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd)
{
	//������ĵ�λ������ǳ�������(m)�������ʱ������(us)
	CString s;
	if(strcmp(TempAttribLabel.GetAt(kAttrib),"AXIS-ID")==0)
	{
		sprintf(Axis[NumAxis-1].AxisID,"%-.255s",&CVALUE[0]);
	}
	else if(strcmp(TempAttribLabel.GetAt(kAttrib),"COORDINATES")==0)
	{
		if(ICount>MAX_DIM_NUM)
		{
			s.Format ("ʵ��ά��Ϊ:%u ���ڳ�����:%u",ICount,MAX_DIM_NUM);
			AfxMessageBox(s);
			return false;
		}
		else
		{
			float tempf;
			char str[256];
			strcpy(str,AttribUnits);
			DlisIn.UnitsTrans(str,tempf); //ȡ�õ��׻�����ת��ϵ��
			Axis[NumAxis-1].NumC=WORD(ICount);
			for(DWORD i=0;i<ICount;i++)
			{
				//ʱ������
				if(strcmp(str,"s")==0)
				{
					Axis[NumAxis-1].Coordinates[i]=VALUE[i]*tempf*1000000;
				}
				else
				{
					Axis[NumAxis-1].Coordinates[i]=VALUE[i]*tempf;
				}
			}
			strcpy(Axis[NumAxis-1].Units,AttribUnits);
		}
	}
	else if(strcmp(TempAttribLabel.GetAt(kAttrib),"SPACING")==0)
	{
		float tempf;
		char str[256];
		strcpy(str,AttribUnits);
		DlisIn.UnitsTrans(str,tempf); //ȡ�õ��׻�����ת��ϵ��
		if(strcmp(str,"s")==0)
		{
			Axis[NumAxis-1].Spacing=VALUE[0]*tempf*1000000;
			strcpy(str,"us");
		}
		else
		{
			Axis[NumAxis-1].Spacing=VALUE[0]*tempf;
		}
		strcpy(Axis[NumAxis-1].Units,str);

		//��λ����
	}
	return true;
}
bool CDlisRead::ValuesToCHANNL(char *ObjName,char *AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd)
{
	CString s;
	if(strcmp(TempAttribLabel.GetAt(kAttrib),"LONG-NAME")==0)
	{
		sprintf(Channel[NumChannel-1].LongName,"%-.255s",&CVALUE[0]);
	}
	else if(strcmp(TempAttribLabel.GetAt(kAttrib),"PROPERTIES")==0)
	{
		strcpy(Channel[NumChannel-1].Properties,"");
		int ip=0;
		for(DWORD i=0;i<ICount;i++)
		{
			//������ͨ�����Ե���󳤶�Ϊ255
			if(strlen(Channel[NumChannel-1].Properties)+LCVALUE[i]+2 > 255) break;
			if(i>0)
			{
				strcat(Channel[NumChannel-1].Properties,".");
			}
			strcat(Channel[NumChannel-1].Properties,&CVALUE[ip]);
			ip+=LCVALUE[i]+1;
		}
	}
	else if(strcmp(TempAttribLabel.GetAt(kAttrib),"REPRESENTATION-CODE")==0)
	{
		Channel[NumChannel-1].RepCode=IVALUE[0];
	}
	else if(strcmp(TempAttribLabel.GetAt(kAttrib),"UNITS")==0)
	{
		sprintf(Channel[NumChannel-1].Units,"%-.255s",&CVALUE[0]);
	}
	else if(strcmp(TempAttribLabel.GetAt(kAttrib),"DIMENSION")==0)
	{
		if(ICount>MAX_DIM_NUM)
		{
			s.Format ("ʵ��ά��Ϊ:%u ���ڳ�����:%u",ICount,MAX_DIM_NUM);
			AfxMessageBox(s);
			return false;
		}
		else
		{
			Channel[NumChannel-1].NumDim=WORD(ICount);
			Channel[NumChannel-1].Sample=1;
			for(DWORD i=0;i<ICount;i++)
			{
				Channel[NumChannel-1].Dimension[i]=IVALUE[i];
				Channel[NumChannel-1].Sample*=Channel[NumChannel-1].Dimension[i];
			}
			//�жϳ�����������(MAX_DATAITEM_NUM)�Ƿ����Ҫ��
			if(Channel[NumChannel-1].Sample>MAX_DATAITEM_NUM)
			{
				s.Format ("����:%s\n ������Ϊ:%u\n���ڳ�����:%u",
					Channel[NumChannel-1].Name,Channel[NumChannel-1].Sample,MAX_DATAITEM_NUM);
				AfxMessageBox(s);
				return false;
			}
		}
	}
	else if(strcmp(TempAttribLabel.GetAt(kAttrib),"AXIS")==0)
	{
		int ip=0;
		Channel[NumChannel-1].nAxis=WORD(ICount);
		for(DWORD i=0;i<ICount;i++)
		{
			Channel[NumChannel-1].Axis[i].Origin=IORIGIN[i];
			Channel[NumChannel-1].Axis[i].CopyNum=ICOPYNUM[i];
			strcpy(Channel[NumChannel-1].Axis[i].Name,&CVALUE[ip]);
			ip+=LCVALUE[i]+1;
		}
	}
	else if(strcmp(TempAttribLabel.GetAt(kAttrib),"ELEMENT-LIMIT")==0)
	{
		if(ICount>MAX_DIM_NUM)
		{
			s.Format ("�ļ�ά�ļ�����Ϊ:%u ���ڳ�����:%u",ICount,MAX_DIM_NUM);
			AfxMessageBox(s);
			return false;
		}
		else
		{
			Channel[NumChannel-1].NumEle=WORD(ICount);
			for(DWORD i=0;i<ICount;i++)
			{
				Channel[NumChannel-1].ElementLimt[i]=IVALUE[i];
			}
		}
	}
	else if(strcmp(TempAttribLabel.GetAt(kAttrib),"SOURCE")==0)
	{
		//���ȡǰ��255�ֽ�
		sprintf(Channel[NumChannel-1].Source,"%-.255s",&CVALUE[0]);
	}
	return true;
}
bool CDlisRead::ValuesToOLR(char *ObjName,char *AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd)
{
	CString s;
	//����ORIGIN
	BYTE DataType=DlisCodeOutType(IRepCd);
	if(strcmp(SetType,"ORIGIN")==0)
	{
		char strFormat[7];
		sprintf(strFormat,"%%-.%us",MAX_ORIGIN_LEN); //�����ִ�
		CString sValue;
		int ip=0;
		for(DWORD i=0;i<ICount;i++)
		{
			if(i>0)
			{
				switch(DataType)
				{
				case 1:
				case 2:
					sValue+=" ";
				break;
				default:
					sValue+=".";
					break;
				}
			}
			if(DataType==1)
			{
				s.Format (" %G",VALUE[i]);
				sValue+=s;
			}
			else if(DataType==2)
			{
				s.Format (" %d",IVALUE[i]);
				sValue+=s;
			}
			else
			{
				sValue+=&CVALUE[ip];
			}
			ip+=LCVALUE[i]+1;
		}
		if(strcmp(TempAttribLabel.GetAt(kAttrib),"FILE-ID")==0)
		{
			sprintf(Origin.FileID,strFormat,sValue);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"FILE-SET-NAME")==0)
		{
			sprintf(Origin.FileSetName,strFormat,sValue);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"FILE-SET-NUMBER")==0)
		{
			sprintf(Origin.FileSetNumber,strFormat,sValue);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"FILE-NUMBER")==0)
		{
			sprintf(Origin.FileNumber,strFormat,sValue);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"FILE-TYPE")==0)
		{
			sprintf(Origin.FileType,strFormat,sValue);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"PRODUCT")==0)
		{
			sprintf(Origin.Product,strFormat,sValue);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"VERSION")==0)
		{
			sprintf(Origin.Version,strFormat,sValue);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"PROGRAMS")==0)
		{
			sprintf(strFormat,"%%-.%us",4*MAX_ORIGIN_LEN); //�����ִ�
			sprintf(Origin.Programs,strFormat,sValue);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"CREATION-TIME")==0)
		{
			sprintf(Origin.CreationTime,strFormat,sValue);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"ORDER-NUMBER")==0)
		{
			sprintf(Origin.OrderNumber,strFormat,sValue);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"DESCENT-NUMBER")==0)
		{
			sprintf(Origin.DescentNumber,strFormat,sValue);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"RUN-NUMBER")==0)
		{
			sprintf(Origin.RunNumber,strFormat,sValue);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"WELL-ID")==0)
		{
			sprintf(Origin.WellID,strFormat,sValue);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"WELL-NAME")==0)
		{
			sprintf(Origin.WellName,strFormat,sValue);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"FIELD-NAME")==0)
		{
			sprintf(Origin.FieldName,strFormat,sValue);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"PRODUCER-CODE")==0)
		{
			sprintf(Origin.ProducerCode,strFormat,sValue);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"PRODUCER-NAME")==0)
		{
			sprintf(Origin.ProducerName,strFormat,sValue);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"COMPANY")==0)
		{
			sprintf(Origin.Company,strFormat,sValue);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"NAME-SPACE-NAME")==0)
		{
			sprintf(Origin.NameSpaceName,strFormat,sValue);
		}
		else if(strcmp(TempAttribLabel.GetAt(kAttrib),"NAME-SPACE-VERSION")==0)
		{
			sprintf(Origin.NameSpaceVersion,strFormat,sValue);
		}
	}
	return true;
}
bool CDlisRead::InitObject(DWORD IVORG1,BYTE IVCOPY1,char *ObjName)
{
	CString s;
	switch(LogRecHead.Type)
	{
	case 0:// FHLR
		break;
	case 1:	// OLR
		break;
	case 2:	// AXIS
		if(NumAxis >= MAX_AXIS_NUM)
		{
			s.Format ("����NumAxis�Ѵﵽ��%u ����������:MAX_AXIS_NUM=%u",NumAxis+1,MAX_AXIS_NUM);
			AfxMessageBox(s);
			return false;
		}
		else
		{
			NumAxis++; //����ֻ�д˴��仯
			sprintf(Axis[NumAxis-1].Name,"%s",ObjName);
			Axis[NumAxis-1].Origin=IVORG1;
			Axis[NumAxis-1].CopyNum=IVCOPY1;
		}
		break;
	case 3:	// CHANNL
		if(NumChannel >= MAX_FILE_CHANNEL_NUM)
		{
			s.Format ("ͨ����NumChannel�Ѵﵽ��%u ����������:MAX_FILE_CHANNEL_NUM=%u",NumChannel+1,MAX_FILE_CHANNEL_NUM);
			AfxMessageBox(s);
			return false;
		}
		else
		{
			NumChannel++; //������ֻ�д˴��仯
			sprintf(Channel[NumChannel-1].Name,"%s",ObjName);
			Channel[NumChannel-1].Origin=IVORG1;
			Channel[NumChannel-1].CopyNum=IVCOPY1;
		}
		break;
	case 4:	// FRAME
		if(NumFrame >= MAX_FRAME_OBJ_NUM)
		{
			s.Format ("֡��NumFrame�Ѵﵽ��%u ����������:MAX_FRAME_OBJ_NUM=%u",NumFrame+1,MAX_FRAME_OBJ_NUM);
			AfxMessageBox(s);
			return false;
		}
		else
		{
			NumFrame++;
			strcpy(Frame[NumFrame-1].Name,ObjName);
			Frame[NumFrame-1].Origin=IVORG1;
			Frame[NumFrame-1].CopyNum=IVCOPY1;
		}
		break;
	case 5:	// STATIC
		break;
	case 6:	// SCRIPT
		break;
	case 7:	// UPDATE
		break;
	case 8:	// UDI
		break;
	case 9: // LNAME
		break;
	case 10:// SPEC
		break;
	case 11:// DICT
		break;
	default:
		break;
	}
	return true;
}
void CDlisRead::Prt_Eod(int IVORG, int IVCOPY, char *ObjName)
{
	fprintf(fp,"\n");
	Prt_RecordHeader();
	fprintf(fp,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Record %5d ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n",NumLogRec);
	fprintf(fp,"%30s: Origin=%u Copy=%u Name=%s\n","EOD",IVORG,IVCOPY, ObjName);
	fprintf(fp,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}
int CDlisRead::GetFrameNo(int IVORG, int IVCOPY, char *ObjName)
{
	int iFrame=-1;
	for(WORD i=0;i<NumFrame;i++)
	{
		if(IVORG != int(Frame[i].Origin)) continue;
		if(IVCOPY != int(Frame[i].CopyNum)) continue;
		if(strcmp(ObjName, Frame[i].Name)==0)
		{
			iFrame = i;
			break;
		}
	}
	return iFrame;
}

void CDlisRead::GetIflrObject(int &IVORG, int &IVCOPY, int &NBYTE, char *ObjName)
{
	NBYTE=0;
	int ObjNameLen; // ����ı�ʶ������
	DlisIn.DLIS_OBNAME(DlisBuf, IVORG, IVCOPY, NBYTE, ObjNameLen);
	memcpy(ObjName, &DlisBuf[NBYTE], ObjNameLen);
	ObjName[ObjNameLen] = '\0';
	NBYTE += ObjNameLen;
}
void CDlisRead::ScanDlisFile(CString sDataFile,FILE *fo)
{
	CString s;
	bPrintEFLR=true; //������̴�ӡ��ֵ
	m_sDataFile=sDataFile;
	if(!Open(m_sDataFile,fo))
	{
		return ; //�ļ��޷���
	}
	ReadStoUnitLab();
	CString DlisLabel;
	DlisLabel.Format("%-6.6s",StoUnitLab.Stru);
	if(DlisLabel.CompareNoCase("RECORD")!=0)
	{
		Close();
		return; //����DLIS�ļ�
	}
	Prt_StoUnitLab(); //����洢��־
	bool bResult=true;
	for(NumLogRec=1;;NumLogRec++)
	{
		if(!bResult) break;
		if(GetLogicalRecord()<1) break;
		if(LogRecAttrib.Stru)
		{
			//ֱ�Ӹ�ʽ�߼���¼(Explicity Formatted)
			if(bPrtBaseInfo)
			{
				Prt_RecordHeader();
			}
			if(LogRecAttrib.Encry)
			{
				if(bPrtBaseInfo)
				{
					fprintf(fp,"*%30s: %s\n","���ܼ�¼","����");
				}
				continue;//���ܼ�¼
			}
			switch(LogRecHead.Type)
			{
			case 0: //FHLR
				if(NumLogFile>0)
				{
					//��һ�߼��ļ��Ѿ���������ʾ�߼��ļ�����
					RenewChannelData();
					Prt_LogFileIndex();
				}
				NumLogFile++; //���ļ�ͷ��¼��ʾһ���߼��ļ��Ŀ�ʼ
				VariableReset();
			break;
			case 1: //OLR
				NumOrigin++; //Դ��¼��
				break;
			case 2: //AXIS
			//	Prt_RecordHeader();
				break;
			case 3: //CHANNL
			case 4: //FRAME
			case 5: //STATIC
			default:
				break;
			}
			bResult=DlisBufToEFLR();
		}
		else
		{
			//��Ӹ�ʽ�߼���¼(Indirectly Formatted)
			int IVORG;
			int IVCOPY;
			int NBYTE;
			char ObjName[256];
			switch(LogRecHead.Type)
			{
			case 0:	//FDATA:Frame Data
				GetIflrObject(IVORG, IVCOPY, NBYTE, ObjName); //�߼���¼����
				bResult=FdataDecode(IVORG, IVCOPY, NBYTE, ObjName);
				break;
			case 1: //NOFORM:Unformatted Data
				break;
			case 127: //EOD:End Data
				GetIflrObject(IVORG, IVCOPY, NBYTE, ObjName); //�߼���¼����
				if(bPrtBaseInfo && bPrintEFLR)
				{
					Prt_Eod(IVORG, IVCOPY, ObjName);
				}
				break;
			default:
				break;
			}
		}
	}
	Close();
	//////////////////////////////////
	RenewChannelData();
	if(bResult)
	{
		Prt_LogFileIndex();
	}
	else
	{
		fprintf(fp,"\n----------- ���й��д� --------------\n\n");
		Beep(500,500);
		s.Format ("���г���,���ļ�û�����!\nɨ���ļ�Ϊ��%s",m_sDataFile);
		AfxMessageBox(s,MB_ICONSTOP);
	}
	////////////////////////////////////////////////////////////////////////////////
	fprintf(fp,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	fprintf(fp,"%40s: %u\n","Physical Record Number",NumPhyRec);
	fprintf(fp,"%40s: %u\n","Logical Record Number",NumLogRec);
	fprintf(fp,"%40s: %u\n","Buffer Length",BufLen);
	fprintf(fp,"%40s: %u\n","Logical File Number",NumLogFile);
	fprintf(fp,"%40s: %u\n","Encrypt Logical Record Number",NumEncry);
	fprintf(fp,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}
void CDlisRead::Prt_FData(DWORD i,int iChannel,int ICount,BYTE IRepCd)
{
	fprintf(fp,"%3d %+8s =",i+1,Channel[iChannel].Name);
	if(ICount==1)
	{
		fprintf(fp," %12.5f",VALUE[0]);
	}
	else
	{
		int Count=ICount;
		if(Count>NumPrtSample)Count=NumPrtSample;
		for(int k=0;k<Count;k++)
		{
			fprintf(fp," %10G",VALUE[k]);
			if((k+1)%10==0 && k<ICount-1)
			{
				fprintf(fp,"\n%13s="," ");
			}
		}
		if(Count<ICount)
		{
			fprintf(fp," ...[%d] ",ICount);
		}
	}
	fprintf(fp," %s\n",Channel[iChannel].Units);
}



void CDlisRead::Prt_LogFileIndex()
{
	fprintf(fp,"\n\n");
	Prt_FileHeader(); //��ӡ�ļ�ͷ��Ϣ	fprintf(fp,"*%30s: %s\n","ID",FileHeader.ID);
	Prt_Origin(); //��ӡԴ����(OLR:ORIGIN)��Ϣ
	Prt_Channel(); //��ӡͨ��������Ϣ
	Prt_Axis();  //��ӡ�����(AXIS:AXIS)��Ϣ
	Prt_Frame(); //��ӡ֡����(FRAME:FRAME)��Ϣ
	fprintf(fp,"\n\n");
//	Prt_LogFileCurveInfo();
}
int CDlisRead::GetDlisLogicalFileNumber()
{
	int NumLogFile=0;
	DWORD NextPos=sizeof(StoUnitLab);
	BYTE buf[8];
	LogFilePos.RemoveAll();
	for(DWORD i=0;;i++)
	{
		FilePos=NextPos;
		if(FilePos>=(FileSize-20)) break;
		fDlis.Seek(FilePos,CFile::begin);
		fDlis.Read(buf,8);
		WORD PhyRecLen=256*buf[0]+buf[1];
		WORD LogRecLen=256*buf[4]+buf[5];
		if((buf[6]>>7)==1 && (buf[7]==0x0))
		{
			LogFilePos.Add(FilePos);
			NumLogFile++;
		}
		NextPos=FilePos+PhyRecLen;
	}
	fprintf(fp,"Logical File Number: %d\n",NumLogFile);
	return NumLogFile;
}
bool CDlisRead::ScanDlisOneLogicalFile(int nf)
{
	InitVariable();
	VariableReset();
	////////////////////////////////////
	bPrintEFLR=false;
	bool bResult=true;
	bool bEndLogical=false;
	bTrans=false;
	FilePos=LogFilePos.GetAt(nf);
	int NumScanLogFile=0;//�����߼��ļ�
	for(NumLogRec=1;;NumLogRec++)
	{
		if(bEndLogical) break;
		if(!bResult) break;
		if(GetLogicalRecord()<1) break;
		if(LogRecAttrib.Encry) continue;
		if(LogRecAttrib.Stru)
		{
			//ֱ�Ӹ�ʽ�߼���¼(Explicity Formatted)
			switch(LogRecHead.Type)
			{
			case 0: //FHLR
				NumScanLogFile++;
				if(NumScanLogFile>1)
					bEndLogical=true;
				break;
			case 1: //OLR
				NumOrigin++; //Դ��¼��
				break;
			}
			if(bEndLogical) break;
			bResult=DlisBufToEFLR();
		}
		else
		{
			//��Ӹ�ʽ�߼���¼(Indirectly Formatted)
			int IVORG;
			int IVCOPY;
			int NBYTE;
			char ObjName[256];
			switch(LogRecHead.Type)
			{
			case 0:	//FDATA:Frame Data
				GetIflrObject(IVORG, IVCOPY, NBYTE, ObjName); //�߼���¼����
				bResult=FdataDecode(IVORG, IVCOPY, NBYTE, ObjName);

				break;
			case 1: //NOFORM:Unformatted Data
				break;
			case 127: //EOD:End Data
				GetIflrObject(IVORG, IVCOPY, NBYTE, ObjName); //�߼���¼����
				break;
			default:
				break;
			}
		}
	}
	////////////////////////////////////////////////////////
	RenewChannelData();
	return true;
}
bool CDlisRead::ReadLogicalFile(int nf)
{
	CString s;
	InitVariable();
	////////////////////////////////////
	bPrintEFLR=false;
	bool bResult=true;
	bool bEndLogical=false;
	int NumScanLogFile=0;//���ڽ����߼��ļ�
	if(fMid!=NULL)
	{
		delete []fMid; fMid=NULL;
	}
	fMid=new CFile[NumFrame];
	for(int i=0;i<sMidFile.GetSize();i++)
	{
		remove(sMidFile.GetAt(i));
	}
	sMidFile.RemoveAll();

	//////////////////////////////////////////////////
	TCHAR szTempPath[MAX_PATH];
	DWORD dwResult=::GetTempPath (MAX_PATH, szTempPath);
	ASSERT (dwResult);
	////////////////////////////////////////////////
	bTrans=true; 
	FilePos=LogFilePos.GetAt(nf);
	fprintf(fp,"\n��ʼ���� %d ���߼��ļ����ݣ���ʼλ��:%u Bytes\n",nf+1,FilePos);
	for(i=0;i<NumFrame;i++)
	{
		s.Format("%s~DLIS%04d.tmp",szTempPath,i);
		if(IsFileName(s))
		{
			remove((const char*)s);
		}
		sMidFile.Add(s);
		if(fMid[i].Open(s,CFile::modeCreate|CFile::modeWrite)==FALSE) 
		{
			s.Format("������ʱ�ļ� %s ����!",s);
			AfxMessageBox(s,MB_ICONSTOP);
			return false;;
		}
	}
	//����һ֡���ݽ���󳤶�
	for(i=0;i<NumFrame;i++)
	{
		Frame[i].FdataSize=0;
		for(int j=0;j<Frame[i].NumCurve;j++)
		{
			if(Frame[i].bDecode[j])
			{
				int k=Frame[i].ChannelID[j];
				Frame[i].FdataSize+=Channel[k].Sample*PcCodeLen(Channel[k].PcCode);
			}
		}
	}
	//������ʱ�ļ�����
	for(i=0;i<NumFrame;i++)
	{
		int fs=Frame[i].FdataSize*Frame[i].noFrame;
		fMid[i].SetLength(fs);
		fMid[i].SeekToBegin();
	}
	//֡������
	for(i=0;i<NumFrame;i++)
	{
		Frame[i].noFrame=0;
	}
	for(NumLogRec=1;;NumLogRec++)
	{
		if(bEndLogical) break;
		if(!bResult) break;
		if(GetLogicalRecord()<1) break;
		if(LogRecAttrib.Encry) continue;
		if(LogRecAttrib.Stru)
		{
			//ֱ�Ӹ�ʽ�߼���¼(Explicity Formatted)
			switch(LogRecHead.Type)
			{
			case 0: //FHLR
				NumScanLogFile++;
				if(NumScanLogFile>1)
				{
					bEndLogical=true;
				}
				break;
			}
		}
		else
		{
			//��Ӹ�ʽ�߼���¼(Indirectly Formatted)
			int IVORG;
			int IVCOPY;
			int NBYTE;
			char ObjName[256];
			switch(LogRecHead.Type)
			{
			case 0:	//FDATA:Frame Data
				GetIflrObject(IVORG, IVCOPY, NBYTE, ObjName); //�߼���¼����
				bResult=FdataDecode(IVORG, IVCOPY, NBYTE, ObjName);
				break;
			case 1: //NOFORM:Unformatted Data
				break;
			case 127: //EOD:End Data
				GetIflrObject(IVORG, IVCOPY, NBYTE, ObjName); //�߼���¼����
				break;
			default:
				break;
			}
		}
	}
	for(i=0;i<NumFrame;i++)
	{
		fMid[i].Close();
	}
	delete []fMid; fMid=NULL;
	return true;
}
bool CDlisRead::FdataDecode(int IVORG, int IVCOPY, int NBYTE, char *ObjName)
{
	CString s;
	bool bResult=true;
	//�ҳ�֡���
	int iFrame=GetFrameNo(IVORG, IVCOPY, ObjName);
	if(iFrame==-1) 
	{
		s.Format("Recrod=%u  ֡����:%d&%d&%d'%s  û���ҵ�˵������",NumLogRec,IVORG,IVCOPY,strlen(ObjName),ObjName);
		AfxMessageBox(s);
		return false;
	}
	if(bTrans && Frame[iFrame].FdataSize==0)
	{
		return true; //ת�����̣���Ҫ���֡���ݳ���Ϊ0����ʾû��������Ҫת��
	}
	DWORD IpntLenC_Save;
	DWORD IpntLenC=NBYTE; //λ��ָ��
	int iChannel; //��ǰͨ����
	int ICount; //������
	BYTE IRepCd; //��ʾ��

	DlisIn.DLIS_UVARI(&DlisBuf[IpntLenC], Frame[iFrame].noFrame, NBYTE); //ȡ��֡��
	IpntLenC += NBYTE;
	iChannel = Frame[iFrame].IndexNum;
	if (iChannel>=0)
	{
		iChannel=Frame[iFrame].ChannelID[0];
		ICount = Channel[iChannel].Sample;
		IRepCd = Channel[iChannel].RepCode;
		IpntLenC_Save = IpntLenC;
		bResult=DlisIn.Decode(&DlisBuf[IpntLenC],IpntLenC,ICount,IRepCd,
			IVALUE,VALUE,CVALUE,LCVALUE,IORIGIN,ICOPYNUM);
		if(!bResult)return false; //�����д�
		IpntLenC = IpntLenC_Save;
		if (Frame[iFrame].noFrame==1)
		{
			Frame[iFrame].Index_Start = VALUE[0]*Frame[iFrame].IndexFactor;
		}
		Frame[iFrame].Index_Stop = VALUE[0]*Frame[iFrame].IndexFactor;
	}
	else
	{
		if (Frame[iFrame].noFrame==1)
		{
			Frame[iFrame].Index_Start = float(Frame[iFrame].noFrame);
		}
		Frame[iFrame].Index_Stop = float(Frame[iFrame].noFrame);
	}
	if(!bTrans)
	{	//ɨ�裬��ʾһЩ����
		if(bPrintEFLR)
		{
			if(Frame[iFrame].noFrame>NumPrtFdata) return true;//ֻ��ʾǰ�漸֡����
			Prt_RecordHeader();
			fprintf(fp," Object Name: %d&%d&%d'%s\n",IVORG,IVCOPY,strlen(ObjName),ObjName);
		}
	}
	for(WORD i=0;i<Frame[iFrame].NumCurve;i++)
	{
		iChannel=Frame[iFrame].ChannelID[i];
		ICount = Channel[iChannel].Sample;
		IRepCd = Channel[iChannel].RepCode;
		bResult=DlisIn.Decode(&DlisBuf[IpntLenC],IpntLenC,ICount,IRepCd,
			IVALUE,VALUE,CVALUE,LCVALUE,IORIGIN,ICOPYNUM);
		if(!bResult)return false; //�����д�
		if(!bTrans)
		{
			if(bPrintEFLR)
			{
				Prt_FData(i,iChannel,ICount,IRepCd);
			}
		}
		else
		{
			if((Frame[iFrame].noFrame>0) && (i==0))
			{
				//���֡�ĵ�һ����Ϊ����������ֵ���е�λת��
				VALUE[0]=VALUE[0]*Frame[iFrame].IndexFactor;
				IVALUE[0]=int(IVALUE[0]*Frame[iFrame].IndexFactor);
			}
			//����֡���ݵ�ָ����ʱ�ļ�
			if(Frame[iFrame].bDecode[i])
			{
				int k=Frame[iFrame].ChannelID[i];
				//fMid[iFrame].Seek(Frame[iFrame].noFrame*Frame[iFrame].FdataSize,CFile::begin);
				int CurveSize=Channel[k].Sample*PcCodeLen(Channel[k].PcCode);
				switch(Channel[k].PcCode)
				{
				case PC_REPR_FLOAT: //4�ֽڸ���
					fMid[iFrame].Write(&VALUE[0],CurveSize);
					break;
				case PC_REPR_LONG: //4�ֽ�����
					fMid[iFrame].Write(&IVALUE[0],CurveSize);
					break;
				case PC_REPR_SHORT: //2�ֽڲ�������
					{
					short *VSHORT=new short[ICount];
					for(int j=0;j<ICount;j++)
					{
						VSHORT[j]=IVALUE[j];
					}
					fMid[iFrame].Write(&VSHORT[0],CurveSize);
					delete []VSHORT; VSHORT=NULL;
					}
					break;
				case PC_REPR_CHAR: //8λ��������
					{
					char *VCHAR=new char[ICount];
					for(int j=0;j<ICount;j++)
					{
						VCHAR[j]=IVALUE[j];
					}
					fMid[iFrame].Write(&VCHAR[0],CurveSize);
					delete []VCHAR; VCHAR=NULL;
					}
					break;
				}
			}
		}
	}
	return true;
}
bool CDlisRead::ReadCurve(int iFrame, int iCurve,int Num,float *buffer)
{
	if(Num<1) return false;
	if(!Frame[iFrame].bDecode[iCurve]) return false; //������û�����
	if(fTemp.Open(sMidFile.GetAt(iFrame),CFile::modeRead|CFile::shareDenyNone)==FALSE) return false;
	DWORD pos=0;
	int OffSet=0;
	for(int i=0;i<iCurve;i++)
	{
		if(Frame[iFrame].bDecode[i])
		{
			int k=Frame[iFrame].ChannelID[i];
			int CurveSize=Channel[k].Sample*PcCodeLen(Channel[k].PcCode);
			OffSet+=CurveSize;
		}
	}
	int k=Frame[iFrame].ChannelID[iCurve];
	BYTE RepCode=Channel[k].PcCode;
	BYTE CodeLen=PcCodeLen(RepCode);
	int Count=1;
	bool bIncreasing=true;//����Ϊ��
	if(Frame[iFrame].Index_Start>Frame[iFrame].Index_Stop)
	{
		bIncreasing=false;
	}
	for(i=0;i<Num;i++)
	{
		int n=i;
		if(!bIncreasing)n=Num-i-1; //�ϲ�
		pos=i*Frame[iFrame].FdataSize+OffSet;
		ReadData(pos,RepCode,CodeLen,Count,&buffer[n]);
	}
	fTemp.Close();
	return true;
}
bool CDlisRead::ReadData(DWORD pos,BYTE RepCode,BYTE CodeLen,DWORD count,float *buf)
{
	fTemp.Seek(pos,CFile::begin);
	int *vInt=NULL;
	DWORD *vDword=NULL;
	short *vShort=NULL;
	WORD *vWord=NULL;
	char *vChar=NULL;
	BYTE *vByte=NULL;
	double *vDouble=NULL;
	DWORD i;
	switch(RepCode)
	{
	case PC_REPR_INT: //�з�������
	case PC_REPR_LONG: //�з��ų�������
		vInt=new int[count];
		fTemp.Read(vInt,count*CodeLen);
		for(i=0;i<count;i++)
		{
			buf[i]=float(vInt[i]);
		}
		delete []vInt; vInt=NULL;
		break;
	case PC_REPR_UINT: //�޷�������
	case PC_REPR_ULONG: //�޷��ų�������
		vDword=new DWORD[count];
		fTemp.Read(vDword,count*CodeLen);
		for(i=0;i<count;i++)
		{
			buf[i]=float(vDword[i]);
		}
		delete []vDword; vDword=NULL;
		break;
	case PC_REPR_SHORT: //�з��Ŷ�������
		vShort=new short[count];
		fTemp.Read(vShort,count*CodeLen);
		for(i=0;i<count;i++)
		{
			buf[i]=float(vShort[i]);
		}
		delete []vShort; vShort=NULL;
		break;
	case PC_REPR_USHORT: //�޷��Ŷ�������
		vWord=new WORD[count];
		fTemp.Read(vWord,count*CodeLen);
		for(i=0;i<count;i++)
		{
			buf[i]=float(vWord[i]);
		}
		delete []vWord; vWord=NULL;
		break;
	case PC_REPR_FLOAT: //IEEE�����ȸ����� 
		fTemp.Read(buf,count*CodeLen);
		break;
	case PC_REPR_DOUBLE: //IEE˫���ȸ�����
		vDouble=new double[count];
		fTemp.Read(vDouble,count*CodeLen);
		for(i=0;i<count;i++)
		{
			buf[i]=float(vDouble[i]);
		}
		delete []vDouble; vDouble=NULL;
		break;
	case PC_REPR_CHAR: //�з����ֽ���
		vChar=new char[count];
		fTemp.Read(vChar,count*CodeLen);
		for(i=0;i<count;i++)
		{
			buf[i]=float(vChar[i]);
		}
		delete []vChar; vChar=NULL;
		break;
	case PC_REPR_UCHAR: //�޷����ֽ���
		vByte=new BYTE[count];
		fTemp.Read(vByte,count*CodeLen);
		for(i=0;i<count;i++)
		{
			buf[i]=float(vByte[i]);
		}
		delete []vByte; vByte=NULL;
		break;
	case PC_REPR_STRING: //�ַ���
	default:
		for(i=0;i<count;i++)
		{
			buf[i]=-999.25;
		}
		break;
	}
	return true;
}
void CDlisRead::RenewChannelData()
{
	//����֡����������������,�����������Ч
	for(WORD i=0;i<NumFrame;i++)
	{
		if(Frame[i].noFrame==0)
		{
			Frame[i].Index_Start=-999.25;
			Frame[i].Index_Stop=-999.25;
			Frame[i].Index_Step=-999.25;
		}
		else
		{
			if(Frame[i].noFrame>1)
			{
				Frame[i].Index_Step=(Frame[i].Index_Stop-Frame[i].Index_Start)/(Frame[i].noFrame-1);
			}
			else
			{
				Frame[i].Index_Step=-999.25;
			}
		}
		if(Frame[i].bDiscrete)
		{
			Frame[i].Index_Step=-999.25;
		}
	}
	//�����������Ӧ��C++�������ݴ���
	for(i=0;i<NumChannel;i++)
	{
		Channel[i].PcCode=DlisCodeToFid(Channel[i].RepCode);
	}
	//�����
	for(i=0;i<NumChannel;i++)
	{
		for(int n=0;n<Channel[i].nAxis;n++)
		{
			for(int j=0;j<NumAxis;j++)
			{
				if(Channel[i].Axis[n].Origin != Axis[j].Origin) continue;
				if(Channel[i].Axis[n].CopyNum != Axis[j].CopyNum) continue;
				if(strcmp(Channel[i].Axis[n].Name,Axis[j].Name)==0)
				{
					Channel[i].AxisStart[n]=Axis[j].Coordinates[Axis[j].NumC-1];
					Channel[i].AxisStep[n]=Axis[j].Spacing;
					strcpy(Channel[i].AxisUnits[n],Axis[j].Units);
					strcpy(Channel[i].AxisID[n],Axis[j].AxisID);
					break;
				}
			}
		}
	}
}
//�߼��ļ�������Ϣ������ض��ļ�
void CDlisRead::Prt_LogFileCurveInfo()
{
	CString s;
	int NumLog=0;
	static FILE *fCurve=NULL; //�þ�̬�������Ա���һ�ε��ÿ���
	char szCurveFile[_MAX_PATH];
	char WellName[255];
	sprintf(WellName,"%s",MyGetFileTitle(m_sDataFile));
	sprintf(szCurveFile,"%s_%03d.xls",MyGetPathNameFileTitle(m_sDataFile),NumLogFile);
	if(fCurve==NULL)
	{
		fCurve=fopen(szCurveFile,"w");
		if(fCurve==NULL)
		{
			s.Format ("��������ļ�:%s\n",szCurveFile);
			AfxMessageBox(s);
			return;
		}
		fprintf(fCurve,"����\t���\t������\tԴ\t������\t����\t��λ\t������\tά\tԪ��\t��ʾ��\t֡��\t֡��\t֡��\t��������\t������ʽ\t��ʼ����\t��ֹ����\t�������\t������λ\n");
	}

	for(WORD i=0;i<NumFrame;i++)
	{
		for(WORD j=0;j<Frame[i].NumCurve;j++)
		{
			NumLog++;
			WORD k=Frame[i].ChannelID[j];
			fprintf(fCurve,"%s",WellName);
			fprintf(fCurve,"\t%4d",NumLog);
			fprintf(fCurve,"\t%s",Channel[k].Name);
			fprintf(fCurve,"\t%u",Channel[k].Origin);
			fprintf(fCurve,"\t%u",Channel[k].CopyNum);
			fprintf(fCurve,"\t%s",Channel[k].LongName);
			fprintf(fCurve,"\t%s",Channel[k].Units);
			fprintf(fCurve,"\t%d",Channel[k].Sample);
			CString sValue="[";
			for(int k1=0;k1<Channel[k].NumDim;k1++)
			{
				if(k1>0)
				{
					sValue+=",";
				}
				s.Format ("%d",Channel[k].Dimension[k1]);
				sValue+=s;
			}
			sValue+="]";
			fprintf(fCurve,"\t%s",(const char*)sValue);
			sValue="[";
			for(k1=0;k1<Channel[k].NumEle;k1++)
			{
				if(k1>0)
				{
					sValue+=",";
				}
				s.Format ("%d",Channel[k].ElementLimt[k1]);
				sValue+=s;
			}
			sValue+="]";
			fprintf(fCurve,"\t%s",(const char*)sValue);
			s.Format ("%s(%u)",DlisCodeToStr(Channel[k].RepCode),Channel[k].RepCode);
			fprintf(fCurve,"\t%s",(const char*)s);
			fprintf(fCurve,"\t%u",i+1);
			fprintf(fCurve,"\t%u %u %s",Frame[i].Origin,Frame[i].CopyNum,Frame[i].Name);
			fprintf(fCurve,"\t%d",Frame[i].noFrame);
			fprintf(fCurve,"\t%s",Frame[i].Index_Type);
			fprintf(fCurve,"\t%s",Frame[i].Direction);
			fprintf(fCurve,"\t%G",Frame[i].Index_Start);
			fprintf(fCurve,"\t%G",Frame[i].Index_Stop);
			fprintf(fCurve,"\t%G",Frame[i].Index_Step);
			fprintf(fCurve,"\t%s",Frame[i].IndexUnits);
			fprintf(fCurve,"\n");
		}
	}
	fprintf(fCurve,"\n");
//	fclose(fCurve);
}

bool CDlisRead::ReadWaveArray(int iFrame, int iCurve,float Depth,float *buffer)
{
	CString s;
	if(!Frame[iFrame].bDecode[iCurve]) 
	{
		//s.Format ("ERROR:%d %d %f \n",iFrame,iCurve,Depth);
		//AfxMessageBox(s);
		return false; //������û�����
	}
	int noFra=DWORD((Depth-Frame[iFrame].Index_Start)/Frame[iFrame].Index_Step+0.5);
	if(noFra<0 || noFra>=Frame[iFrame].noFrame) return false; //����

	if(fTemp.Open(sMidFile.GetAt(iFrame),CFile::modeRead|CFile::shareDenyNone)==FALSE) return false;
	int OffSet=0;
	for(int i=0;i<iCurve;i++)
	{
		if(Frame[iFrame].bDecode[i])
		{
			int index=Frame[iFrame].ChannelID[i];
			int CurveSize=Channel[index].Sample*PcCodeLen(Channel[index].PcCode);
			OffSet+=CurveSize;
		}
	}
	int index=Frame[iFrame].ChannelID[iCurve];
	BYTE RepCode=Channel[index].PcCode;
	BYTE CodeLen=PcCodeLen(RepCode);
	int Count=Channel[index].Sample;
	DWORD pos=noFra*Frame[iFrame].FdataSize+OffSet;
	ReadData(pos,RepCode,CodeLen,Count,buffer);
	fTemp.Close();
	return true;
}
void CDlisRead::GetNewName(CString &sNewName,int noFrame,int Index,int CopyNum,int Origin,BOOL m_bAddFrameNo,BOOL m_bAddOrigin,BOOL m_bAddCopyNum)
{
	CString s;
	if(!m_bAddFrameNo && !m_bAddCopyNum && !m_bAddOrigin)
	{
		return ;
	}
	for(int i=0;i<=noFrame;i++)
	{
		for(int j=0;j<Frame[i].NumCurve;j++)
		{
			if(i==noFrame && j>=Index)
			{
				continue;
			}
			if(Frame[i].bDecode[j])
			{
				int k=Frame[i].ChannelID[j];
				if(sNewName.CompareNoCase(Channel[k].Name)==0)
				{
					if(m_bAddFrameNo)
					{
						s.Format ("%d",noFrame);
						sNewName+=s;
					}
					if(m_bAddCopyNum)
					{
						s.Format ("%d",CopyNum);
						sNewName+=s;
					}
					if(m_bAddOrigin)
					{
						s.Format ("%d",Origin);
						sNewName+=s;
					}
					return ;
				}
			}
		}
	}
}
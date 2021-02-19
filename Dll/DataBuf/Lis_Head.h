#if !defined(_LIS_HEAD_INCLUDED_)
#define _LIS_HEAD_INCLUDED_

#define MAX_PHYRECORD_LENGTH 8192
#define MAX_LIS_BLOCKLEN 81920

/*******************************************************************
1.Physical Record Head(4 Bytes) Description:
bit     0-15     Record Length;
bit     16       Undefined(Reserved);
bit     17  =0   Physical Record Type;
bit     18       Checksum Type Bit 1;
bit     19       Checksum Type Bit 2;
00 - No checksum present;
01 - 16 bit checksum;
10 - Undefined;
11 - Undefined;
bit     20       Undefined;
bit     21       File Number Presence;   1 - Present;
bit     22       Record Number Presence; 1 - Present;
bit     23       Undefined;
bit     24       Undefined;
bit     25       Parity Error;           0 - No parity;
bit     26       Checksum Error;         0 - No checksum error;
bit     27       Undefined;
bit     28       Undefined;
bit     29       Undefined;
bit     30       Predecessor Continuation Bit;
0 - This physical record is not associated with the previous physical record;
bit     31       Successor Continuation Bit;
0 - This physical record is not associated with the next physical record;
2.Physical Record Trailer:

  3.Logical Record(2 Bytes) Desciption:
  byte      1       Logical Record Type
  byte      2       Reserved
  
	Logical Record Type:
	132         Reel Header;
	130         Tape Header;
	128         File Header;
	34         Information;
	47         Table Dump;
	64         Data Format SpeFIDication;
	0         Log Data;
	129         File Trailer;
	131         Tape Trailer;
	133         Reel Trailer;
	232         Comment;
************************************************************/
const LIS_REPC_FLOAT16=49;
const LIS_REPC_FLOAT32LOW=50;
const LIS_REPC_INT8=56;
const LIS_REPC_ASCII=65;
const LIS_REPC_UINT8=66;
const LIS_REPC_FLOAT32=68;
const LIS_REPC_FLOAT32FIX=70;
const LIS_REPC_INT32=73;
const LIS_REPC_INT16=79;

typedef struct tagLisPhyRecordHeader
{
	unsigned short Length;  //0-15bit:�����¼����
	bool Physical_Record_Type; //17bit:�����¼���ͱ�־��Ϊ0
	bool Checksum_Type_Bit1; //18bit:У������ͱ�־1
	bool Checksum_Type_Bit2; //19bit:У������ͱ�־2
	bool File_Number_Presence; //21bit:�ļ��Ŵ��ڱ�־,1��ʾ����
	bool Record_Number_Presence; //22bit:��¼�Ŵ��ڱ�־,1��ʾ����
	bool Parity_Error; //25bit:��ż������ڱ�־,1��ʾ����
	bool Checksum_Error; //26bit:У��ʹ����־,1��ʾ����
	bool Predecessor_Continuation_Bit; //30bit:ǰ�����ӱ�־,1��ʾ������
	bool Successor_Continuation_Bit; //31bit:�������ӱ�־,1��ʾ������
} PhyRecordHeader;


typedef struct tagLisRecordHeader
{
	unsigned char type; //Record type
	unsigned char nul; //Undefined
}RecordHeader;
typedef struct tagLisTapeHeaderRec
{
	char  SerName[6]; //�����
	char  Nul1[6]; //Undefined
	char  Date[8]; //�γ��ļ�����
	char  Nul2[2]; //Undefined
	char  OriginData[4]; //������Դ
	char  Nul3[2]; //Undefined
	char  Name[8]; //����
	char  Nul4[2]; //Undefined
	char  ConnectNum[2]; //����߼����Ӻ�
	char  Nul5[2]; //Undefined
	char  PreConName[8]; //ǰ�ھ���
	char  Nul6[2]; //Undefined
	char  Comments[74]; //ע��
}TapeHeaderRec;
typedef struct tagFileHeadRec
{
	char  Name[10];  //Filename
	char  Nul1[2];  //Undefined
	char  SubSerName[6]; //Sub-Service Name
	char  Version[8];  //Version
	char  Date[8]; //Date
	char  Nul2;  //Undefined
	char  MaxPRL[5]; //Maximum Physical Record Length
	char  Nul3[2]; //Undefined
	char  FileType[2]; //File Type
	char  Nul4[2]; //Undefined
	char  PreConName[10]; //Predecessor Continuation Filename
} FileHeadRec;
typedef struct ComponentBlocks
{
	unsigned char Type; //�ɷ����ͺ�
	unsigned char Code; //�ɷݴ����
	unsigned char Length; //�ɷݳɳ���
	unsigned char Category; //�ɷݷ����
	char Name[4]; //�ɷ�����
	char Units[4]; //�ɷݵ�λ
} ComponentBlocks;
typedef struct tagEntry_Block
{
  unsigned char Type;
  unsigned char Length;
  unsigned char Code;
} Entry_Block;

typedef struct tagEntry_Value
{
	unsigned char DaRecTyp;    //1:���ݼ�¼���߼�����
	unsigned char DSBTyp;      //2:����˵��������
	unsigned short DaFraSiz;    //3:���ݼ�¼��ÿ֡���ȣ��ֽڣ�
	unsigned char UDFlag;      //4:�ϲ�/�²��־,1Ϊ�ϲ�
	unsigned char DepUnitFlag; //5:��ȵ�λ��־,1ΪӢ�ߣ�255Ϊ��,0Ϊʱ��
	int           DaRefP;      //6:���ݲο��㵽����������Ծ���
	char          DaRefUnt[4]; //7:���ݲο��㵽����������Ծ��뵥λ������Ϊ0.1in(25.4mm)
	float         FraSpace;    //8:֮֡��Ĳ�����ࡣ���ޣ��ɸ��������ȼ���
	char          FraUnt[4];   //9:�������ĵ�λ������Ϊ0.1in(25.4mm)
	unsigned char MaxFraRec;   //11:ÿ����¼�Ĵ�֡�����ɸ���ÿ֡��ʵ�ʳ��Ⱥ�ʵ�ʼ�¼���ȼ���
	float         AbsVal;      //12:ȱʡֵ,һ��Ϊ-999.25
	unsigned char DepMod;      //13:��ȼ�¼��ʽ��־��1��ʾ��ȼ�¼��ÿ֡ǰ(��λ����14��15��)��
	                           //   0��ʾ�����Ϊ���߼�¼
	char          DepUnt[4];   //14:���ֵ������λ����ȷ�ʽΪ1ʱ������Ϊ0.1in(25.4mm)
	unsigned char DepRpc;      //15:���ֵ��Ϣ����(��ȷ�ʽΪ1ʱ)
	unsigned char DSBSTyp;     //16:����˵���������ͺ�
} EntryValue;
typedef struct tagDataSpeBlockRec
{
	char      Mnemon[4]; //������
	char      ServID[6]; //������
	char      ServOrd[8]; //�����
	char      Units[4]; //������λ
	int       APICode; //API����,����0�Ϳɽ����Ϊ4����:�⾮���͡��������͡����ߵȼ�����������
	unsigned short FileNumber; //�ļ���,ָ�״λ������ʱ���ļ���
	unsigned short Size; //�����ܳ��ȣ�ָһ֡����ռ���ֽ���
	short	  Nul; //���ֽڣ�ֵΪ0
	char      ProLvl; //����ˮƽ(����0)����������1û��
	unsigned char  Samples; //��������
	unsigned char  RepC; //���ݴ���
	char      ProInd[5]; //����ˮ�ָʾ��(����1),��������0û��
} DataSpeBlockRec;
typedef struct tagCurveRec
{
	char      Name[4]; //������
	char      Units[4]; //������λ
	char      ServID[6]; //������
	char      ServOrd[8]; //�����
	int       APICode; //API����,����0�Ϳɽ����Ϊ4����:�⾮���͡��������͡����ߵȼ�����������
	unsigned short FileNumber; //�ļ���,ָ�״λ������ʱ���ļ���
	char      ProInd[5]; //����ˮ�ָʾ��(����1),��������0û��
	short     Samples; //��������
	short     Size; //�����ܳ��ȣ�ָһ֡����ռ���ֽ���
	short	  NumX; //ˮƽ����������
	char      ProLvl; //����ˮƽ(����0)����������1û��
	int NumOfDimension; //ά��
	int OffSet; //����ƫ����
	unsigned char iRepC; //�������ݴ���
	unsigned char iCodLen; //������볤��
	unsigned char oRepC; //������ݴ���
	unsigned char oCodLen; //������볤��
} CurveInfo;
////////////////////
#endif //(_LIS_HEAD_INCLUDED_)



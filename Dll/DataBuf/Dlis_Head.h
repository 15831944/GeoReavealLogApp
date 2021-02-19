#ifndef __DLIS_HEAD__
#define __DLIS_HEAD__

#define MAX_DLIS_BLOCKLEN 65536  //Buffer Length Maximum
#define MAX_FILE_CHANNEL_NUM 2048 //���������
#define MAX_AXIS_NUM 2048 //�������
#define MAX_FRAME_OBJ_NUM 64 //���֡������
#define MAX_FRAME_CHANNEL_NUM 1024 //һ�����������������
#define MAX_OBJECT_NUM 1024 //��������(��OBNAME��OBJREF��ATTREFD��ʹ��)
#define MAX_DATAITEM_NUM 102400//�����������
#define MAX_DIM_NUM 4 //���ά��
#define MAX_ORIGIN_LEN 127 //OLR:ORIGINÿ����������ֽ���
#define MAX_ATTRIB_NUM 128 //ģ�����������ֵ
const int MIN_LOG_LENGTH=16;

const DLIS_REPC_FSHORT=1;
const DLIS_REPC_FSINGL=2;
const DLIS_REPC_FSING1=3;
const DLIS_REPC_FSING2=4;
const DLIS_REPC_ISINGL=5;
const DLIS_REPC_VSINGL=6;
const DLIS_REPC_FDOUBL=7;
const DLIS_REPC_FDOUB1=8;
const DLIS_REPC_FDOUB2=9;
const DLIS_REPC_CSINGL=10;
const DLIS_REPC_CDOUBL=11;
const DLIS_REPC_SSHORT=12;
const DLIS_REPC_SNORM=13;
const DLIS_REPC_SLONG=14;
const DLIS_REPC_USHORT=15;
const DLIS_REPC_UNORM=16;
const DLIS_REPC_ULONG=17;
const DLIS_REPC_UVARI=18;
const DLIS_REPC_IDENT=19;
const DLIS_REPC_ASCII=20;
const DLIS_REPC_DTIME=21;
const DLIS_REPC_ORIGIN=22;
const DLIS_REPC_OBNAME=23;
const DLIS_REPC_OBJREF=24;
const DLIS_REPC_ATTREF=25;
const DLIS_REPC_ATATUS=26;
const DLIS_REPC_UNITS=27;

typedef struct tagStoUnitLab	//Storage Unit Label
{
	char SeqNum[4];  	//Storage Unit Sequence Number
	char DLISVer[5];	//DLIS Version
	char Stru[6];  	    //Storage Unit Structure
	char MaxRecLen[5]; 	//Maximum Record Length
	char StoSetIden[60];//Storage Set Identifier
}_StoUnitLab;
typedef struct tagPhyRecHead  //Physical Record Structure
{
	unsigned short VisRecLen; 	//Visiable Record Length
	unsigned char FormtVer[2]; 	//Format Version
}_PhyRecHead;
typedef struct tagLogRecHead //Logical Record Segment Header
{
	unsigned short Len;  	//Logical Record Segment Length
	unsigned char Attrib; //Logical Record Attrib
	unsigned char Type;	//Logical Record Segment Type
}_LogRecHead;
typedef struct tagLogRecAttrib
{
	unsigned char Stru; 	//Logical Record Structure
	unsigned char Pred; 	//Predecessor
	unsigned char Succ; 	//Successor
	unsigned char Encry;	//Encryption
	unsigned char EncryPack;//Encryption Packet
	unsigned char Checksum; //Checksum
	unsigned char TraiLen;  //Trailing Length
	unsigned char Padd;	  //Padding
}_LogRecAttrib;

typedef struct tagFileHeader
{
	char SequenceNumber[10+1]; //���к�
	char ID[65+1]; //ID��
}_FileHeader;
typedef struct tagObjName
{
	DWORD Origin;
	BYTE CopyNum;
	char Name[256];
}_ObjName;
typedef struct tagChannel
{
	char   Name[256]; //����������Ӧ�ڶ�����
	char   LongName[256]; //����:C=1,R=(OBNAME,ASCII)���˴�ֻȡ��ASCIIֵ
	char   Properties[256]; //���Ա�R=IDENT
	char   Units[256]; //��λ��C=1,R=UNITS
	_ObjName Axis[MAX_DIM_NUM]; //�᣺R=OBNAME
	float AxisStart[MAX_DIM_NUM]; //������:����ʼֵ
	float AxisStep[MAX_DIM_NUM]; //������:�Ჽ��
	char  AxisUnits[MAX_DIM_NUM][16]; //������:�ᵥλ
	char  AxisID[MAX_DIM_NUM][256]; //������:��ID��
	char  Source[256]; //����ο���C=1,R=OBJREF,�ñ�����һ�����Ͻṹ
	DWORD Origin; //Դ
	int   Sample; //�������ݸ���
	int   Dimension[MAX_DIM_NUM]; //άԪ�أ�R=UVARI
	int   ElementLimt[MAX_DIM_NUM]; //Ԫ�����ƣ�R=UVARI
	BYTE  CopyNum; //������
	BYTE  RepCode; //��ʾ�룺C=1,R=USHORT
	int   NumDim; //ά��
	int   NumEle; //ElementLimt���ϱ�
	int   nAxis; //��ά��
	BYTE  PcCode; //�����΢���ı�ʾ��
}_Channel;
typedef struct tagAxis
{
	DWORD  Origin; //Դ
	BYTE   CopyNum; //������
	char   Name[256]; //��������Ӧ�ڶ�����
	char   AxisID[256]; //��ID��:C=1,R=IDENT
	WORD   NumC; //������
	float  Coordinates[MAX_DIM_NUM]; //����
	char   Units[16]; //���굥λ
	float  Spacing; //���
}_Axis;

typedef struct tagFrame
{
	char  Name[256]; //֡������������
	char  Description[256]; //��֡����
	char  Index_Type[256]; //��������
	char  Direction[256]; //�⾮����
	char  IndexUnits[256]; //������λ 
	BYTE  CopyNum; //������������
	BYTE  Encrypted; //����
	DWORD Origin; //Դ������
	WORD  NumCurve; //��֡������
	WORD  IndexNum; //������֡�е���ţ�-1��ʾ����
	int   noFrame; //֡��
	float Index_Step; //����
	float Index_Start; //������Сֵ
	float Index_Stop; //�������ֵ
	float IndexFactor; //��Ȼ�������
	DWORD FdataSize; //һ֡���ݽ���(�����)�󳤶�
	short ChannelID[MAX_FRAME_CHANNEL_NUM]; //��֡������ͨ���е�����
	int   OffSet[MAX_FRAME_CHANNEL_NUM]; //����ƫ����
	bool  bDecode[MAX_FRAME_CHANNEL_NUM]; //�������Ƿ���
	bool  bDiscrete; //�Ƿ�Ϊ��ɢ����(trueΪ��ɢ����)
	
}_Frame;

typedef struct tagOrigin
{
	char  FileID[MAX_ORIGIN_LEN+1]; //FILE-ID
	char  FileSetName[MAX_ORIGIN_LEN+1]; //FILE-SET-NAME
	char  FileSetNumber[MAX_ORIGIN_LEN+1]; //FILE-SET-NUMBER
	char  FileNumber[MAX_ORIGIN_LEN+1]; //FILE-NUMBER
	char  FileType[MAX_ORIGIN_LEN+1]; //FILE-TYPE
	char  Product[MAX_ORIGIN_LEN+1]; //PRODUCT
	char  Version[MAX_ORIGIN_LEN+1]; //VERSION
	char  Programs[4*(MAX_ORIGIN_LEN+1)]; //PROGRAMS
	char  CreationTime[MAX_ORIGIN_LEN+1]; //CREATION-TIME
	char  OrderNumber[MAX_ORIGIN_LEN+1]; //ORDER-NUMBER
	char  DescentNumber[MAX_ORIGIN_LEN+1]; //DESCENT-NUMBER
	char  RunNumber[MAX_ORIGIN_LEN+1]; //RUN-NUMBER
	char  WellID[MAX_ORIGIN_LEN+1]; //WELL-ID
	char  WellName[MAX_ORIGIN_LEN+1]; //WELL-NAME
	char  FieldName[MAX_ORIGIN_LEN+1]; //FIELD-NAME
	char  ProducerCode[MAX_ORIGIN_LEN+1]; //PRODUCER-CODE
	char  ProducerName[MAX_ORIGIN_LEN+1]; //PRODUCER-NAME
	char  Company[MAX_ORIGIN_LEN+1]; //COMPANY
	char  NameSpaceName[MAX_ORIGIN_LEN+1]; //NAME-SPACE-NAME
	char  NameSpaceVersion[MAX_ORIGIN_LEN+1]; //NAME-SPACE-VERSION
	float DefaultVal; //ȱʡֵ
}_Origin;


///////////////////////////////////////////
#endif 


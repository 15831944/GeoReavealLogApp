#ifndef __DLIS_HEAD__
#define __DLIS_HEAD__

#define MAX_DLIS_BLOCKLEN 65536  //Buffer Length Maximum
#define MAX_FILE_CHANNEL_NUM 2048 //最多曲线数
#define MAX_AXIS_NUM 2048 //最多轴数
#define MAX_FRAME_OBJ_NUM 64 //最多帧对象数
#define MAX_FRAME_CHANNEL_NUM 1024 //一个对象中最多曲线数
#define MAX_OBJECT_NUM 1024 //最多对象数(在OBNAME、OBJREF和ATTREFD中使用)
#define MAX_DATAITEM_NUM 102400//最多数据项数
#define MAX_DIM_NUM 4 //最大维数
#define MAX_ORIGIN_LEN 127 //OLR:ORIGIN每个属性最多字节数
#define MAX_ATTRIB_NUM 128 //模板属性数最大值
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
	char SequenceNumber[10+1]; //序列号
	char ID[65+1]; //ID号
}_FileHeader;
typedef struct tagObjName
{
	DWORD Origin;
	BYTE CopyNum;
	char Name[256];
}_ObjName;
typedef struct tagChannel
{
	char   Name[256]; //曲线名，对应于对象名
	char   LongName[256]; //长名:C=1,R=(OBNAME,ASCII)，此处只取其ASCII值
	char   Properties[256]; //属性表：R=IDENT
	char   Units[256]; //单位：C=1,R=UNITS
	_ObjName Axis[MAX_DIM_NUM]; //轴：R=OBNAME
	float AxisStart[MAX_DIM_NUM]; //轴属性:轴起始值
	float AxisStep[MAX_DIM_NUM]; //轴属性:轴步长
	char  AxisUnits[MAX_DIM_NUM][16]; //轴属性:轴单位
	char  AxisID[MAX_DIM_NUM][256]; //轴属性:轴ID号
	char  Source[256]; //对象参考：C=1,R=OBJREF,该变量是一个复合结构
	DWORD Origin; //源
	int   Sample; //采样数据个数
	int   Dimension[MAX_DIM_NUM]; //维元素：R=UVARI
	int   ElementLimt[MAX_DIM_NUM]; //元素限制：R=UVARI
	BYTE  CopyNum; //拷贝数
	BYTE  RepCode; //表示码：C=1,R=USHORT
	int   NumDim; //维数
	int   NumEle; //ElementLimt的上标
	int   nAxis; //轴维数
	BYTE  PcCode; //输出到微机的表示码
}_Channel;
typedef struct tagAxis
{
	DWORD  Origin; //源
	BYTE   CopyNum; //拷贝数
	char   Name[256]; //轴名，对应于对象名
	char   AxisID[256]; //轴ID号:C=1,R=IDENT
	WORD   NumC; //坐标数
	float  Coordinates[MAX_DIM_NUM]; //坐标
	char   Units[16]; //坐标单位
	float  Spacing; //间隔
}_Axis;

typedef struct tagFrame
{
	char  Name[256]; //帧名（对象名）
	char  Description[256]; //本帧描述
	char  Index_Type[256]; //索引类型
	char  Direction[256]; //测井方向
	char  IndexUnits[256]; //索引单位 
	BYTE  CopyNum; //拷贝数（对象）
	BYTE  Encrypted; //加密
	DWORD Origin; //源（对象）
	WORD  NumCurve; //本帧曲线数
	WORD  IndexNum; //索引在帧中的序号，-1表示不在
	int   noFrame; //帧号
	float Index_Step; //步长
	float Index_Start; //索引最小值
	float Index_Stop; //索引最大值
	float IndexFactor; //深度换算因子
	DWORD FdataSize; //一帧数据解码(或编码)后长度
	short ChannelID[MAX_FRAME_CHANNEL_NUM]; //本帧曲线在通道中的索引
	int   OffSet[MAX_FRAME_CHANNEL_NUM]; //曲线偏移量
	bool  bDecode[MAX_FRAME_CHANNEL_NUM]; //该曲线是否解编
	bool  bDiscrete; //是否为离散数据(true为离散数据)
	
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
	float DefaultVal; //缺省值
}_Origin;


///////////////////////////////////////////
#endif 


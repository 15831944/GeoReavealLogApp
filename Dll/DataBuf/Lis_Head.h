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
	unsigned short Length;  //0-15bit:物理记录长度
	bool Physical_Record_Type; //17bit:物理记录类型标志，为0
	bool Checksum_Type_Bit1; //18bit:校验和类型标志1
	bool Checksum_Type_Bit2; //19bit:校验和类型标志2
	bool File_Number_Presence; //21bit:文件号存在标志,1表示存在
	bool Record_Number_Presence; //22bit:记录号存在标志,1表示存在
	bool Parity_Error; //25bit:奇偶错误存在标志,1表示存在
	bool Checksum_Error; //26bit:校验和错误标志,1表示存在
	bool Predecessor_Continuation_Bit; //30bit:前邻连接标志,1表示有连接
	bool Successor_Continuation_Bit; //31bit:后邻连接标志,1表示有连接
} PhyRecordHeader;


typedef struct tagLisRecordHeader
{
	unsigned char type; //Record type
	unsigned char nul; //Undefined
}RecordHeader;
typedef struct tagLisTapeHeaderRec
{
	char  SerName[6]; //服务号
	char  Nul1[6]; //Undefined
	char  Date[8]; //形成文件日期
	char  Nul2[2]; //Undefined
	char  OriginData[4]; //数据来源
	char  Nul3[2]; //Undefined
	char  Name[8]; //卷名
	char  Nul4[2]; //Undefined
	char  ConnectNum[2]; //卷间逻辑连接号
	char  Nul5[2]; //Undefined
	char  PreConName[8]; //前邻卷名
	char  Nul6[2]; //Undefined
	char  Comments[74]; //注释
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
	unsigned char Type; //成份类型号
	unsigned char Code; //成份代码号
	unsigned char Length; //成份成长度
	unsigned char Category; //成份范畴号
	char Name[4]; //成份名称
	char Units[4]; //成份单位
} ComponentBlocks;
typedef struct tagEntry_Block
{
  unsigned char Type;
  unsigned char Length;
  unsigned char Code;
} Entry_Block;

typedef struct tagEntry_Value
{
	unsigned char DaRecTyp;    //1:数据记录的逻辑类型
	unsigned char DSBTyp;      //2:数据说明块类型
	unsigned short DaFraSiz;    //3:数据记录中每帧长度（字节）
	unsigned char UDFlag;      //4:上测/下测标志,1为上测
	unsigned char DepUnitFlag; //5:深度单位标志,1为英尺，255为米,0为时间
	int           DaRefP;      //6:数据参考点到测量点间的相对距离
	char          DaRefUnt[4]; //7:数据参考点到测量点间的相对距离单位，隐含为0.1in(25.4mm)
	float         FraSpace;    //8:帧之间的采样间距。若无，可根据相对深度计算
	char          FraUnt[4];   //9:采样间距的单位。隐含为0.1in(25.4mm)
	unsigned char MaxFraRec;   //11:每个记录的大帧数。可根据每帧的实际长度和实际记录长度计算
	float         AbsVal;      //12:缺省值,一般为-999.25
	unsigned char DepMod;      //13:深度记录方式标志。1表示深度记录于每帧前(单位放于14和15项)，
	                           //   0表示深度作为曲线记录
	char          DepUnt[4];   //14:深度值计数单位，深度方式为1时，隐含为0.1in(25.4mm)
	unsigned char DepRpc;      //15:深度值信息代码(深度方式为1时)
	unsigned char DSBSTyp;     //16:数据说明块子类型号
} EntryValue;
typedef struct tagDataSpeBlockRec
{
	char      Mnemon[4]; //曲线名
	char      ServID[6]; //仪器名
	char      ServOrd[8]; //服务号
	char      Units[4]; //测量单位
	int       APICode; //API代码,对于0型可将其分为4部分:测井类型、曲线类型、曲线等级、曲线修饰
	unsigned short FileNumber; //文件号,指首次获得数据时的文件号
	unsigned short Size; //样点总长度，指一帧内所占的字节数
	short	  Nul; //两字节，值为0
	char      ProLvl; //处理水平(类型0)，对于类型1没用
	unsigned char  Samples; //采样点数
	unsigned char  RepC; //数据代码
	char      ProInd[5]; //处理水浃指示器(类型1),对于类型0没用
} DataSpeBlockRec;
typedef struct tagCurveRec
{
	char      Name[4]; //曲线名
	char      Units[4]; //测量单位
	char      ServID[6]; //仪器名
	char      ServOrd[8]; //服务号
	int       APICode; //API代码,对于0型可将其分为4部分:测井类型、曲线类型、曲线等级、曲线修饰
	unsigned short FileNumber; //文件号,指首次获得数据时的文件号
	char      ProInd[5]; //处理水浃指示器(类型1),对于类型0没用
	short     Samples; //采样点数
	short     Size; //样点总长度，指一帧内所占的字节数
	short	  NumX; //水平数据样点数
	char      ProLvl; //处理水平(类型0)，对于类型1没用
	int NumOfDimension; //维数
	int OffSet; //数据偏移量
	unsigned char iRepC; //输入数据代码
	unsigned char iCodLen; //输入代码长度
	unsigned char oRepC; //输出数据代码
	unsigned char oCodLen; //输出代码长度
} CurveInfo;
////////////////////
#endif //(_LIS_HEAD_INCLUDED_)



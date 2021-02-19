#if !defined(_WIS_HEAD_INCLUDED_)
#define _WIS_HEAD_INCLUDED_

#include <time.h>

//WIS文件数据类型定义
//					代码值		类型长度  	类型说明
#define WIS_REPR_INT     1	//	4			有符号整型
#define WIS_REPR_SHORT   2	//	2			有符号短整型数
#define WIS_REPR_LONG    3	//	4			有符号长整型数
#define WIS_REPR_FLOAT   4	//	4			IEEE单精度浮点数	
#define WIS_REPR_DOUBLE  5	//	8			IEE双精度浮点数
#define WIS_REPR_STRING  6	//  不定		字符串
#define WIS_REPR_CHAR    7	//	1			有符号字节数
#define WIS_REPR_UCHAR   8	//	1			无符号字节数
#define WIS_REPR_USHORT  9	//	2			无符号短整型数
#define WIS_REPR_UINT   10	//	4			无符号整型
#define WIS_REPR_ULONG  11	//	4			无符号长整型数

//对象编码
#define WIS_CHANNEL_OBJECT    0x0001
#define WIS_TABLE_OBJECT      0x0002
#define WIS_STREAM_OBJECT     0x0003

#define WIS_OBJECT_NORMAL     0x0000
#define WIS_OBJECT_DISCARD    0x0001
#define WIS_OBJECT_DELETE     0x0002

#define WIS_CURVE_OBJECT    0x0001
#define WIS_WAVE_OBJECT     0x0002
#define WIS_FMT_OBJECT      0x0003
#define WIS_TDT_OBJECT      0x0004
#define WIS_VDL_OBJECT      0x0004

#define WIS_RESULT_OBJECT   0x0001
#define WIS_CORE_OBJECT     0x0002
#define WIS_LITH_OBJECT     0x0003

#define WIS_PARA_OBJECT     0x0001
#define WIS_BINARY_OBJECT   0x0002

typedef struct tagWIS_HEAD  //文件头结构，起始偏移量为10字节
{
	WORD  MachineType;  // 机器类型：0-PC  1-SUN   2-IBM  3-HP
	WORD  MaxObjectNumber; //允许记录的最大对象数。缺省为512，该值可以在文件产生时给出
	WORD  ObjectNumber; //当前记录的对象总数（包括删除和抛弃的对象）
	WORD  BlockLen; //块长。WIS文件对象占用的磁盘空间以块为单位，该值指示每一数据块的字节数
	DWORD EntryOffset;  //对象入口记录从文件开始的偏移量
	DWORD DataOffset;  //对象数据记录从文件开始的偏移量
	DWORD FileSize;  //WIS文件的字节数大小
	time_t TimeCreate;  //WIS文件产生的时间
	char  Reserved[32];  //保留字节
}WIS_HEAD;

typedef struct tagWIS_OBJECT_ENTRY  //对象入口
{
	char   Name[16];  //对象名称，以零结尾的字符串
	long   Status;	//对象的状态：=0为正常；=1为抛弃；=2为删除
	short  Attribute;  //对象的主属性：=1为通道对象；=2为表对象；=3为流对象
	short  SubAttribute;  //对象的子属性，描述对应主属性的子属性
	DWORD  Position;  //对象数据体从文件开始处的偏移量
	DWORD  BlockNum;  //对象数据体占用磁盘的块数
	time_t TimeCreate;  //对象产生的时间
	time_t TimeWrite;   //对象最近修改的时间
	char  Reserved[32];  //保留字节
}WIS_OBJECT_ENTRY;

typedef struct tagWIS_CHANNEL_DIMENSION  //通道维信息结构
{
	char   Name[8];  //维的名称，以零结尾的字符串
	char   Unit[8];  //维的单位，以零结尾的字符串
	char   AliasName[16];  //维的别名，以零结尾的字符串
	float  StartVal;  //维的开始值
	float  Delta;  //维的采集或计算增量。对于离散数据，该值为0，数据中记录该录该维的值
	DWORD  Samples; //维的数据采样点数。如果该值为0，采栏点数为可变值，数据中记录该植。对于第一维数据，该值不能为0
	DWORD  MaxSamples;  //维的数据采样最大点数。该值仅当采样点数信息为0（可变采样点）时有效，该信息在数据中所占用的字节数通过该值计算
	DWORD  Size;  //该维上每一采样点所占用的字节数
	WORD   RepCode;  //维的数据类型
	WORD   Reserved;  //保留字节
}WIS_CHANNEL_DIMENSION;

typedef struct tagWIS_CHANNEL  //通道对象
{
	char   Unit[8];  //对象的单位，以零结尾的字符串
	char   AliasName[16];  //对象的别名，以零结尾的字符串
	char   AliasUnit[16];  //单位的别名，以零结尾的字符串
	WORD   RepCode;	  //对象的数据类型
	WORD   CodeLen;	  //对象类型的长度
	float  MinVal;	 //对象的最小值（测井曲线所缺省左刻度值）
	float  MaxVal;	//对象的最大值（测井曲线所缺省右刻度值）
	WORD   Reserved;  //保留字节
	WORD   NumOfDimension;  //对象维信息数
	WIS_CHANNEL_DIMENSION DimInfo[4];  //对象维信息
}WIS_CHANNEL;

typedef struct tagWIS_TABLE_FIELD  //表对象中的字段信息结构
{
	char   Name[32];  //字段的名称，以零结尾的字符串
	WORD   RepCode;  //字段的数据类型
	WORD   Length;  //字段值的长度
	DWORD  Reserved;  //保留字节
} WIS_TABLE_FIELD;

typedef struct tagWIS_TABLE  //表对象结构
{
	DWORD  RecordCount;  //表的记录数
	DWORD  FieldCount;  //表的字段数
	WIS_TABLE_FIELD *pField;  //指向字段信息结构的指针
}WIS_TABLE;

////////////////////
#endif //(_WIS_HEAD_INCLUDED_)
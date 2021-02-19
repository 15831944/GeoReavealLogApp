#if !defined(_FID_HEADER_INCLUDED_)
#define _FID_HEADER_INCLUDED_

//Fid_Head.h:FID文件数据的头信息定义文件
//以下代码定义与WIS文件定义一致
#define FID_REPR_INT 1
#define FID_REPR_SHORT 2
#define FID_REPR_LONG 3
#define FID_REPR_FLOAT 4
#define FID_REPR_DOUBLE 5
#define FID_REPR_STRING 6
#define FID_REPR_CHAR 7
#define FID_MAX_CURVENAME_LEN 20 //FID曲线名最长度限制
#define FID_MAX_CURVE_DIM 5 //FID曲线最大维数

typedef struct tagFID_HEAD// FID数据的头信息
{
	char Oil_Field[80];
	char Region[80];
	char Company[80];
	char Well[80];
	char X_Coordinate[40];
	char Y_Coordinate[40];
	char Exformation[80];
	int NumLog;
} FID_HEAD;

typedef struct tagFID_CHANNEL_DIMENSION// FID曲线维索引
{
	char Name[FID_MAX_CURVENAME_LEN];
	char Unit[FID_MAX_CURVENAME_LEN];
	int RepCode;
	int CodeLen;
	int Nps;
	int Npw;
	float Start;
	float Stop;
	float Rlev;
} FID_CHANNEL_DIMENSION;
typedef struct tagFID_CHANNEL// FID曲线索引
{
	char CurveName[FID_MAX_CURVENAME_LEN];
	int NumOfDimension;
	FID_CHANNEL_DIMENSION DimInfo[FID_MAX_CURVE_DIM];
} FID_CHANNEL;

#endif // !defined(FID_HEAD_)


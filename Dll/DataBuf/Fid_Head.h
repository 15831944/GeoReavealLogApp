#if !defined(_FID_HEADER_INCLUDED_)
#define _FID_HEADER_INCLUDED_

//Fid_Head.h:FID�ļ����ݵ�ͷ��Ϣ�����ļ�
//���´��붨����WIS�ļ�����һ��
#define FID_REPR_INT 1
#define FID_REPR_SHORT 2
#define FID_REPR_LONG 3
#define FID_REPR_FLOAT 4
#define FID_REPR_DOUBLE 5
#define FID_REPR_STRING 6
#define FID_REPR_CHAR 7
#define FID_MAX_CURVENAME_LEN 20 //FID�������������
#define FID_MAX_CURVE_DIM 5 //FID�������ά��

typedef struct tagFID_HEAD// FID���ݵ�ͷ��Ϣ
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

typedef struct tagFID_CHANNEL_DIMENSION// FID����ά����
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
typedef struct tagFID_CHANNEL// FID��������
{
	char CurveName[FID_MAX_CURVENAME_LEN];
	int NumOfDimension;
	FID_CHANNEL_DIMENSION DimInfo[FID_MAX_CURVE_DIM];
} FID_CHANNEL;

#endif // !defined(FID_HEAD_)


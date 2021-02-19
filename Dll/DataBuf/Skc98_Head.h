#if !defined(_SKC98_HEADER_INCLUDED_)
#define _SKC98_HEADER_INCLUDED_
//Skc98_Head.h:SKC98�ļ����ݵ�ͷ��Ϣ�����ļ�
typedef struct
{
	DWORD SKCFlag; //0:4 bytes
	char CompanyName[80]; //4:80 bytes
	char WellName[80]; //84:80 bytes
	short NumLog; //164:2 bytes
	short DriverFlag; //166:2 bytes;  "0" -- Depth Driver Logging,"1" --Time Driver Logging;
	char CurveName[30][4]; //168:120 bytes
	short CurveTypes[30]; //288:MaxCurveNum*2=60 Bytes   
	float Stdep; //348:4 bytes
	float Endep; //352:4 bytes
	float Rlev; //356:4 bytes
	short Idc; //360:2 bytes
	short Iee; //362:2 bytes
	float Feet; //364:4 bytes
	float Fill1; //368:4 Bytes
	float Fill2; //372:4 Bytes
	char LogDate[18]; //376:18 bytes
	char LogTime[18]; //394:18 bytes
	char TeamName[50]; //412:50 bytes
	char AreaName[50]; //462:50bytes
} SKC98_HEAD;

//���ݿ����������ݼ�¼��ɣ�ÿһ��ȵ���������ߵĲ�������һ�����ݼ�¼��
//���ݼ�¼�ĳ��ȸ����������ʵı仯���仯��
//���ݼ�¼�ĳ���= CurveTypes�����и�Ԫ�صĺ͡�
//CurveTypes=���߲�����/����������
//��SKC92��ͬ��CCL���ߵĲ����ʲ��ٵ�������
//����VDLΪ���ܶ����ߣ�һ��Ϊ960�����ݣ�ʱ��������Ϊ2us,��������Ϊshort
#endif //!defined(SKC98_HEAD_)

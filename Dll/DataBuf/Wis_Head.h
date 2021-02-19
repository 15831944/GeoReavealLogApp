#if !defined(_WIS_HEAD_INCLUDED_)
#define _WIS_HEAD_INCLUDED_

#include <time.h>

//WIS�ļ��������Ͷ���
//					����ֵ		���ͳ���  	����˵��
#define WIS_REPR_INT     1	//	4			�з�������
#define WIS_REPR_SHORT   2	//	2			�з��Ŷ�������
#define WIS_REPR_LONG    3	//	4			�з��ų�������
#define WIS_REPR_FLOAT   4	//	4			IEEE�����ȸ�����	
#define WIS_REPR_DOUBLE  5	//	8			IEE˫���ȸ�����
#define WIS_REPR_STRING  6	//  ����		�ַ���
#define WIS_REPR_CHAR    7	//	1			�з����ֽ���
#define WIS_REPR_UCHAR   8	//	1			�޷����ֽ���
#define WIS_REPR_USHORT  9	//	2			�޷��Ŷ�������
#define WIS_REPR_UINT   10	//	4			�޷�������
#define WIS_REPR_ULONG  11	//	4			�޷��ų�������

//�������
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

typedef struct tagWIS_HEAD  //�ļ�ͷ�ṹ����ʼƫ����Ϊ10�ֽ�
{
	WORD  MachineType;  // �������ͣ�0-PC  1-SUN   2-IBM  3-HP
	WORD  MaxObjectNumber; //�����¼������������ȱʡΪ512����ֵ�������ļ�����ʱ����
	WORD  ObjectNumber; //��ǰ��¼�Ķ�������������ɾ���������Ķ���
	WORD  BlockLen; //�鳤��WIS�ļ�����ռ�õĴ��̿ռ��Կ�Ϊ��λ����ֵָʾÿһ���ݿ���ֽ���
	DWORD EntryOffset;  //������ڼ�¼���ļ���ʼ��ƫ����
	DWORD DataOffset;  //�������ݼ�¼���ļ���ʼ��ƫ����
	DWORD FileSize;  //WIS�ļ����ֽ�����С
	time_t TimeCreate;  //WIS�ļ�������ʱ��
	char  Reserved[32];  //�����ֽ�
}WIS_HEAD;

typedef struct tagWIS_OBJECT_ENTRY  //�������
{
	char   Name[16];  //�������ƣ������β���ַ���
	long   Status;	//�����״̬��=0Ϊ������=1Ϊ������=2Ϊɾ��
	short  Attribute;  //����������ԣ�=1Ϊͨ������=2Ϊ�����=3Ϊ������
	short  SubAttribute;  //����������ԣ�������Ӧ�����Ե�������
	DWORD  Position;  //������������ļ���ʼ����ƫ����
	DWORD  BlockNum;  //����������ռ�ô��̵Ŀ���
	time_t TimeCreate;  //���������ʱ��
	time_t TimeWrite;   //��������޸ĵ�ʱ��
	char  Reserved[32];  //�����ֽ�
}WIS_OBJECT_ENTRY;

typedef struct tagWIS_CHANNEL_DIMENSION  //ͨ��ά��Ϣ�ṹ
{
	char   Name[8];  //ά�����ƣ������β���ַ���
	char   Unit[8];  //ά�ĵ�λ�������β���ַ���
	char   AliasName[16];  //ά�ı����������β���ַ���
	float  StartVal;  //ά�Ŀ�ʼֵ
	float  Delta;  //ά�Ĳɼ������������������ɢ���ݣ���ֵΪ0�������м�¼��¼��ά��ֵ
	DWORD  Samples; //ά�����ݲ��������������ֵΪ0����������Ϊ�ɱ�ֵ�������м�¼��ֲ�����ڵ�һά���ݣ���ֵ����Ϊ0
	DWORD  MaxSamples;  //ά�����ݲ�������������ֵ��������������ϢΪ0���ɱ�����㣩ʱ��Ч������Ϣ����������ռ�õ��ֽ���ͨ����ֵ����
	DWORD  Size;  //��ά��ÿһ��������ռ�õ��ֽ���
	WORD   RepCode;  //ά����������
	WORD   Reserved;  //�����ֽ�
}WIS_CHANNEL_DIMENSION;

typedef struct tagWIS_CHANNEL  //ͨ������
{
	char   Unit[8];  //����ĵ�λ�������β���ַ���
	char   AliasName[16];  //����ı����������β���ַ���
	char   AliasUnit[16];  //��λ�ı����������β���ַ���
	WORD   RepCode;	  //�������������
	WORD   CodeLen;	  //�������͵ĳ���
	float  MinVal;	 //�������Сֵ���⾮������ȱʡ��̶�ֵ��
	float  MaxVal;	//��������ֵ���⾮������ȱʡ�ҿ̶�ֵ��
	WORD   Reserved;  //�����ֽ�
	WORD   NumOfDimension;  //����ά��Ϣ��
	WIS_CHANNEL_DIMENSION DimInfo[4];  //����ά��Ϣ
}WIS_CHANNEL;

typedef struct tagWIS_TABLE_FIELD  //������е��ֶ���Ϣ�ṹ
{
	char   Name[32];  //�ֶε����ƣ������β���ַ���
	WORD   RepCode;  //�ֶε���������
	WORD   Length;  //�ֶ�ֵ�ĳ���
	DWORD  Reserved;  //�����ֽ�
} WIS_TABLE_FIELD;

typedef struct tagWIS_TABLE  //�����ṹ
{
	DWORD  RecordCount;  //��ļ�¼��
	DWORD  FieldCount;  //����ֶ���
	WIS_TABLE_FIELD *pField;  //ָ���ֶ���Ϣ�ṹ��ָ��
}WIS_TABLE;

////////////////////
#endif //(_WIS_HEAD_INCLUDED_)
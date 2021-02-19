#if !defined(_XTF_HEADER_INCLUDED_)
#define _XTF_HEADER_INCLUDED_

//XTF�ļ��������Ͷ���
//					����ֵ		���ͳ���  	����˵��
#define XTF_REPR_BIT     1	//	1			λֵ
#define XTF_REPR_INT2    2	//	2			�з��Ŷ�������
#define XTF_REPR_UCHAR   3	//	1			�޷����ֽ�
#define XTF_REPR_FLOAT   4	//	4			IEEE�����ȸ�����	
#define XTF_REPR_I12S    5	//	?			���������
#define XTF_REPR_I12U    6	//  ?			���������
#define XTF_REPR_CHAR    7	//	1			�з����ֽ���
#define XTF_REPR_DOUBLE  8	//	8			˫���ȸ���
#define XTF_REPR_UINT2   9	//	2			�޷��Ŷ�������
#define XTF_REPR_INT4    10	//	4			������
#define XTF_REPR_UINT4   11	//	4			�޷��ų�������
#define XTF_REPR_UNKNOWN  12	//	?		���������

//XTF�����ļ���ͷ��Ϣ�ṹ
//�û������������ͣ���XTF�ļ���һ����¼���õ�
typedef struct
{
	char Name[8];  //����������
	int StartRecord;  //��ʼ��¼
	int StopRecord;   //��ֹ��¼
}USERDATA;

//ÿ���������ͼ����ԣ���XTF�ļ����߸���¼���õ�
typedef struct
{
	BYTE CurType;  //��������(1-3)�����桢���Ρ�����
	BYTE RepCode;  //��������(1-12)
	BYTE HorType;   //ˮƽ����
	BYTE VerType;  //��ֱ����
}CURVEATTRIB;

typedef struct tagXTF_HEAD
{
	//��һ��ͷ��¼��1-4096
	//�����ļ�����ϵͳ���롢�������͵�
	int header_location[8];  //�ļ�ͷ��¼��λ��
	int iunused[182];   //δ��
	char extension[4];  //��չ����һ��Ϊ��.XTF��
	char FileFullName[80];  //�ļ�ȫ������·������
	char Depth_Unit[8];  //��ȵ�λ
	char AIF_Revision_numbar[8];  //AIF��ð汾��
	char strunused[88];  //δ��
	BYTE System_Code;  //ϵͳ���룬1=PC��2=PE��3=VAX��4=IBM��5=HP9000/SUN
	BYTE QC; //����ָʾĿ��ϵͳ�ڽ������и�ʽת��
	BYTE Major_Revision_Number; //SURVLIB ���汾��
	BYTE Minor_Revision_Number; //SURVLIB ���汾��
	short hunused[22];  //δ��
	int isnumcv;  //��������
	int ismaxcv;  //���������,һ��Ϊ512
	int iunused2[5];  //δ��
	USERDATA UserData[64];  //���ݼ�¼���ͼ�����ֹ��¼
	float wttop;      //ȱʡ��ʼ���
	float wtbottom;   //ȱʡ�������
	float wtlevel;    //ȱʡ�������
	char strunused2[1012];  //δ��
	char strunused3[1020]; //δ��
	int  FileRecord;  //�ļ���¼��

	//�ڶ�ͷ��¼��4097-8192
	//�����̴洢��Ϣ������ȷ���ļ���δʹ�õĻ���ɾ���������Լ��ļ�
	//��ʱ��������¼�����Ĳ��֡�
	//������һ��4096�ֽڵ����ݿ�ʱ�����Ϊ1
	//�ܿռ�4072*8*4096=133,431,296�ֽ� =127.25MB
	BYTE Datamap[4072];  //λͼ����һ�ֽڷ�ӳ8��ͷ��¼�����ӵĵ�����ͼλ��
	int bitmapfullflag;  //λͼ���ر��
	int firstrecord;     //λͼ�е������¼
	int lastrecord;      //λͼ�е����һ����¼
	int prerecord;       //ǰһ��λͼ�ļ�¼��
	int thisrecord;      //��ǰλͼ��¼��
	int nextrecord;      //��һ��λͼ�ļ�¼��
	
	//����ͷ��¼��8193-12288
	//����������Ϊ��ʱ��ʾ�����߲����ڻ��ѱ�ɾ��
	char CurveName[512][8];
	
	//����ͷ��¼��12289-16384
	//���ߵ���ʼλ�ü���������
	//ע�⣺1)��ʼλ�õ�������1,257,2,258,...256,512;
	//      2)���߲���������������ʼλ����ͬ
	int StartOffset[512];   //ÿ�����ߵ���ʼ��¼�ţ���1��ʼ����
	int LevelNumber[512];   //ÿ�����ߵĲ�����

	//����ͷ��¼:16385-20480
	//����ά�����壬���˳��Ϊ��
	//����1��ά��,��һά��Ԫ��,��257��ά��,��һά��Ԫ��,����2��...
	//����256��ά��,��һά��Ԫ��,��512��ά��,��һά��Ԫ��
	//����1�ڶ�ά�ĵ�Ԫ�����ڶ�ά��Ԫ��,
	//����257�ڶ�ά�ĵ�Ԫ�����ڶ�ά��Ԫ��,
	//......
	//����256�ڶ�ά�ĵ�Ԫ�����ڶ�ά��Ԫ��,
	//����512�ڶ�ά�ĵ�Ԫ�����ڶ�ά��Ԫ��,
	short ndimension[512]; //���ߵ�ά����ȡֵΪ1-3
	short idimen1[512];    //���ߵ�һά�ĵ�Ԫ��
	short idimen2[512];    //���ߵڶ�ά�ĵ�Ԫ��
	short idimen3[512];    //���ߵ���ά�ĵ�Ԫ��

	//����ͷ��¼:20481-24576
	//��¼ÿ�����ߵ���ֹ���,�����ȸ�����
	//��¼˳��
	//��1,257,2,258,...,256,512���ߵĶ������
	//��1,257,2,258,...,256,512���ߵĵײ����
	float Sdep[512];  //���ߵ���ʼ���
	float Edep[512];    //���ߵĽ������

	//����ͷ��¼:24577-28672
	//��¼ÿ�����߲���������������͡��������Ϊfloat�ͣ�
	//�������Ͷ������ߡ����ݡ�ˮƽ�ʹ�ֱ����������
	//��¼˳��
	//��1,257,2,258,...,256,512���ߵĲ������
	//��1,257,2,258,...,256,512���ߵ���������
	float Level[512];     //���߲������
	CURVEATTRIB CurveAttrib[512]; //���ߡ����ݡ�ˮƽ�ʹ�ֱ����������

	//�ڰ�ͷ��¼:28673-32768
	//������Ϣ������4������ÿ����������ͬ����������
	//��һ����ASCII��Ϣ
	//�ڶ������������������Ⱥ�˫���ȣ�
	//�����������ͣ������ͺͳ����ͣ�
	//�������������ƣ�δ�ã�
	char Wellsiteinfo[8];  //��WSI����ǣ���ʾһ����Ч�ľ�����Ϣ��¼
	char Wellname[80];     //����
	char Fieldname[80];    //��������
	char Companyname[80];  //��˾��
	char Countryname[32];  //������
	char Countyname[32];   //����
	char Statename[16];    //����
	char OCTname[8];       //�������Ʊ���
	char Welllocation[40]; //����λ��
	char Section[4];       //������
	char Township[4];      //������
	char Range[4];         //��Χ
	char Permanentdatum[2]; //��������,��gl����sl��
	char Drillingmeasurementdatum[2];  //�꾮��������(��kb������gl����)
	char LogAnalysisCenter[4];  //�⾮�������Ĵ���
	char Wellunits[8];          //����������λ
	char strunused4[620];    //δ��

	float Latitude;   //����
	float Longitude;  //γ��
	float Kellybushing;  //���겹�ĺ����߶�
	float Derrickfloor;  //����ƽ̨�����߶�
	float Elevation;    //������������
	float MagneticDeclination;  //��ƫ��
	float ECC;   //ECC��
	float Groundtosealevelelevation; //���溣���߶�
	float funused[120];   //δ�õ�������
	double dunused[64];   //δ��˫������

	short hunused2[6];  //δ��
	short Metricflag;  //���Ʊ��:0Ϊ����,1ΪӢ��
	short Maximumcurveversion; //�����������(1-99)
	short hunused3[504];  //δ��
	char unformattedbinaryinformation[1024]; //δ��ʽ����������Ϣ

	//���ߵ�λ,���ݴ�����ͷ�еõ�
	char Units[512][8];
	char IndexUnit[8]; //������λ

}XTF_HEAD;

//����ͷ��Ϣ
typedef struct 
{
	char Name[8];  //����������������ĸ��ͷ
	char Units[8]; //����������Һ��ɴ�ӡ�ַ�
	char Comment[24];   //�����߱�ע�ֶΣ�����AIF�ļ�����ı�ע��
	char ServiceCompany[8]; //����˾,�硰ATLAS��
	char ToolType[8];  //��������
	char CurveClassType[8];  //���߷������ͣ��硰WAVEFORM������DIPLOG����
	char DepthUnits[8];  //��ȵ�λ���ṩ��λ������
	char TimeIncrementUnits[8];  //�������ߵ�ʱ��������λ
	char StartTimeUnits[8];  //��ʼʱ�䵥λ,���������й̶��Ϳɱ����ʼʱ��
	char RawTapeHeaderMnemonic[8];  //ԭʼ����ͷ���Ƿ�
	char UnitsOfStationary[8]; //��̬��ȵ�λ
	char CorrelogramParamterUnits[8];  //���ͼ������λ
	char CbilOrientationMnemonic[8];  //CBIL��λ���Ƿ�
	char LoggingDirection[8];  //�⾮����,ȡֵΪ��up������down����
	char AifCurveName[24]; //AIF�������Ͱ汾
	char OCT[8];  //���������ߵ�OCT
	char ToolNames[48];  //8�����ַ���������
	char strunused[8];  //δ��
	char strunused2[8];  //δ��
	char LisName[4];  //LIS������
	char strunused3[4]; //δ��
	char CreationRoutineName[16]; //ʮ���ַ��Ĵ������̺Ͱ汾
	char ModifiedRoutineName[16]; //ʮ���ַ����޸����̺Ͱ汾
	char strunused4[28];   //δ��
	char LongName[20];  //�ӳ��˵�������
	char strunused5[712];  //δ��

	float deptop;  //���ߵĶ������
	float depbot;  //���ߵĵײ����
	float rlevcv;  //���ߵĲ������
	float curvmin; //������Сֵ
	float curvmax; //�������ֵ
	float curvavg; //����ƽ��ֵ
	float timeinc; //ʱ�����������������
	float starttm; //������������ʼʱ��
	float stadepth; //��̬�������
	float stddev;  //��׼����
	float cpwinbeg; //���ͼ������Window begin��
	float cpwinstp; //���ͼ������Window step��
	float cpwinlen; //���ͼ������Window length��
	float cbilodeg; //CBIL��λ�Ķ���
	float IntervalTR;  //���������������ľ���
	float IntervalRR;  //���������������ľ���
	float RawOffset;   //ԭʼ����ƫת
	float TimeDelay;   //ʱ���ӳ�
	float Angle;       //�ο��۵������б۵�ƽ��
	float XPad;    //�����ϴ�������Xλ��
	float YPad;    //�����ϴ�������Yλ��
	float LossMinor[6]; //�μ���ʧ����ֵ
	float funused[101]; //δ��

	double dblinit;  //˫���ȳ�ʼֵ
	double dbtopdep; //˫���ȶ������
	double dbbotdep; //˫���ȵײ����
	double dbrlevel; //˫���Ȳ������
	double smvalsdb[6]; //Array pf 6 secondary Missing Datum Value
	double idxtop2; //�μ���������ͷ��
	double idxbot2; //�μ����������ײ�
	double idxrlev2; //�μ������������
	double dunused[51]; //δ��

	int EncodedCreationDate;  //�������ڱ���
	int EncodedCreationTime;  //����ʱ�����
	int EncodedLastAccessDate;  //����ȡ���ڱ���
	int EncodedLastAccessTime;  //����ȡʱ�����
	int NumberOfCurveHeaderrecords;  //����ͷ��¼��
	int RecordNumberOfLastCurvedataRecord;  //�������һ�����ݼ�¼��
	int DatumInitializationVariable;  //���ݳ�ʼ������
	int NumberOfLevelsForTheCurve;  //���ߵĲ�����
	int CurrentEncodedDateSignature;  //��ǰ�������ڱ��
	int CurrentEncodedTimeSignature;  //��ǰ����ʱ����
	int LossMinor2[6]; //6���μ���ʧ����ֵ������
	int Arrays2[2];  //��ֵ���У�����ṹ�����߶������ĩ��¼
	int Arrays3[2];  //��ֵ���У�����ṹ����������������ĩ��¼
	int iunused[108];  //δ��
	
	short SurvlibMajor;  //SURVLIB��Ҫ��
	short SurvlibMinor; //SURVLIB��Ҫ��
	short Ihcurv;  //����ʱ�����߾��
	short Ictype;  //��������:1-����,2-����,3-����
	short RepCode;  //��������:2-INT*2,4=REAL*4,��
	short Ivtype;  //��ֱ��������
	short Ihtype;  //ˮƽ��������
	short Ndims;   //���ߵ�ά��
	short Idims[7];  //��iά�ĵ�Ԫ��
	short I2init;  //INT*2�����ĳ�ʼֵ
	short Intflfix;  //1-INT/2-FLOAT/3-FIXED�ı�־
	short Isign; //�з���/�޷��ű��
	short Numbits; //ÿ���ݵ�Ԫ����λ��
	short Lisrepcd; //��ȡ��ֵ����������ԭʼLIS������ʽ
	short Lisnsamp;  //Nsamp���ͬʱLisrepcd�й�
	short Lisrcsiz;  //LIS�����С���ֽڣ�
	short Lisnsize;  //ÿ������LIS�����ܽڽ���
	short Istartfv;  //0-�̶���������ʼʱ��,1-�ɱ��������ʼʱ��
	short Igainfv;   //��Ϊ0����������������̶�
	short Igainepx;  //���й̶���������л����ߵ�����ָ��
	short Igainmth;  //������������л����ߵ������ʾ��-�̶���ɱ�
	short Igainapp;  //����Ӧ�ñ�־
	short Lntaphed;  //RAWTPHD�ĳ���
	short Ntools;    //������
	short Idxingrp;  //��������
	short Smvalsi2[6];  //6���μ���ʧ����ֵ������
	short hunused[219];  //δ��
	
	BYTE TransmitterID[24];  //24�ֽڴ���24��������ID��
	BYTE ReceiverID[24];  //24�ֽڴ���24��������ID��
	BYTE RawTapeHeaderInfo[276];  //ԭʼ��ͷ��Ϣ
	BYTE LossMinor3[6];  //6�ֽڴ������ʧ��ֵ������
	BYTE LossMinor4[6];  //6�ֽڴ������ʧ��ֵ������
	BYTE bunused[687];   //δ��
	BYTE SystemCode;    //ϵͳ����:1-PC,2-PE,3-VAX,4-IBM Mainframe,5-hp9000/Sun,etc.

}CURVE_HEAD;
#endif //(_XTF_HEADER_INCLUDED_)
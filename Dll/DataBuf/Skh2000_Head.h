#if !defined(_SKH2000_HEADER_INCLUDED_)
#define _SKH2000_HEADER_INCLUDED_
//Skh2000_Head.h:SKH2000�ļ����ݵ�ͷ��Ϣ�����ļ�
typedef struct
{
	float Ecc; //0:4 �ɹ���
	char CompanyName[40]; //4:80 ��˾��
	char TeamName[20]; //44:20 ����
	char Operator[20]; //64:20 ����Ա
	char WellName[40]; //84:40 ����
	char WellSite[40]; //124:40 ��λ
	short NumLog; //164:2 ��������
	short Iz; //166:2 δʹ��
	char CurveName[20][4]; //168:80 ��������ÿ����4�ַ�
	float Stdep; //248:4 ��ʼ���
	float Endep; //252:4 ��ֹ���
	float Rlev; //256:4 ����������ף�
	short Idc; //260:2 δʹ��
	short Iee; //262:2 δʹ��
	float Spcpr; //264:4 ÿ��¼���ݸ������̶�128��
	BYTE N_SERV[6]; //268:6 �⾮��Ŀ��(FIRST BYTE)
	WORD IFILE; //274:2 δʹ��
	BYTE YEAR[2]; //276:2  ��(BCD CODE)
	BYTE MONTH; //278:1 ��(BCD CODE)
	BYTE DATE; //279:1 ��(BCD CODE)
	BYTE HOUR; //280:1  ʱ(BCD CODE)
	BYTE MINUTE; //281:1 ��(BCD CODE)
	char TOOL_TYPE[5][8]; //282:40 ��������(8)
	char TOOL_SN[5][8]; //322:40 �������(8)
	char szReserved[148]; //����szReserved�Ǻ�������ļ򻯣�
	//���Ҫʹ�������������ȥ���ñ��������ڱ���ʱӦ���ö���Ϊ2�ֽڻ�1�ֽڣ�
	//���÷�����Project->Settings...->C/C++->Categoty:Code Generation->2 Bytes

	//BYTE EDT_TAG; //362:1 �༭��־��1=�༭
	//BYTE TAG_TVRC; //363:1 0=ʱ��,1=���ܶ�,2=RFT,3=����
	//float DRIL_DEPTH; //364:4 �꾮���
	//float CASE_DEPTH; //368:4 �׹����
	//char LIQ_TYPE[12]; //372:12 �ཬ����
	//float DCASE; //384:4 �׹��ھ�
	//float HPLAT; //388:4 ���ĸ߶�
	//float LIQ_DEN; //392:4 �ཬ����
	//float LIQ_NIAN; //396:4 �ཬճ��
	//float LIQ_PH; //400:4 �ཬPHֵ
	//float LIQ_LOSS; //404:4 �ཬʧˮ��
	//BYTE DEP_CORR; //408:1 ���У������
	//BYTE BCP_TAG; //409:1 0=���ѹ��,1=�¶�,2=ͼͷ���
	//float CASE_SIZEO; //410:4 �׹��⾶
	//char WITNESS[10]; //414:10 ����Ա��
	//char G_LEADER[10]; //424:10 �ӳ���
	//BYTE RFT_TAG; //434:1 RFT�⾮��־
	//BYTE RFT_PRS; //435:1 RFTѹ����ʾ��־
	//float DDRV1; //436:4 ��ͷֱ��1
	//float DDRV2; //440:4 ��ͷֱ��2
	//float DDRV3; //444:4 ��ͷֱ��3
	//float DDRV4; //448:4 ��ͷֱ��4
	//float DEPDRV1; //452:4 ��ͷ���1
	//float DEPDRV2; //456:4 ��ͷ���2
	//float DEPDRV3; //460:4 ��ͷ���3
	//float DEPDRV4; //464:4 ��ͷ���4
	//float RAO; //468:4 �ཬ������
	//float DEPTH_MEA; //472:4 ��ʱ����
	//float TEMPC; //476:4 �����¶�
	//char CurveName2[5][4]; //480:20 CHAR ������(5X4)
	//short DIFF_PRSS; //500:2 ���ѹ������
	//short DIFF_TEMPC; //502:2 ����¶Ⱦ���
	//float HIGH_CER; //504:4 Ԥ��ˮ�෵��
	//float TIME_GJ; //508:4 �̾�ʱ��
} SKH2000_HEAD;
#endif //!defined(SKH2000_HEAD_)

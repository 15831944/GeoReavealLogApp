#ifndef __BIT_HEADER__
#define __BIT_HEADER__
//Bit_Head.h:BIT�ļ����ݵ�ͷ��Ϣ�����ļ�
typedef struct // BIT���ݵ�ͷ��Ϣ
{
	BYTE NumWF;//��������
	BYTE RlevTime;//���β���ʱ������US��
	short NumSample;//ÿ�����εĲ�������
    char CompanyName[72]; //��˾��
	short nMonth; //��
	short nDay; //��
	short nYear; //��
	short nReserved; //δ����
	char WellName[72]; //����
	short nHour; //ʱ
	short nMinute; //��
	short nSecond; //��
	short nReserved2; //δ����
    short NumLog; //�⾮��������
	short UnitFlag; //��λ��־(����/Ӣ��)
    char CurveName[20][4]; //������
    float Stdep; //����
	float Endep; //ֹ��
	float Rlev; //�������
    short Idc; //δ����
	short DataFlag; //��ʽ��־��1024Ϊ���Σ�0Ϊ����
    float Spcpr; //��������
	char  LogServ[6]; //�⾮�����
	char  FileNum[2]; //�ļ���
} BIT_HEAD;
#endif 

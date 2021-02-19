#if !defined(QH716_HEAD_INCLUDE_)
#define QH716_HEAD_INCLUDE_
//Qh716_Head.h ͷ��Ϣ����
typedef struct
{
	int Head; 
	float ECC; 
	char CorpName[80];
	char WellName[80];
	short NumLog,IZ;
	char CurveName[128][4];
	float Stdep;
	float Endep;
	float Rlev;
	short IDC,IZZ;
	float Feet;
	short DumFull[4];
	int End; 
}QH716_HEAD;

#endif // !defined(QH716_HEAD_INCLUDE_)
/*
�ຣ����LA716�ļ���ʽ
1.�ļ�������ܳ��ȴ��ڻ����284�ֽ�
				����			����		����		��ֵ
Head  			�ļ�ͷ��ʼ		long		4�ֽ�		276
ECC				�ɹ���			float		4�ֽ�		1.0
CorpName		��˾��			char		80�ֽ�		ʵ�ʹ�˾��
WellName		����			char		80�ֽ�		ʵ�ʾ���
NumLog			������			short		2�ֽ�		ʵ��������
IZ				��λ���		short		2�ֽ�		0
LogName			������			char		80���		���㲹�ո�
StartDep		��ʼ���		float		4�ֽ�		ʵ����ʼ���
EndDep			�������		float	 	4�ֽ�		ʵ�ʽ������
Rlev			�������		float		4�ֽ�		ʵ�ʲ������
IDC								short		2�ֽ�		12842
IZZ								short		2�ֽ�		0
Feet			��������		float		4�ֽ�		125��64
DumFull			�����			short		4*2�ֽ�		�ո�ֵ
LogName��������(ֻ�е�����������20ʱ����)
End	
2.�ļ����ݿ�
һ�����ݿ飽4+4*NumLog*Feet+4�ֽ�
(ǰ��ͺ����4�ֽ�Ϊ��꣬��¼�����ֽ���)
3.�����ʽ
  ����ļ����룺SUN����վ��ʽ����΢����ʽ˳���෴
*/
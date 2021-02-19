#if !defined(_HEAD_XB716_INCLUDED_)
#define _HEAD_XB716_INCLUDED_
typedef struct /* ���ǹ�˾LA716�����ļ�ͷ��Ϣ*/
{
	float ECC; 
	char CorpName[80];
	char WellName[80];
	short NumLog,IZ;
	char CurveName[64][4];
	float Stdep;
	float Endep;
	float Rlev;
	short IDC,IZZ;
	float Feet;
	char  DumFull[68];
} XB716_HEAD;
#endif //(_HEAD_XB716_INCLUDED_)
/*
������LA716�ļ���ʽ
1.�ļ�������ܳ��ȵ���512�ֽ�
				����			����		����		��ֵ
ECC				�ɹ���			float		4�ֽ�		0.0
CorpName		��˾��			char		80�ֽ�		ʵ�ʹ�˾��
WellName		����			char		80�ֽ�		ʵ�ʾ���
NumLog			������			short		2�ֽ�		ʵ��������
IZ				��λ���		short		2�ֽ�		0
LogName			������			char		256���		���㲹�ո�
StartDep		��ʼ���		float		4�ֽ�		ʵ����ʼ���
EndDep			�������		float	 	4�ֽ�		ʵ�ʽ������
Rlev			�������		float		4�ֽ�		ʵ�ʲ������
IDC								short		2�ֽ�		0
IZZ								short		2�ֽ�		0
Feet			��������		float		4�ֽ�		125��64
DumFull			�����			char		68�ֽ�		�ո�ֵ
End	
2.�ļ���С
  һ�����ݿ飽NumLog*4�ֽ�
  �ļ���С=512+4*NumLog*ÿ�����߲�������
3.�����ʽ
  ����ļ����룺΢����ʽ
*/

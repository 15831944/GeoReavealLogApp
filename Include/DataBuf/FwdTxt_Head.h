#if !defined(FWDTXT_HEAD_INCLUDE_)
#define FWDTXT_HEAD_INCLUDE_
//FwdTxt_Head.h:Forward_Text_Format1.0ͷ��Ϣ����
typedef struct
{
    char Mark[1024];  //�ļ���־
    float Stdep,Endep,Rlev; //���ֹ��������
    int   NumLog; //������
	int   NumPoint; //����
    char CurveName[512][16]; //������
}FWDTXT_HEAD;

#endif // !defined(FWDTXT_HEAD_INCLUDE_)

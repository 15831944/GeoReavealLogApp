#if !defined(FWDLAS_HEAD_INCLUDE_)
#define FWDLAS_HEAD_INCLUDE_
//FwdLas_Head.h:Forward_Text_Format1.0ͷ��Ϣ����
typedef struct
{
    char Mark[1024];  //�ļ���־
    float Stdep,Endep,Rlev; //���ֹ��������
    int   NumLog; //������
	int   NumPoint; //����
    char CurveName[512][16]; //������
}FWDLAS_HEAD;

#endif // !defined(FWDLAS_HEAD_INCLUDE_)

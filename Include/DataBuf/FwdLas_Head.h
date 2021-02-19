#if !defined(FWDLAS_HEAD_INCLUDE_)
#define FWDLAS_HEAD_INCLUDE_
//FwdLas_Head.h:Forward_Text_Format1.0头信息定义
typedef struct
{
    char Mark[1024];  //文件标志
    float Stdep,Endep,Rlev; //起深、止深、采样间隔
    int   NumLog; //曲线数
	int   NumPoint; //点数
    char CurveName[512][16]; //曲线名
}FWDLAS_HEAD;

#endif // !defined(FWDLAS_HEAD_INCLUDE_)

#if !defined(SLC716_HEAD_)
#define SLC716_HEAD_
//Slc716_Head.h:微机716数据头信息定义
const int MAXLOG=128;

typedef struct //测井数据文件的头信息结构
{
    char WellName[80],CompanyName[80];
    float Stdep,Endep,Rlev,Spcpr;
    float Ecc;
    short NumLog,Iz;
    char Reserverd[72];
    char CurveName[MAXLOG][4];
    char Reserverd2[256];
}SLC716_HEAD;

#endif // !defined(SLC716_HEAD_)

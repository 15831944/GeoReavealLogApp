#if !defined(_SKC92_HEADER_INCLUDED_)
#define _SKC92_HEADER_INCLUDED_
//---------------- XSKC92头格式  ------------------
typedef struct
{
	WORD VdlFlag; //0:2字节,VDL开关
	WORD CblFlag; //2:2字节,CBL开关
	char CompanyName[80]; //4:80字节bytes
	char WellName[80]; //84:80字节
	short NumLog; //164:2字节
	short Iz; //166:2字节   
	char CurveName[20][4]; //168:80字节
	float Stdep; //248:4字节
	float Endep; //252:4字节
	float Rlev; //256:4字节,对于CBL/VDL则为VDLMUL
	short Idc; //260:2字节
	short Iee; //262:2字节
	float Spcpr; //264:4字节,没意义
	short Year; //268:2字节
	char Day; //270:1字节
	char Month; //271:1字节
	char Minute; //272:1字节
	char Hour; //273:1字节
	char Reserved; //274:1字节
	char Second; //275:字节
} SKC92_HEAD; //共276字节
//数据块由若干数据记录组成，每一深度点的所有曲线的采样值是一个数据记录。
//数据记录的长度根据曲线性质的变化而变化。
//(1)常规曲线：一个采样点记录1个浮点数，占4字节，
//(2)CCL曲线：一个采样点记录4个浮点数，占4*sizeof(float)
//(3)VDL(486机):一个采样点记录960个无符号整数，占960*sizeof(unsigned int)
//      (386机):一个采样点记录960个无符号字符，占960*sizeof(unsigned char)
//(4)数据记录的长度=所有记录曲线记录字节数之和。

//(5)SKC92的变密度数据为2字节无符号整型数，且只有高12位有用，
//   由于12位整型的最大值为4096，为了得到具有正负半周的波形曲线，
//   应对该整型数右移4位，并减去4096/2=2048

//(6)VDL曲线记录机型的判别：可以根据文件大小及文件记录的深度综合判别，
//   一般常规曲线记录块大小不超过92字节(19*4+4*4,含一条CCL曲线)
//   当为386机型(变密度以单字节记录):记录大小为960-1036字节
//   当为386机型(变密度以双字节记录):记录大小为1920-1996字节
//   判别时应考虑文件实际记录大小与用深度计算结果有判别，多(或少)几
//   个记录,一般不会超过10个记录。[注：该项功能没做!]

#endif //(_SKC92_HEADER_INCLUDED_)

#if !defined(_SKH2000_HEADER_INCLUDED_)
#define _SKH2000_HEADER_INCLUDED_
//Skh2000_Head.h:SKH2000文件数据的头信息定义文件
typedef struct
{
	float Ecc; //0:4 成果号
	char CompanyName[40]; //4:80 公司名
	char TeamName[20]; //44:20 队名
	char Operator[20]; //64:20 操作员
	char WellName[40]; //84:40 井名
	char WellSite[40]; //124:40 井位
	short NumLog; //164:2 曲线条数
	short Iz; //166:2 未使用
	char CurveName[20][4]; //168:80 曲线名，每个名4字符
	float Stdep; //248:4 起始深度
	float Endep; //252:4 终止深度
	float Rlev; //256:4 采样间隔（米）
	short Idc; //260:2 未使用
	short Iee; //262:2 未使用
	float Spcpr; //264:4 每记录数据个数（固定128）
	BYTE N_SERV[6]; //268:6 测井项目号(FIRST BYTE)
	WORD IFILE; //274:2 未使用
	BYTE YEAR[2]; //276:2  年(BCD CODE)
	BYTE MONTH; //278:1 月(BCD CODE)
	BYTE DATE; //279:1 日(BCD CODE)
	BYTE HOUR; //280:1  时(BCD CODE)
	BYTE MINUTE; //281:1 分(BCD CODE)
	char TOOL_TYPE[5][8]; //282:40 仪器类型(8)
	char TOOL_SN[5][8]; //322:40 仪器编号(8)
	char szReserved[148]; //变量szReserved是后面参数的简化，
	//如果要使用下面参数，则去掉该变量，并在编译时应设置对齐为2字节或1字节，
	//设置方法：Project->Settings...->C/C++->Categoty:Code Generation->2 Bytes

	//BYTE EDT_TAG; //362:1 编辑标志：1=编辑
	//BYTE TAG_TVRC; //363:1 0=时间,1=变密度,2=RFT,3=坐标
	//float DRIL_DEPTH; //364:4 钻井深度
	//float CASE_DEPTH; //368:4 套管深度
	//char LIQ_TYPE[12]; //372:12 泥浆类型
	//float DCASE; //384:4 套管内径
	//float HPLAT; //388:4 补心高度
	//float LIQ_DEN; //392:4 泥浆比重
	//float LIQ_NIAN; //396:4 泥浆粘度
	//float LIQ_PH; //400:4 泥浆PH值
	//float LIQ_LOSS; //404:4 泥浆失水率
	//BYTE DEP_CORR; //408:1 深度校正方法
	//BYTE BCP_TAG; //409:1 0=差分压力,1=温度,2=图头编号
	//float CASE_SIZEO; //410:4 套管外径
	//char WITNESS[10]; //414:10 验收员名
	//char G_LEADER[10]; //424:10 队长名
	//BYTE RFT_TAG; //434:1 RFT测井标志
	//BYTE RFT_PRS; //435:1 RFT压力显示标志
	//float DDRV1; //436:4 钻头直径1
	//float DDRV2; //440:4 钻头直径2
	//float DDRV3; //444:4 钻头直径3
	//float DDRV4; //448:4 钻头直径4
	//float DEPDRV1; //452:4 钻头深度1
	//float DEPDRV2; //456:4 钻头深度2
	//float DEPDRV3; //460:4 钻头深度3
	//float DEPDRV4; //464:4 钻头深度4
	//float RAO; //468:4 泥浆电阻率
	//float DEPTH_MEA; //472:4 测时井深
	//float TEMPC; //476:4 地面温度
	//char CurveName2[5][4]; //480:20 CHAR 曲线名(5X4)
	//short DIFF_PRSS; //500:2 差分压力距离
	//short DIFF_TEMPC; //502:2 差分温度距离
	//float HIGH_CER; //504:4 预计水泥返高
	//float TIME_GJ; //508:4 固井时间
} SKH2000_HEAD;
#endif //!defined(SKH2000_HEAD_)

// DIP_Calculate.h : interface of the CDIP_Calculate class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _DIP_CALCULATE_H__
#define _DIP_CALCULATE_H__

#define NN 10  //FOURIER拟合,调和数最高维数定义
#define MM 100  //FOURIER拟合,输入数据组最高维数定义

#ifndef PI
#define PI 3.1415926257
#endif
class AFX_EXT_CLASS CDIPCalculate 
{
public: 
	CDIPCalculate();
	virtual ~CDIPCalculate();
public:	
	double	m_dep,m_dip,m_dir,m_sigma2;
	
	double	m_daz;
	double	m_dipa;

	double	m_dip1,m_dir1;
	double	m_dips,m_dirs;
	
	double x[MM],y[MM];	
	double a[2*NN][2*NN],b[2*NN];

	//总可用点数
	int vnn;
	//定义各点之间总的弧长
	double ll;
	//井斜角、井斜方位、1号极板方位、1号极板相对方位 单位:度
	double dev,daz,az1,az1r;
	//地层倾角、方位 单位:度
	double dip,dir;

	//中心化方法，用于子程序fourier0
	double w,ya,ys,x1a,x2a,x1s,x2s,x12,x1y,x2y,l1y,l2y,lyy; 
// Operations
public:
	void SLE(int n,double zero);
	void FOURIER0(int m,double ff);
	void BHCORR();
	void FOURIER(int m, int n, double ff);
// Implementation
public:
	
	//地层倾角、方位 单位:度
	//输入总点数vnn,各点之间总的弧长ll,第一点方位m_az0,各点x[i] y[i],
	//输出地层倾角m_dip、方位m_dir、极大似然估计值m_sigma2
	void Compute();

	//视倾角计算 单位:度
	//输入地层倾角m_dip、方位m_dir,剖面方位m_daz
	//输出地层视倾角m_dipa
	void ComputeDIPa();

	//背景校正(移去构造倾角) 单位:度
	//输入地层倾角m_dip、方位m_dir
	//输入背景倾角m_dip1、方位m_dir1
	//输出地层真倾角m_dips,m_dirs
	void ComputeDIPs(); 
};

#endif 
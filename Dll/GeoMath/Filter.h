#pragma once

class AFX_EXT_CLASS CFilter
{
public:
	CFilter(){};
	~CFilter(){};
protected:
	//单位冲击响应
	double m_h[32];
	double Computewin(int type,int n,int i,double beta);
	double kaiser(int i,int n,double beta);
	double bessel(double x);
public:
	//根据参数计算梯形窗H值
	//窗口类型 wintype=3;
	//滤波器类型;firtype=3带通
	void Init(int firtype,float fln,float fhn,int wintype);
	void Filter(float x[],int Num,float y[]);
};

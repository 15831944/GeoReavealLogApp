#pragma once

class AFX_EXT_CLASS CFilter
{
public:
	CFilter(){};
	~CFilter(){};
protected:
	//��λ�����Ӧ
	double m_h[32];
	double Computewin(int type,int n,int i,double beta);
	double kaiser(int i,int n,double beta);
	double bessel(double x);
public:
	//���ݲ����������δ�Hֵ
	//�������� wintype=3;
	//�˲�������;firtype=3��ͨ
	void Init(int firtype,float fln,float fhn,int wintype);
	void Filter(float x[],int Num,float y[]);
};

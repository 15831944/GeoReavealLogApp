// DIP_Calculate.h : interface of the CDIP_Calculate class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _DIP_CALCULATE_H__
#define _DIP_CALCULATE_H__

#define NN 10  //FOURIER���,���������ά������
#define MM 100  //FOURIER���,�������������ά������

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

	//�ܿ��õ���
	int vnn;
	//�������֮���ܵĻ���
	double ll;
	//��б�ǡ���б��λ��1�ż��巽λ��1�ż�����Է�λ ��λ:��
	double dev,daz,az1,az1r;
	//�ز���ǡ���λ ��λ:��
	double dip,dir;

	//���Ļ������������ӳ���fourier0
	double w,ya,ys,x1a,x2a,x1s,x2s,x12,x1y,x2y,l1y,l2y,lyy; 
// Operations
public:
	void SLE(int n,double zero);
	void FOURIER0(int m,double ff);
	void BHCORR();
	void FOURIER(int m, int n, double ff);
// Implementation
public:
	
	//�ز���ǡ���λ ��λ:��
	//�����ܵ���vnn,����֮���ܵĻ���ll,��һ�㷽λm_az0,����x[i] y[i],
	//����ز����m_dip����λm_dir��������Ȼ����ֵm_sigma2
	void Compute();

	//����Ǽ��� ��λ:��
	//����ز����m_dip����λm_dir,���淽λm_daz
	//����ز������m_dipa
	void ComputeDIPa();

	//����У��(��ȥ�������) ��λ:��
	//����ز����m_dip����λm_dir
	//���뱳�����m_dip1����λm_dir1
	//����ز������m_dips,m_dirs
	void ComputeDIPs(); 
};

#endif 
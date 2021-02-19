// DIP_Calculate.cpp : implementation of the CDIPCalculate class
//

#include "stdafx.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>

#include "DIPCalculate.h"

/////////////////////////////////////////////////////////////////////////////
// CDIPCalculate construction/destruction

CDIPCalculate::CDIPCalculate()
{

}

CDIPCalculate::~CDIPCalculate()
{
}

//////////////////////////////////////////////////////////////////////////////
//  �ز���ǡ���λ ��λ:��
//  �����ܵ���vnn,����֮���ܵĻ���ll,��һ�㷽λm_az0,����x[i] y[i],
//  ����ز����m_dip����λm_dir��������Ȼ����ֵm_sigma2
//
void CDIPCalculate::Compute() 
{
	int i,j;
	double y1,cali;
	//��ʼ������
	for(i=0;i<2*NN;i++)
	{
		for(j=0;j<2*NN;j++)
			a[i][j]=0.;
		b[i]=0.;
	}
	if(vnn<3) //�������ݵ㲻��3�������½����б�
	{
		m_dir=-999.25;m_dip=-999.25;
		return;
	}
	
	FOURIER0(vnn,1./ll); //���Ļ�������С���˷�������淽�̲����
	//����ͳ�����Ĺ۲�ֵ�Լ��в�ֲ��ļ�����Ȼ����ֵ
	double qe=lyy-(b[0]*l1y+b[1]*l2y);
	double sigma2=qe/double(vnn);
	//�õ�����ϵ��
	double belt=ya-(b[0]*x1a+b[1]*x2a);
	b[2]=b[1];
	b[1]=b[0];
	b[0]=belt;

	int i_min,i_max;
	double ymin,ymax;
	ymin=1.0E32;ymax=-1.0E32;
	for(i=0;i<ll;i++)
	{
		y1=b[0]+b[1]*cos(2.*PI/ll*double(i))+b[2]*sin(2.*PI/ll*double(i));
		if(y1<ymin)
		{
			ymin=y1;
			i_min=i;
		}
		if(y1>ymax)
		{
			ymax=y1;
			i_max=i;
		}
	}
	dir=2.0*PI*(double)(i_max)/ll;
	m_dep=(ymax+ymin)/2.;
	//����,δ���Ǿ�����Բ�����
	cali=ll/PI;
	dip=atan(fabs(ymax-ymin)/cali);

	m_dip=fabs(dip)*180./PI;
	m_dir=dir*180./PI;
	m_sigma2=sigma2;
	while(m_dir>360.) 
		m_dir-=360.;
	while(m_dir<0.) 
		m_dir+=360.;
}

//����Ԫ��ȥ�������Է������ӳ���,�跽��Ϊax=b,���ս�����Ϊb,
//N1Ϊ�����������ã�Ϊ���ֵ��nΪʵ��ά��(���̸���)��N1>=n;zeroΪ��С���
void CDIPCalculate::SLE(int n, double zero)
{
	long i,is,k,j;
	double div,c,d,ratio;
	for(i=0;i<n;i++)
	{
		is=i;
		div=a[i][i];
		for(k=i;k<n;k++)
		{
			if(a[k][i]*a[k][i]>div*div)
			{
				is=k;
				div=a[is][i];
			}
		}
		if(is>i)
		{
			for(j=0;j<n;j++)
			{
				c=a[is][j];
				a[is][j]=a[i][j];
				a[i][j]=c;
			}
			d=b[is];
			b[is]=b[i];
			b[i]=d;
		}
		if(div*div>zero*zero)
		{
			for(j=0;j<n;j++) a[i][j]=a[i][j]/div;
			b[i]=b[i]/div;
			for(j=0;j<n;j++)
			{
				if(i!=j)
				{
					ratio=a[j][i];
					for(k=0;k<n;k++) a[j][k]=a[j][k]-ratio*a[i][k];
					b[j]=b[j]-ratio*b[i];
				}
			}
		}
	}
}

//���и���Ҷ�ع飬x[],y[]Ϊ����������b[]Ϊ������
//mΪʵ��ʵ�����ݵ���������ֵ���������ж���ΪMM
//nΪ������,���ֵ���������ж���ΪNN
//ffΪ����Ƶ��,=w/2PI
void CDIPCalculate::FOURIER(int m, int n, double ff)
{
	int i,j,k;
	double w;
	w=2.*PI*ff;
	a[0][0]=m;
	for(i=0;i<m;i++)
		b[0]=y[i]+b[0];
	for(i=1;i<2*n;i=i+2)
		for(j=0;j<m;j++)
		{
			a[i][0]=cos(int((i+1)/2)*w*x[j])+a[i][0];
			b[i]=y[j]*cos(int((i+1)/2)*w*x[j])+b[i];
		}
	for(i=2;i<2*n+1;i=i+2)
		for(j=0;j<m;j++)
		{
			a[i][0]=sin(int(i/2)*w*x[j])+a[i][0];
			b[i]=y[j]*sin(int(i/2)*w*x[j])+b[i];
		}
	for(i=1;i<2*n;i=i+2)
		for(j=0;j<m;j++)
			a[0][i]=cos(int((i+1)/2)*w*x[j])+a[0][i];
	
	for(i=2;i<2*n+1;i=i+2)
		for(j=0;j<m;j++)
			a[0][i]=sin(int(i/2)*w*x[j])+a[0][i];
	for(i=1;i<2*n;i=i+2)
		for(j=1;j<2*n;j=j+2)
			for(k=0;k<m;k++)
				a[i][j]=cos(int((i+1)/2)*w*x[k])*cos(int((j+1)/2)*w*x[k])+a[i][j];
	for(i=2;i<2*n+1;i=i+2)
		for(j=2;j<2*n+1;j=j+2)
			for(k=0;k<m;k++)
				a[i][j]=sin(int(i/2)*w*x[k])*sin(int(j/2)*w*x[k])+a[i][j];
	SLE(2*n+1,1.2E-64);
}

void CDIPCalculate::BHCORR()
{
	dip=dip*PI/180.;
	dir=dir*PI/180.;
	dev=dev*PI/180.;
	daz=daz*PI/180.;
	az1=az1*PI/180.;
	az1r=az1r*PI/180.;
	double dir2,dir3,dir4,dir5,dip2,dip3,dip4,dip5;
	//����I�ż�����Է�λ
	if(az1r<0.)
		az1r=az1-daz;
	//��2��
	dir2=dir+az1r;
	dip2=dip;
	//��3��
	if(dir2==PI*0.5||dir2==PI*1.5)
	{
		dir3=dir2;
	    if(dip2<0.) dip3=PI*0.5+dip2;
		if(dip2>0.) dip3=PI*0.5-dip2;
	}
	else
	{
		dir3=atan(1./(tan(dip2)*cos(dir2)));
		dip3=atan(1./(tan(dir2)*cos(dir3)));
	}
	//��4��
	dir4=dir3+dev;
	dip4=dip3;
	//��5��
	if(dir4==PI*0.5||dir4==PI*1.5)
	{
		dir5=dir4;
		if(dip4<0.) dip5=PI*0.5+dip4;
		if(dip4>0.) dip5=PI*0.5-dip4;
	}
	else
	{
		dir5=atan(1./(tan(dip4)*cos(dir4)));
		dip5=atan(1./(tan(dir4)*cos(dir5)));
	}
	
	//��6��	
	dir=dir5+daz;//+az1-atan(sin(az1r)/(cos(dev)*cos(az1r)));//
	dip=dip5;	
	if(dip<0) 	
	{
		dip*=-1;
		dir+=PI;  //������������˷�λ��Ҫ��180��
	}
	/*
	double dev1=fabs(atan(tan(dev)*cos(daz-dir)));
	dip+=dev1;*/
	dip=dip*180./PI;
	dir=dir*180./PI;
	while(dir<0.)
		dir+=360.;
	while(dir>360.)
		dir-=360.;
}

//���и���Ҷ�ع飬x[],y[]Ϊ����������b[]Ϊ������
//mΪʵ��ʵ�����ݵ���������ֵ���������ж���ΪMM
//ffΪ����Ƶ��,=w/2PI
void CDIPCalculate::FOURIER0(int m, double ff)
{
	long i;
	ya=0.;
	ys=0.;
	x1a=0.;
	x2a=0.;
	x1s=0.;
	x2s=0.;
	x12=0.;
	x1y=0.;
	x2y=0.;
	w=2.*PI*ff;
	a[0][0]=m;
	for(i=0;i<m;i++)
	{
		ya=ya+y[i];
		ys=ys+y[i]*y[i];	
		x1a=x1a+cos(w*x[i]);
		x2a=x2a+sin(w*x[i]);
		x1s=x1s+cos(w*x[i])*cos(w*x[i]);
		x2s=x2s+sin(w*x[i])*sin(w*x[i]);
		x12=x12+cos(w*x[i])*sin(w*x[i]);
		x1y=x1y+cos(w*x[i])*y[i];
		x2y=x2y+sin(w*x[i])*y[i];
	}
	ya=ya/m;
	x1a=x1a/m;
	x2a=x2a/m;
	a[0][0]=x1s-m*x1a*x1a;
	a[1][1]=x2s-m*x2a*x2a;
	a[0][1]=x12-m*x1a*x2a;
	a[1][0]=a[0][1];
	b[0]=x1y-m*x1a*ya;
	b[1]=x2y-m*x2a*ya;
	l1y=x1y-m*x1a*ya;//��3�����ڼ�����Ȼ����
	l2y=x2y-m*x2a*ya;
	lyy=ys-m*ya*ya;
	SLE(2,1.2E-164);
}

///////////////////////////////////////////////////////////////////////////////
//  ����Ǽ��� ��λ:��
//  ����ز����m_dip����λm_dir,��б��λm_daz
//  ����ز������m_dipa
//
void CDIPCalculate::ComputeDIPa() 
{
	m_dip=m_dip*PI/180.;
	m_dir=m_dir*PI/180.;
	m_daz=m_daz*PI/180.;
	m_dipa=atan(tan(m_dip)*cos(m_daz-m_dir));
	m_dipa=fabs(m_dipa*180./PI);
}

///////////////////////////////////////////////////////////////////////////////
//  ����У��(��ȥ�������) ��λ:��
//  ����ز����m_dip����λm_dir
//  ���뱳�����m_dip1����λm_dir1
//  ����ز������m_dips,m_dirs
//
void CDIPCalculate::ComputeDIPs() 
{
	double dip,dir,dir_t;
	m_dip=m_dip*PI/180.;
	m_dir=m_dir*PI/180.;
	m_dip1=m_dip1*PI/180.;
	m_dir1=m_dir1*PI/180.;

	//��2��
	dir=m_dir-m_dir1;
	dip=m_dip;
	//��3��
	if(dir==PI*0.5 || dir==PI*1.5)
	{
		if(dip<=0.) dip=PI/2.+dip;
		if(dip>0.) dip=PI/2.-dip;
	}
	else
	{
		dir_t=dir;
		dir=atan(1./(tan(dip)*cos(dir_t)));
		dip=atan(1./(tan(dir_t)*cos(dir)));
	}
	//��4��
	dir=dir+m_dip1;
	//��5��
	if(dir==PI*0.5||dir==PI*1.5)
	{
		if(dip<=0.) dip=PI/2.+dip;
		if(dip>0.) dip=PI/2.-dip;
	}
	else
	{
		dir_t=dir;
		dir=atan(1./(tan(dip)*cos(dir_t)));
		dip=atan(1./(tan(dir_t)*cos(dir)));
	}
	//��6��
	dir=dir+m_dir1;
	if(dip<0)
	{
		dip*=-1.;
		dir+=PI;  //������������˷�λ��Ҫ��180��
	}
	if(dir>2.*PI) dir=dir-2.*PI;
	if(dir<0.) dir=dir+2.*PI;
	m_dips=dip*180/PI;
	m_dirs=dir*180/PI;
	while(m_dirs>360.) 
		m_dirs-=360.;
	while(m_dirs<0.) 
		m_dirs+=360.;
}

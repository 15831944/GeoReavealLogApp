// Integrate.cpp: implementation of the CIntegrate class.
//
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//
//                   ����ֵ���
//
/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////                    
//
//
//              ���ϳ���ѧԺ  ��Ϣ������ѧϵ  
//                        �ƽ���
//                 jiangfenghuang@msn.com
//                 jiangfenghuang@163.net
//                       2003.4.10          
//
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//
// �ص㣺
//    1. ֧�ָ��ָ��ӵĻ��ֱ��ʽ���ɺ��κ���ѧ������
//       ��  3*exp(x)*tan(x)/(8+log(x)) 
//    2. �ɼ������ʽ����д���� 
//    3. �����û��־���
//    4. ʹ�÷����
//
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//
//                    ʹ�÷���
// 1. �����CIntegrate ���󣬹���ʱ��ʼ������
//    initial(char *s,double Upperlimit,double LowerLimit)]
//    ������ʼ��
// 2.����SetError(double Error)�������
// 3.����integrate(void)���л��֣�������ֵΪ1�������
//   getvalue(void)ȡ�û��ֽ����������ֽ���д�Ӧ����
//
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//
//                ֧�ֵĺ����ı�ż�����
//
// ���     ����         ����         ���õ�math.h�⺯��     ��ע
//
//   1      abs(x)      ȡ����ֵ|x|    fabs(x)      
//   2      acos(x)     �����Һ���     acos(x)            -1<=x<=1
//   3      asin(x)     �����Һ���     asin(x)            -1<=x<=1
//   4      atan(x)     �����к���     atan(x)
//   5      acot(x)     �����к���     PI/2-atan(x)
//   6      cos(x)      ���Һ���       cos(x)
//   7      cosh(x)     ˫�����Һ���   cosh(x)
//   8      cot(x)      ���к���       1/tan(x)            tan(x)!=0
//   9      exp(x)      e��x�η�       exp(x)
//  10      floor(x)    ��x��          floor(x)   �󲻴���x��������� 
//  11      mod(x,y)     x%y           mod(x,y)             y!=0
//  12      ln(x)       ȡ��Ȼ����     log(x)               x>0
//  13      log(x)      ȡ10�Ķ���     log10(x)             x>0
//  14      pow(x,y)    x��y�η�       pow(x,y)
//  15      sin(x)      ���Һ���       sin(x)
//  16      sinh(x)     ˫�����Һ���   sinh(x)
//  17      sqrt(x)     ��x����        sqrt(x)            x>=0
//  18      tan(x)      ���к���       tan(x)
//  19      tanh(x)     ˫�����к���   tanh(x)
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Integrate.h"
#include "string.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIntegrate::CIntegrate() //���캯��,�趨����ʼֵΪ0.0001
{
	Error=0.0001;       //����ʼֵΪ0.0001

}

CIntegrate::~CIntegrate()
{

}

//���캯�������ַ���str�������
//��ʽ���ַ���mathStr
//�趨����ʼֵΪ0.0001
CIntegrate::CIntegrate(char *str)        
{
	Error=0.0001;     //�趨����ʼֵΪ0.0001
    strcpy(mathStr,str);
}

//���캯�������ַ���str�������������
//��ʽ���ַ���mathStr,uplimit��lowlimit
//�ֱ𸳸�����UpperLimit������LowerLimit
//�趨����ʼֵΪ0.0001
CIntegrate::CIntegrate(char *str,double uplimit,double lowlimit)
{
	Error=0.0001;
	strcpy(mathStr,str);
	UpperLimit=uplimit;
	LowerLimit=lowlimit;
}

//��ʼ�����������ַ���str�������������
//��ʽ���ַ���mathStr,uplimit��lowlimit
//�ֱ𸳸�����UpperLimit������LowerLimit
int CIntegrate::initial(char *str,double uplimit,double lowlimit) 
{
	strcpy(mathStr,str);
	UpperLimit=uplimit;
	LowerLimit=lowlimit;
	return 1;
}

void  CIntegrate::setError(double error)    //�趨���
{
	Error=error;
}

//���ֺ�����
//�����ʽ�Ƿ���ȷ
//������������ֺ���RombergIntgrt()
//�����ʽ�ͻ�����ȷ������1�����򷵻�0                           
int CIntegrate::integrate(void)  
{
	IntgrtStr.Initial(mathStr);   
	                   //��ʼ������IntgrtStr
	                   //���������ʽmathStr����string
	if(!IntgrtStr.checkString())
		return 0;    //�����ʽ�Ƿ���ȷ����ȷ����������򷵻�0

    if(RombergIntgrt())         // ������ȷ,����1
       return 1;
	else return 0;              //���򷵻�0

}

//��������ֺ���
//���ݻ������ޡ����ޡ��������ʽ��Ҫ����������������㷨
//����֡����������ȷ�����������result��������1�����򷵻�0
int CIntegrate::RombergIntgrt()
{
	double h,         //����
		t1,t2,sum,    //���ι�ʽ���м�ֵ
		s1,s2=0.0,        //Sinpson��ʽ���м�ֵ
		c1,c2=0.0,        //Cotes��ʽ���м�ֵ
		r1,r2=0.0,        //Romberg��ʽ�м�ֵ
		k,x,          //�ӹ�����������x
		a,b;          //���ޡ�����
	
	flag=1;           //��ʶ���ֹ����е�f(X)Y�������޴�
	                  //�ĳ�ʼֵΪ1
	a=LowerLimit;
	b=UpperLimit;
	h=b-a;
	t1=h*(f(a)+f(b))/2;
	k=1;
	while(1)       //�������ڸ������
	{
		//���ι�ʽ
		sum=0;
		x=a+h/2;
		while(x<b)
		{
			sum=sum+f(x);
			x=x+h;
		}
		t2=t1/2+h*sum/2;

		//Simpson��ʽ
		s2=t2+(t2-t1)/3;
		if(k==1)
		{
			k++;
			h=h/2;
			t1=t2;
			s1=s2;
			continue;
		}
		 
		//Cotes��ʽ
		c2=s2+(s2-s1)/15;
		if(k==2)
		{
			c1=c2;
			k++;
			h=h/2;
			t1=t2;
			s1=s2;
			continue;
		}

		//Romberg��ʽ
		r2=c2+(c2-c1)/63;
		if(k==3)
		{
			r1=r2;
			c1=c2;
			k++;
			h=h/2;
			t1=t2;
			s1=s2;
			continue;
		}
		if(fabs(r2-r1)<Error || !flag)    //�����������f��x������
			break;                        //���˳�
		else                     //����ı�����������
		{
			r1=r2;
			c1=c2;
			k++;
			h=h/2;
			t1=t2;
			s1=s2;
			continue;
		}
	
	}//while(1)
	                                    //�������޴��������
	if(flag)      //���ֹ������޴����򽫽������result������1
	{
		result=r2;
        return 1;
	}
	else        //���򷵻�0
		return 0;

}

//���ʽ����ֵ����
//����ֵΪ��x������ʽ��ֵ
//�������г��ִ����򽫱�ʶ��flag��0
double CIntegrate::f(double x)
{
	IntgrtStr.setX(x);   //������x����IntgrtStr�е�x

	IntgrtStr.stringToBolan();  //�����ʽת���ɲ���ʽ
	if(!IntgrtStr.compvalue())  //compvalue()����ֵΪ0ʱ��ʾ
		                        //������ȷ
		return(IntgrtStr.getvalue());
	else
	{
		flag=0;
		return 0;
	}


}

double CIntegrate::getvalue(void)   //���ػ��ֽ��result��ֵ
{
	return result;
}
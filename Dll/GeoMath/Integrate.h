// Integrate.h: interface for the CIntegrate class.
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
#if !defined(AFX_INTEGRATE_H__92364C9D_873B_4B2C_9BFB_2556E787BA89__INCLUDED_)
#define AFX_INTEGRATE_H__92364C9D_873B_4B2C_9BFB_2556E787BA89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define  MAX  100         //�����������ʽ�Ϊ100
#include "MathString.h"

class AFX_EXT_CLASS CIntegrate  
{
public:
    CMathString IntgrtStr;     //���ַ�����ȫ�ֶ���
	char mathStr[MAX];        //����������ʽ���ַ���
	double UpperLimit,        //��������
		LowerLimit,           //��������
		Error,                //���, ��ʼֵΪ0.0001
		result;               //���ֽ��
	int flag;                 //��ʶ���ֹ����е�f(X)�������޴�
	                          //������0����ʼֵΪ1

 
	CIntegrate();            //���캯��,�趨����ʼֵΪ0.0001
	CIntegrate(char str[MAX]);   //���캯�������ַ���str�������������
	                         //��ʽ���ַ���MathStr
                           	 //�趨����ʼֵΪ0.0001
	CIntegrate(char str[MAX],double uplimit,double lowlimit);
	                         //���캯�������ַ���str�������������
	                         //��ʽ���ַ���mathStr,uplimit��lowlimit
	                         //�ֱ𸳸�����UpperLimit������LowerLimit
	                         //�趨����ʼֵΪ0.0001
	int initial(char str[MAX],double uplimit,double lowlimit);
                             //��ʼ�����������ַ���str�������������
	                         //��ʽ���ַ���mathStr,uplimit��lowlimit
	                         //�ֱ𸳸�����UpperLimit������LowerLimit
    void setError(double error);    //�趨���
 
	int integrate(void);     //���ֺ�����
	                         //������������ֺ���RombergIntgrt()
	                         //��������ȷ������1�����򷵻�0


	double getvalue(void);   //���ػ��ֽ��result��ֵ

    int RombergIntgrt(void);     
	              //��������ֺ���
	              //���ݻ������ޡ����ޡ��������ʽ��Ҫ�����С
	              //�����������㷨�����
	              //���������ȷ�����������result��������1��
	              //���򷵻�0

	double f(double x);      //���ʽ����ֵ����
	                         //����ֵΪ��x������ʽ��ֵ
	                         //�������г��ִ����򽫱�ʶ��
	                         //flag��0

	virtual ~CIntegrate();

};

#endif // !defined(AFX_INTEGRATE_H__92364C9D_873B_4B2C_9BFB_2556E787BA89__INCLUDED_)

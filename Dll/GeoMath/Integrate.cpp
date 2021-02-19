// Integrate.cpp: implementation of the CIntegrate class.
//
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//
//                   求积分的类
//
/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////                    
//
//
//              湖南城市学院  信息与计算科学系  
//                        黄江峰
//                 jiangfenghuang@msn.com
//                 jiangfenghuang@163.net
//                       2003.4.10          
//
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//
// 特点：
//    1. 支持各种复杂的积分表达式，可含任何数学函数，
//       如  3*exp(x)*tan(x)/(8+log(x)) 
//    2. 可检查出表达式的书写错误 
//    3. 可设置积分精度
//    4. 使用方便简单
//
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//
//                    使用方法
// 1. 定义个CIntegrate 对象，构造时初始化或用
//    initial(char *s,double Upperlimit,double LowerLimit)]
//    函数初始化
// 2.调用SetError(double Error)设置误差
// 3.调用integrate(void)进行积分，若返回值为1，则调用
//   getvalue(void)取得积分结果，否则积分结果有错，应舍弃
//
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//
//                支持的函数的编号及功能
//
// 编号     函数         功能         调用的math.h库函数     备注
//
//   1      abs(x)      取绝对值|x|    fabs(x)      
//   2      acos(x)     反余弦函数     acos(x)            -1<=x<=1
//   3      asin(x)     反正弦函数     asin(x)            -1<=x<=1
//   4      atan(x)     反正切函数     atan(x)
//   5      acot(x)     反余切函数     PI/2-atan(x)
//   6      cos(x)      余弦函数       cos(x)
//   7      cosh(x)     双曲余弦函数   cosh(x)
//   8      cot(x)      余切函数       1/tan(x)            tan(x)!=0
//   9      exp(x)      e的x次方       exp(x)
//  10      floor(x)    ⊥x⊥          floor(x)   求不大于x的最大整数 
//  11      mod(x,y)     x%y           mod(x,y)             y!=0
//  12      ln(x)       取自然对数     log(x)               x>0
//  13      log(x)      取10的对数     log10(x)             x>0
//  14      pow(x,y)    x的y次方       pow(x,y)
//  15      sin(x)      正弦函数       sin(x)
//  16      sinh(x)     双曲正弦函数   sinh(x)
//  17      sqrt(x)     对x开方        sqrt(x)            x>=0
//  18      tan(x)      正切函数       tan(x)
//  19      tanh(x)     双曲正切函数   tanh(x)
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

CIntegrate::CIntegrate() //构造函数,设定误差初始值为0.0001
{
	Error=0.0001;       //误差初始值为0.0001

}

CIntegrate::~CIntegrate()
{

}

//构造函数，将字符串str赋给存放
//达式的字符串mathStr
//设定误差初始值为0.0001
CIntegrate::CIntegrate(char *str)        
{
	Error=0.0001;     //设定误差初始值为0.0001
    strcpy(mathStr,str);
}

//构造函数，将字符串str赋给存放算术表
//达式的字符串mathStr,uplimit和lowlimit
//分别赋给上限UpperLimit和下限LowerLimit
//设定误差初始值为0.0001
CIntegrate::CIntegrate(char *str,double uplimit,double lowlimit)
{
	Error=0.0001;
	strcpy(mathStr,str);
	UpperLimit=uplimit;
	LowerLimit=lowlimit;
}

//初始化函数，将字符串str赋给存放算术表
//达式的字符串mathStr,uplimit和lowlimit
//分别赋给上限UpperLimit和下限LowerLimit
int CIntegrate::initial(char *str,double uplimit,double lowlimit) 
{
	strcpy(mathStr,str);
	UpperLimit=uplimit;
	LowerLimit=lowlimit;
	return 1;
}

void  CIntegrate::setError(double error)    //设定误差
{
	Error=error;
}

//积分函数，
//检查表达式是否正确
//调用龙贝格积分函数RombergIntgrt()
//若表达式和积分正确，返回1，否则返回0                           
int CIntegrate::integrate(void)  
{
	IntgrtStr.Initial(mathStr);   
	                   //初始化对象IntgrtStr
	                   //将算术表达式mathStr赋给string
	if(!IntgrtStr.checkString())
		return 0;    //检查表达式是否正确，正确则继续，否则返回0

    if(RombergIntgrt())         // 积分正确,返回1
       return 1;
	else return 0;              //否则返回0

}

//龙贝格积分函数
//根据积分上限、下限、函数表达式和要求的最误差，用龙贝格算法
//求积分。如果积分正确，将结果赋给result。并返回1，否则返回0
int CIntegrate::RombergIntgrt()
{
	double h,         //步长
		t1,t2,sum,    //梯形公式的中间值
		s1,s2=0.0,        //Sinpson公式的中间值
		c1,c2=0.0,        //Cotes公式的中间值
		r1,r2=0.0,        //Romberg公式中间值
		k,x,          //加工次数、变量x
		a,b;          //上限、下限
	
	flag=1;           //标识积分过程中的f(X)Y运算有无错
	                  //的初始值为1
	a=LowerLimit;
	b=UpperLimit;
	h=b-a;
	t1=h*(f(a)+f(b))/2;
	k=1;
	while(1)       //当误差大于给定误差
	{
		//梯形公式
		sum=0;
		x=a+h/2;
		while(x<b)
		{
			sum=sum+f(x);
			x=x+h;
		}
		t2=t1/2+h*sum/2;

		//Simpson公式
		s2=t2+(t2-t1)/3;
		if(k==1)
		{
			k++;
			h=h/2;
			t1=t2;
			s1=s2;
			continue;
		}
		 
		//Cotes公式
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

		//Romberg公式
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
		if(fabs(r2-r1)<Error || !flag)    //如果满足误差或f（x）出错
			break;                        //则退出
		else                     //否则改变条件，继续
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
	                                    //运算中无错误，则继续
	if(flag)      //积分过程中无错误，则将结果赋给result，返回1
	{
		result=r2;
        return 1;
	}
	else        //否则返回0
		return 0;

}

//表达式的求值函数
//返回值为将x代入表达式的值
//若计算中出现错误，则将标识符flag赋0
double CIntegrate::f(double x)
{
	IntgrtStr.setX(x);   //将变量x赋给IntgrtStr中的x

	IntgrtStr.stringToBolan();  //将表达式转换成波兰式
	if(!IntgrtStr.compvalue())  //compvalue()返回值为0时表示
		                        //计算正确
		return(IntgrtStr.getvalue());
	else
	{
		flag=0;
		return 0;
	}


}

double CIntegrate::getvalue(void)   //返回积分结果result的值
{
	return result;
}
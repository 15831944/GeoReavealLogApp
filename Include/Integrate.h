// Integrate.h: interface for the CIntegrate class.
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
#if !defined(AFX_INTEGRATE_H__92364C9D_873B_4B2C_9BFB_2556E787BA89__INCLUDED_)
#define AFX_INTEGRATE_H__92364C9D_873B_4B2C_9BFB_2556E787BA89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define  MAX  100         //定义算术表达式最长为100
#include "MathString.h"

class AFX_EXT_CLASS CIntegrate  
{
public:
    CMathString IntgrtStr;     //求字符串的全局对象
	char mathStr[MAX];        //存放算术表达式的字符串
	double UpperLimit,        //积分上限
		LowerLimit,           //积分下限
		Error,                //误差, 初始值为0.0001
		result;               //积分结果
	int flag;                 //标识积分过程中的f(X)运算有无错
	                          //若有则赋0。初始值为1

 
	CIntegrate();            //构造函数,设定误差初始值为0.0001
	CIntegrate(char str[MAX]);   //构造函数，将字符串str赋给存放算术表
	                         //达式的字符串MathStr
                           	 //设定误差初始值为0.0001
	CIntegrate(char str[MAX],double uplimit,double lowlimit);
	                         //构造函数，将字符串str赋给存放算术表
	                         //达式的字符串mathStr,uplimit和lowlimit
	                         //分别赋给上限UpperLimit和下限LowerLimit
	                         //设定误差初始值为0.0001
	int initial(char str[MAX],double uplimit,double lowlimit);
                             //初始化函数，将字符串str赋给存放算术表
	                         //达式的字符串mathStr,uplimit和lowlimit
	                         //分别赋给上限UpperLimit和下限LowerLimit
    void setError(double error);    //设定误差
 
	int integrate(void);     //积分函数，
	                         //调用龙贝格积分函数RombergIntgrt()
	                         //若积分正确，返回1，否则返回0


	double getvalue(void);   //返回积分结果result的值

    int RombergIntgrt(void);     
	              //龙贝格积分函数
	              //根据积分上限、下限、函数表达式和要求的最小
	              //误差，用龙贝格算法求积分
	              //如果积分正确，将结果赋给result。并返回1，
	              //否则返回0

	double f(double x);      //表达式的求值函数
	                         //返回值为将x代入表达式的值
	                         //若计算中出现错误，则将标识符
	                         //flag赋0

	virtual ~CIntegrate();

};

#endif // !defined(AFX_INTEGRATE_H__92364C9D_873B_4B2C_9BFB_2556E787BA89__INCLUDED_)

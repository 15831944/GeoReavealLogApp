// MathString.h: interface for the CMathString class.
//
// MathString.h: interface for the CMathString class.

//////////////////////////////////////////////////////////////////////                    
//
//                   求算术表达式值的类
//
//              湖南城市学院  信息与计算科学系  
//                        黄江峰
//                 jiangfenghuang@msn.com
//                 jiangfenghuang@163.net
//                       2003.4.5           
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//
// 功能：
//      求在字符串中的算术表达式的值，
//   
// 特点：
//      1.支持字符串中含有各种常用函数，如"7.5+sin(6*ln(8))/exp(5)"
//      2.具有很好的纠错能力，能检查出表达式中括号是否配对、库函数是
//        否正确
//      3.运算过程中能检查出各种异常，如除数为0、开方函数sqrt(x)中x<0
//        反余弦函数acos(x)中的x<-1或x>1等
//      4.为支持积分、求方程，算术表达式中可含有x，计算时将用类中的成
//        员变量xx代替 
//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////// 
//
// 使用方法：
//      1. 定义一个CMathString 的对象，用要求的算术表达式将其初始化
//         如： char *s1="7.5+sin(6)/exp(5)";
//              char *s2="4*pow(x,2)+2*x+4";
//              CMathString  mathstr1(s1);
//              CMathString  mathstr2(s2);
//      2.调用checkString方法检查表达式是否合法
//      3.如果是求积分或j解方程，调用setX(double x)设置表达式中x的值
//      4.调用stringToBolan()将算术表达式转换为波兰表达式
//      5.调用compvalue()对波兰表达式进行计算，若返回值为0，则调
//        用getvalue()取得计算结果，否则可根据返回值判断出错地方
//
/////////////////////////////////////////////////////////////////////
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
#if !defined(AFX_MATHSTRING_H__669187A1_784A_421F_B9EA_EAED6FA6D9AF__INCLUDED_)
#define AFX_MATHSTRING_H__669187A1_784A_421F_B9EA_EAED6FA6D9AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX    100    //表达式最长为100个字符

class AFX_EXT_CLASS CMathString  
{
public:
void Initial(char *str,double x);
	void Initial(char *s);
//成员变量	
	char string[MAX];     //存放算术表达式，长度为MAX=100
	double xx,             //存放积分或函数中的x
		result;            //算术表达式的值
    
	struct Bolan          //存放波兰表达式的结构
	{
		int     flag;     //标识该节点是数值、运算符还是函数
		                  //0为数值，1为运算符，2为函数
		double  value;    //数值值
		char    oper;     //运算符 +,-,*,/,~(取反),(,)
		int     func;     //函数的编号，各函数编号及功能见开始部分
		
	};
	Bolan bolan[MAX];     //存放转化后的波兰表达式，长度为MAX=100
    
	CMathString();
	CMathString(char *s);  //构造函数，将算术表达式*s赋给string
	CMathString(char *s,double x);   //表达式中带未知数的构造函数
	                                  //算术表达式*s赋给string，未知数x赋给xx
	                                  //用于积分表达式
//成员函数
	void setX(double x);     //设置成员变量xx的值
	int checkString(void);  //检查输入的表达式是否正确，包括括号是否
	                        //配对、库函数是否正确，不检查小数点、逗号
	                        //是否正确
	                        //若正确，将表达式中的大、中括号该成小括号
	                        //并在表达式尾加‘#’标识
	                        //若正确，返回值为1；否则返回0；
	                        //因积分表达式中有x，所以表达式中的x作为
	                        //字符处理
	
	int stringToBolan(void); //将算术表达式string转化成波兰表达式，
	                        //波兰表达式放在bolan[]中
	int compvalue(void);  //计算波兰表达式bolan[]的值
	                      //若计算中途没有发生错误，则返回0，并将求得
	                      //的波兰表达式值存入result
	                      //若计算中途发生错误（如除数为0），则
	double getvalue(void);   //返回计算得到的表达式值，
	                          
//内部功能函数
	int seekStr(char *str,int &i,char *s);
	   //检查字符窜str的第i个位置开始的后面字符是否与字符窜s相匹配
	   //是的话将i=i+len(s),并返回1，否则返回0
	double stringToDigital(char *s);  //将数字字符串转化成数值
                                      //返回值为字符串的数值

	virtual ~CMathString();


};

#endif // !defined(AFX_MATHSTRING_H__669187A1_784A_421F_B9EA_EAED6FA6D9AF__INCLUDED_)

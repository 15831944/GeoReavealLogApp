// MathString.cpp: implementation of the CMathString class.
//
////////////////////////////////////////////////////////////////////
// MathString.cpp: implementation of the CMathString class.
//
//////////////////////////////////////////////////////////////////////
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
//        或调用Initial(char *s)、Initial(char *s,double x)将其初始化
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

#include "stdafx.h"
#include "MathString.h"
#include "math.h"
#include "string.h"

#ifndef PI
#define PI 3.1415926257
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMathString::CMathString()
{

}

CMathString::~CMathString()
{

}


CMathString::CMathString(char *str)  //构造函数，将算术表达式str赋给String
{
	strcpy(string,str);            
}

CMathString::CMathString(char *str,double x)
{    //表达式中带未知数的构造函数
	 //算术表达式*str赋给string，未知数x赋给xx
	 //用于积分表达式
	strcpy(string,str);
	xx=x;
}

void CMathString::setX(double x)      //设置成员变量xx的值
{
	xx=x;
}

int CMathString::seekStr(char *str,int &i, char *s)
{   //检查字符窜str的第i个位置开始的后面字符是否与字符窜s相匹配
	//是的话将i=i+len(s),并返回1，否则返回0
	int j=0;
	for(j=0;j<strlen(s);j++)
		if(str[i+j]!=s[j])
			break;
	if(j==strlen(s))
	{
		i=i+strlen(s);
		return 1;
	}
	else return 0;
}

void CMathString::Initial(char *s)  //初始化函数，将算术表达式s赋给String
{
	strcpy(string,s); 

}

void CMathString::Initial(char *str, double x)
{
	//表达式中带未知数的初始化函数
	 //算术表达式*str赋给string，未知数x赋给xx
	 //用于积分表达式
	strcpy(string,str);
	xx=x;

}


int CMathString::checkString()
{ //检查输入的表达式是否正确，包括括号是否配对、库函数是否正确，
  //不包括小数点是否正确，若正确，将表达式中的大、中括号该成小括号
  //并在表达式尾加‘#’标识
  //若正确，返回值为1；否则返回0；
  //因积分表达式中有x，所以表达式中的x作为字符处理
	char st[MAX/2];        //存放括号的栈
	int top=0,     //栈顶
		i;
	for(i=0;string[i]!='\0';i++)
	{
		//如果是左括号，则入栈
		if(string[i]=='(' || string[i]=='[' || string[i]=='{')
		{  //如果是左括号，则入栈
			top++;
			st[top]=string[i];
			continue;
		}
        
		//小括号
		if(string[i]==')')   //小括号
		{  
			if(st[top]=='(')   //配对
			{
				top--;         //出栈
				continue;
			}
			else return 0;    //不配对，返回0
		}
        //中括号
		if(string[i]==']')    
		{  
			if(st[top]=='[')   //配对
			{
				top--;         //出栈
				continue;
			}
			else return 0;    //不配对，返回0
		}
		//大括号
		if(string[i]=='}')    
		{  
			if(st[top]=='{')   //配对
			{
				top--;         //出栈
				continue;
			}
			else return 0;    //不配对，返回0
		}

		//运算符
		if(string[i]=='+' || string[i]=='-' || string[i]=='*' || string[i]=='/')
		{   //如果有连续运算符则错误
			if(string[i+1]=='+' || string[i+1]=='-' || string[i+1]=='*' ||string[i+1]=='/')
				return 0;  //错误，返回0
			else
				continue;  //无连续运算符情况
		}

		//数字、小数点、逗号
 		if(('0'<=string[i] && string[i]<='9') || string[i]=='.'
			||string[i]==',')
 			continue;

		//未知数x
		if(string[i]=='x')
			continue;

		//是以a开头的函数
		if(string[i]=='a')
		{
			if(seekStr(string,i,"abs")||seekStr(string,i,"acos")||
				seekStr(string,i,"asin")||seekStr(string,i,"atan")||
				seekStr(string,i,"acot"))//是以a开头的库函数
			{
				if(string[i]=='('||string[i]=='['||string[i]=='{')
				{   //如果函数后是括号
					st[++top]=string[i];  // 括号入栈  
					continue;
				}
				else //如果函数后不是括号,返回0
					return 0;
			}
			else    //不是以a开头的库函数,返回0
				return 0;
		}

		//是以c开头的函数
		if(string[i]=='c')
		{
			if(seekStr(string,i,"cos")||seekStr(string,i,"cosh")||
				seekStr(string,i,"cot"))
			{
				if(string[i]=='('||string[i]=='['||string[i]=='{')
				{   //如果函数后是括号
					st[++top]=string[i];  // 括号入栈  
					continue;
				}
				else //如果函数后不是括号,返回0
					return 0;
			}
			else    //不是以c开头的库函数,返回0
				return 0;
		}
		//是以e开头的函数
		if(string[i]=='e')
		{
			if(seekStr(string,i,"exp"))
			{
				if(string[i]=='('||string[i]=='['||string[i]=='{')
				{   //如果函数后是括号
					st[++top]=string[i];  // 括号入栈  
					continue;
				}
				else //如果函数后不是括号,返回0
					return 0;
			}
			else    //不是以e开头的库函数,返回0
				return 0;
		}
		//是以f开头的函数
		if(string[i]=='f')
		{
			if(seekStr(string,i,"floor"))
			{
				if(string[i]=='('||string[i]=='['||string[i]=='{')
				{   //如果函数后是括号
					st[++top]=string[i];  // 括号入栈  
					continue;
				}
				else //如果函数后不是括号,返回0
					return 0;
			}
			else    //不是以f开头的库函数,返回0
				return 0;
		}
		//是以m开头的函数
		if(string[i]=='m')
		{
			if(seekStr(string,i,"mod"))
			{
				if(string[i]=='('||string[i]=='['||string[i]=='{')
				{   //如果函数后是括号
					st[++top]=string[i];  // 括号入栈  
					continue;
				}
				else //如果函数后不是括号,返回0
					return 0;
			}
			else    //不是以m开头的库函数,返回0
				return 0;
		}
		//是以l开头的函数
		if(string[i]=='l')
		{
			if(seekStr(string,i,"ln")||seekStr(string,i,"log"))
			{
				if(string[i]=='('||string[i]=='['||string[i]=='{')
				{   //如果函数后是括号
					st[++top]=string[i];  // 括号入栈  
					continue;
				}
				else //如果函数后不是括号,返回0
					return 0;
			}
			else    //不是以l开头的库函数,返回0
				return 0;
		}
		//是以p开头的函数
		if(string[i]=='p')
		{
			if(seekStr(string,i,"pow"))
			{
				if(string[i]=='('||string[i]=='['||string[i]=='{')
				{   //如果函数后是括号
					st[++top]=string[i];  // 括号入栈  
					continue;
				}
				else //如果函数后不是括号,返回0
					return 0;
			}
			else    //不是以p开头的库函数,返回0
				return 0;
		}
		//是以s开头的函数
		if(string[i]=='s')
		{
			if(seekStr(string,i,"sin")||seekStr(string,i,"sinh")||
				seekStr(string,i,"sqrt"))
			{
				if(string[i]=='('||string[i]=='['||string[i]=='{')
				{   //如果函数后是括号
					st[++top]=string[i];  // 括号入栈  
					continue;
				}
				else //如果函数后不是括号,返回0
					return 0;
			}
			else    //不是以s开头的库函数,返回0
				return 0;
		}
		//是以t开头的函数
		if(string[i]=='t')
		{
			if(seekStr(string,i,"tan")||seekStr(string,i,"tanh"))
			{
				if(string[i]=='('||string[i]=='['||string[i]=='{')
				{   //如果函数后是括号
					st[++top]=string[i];  // 括号入栈  
					continue;
				}
				else //如果函数后不是括号,返回0
					return 0;
			}
			else    //不是以t开头的库函数,返回0
				return 0;
		}
		
		else // 如果string[i]是其他字符,则返回0
			return 0;
	}//for(i=0;string[i]!='\0';i++)
	
	if(top!=0)       //括号不匹配,返回0
		return 0;
    
	//表达式正确
	//在表达式尾加‘#’标识
	string[i]='#';
	string[++i]='\0';

	//将表达式中的大、中括号该成小括号
	for(i=0;string[i]!='\0';i++)
	{
		if((string[i]=='[')||(string[i]=='{'))
			string[i]='(';
		else if((string[i]==']')||(string[i]=='}'))
			string[i]=')';
	}
	return 1;	
}

//将算术表达式string转化成波兰表达式，
//波兰表达式放在bolan[]中
//转换方法：
//1.判定string[i]为数值，直接存入bolan数组中
//2.判定string[i]为'(',将此'('压入栈,
//  若'('前为数字，则在'('前加一对'*'的处理
//3.判定string[i]为')',则将栈stact中左括号'('以后的运算符和函数依次
//  弹出,并入数组bolan中,然后将左括号'('弹出
//4.判定string[i]为'-','+',则将当前栈stack中左括号'('以后（如无'('则
//  将栈stack中所有）的运算符和函数依次弹出，存入数组bolan中,然后将
//  '-'或'+'压入栈stack中
//  如果'-'在第一个位置或其前是'('，即'-'是负号，将'~'（取负）压入stack中
//5.判定string[i]为'*'或'/',则将栈stact中顶端连续的'*'或'/'或函数依次
//  弹出，存入数组bolan中,然后将'*'或'/'压入栈stact中
//6.判定string[i]为函数，则将该函数入栈stact中
//  存入栈stact中变量func的是该函数的编号，详细编号见头文件
//  若该函数前为数字，则在该函数入栈stact前加一次对'*'的处理
//7.判定string[i]=',',将栈stact中'('后的运算符和函数全部弹出，压入数
//  组bolan中，
//8.判定string[i]='x',则用类中未知数变量xx代替，压入数组bolan中 
//9.转换结束时，若栈stact不为空，则将栈内所有运算符和函数弹出，存入
//  数组bolan中
//10.转换结束，在数组bolan尾加一opera='#'作为结束符
int CMathString::stringToBolan()
{
	Bolan stack[MAX];         //存放运算符和函数的栈
	int top=0,         //存放栈stact的下标
		i,           //string的下标
		j=0,           //bolan数组的下标
		t;
	char digitStr[20];     //存放要转化成数值的字符串
	
	for(i=0;string[i]!='#';i++)
	{
		//判定为数值，直接存入bolan数组中
		if(('0'<=string[i] && string[i]<='9')||string[i]=='.') //数值
		{
			t=0;
			digitStr[t]=string[i];   //数字存入要转化成数值的字符串
			while(('0'<=string[i+1] && string[i+1]<='9')
				||string[i+1]=='.')  
			{                       //将连续数字入要转化成数值的字符串
				digitStr[++t]=string[i+1];
				i++;
			}
			digitStr[++t]='\0';
        	 //将数字字符串转化成数值,并且入bolan数组
			bolan[j].flag=0;    //标识该节点为数值
			bolan[j].value=stringToDigital(digitStr);
			j++;
			continue;
		}

		//判定为'(',将此'('压入栈
		//若'('前为数字，则在'('前加一对'*'的处理
		if(string[i]=='(')
		{
			if(string[i-1]<='0' || '9'<=string[i-1])//'('前不为数字
			{
			top++;
			stack[top].flag=1;  //标识该节点为运算符
			stack[top].oper='(';
			continue;
			}
			else      //'('前为数字
			{         //加一次对'*'的处理
				while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
				{
				   bolan[j]=stack[top];
				   j++;
				   top--;
				}
			    //将'*'压入栈stact中
		    	top++;
			    stack[top].flag=1;
			    stack[top].oper='*';

				//将'('压入栈stact中
				top++;
				stack[top].flag=1;
				stack[top].oper='(';
				continue;
			}
		}

		//判定为')',则将栈stact中左括号'('以后的运算符和函数依次弹出
		//寸入数组bolan中,然后将左括号'('弹出
		if(string[i]==')')
		{
			while(stack[top].oper!='(')
			{
				bolan[j]=stack[top];
				j++;
				top--;
			}
			top--;  //将左括号'('弹出
			continue;
		}

		//判定为'-','+',则将当前栈stack中左括号'('以后（如无'('则将
		//栈stack中所有）的运算符和函数依次弹出，存入数组bolan中,
		//然后将'-'或'+'压入栈stack中
		//如果'-'在第一个位置或其前是'('，即'-'是负号，将'~'压入stack中
		if(string[i]=='+' || string[i]=='-')
		{
			if(string[i]=='-'&&(i==0 || string[i-1]=='(')) //'-'是负号
			{
				top++;
				stack[top].flag=1;
				stack[top].oper='~';
				continue;
			}
			else             //'-','+'是加减符号
			{
				while(stack[top].oper!='(' && top!=0)
				{
					bolan[j]=stack[top]; //当前栈stack中的运算符和
				                        //函数依次弹出存入数组bolan中
				    j++;
				    top--;
				}
				//将'-'或'+'压入栈stack中
				top++;
				stack[top].flag=1;
				stack[top].oper=string[i];
				continue;
			}
		}

		//判定string[i]为'*'或'/',则将栈stact中顶端连续的'*'或'/'或函
		//数依次弹出，存入数组bolan中,然后将'*'或'/'压入栈stact中
		if(string[i]=='*' || string[i]=='/')
		{
			while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
			{
				bolan[j]=stack[top];
				j++;
				top--;
			}
			//将'*'或'/'压入栈stact中
			top++;
			stack[top].flag=1;
			stack[top].oper=string[i];
			continue;
		}

		//判定string[i]为函数，则将该函数入栈stact中
		//存入栈stact中变量func的是该函数的编号，详细编号见头文件
		//若该函数前为数字，则在该函数入栈stact前加一次对'*'的处理
		if(string[i]=='a')
		{
			if('0'<=string[i-1] && string[i-1]<='9')  //该函数前为数字
			{              //在该函数入栈stact前加一次对'*'的处理
				while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
				{
				   bolan[j]=stack[top];
				   j++;
				   top--;
				}
			    //将'*'压入栈stact中
		    	top++;
			    stack[top].flag=1;
			    stack[top].oper='*';
			}
		 
		    //为abs(x)函数
		    if(seekStr(string,i,"abs"))   
			{                            //将该函数入栈stact中
				top++;
				stack[top].flag=2;
				stack[top].func=1;
				i--;                    //因为在seekStr函数中
				continue;               //i=i+strlen("abs");
				                        //i已指向函数abs的下一个字符
				                        //而在for语句中有i++,所以此处
				                        //i要减1
			}
			//为acos(x)函数
			if(seekStr(string,i,"acos"))   
			{                            //将该函数入栈stact中
				top++;
				stack[top].flag=2;
				stack[top].func=2;
				i--;                     
				continue;  
			}
			//为asin(x)函数
			if(seekStr(string,i,"asin"))   
			{                            //将该函数入栈stact中
				top++;
				stack[top].flag=2;
				stack[top].func=3;
				i--;                     
				continue;  
			}
			//为atan(x)函数
			if(seekStr(string,i,"atan"))   
			{                            //将该函数入栈stact中
				top++;
				stack[top].flag=2;
				stack[top].func=4;
				i--;                     
				continue;  
			}
			//为acot(x)函数
			if(seekStr(string,i,"acot"))   
			{                            //将该函数入栈stact中
				top++;
				stack[top].flag=2;
				stack[top].func=5;
				i--;                     
				continue;  
			}
		}//if(string[i]=='a')
		//string[i]为以c开头的库函数
		if(string[i]=='c')
		{
			if('0'<=string[i-1] && string[i-1]<='9')  //该函数前为数字
			{              //在该函数入栈stact前加一次对'*'的处理
				while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
				{
				   bolan[j]=stack[top];
				   j++;
				   top--;
				}
			    //将'*'压入栈stact中
		    	top++;
			    stack[top].flag=1;
			    stack[top].oper='*';
			}
			//为cos(x)函数
			if(seekStr(string,i,"cos"))   
			{                            //将该函数入栈stact中
				top++;
				stack[top].flag=2;
				stack[top].func=6;
				i--;                     
				continue;  
			}
			//为cosh(x)函数
			if(seekStr(string,i,"cosh"))   
			{                            //将该函数入栈stact中
				top++;
				stack[top].flag=2;
				stack[top].func=7;
				i--;                     
				continue;  
			}
			//为cot(x)函数
			if(seekStr(string,i,"cos"))   
			{                            //将该函数入栈stact中
				top++;
				stack[top].flag=2;
				stack[top].func=8;
				i--;                     
				continue;  
			}
		}//if(string[i]=='c')
		//string[i]为以e开头的库函数
		if(string[i]=='e')
		{
			if('0'<=string[i-1] && string[i-1]<='9')  //该函数前为数字
			{              //在该函数入栈stact前加一次对'*'的处理
				while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
				{
				   bolan[j]=stack[top];
				   j++;
				   top--;
				}
			    //将'*'压入栈stact中
		    	top++;
			    stack[top].flag=1;
			    stack[top].oper='*';
			}
			//为exp(x)函数
			if(seekStr(string,i,"exp"))   
			{                            //将该函数入栈stact中
				top++;
				stack[top].flag=2;
				stack[top].func=9;
				i--;                     
				continue;  
			}
		}//if(string[i]=='e')
		//string[i]为以f开头的库函数
		if(string[i]=='f')
		{
			if('0'<=string[i-1] && string[i-1]<='9')  //该函数前为数字
			{              //在该函数入栈stact前加一次对'*'的处理
				while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
				{
				   bolan[j]=stack[top];
				   j++;
				   top--;
				}
			    //将'*'压入栈stact中
		    	top++;
			    stack[top].flag=1;
			    stack[top].oper='*';
			}
			//为exp(x)函数
			if(seekStr(string,i,"floor"))   
			{                            //将该函数入栈stact中
				top++;
				stack[top].flag=2;
				stack[top].func=10;
				i--;                     
				continue;  
			}
		}//if(string[i]=='f')
		//string[i]为以m开头的库函数
		if(string[i]=='m')
		{
			if('0'<=string[i-1] && string[i-1]<='9')  //该函数前为数字
			{              //在该函数入栈stact前加一次对'*'的处理
				while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
				{
				   bolan[j]=stack[top];
				   j++;
				   top--;
				}
			    //将'*'压入栈stact中
		    	top++;
			    stack[top].flag=1;
			    stack[top].oper='*';
			}
			//为mod(x)函数
			if(seekStr(string,i,"mod"))   
			{                            //将该函数入栈stact中
				top++;
				stack[top].flag=2;
				stack[top].func=11;
				i--;                     
				continue;  
			}
		}//if(string[i]=='m')
		//string[i]为以l开头的库函数
		if(string[i]=='l')
		{
			if('0'<=string[i-1] && string[i-1]<='9')  //该函数前为数字
			{              //在该函数入栈stact前加一次对'*'的处理
				while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
				{
				   bolan[j]=stack[top];
				   j++;
				   top--;
				}
			    //将'*'压入栈stact中
		    	top++;
			    stack[top].flag=1;
			    stack[top].oper='*';
			}
			//为ln(x)函数
			if(seekStr(string,i,"ln"))   
			{                            //将该函数入栈stact中
				top++;
				stack[top].flag=2;
				stack[top].func=12;
				i--;                     
				continue;  
			}
			//为log(x)函数
			if(seekStr(string,i,"log"))   
			{                            //将该函数入栈stact中
				top++;
				stack[top].flag=2;
				stack[top].func=13;
				i--;                     
				continue;  
			}
		}//if(string[i]=='l')
		//string[i]为以p开头的库函数
		if(string[i]=='p')
		{
			if('0'<=string[i-1] && string[i-1]<='9')  //该函数前为数字
			{              //在该函数入栈stact前加一次对'*'的处理
				while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
				{
				   bolan[j]=stack[top];
				   j++;
				   top--;
				}
			    //将'*'压入栈stact中
		    	top++;
			    stack[top].flag=1;
			    stack[top].oper='*';
			}
			//为pow(x)函数
			if(seekStr(string,i,"pow"))   
			{                            //将该函数入栈stact中
				top++;
				stack[top].flag=2;
				stack[top].func=14;
				i--;                     
				continue;  
			}
		}//if(string[i]=='p')
		//string[i]为以s开头的库函数
		if(string[i]=='s')
		{
			if('0'<=string[i-1] && string[i-1]<='9')  //该函数前为数字
			{              //在该函数入栈stact前加一次对'*'的处理
				while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
				{
				   bolan[j]=stack[top];
				   j++;
				   top--;
				}
			    //将'*'压入栈stact中
		    	top++;
			    stack[top].flag=1;
			    stack[top].oper='*';
			}
			//为sin(x)函数
			if(seekStr(string,i,"sin"))   
			{                            //将该函数入栈stact中
				top++;
				stack[top].flag=2;
				stack[top].func=15;
				i--;                     
				continue;  
			}
			//为sinh(x)函数
			if(seekStr(string,i,"sinh"))   
			{                            //将该函数入栈stact中
				top++;
				stack[top].flag=2;
				stack[top].func=16;
				i--;                     
				continue;  
			}
			//为sin(x)函数
			if(seekStr(string,i,"sqrt"))   
			{                            //将该函数入栈stact中
				top++;
				stack[top].flag=2;
				stack[top].func=17;
				i--;                     
				continue;  
			}
		}//if(string[i]=='s')
		//string[i]为以t开头的库函数
		if(string[i]=='t')
		{
			if('0'<=string[i-1] && string[i-1]<='9')  //该函数前为数字
			{              //在该函数入栈stact前加一次对'*'的处理
				while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
				{
				   bolan[j]=stack[top];
				   j++;
				   top--;
				}
			    //将'*'压入栈stact中
		    	top++;
			    stack[top].flag=1;
			    stack[top].oper='*';
			}
			//为tan(x)函数
			if(seekStr(string,i,"tan"))   
			{                            //将该函数入栈stact中
				top++;
				stack[top].flag=2;
				stack[top].func=18;
				i--;                     
				continue;  
			}
			//为tanh(x)函数
			if(seekStr(string,i,"tanh"))   
			{                            //将该函数入栈stact中
				top++;
				stack[top].flag=2;
				stack[top].func=19;
				i--;                     
				continue;  
			}
		}//if(string[i]=='p')
	    
		//判定string[i]=',',将栈stact中'('后的运算符和函数全部弹出，
		//压入数组bolan中，
		if(string[i]==',')
		{
			while(stack[top].oper!='(')
			{
				bolan[j]=stack[top];
				j++;
				top--;
			}
			continue;
		}

		//判定string[i]='x',则用类中未知数变量xx代替，压入数组bolan中 
		if(string[i]=='x')
		{
			bolan[j].flag=0;   //标识为数值
			bolan[j].value=xx;
			j++;
		}
    }

	//转换结束时，若栈stact不为空，则将栈内所有运算符和函数弹出，存入
	//数组bolan中
	while(top!=0)
	{
		bolan[j]=stack[top];
		j++;
		top--;
	}

	//转换结束，在数组bolan尾加一opera='#'作为结束符
    bolan[j].oper='#';

    return 1;
}

//将数字字符串转化成数值
double CMathString::stringToDigital(char *s)
{
	double sum=0,    //转化后的数值
		temp,
		ten=0.1;     //小数变化值
	int i,ch,
		flag=0;    //标识是小数还是整数，0为整数
	for(i=0;s[i]!='\0';i++)
	{
		if(s[i]!='.')
		{
			if(flag==0)  //整数部分
			{
				ch=s[i]-'0';
				sum=sum+ch;
				sum=sum*10;
			}
			else        //小数部分
			{
				ch=s[i]-'0';
				temp=ch*ten;   //小数点移位
				sum=sum+temp;
				ten=ten*0.1;    //改变小数点位置
			}
		}
		else     //小数点
		{
			sum=sum/10;
			flag=1;
		}
	}//for
	if(flag==0)    //无小数部分
		sum=sum/10;
	return sum;

}

//求波兰式bolan的值
//方法如下：
//1.若节点bolan[i]为数值，则如数据栈dataStact
//2.若节点bolan[i]为运算符，则从数据栈dataStact弹出数据进行计算，并
//  将结果压入数据栈dataStact中
//3.若节点bolan[i]为函数，则从数据栈dataStact弹出数据，调用相应的库
//  函数进行计算，并将结果压入数据栈dataStact中
//4.若节点bolan[i]为结束符'#',则数据栈dataStact中的数据弹出，赋给
//  result，并返回0
//返回值:
//1. 计算正确，返回0
//2. 在计算中若除数为0，返回1
//3. 反余弦函数acos(x)中的x不满足条件，返回2
//4. 反正弦函数asin(x)中的x不满足条件，返回3
//5. 余切函数cot(x) 中tan（x）为0，返回4
//6.取模x%y函数mod(x,y)中y为0，返回5
//7.自然对数函数ln(x),如果x<=0,则返回6
//8.取10的对数函数log10(x),如果x<=0,则返回7
//9.开方函数sqrt(x),如果x<0,则返回8
//10.计算中有其他错误，返回-1
int CMathString::compvalue(void)
{
	double  dataStack[MAX/2];   //存放中间数据的数据栈
	int top=0,            //数据栈dataStact的栈顶
		i;              //数组bolan的下标
	for(i=0;bolan[i].oper!='#';i++)
	{
		//节点bolan[i]为数值，则如数据栈dataStact
		if(bolan[i].flag==0)
		{
			top++;
			dataStack[top]=bolan[i].value;
			continue;
		}

		//节点bolan[i]为运算符，则从数据栈dataStact弹出数据进行计算，并
        //将结果压入数据栈dataStact中
		//在计算中若除数为0，返回1
		if(bolan[i].flag==1)
		{
			switch(bolan[i].oper)
			{
			case '+':
				dataStack[top-1]=dataStack[top-1]+dataStack[top];
				top--;
				break;
				
			case  '-':
				dataStack[top-1]=dataStack[top-1]-dataStack[top];
				top--;
				break;

			case '*':
                dataStack[top-1]=dataStack[top-1]*dataStack[top];
				top--;
				break;

			case '/':
				if(dataStack[top]!=0.0)
				{
					dataStack[top-1]=dataStack[top-1]/dataStack[top];
				    top--;
				    break;
				}
				else  //除数为0，返回1
				{
					return 1;
				}
			case '~':      //取负
				dataStack[top]=0-dataStack[top];
				break;
			}//switch
			continue;
		}//if(bolan[i].flag==1)

		//若节点bolan[i]为函数，则从数据栈dataStact弹出数据，调用相应
		//的库函数进行计算，并将结果压入数据栈dataStact中
		if(bolan[i].flag==2)
		{
			switch(bolan[i].func)
			{
			case 1:    //求绝对值函数abs(x)
				dataStack[top]=fabs(dataStack[top]);
				break;

			case 2:    //反余弦函数acos(x)
				if(-1.0<=dataStack[top] && dataStack[top]<=1.0)
				{
				    dataStack[top]=acos(dataStack[top]);
				    break;
				}
				else  //反余弦函数acos(x)中的x不满足条件，返回2
				{
					return 2;
				}

			case 3:   //反正弦函数asin(x)
				if(-1<=dataStack[top] && dataStack[top]<=1)
				{
				    dataStack[top]=asin(dataStack[top]);
				    break;
				}
				else  //反正弦函数asin(x)中的x不满足条件，返回3
				{
					return 3;
				}

			case 4:   //反正切函数atan(x)
				dataStack[top]=atan(dataStack[top]);
				break;

			case 5:  //反余切函数acot(x)
				dataStack[top]=PI/2-atan(dataStack[top]);
				break;

			case 6:   //余弦函数cos(x)
				dataStack[top]=cos(dataStack[top]);
				break;
               
			case 7:    //双曲余弦cosh(x)
				dataStack[top]=cosh(dataStack[top]);
				break;

			case 8:    //余切函数cot(x)   cot(x)=1/tan(x)
				if(tan(dataStack[top])!=0)
				{
					dataStack[top]=1/tan(dataStack[top]);
					break;
				}
				else    //余切函数cot(x) 中tan（x）为0，返回4
				{
					return 4;
				}
				
			case 9:    //e的x次方函数exp(x)
				dataStack[top]=exp(dataStack[top]);
				break;

			case 10:   //求不大于x的最大整数 floor(x)
				dataStack[top]=floor(dataStack[top]);
				break;

			case 11:   //取模x%y函数mod(x,y),即
				if(dataStack[top]!=0)
				{
				   dataStack[top-1]=fmod(dataStack[top-1],dataStack[top]);
				   top--;
				   break;
				}
				else   //取模x%y函数mod(x,y)中y为0，返回5
				{
					return 5;
				}

			case 12:    //自然对数函数ln(x),如果x<=0,则返回6
				if(dataStack[top]>0)
				{          //ln(x)的库函数为log(x)
					dataStack[top]=log(dataStack[top]);
					break;
				}
				else
				{
					return 6;
				}

			case 13:    //取10的对数函数log10(x),如果x<=0,则返回7
				if(dataStack[top]>0)
				{          //log10(x)的库函数为log10(x)
					dataStack[top]=log10(dataStack[top]);
					break;
				}
				else
				{
					return 7;
				}

			case 14:    //x的y次方函数pow(x,y)
				dataStack[top-1]=pow(dataStack[top-1],dataStack[top]);
				top--;
				break;

			case 15:    //正弦函数sin(x)
				dataStack[top]=sin(dataStack[top]);
				break;

			case 16:    //双曲正弦sinh(x)
				dataStack[top]=sinh(dataStack[top]);
				break;

			case 17:   //开方函数sqrt(x),如果x<0,则返回8
				if(dataStack[top]>=0)
				{
					dataStack[top]=sqrt(dataStack[top]);
					break;
				}
				else
				{
					return 8;
				}

			case 18:    //正切函数tan(x)
				dataStack[top]=tan(dataStack[top]);
				break;

			case 19:    //双曲正切函数tanh(x)
				dataStack[top]=tanh(dataStack[top]);
				break;
            }//switch(bolan[i].func)
			continue;
		}//if(bolan[i].flag==2)
	}//for(i=0;bolan[i].oper!='#';i++)
	if(top==1)       //计算正确
	{
		result=dataStack[top];
		return 0;    //返回0
	}
	else           //计算中有其他错误，返回-1
		return -1;   
}

double CMathString::getvalue(void)  //返回计算得到的表达式值
{
	return result;
}



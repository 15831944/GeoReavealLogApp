// MathString.cpp: implementation of the CMathString class.
//
////////////////////////////////////////////////////////////////////
// MathString.cpp: implementation of the CMathString class.
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////                    
//
//                   ���������ʽֵ����
//
//              ���ϳ���ѧԺ  ��Ϣ������ѧϵ  
//                        �ƽ���
//                 jiangfenghuang@msn.com
//                 jiangfenghuang@163.net
//                       2003.4.5           
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//
// ���ܣ�
//      �����ַ����е��������ʽ��ֵ��
//   
// �ص㣺
//      1.֧���ַ����к��и��ֳ��ú�������"7.5+sin(6*ln(8))/exp(5)"
//      2.���кܺõľ����������ܼ������ʽ�������Ƿ���ԡ��⺯����
//        ����ȷ
//      3.����������ܼ��������쳣�������Ϊ0����������sqrt(x)��x<0
//        �����Һ���acos(x)�е�x<-1��x>1��
//      4.Ϊ֧�ֻ��֡��󷽳̣��������ʽ�пɺ���x������ʱ�������еĳ�
//        Ա����xx���� 
//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////// 
//
// ʹ�÷�����
//      1. ����һ��CMathString �Ķ�����Ҫ����������ʽ�����ʼ��
//         �磺 char *s1="7.5+sin(6)/exp(5)";
//              char *s2="4*pow(x,2)+2*x+4";
//              CMathString  mathstr1(s1);
//              CMathString  mathstr2(s2);
//        �����Initial(char *s)��Initial(char *s,double x)�����ʼ��
//      2.����checkString���������ʽ�Ƿ�Ϸ�
//      3.���������ֻ�j�ⷽ�̣�����setX(double x)���ñ��ʽ��x��ֵ
//      4.����stringToBolan()���������ʽת��Ϊ�������ʽ
//      5.����compvalue()�Բ������ʽ���м��㣬������ֵΪ0�����
//        ��getvalue()ȡ�ü�����������ɸ��ݷ���ֵ�жϳ���ط�
//
/////////////////////////////////////////////////////////////////////
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


CMathString::CMathString(char *str)  //���캯�������������ʽstr����String
{
	strcpy(string,str);            
}

CMathString::CMathString(char *str,double x)
{    //���ʽ�д�δ֪���Ĺ��캯��
	 //�������ʽ*str����string��δ֪��x����xx
	 //���ڻ��ֱ��ʽ
	strcpy(string,str);
	xx=x;
}

void CMathString::setX(double x)      //���ó�Ա����xx��ֵ
{
	xx=x;
}

int CMathString::seekStr(char *str,int &i, char *s)
{   //����ַ���str�ĵ�i��λ�ÿ�ʼ�ĺ����ַ��Ƿ����ַ���s��ƥ��
	//�ǵĻ���i=i+len(s),������1�����򷵻�0
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

void CMathString::Initial(char *s)  //��ʼ�����������������ʽs����String
{
	strcpy(string,s); 

}

void CMathString::Initial(char *str, double x)
{
	//���ʽ�д�δ֪���ĳ�ʼ������
	 //�������ʽ*str����string��δ֪��x����xx
	 //���ڻ��ֱ��ʽ
	strcpy(string,str);
	xx=x;

}


int CMathString::checkString()
{ //�������ı��ʽ�Ƿ���ȷ�����������Ƿ���ԡ��⺯���Ƿ���ȷ��
  //������С�����Ƿ���ȷ������ȷ�������ʽ�еĴ������Ÿó�С����
  //���ڱ��ʽβ�ӡ�#����ʶ
  //����ȷ������ֵΪ1�����򷵻�0��
  //����ֱ��ʽ����x�����Ա��ʽ�е�x��Ϊ�ַ�����
	char st[MAX/2];        //������ŵ�ջ
	int top=0,     //ջ��
		i;
	for(i=0;string[i]!='\0';i++)
	{
		//����������ţ�����ջ
		if(string[i]=='(' || string[i]=='[' || string[i]=='{')
		{  //����������ţ�����ջ
			top++;
			st[top]=string[i];
			continue;
		}
        
		//С����
		if(string[i]==')')   //С����
		{  
			if(st[top]=='(')   //���
			{
				top--;         //��ջ
				continue;
			}
			else return 0;    //����ԣ�����0
		}
        //������
		if(string[i]==']')    
		{  
			if(st[top]=='[')   //���
			{
				top--;         //��ջ
				continue;
			}
			else return 0;    //����ԣ�����0
		}
		//������
		if(string[i]=='}')    
		{  
			if(st[top]=='{')   //���
			{
				top--;         //��ջ
				continue;
			}
			else return 0;    //����ԣ�����0
		}

		//�����
		if(string[i]=='+' || string[i]=='-' || string[i]=='*' || string[i]=='/')
		{   //�������������������
			if(string[i+1]=='+' || string[i+1]=='-' || string[i+1]=='*' ||string[i+1]=='/')
				return 0;  //���󣬷���0
			else
				continue;  //��������������
		}

		//���֡�С���㡢����
 		if(('0'<=string[i] && string[i]<='9') || string[i]=='.'
			||string[i]==',')
 			continue;

		//δ֪��x
		if(string[i]=='x')
			continue;

		//����a��ͷ�ĺ���
		if(string[i]=='a')
		{
			if(seekStr(string,i,"abs")||seekStr(string,i,"acos")||
				seekStr(string,i,"asin")||seekStr(string,i,"atan")||
				seekStr(string,i,"acot"))//����a��ͷ�Ŀ⺯��
			{
				if(string[i]=='('||string[i]=='['||string[i]=='{')
				{   //���������������
					st[++top]=string[i];  // ������ջ  
					continue;
				}
				else //���������������,����0
					return 0;
			}
			else    //������a��ͷ�Ŀ⺯��,����0
				return 0;
		}

		//����c��ͷ�ĺ���
		if(string[i]=='c')
		{
			if(seekStr(string,i,"cos")||seekStr(string,i,"cosh")||
				seekStr(string,i,"cot"))
			{
				if(string[i]=='('||string[i]=='['||string[i]=='{')
				{   //���������������
					st[++top]=string[i];  // ������ջ  
					continue;
				}
				else //���������������,����0
					return 0;
			}
			else    //������c��ͷ�Ŀ⺯��,����0
				return 0;
		}
		//����e��ͷ�ĺ���
		if(string[i]=='e')
		{
			if(seekStr(string,i,"exp"))
			{
				if(string[i]=='('||string[i]=='['||string[i]=='{')
				{   //���������������
					st[++top]=string[i];  // ������ջ  
					continue;
				}
				else //���������������,����0
					return 0;
			}
			else    //������e��ͷ�Ŀ⺯��,����0
				return 0;
		}
		//����f��ͷ�ĺ���
		if(string[i]=='f')
		{
			if(seekStr(string,i,"floor"))
			{
				if(string[i]=='('||string[i]=='['||string[i]=='{')
				{   //���������������
					st[++top]=string[i];  // ������ջ  
					continue;
				}
				else //���������������,����0
					return 0;
			}
			else    //������f��ͷ�Ŀ⺯��,����0
				return 0;
		}
		//����m��ͷ�ĺ���
		if(string[i]=='m')
		{
			if(seekStr(string,i,"mod"))
			{
				if(string[i]=='('||string[i]=='['||string[i]=='{')
				{   //���������������
					st[++top]=string[i];  // ������ջ  
					continue;
				}
				else //���������������,����0
					return 0;
			}
			else    //������m��ͷ�Ŀ⺯��,����0
				return 0;
		}
		//����l��ͷ�ĺ���
		if(string[i]=='l')
		{
			if(seekStr(string,i,"ln")||seekStr(string,i,"log"))
			{
				if(string[i]=='('||string[i]=='['||string[i]=='{')
				{   //���������������
					st[++top]=string[i];  // ������ջ  
					continue;
				}
				else //���������������,����0
					return 0;
			}
			else    //������l��ͷ�Ŀ⺯��,����0
				return 0;
		}
		//����p��ͷ�ĺ���
		if(string[i]=='p')
		{
			if(seekStr(string,i,"pow"))
			{
				if(string[i]=='('||string[i]=='['||string[i]=='{')
				{   //���������������
					st[++top]=string[i];  // ������ջ  
					continue;
				}
				else //���������������,����0
					return 0;
			}
			else    //������p��ͷ�Ŀ⺯��,����0
				return 0;
		}
		//����s��ͷ�ĺ���
		if(string[i]=='s')
		{
			if(seekStr(string,i,"sin")||seekStr(string,i,"sinh")||
				seekStr(string,i,"sqrt"))
			{
				if(string[i]=='('||string[i]=='['||string[i]=='{')
				{   //���������������
					st[++top]=string[i];  // ������ջ  
					continue;
				}
				else //���������������,����0
					return 0;
			}
			else    //������s��ͷ�Ŀ⺯��,����0
				return 0;
		}
		//����t��ͷ�ĺ���
		if(string[i]=='t')
		{
			if(seekStr(string,i,"tan")||seekStr(string,i,"tanh"))
			{
				if(string[i]=='('||string[i]=='['||string[i]=='{')
				{   //���������������
					st[++top]=string[i];  // ������ջ  
					continue;
				}
				else //���������������,����0
					return 0;
			}
			else    //������t��ͷ�Ŀ⺯��,����0
				return 0;
		}
		
		else // ���string[i]�������ַ�,�򷵻�0
			return 0;
	}//for(i=0;string[i]!='\0';i++)
	
	if(top!=0)       //���Ų�ƥ��,����0
		return 0;
    
	//���ʽ��ȷ
	//�ڱ��ʽβ�ӡ�#����ʶ
	string[i]='#';
	string[++i]='\0';

	//�����ʽ�еĴ������Ÿó�С����
	for(i=0;string[i]!='\0';i++)
	{
		if((string[i]=='[')||(string[i]=='{'))
			string[i]='(';
		else if((string[i]==']')||(string[i]=='}'))
			string[i]=')';
	}
	return 1;	
}

//���������ʽstringת���ɲ������ʽ��
//�������ʽ����bolan[]��
//ת��������
//1.�ж�string[i]Ϊ��ֵ��ֱ�Ӵ���bolan������
//2.�ж�string[i]Ϊ'(',����'('ѹ��ջ,
//  ��'('ǰΪ���֣�����'('ǰ��һ��'*'�Ĵ���
//3.�ж�string[i]Ϊ')',��ջstact��������'('�Ժ��������ͺ�������
//  ����,��������bolan��,Ȼ��������'('����
//4.�ж�string[i]Ϊ'-','+',�򽫵�ǰջstack��������'('�Ժ�����'('��
//  ��ջstack�����У���������ͺ������ε�������������bolan��,Ȼ��
//  '-'��'+'ѹ��ջstack��
//  ���'-'�ڵ�һ��λ�û���ǰ��'('����'-'�Ǹ��ţ���'~'��ȡ����ѹ��stack��
//5.�ж�string[i]Ϊ'*'��'/',��ջstact�ж���������'*'��'/'��������
//  ��������������bolan��,Ȼ��'*'��'/'ѹ��ջstact��
//6.�ж�string[i]Ϊ�������򽫸ú�����ջstact��
//  ����ջstact�б���func���Ǹú����ı�ţ���ϸ��ż�ͷ�ļ�
//  ���ú���ǰΪ���֣����ڸú�����ջstactǰ��һ�ζ�'*'�Ĵ���
//7.�ж�string[i]=',',��ջstact��'('���������ͺ���ȫ��������ѹ����
//  ��bolan�У�
//8.�ж�string[i]='x',��������δ֪������xx���棬ѹ������bolan�� 
//9.ת������ʱ����ջstact��Ϊ�գ���ջ������������ͺ�������������
//  ����bolan��
//10.ת��������������bolanβ��һopera='#'��Ϊ������
int CMathString::stringToBolan()
{
	Bolan stack[MAX];         //���������ͺ�����ջ
	int top=0,         //���ջstact���±�
		i,           //string���±�
		j=0,           //bolan������±�
		t;
	char digitStr[20];     //���Ҫת������ֵ���ַ���
	
	for(i=0;string[i]!='#';i++)
	{
		//�ж�Ϊ��ֵ��ֱ�Ӵ���bolan������
		if(('0'<=string[i] && string[i]<='9')||string[i]=='.') //��ֵ
		{
			t=0;
			digitStr[t]=string[i];   //���ִ���Ҫת������ֵ���ַ���
			while(('0'<=string[i+1] && string[i+1]<='9')
				||string[i+1]=='.')  
			{                       //������������Ҫת������ֵ���ַ���
				digitStr[++t]=string[i+1];
				i++;
			}
			digitStr[++t]='\0';
        	 //�������ַ���ת������ֵ,������bolan����
			bolan[j].flag=0;    //��ʶ�ýڵ�Ϊ��ֵ
			bolan[j].value=stringToDigital(digitStr);
			j++;
			continue;
		}

		//�ж�Ϊ'(',����'('ѹ��ջ
		//��'('ǰΪ���֣�����'('ǰ��һ��'*'�Ĵ���
		if(string[i]=='(')
		{
			if(string[i-1]<='0' || '9'<=string[i-1])//'('ǰ��Ϊ����
			{
			top++;
			stack[top].flag=1;  //��ʶ�ýڵ�Ϊ�����
			stack[top].oper='(';
			continue;
			}
			else      //'('ǰΪ����
			{         //��һ�ζ�'*'�Ĵ���
				while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
				{
				   bolan[j]=stack[top];
				   j++;
				   top--;
				}
			    //��'*'ѹ��ջstact��
		    	top++;
			    stack[top].flag=1;
			    stack[top].oper='*';

				//��'('ѹ��ջstact��
				top++;
				stack[top].flag=1;
				stack[top].oper='(';
				continue;
			}
		}

		//�ж�Ϊ')',��ջstact��������'('�Ժ��������ͺ������ε���
		//��������bolan��,Ȼ��������'('����
		if(string[i]==')')
		{
			while(stack[top].oper!='(')
			{
				bolan[j]=stack[top];
				j++;
				top--;
			}
			top--;  //��������'('����
			continue;
		}

		//�ж�Ϊ'-','+',�򽫵�ǰջstack��������'('�Ժ�����'('��
		//ջstack�����У���������ͺ������ε�������������bolan��,
		//Ȼ��'-'��'+'ѹ��ջstack��
		//���'-'�ڵ�һ��λ�û���ǰ��'('����'-'�Ǹ��ţ���'~'ѹ��stack��
		if(string[i]=='+' || string[i]=='-')
		{
			if(string[i]=='-'&&(i==0 || string[i-1]=='(')) //'-'�Ǹ���
			{
				top++;
				stack[top].flag=1;
				stack[top].oper='~';
				continue;
			}
			else             //'-','+'�ǼӼ�����
			{
				while(stack[top].oper!='(' && top!=0)
				{
					bolan[j]=stack[top]; //��ǰջstack�е��������
				                        //�������ε�����������bolan��
				    j++;
				    top--;
				}
				//��'-'��'+'ѹ��ջstack��
				top++;
				stack[top].flag=1;
				stack[top].oper=string[i];
				continue;
			}
		}

		//�ж�string[i]Ϊ'*'��'/',��ջstact�ж���������'*'��'/'��
		//�����ε�������������bolan��,Ȼ��'*'��'/'ѹ��ջstact��
		if(string[i]=='*' || string[i]=='/')
		{
			while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
			{
				bolan[j]=stack[top];
				j++;
				top--;
			}
			//��'*'��'/'ѹ��ջstact��
			top++;
			stack[top].flag=1;
			stack[top].oper=string[i];
			continue;
		}

		//�ж�string[i]Ϊ�������򽫸ú�����ջstact��
		//����ջstact�б���func���Ǹú����ı�ţ���ϸ��ż�ͷ�ļ�
		//���ú���ǰΪ���֣����ڸú�����ջstactǰ��һ�ζ�'*'�Ĵ���
		if(string[i]=='a')
		{
			if('0'<=string[i-1] && string[i-1]<='9')  //�ú���ǰΪ����
			{              //�ڸú�����ջstactǰ��һ�ζ�'*'�Ĵ���
				while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
				{
				   bolan[j]=stack[top];
				   j++;
				   top--;
				}
			    //��'*'ѹ��ջstact��
		    	top++;
			    stack[top].flag=1;
			    stack[top].oper='*';
			}
		 
		    //Ϊabs(x)����
		    if(seekStr(string,i,"abs"))   
			{                            //���ú�����ջstact��
				top++;
				stack[top].flag=2;
				stack[top].func=1;
				i--;                    //��Ϊ��seekStr������
				continue;               //i=i+strlen("abs");
				                        //i��ָ����abs����һ���ַ�
				                        //����for�������i++,���Դ˴�
				                        //iҪ��1
			}
			//Ϊacos(x)����
			if(seekStr(string,i,"acos"))   
			{                            //���ú�����ջstact��
				top++;
				stack[top].flag=2;
				stack[top].func=2;
				i--;                     
				continue;  
			}
			//Ϊasin(x)����
			if(seekStr(string,i,"asin"))   
			{                            //���ú�����ջstact��
				top++;
				stack[top].flag=2;
				stack[top].func=3;
				i--;                     
				continue;  
			}
			//Ϊatan(x)����
			if(seekStr(string,i,"atan"))   
			{                            //���ú�����ջstact��
				top++;
				stack[top].flag=2;
				stack[top].func=4;
				i--;                     
				continue;  
			}
			//Ϊacot(x)����
			if(seekStr(string,i,"acot"))   
			{                            //���ú�����ջstact��
				top++;
				stack[top].flag=2;
				stack[top].func=5;
				i--;                     
				continue;  
			}
		}//if(string[i]=='a')
		//string[i]Ϊ��c��ͷ�Ŀ⺯��
		if(string[i]=='c')
		{
			if('0'<=string[i-1] && string[i-1]<='9')  //�ú���ǰΪ����
			{              //�ڸú�����ջstactǰ��һ�ζ�'*'�Ĵ���
				while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
				{
				   bolan[j]=stack[top];
				   j++;
				   top--;
				}
			    //��'*'ѹ��ջstact��
		    	top++;
			    stack[top].flag=1;
			    stack[top].oper='*';
			}
			//Ϊcos(x)����
			if(seekStr(string,i,"cos"))   
			{                            //���ú�����ջstact��
				top++;
				stack[top].flag=2;
				stack[top].func=6;
				i--;                     
				continue;  
			}
			//Ϊcosh(x)����
			if(seekStr(string,i,"cosh"))   
			{                            //���ú�����ջstact��
				top++;
				stack[top].flag=2;
				stack[top].func=7;
				i--;                     
				continue;  
			}
			//Ϊcot(x)����
			if(seekStr(string,i,"cos"))   
			{                            //���ú�����ջstact��
				top++;
				stack[top].flag=2;
				stack[top].func=8;
				i--;                     
				continue;  
			}
		}//if(string[i]=='c')
		//string[i]Ϊ��e��ͷ�Ŀ⺯��
		if(string[i]=='e')
		{
			if('0'<=string[i-1] && string[i-1]<='9')  //�ú���ǰΪ����
			{              //�ڸú�����ջstactǰ��һ�ζ�'*'�Ĵ���
				while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
				{
				   bolan[j]=stack[top];
				   j++;
				   top--;
				}
			    //��'*'ѹ��ջstact��
		    	top++;
			    stack[top].flag=1;
			    stack[top].oper='*';
			}
			//Ϊexp(x)����
			if(seekStr(string,i,"exp"))   
			{                            //���ú�����ջstact��
				top++;
				stack[top].flag=2;
				stack[top].func=9;
				i--;                     
				continue;  
			}
		}//if(string[i]=='e')
		//string[i]Ϊ��f��ͷ�Ŀ⺯��
		if(string[i]=='f')
		{
			if('0'<=string[i-1] && string[i-1]<='9')  //�ú���ǰΪ����
			{              //�ڸú�����ջstactǰ��һ�ζ�'*'�Ĵ���
				while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
				{
				   bolan[j]=stack[top];
				   j++;
				   top--;
				}
			    //��'*'ѹ��ջstact��
		    	top++;
			    stack[top].flag=1;
			    stack[top].oper='*';
			}
			//Ϊexp(x)����
			if(seekStr(string,i,"floor"))   
			{                            //���ú�����ջstact��
				top++;
				stack[top].flag=2;
				stack[top].func=10;
				i--;                     
				continue;  
			}
		}//if(string[i]=='f')
		//string[i]Ϊ��m��ͷ�Ŀ⺯��
		if(string[i]=='m')
		{
			if('0'<=string[i-1] && string[i-1]<='9')  //�ú���ǰΪ����
			{              //�ڸú�����ջstactǰ��һ�ζ�'*'�Ĵ���
				while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
				{
				   bolan[j]=stack[top];
				   j++;
				   top--;
				}
			    //��'*'ѹ��ջstact��
		    	top++;
			    stack[top].flag=1;
			    stack[top].oper='*';
			}
			//Ϊmod(x)����
			if(seekStr(string,i,"mod"))   
			{                            //���ú�����ջstact��
				top++;
				stack[top].flag=2;
				stack[top].func=11;
				i--;                     
				continue;  
			}
		}//if(string[i]=='m')
		//string[i]Ϊ��l��ͷ�Ŀ⺯��
		if(string[i]=='l')
		{
			if('0'<=string[i-1] && string[i-1]<='9')  //�ú���ǰΪ����
			{              //�ڸú�����ջstactǰ��һ�ζ�'*'�Ĵ���
				while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
				{
				   bolan[j]=stack[top];
				   j++;
				   top--;
				}
			    //��'*'ѹ��ջstact��
		    	top++;
			    stack[top].flag=1;
			    stack[top].oper='*';
			}
			//Ϊln(x)����
			if(seekStr(string,i,"ln"))   
			{                            //���ú�����ջstact��
				top++;
				stack[top].flag=2;
				stack[top].func=12;
				i--;                     
				continue;  
			}
			//Ϊlog(x)����
			if(seekStr(string,i,"log"))   
			{                            //���ú�����ջstact��
				top++;
				stack[top].flag=2;
				stack[top].func=13;
				i--;                     
				continue;  
			}
		}//if(string[i]=='l')
		//string[i]Ϊ��p��ͷ�Ŀ⺯��
		if(string[i]=='p')
		{
			if('0'<=string[i-1] && string[i-1]<='9')  //�ú���ǰΪ����
			{              //�ڸú�����ջstactǰ��һ�ζ�'*'�Ĵ���
				while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
				{
				   bolan[j]=stack[top];
				   j++;
				   top--;
				}
			    //��'*'ѹ��ջstact��
		    	top++;
			    stack[top].flag=1;
			    stack[top].oper='*';
			}
			//Ϊpow(x)����
			if(seekStr(string,i,"pow"))   
			{                            //���ú�����ջstact��
				top++;
				stack[top].flag=2;
				stack[top].func=14;
				i--;                     
				continue;  
			}
		}//if(string[i]=='p')
		//string[i]Ϊ��s��ͷ�Ŀ⺯��
		if(string[i]=='s')
		{
			if('0'<=string[i-1] && string[i-1]<='9')  //�ú���ǰΪ����
			{              //�ڸú�����ջstactǰ��һ�ζ�'*'�Ĵ���
				while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
				{
				   bolan[j]=stack[top];
				   j++;
				   top--;
				}
			    //��'*'ѹ��ջstact��
		    	top++;
			    stack[top].flag=1;
			    stack[top].oper='*';
			}
			//Ϊsin(x)����
			if(seekStr(string,i,"sin"))   
			{                            //���ú�����ջstact��
				top++;
				stack[top].flag=2;
				stack[top].func=15;
				i--;                     
				continue;  
			}
			//Ϊsinh(x)����
			if(seekStr(string,i,"sinh"))   
			{                            //���ú�����ջstact��
				top++;
				stack[top].flag=2;
				stack[top].func=16;
				i--;                     
				continue;  
			}
			//Ϊsin(x)����
			if(seekStr(string,i,"sqrt"))   
			{                            //���ú�����ջstact��
				top++;
				stack[top].flag=2;
				stack[top].func=17;
				i--;                     
				continue;  
			}
		}//if(string[i]=='s')
		//string[i]Ϊ��t��ͷ�Ŀ⺯��
		if(string[i]=='t')
		{
			if('0'<=string[i-1] && string[i-1]<='9')  //�ú���ǰΪ����
			{              //�ڸú�����ջstactǰ��һ�ζ�'*'�Ĵ���
				while(stack[top].oper=='*' || stack[top].oper=='/'||
				stack[top].flag==2)
				{
				   bolan[j]=stack[top];
				   j++;
				   top--;
				}
			    //��'*'ѹ��ջstact��
		    	top++;
			    stack[top].flag=1;
			    stack[top].oper='*';
			}
			//Ϊtan(x)����
			if(seekStr(string,i,"tan"))   
			{                            //���ú�����ջstact��
				top++;
				stack[top].flag=2;
				stack[top].func=18;
				i--;                     
				continue;  
			}
			//Ϊtanh(x)����
			if(seekStr(string,i,"tanh"))   
			{                            //���ú�����ջstact��
				top++;
				stack[top].flag=2;
				stack[top].func=19;
				i--;                     
				continue;  
			}
		}//if(string[i]=='p')
	    
		//�ж�string[i]=',',��ջstact��'('���������ͺ���ȫ��������
		//ѹ������bolan�У�
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

		//�ж�string[i]='x',��������δ֪������xx���棬ѹ������bolan�� 
		if(string[i]=='x')
		{
			bolan[j].flag=0;   //��ʶΪ��ֵ
			bolan[j].value=xx;
			j++;
		}
    }

	//ת������ʱ����ջstact��Ϊ�գ���ջ������������ͺ�������������
	//����bolan��
	while(top!=0)
	{
		bolan[j]=stack[top];
		j++;
		top--;
	}

	//ת��������������bolanβ��һopera='#'��Ϊ������
    bolan[j].oper='#';

    return 1;
}

//�������ַ���ת������ֵ
double CMathString::stringToDigital(char *s)
{
	double sum=0,    //ת�������ֵ
		temp,
		ten=0.1;     //С���仯ֵ
	int i,ch,
		flag=0;    //��ʶ��С������������0Ϊ����
	for(i=0;s[i]!='\0';i++)
	{
		if(s[i]!='.')
		{
			if(flag==0)  //��������
			{
				ch=s[i]-'0';
				sum=sum+ch;
				sum=sum*10;
			}
			else        //С������
			{
				ch=s[i]-'0';
				temp=ch*ten;   //С������λ
				sum=sum+temp;
				ten=ten*0.1;    //�ı�С����λ��
			}
		}
		else     //С����
		{
			sum=sum/10;
			flag=1;
		}
	}//for
	if(flag==0)    //��С������
		sum=sum/10;
	return sum;

}

//����ʽbolan��ֵ
//�������£�
//1.���ڵ�bolan[i]Ϊ��ֵ����������ջdataStact
//2.���ڵ�bolan[i]Ϊ��������������ջdataStact�������ݽ��м��㣬��
//  �����ѹ������ջdataStact��
//3.���ڵ�bolan[i]Ϊ�������������ջdataStact�������ݣ�������Ӧ�Ŀ�
//  �������м��㣬�������ѹ������ջdataStact��
//4.���ڵ�bolan[i]Ϊ������'#',������ջdataStact�е����ݵ���������
//  result��������0
//����ֵ:
//1. ������ȷ������0
//2. �ڼ�����������Ϊ0������1
//3. �����Һ���acos(x)�е�x����������������2
//4. �����Һ���asin(x)�е�x����������������3
//5. ���к���cot(x) ��tan��x��Ϊ0������4
//6.ȡģx%y����mod(x,y)��yΪ0������5
//7.��Ȼ��������ln(x),���x<=0,�򷵻�6
//8.ȡ10�Ķ�������log10(x),���x<=0,�򷵻�7
//9.��������sqrt(x),���x<0,�򷵻�8
//10.���������������󣬷���-1
int CMathString::compvalue(void)
{
	double  dataStack[MAX/2];   //����м����ݵ�����ջ
	int top=0,            //����ջdataStact��ջ��
		i;              //����bolan���±�
	for(i=0;bolan[i].oper!='#';i++)
	{
		//�ڵ�bolan[i]Ϊ��ֵ����������ջdataStact
		if(bolan[i].flag==0)
		{
			top++;
			dataStack[top]=bolan[i].value;
			continue;
		}

		//�ڵ�bolan[i]Ϊ��������������ջdataStact�������ݽ��м��㣬��
        //�����ѹ������ջdataStact��
		//�ڼ�����������Ϊ0������1
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
				else  //����Ϊ0������1
				{
					return 1;
				}
			case '~':      //ȡ��
				dataStack[top]=0-dataStack[top];
				break;
			}//switch
			continue;
		}//if(bolan[i].flag==1)

		//���ڵ�bolan[i]Ϊ�������������ջdataStact�������ݣ�������Ӧ
		//�Ŀ⺯�����м��㣬�������ѹ������ջdataStact��
		if(bolan[i].flag==2)
		{
			switch(bolan[i].func)
			{
			case 1:    //�����ֵ����abs(x)
				dataStack[top]=fabs(dataStack[top]);
				break;

			case 2:    //�����Һ���acos(x)
				if(-1.0<=dataStack[top] && dataStack[top]<=1.0)
				{
				    dataStack[top]=acos(dataStack[top]);
				    break;
				}
				else  //�����Һ���acos(x)�е�x����������������2
				{
					return 2;
				}

			case 3:   //�����Һ���asin(x)
				if(-1<=dataStack[top] && dataStack[top]<=1)
				{
				    dataStack[top]=asin(dataStack[top]);
				    break;
				}
				else  //�����Һ���asin(x)�е�x����������������3
				{
					return 3;
				}

			case 4:   //�����к���atan(x)
				dataStack[top]=atan(dataStack[top]);
				break;

			case 5:  //�����к���acot(x)
				dataStack[top]=PI/2-atan(dataStack[top]);
				break;

			case 6:   //���Һ���cos(x)
				dataStack[top]=cos(dataStack[top]);
				break;
               
			case 7:    //˫������cosh(x)
				dataStack[top]=cosh(dataStack[top]);
				break;

			case 8:    //���к���cot(x)   cot(x)=1/tan(x)
				if(tan(dataStack[top])!=0)
				{
					dataStack[top]=1/tan(dataStack[top]);
					break;
				}
				else    //���к���cot(x) ��tan��x��Ϊ0������4
				{
					return 4;
				}
				
			case 9:    //e��x�η�����exp(x)
				dataStack[top]=exp(dataStack[top]);
				break;

			case 10:   //�󲻴���x��������� floor(x)
				dataStack[top]=floor(dataStack[top]);
				break;

			case 11:   //ȡģx%y����mod(x,y),��
				if(dataStack[top]!=0)
				{
				   dataStack[top-1]=fmod(dataStack[top-1],dataStack[top]);
				   top--;
				   break;
				}
				else   //ȡģx%y����mod(x,y)��yΪ0������5
				{
					return 5;
				}

			case 12:    //��Ȼ��������ln(x),���x<=0,�򷵻�6
				if(dataStack[top]>0)
				{          //ln(x)�Ŀ⺯��Ϊlog(x)
					dataStack[top]=log(dataStack[top]);
					break;
				}
				else
				{
					return 6;
				}

			case 13:    //ȡ10�Ķ�������log10(x),���x<=0,�򷵻�7
				if(dataStack[top]>0)
				{          //log10(x)�Ŀ⺯��Ϊlog10(x)
					dataStack[top]=log10(dataStack[top]);
					break;
				}
				else
				{
					return 7;
				}

			case 14:    //x��y�η�����pow(x,y)
				dataStack[top-1]=pow(dataStack[top-1],dataStack[top]);
				top--;
				break;

			case 15:    //���Һ���sin(x)
				dataStack[top]=sin(dataStack[top]);
				break;

			case 16:    //˫������sinh(x)
				dataStack[top]=sinh(dataStack[top]);
				break;

			case 17:   //��������sqrt(x),���x<0,�򷵻�8
				if(dataStack[top]>=0)
				{
					dataStack[top]=sqrt(dataStack[top]);
					break;
				}
				else
				{
					return 8;
				}

			case 18:    //���к���tan(x)
				dataStack[top]=tan(dataStack[top]);
				break;

			case 19:    //˫�����к���tanh(x)
				dataStack[top]=tanh(dataStack[top]);
				break;
            }//switch(bolan[i].func)
			continue;
		}//if(bolan[i].flag==2)
	}//for(i=0;bolan[i].oper!='#';i++)
	if(top==1)       //������ȷ
	{
		result=dataStack[top];
		return 0;    //����0
	}
	else           //���������������󣬷���-1
		return -1;   
}

double CMathString::getvalue(void)  //���ؼ���õ��ı��ʽֵ
{
	return result;
}



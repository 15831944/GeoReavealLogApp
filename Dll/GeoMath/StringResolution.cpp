// StringResolution.cpp: implementation of the CStringResolution class.
/***********************************************************************************************
*	Copyright (c)
*	All rights reserved EXCEPT as allow by the following statements:
*	You can freely use this file for your own work (personal or commercial).
*	But,you cannot remove this copyright and notice. 
*	You cannot distribute modified versions of the source code in this package. 
*	You cannot use this file in printed media without the express permission of the author.
*	Zhou Yuncai makes no representation about the suitability of this software for any purpose.
*	It is provided "as is" without express or implied warranty of any kind, including any implied
*	warranty of merchantability, fitness for a particular purpose, or non-infringement.
*	The entire risk as to the quality and performance of the software is with you. 
*	Zhou Yuncai shall not be liable for any damages suffered by you or any third party as a result of
*	using or distributing this software. 
*	In no event will Zhou Yuncai be liable for any lost revenue, profit, or data, or for direct,
*	indirect, special, consequential, incidental, or punitive damages, however caused and regardless of
*	the theory of liability, arising out of the use of or inability to use software, even if Zhou Yuncai
*	has been advised of the possibility of such damages. 
*	Should the software prove defective, you assume the cost of all necessary servicing, repair, 
*	or correction. If you think you've found an error, please e-mail the correction using the 
*	form you will find to zyc262@163.net
**********************************************************************************************/

#include "stdafx.h"
#include "StringResolution.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef _ERROR_INFO_H_
BEGIN_CATCH_MATH_ERROR
_BEGIN_GET_TYPE
GET_ERROR_TYPE(_DOMAIN,BIGNUMBER,Argument domain error.)
GET_ERROR_TYPE(_SING,BIGNUMBER,Argument singularity.)
GET_ERROR_TYPE(_OVERFLOW,BIGNUMBER,Overflow range error.)
GET_ERROR_TYPE(_UNDERFLOW,0,Underflow range error.)
GET_ERROR_TYPE(_TLOSS,0,Total loss of precision.)
GET_ERROR_TYPE(_PLOSS,0,Partial loss of precision.)
_END_GET_TYPE
GET_ERROR_FUNCTION(pow,power function exception. )
GET_ERROR_FUNCTION(tan,tangent function exception. )
GET_ERROR_FUNCTION(asin,arcsine function exception. )
GET_ERROR_FUNCTION(acos,arccosine function exception. )
END_CATCH_MATH_ERROR

IMPLEMENT_CATCH_ERROR(CStringResolution)
#endif

CStringResolution::CStringResolution(char*formula)
:m_oprandNum(0),m_oprandANum(0),m_operatorNum(0)
{
	m_operator=0;
	m_oprand=0;
	m_oprandA=0;
#ifdef _ERROR_INFO_H_
	_SAVE_ERROR_INFO(0,FALSE,"Good");
#endif
	if(formula)
	{
		m_formula=formula;
		Initialize();
	}
}

CStringResolution::~CStringResolution()
{
	if(m_operator)
		delete[]m_operator;
	if(m_oprand)
		delete[]m_oprand;
	if(m_oprandA)
		delete[]m_oprandA;
}

void CStringResolution::SetFormula(char *formula)
{
	if(!formula)
		return;
	m_formula=formula;
	if(m_operator)
	{
		delete[]m_operator;
		m_operator=0;
	}
	if(m_oprand)
	{
		delete[]m_oprand;
		m_oprand=0;
	}
	if(m_oprandA)
	{
		delete[]m_oprandA;
		m_oprandA=0;
	}
	m_oprandNum=0,m_oprandANum=0,m_operatorNum=0;
#ifdef _ERROR_INFO_H_
	_SAVE_ERROR_INFO(0,FALSE,"Good!");
#endif
	Initialize();
}
int CStringResolution::IsFormula()
{
	int bRet=0;
	stack<char> charStack;
	int i,num=m_formula.GetLength();
	char notChar[]={';','\'',':',' ','[',']','{','}','\\','|',',','\"','=','&','%','$','@','#','!','`','~','?'};
	int notNum=sizeof notChar;
	for(i=0;i<num;i++)
	{
		for(int j=0;j<notNum;j++)
		{
			if(m_formula[i]==notChar[j])
			{
				CString s;
				s.Format("The %dth character,\"%c\" is invalidated!",i+1,m_formula[i]);
#ifdef _ERROR_INFO_H_
				_SAVE_ERROR_INFO(-i-1,TRUE,s);
#endif
				return -i-1;//the ith symbol is invalidating.
			}
		}
	}
	for(i=0;i<num;i++)
	{
		if(m_formula[i]=='(')
			charStack.push('(');
		if(m_formula[i]==')')
		{
			if(charStack.empty())
			{
#ifdef _ERROR_INFO_H_
				_SAVE_ERROR_INFO(1,TRUE,"There are some more right brackets!");
#endif
				return 1;//right bracket if more.
			}
			else
				charStack.pop();
		}
	}
	if(!charStack.empty())
	{
#ifdef _ERROR_INFO_H_
		_SAVE_ERROR_INFO(2,TRUE,"There are some more left brackets!");
#endif
		return 2;//left bracket is more.
	}
	return bRet;
}

BOOL CStringResolution::GetStack(OperatorStack &Operator, OperandStack &Oprand)
{
	if(IsFormula())
		return FALSE;//illogicality return
	BOOL bRet=TRUE;
	EmptyStack(Operator);
	EmptyStack(Oprand);
	CString string=m_formula;
	bRet=bRet&&GetOperatorStack(Operator,string);
	bRet=bRet&&GetOperandStack(Oprand,string);
	return bRet;
}
BOOL CStringResolution::Initialize()
{
	BOOL bRet=FALSE;
	OperatorStack Op; OperandStack Od;
	bRet=GetStack(Op,Od);
	if(!bRet)
	{
		EmptyStack(Op);
		EmptyStack(Od);
		return FALSE;
	}
	m_oprandNum=Od.size(),m_operatorNum=Op.size();
	if(m_operatorNum)
		m_operator=new COperator[m_operatorNum];
	if(m_oprandNum)
		m_oprand=new COperand[m_oprandNum];
	int i;
	for(i=0;i<m_operatorNum;i++)
	{
		COperator*op=Op.top();
		m_operator[m_operatorNum-i-1]=*op;
		Op.pop();
		delete op;
	}
	for(i=0;i<m_oprandNum;i++)
	{
		COperand*od=Od.top();
		m_oprand[m_oprandNum-i-1]=*od;
		Od.pop();
		delete od;
	}
	m_oprandA=new COperand[m_oprandNum];
	BOOL IsIn=FALSE;
	for(i=0;i<m_oprandNum;i++)
	{
		for(int j=0;j<m_oprandANum;j++)
		{
			if(m_oprand[i]==m_oprandA[j])
			{
				IsIn=TRUE;
				break;
			}
		}
		if(!IsIn&&!m_oprand[i].m_IsConst)
		{
			m_oprandA[m_oprandANum]=m_oprand[i];
			m_oprandANum++;
		}
		IsIn=FALSE;
	}
	return TRUE;
}

double CStringResolution::Computer(OperatorStack &Operator, OperandStack &Oprand)
{
	double value =0.0;
	if(Operator.empty())
	{
		if(Oprand.size()==1)
		{	
			COperand* od=Oprand.top();
			value=atof(od->m_name);
			delete od;
			return value;
		}
		else
		{
			while(!Oprand.empty())
			{
				COperand*od=Oprand.top();
				Oprand.pop();
				delete od;
			}
#ifdef _ERROR_INFO_H_
			_SAVE_ERROR_INFO(12,TRUE,"Oprand is less or more!");
#endif
			return BIGNUMBER;
		}
	}
	OperatorStack tmpOO;
	OperandStack tmpOD;
	COperator* op=0,*op1=0;
	COperand *oprand=0,*oprand1=0;
	op=Operator.top();
	Operator.pop();
	if(!Operator.empty())
		op1=Operator.top();
	while(op1&&(op1->m_level>op->m_level))
	{
		tmpOO.push(op);
		if(op->m_type==BINARY)
		{
			if(!Oprand.empty())
			{
				oprand=Oprand.top();
				Oprand.pop();
				tmpOD.push(oprand);
			}
		}
		op=op1;
		Operator.pop();
		if(!Operator.empty())
			op1=Operator.top();
		else
			op1=0;
	}
	if(op->m_type==UNARY)
	{
		if(Oprand.empty())
		{
#ifdef _ERROR_INFO_H_
			_SAVE_ERROR_INFO(12,TRUE,"Oprand is less!");
#endif
			return BIGNUMBER;
		}
		oprand=Oprand.top();
		double x=computing(op,oprand);
		oprand->m_name.Format("%g",x);
	}
	else
	{
		if(Oprand.empty())
		{
#ifdef _ERROR_INFO_H_
			_SAVE_ERROR_INFO(12,TRUE,"Oprand less!");
#endif
			return BIGNUMBER;
		}
		oprand1=Oprand.top();
		Oprand.pop();
		if(Oprand.empty())
		{
#ifdef _ERROR_INFO_H_
			_SAVE_ERROR_INFO(12,TRUE,"Oprand less!");
#endif
			return BIGNUMBER;
		}
		oprand=Oprand.top();
		double x=computing(op,oprand1,oprand);
		oprand->m_name.Format("%g",x);
		delete oprand1;
	}
	delete op;
	while(!tmpOO.empty())
	{
		op=tmpOO.top();
		tmpOO.pop();
		Operator.push(op);
	}
	while(!tmpOD.empty())
	{
		oprand=tmpOD.top();
		tmpOD.pop();
		Oprand.push(oprand);
	}
	return Computer(Operator,Oprand);
}

double CStringResolution::computer(double variantValue[], int num)
{
#ifdef _ERROR_INFO_H_
	PROCESSION_ERROR
#endif
	double value=0.0;
	int i;
	if(num<m_oprandANum)
	{
#ifdef _ERROR_INFO_H_
		_SAVE_ERROR_INFO(10,FALSE,"There are less digital number than variants");
#endif
		return BIGNUMBER;
	}
	OperatorStack Operator;
	OperandStack Oprand;
	for(i=0;i<m_operatorNum;i++)
	{
		COperator*op=new COperator;
		*op=m_operator[m_operatorNum-i-1];
		Operator.push(op);
	}
	for(i=0;i<m_oprandNum;i++)
	{
		COperand*od=new COperand;
		*od=m_oprand[m_oprandNum-i-1];
		for(int j=0;j<m_oprandANum;j++)
		{
			if(*od==m_oprandA[j])
				od->m_name.Format("%g",variantValue[j]);
		}
		Oprand.push(od);
	}
	value=Computer(Operator,Oprand);
	return value;
}

CString CStringResolution::GetErrorInformation()
{
	CString value;
#ifndef _ERROR_INFO_H_
	value="There is no error information.";
#else
	GET_ERROR_INFORMATION(value)
#endif
	return value;
}

BOOL CStringResolution::EmptyStack(OperatorStack Op)
{
	while(!Op.empty())
	{
		COperator*op=Op.top();
		Op.pop();
		delete op;
	}
	return TRUE;
}
BOOL CStringResolution::EmptyStack(OperandStack Od)
{
	while(!Od.empty())
	{
		COperand*od=Od.top();
		Od.pop();
		delete od;
	}
	return TRUE;
}

BOOL CStringResolution::GetOperatorStack(OperatorStack &Operator, CString &string)
{
	BOOL bRet=TRUE;
	int num=string.GetLength();
	int i=0;
	int level=0;
	while(i<num)
	{
		if(string[i]=='(')
		{
			level+=LEVELS;
			string.SetAt(i,'#');
			i++;
		}
		else if(string[i]==')')
		{
			level-=LEVELS;
			string.SetAt(i,'@');
			i++;
		}
		else if(string[i]=='+'||string[i]=='-')
		{
			COperator* op=new COperator;
			op->m_level=level;
			op->m_operator=string.Mid(i,1);
			op->m_startIndex=i;
			if(i==0||string[i-1]=='#')
				op->m_type=UNARY;
			else
				op->m_type=BINARY;
			Operator.push(op);
			string.SetAt(i,'@');
			i++;
		}
		else if(string[i]=='*'||string[i]=='/')
		{
			COperator* op=new COperator;
			op->m_level=level+1;
			op->m_operator=string.Mid(i,1);
			op->m_startIndex=i;
			op->m_type=BINARY;
			Operator.push(op);
			string.SetAt(i,'@');
			i++;
		}
		else if(string[i]=='^')
		{
			COperator* op=new COperator;
			op->m_level=level+2;
			op->m_operator=string.Mid(i,1);
			op->m_startIndex=i;
			op->m_type=BINARY;
			Operator.push(op);
			string.SetAt(i,'@');
			i++;
		}
		else if(string.Mid(i,4)=="sqrt")
		{
			COperator* op=new COperator;
			op->m_level=3+level;
			op->m_operator="sqrt";
			op->m_startIndex=i;
			op->m_type=UNARY;
			Operator.push(op);
			string.SetAt(i,'@');string.SetAt(i+1,'@');string.SetAt(i+2,'@');string.SetAt(i+3,'@');
			i+=3;
		}
		else if(string.Mid(i,3)=="log")
		{
			COperator* op=new COperator;
			op->m_level=3+level;
			op->m_operator="log";
			op->m_startIndex=i;
			op->m_type=UNARY;
			Operator.push(op);
			string.SetAt(i,'@');string.SetAt(i+1,'@');string.SetAt(i+2,'@');
			i+=3;
		}
		else if(string.Mid(i,2)=="ln")
		{
			COperator* op=new COperator;
			op->m_level=3+level;
			op->m_operator="ln";
			op->m_startIndex=i;
			op->m_type=UNARY;
			Operator.push(op);
			string.SetAt(i,'@');string.SetAt(i+1,'@');string.SetAt(i+2,'@');
			i+=3;
		}
		else if(string.Mid(i,3)=="exp")
		{
			COperator* op=new COperator;
			op->m_level=3+level;
			op->m_operator="exp";
			op->m_startIndex=i;
			op->m_type=UNARY;
			Operator.push(op);
			string.SetAt(i,'@');string.SetAt(i+1,'@');string.SetAt(i+2,'@');
			i+=3;
		}
		else if(string.Mid(i,3)=="sin")
		{
			COperator* op=new COperator;
			op->m_level=3+level;
			op->m_operator="sin";
			op->m_startIndex=i;
			op->m_type=UNARY;
			Operator.push(op);
			string.SetAt(i,'@');string.SetAt(i+1,'@');string.SetAt(i+2,'@');
			i+=3;
		}
		else if(string.Mid(i,3)=="cos")
		{
			COperator* op=new COperator;
			op->m_level=3+level;
			op->m_operator="cos";
			op->m_startIndex=i;
			op->m_type=UNARY;
			Operator.push(op);
			string.SetAt(i,'@');string.SetAt(i+1,'@');string.SetAt(i+2,'@');
			i+=3;
		}
		else if(string.Mid(i,3)=="tan")
		{
			COperator* op=new COperator;
			op->m_level=3+level;
			op->m_operator="tan";
			op->m_startIndex=i;
			op->m_type=UNARY;
			Operator.push(op);
			string.SetAt(i,'@');string.SetAt(i+1,'@');string.SetAt(i+2,'@');
			i+=3;
		}
		else if(string.Mid(i,3)=="cot")
		{
			COperator* op=new COperator;
			op->m_level=3+level;
			op->m_operator="cot";
			op->m_startIndex=i;
			op->m_type=UNARY;
			Operator.push(op);
			string.SetAt(i,'@');string.SetAt(i+1,'@');string.SetAt(i+2,'@');
			i+=3;
		}
		else if(string.Mid(i,4)=="asin")
		{
			COperator* op=new COperator;
			op->m_level=3+level;
			op->m_operator="asin";
			op->m_startIndex=i;
			op->m_type=UNARY;
			Operator.push(op);
			string.SetAt(i,'@');string.SetAt(i+1,'@');string.SetAt(i+2,'@');string.SetAt(i+3,'@');
			i+=3;
		}
		else if(string.Mid(i,4)=="acos")
		{
			COperator* op=new COperator;
			op->m_level=3+level;
			op->m_operator="acos";
			op->m_startIndex=i;
			op->m_type=UNARY;
			Operator.push(op);
			string.SetAt(i,'@');string.SetAt(i+1,'@');string.SetAt(i+2,'@');string.SetAt(i+3,'@');
			i+=3;
		}
		else if(string.Mid(i,4)=="atan")
		{
			COperator* op=new COperator;
			op->m_level=3+level;
			op->m_operator="atan";
			op->m_startIndex=i;
			op->m_type=UNARY;
			Operator.push(op);
			string.SetAt(i,'@');string.SetAt(i+1,'@');string.SetAt(i+2,'@');string.SetAt(i+3,'@');
			i+=3;
		}
		else
			i++;
	}
	return bRet;
}

BOOL CStringResolution::GetOperandStack(OperandStack &Oprand, CString &string)
{
	int i=0;
	int num=string.GetLength();
	while(i<num)
	{
		if(string[i]=='#'||string[i]=='@')
			i++;
		else
		{
			COperand * oprand=new COperand;
			if(isdigit(string[i]))
				oprand->m_IsConst=TRUE;
			oprand->m_startIndex=i;
			BOOL HasPoint=FALSE;
			while(i<num&&string[i]!='#'&&string[i]!='@')
			{
				if(string[i]=='.')
				{
					if(HasPoint)
					{
#ifdef _ERROR_INFO_H_
						_SAVE_ERROR_INFO(11,TRUE,"There are more than one point continuously!");
#endif
						return FALSE;
					}
					else
						HasPoint=TRUE;
				}
				if(oprand->m_IsConst&&!isdigit(string[i])&&(string[i]!='.'))
				{
					CString s;
					s.Format("Variant cann't start with digital (%d in expression)!",i+1-oprand->m_name.GetLength());
#ifdef _ERROR_INFO_H_
					_SAVE_ERROR_INFO(11,TRUE,s);
#endif
					return FALSE;
				}
				oprand->m_name+=string.Mid(i,1);
				i++;
			}
			Oprand.push(oprand);
		}
	}
	return TRUE;
}


int CStringResolution::GetErrorNumber()
{
#ifdef _ERROR_INFO_H_
	GET_ERROR_NUMBER
#else
	return 0;
#endif
}
double CStringResolution::computing(const COperator*op,const COperand*oprand)
{
	double x=atof(oprand->m_name);
	if(op->m_operator=="-")
	{
		x=-x;
	}
	else if(op->m_operator=="log")
	{
		if(x>0)
			x=log10(x);
		else
		{
#ifdef _ERROR_INFO_H_
			_SAVE_ERROR_INFO(4,oprand->m_IsConst,"log function exception. Argument domain error.");
#endif
			x=BIGNUMBER;
		}
	}
	else if(op->m_operator=="ln")
	{
		if(x>0)
			x=log(x);
		else
		{
#ifdef _ERROR_INFO_H_
			_SAVE_ERROR_INFO(4,oprand->m_IsConst,"lg function exception. Argument domain error.");
#endif
			x=BIGNUMBER;
		}
	}
	else if(op->m_operator=="exp")
		x=exp(x);
	else if(op->m_operator=="sqrt")
	{
		if(x>=0)
			x=sqrt(x);
		else
		{
#ifdef _ERROR_INFO_H_
			_SAVE_ERROR_INFO(5,oprand->m_IsConst,"Negative has no square root!");
#endif
			x=BIGNUMBER;
		}
	}
	else if(op->m_operator=="sin")
		x=sin(x);
	else if(op->m_operator=="cos")
		x=cos(x);
	else if(op->m_operator=="tan")
	{
		x=tan(x);
#ifdef _ERROR_INFO_H_
		_CLEAR_ERROR(oprand->m_IsConst);
#endif
	}
	else if(op->m_operator=="cot")
	{
		if(fabs(sin(x))>DERROR)
			x=cos(x)/sin(x);
		else
		{
#ifdef _ERROR_INFO_H_
			_SAVE_ERROR_INFO(7,oprand->m_IsConst,"Cotangent fonction exception!");
#endif
			x=BIGNUMBER;
		}
	}
	else if(op->m_operator=="asin")
	{
		x=asin(x);
#ifdef _ERROR_INFO_H_
		_CLEAR_ERROR(oprand->m_IsConst);
#endif
	}
	else if(op->m_operator=="acos")
	{
		x=acos(x);
#ifdef _ERROR_INFO_H_
		_CLEAR_ERROR(oprand->m_IsConst);
#endif
	}
	else if(op->m_operator=="atan")
		x=atan(x);
	return x;
}
double CStringResolution::computing(const COperator*op,const COperand*Loprand,const COperand*Roprand)
{
	double x=BIGNUMBER;
	switch(op->m_operator[0])
	{
	case '+':
		x=atof(Loprand->m_name)+atof(Roprand->m_name);
		break;
	case '-':
		x=atof(Loprand->m_name)-atof(Roprand->m_name);
		break;
	case '*':
		x=atof(Loprand->m_name)*atof(Roprand->m_name);
		break;
	case '/':
		{
			double y=atof(Roprand->m_name);
			x=atof(Loprand->m_name);
			if(fabs(y)>DERROR)
				x=x/y;
			else
			{
#ifdef _ERROR_INFO_H_
				_SAVE_ERROR_INFO(9,Roprand->m_IsConst,"Divisor is zero!");
#endif
				x=BIGNUMBER;
			}
		}
		break;
	case '^':
		{
			double y=atof(Roprand->m_name);
			x=atof(Loprand->m_name);
			x=pow(x,y);
#ifdef _ERROR_INFO_H_
			_CLEAR_ERROR(Roprand->m_IsConst);
#endif
		}
		break;
	}
	return x;
}

CString CStringResolution::GetDigitalString(double* variantValue,int num)
{
	CString value=m_formula;
	if(num>=m_oprandANum)
	{
		int i,j;
		CString* ppchar;
		ppchar=new CString[m_oprandNum+1];
		for(i=0;i<=m_oprandNum;i++)
		{
			if(i==0)
				ppchar[0]=m_formula.Left(m_oprand[0].m_startIndex);
			else if(i<m_oprandNum)
			{
				WORD start=m_oprand[i-1].m_startIndex+m_oprand[i-1].m_name.GetLength();
				WORD length=m_oprand[i].m_startIndex-start;
				ppchar[i]=m_formula.Mid(start,length);
			}
			else
			{
				WORD start=m_oprand[i-1].m_startIndex+m_oprand[i-1].m_name.GetLength();
				ppchar[i]=m_formula.Mid(start);
			}
		}
		CString* digitchar=new CString[m_oprandNum];
		for(i=0;i<m_oprandNum;i++)
		{
			if(m_oprand[i].m_IsConst)
				digitchar[i]=m_oprand[i].m_name;
			else
			{
				for(j=0;j<m_oprandANum;j++)
				{
					if(m_oprand[i]==m_oprandA[j])
					{
						if(variantValue[j]<0)
							digitchar[i].Format("(%g)",variantValue[j]);
						else
							digitchar[i].Format("%g",variantValue[j]);
					}
				}
			}
		}
		value=ppchar[0];
		for(i=0;i<m_oprandNum;i++)
			value+=digitchar[i]+ppchar[i+1];
		delete[]ppchar;
		delete[]digitchar;
	}
	return value;
}

BOOL CStringResolution::ExpressionIsError()
{
#ifndef _ERROR_INFO_H_
	return 0;
#else
	GET_EXPRESSION_ERROR 
#endif
}
const COperand* CStringResolution::GetVariantTable()
{
	return m_oprandA;
}

COperator::COperator():m_startIndex(0),m_level(0),m_type(BINARY)
{
}

COperator::~COperator()
{
}

COperand::COperand():m_startIndex(0)
{
	m_IsConst=FALSE;
}
COperand::~COperand()
{
}
BOOL COperand::operator ==(const COperand & od)
{
	BOOL bRet=TRUE;
	bRet=(m_IsConst==od.m_IsConst);
	bRet=(bRet&&(m_name==od.m_name));
	return bRet;
}

void CStringResolution::Destroy()
{
	if(m_operator)
	{
		delete[]m_operator;
		m_operator=0;
	}
	if(m_oprand)
	{
		delete[]m_oprand;
		m_oprand=0;
	}
	if(m_oprandA)
	{
		delete[]m_oprandA;
		m_oprandA=0;
	}
	m_oprandNum=0,m_oprandANum=0,m_operatorNum=0;
}

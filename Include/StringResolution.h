// StringResolution.h: interface for the CStringResolution class.
/***********************************************************************************************
*	Copyright (c) Zhou Yuncai(周云才 江汉石油学院计算机系),2002
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
*	address:湖北省荆州市江汉石油学院计算机系
**********************************************************************************************/
#include "my.h"//error process,otherwise no compile error,but no error information

#if !defined(AFX_STRINGRESOLUTION_H__69522ED3_1E49_404E_8D5E_1261F7D732DD__INCLUDED_)
#define AFX_STRINGRESOLUTION_H__69522ED3_1E49_404E_8D5E_1261F7D732DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <stack>
using namespace std;

#define LEVELS 6
#define DERROR 0.0000001
#define BIGNUMBER 9999999.0
#define UNARY 0
#define BINARY 1

class COperator  
{
public:
	COperator();
	~COperator();
	CString m_operator;//operator name
	WORD m_level;//+,- : 0 level; *,/ :1 level; ^: 2 level; function：3 level. 
				//level add LEVELS if operator in a bracket.
	WORD m_type;//0--unary operator, 1--binary operator,...
	WORD m_startIndex;// start index which in formula
};

class COperand  
{
public:
	COperand();
	~COperand();
	BOOL operator ==(const COperand&od);//judge two operand are equal or not
	CString m_name;//digital string or variant name
	BOOL m_IsConst;//TRUE when it is digital, FALSE otherwise
	WORD m_startIndex;//start index which in formula
};

typedef stack<COperator*> OperatorStack;
typedef stack<COperand*> OperandStack;

class AFX_EXT_CLASS CStringResolution  
{
public:
	void Destroy();
	BOOL ExpressionIsError();//TRUE when expression has errors,FALSE otherwise.But when no include my.h,always FALSE.
	CString GetDigitalString(double* variantValue=0,int num=-1);//replace variant as it's value
	int GetErrorNumber();//get error number,always 0 when no include my.h
	CString GetErrorInformation();//get the error description, 
								//always "There is no error information." when no include my.h
	double computer(double variantValue[],int num);//evaluate variant(s) in m_oprandA by variantValue,then computing.
	void SetFormula(char*formula);//reset formula
	CStringResolution(char*formula=0);//constructor
	int GetVariantTableSize(){return m_oprandANum;}//get variant number
	const COperand* GetVariantTable();// get variant table
	~CStringResolution();//destructor
protected:
#ifdef _ERROR_INFO_H_
	DECLEAR_CATCH_ERROR
#endif
	int m_oprandNum;//operand number
	int m_oprandANum;//variant number
	int m_operatorNum;//operator number
	CString m_formula;//formula string
	COperator*m_operator;//pointer points to operators set
	COperand*m_oprand;//pointer points to operands set
	COperand*m_oprandA;//pointer points to variants set
	int IsFormula();//formula is availability or not.
	BOOL GetStack(OperatorStack &Op, OperandStack &Od);//get operators stack and operands stack.
	BOOL Initialize();//initialize variant member by m_formula.
	double Computer(OperatorStack &Operator, OperandStack &Oprand);//computing by stack.
	BOOL EmptyStack(OperatorStack st);//empty stack
	BOOL EmptyStack(OperandStack st);//
	BOOL GetOperandStack(OperandStack&Oprand,CString&string);//get operator stack in formula.
	BOOL GetOperatorStack(OperatorStack&Operator,CString &s);//get operand stack after GetOperandStack.
	double computing(const COperator*op,const COperand*oprand);//unary computing
	double computing(const COperator*op,const COperand*Loprand,const COperand*Roprand);//binary computing
};

#endif // !defined(AFX_STRINGRESOLUTION_H__69522ED3_1E49_404E_8D5E_1261F7D732DD__INCLUDED_)

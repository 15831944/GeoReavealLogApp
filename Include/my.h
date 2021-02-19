//my.h file
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

#ifndef _ERROR_INFO_H_
#define _ERROR_INFO_H_

#define BEGIN_CATCH_MATH_ERROR int entire_error;CString entire_info;\
int _matherr( struct _exception *except){

#define _BEGIN_GET_TYPE	switch(except->type){

#define GET_ERROR_TYPE(type,value,string) \
	case type: \
		entire_error=type;\
		except->retval=value;\
		entire_info=#string;\
		break;

#define _END_GET_TYPE 	default:\
		break;\
	}

#define GET_ERROR_FUNCTION(function,string)	if(strcmp( except->name, #function) == 0)\
		entire_info=(#string)+entire_info;

#define END_CATCH_MATH_ERROR	return 1;}

#define DECLEAR_CATCH_ERROR void clearError(BOOL);\
		void SaveErrorInfo(int,BOOL,CString);\
		struct _errorInfo{\
		int m_errorNumber;\
		BOOL m_formulaError;\
		CString m_errorInformation;\
	}m_errorInfo;

#define IMPLEMENT_CATCH_ERROR(calss_name) void calss_name::clearError(BOOL f){\
	if(entire_error){\
		SaveErrorInfo(entire_error,f,entire_info);\
		entire_error=0;}\
}\
void calss_name::SaveErrorInfo(int error, BOOL f, CString info){\
	m_errorInfo.m_errorNumber=error;\
	m_errorInfo.m_errorInformation=info;\
	m_errorInfo.m_formulaError=f;\
}

#define _CLEAR_ERROR(value) clearError(value);
#define _SAVE_ERROR_INFO(error,bRet,string) SaveErrorInfo(error,bRet,string);
#define GET_ERROR_INFORMATION(value) 	if(m_errorInfo.m_formulaError)\
		value="Expression error: "+m_errorInfo.m_errorInformation;\
	else\
		value="Evaluate error: "+m_errorInfo.m_errorInformation;
#define GET_ERROR_NUMBER return m_errorInfo.m_errorNumber;
#define GET_EXPRESSION_ERROR return m_errorInfo.m_formulaError;
#define PROCESSION_ERROR if(m_errorInfo.m_errorNumber){\
		if(m_errorInfo.m_formulaError)\
			return BIGNUMBER;\
		else\
			_SAVE_ERROR_INFO(0,FALSE,"Good!");\
	}

#endif
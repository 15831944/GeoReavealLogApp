// AACCorrect.h: interface for the CAACCorrect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AACCORRECT_H__6C9C57C7_9DF6_4D7B_A176_5C033D1F74FF__INCLUDED_)
#define AFX_AACCORRECT_H__6C9C57C7_9DF6_4D7B_A176_5C033D1F74FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DepCorrect.h"
class CAACCorrect : public CDepCorrect  
{
	DECLARE_SERIAL(CAACCorrect);
public:
	CAACCorrect();
	CAACCorrect(CString szNodename,BOOL bResult);
	virtual ~CAACCorrect();
// Attributes
public:
	CString m_szAACName;                  //加速度标识曲线
//  Implementation
public:
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
};

#endif // !defined(AFX_AACCORRECT_H__6C9C57C7_9DF6_4D7B_A176_5C033D1F74FF__INCLUDED_)

// PadCorrect.h: interface for the CPadCorrect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PADCORRECT_H__6C9C57C7_9DF6_4D7B_A176_5C033D1F74FF__INCLUDED_)
#define AFX_PADCORRECT_H__6C9C57C7_9DF6_4D7B_A176_5C033D1F74FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DepCorrect.h"
class CPadCorrect : public CDepCorrect  
{
	DECLARE_SERIAL(CPadCorrect);
public:
	CPadCorrect();
	CPadCorrect(CString szNodename,BOOL bResult);
	virtual ~CPadCorrect();
// Attributes
public:
	float m_ButDalta;                      //电扣间距
	float m_DepDalta[10];                  //极板深度差 0.062米
	BOOL  m_bDipDepPro,m_bRADSDepPro,m_bACDepPro;
	float m_MinSemblance;
//  Implementation
public:
	virtual BOOL Run();
	virtual void Serialize(CArchive& ar);
	BOOL AccelerationPro();                //极板加速度校正
	BOOL PadDepPro();                      //极板深度校正
	BOOL EMIButtonPro(float dalta);        //Halliburton电成像仪电扣深度校正
	BOOL STARButtonPro(float dalta);       //Baker Atlas电成像仪电扣深度校正 
	BOOL DIPDepPro();
	BOOL RADSDepPro();
};

#endif // !defined(AFX_PADCORRECT_H__6C9C57C7_9DF6_4D7B_A176_5C033D1F74FF__INCLUDED_)

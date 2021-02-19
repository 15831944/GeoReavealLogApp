/******************************************************************
*   TVDDraw.h - declarations of CCL Draw object class         *           
*******************************************************************/
#pragma once

#include "FIDDraw.h"
#include "afxtempl.h"
#include "CurveDraw.h"


class CTVDObj : public CCurveObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CTVDObj)
#else
	DECLARE_SERIAL(CTVDObj);
#endif
public:
	CTVDObj();
	CTVDObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDocument);
	~CTVDObj();
//Attributes
public:    

	//起始垂深校正量
	float   m_DaltaDep;
	//显示深度间距
	int m_nLevel;
	//细刻度线数
	int m_nGrid;
	
public:
	
	int CreatePoint(double sdep, double edep);

//Implementation
public:
	virtual void Serialize(CArchive& ar);
	virtual void DrawHead(CDC *pDC);	
	virtual void DrawClient(CDC *pDC);	
	
	virtual BOOL Properties(CView* pView);
};                

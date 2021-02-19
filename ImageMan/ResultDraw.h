/******************************************************************
*   Resuldrw.h - declarations of Result Draw object class         *           
*******************************************************************/


#ifndef __RESULDRW_H__
#define __RESULDRW_H__

#include "FIDDraw.h"

class CResultPoint : public CObject
{                  
public:
	CResultPoint(CRect rect, COLORREF ResultColor,LOGFONT ResultFont,CString nResultNo,
		CString NoStr,int nDirect,CImageManDoc* pDocumnet);
	void Draw(CDC* pDC);
	CImageManDoc* pDoc;
	CRect m_Rect;
	CString m_ResultNo,m_NoStr;
	float m_Sdep,m_Edep;
	WORD m_Direct;
	COLORREF m_ResultColor;
	LOGFONT m_ResultFont;
	CRect GetRect(){return m_Rect;};
	void SetRect(const CRect& rect){m_Rect = rect;};
	void SetClipRect(CDC* pDC, CRect rect);
};

class CResultObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CResultObj)
#else
	DECLARE_SERIAL(CResultObj);
#endif
public:
	CResultObj();
	CResultObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDocument);
	~CResultObj();

//Attributes
public:    
	COLORREF m_ResultOrderColor;
	CObList m_ResultPoint;
	WORD m_ResultLR;
	LOGFONT m_ResultOrderFont; 
private:
	BOOL bRButtonDown;

//operations
	virtual void CreatePoint();

//Implementation
public:
	virtual int HitTest(CView* pView,const CPoint& point);
	virtual void Serialize(CArchive& ar);
	virtual void DrawHead(CDC *pDC);	
	virtual void DrawClient(CDC *pDC);	
	virtual BOOL Properties(CView* pView);
	virtual void SetRect(const CRect& rect);
};                

#endif //__RESULDRW_H__
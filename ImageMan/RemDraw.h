/******************************************************************
*   Remdrw.h - declarations of the Rem Draw object class          *           
*******************************************************************/

#ifndef __REMDRW_H__
#define __REMDRW_H__

#ifndef __DRAWOBJ_H__
#include "DrawObj.h" 
#endif 

class CRemDrawObj : public CDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CRemDrawObj)
#else
	DECLARE_SERIAL(CRemDrawObj);
#endif
public:
	CRemDrawObj();
	CRemDrawObj(const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDoc);

//Attributes
public:
	LOGPEN m_LogPen;
	LOGBRUSH m_LogBrush;
private:
	BOOL bRButtonDown;

//Operations
public:
	virtual void OnFill();
	virtual void OnLine();

//  Implementtation	
public:
	virtual void Serialize(CArchive& ar);
	virtual BOOL Properties(CView* pView);
	virtual void SetRect(const CRect& rect);          
	virtual void OnChangeDepC(double depc, int flag);
	virtual void AlignToGrid(const CPoint& point, const CSize& size);
};   
                  
#endif //__REMDRW_H__                  

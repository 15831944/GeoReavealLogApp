/******************************************************************
*   CCLDraw.h - declarations of CCL Draw object class         *           
*******************************************************************/


#ifndef __CCLDRAW_H__
#define __CCLDRAW_H__

#include "FIDDraw.h"
#include "afxtempl.h"
#include "CurveDraw.h"

class CCCLObj;
class CCCLFill : public CObject
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CCCLFill)
#else
	DECLARE_SERIAL(CCCLFill);
#endif
public:
	CCCLFill();
	CCCLFill(CCCLFill&);
	void Serialize(CArchive& ar);
public:
	COLORREF m_Color;
	WORD m_LthNo;
	WORD m_FillMode;
	CCurveObj* pSourceCurve;
	CCurveObj* pTargetCurve;
	CString m_SourceCurveName;
	CString m_TargetCurveName;
	CString m_LthCurveName;
	CString m_Text;
};

class CCCLObj : public CCurveObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CCCLObj)
#else
	DECLARE_SERIAL(CCCLObj);
#endif
public:
	CCCLObj();
	CCCLObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CCCLObj();

//Attributes
public:    
	CLinePen m_LinePen;
	WORD m_CCLWidth,m_CCLStyleNo;
	COLORREF m_CCLColor;
	WORD  m_CalType,m_FilterNum;
	short m_Factor;
	CCCLObj* pFillCCLObj; 
	LOGFONT m_ScaleFont; 
	BOOL m_bDataEdit;
	BOOL m_bHeadShowFill;
	CArray<CCCLFill*,CCCLFill*> m_FillList;
	CPoint* m_Point;
	float* m_Value;
	int m_PointNum;
	BOOL SecondDraw;
	double m_LeftVal1,m_RightVal1;

	BOOL	m_DisCCLDep,m_DisCCLLong;
	float   m_HighValue,m_LowValue;
	int     m_CCLNum;
	float   *m_pCCLdep,*m_pCCLValue;
	int flag;
	int nBufferPointNum;
private:
	CPoint pt[3];
	float value[3];	
	CRect m_HeadRect0,m_Rect0;
	BOOL bErrorLthCCL;
	//operations
public:
	BOOL IsGridCCL();
	int GetGridValue();
	void CreatCCLPoint();
	BOOL IsFillCCL(CCCLObj* pObj);
	BOOL IsFillCCL(CString name);
	int RemoveFillCCL(CCCLObj* pObj);
	int RemoveFillCCL(CString name);
	int AddFillCCL(CCCLFill* pFill);
	void RemoveAllFillCCL();
	void FindAllFillCCL();
	int CreatePoint(double& sdep,double& edep);
	int GetPoint(double& sdep,double& edep,CPoint*& point);
	int GetValue(double& sdep,double& edep,float*& value);
	void Line(int x1,int y1,int x2,int y2,CDC* pDC);
	void DrawCCLDep(CDC *pDC,float sdep,float edep);
	void SaveCCLData();
//Implementation
public:
	virtual void Serialize(CArchive& ar);
	virtual void DrawHead(CDC *pDC);	
	virtual void DrawClient(CDC *pDC);	
	virtual void DrawFill(CView* pView, CDC* pDC);
	virtual BOOL Properties(CView* pView);
	virtual void DrawTracker(CView* pView, CDC* pDC, TrackerState state);
	virtual void DrawFocus(CView* pView);
	virtual int HitTest(CView* pView,const CPoint& point);
	virtual BOOL OnToolTip(CView* pView,CRect& rect,CString& text);
//Operations	
private:
	void DrawCCLFocuse(CView* pView);
};                

#endif //__CCLDRAW_H__
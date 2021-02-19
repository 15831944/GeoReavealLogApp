/******************************************************************
*   CurveDraw.h - declarations of Curve Draw object class         *           
*******************************************************************/


#ifndef __CURVEDRAW_H__
#define __CURVEDRAW_H__

#include "FIDDraw.h"
#include "afxtempl.h"

#define NORMAL_CURVE 0
#define ADD_CURVE 1
#define MINUS_CURVE 2
#define ABS_CURVE 3

#define FILLLEFT 0
#define FILLRIGHT 1
#define FILLLEFTRIGHT 2

typedef struct tagLINE
{
	CLinePen* pPen;
	CDC* pDC;
}LINE;

class CCurveObj;
class CCurveFill : public CObject
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CCurveFill)
#else
	DECLARE_SERIAL(CCurveFill);
#endif
public:
	CCurveFill();
	CCurveFill(CCurveFill&);
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

class CCurveObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CCurveObj)
#else
	DECLARE_SERIAL(CCurveObj);
#endif
public:
	CCurveObj();
	CCurveObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDocument);
	~CCurveObj();
//Attributes
public: 
	CObList m_ScriptPoint;

public:    
	CLinePen m_LinePen;
	WORD m_CurveWidth,m_CurveStyleNo;
	COLORREF m_CurveColor;
	short  m_CalType,m_FilterNum;
	short m_Factor;
	CCurveObj* pFillCurveObj; 
	LOGFONT m_ScaleFont; 
	BOOL m_bDataEdit;
	BOOL m_bHeadShowFill;
	CArray<CCurveFill*,CCurveFill*> m_FillList;
	CPoint* m_Point;
	float* m_Value;
	int m_PointNum ;
	double m_PointDep;
	double m_LeftVal1,m_RightVal1;
	int m_MovePointX,m_MovePointY;
private:
	CPoint pt[3];
	float value[3];
	CRect m_HeadRect0,m_Rect0;
	BOOL bErrorLthCurve;
	//operations
public:
	void CreatScriptPoint();
	BOOL IsGridCurve();
	int GetGridValue();
	BOOL IsFillCurve(CCurveObj* pObj);
	BOOL IsFillCurve(CString name);
	int RemoveFillCurve(CCurveObj* pObj);
	int RemoveFillCurve(CString name);
	int AddFillCurve(CCurveFill* pFill);
	void RemoveAllFillCurve();
	void FindAllFillCurve();
	int CreatePoint(double& sdep,double& edep);
	int GetPoint(double& sdep,double& edep,CPoint*& point);
	int GetValue(double& sdep,double& edep,float*& value);

	void SetClipRect(CDC* pDC , CRect rect);
	void Line(int x1,int y1,int x2,int y2,CDC* pDC);
	void DLine(int x1, int y1,int x2, int y2, LINE* fpLine);
	void AntiDLine(int x1, int y1,int x2, int y2, LINE* fpLine);

//Implementation
public:
	virtual void Serialize(CArchive& ar);
	virtual void DrawHead(CDC *pDC);	
	virtual void DrawClient(CDC *pDC);	
	virtual void DrawFill(CView* pView, CDC* pDC);
	virtual BOOL Properties(CView* pView);
	virtual void DrawTracker(CView* pView, CDC* pDC, TrackerState state);
	virtual HCURSOR GetHandleCursor(int nHandle);
	virtual void DrawFocus(CView* pView);
	virtual int HitTest(CView* pView,const CPoint& point);
	virtual BOOL OnToolTip(CView* pView,CRect& rect,CString& text);
	virtual void OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CView* pView, UINT nFlags, const CPoint& point);
	virtual void InitStatusBar();
	virtual void ShowStatus();
//Operations	
private:
	void DrawCurveFocuse(CView* pView);
};                

#endif //__CURVEDRAW_H__
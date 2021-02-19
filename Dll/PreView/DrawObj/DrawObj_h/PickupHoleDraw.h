/******************************************************************
*   PickupHoleDraw.h - declarations of Stratum Draw object class       *           
*******************************************************************/


#ifndef __PICKUPHOLEDRAW_H__
#define __PICKUPHOLEDRAW_H__

#include "FIDDraw.h"
#include "Grid.h"

class CContour : public CObject  //颗粒轮廓类
{
public:
	CContour(BOOL bAdd);
	CContour();
	virtual ~CContour();
protected:
	DECLARE_SERIAL(CContour);
public:
	long ComputeArea();
	BOOL IsIntersect(CRect rt);//判断此颗粒区域是否与矩形rt相交
	BOOL InRegion(CPoint pt);  //判断pt点是否在此颗粒区域内

public:
	CArray<CPoint,CPoint&>m_pointArray; //存放颗粒轮廓的坐标
	CRect m_rect;						//存放外接此颗粒的矩形
	BOOL  m_bAdd;                       //m_bAdd表示是否是增加颗粒
	float m_Ver;                        //对象版本号
	float m_Radius;						//溶洞半径
	float m_Round;						//溶洞圆度
	virtual void Serialize(CArchive& ar);
};

class CPickupHoleObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CPickupHoleObj)
#else
	DECLARE_SERIAL(CPickupHoleObj);
#endif
public:
	CPickupHoleObj();
	CPickupHoleObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CPickupHoleObj();
	
	//读取图像相关控制参数
	BOOL	m_bDrawRadius;				   //显示孔洞直径
	CString m_PickupFIDName;               //曲线全名
	CString m_PickupAliasName;             //曲线别名
	int 	m_PickupBit;                   //钻头直径 mm
	int 	m_PickupHorScale;              //横向绘图比例
	CRect	m_PickupRect;                  //新建时的图像大小 
	float	m_PickupStDep;                 //新建时的起始深度
	float	m_PickupEnDep;                 //新建时的结束深度
	float   m_PickupVerScale;              //纵向绘图比例
	float	m_CoverageRate;                //自动拾取覆盖率
	//存储已拾取的颗粒或孔洞
	CTypedPtrList<CObList,CContour*>m_ContourList;

	void CreatePoint();
	void DrawContour(CDC* pDC);
//Attributes
protected:   
	WORD m_PenWidth;
	COLORREF m_PenColor,m_FillColor;
	LOGFONT  m_ScaleFont;
	int	 m_bFill;
//Implementation
public:
	virtual void Serialize(CArchive& ar);
	virtual void DrawHead(CDC *pDC);	
	virtual void DrawClient(CDC *pDC);	
	virtual BOOL Properties(CView* pView);
};                

#endif //__PICKUPHOLEDRAW_H__
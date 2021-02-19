// FitCircle.h: interface for the CFitCircle class.
//
//////////////////////////////////////////////////////////////////////
//最小二乘法拟合圆
#if !defined(AFX_FITCIRCLE_H__423CC5C5_ECD7_4923_9265_E33FF403DFF4__INCLUDED_)
#define AFX_FITCIRCLE_H__423CC5C5_ECD7_4923_9265_E33FF403DFF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct tagPointINFO { 
    int   x; 
    int   y; 
} CIRCLE_POINT;

class AFX_EXT_CLASS CFitCircle  
{
public:
	CFitCircle(int Num);
	virtual ~CFitCircle();

	//有效数据点，小于Num 在调用LeastSquaresFitting 赋值
	int m_nNum;                     
	CIRCLE_POINT *m_points;
	double m_fCenterX,m_fCenterY,m_fRadius;
	void DoCircleFit();
};

#endif // !defined(AFX_FITCIRCLE_H__423CC5C5_ECD7_4923_9265_E33FF403DFF4__INCLUDED_)

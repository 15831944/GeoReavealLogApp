#pragma once

#include "Matrix.h"
#include <math.h>
#include <stdlib.h>
#include <iostream>
#ifndef PI
#define PI 3.1415926257
#endif

typedef struct tagPtInfo { 
    int   x; 
    int   y; 
} ELLIPS_POINT;

class AFX_EXT_CLASS CFitEllipse
{
public:
	//  样本点数
	int frame_length;
	//  点坐标
	ELLIPS_POINT *m_points;

	CFitEllipse(int Num);
	~CFitEllipse();
	BOOL DoEllipseFit();
//	椭圆中心位置(m_Cx, m_Cy)
	double m_Cx, m_Cy;
//  长轴m_Rx和短轴m_Ry
	double m_Rx, m_Ry;
//  长轴的转角
	double m_theta_radians;
	
private:
	double *quadratic_terms;
	double *linear_terms;
};

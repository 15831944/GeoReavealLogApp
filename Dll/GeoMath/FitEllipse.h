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
	//  ��������
	int frame_length;
	//  ������
	ELLIPS_POINT *m_points;

	CFitEllipse(int Num);
	~CFitEllipse();
	BOOL DoEllipseFit();
//	��Բ����λ��(m_Cx, m_Cy)
	double m_Cx, m_Cy;
//  ����m_Rx�Ͷ���m_Ry
	double m_Rx, m_Ry;
//  �����ת��
	double m_theta_radians;
	
private:
	double *quadratic_terms;
	double *linear_terms;
};

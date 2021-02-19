// FitCircle.cpp: implementation of the CFitCircle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FitCircle.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFitCircle::CFitCircle(int Num)
{
	m_points=new CIRCLE_POINT[Num];
	m_nNum=Num;
}

CFitCircle::~CFitCircle()
{
	if(m_points!=NULL)
		delete m_points;
}
//最小二乘法拟合圆
void CFitCircle::DoCircleFit()
{
	double X1=0;
	double Y1=0;
	double X2=0;
	double Y2=0;
	double X3=0;
	double Y3=0;
	double X1Y1=0;
	double X1Y2=0;
	double X2Y1=0;

	for (int i=0;i<m_nNum;i++)
	{
		X1 += m_points[i].x;
		Y1 += m_points[i].y;
		X2 += m_points[i].x*m_points[i].x;
		Y2 += m_points[i].y*m_points[i].y;
		X3 += m_points[i].x*m_points[i].x*m_points[i].x;
		Y3 += m_points[i].y*m_points[i].y*m_points[i].y;
		X1Y1 += m_points[i].x*m_points[i].y;
		X1Y2 += m_points[i].x*m_points[i].y*m_points[i].y;
		X2Y1 += m_points[i].x*m_points[i].x*m_points[i].y;
	}

	double C,D,E,G,H,N;
	double a,b,c;
	N = m_nNum;
	C = N*X2 - X1*X1;
	D = N*X1Y1 - X1*Y1;
	E = N*X3 + N*X1Y2 - (X2+Y2)*X1;
	G = N*Y2 - Y1*Y1;
	H = N*X2Y1 + N*Y3 - (X2+Y2)*Y1;
	a = (H*D-E*G)/(C*G-D*D);
	b = (H*C-E*D)/(D*D-G*C);
	c = -(a*X1 + b*Y1 + X2 + Y2)/N;

	m_fCenterX = a/(-2);
	m_fCenterY = b/(-2);
	m_fRadius = sqrt(a*a+b*b-4*c)/2;
}
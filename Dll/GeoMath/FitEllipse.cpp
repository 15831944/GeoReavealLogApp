#include "StdAfx.h"
#include "FitEllipse.h"

CFitEllipse::CFitEllipse(int Num)
{
	m_Cx=-1.;m_Cy=-1.;
	m_Rx=-1.;m_Ry=-1.;
	m_theta_radians=-1.;

	frame_length = Num;
	m_points=new ELLIPS_POINT[Num];

	quadratic_terms = new double[frame_length*3];
	linear_terms = new double[frame_length*3];	
}

CFitEllipse::~CFitEllipse(void)
{
	delete[] quadratic_terms;
	delete[] linear_terms;
	delete[] m_points;
}

BOOL CFitEllipse::DoEllipseFit()
{
	int index = 0;
	for(int j=0; j<frame_length; j++)
	{
		index = j * 3;
		quadratic_terms[index] = (double)m_points[j].x*m_points[j].x;
		quadratic_terms[index+1] = (double)m_points[j].x*m_points[j].y;
		quadratic_terms[index+2] = (double)m_points[j].y*m_points[j].y;
		
		linear_terms[index] = (double)m_points[j].x;
		linear_terms[index+1] = (double)m_points[j].y;
		linear_terms[index+2] = 1;
	}
	BOOL flag=TRUE;
	double *a1_point;
	double R2_point[9];
	double point[9];
	double EigenValue[3];
	double c[3]; 
	double P[6];
	double Pr[6];
	double Phi, cc, ss;
	double Xc, Yc, F, A, B, tempA;

	int Phi_sign;
	int c_number;
	int p_sign;

	CMatrix D1_Transpose, R2_Transpose_inv, R2_Transpose, S2_Transpose;
	CMatrix S1, S2, Q2, R2_temp, T, M;
	CMatrix eVector;
	CMatrix& EigenVector = eVector;
	CMatrix temp, ta1;

	CMatrix D1(frame_length, 3, quadratic_terms);		//quadratic terms
	CMatrix D2(frame_length, 3, linear_terms );			//linear terms

	D1_Transpose = D1.Transpose();
	S1 = D1_Transpose * D1;
	S2 = D1_Transpose * D2;

	R2_temp = D2;
	if(!R2_temp.SplitQR(Q2) )
	{
	//	AfxMessageBox("QR Split Error!");
		flag=FALSE;
	}
	index = 0;
	for(int i=0; i<3; i++)
	{	
		index = i * 3;
		R2_point[index] = R2_temp.GetElement(i,0);
		R2_point[index+1] = R2_temp.GetElement(i,1);
		R2_point[index+2] = R2_temp.GetElement(i,2);
	}
	CMatrix R2(3, 3, R2_point);

	R2_Transpose = R2.Transpose();
	S2_Transpose = S2.Transpose(); 
	
	if(!R2.InvertGaussJordan())
	{
	//	AfxMessageBox("InvertGaussJordan Error!");
		flag=FALSE;
	}
	R2_Transpose_inv = R2.Transpose();
	temp = R2_Transpose_inv * S2_Transpose; 
	T = R2 * temp;
	T = T * (-1);
	
	temp = S2 * T;
	M = S1 + temp;

	for(int j=0; j<3; j++)
	{
		point[ 6+j ] = M.GetElement(0, j) / 2;
		point[ 3+j ] = M.GetElement(1, j) * (-1);
		point[j] = M.GetElement(2, j) / 2;
	}
	CMatrix CinvM(3, 3, point);

	if(!M.JacobiEigenv(EigenValue, EigenVector, 60, 0.000001) )
	{
	//	AfxMessageBox("JacobiEigenv Error!");
		flag=FALSE;
	}
	c_number = 0;
 	for(int i=0; i<3; i++)
	{
		c[i] = 4 * EigenVector.GetElement(0, i) *EigenVector.GetElement(2, i) - EigenVector.GetElement(1, i) * EigenVector.GetElement(1, i);
		if(c[i]>0)
			c_number++;
	}
	a1_point = new double[ c_number*3 ];
	index = 0;
	for(int i=0; i<3; i++)
	{
		index = i*c_number;
		for(int j=0; j<c_number; j++)
		{
			for(int m=0; m<3; m++)
			{
				if( c[m]>0 )
					a1_point[index+j] = EigenVector.GetElement(i, m);
			}
		}
	}

	CMatrix A1(3, c_number, a1_point);

	ta1 = T*A1;

	P[0] = a1_point[0];
	P[1] = a1_point[1];
	P[2] = a1_point[2];
	P[3] = ta1.GetElement(0, 0);
	P[4] = ta1.GetElement(1, 0);
	P[5] = ta1.GetElement(2, 0);

	if( P[0]> 0 )
		p_sign = 1;
	else
		p_sign = -1;
	for(int i=0; i<6; i++)
		P[i] = p_sign * P[i];

	Phi = atan(P[1]/(P[2]-P[0]))/2;
	cc = cos(Phi);
	ss = sin(Phi);

	Pr[0] = P[0]*cc*cc - P[1]*cc*ss + P[2]*ss*ss;
	Pr[1] = 2*(P[0]-P[2])*cc*ss + (cc*cc-ss*ss)*P[1];
	Pr[2] = P[0]*ss*ss + P[1]*ss*cc + P[2]*cc*cc;
	Pr[3] = P[3]*cc - P[4]*ss;
	Pr[4] = P[3]*ss + P[4]*cc;
	Pr[5] = P[5];

	Xc = -cc*Pr[3] / (2*Pr[0]) - ss*Pr[4] / (2*Pr[2]);
	Yc = ss*Pr[3] / (2*Pr[0]) -cc*Pr[4] / (2*Pr[2]);
	F = -Pr[5] + Pr[3]*Pr[3]/(4*Pr[0]) + Pr[4]*Pr[4]/(4*Pr[2]);
	A = sqrt(F / Pr[0]);
	B = sqrt(F / Pr[2]);
	
	Phi = -Phi;
	if( Phi>0 )
		Phi_sign = 1;
	else
		Phi_sign = -1;
	
	if(A<B)
	{
		Phi = Phi - Phi_sign*PI/2;
		tempA = A;
		A = B;
		B = tempA;
	}
	m_Cx = Xc;
	m_Cy = Yc;
	m_Rx = A;
	m_Ry = B;
	m_theta_radians = Phi;
	delete a1_point;
	return flag;
}

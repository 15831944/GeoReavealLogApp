/*
Module : DIBIMAGE.CPP
Purpose: Implementation for an MFC class that encapsulates DIBs
         and supports a variety of image manipulation functions on it
Created: PJN / DIBSECTION/1 / 23-07-1997
History: None

Copyright (c) 1997 by PJ Naughter.  
All rights reserved.

*/

/////////////////////////////////  Includes  //////////////////////////////////
#include "stdafx.h"
#include "math.h"
#include "dibimage.h"
#include "resource.h"
#include "MainFrm.h"
#include "Guage.h"
#include "FitEllipse.h"
#include "Matrix.h"
#include <math.h>
//#include <stdlib.h>
//#include <iostream>
#ifndef PI
#define PI 3.1415926257
#endif
////////////////////////////// Future additions ///////////////////////////////
/*
BOOL CDibImage::TraceContourFilter()
BOOL CDibImage::EqualizeHistogram()
BOOL CDibImage::StretchHistogram()
BOOL CDibImage::MosaicFilter(int BlockSize)
BOOL CDibImage::DespeckleFilter()
BOOL CDibImage::DilateFilter()
BOOL CDibImage::EmbossFilter()
BOOL CDibImage::ErodeFilter()
BOOL CDibImage::SoftenFilter()
BOOL CDibImage::SharpenFilter()
BOOL CDibImage::UnSharpenFilter()
BOOL CDibImage::Posterize(int BitsPerChannel)
BOOL CDibImage::Solerize(int Threshold)
BOOL CDibImage::CirleDeform()
BOOL CDibImage::HorizontalCylinderDeform()
BOOL CDibImage::VerticalCylinderDeform()
BOOL CDibImage::MotionBlurDeform()
BOOL CDibImage::PentagonDeform()
BOOL CDibImage::PerspectiveHorizontalDeform()
BOOL CDibImage::PerspectiveVerticalDeform()
BOOL CDibImage::PinchDeform()
BOOL CDibImage::PunchDeform()
BOOL CDibImage::SkewDeform()
BOOL CDibImage::WindDeform()
BOOL CDibImage::ModifyHistogram(int* TransformTable, int nSize);
BOOL CDibImage::Resize(const CSize& size);
BOOL CDibImage::Resample(const CSize& size);
static BOOL CDibImage::ArithmeticAdd(CDibImage& dib1, CDibImage& dib2, int Divisor, int Bias, BOOL bClip);
static BOOL CDibImage::ArithmeticSubtract(CDibImage& dib1, CDibImage& dib2, int Divisor, int Bias, BOOL bClip);
static BOOL CDibImage::ArithmeticMultiply(CDibImage& dib1, CDibImage& dib2, int Divisor, int Bias, BOOL bClip);
static BOOL CDibImage::ArithmeticDifference(CDibImage& dib1, CDibImage& dib2, int Divisor, int Bias, BOOL bClip);
static BOOL CDibImage::ArithmeticDarkest(CDibImage& dib1, CDibImage& dib2, int Divisor, int Bias, BOOL bClip);
static BOOL CDibImage::ArithmeticLightest(CDibImage& dib1, CDibImage& dib2, int Divisor, int Bias, BOOL bClip);
BOOL CDibImage::SpacialFilter();
*/

///////////////////////////////// defines /////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//extern int g_nStandard;
//////////////////////////////// Implementation ///////////////////////////////
/*
BOOL Compare2(CDibImage *pDib, int *pArray, int x, int y, int &mark)
{
	int num=0;
	for (int m=-1; m<2; m++)
	{
		for (int n=-1; n<2; n++)
		{
			COLORREF val;
			pDib->GetPixel(x+m,y+n,val);
			if (val==RGB(0,0,0))
			{
				num++;
			}
		}
	}
	if (num==2)
	{
		return TRUE;
	}
	return FALSE;
}
BOOL Compare8(CDibImage *pDib, int *pArray, int x, int y, int &mark)
{
	COLORREF value;
	
	pDib->GetPixel(x, y, value);
	if (value==RGB(0,0,0))
	{
		int tempx=x;
		int tempy=y;
		while (Compare2(pDib, pArray, tempx, tempy, mark))
		{
			int num=0;
			pDib->SetPixel(tempx, tempy, RGB(255,255,255));
			for (int m=-1; m<2; m++)
			{
				for (int n=-1; n<2; n++)
				{
					COLORREF val;
					pDib->GetPixel(tempx+m,tempy+n,val);
					if (val==RGB(0,0,0))
					{
						num++;
						tempx=tempx+m;
						tempy=tempy+n;
					}
				}
			}
			if (num!=1)break;			
		}		
	}
	
	
	return TRUE;
}*/

C3By3Filter::C3By3Filter()
{
	//Default filter is a filter which will not do anything to the image
	for (int i=0; i<3; i++)
	for (int j=0; j<3; j++)
		m_nValues[i][j] = 0;

	m_nValues[1][1] = 1;
	m_nDivision = 1;
	m_nBias = 0;
}


COLORREF C3By3Filter::Filter(CDibImage& dibImage, LPSTR lpDibBits, int x, int y)
{
	int r = 0;
	int g = 0;
	int b = 0;
	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++)
		{
			COLORREF c;
			if (dibImage.GetPixel(x+i-1, y+j-1, c, lpDibBits))
			{
			r += (m_nValues[i][j] * GetRValue(c));
			g += (m_nValues[i][j] * GetGValue(c));
			b += (m_nValues[i][j] * GetBValue(c));
			}
		}
	}

	ASSERT(m_nDivision);
	r=abs(r);g=abs(g);b=abs(b);
	r = min(r/m_nDivision + m_nBias, 255);
	g = min(g/m_nDivision + m_nBias, 255);
	b = min(b/m_nDivision + m_nBias, 255);
	return RGB(r, g, b);
}



C3By3MedianFilter::C3By3MedianFilter()
{
}

COLORREF C3By3MedianFilter::Filter(CDibImage& dibImage, LPSTR lpDibBits, int x, int y)
{
	int nPixels = 0;

	for (int k=0; k<9; k++)
	m_Ordered[k] = 0;

	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++)
		{
			COLORREF c;
			if (dibImage.GetPixel(x+i-1, y+j-1, c, lpDibBits))
			{
			m_Ordered[nPixels] = c;
			nPixels++;
			}
		}
	}

	qsort(&m_Ordered, nPixels, sizeof(COLORREF), CompareFunc);
	return m_Ordered[nPixels/2];
}

int C3By3MedianFilter::CompareFunc(const void *elem1, const void *elem2)
{
	COLORREF c1 = *(COLORREF*)elem1;
	COLORREF c2 = *(COLORREF*)elem2;

	if (c1 == c2)
	return 0;
	else 
	{
		BYTE r1 = GetRValue(c1);
		BYTE g1 = GetGValue(c1);
		BYTE b1 = GetBValue(c1);
		BYTE r2 = GetRValue(c2);
		BYTE g2 = GetGValue(c2);
		BYTE b2 = GetBValue(c2);

		if ((r1 + g1 + b1) > (r2 + g2 + b2))
			return 1;
		else
			return -1;
	}
}

C5By5Filter::C5By5Filter()
{
  //Default filter is a filter which will not do anything to the image
	for (int i=0; i<5; i++)
	for (int j=0; j<5; j++)
		m_nValues[i][j] = 0;

	m_nValues[2][2] = 1;
	m_nDivision = 1;
	m_nBias = 0;
}

COLORREF C5By5Filter::Filter(CDibImage& dibImage, LPSTR lpDibBits, int x, int y)
{
	int r = 0;
	int g = 0;
	int b = 0;
	for (int i=0; i<5; i++)
	{
		for (int j=0; j<5; j++)
		{
			COLORREF c;
			if (dibImage.GetPixel(x+i-2, y+j-2, c, lpDibBits))
			{
				r += (m_nValues[i][j] * GetRValue(c));
				g += (m_nValues[i][j] * GetGValue(c));
				b += (m_nValues[i][j] * GetBValue(c));
			}
		}
	}

	ASSERT(m_nDivision);
	r=abs(r);g=abs(g);b=abs(b);  
	r = min(r/m_nDivision + m_nBias, 255);
	g = min(g/m_nDivision + m_nBias, 255);
	b = min(b/m_nDivision + m_nBias, 255);
	return RGB(r, g, b);
}

C7By7Filter::C7By7Filter()
{
	//Default filter is a filter which will not do anything to the image
	for (int i=0; i<7; i++)
	for (int j=0; j<7; j++)
		m_nValues[i][j] = 0;

	m_nValues[3][3] = 1;
	m_nDivision = 1;
	m_nBias = 0;
}

COLORREF C7By7Filter::Filter(CDibImage& dibImage, LPSTR lpDibBits, int x, int y)
{
	int r = 0;
	int g = 0;
	int b = 0;
	for (int i=0; i<7; i++)
	{
		for (int j=0; j<7; j++)
		{
			COLORREF c;
			if (dibImage.GetPixel(x+i-3, y+j-3, c, lpDibBits))
			{
			r += (m_nValues[i][j] * GetRValue(c));
			g += (m_nValues[i][j] * GetGValue(c));
			b += (m_nValues[i][j] * GetBValue(c));
			}
		}
	}

	ASSERT(m_nDivision);
	r=abs(r);g=abs(g);b=abs(b);  
	r = min(r/m_nDivision + m_nBias, 255);
	g = min(g/m_nDivision + m_nBias, 255);
	b = min(b/m_nDivision + m_nBias, 255);
	return RGB(r, g, b);
}

IMPLEMENT_SERIAL(CContour, CObject, 0)

CContour::CContour(BOOL bAdd)
{
	m_pointArray.SetSize(0,10);
	m_bAdd=bAdd;
	m_Radius=-999.25f;						//孔洞半径
	m_Round=-999.25f;						//孔洞圆度
	m_Rx=-999.25f;							//孔洞长轴长度
	m_Ry=-999.25f;							//孔洞短轴长度
	m_Phi=-999.25f;							//孔洞长轴角度
	m_Value=-999.25f;						//平均电导率值
}
CContour::CContour()
{
	m_Radius=-999.25f;						//孔洞半径
	m_Round=-999.25f;						//孔洞圆度
	m_Rx=-999.25f;							//孔洞长轴长度
	m_Ry=-999.25f;							//孔洞短轴长度
	m_Phi=-999.25f;							//孔洞长轴角度
	m_Value=-999.25f;						//平均电导率值
}
CContour::~CContour()
{
	m_pointArray.RemoveAll();
	m_pointArray.SetSize(0);	
}
/*
*	说明：该函数用于擦除图像中的孔洞，使其变为白色
*/
BOOL CContour::EraserGrain(CDibImage& ds)
{
	CRgn rgn;
	BOOL bSuccess=TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(ds.GetHDIB());
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	
	for(int i=0;bSuccess&&i<m_pointArray.GetSize();i++)//擦除区域边界
	{
		COLORREF c=RGB(255,255,255);
		bSuccess = ds.SetPixel(m_pointArray[i].x, m_pointArray[i].y, c, lpDibBits);		
	}
	

	if(!rgn.CreatePolygonRgn(&m_pointArray[0],m_pointArray.GetSize(),ALTERNATE))
	{
//		TRACE("Cannot Create PolygonRgn\n");
		return FALSE;
	}	
	for (int y=m_rect.top; bSuccess && y<m_rect.bottom; y++)//擦除区域内部
	{
		for (int x=m_rect.left; bSuccess && x<m_rect.right; x++)
		{
			if(rgn.PtInRegion(CPoint(x,y)))
			{
				COLORREF c=RGB(255,255,255);
				bSuccess = ds.SetPixel(x, y, c, lpDibBits);
			}
		}
	}
	rgn.DeleteObject();
	GlobalUnlock((HGLOBAL) ds.GetHDIB());
	return bSuccess;	
}

BOOL CContour::EraserColorGrain(CDibImage& ds)
{
	CRgn rgn;
	BOOL bSuccess=TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(ds.GetHDIB());
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	
	for(int i=0;bSuccess&&i<m_pointArray.GetSize();i++)//擦除区域边界
	{
		COLORREF c=RGB(255,255,255);
		bSuccess = ds.SetPixel(m_pointArray[i].x, m_pointArray[i].y, c, lpDibBits);		
	}
		
	if(!rgn.CreatePolygonRgn(&m_pointArray[0],m_pointArray.GetSize(),ALTERNATE))
	{
//		TRACE("Cannot Create PolygonRgn\n");
		return FALSE;
	}	
	for (int y=m_rect.top; bSuccess && y<m_rect.bottom; y++)//擦除区域内部
	{
		for (int x=m_rect.left; bSuccess && x<m_rect.right; x++)
		{
			if(rgn.PtInRegion(CPoint(x,y)))
			{
				COLORREF c=RGB(255,255,255);
				bSuccess = ds.SetPixel(x, y, c, lpDibBits);
			}
		}
	}
	rgn.DeleteObject();
	GlobalUnlock((HGLOBAL) ds.GetHDIB());
	return bSuccess;	
}
BOOL CContour::FaultGrainRadius(const CPoint& point,int& r1,int& r2,CDibImage& ds)
{    
	r1=(int)sqrt(pow((m_pointArray[0].x-point.x),2)+
		pow((m_pointArray[0].y-point.y),2));
	r2=(int)sqrt(pow((m_pointArray[0].x-point.x),2)+
		pow((m_pointArray[0].y-point.y),2));
	for(int i=0;i<m_pointArray.GetSize();i++)//擦除区域边界
	{	
		int rr=(int)sqrt(pow((m_pointArray[i].x-point.x),2)+
			pow((m_pointArray[i].y-point.y),2));
		r1=min(r1,rr);
		r2=max(r2,rr);
	}
	return TRUE;	
}

BOOL CContour::GrainMaxValue(int Red,int Green,int Blue,CDibImage& ds,int nBiasSize,int nSize)
{
	CRgn rgn;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(ds.GetHDIB());
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	
	COLORREF c=RGB(Red,Green,Blue);
	COLORREF max=RGB(Red,Green,Blue);
	for(int i=0;i<m_pointArray.GetSize();i++)//擦除区域边界
	{		
		ds.GetPixel(m_pointArray[i].x, m_pointArray[i].y, c, lpDibBits);
		int r=GetRValue(c);
		int g=GetGValue(c);
		int b=GetBValue(c);
		int mr=GetRValue(max);
		int mg=GetGValue(max);
		int mb=GetBValue(max);
		if(abs(r-Red)>abs(mr-Red)&&abs(g-Green)>abs(mg-Green)&&abs(b-Blue)>abs(mb-Blue))
			max=RGB(r,g,b);		
	}	
	
	if(!rgn.CreatePolygonRgn(&m_pointArray[0],m_pointArray.GetSize(),ALTERNATE))
	{
//		TRACE("Cannot Create PolygonRgn\n");
		return FALSE;
	}	
	for (int y=m_rect.top; y<m_rect.bottom; y++)//擦除区域内部
	{
		for (int x=m_rect.left; x<m_rect.right; x++)
		{
			if(rgn.PtInRegion(CPoint(x,y)))
			{
				ds.GetPixel(x, y, c, lpDibBits);
				int r=GetRValue(c);
				int g=GetGValue(c);
				int b=GetBValue(c);
				int mr=GetRValue(max);
				int mg=GetGValue(max);
				int mb=GetBValue(max);
				if(abs(r-Red)>abs(mr-Red)&&abs(g-Green)>abs(mg-Green)&&abs(b-Blue)>abs(mb-Blue))
					max=RGB(r,g,b);		
			}
		}
	}
	rgn.DeleteObject();
	GlobalUnlock((HGLOBAL) ds.GetHDIB());
	
	int mr=GetRValue(max);
	int mg=GetGValue(max);
	int mb=GetBValue(max);
	
	int n=(nSize-nBiasSize)/2;
	int UpRed=min(Red+n,255);
	int DownRed=max(Red-n,0);
	int UpGreen=min(Green+n,255);
	int DownGreen=max(Green-n,0);
	int UpBlue=min(Blue+n,255);
	int DownBlue=max(Blue-n,0);
	if((mr>=DownRed&&mr<=UpRed)&&(mg>=DownGreen&&mg<=UpGreen)&&(mb>=DownBlue&&mb<=UpBlue))
		return TRUE;
	else
		return FALSE;  
}

/*
 *	说明：该函数的作用是画出孔洞的轮廓
 */
void CContour::DrawContour(CDC* pDC,int nWidth,COLORREF c,float zoom)
{
	if(m_pointArray.GetSize()==1)
		pDC->SetPixelV(int(m_pointArray[0].x*zoom),int(m_pointArray[0].y*zoom),c);
	else if(m_pointArray.GetSize()>=2)
	{
		CPen pen(PS_SOLID,nWidth,c);
		CPen* pOldPen;
		pOldPen=pDC->SelectObject(&pen);
		CArray<CPoint,CPoint&>pointArray;
		pointArray.SetSize(m_pointArray.GetSize(),5);
		for(int i=0;i<m_pointArray.GetSize();i++)
		{
			pointArray[i].x=(long)(m_pointArray[i].x*zoom);
			pointArray[i].y=(long)(m_pointArray[i].y*zoom);
		}
		pDC->Polyline(&pointArray[0],m_pointArray.GetSize());
		pDC->SelectObject(pOldPen);
		pointArray.SetSize(0);
	}
}

//得到孔洞的RGB平均值
COLORREF CContour::GetTgtPtAvgRGB(CDibImage& ds)
{
	CRgn rgn;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(ds.GetHDIB());
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	COLORREF c;

	int rsum=0;
	int gsum=0;
	int bsum=0;
	int sum=0;
	int number=0;
	int ravg=0;
	int gavg=0;
	int bavg=0;
	COLORREF avgc;//=RGB(ravg,gavg,bavg);
    int r,g,b;
	
	if(rgn.CreatePolygonRgn(&m_pointArray[0],m_pointArray.GetSize(),ALTERNATE))
	{
		
			for (int h=m_rect.top; h<=m_rect.bottom; h++)
			{
				for (int w=m_rect.left; w<=m_rect.right; w++)
				{
					if(rgn.PtInRegion(CPoint(w,h)))      
					{
						ds.GetPixel(w, h, c, lpDibBits);
						r=GetRValue(c);
						g=GetGValue(c);
						b=GetBValue(c);
						rsum+=r;
						gsum+=g;
						bsum+=b;
					}				
					
					number++;
				}
			}
			ravg=rsum/number;
			gavg=gsum/number;
			bavg=bsum/number;
			
		
		rgn.DeleteObject();

	}
	avgc=RGB(ravg,gavg,bavg);
	return avgc;
	GlobalUnlock((HGLOBAL) ds.GetHDIB());   
}
//得到每个孔洞的平均值与给定的差值
//COLORREF CContour::GetAvgRGB(CDibImage& ds)
COLORREF  CContour::GetAvgRGB(int Red,int Green,int Blue,CDibImage& ds)
{	
	CRgn rgn;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(ds.GetHDIB());
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);

    COLORREF c;
	int rsum=0;
	int gsum=0;
	int bsum=0;
	int sum=0;
	int number=0;
	int ravg=0;
	int gavg=0;
	int bavg=0;
	COLORREF avgc;//=RGB(ravg,gavg,bavg);
    int r,g,b;
	if(rgn.CreatePolygonRgn(&m_pointArray[0],m_pointArray.GetSize(),ALTERNATE))
	{	
		for (int y=m_rect.top; y<=m_rect.bottom; y++)//计算每个孔洞的RGB平均值
		{
			for (int x=m_rect.left; x<=m_rect.right; x++)
			{
				if(rgn.PtInRegion(CPoint(x,y)))      
				{
					ds.GetPixel(x, y, c, lpDibBits);
					 r=GetRValue(c);
                     g=GetGValue(c);
					 b=GetBValue(c);
					 rsum+=r;
					 gsum+=g;
					 bsum+=b;
				}				
				 
				 number++;
			}
		}
		ravg=rsum/number;
		gavg=gsum/number;
		bavg=bsum/number;

		int rsub=abs(ravg-Red);
		int gsub=abs(gavg-Green);
		int bsub=abs(bavg-Blue);
		avgc=RGB(rsub,gsub,bsub);
		rgn.DeleteObject();
	}
	return avgc;
	GlobalUnlock((HGLOBAL) ds.GetHDIB());	
}
/*
 *	说明：该函数的作用是计算孔洞的面积
 */
long CContour::ComputeArea()
{
	long number=0;
	int k=m_pointArray.GetSize();
	if(k<4)
		number=k;
	else
	{
		int x,y;
		CRgn rgn;
		CRect rect;
		if(rgn.CreatePolygonRgn(&m_pointArray[0],k,ALTERNATE))
		{
			rgn.GetRgnBox(&rect);
			for (y=rect.top; y<=rect.bottom; y++)//计算区域内部的面积
				for (x=rect.left; x<=rect.right; x++)
					if(rgn.PtInRegion(CPoint(x,y)))
						number++;
			rgn.DeleteObject();
		}
	}
	return number;	
}
/*
 *	说明：该函数的作用是计算孔洞的圆度
 *  Ver Hor为图像纵横向比例 缺省为1
 */
float CContour::ComputeRound(float Hor,float Ver)
{
	m_Round=-999.25f;
	//  样本点数
	int DataNum=m_pointArray.GetSize();
	if(DataNum<10)
		return 100.f;	
	else
	{
		int i,j,k;
		DataNum=(DataNum-DataNum%2)/2;
		//二次方程式
		double *pQuadratic = new double[DataNum*3];
		double *pLinear = new double[DataNum*3];

		//拟合椭圆
		int index = 0;
		CPoint pt=m_pointArray[0];
		CPoint pt0=m_pointArray[0];
		for(j=0;j<DataNum;j++)
		{
			if(j>0)//对点作纵横向比例转换
			{
				pt.x=pt0.x+(m_pointArray[j*2].x-pt0.x)*Hor;
				pt.y=pt0.y+(m_pointArray[j*2].y-pt0.y)*Ver;
			}
			index = j * 3;
			pQuadratic[index]   = (double)(pt.x*pt.x);
			pQuadratic[index+1] = (double)(pt.x*pt.y);
			pQuadratic[index+2] = (double)(pt.y*pt.y);
			
			pLinear[index]   = (double)pt.x;
			pLinear[index+1] = (double)pt.y;
			pLinear[index+2] = 1;
		}
		
		double R2_pt[9];
		double point[9];
		double EigenValue[3];//特征值
		double C[3]; 
		double P[6];
		double Pr[6];
		double Vcos,Vsin;//余弦正弦值

		int Phi_sign;
		int c_number;
		int p_sign;

		CMatrix D1_Transpose, R2_Transpose_inv, R2_Transpose, S2_Transpose;
		CMatrix S1, S2, Q2, R2_tmp, T, M;
		CMatrix eVector;
		CMatrix& EigenVector = eVector;
		CMatrix temp, ta1;

		CMatrix D1(DataNum, 3, pQuadratic);		//quadratic terms
		CMatrix D2(DataNum, 3, pLinear );			//linear terms

		D1_Transpose = D1.Transpose();
		S1 = D1_Transpose * D1;
		S2 = D1_Transpose * D2;

		R2_tmp = D2;
		if(!R2_tmp.SplitQR(Q2))
		{
			delete[] pQuadratic;
			delete[] pLinear;
			return m_Round;
		}

		index = 0;
		for(i=0; i<3; i++)
		{	
			index = i * 3;
			R2_pt[index]   = R2_tmp.GetElement(i,0);
			R2_pt[index+1] = R2_tmp.GetElement(i,1);
			R2_pt[index+2] = R2_tmp.GetElement(i,2);
		}
		CMatrix R2(3, 3, R2_pt);

		R2_Transpose = R2.Transpose();
		S2_Transpose = S2.Transpose(); 
		
		if(!R2.InvertGaussJordan())
		{
			delete[] pQuadratic;
			delete[] pLinear;
			return m_Round;
		}
		R2_Transpose_inv = R2.Transpose();
		temp = R2_Transpose_inv * S2_Transpose; 
		T = R2 * temp;
		T = T * (-1);
		
		temp = S2 * T;
		M = S1 + temp;

		for(j=0; j<3; j++)
		{
			point[ 6+j ] = M.GetElement(0, j) / 2;
			point[ 3+j ] = M.GetElement(1, j) * (-1);
			point[j] = M.GetElement(2, j) / 2;
		}
		CMatrix CinvM(3, 3, point);

		if(!M.JacobiEigenv(EigenValue, EigenVector, 60, 0.000001))
		{
			delete[] pQuadratic;
			delete[] pLinear;
			return m_Round;
		}
			
		c_number = 0;
 		for(i=0; i<3; i++)
		{
			C[i] = 4 * EigenVector.GetElement(0, i) *EigenVector.GetElement(2, i) - EigenVector.GetElement(1, i) * EigenVector.GetElement(1, i);
			if(C[i]>0)
				c_number++;
		}
		double *a1_point = new double[ c_number*3 ];
		index = 0;
		for(i=0; i<3; i++)
		{
			index = i*c_number;
			for(j=0; j<c_number; j++)
			{
				for(k=0; k<3; k++)
				{
					if( C[k]>0 )
						a1_point[index+j] = EigenVector.GetElement(i, k);
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
		for(i=0; i<6; i++)
			P[i] = p_sign * P[i];

		delete a1_point;
		delete pQuadratic;
		delete pLinear;

		//计算旋转角
		m_Phi = atan(P[1]/(P[2]-P[0]))/2;

		Vcos = cos(m_Phi);
		Vsin = sin(m_Phi);

		Pr[0] = P[0]*Vcos*Vcos - P[1]*Vcos*Vsin + P[2]*Vsin*Vsin;
		Pr[1] = 2*(P[0]-P[2])*Vcos*Vsin + (Vcos*Vcos-Vsin*Vsin)*P[1];
		Pr[2] = P[0]*Vsin*Vsin + P[1]*Vsin*Vcos + P[2]*Vcos*Vcos;
		Pr[3] = P[3]*Vcos - P[4]*Vsin;
		Pr[4] = P[3]*Vsin + P[4]*Vcos;
		Pr[5] = P[5];

		//孔洞中心位置(m_Cx, m_Cy)
		m_Cx = (float)(-Vcos*Pr[3] / (2*Pr[0]) - Vsin*Pr[4] / (2*Pr[2]));
		m_Cy = (float)(Vsin*Pr[3] / (2*Pr[0]) -Vcos*Pr[4] / (2*Pr[2]));
		double tp = -Pr[5] + Pr[3]*Pr[3]/(4*Pr[0]) + Pr[4]*Pr[4]/(4*Pr[2]);
		//m_Rx = (float)sqrt(tp / Pr[0]);//孔洞长轴长度
		//m_Ry = (float)sqrt(tp / Pr[2]);//孔洞短轴长度
		double tpx=tp/Pr[0];
		double tpy=tp/Pr[2];
		if(tpx>0.f&&tpy>0.f)
		{
			m_Rx = (float)sqrt(tpx);
			m_Ry = (float)sqrt(tpy);
			m_Phi = -m_Phi;
			if(m_Phi>0)
				Phi_sign = 1;
			else
				Phi_sign = -1;
			if(m_Rx<m_Ry)
			{
				m_Phi=m_Phi - Phi_sign*PI/2;
				tp = m_Rx;
				m_Rx = m_Ry;
				m_Ry = tp;
			}
			m_Round=m_Ry/m_Rx*100.f;//孔洞圆度
		}
		else if(tpx<0.f&&tpy<0.f)
			return m_Round;
		else
		{
			if(tpx>0.f)
			{
				/*m_Phi=0.f;*/
				m_Rx = (float)sqrt(tpx);
			}
			else
			{
				m_Phi+=PI/2;
				m_Rx = (float)sqrt(tpy);
			}
			m_Round=0.001f;
		}
	}
	return m_Round;	
}
/*
 *	判断此孔洞区域是否与矩形rt相交
 */
BOOL CContour::IsIntersect(CRect rt)
{
	BOOL bIntersect=FALSE;
	for(int i=0;i<m_pointArray.GetSize()&&!bIntersect;i++)
	{
		bIntersect=rt.PtInRect(CPoint(m_pointArray[i].x,m_pointArray[i].y));		
	}
	CRgn rgn;
	BOOL bSuccess=rgn.CreatePolygonRgn(&m_pointArray[0],m_pointArray.GetSize(),ALTERNATE);
	for(int y=rt.top;bSuccess&&y<rt.bottom&&!bIntersect;y++)
		for(int x=rt.left;bSuccess&&x<rt.right&&!bIntersect;x++)
			bIntersect=rgn.PtInRegion(CPoint(x,y));
	if(bSuccess) 
		rgn.DeleteObject();
	return bIntersect;
}
/*
 *	判断pt点是否在此孔洞区域内
 */
BOOL CContour::InRegion(CPoint pt)
{
	BOOL bIn=FALSE;
	for(int i=0;i<m_pointArray.GetSize()&&!bIn;i++)
		if(pt==m_pointArray[i])
			bIn=TRUE;
	if(!bIn)
	{
		CRgn rgn;
		BOOL bSuccess=rgn.CreatePolygonRgn(&m_pointArray[0],m_pointArray.GetSize(),ALTERNATE);
		if(bSuccess)
		{
			bIn=rgn.PtInRegion(pt);	
			rgn.DeleteObject();
		}
	}
	return bIn;
}

CDibImage::CDibImage()
{
	m_hDib = NULL;
	m_hBitmap=NULL;
	m_Pal = NULL;  
	m_pPalette  = NULL;
	m_pBitmap   = NULL;  
	m_pWorkingArea = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nScanWidth = 0;
	m_nBitsPerPixel = 0;
	m_Proportion=0;
	m_GrainArea.SetSize(0,5);

	// 特征值聚类数
	m_KPara=110;
	//仅拾取大于m_HoleMinAcr的孔洞
	m_HoleMinAcr=10.f;
	m_bHoleMinAcr=FALSE;
	//仅拾取小于m_HoleMaxAcr的孔洞
	m_bHoleMaxAcr=FALSE;
	m_HoleMaxAcr=200.f;

	//图像纵横向比例 仅计算园度时使用
	m_Hor=1.f;
	m_Ver=1.f;
	//对面积线性刻度参数s=s*m_fA+m_fB 仅在以直经为条件去除某孔洞时使用;
	m_fA=1.f;
	m_fB=0.f;	
}

CDibImage::CDibImage(const CDibImage& ds)
{
	m_hDib = NULL;
	m_pWorkingArea = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nScanWidth = 0;
	m_nBitsPerPixel = 0;
	m_Pal = NULL;
	m_GrainArea.SetSize(0,4);  
	Attach((HDIB) CopyHandle(ds.m_hDib));
	SetWorkingArea(ds.m_pWorkingArea->Clone());

	// 特征值聚类数
	m_KPara=110;
	//仅拾取大于m_HoleMinAcr的孔洞
	m_HoleMinAcr=10.f;
	m_bHoleMinAcr=FALSE;
	//仅拾取小于m_HoleMaxAcr的孔洞
	m_bHoleMaxAcr=FALSE;
	m_HoleMaxAcr=200.f;

	//仅拾取圆度大于m_HoleMinRound的孔洞
	m_bMinRound=FALSE;
	m_HoleMinRound=70.f;
	m_bMaxRound=FALSE;
	m_HoleMaxRound=30.f;

	//图像纵横向比例 仅计算园度时使用
	m_Hor=1.f;
	m_Ver=1.f;
	//对面积线性刻度参数s=s*m_fA+m_fB 仅在以直经为条件去除某孔洞时使用;
	m_fA=1.f;
	m_fB=0.f;	
}

CDibImage::~CDibImage()
{
	Destroy();
	TRACE("~CDibImage()\n");
	POSITION temp;
	POSITION pos=m_ContourList.GetHeadPosition();	 
	while(pos!=NULL)
	{
		temp=pos;
		m_ContourList.GetNext(pos);
		m_ContourList.RemoveAt(temp);
	}
}


CDibImage& CDibImage::operator=(const CDibImage& ds)
{
	Attach((HDIB) CopyHandle(ds.m_hDib));
	SetWorkingArea(ds.m_pWorkingArea->Clone());
	return *this;
}


BOOL CDibImage::Create(CSize size, WORD nBitCount)
{
	Destroy();

	BOOL bSuccess = TRUE;
	CWindowDC dcScreen(NULL);

	// should not already have palette
	ASSERT(m_Pal == NULL);

	// Fill in the BITMAPINFO Structure
	BITMAPINFO bmi;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = size.cx;
	bmi.bmiHeader.biHeight = size.cy;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = nBitCount;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;
	bmi.bmiHeader.biXPelsPerMeter = 0;
	bmi.bmiHeader.biYPelsPerMeter = 0;
	int nPaletteSize = ComputePaletteSize(nBitCount);
	bmi.bmiHeader.biClrUsed = nPaletteSize;
	bmi.bmiHeader.biClrImportant = nPaletteSize;

	m_nWidth = size.cx;
	m_nHeight = size.cy;
	m_nBitsPerPixel = nBitCount;
	m_nScanWidth = WIDTHBYTES(m_nWidth*m_nBitsPerPixel);
	m_pWorkingArea = new CRectWorkingArea(Rect());
	if(nBitCount==24)
	{
		m_Pal = ::CreateHalftonePalette(dcScreen.m_hDC);
		if (m_Pal == NULL)
			return FALSE;
		DWORD dwSize = sizeof(bmi) + m_nScanWidth*m_nHeight;
		m_hDib = (HDIB) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwSize);
		if (m_hDib == NULL)
			return FALSE;
		LPSTR pDib = (LPSTR) ::GlobalLock((HGLOBAL) m_hDib);
		memcpy(pDib, &bmi, sizeof(bmi));
		::GlobalUnlock((HGLOBAL) m_hDib); 	  
	}
	else if(nBitCount==8)
	{
		int j=sizeof(bmi);
		m_hDib=(HDIB)::GlobalAlloc(GHND,40+sizeof(RGBQUAD)*256+		  
			WIDTHBYTES(Width()*8)*Height());//给灰度图分配空间
		if(m_hDib==NULL)
			return FALSE;
		LPBITMAPINFO pDib = (LPBITMAPINFO) ::GlobalLock((HGLOBAL) m_hDib);
		for(int i=0;i<256;i++)
		{
			pDib->bmiColors[i].rgbRed=i;
			pDib->bmiColors[i].rgbGreen=i;
			pDib->bmiColors[i].rgbBlue=i;
			pDib->bmiColors[i].rgbReserved=0;
		}
		memcpy((LPSTR)pDib, &bmi, 40);
		::GlobalUnlock((HGLOBAL) m_hDib); 	  
		CPalette pal;
		::CreateDIBPalette(m_hDib, &pal);
		m_Pal = (HPALETTE) pal.Detach();	  
	}
	return bSuccess;  
}


int CDibImage::ComputePaletteSize(DWORD nBitCount)
{
	int nSize = 0;
	switch(nBitCount) 
	{
		case 1:  nSize = 2;     break;
		case 4:  nSize = 16;    break;
		case 8:	 nSize = 256;   break;
		case 16:
		case 24:
		case 32: nSize = 0;		  break;
		default: ASSERT(FALSE); break;
	}

	return nSize;
}

void CDibImage::Destroy()
{
	if(m_pWorkingArea)
	{
		delete m_pWorkingArea;
		m_pWorkingArea = NULL;
	}

	if (m_hDib)
	GlobalFree(m_hDib);

	::DeleteObject(m_Pal); m_Pal=NULL;
	::DeleteObject(m_hBitmap); m_hBitmap=NULL;
}


BOOL CDibImage::Attach(HGLOBAL hGlobal)
{
	Destroy();
	BOOL bSuccess = FALSE;
	if(hGlobal)
	{
		m_hDib = (HDIB) hGlobal;
		DWORD dwSize = GlobalSize(m_hDib);
		ASSERT(dwSize);
		LPSTR lpDib = (LPSTR) ::GlobalLock(m_hDib);
		ASSERT(lpDib);
		m_nWidth = ::DIBWidth(lpDib);
		m_nHeight = ::DIBHeight(lpDib);
			::GlobalUnlock((HGLOBAL) m_hDib);
		m_nBitsPerPixel = GetBitsPerPixel();
		m_nScanWidth = WIDTHBYTES(m_nWidth*m_nBitsPerPixel);
		m_pWorkingArea = new CRectWorkingArea(Rect());
		CPalette pal;
			::CreateDIBPalette(m_hDib, &pal);
		m_Pal = (HPALETTE) pal.Detach();
		m_hBitmap=::DIBToDIBSection(m_hDib);
		bSuccess = TRUE;
		//将24位转换成256色的灰度图
		if(BitsPerPixel()==24)
			Greyscale();
	}
	
	return bSuccess;
}

BOOL CDibImage::Load(LPCTSTR lpszPathName)
{
	CFile f;
	if (!f.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite))
		return FALSE;

	TCHAR pszExt[_MAX_EXT];
	_tsplitpath(lpszPathName, NULL, NULL, NULL, pszExt);
	CString sExt(pszExt);   
	if ((sExt.CompareNoCase(_T(".bmp")) == 0) || (sExt.CompareNoCase(_T(".dib")) == 0))
		return Attach(::ReadDIBFile(f));
	else
	{
		TRACE(_T("Could not detemine what file type to load based on the filename extension\n"));
		return FALSE;
	}
}

BOOL CDibImage::Load(HINSTANCE hInst, LPCTSTR lpResourceName)
{
	HRSRC hSrc = ::FindResource(hInst, lpResourceName, RT_BITMAP);
	BOOL bSuccess = FALSE;
	if(hSrc)
	{
		HGLOBAL hResData = ::LoadResource(hInst, hSrc);
		if(hResData)
		{
			LPVOID lpResData = LockResource(hResData);
			if(lpResData)
			{
				DWORD dwSize = SizeofResource(hInst, hSrc);
				if(dwSize)
				{
					HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwSize);
					if (hGlobal)
					{
						LPVOID lpBitmapData = GlobalLock(hGlobal);
						if (lpBitmapData)
						{
							CopyMemory(lpBitmapData, lpResData, dwSize);
							GlobalUnlock(hGlobal);
							bSuccess = Attach(hGlobal);
						}
					}
				}
			}
		}
	}

	return bSuccess;
}

BOOL CDibImage::ReverseGray()
{
	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());
	
	if (m_hDib==INVALID_HANDLE_VALUE) return FALSE;
	
	LPSTR lpDibHdr = (LPSTR)::GlobalLock(m_hDib);
    LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	unsigned char maxGray = 0;
	
	for (int row=r.top; row<r.bottom+1; row++)
	{
		for (int col=r.left; col<r.right+1; col++)
		{
			int x = col;
			int y = row;
			if (m_pWorkingArea->PointInSelection(CPoint(x,y)))
			{
				unsigned char *lpSrc = (unsigned char*)lpDibBits+m_nScanWidth*
					(m_nHeight-1-row)+col;
				
				*lpSrc = (unsigned char)255-*lpSrc;
			}			
		}
	}
	::GlobalUnlock(m_hDib);
	return TRUE;
}

BOOL CDibImage::DoubleGray()
{
	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());
	
	if (m_hDib==INVALID_HANDLE_VALUE) return FALSE;
	
	LPSTR lpDibHdr = (LPSTR)::GlobalLock(m_hDib);
    LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	unsigned char maxGray = 0;
	
	for (int row=r.top; row<r.bottom+1; row++)
	{
		for (int col=r.left; col<r.right+1; col++)
		{
			int x = col;
			int y = row;
			if (m_pWorkingArea->PointInSelection(CPoint(x,y)))
			{
				unsigned char *lpSrc = (unsigned char*)lpDibBits+m_nScanWidth*
					(m_nHeight-1-row)+col;
				FLOAT A;
				
				if(*lpSrc<255) 
                   A=-4*(*lpSrc)+255.f;
				if(A>255)
					*lpSrc=255;
				if(A<0)
					*lpSrc=0;
				else
					*lpSrc=(unsigned char)(A+0.5f);
			}			
		}
	}
	::GlobalUnlock(m_hDib);
	return TRUE;
}

int MinValue(int nArray[], int n)
{
	int min=nArray[0];
	for (int i=1; i<n; i++)
	{
		if (nArray[i]<min) min = nArray[i];
	}
	return min;
}

void CDibImage::DistanceTransform()
{
	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());
	
	if (m_hDib == INVALID_HANDLE_VALUE)
		return ;
	
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	int nArray[] = {255,255,255,255,255};
	int row , col;
	
	for ( row=r.top+1; row<r.bottom+1; row++)
	{
		for ( col=r.left+1; col<r.right; col++)
		{
			int x = col;
			int y = row;
			if (m_pWorkingArea->PointInSelection(CPoint(x,y)))
			{
				unsigned char *lpBit = (unsigned char*)lpDibBits+m_nScanWidth*
					(m_nHeight-1-row)+col;
				nArray[0] = *lpBit;
				nArray[1] = *(lpBit-1)+3;
				nArray[2] = *(lpBit+m_nScanWidth-1)+4;
				nArray[3] = *(lpBit+m_nScanWidth)+3;
				nArray[4] = *(lpBit+m_nScanWidth+1)+4;
				*lpBit = MinValue(nArray, 5);
			}
		}
	}
	for ( row=r.bottom-1; row>r.top-1; row--)
	{
		for ( col=r.right-1; col>r.left; col--)
		{
			int x = col;
			int y = row;
			if (m_pWorkingArea->PointInSelection(CPoint(x,y)))
			{
				unsigned char *lpBit = (unsigned char*)lpDibBits+m_nScanWidth*
					(m_nHeight-1-row)+col;
				nArray[0] = *lpBit;
				nArray[1] = *(lpBit+1)+3;
				nArray[2] = *(lpBit-m_nScanWidth-1)+4;
				nArray[3] = *(lpBit-m_nScanWidth)+3;
				nArray[4] = *(lpBit-m_nScanWidth+1)+4;
				*lpBit = MinValue(nArray, 5);
			}
		}
	}
	::GlobalUnlock(m_hDib);
}

unsigned char CDibImage::FindMaxGray()
{
	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());
	
	if (m_hDib==INVALID_HANDLE_VALUE) return FALSE;
	
	LPSTR lpDibHdr = (LPSTR)::GlobalLock(m_hDib);
    LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	unsigned char maxGray = 0;
	
	for (int row=r.top; row<r.bottom+1; row++)	
	{
		for (int col=r.left; col<r.right+1; col++)
		{
			int x = col;
			int y = row;
			if (m_pWorkingArea->PointInSelection(CPoint(x,y)))
			{
				unsigned char *lpSrc = (unsigned char*)lpDibBits+m_nScanWidth*
					(m_nHeight-1-row)+col;
				if (*lpSrc>maxGray) maxGray = *lpSrc;
				
			}			
		}
	}
	::GlobalUnlock(m_hDib);
	return maxGray;
}

BOOL CompareNear20(CDibImage *pDib, int *pArray, int x, int y)
{
	int curPoint = x+y*(pDib->m_nWidth);
	
	if (pArray[curPoint-2]==0&&pArray[curPoint-1]==0&&pArray[curPoint+1]==0
			&&pArray[curPoint+2]==0&&pArray[curPoint-(pDib->m_nWidth)-2]==0
			&&pArray[curPoint-(pDib->m_nWidth)-1]==0&&pArray[curPoint-(pDib->m_nWidth)]==0
			&&pArray[curPoint-(pDib->m_nWidth)+1]==0&&pArray[curPoint-(pDib->m_nWidth)+2]==0
			&&pArray[curPoint+(pDib->m_nWidth)-2]==0&&pArray[curPoint+(pDib->m_nWidth)-1]==0
			&&pArray[curPoint+(pDib->m_nWidth)]==0&&pArray[curPoint+(pDib->m_nWidth)+1]==0
			&&pArray[curPoint+(pDib->m_nWidth)+2]==0&&pArray[curPoint+2*(pDib->m_nWidth)-1]==0
			&&pArray[curPoint+2*(pDib->m_nWidth)]==0&&pArray[curPoint+2*(pDib->m_nWidth)+1]==0
			&&pArray[curPoint-2*(pDib->m_nWidth)-1]==0&&pArray[curPoint-2*(pDib->m_nWidth)]==0
			&&pArray[curPoint-2*(pDib->m_nWidth)+1]==0)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CompareNear8(CDibImage *pDib, int *pArray, int x, int y, int &mark)
{
	int width = pDib->m_nWidth;
	int curPoint = x+y*width;
	int nClass = 0;
	int prePoint = 0;
	
	for (int m=-1; m<2; m++)
	{
		for (int n=-1; n<2; n++)
		{
			int first = pArray[curPoint+m*width+n];
			if (first>0&&first!=prePoint)
			{
				nClass++;
				if (nClass>1)
				{
					return FALSE;
				}
				prePoint = first;
				mark = first;
			}			
		}
	}
	return TRUE;
}

BOOL CompareNear48(CDibImage *pDib, int *pArray, int x, int y, int &mark)
{
	int width = pDib->m_nWidth;
	int curPoint = x+y*width;
	int nClass = 0;
	int prePoint = 0;
	
	for (int m=-3; m<4; m++)
	{
		for (int n=-3; n<4; n++)
		{
			int first = pArray[curPoint+m*width+n];
			if (first>0)
			{
				mark = first;
				return FALSE;
			}			
		}
	}
	return TRUE;
}

BOOL CDibImage::WaterSheds(unsigned char level)//分水岭分割
{
	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());
	
	if (m_hDib==INVALID_HANDLE_VALUE) return FALSE;
	
	LPSTR lpDibHdr = (LPSTR)::GlobalLock(m_hDib);
    LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	
	int *pMark = new int[m_nHeight*m_nWidth];   //存放标志的数组
	memset(pMark,0,m_nHeight*m_nWidth*sizeof(int));//数组元素全部赋值为0
	int nSeed = 1;
	
	for (int i=level; i>0; i-=1)
	{
		for (int row=r.top+3; row<r.bottom-2; row++)
		{
			for (int col=r.left+3; col<r.right-2; col++)
			{
				int x = col;
				int y = row;
				if (m_pWorkingArea->PointInSelection(CPoint(x,y)))
				{
					unsigned char *lpBit = (unsigned char*)lpDibBits+m_nScanWidth*
						(m_nHeight-1-row)+col;
					int a = *lpBit;
					if (*lpBit==i)
					{
						int nMark = 0;
						if (CompareNear20(this,pMark,x,y))	//周围20个点没做标记。							
						{
							if (CompareNear48(this,pMark,x,y,nMark))//周围48个点没做标记
							{
								pMark[x+y*m_nWidth] = nSeed++;//标号加1
							}
							else
							{
								pMark[x+y*m_nWidth] = nMark;
								continue;
							}
						}

						else if (CompareNear8(this,pMark,x,y,nMark))//周围8个点没做标记
						{
							pMark[x+y*m_nWidth] = nMark;
						}
						else
						{
							*lpBit = 0;//粘连处
							
						}											
					}
				}
			}
		}
	}
	return TRUE;
}

void CDibImage::DeleteContour()
{
	while(!m_ContourList.IsEmpty())
		delete m_ContourList.RemoveHead();
}

//边界轮廓跟踪法求孔洞数和面积,bAdd表示是否是增加孔洞,OSTU法进行二值化
BOOL CDibImage::CountContour(BOOL bAdd)//粘连孔洞的分割
{	
	if(!bAdd)
	{
		POSITION temp;
		POSITION pos=m_ContourList.GetHeadPosition();	 
		while(pos!=NULL)
		{
			temp=pos;
			m_ContourList.GetNext(pos);
			m_ContourList.RemoveAt(temp);
		}
	}
	CDibImage* pCopy=new CDibImage(*this); 
	//将24位转换成256色的灰度图
    if(BitsPerPixel()==24)
		pCopy->Greyscale();

	pCopy->Ostu(GetWorkingArea());
	pCopy->ReverseGray();	
	pCopy->DistanceTransform();
	pCopy->DoubleGray();
	int level = pCopy->FindMaxGray();
	pCopy->WaterSheds(level);
	pCopy->ReverseGray();
	pCopy->Threshold(GetWorkingArea(),255); 
	pCopy->TraceContour(bAdd);
	pCopy->EraseNoise();	
	
	POSITION pos=pCopy->m_ContourList.GetHeadPosition();
	while(pos!=NULL)
	{
		POSITION temp=pos;
		m_ContourList.AddTail(pCopy->m_ContourList.GetNext(pos));
		pCopy->m_ContourList.SetAt(temp,NULL);		
	}
	delete pCopy;
	return TRUE;	
}

BOOL CDibImage::EraseNoise()
{
	CContour* pContour=NULL;
    POSITION pos=m_ContourList.GetHeadPosition();
	POSITION temp=NULL;
	long s;float r;
	while(pos!=NULL)
	{
		temp=pos;
		pContour=(CContour*)m_ContourList.GetNext(pos);
		s=pContour->ComputeArea();
		s*=m_Hor*0.254f;
		s*=m_Ver*0.254f;
		//线性刻度
		s=s*m_fA+m_fB;
		r=(float)sqrt(s/3.1415926)*2.f;
		pContour->m_Radius=r;
		//仅拾取大于m_HoleMinAcr的孔洞
		if(m_bHoleMinAcr&&r<m_HoleMinAcr)
			m_ContourList.RemoveAt(temp);
		//仅拾取小于m_HoleMaxAcr的孔洞
		else if(m_bHoleMaxAcr&&r>m_HoleMaxAcr)
			m_ContourList.RemoveAt(temp);
	}
	return TRUE;
}

BOOL CDibImage::EraseEllipse()
{
	CContour* pContour=NULL;
    POSITION pos=m_ContourList.GetHeadPosition();
	POSITION pos1=NULL;
	float r;
	while(pos!=NULL)
	{
		pos1=pos;
		CContour* pContour=(CContour*)m_ContourList.GetNext(pos);
		r=pContour->ComputeRound(m_Hor,m_Ver);
		//仅拾取圆度大于m_HoleMinRound的孔洞
		if(m_bMinRound&&r<m_HoleMinRound)
			m_ContourList.RemoveAt(pos1);
		//仅拾取圆度小于m_HoleMaxRound的孔洞
		else if(m_bMaxRound&&r>m_HoleMaxRound)
			m_ContourList.RemoveAt(pos1);
	}
	return TRUE;
}
//边界轮廓跟踪法求孔洞数和面积,bAdd表示是否是增加孔洞,OSTU法进行二值化
BOOL CDibImage::GetContour(BOOL bAdd)
{
	if(!bAdd)
	{
		POSITION temp;
		POSITION pos=m_ContourList.GetHeadPosition();	 
		while(pos!=NULL)
		{
			temp=pos;
			m_ContourList.GetNext(pos);
			m_ContourList.RemoveAt(temp);
		}
	}

	if(Ostu(GetWorkingArea()))
	{
		if(TraceContour(bAdd))
		{
			if(m_bHoleMinAcr||m_bHoleMaxAcr)
				EraseNoise();
			if(m_bMinRound||m_bMaxRound)
				EraseEllipse();
			return TRUE;
		}
	}
	return FALSE;	
}

int CDibImage::GetBitsPerPixel() const
{
  int nBitCount;
  if (m_hDib == NULL)
    nBitCount = 0;
  else
  {
  	LPSTR lpDib = (LPSTR) ::GlobalLock(m_hDib);

	  //  Calculate the number of bits per pixel for the DIB.
	  if (IS_WIN30_DIB(lpDib))
		  nBitCount = ((LPBITMAPINFOHEADER)lpDib)->biBitCount;
	  else
		  nBitCount = ((LPBITMAPCOREHEADER)lpDib)->bcBitCount;

	 	::GlobalUnlock((HGLOBAL) m_hDib);
  }

	return nBitCount;
}


BOOL CDibImage::Draw(CDC& dc, const CRect* rcDst, const CRect* rcSrc, CPalette* pPal)
{
  CRect DCRect(Rect());
  if (rcDst)
    DCRect = *rcDst;

  CRect DibRect(Rect());
  if (rcSrc)
    DibRect = *rcSrc;

  CPalette pal;
  pal.Attach(m_Pal);
  CPalette* pPalette = NULL;
  if (pPal)
    pPalette = pPal;
  else
    pPalette = &pal;
  DWORD size=GlobalSize(m_hDib);
  TRACE("%d\n",size);
  BOOL bSuccess = ::PaintDIB(dc.m_hDC, &DCRect, m_hDib,	&DibRect,	pPalette);
  pal.Detach();
  return bSuccess;
}


BOOL CDibImage::GetPixel(int x, int y, COLORREF& value, LPSTR lpDibBits) const
{
  if (m_hDib == NULL)
    return FALSE;

  //Currently only support pixel access for 24 bit images
  //May support other formats in the future
  if (m_nBitsPerPixel == 24)
  {
	  //position is out of range
	  if (x >= m_nWidth || x < 0 || y >= m_nHeight || y < 0)
		  return FALSE;
	  BOOL bLocked = FALSE;
	  if (lpDibBits == NULL)
	  {
		  LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
		  lpDibBits = ::FindDIBBits(lpDibHdr);
		  bLocked = TRUE;
	  }
	  BYTE* pData = (BYTE*) lpDibBits + (m_nHeight-1-y)*m_nScanWidth + x*3;
	  value = RGB(pData[2], pData[1], pData[0]);

	  if (bLocked)
		  ::GlobalUnlock((HGLOBAL) m_hDib);
	  return TRUE;
  }
  else if(m_nBitsPerPixel==8)
  {
	  if (x >= m_nWidth || x < 0 || y >= m_nHeight || y < 0)
		  return FALSE;
	  BOOL bLocked = FALSE;
	  if (lpDibBits == NULL)
	  {
		  LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
		  lpDibBits = ::FindDIBBits(lpDibHdr);
		  bLocked = TRUE;
	  }
	  BYTE* pData = (BYTE*) lpDibBits + (m_nHeight-1-y)*m_nScanWidth + x;
	  value = RGB(pData[0], pData[0], pData[0]);//实际上把pData所指的索引值返回就行了，但由于
	                                            //这函数已经这样定义在这里了，故通过RGB把pData
	                                            //所指的索引值返回

	  if (bLocked)
		  ::GlobalUnlock((HGLOBAL) m_hDib);
	  return TRUE;

  }
  else
	  return FALSE;

}


BOOL CDibImage::SetPixel(int x, int y, const COLORREF& value, LPSTR lpDibBits)
{
  if (m_hDib == NULL)
    return FALSE;

  //Currently only support pixel access for 24 bit images
  //May support other formats in the future
  if (m_nBitsPerPixel == 24)
  {
	  //check the position
	  ASSERT(x < m_nWidth && x >= 0 && y < m_nHeight && y >= 0);
	  BOOL bLocked = FALSE;
	  if (lpDibBits == NULL)
	  {
		  LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
		  lpDibBits = ::FindDIBBits(lpDibHdr);
		  bLocked = TRUE;
	  }
	  BYTE* pData = (BYTE*) lpDibBits + (m_nHeight-1-y)*m_nScanWidth + x*3;  //THIS NEEDS TO BE TESTED
	  pData[0] = GetBValue(value);
	  pData[1] = GetGValue(value);
	  pData[2] = GetRValue(value);
	  if (bLocked)
		  ::GlobalUnlock((HGLOBAL) m_hDib);
	  return TRUE;
  }
  else if(m_nBitsPerPixel==8)
  {
	  ASSERT(x < m_nWidth && x >= 0 && y < m_nHeight && y >= 0);
	  BOOL bLocked = FALSE;
	  if (lpDibBits == NULL)
	  {
		  LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
		  lpDibBits = ::FindDIBBits(lpDibHdr);
		  bLocked = TRUE;
	  }
	  BYTE* pData = (BYTE*) lpDibBits + (m_nHeight-1-y)*m_nScanWidth + x;  //THIS NEEDS TO BE TESTED
	  pData[0] = GetBValue(value);//因为value是索引值的RGB合成，故只求pData[0]就可以了
//	  pData[1] = GetGValue(value);
//	  pData[2] = GetRValue(value);
	  if (bLocked)
		  ::GlobalUnlock((HGLOBAL) m_hDib);
	  return TRUE;

  }
  else
	  return FALSE;

}

BOOL CDibImage::Save(LPCTSTR lpszPathName)
{
  if(m_hDib == NULL)
	  return FALSE;
  
  //Determine the file format to use based on the filename extension
  TCHAR pszExt[_MAX_EXT];
  _tsplitpath(lpszPathName, NULL, NULL, NULL, pszExt);
  CString sExt(pszExt);   
  if((sExt.CompareNoCase(_T(".bmp")) == 0) || (sExt.CompareNoCase(_T(".dib")) == 0))
  {
	  CFile f;
	  if (!f.Open(lpszPathName, CFile::shareDenyWrite | CFile::modeCreate | CFile::modeWrite))
		  return FALSE;
	  else
		  return SaveDIB(m_hDib, f);	
  }
  else
  {
	  TRACE(_T("Could not determine what file format to save as\n"));
	  return FALSE; //unknown file format
  } 
}


BOOL CDibImage::CopySelection(CDibImage& dib)
{
  if (m_hDib == NULL)
    return FALSE;

  BOOL bSuccess = FALSE;
  ASSERT(m_pWorkingArea);
  CRect r = m_pWorkingArea->BoundingRectangle();
  bSuccess = dib.Create(CSize(r.Width(), r.Height()), BitsPerPixel());

  if (bSuccess)
  {
 	  LPSTR lpDibHdrSrc  = (LPSTR) ::GlobalLock(m_hDib);
	  LPSTR lpDibBitsSrc = ::FindDIBBits(lpDibHdrSrc);
 	  LPSTR lpDibHdrDest  = (LPSTR) ::GlobalLock(dib.m_hDib);
	  LPSTR lpDibBitsDest = ::FindDIBBits(lpDibHdrDest);
    for (int y=r.top; bSuccess && y<r.bottom; y++)
    {
      for (int x=r.left; bSuccess && x<r.right; x++)
      {
        if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
        {
          COLORREF c;
          bSuccess = GetPixel(x, y, c, lpDibBitsSrc);
          bSuccess = bSuccess && dib.SetPixel(x-r.left, y-r.top, c, lpDibBitsDest);
        }
      }
    }
    GlobalUnlock((HGLOBAL) dib.m_hDib);
    GlobalUnlock((HGLOBAL) m_hDib);
  }

  return bSuccess;
}

LPSTR CDibImage::GetDIBBits()
{
 	LPSTR lpDibHdrSrc  = (LPSTR) ::GlobalLock(m_hDib);
	return ::FindDIBBits(lpDibHdrSrc);
}

BOOL CDibImage::CopyToClipboard()
{
	if (m_hDib == NULL)
	return FALSE;

	BOOL bSuccess = FALSE;
	if (OpenClipboard(NULL))
	{
		if (EmptyClipboard())
		{
			CDibImage DibCopy;
			bSuccess = CopySelection(DibCopy);
			if (bSuccess)
			{
				HDIB hNewDib = (HDIB) CopyHandle(DibCopy.m_hDib);
					bSuccess = (::SetClipboardData(CF_DIB, hNewDib) != NULL);
			}
		}

		CloseClipboard();
	}
	return bSuccess;
}


BOOL CDibImage::PasteFromClipboard()
{
	BOOL bSuccess = FALSE;
	if (OpenClipboard(NULL))
	{
		HDIB hNewDib = (HDIB) CopyHandle(::GetClipboardData(CF_DIB));
		CloseClipboard();
		if (hNewDib != NULL)
		bSuccess = Attach(hNewDib);
	}

	return bSuccess;
}


BOOL CDibImage::PasteAvailable()
{
	return IsClipboardFormatAvailable(CF_DIB);
}

CWorkingArea* CDibImage::GetWorkingArea()
{
	return m_pWorkingArea;
}


void CDibImage::SetWorkingArea(CWorkingArea* pWorkingArea)
{
	if (m_pWorkingArea)
	{
		delete m_pWorkingArea;
		m_pWorkingArea = NULL;
	}
	m_pWorkingArea = pWorkingArea;
}


BOOL CDibImage::SetColor(COLORREF color)
{
	if (m_hDib == NULL)
		return FALSE;
	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());
	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
				bSuccess = SetPixel(x, y, color, lpDibBits);
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	return bSuccess;
}


BOOL CDibImage::Flip()
{
	if (m_hDib == NULL)
		return FALSE;

	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());

	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);


	int nMiddle = r.Height()/2 + r.top;

	for (int y=nMiddle; bSuccess && y>r.top; y--)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c1;
				int y1 = nMiddle + (nMiddle - y);
				bSuccess = GetPixel(x, y1, c1, lpDibBits);
			    
				COLORREF c2;
				bSuccess = bSuccess && GetPixel(x, y, c2, lpDibBits);

				bSuccess = bSuccess && SetPixel(x, y, c1, lpDibBits);
				bSuccess = bSuccess && SetPixel(x, y1, c2, lpDibBits);
			}
		}
	}

	GlobalUnlock((HGLOBAL) m_hDib);
	return bSuccess;
}


BOOL CDibImage::Mirror()
{
	if (m_hDib == NULL)
		return FALSE;

	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());
	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	int nMiddle = r.Width()/2 + r.left;
	for (int x=nMiddle; bSuccess && x>r.left; x--)
	{
		for (int y=r.top; bSuccess && y<r.bottom; y++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
			COLORREF c1;
			int x1 = nMiddle + (nMiddle - x);
			bSuccess = GetPixel(x1, y, c1, lpDibBits);
		    
			COLORREF c2;
			bSuccess = bSuccess && GetPixel(x, y, c2, lpDibBits);

			bSuccess = bSuccess && SetPixel(x, y, c1, lpDibBits);
			bSuccess = bSuccess && SetPixel(x1, y, c2, lpDibBits);
			}
		}
	}

	GlobalUnlock((HGLOBAL) m_hDib);
	return bSuccess;
}


BOOL CDibImage::AdjustBrightness(int Percentage)
{
	if (m_hDib == NULL)
		return FALSE;

	if (Percentage < 0)
		return FALSE;


	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());

	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c;
				if (GetPixel(x, y, c, lpDibBits))
				{
					int r = min(GetRValue(c)*Percentage/100, 255);
					int g = min(GetGValue(c)*Percentage/100, 255);
					int b = min(GetBValue(c)*Percentage/100, 255);
					r = max(r, 0);
					g = max(g, 0);
					b = max(b, 0);
					c = RGB(r, g, b);
					bSuccess = SetPixel(x, y, c, lpDibBits);
				}
				else
					bSuccess = FALSE;
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);

	return bSuccess;
}


BOOL CDibImage::AdjustContrast(int Percentage)
{
	if (m_hDib == NULL)
		return FALSE;

	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());

	BOOL bSuccess = TRUE;

	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c;
				bSuccess = GetPixel(x, y, c, lpDibBits);
				int r = min(128 + ((GetRValue(c) - 128)*Percentage/100), 255);
				int g = min(128 + ((GetGValue(c) - 128)*Percentage/100), 255);
				int b = min(128 + ((GetBValue(c) - 128)*Percentage/100), 255);
				r = max(r, 0);
				g = max(g, 0);
				b = max(b, 0);
				if(BitsPerPixel()==24)
				{
					c = RGB(r, g, b);
					bSuccess = SetPixel(x, y, c, lpDibBits);
				}
				else if(BitsPerPixel()==8)
				{
					int m_Color=(r*30+g*59+b*11)/100;//转化为灰度处理
					c=RGB(m_Color,m_Color,m_Color);
					bSuccess=SetPixel(x,y,c,lpDibBits);
				}
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);

	return bSuccess;
}


BOOL CDibImage::AdjustGammaCorrection(float Value)
{
	if(m_hDib == NULL)
		return FALSE;

	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());

	BOOL bSuccess = TRUE;
	double MaxRange = pow((double) 255, (double) Value) / 255;


	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c;
				bSuccess = GetPixel(x, y, c, lpDibBits);

				double dblR = pow((double) GetRValue(c), (double) Value) / MaxRange;
				double dblG = pow((double) GetGValue(c), (double) Value) / MaxRange;
				double dblB = pow((double) GetBValue(c), (double) Value) / MaxRange;

				int r = min((int) (dblR+0.5), 255);
				int g = min((int) (dblG+0.5), 255);
				int b = min((int) (dblB+0.5), 255);
				r = max(r, 0);
				g = max(g, 0);
				b = max(b, 0);
				if(BitsPerPixel()==24)
				{
					c = RGB(r, g, b);
					bSuccess = SetPixel(x, y, c, lpDibBits);
				}
				else if(BitsPerPixel()==8)
				{
					int m_Color=(r*30+g*59+b*11)/100;//转化为灰度处理
					c=RGB(m_Color,m_Color,m_Color);
					bSuccess=SetPixel(x,y,c,lpDibBits);
				}					
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	return bSuccess;
}


BOOL CDibImage::AdjustHighLight(int Percentage)
{
	if (m_hDib == NULL)
		return FALSE;

	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());

	BOOL bSuccess = TRUE;

	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c;
				bSuccess = GetPixel(x, y, c, lpDibBits);
				int r = GetRValue(c);
				int g = GetGValue(c);
				int b = GetBValue(c);
				int l = r+g+b;
				if ((l > (170*3)))
				{
					r = min(r*Percentage/100, 255);
					g = min(g*Percentage/100, 255);
					b = min(b*Percentage/100, 255);
					r = max(r, 0);
					g = max(g, 0);
					b = max(b, 0);
					c = RGB(r, g, b);
					bSuccess = SetPixel(x, y, c, lpDibBits);
				}
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	return bSuccess;
}


BOOL CDibImage::AdjustMidtone(int Percentage)
{
	if (m_hDib == NULL)
		return FALSE;

	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());
	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c;
				bSuccess = GetPixel(x, y, c, lpDibBits);
				int r = GetRValue(c);
				int g = GetGValue(c);
				int b = GetBValue(c);
				int l = r+g+b;
				if ((l >= (85*3)) && (l <= (170*3)))
				{
					r = min(r*Percentage/100, 255);
					g = min(g*Percentage/100, 255);
					b = min(b*Percentage/100, 255);
					r = max(r, 0);
					g = max(g, 0);
					b = max(b, 0);
					c = RGB(r, g, b);
					bSuccess = SetPixel(x, y, c, lpDibBits);
				}
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	return bSuccess;
}


BOOL CDibImage::AdjustShadow(int Percentage)
{
	if (m_hDib == NULL)
		return FALSE;

	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());
	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c;
				bSuccess = GetPixel(x, y, c, lpDibBits);
				int r = GetRValue(c);
				int g = GetGValue(c);
				int b = GetBValue(c);
				int l = r+g+b;
				if (l < (85*3))
				{
					r = min(r*Percentage/100, 255);
					g = min(g*Percentage/100, 255);
					b = min(b*Percentage/100, 255);
					r = max(r, 0);
					g = max(g, 0);
					b = max(b, 0);
					c = RGB(r, g, b);
					bSuccess = SetPixel(x, y, c, lpDibBits);
				}
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	return bSuccess;
}


BOOL CDibImage::AdjustHue(int Percentage)
{
	if (m_hDib == NULL)
		return FALSE;

	if (Percentage < 0)
		return FALSE;

	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());

	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c;
				if (GetPixel(x, y, c, lpDibBits))
				{
					double H;
					double S;
					double L;
					RGBtoHSL(c, &H, &S, &L);
					H = (H*Percentage/100);
					bSuccess = SetPixel(x, y, HLStoRGB(H, L, S), lpDibBits);
				}
				else
					bSuccess = FALSE;
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	return bSuccess;
}


BOOL CDibImage::AdjustSaturation(int Percentage)
{
	if (m_hDib == NULL)
		return FALSE;

	if (Percentage < 0)
		return FALSE;

	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());

	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c;
				if (GetPixel(x, y, c, lpDibBits))
				{
					double H;
					double S;
					double L;
					RGBtoHSL(c, &H, &S, &L);
					S = (S*Percentage/100);
					bSuccess = SetPixel(x, y, HLStoRGB(H, L, S), lpDibBits);
				}
				else
					bSuccess = FALSE;
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	return bSuccess;
}

BOOL CDibImage::AdjustHSL(int PercentHue, int PercentSaturation, int PercentLuminosity)
{
	if (m_hDib == NULL)
		return FALSE;

	if (PercentHue < 0)
		return FALSE;

	if (PercentSaturation < 0)
		return FALSE;

	if (PercentLuminosity < 0)
		return FALSE;

	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());

	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c;
				if (GetPixel(x, y, c, lpDibBits))
				{
					double H;
					double S;
					double L;
					RGBtoHSL(c, &H, &S, &L);
					S = (S*PercentSaturation/100);
					H = (H*PercentHue/100);
					L = (L*PercentLuminosity/100);
					bSuccess = SetPixel(x, y, HLStoRGB(H, L, S), lpDibBits);
				}
				else
					bSuccess = FALSE;
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	return bSuccess;
}

BOOL CDibImage::AdjustFrame(int left,int top,int right,int bottom)
{
	if((left+right>=Width()-1)||(top+bottom>=Height()-1))
		return FALSE;
	CRect rc(left,top,Width()-right,Height()-bottom);
	SetWorkingArea(new CRectWorkingArea(rc));
	CDibImage dibCopy;
	if(!CopySelection(dibCopy))	
		return FALSE;
	HDIB hNewDib = (HDIB) CopyHandle(dibCopy.m_hDib);
	if(!Attach(hNewDib))
		return FALSE;
	return TRUE;
}

WORD CDibImage::GetVersion()
{
	return 0x0101;  //ie 1.01     

	/* Revision History

	1.0  18/11/1997:  Initial Public Release
	1.01 10/11/1998:  See help file for changes

	*/
}


void CDibImage::RGBtoHSL(COLORREF rgb, double* H, double* S, double* L)
{
	double delta;
	double r = (double)GetRValue(rgb)/255;
	double g = (double)GetGValue(rgb)/255;
	double b = (double)GetBValue(rgb)/255;
	double cmax = max(r,max(g,b));
	double cmin = min(r,min(g,b));
	*L = (cmax+cmin)/2.0;
	if (cmax == cmin) 
	{
		*S = 0;
		*H = 0; // it's really undefined
	} 
	else 
	{
		if (*L < 0.5) 
			*S = (cmax-cmin)/(cmax+cmin);
		else
			*S = (cmax-cmin)/(2.0-cmax-cmin);
		delta = cmax - cmin;
		if (r==cmax)
			*H = (g-b)/delta;
		else if (g==cmax)
			*H = 2.0 +(b-r)/delta;
		else
			*H = 4.0+(r-g)/delta;
		*H /= 6.0;
		if (*H < 0.0)
			*H += 1;
	}
}


double CDibImage::HuetoRGB(double m1, double m2, double h)
{
	if (h < 0) 
		h += 1.0;
	if (h > 1) 
		h -= 1.0;
	if (6.0*h < 1)
		return (m1+(m2-m1)*h*6.0);
	if (2.0*h < 1)
		return m2;
	if (3.0*h < 2.0)
		return (m1+(m2-m1)*((2.0/3.0)-h)*6.0);
	return m1;
}


COLORREF CDibImage::HLStoRGB(const double& H, const double& L, const double& S)
{
	double r,g,b;
	double m1, m2;

	if (S==0) 
	{
		r=g=b=L;
	} 
	else 
	{
		if (L <= 0.5)
			m2 = L*(1.0+S);
		else
			m2 = L+S-L*S;
		m1 = 2.0*L-m2;
		r = HuetoRGB(m1, m2, H+1.0/3.0);
		g = HuetoRGB(m1, m2, H);
		b = HuetoRGB(m1, m2, H-1.0/3.0);
	}
	return RGB((BYTE)(r*255),(BYTE)(g*255),(BYTE)(b*255));
}


BOOL CDibImage::AdjustRed(int Percentage)
{
	if (m_hDib == NULL)
		return FALSE;

	if (Percentage < 0)
		return FALSE;

	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());

	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
			COLORREF c;
			if (GetPixel(x, y, c, lpDibBits))
			{
				int r = min(GetRValue(c)*Percentage/100, 255);
				r = max(r, 0);
				c = RGB(r, GetGValue(c), GetBValue(c));
				bSuccess = SetPixel(x, y, c, lpDibBits);
			}
			else
				bSuccess = FALSE;
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	return bSuccess;
}


BOOL CDibImage::AdjustGreen(int Percentage)
{
	if (m_hDib == NULL)
		return FALSE;

	if (Percentage < 0)
		return FALSE;

	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());

	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c;
				if (GetPixel(x, y, c, lpDibBits))
				{
					int g = min(GetGValue(c)*Percentage/100, 255);
					g = max(g, 0);
					c = RGB(GetRValue(c), g, GetBValue(c));
					bSuccess = SetPixel(x, y, c, lpDibBits);
				}
				else
					bSuccess = FALSE;
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	return bSuccess;
}


BOOL CDibImage::AdjustBlue(int Percentage)
{
	if (m_hDib == NULL)
		return FALSE;

	if (Percentage < 0)
		return FALSE;

	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());

	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c;
				if (GetPixel(x, y, c, lpDibBits))
				{
					int b = min(GetBValue(c)*Percentage/100, 255);
					b = max(b, 0);
					c = RGB(GetRValue(c), GetGValue(c), b);
					bSuccess = SetPixel(x, y, c, lpDibBits);
				}
				else
					bSuccess = FALSE;
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	return bSuccess;
}

BOOL CDibImage::AdjustColor(int dr, int dg, int db)
{
	BYTE r, g, b;

	if (m_hDib == NULL)
		return FALSE;
	
	ASSERT(m_pWorkingArea);
	CRect rc(m_pWorkingArea->BoundingRectangle());
	
	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=rc.top; bSuccess && y<rc.bottom; y++)
	{
		for (int x=rc.left; bSuccess && x<rc.right; x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c;
				if (GetPixel(x, y, c, lpDibBits))
				{
					r=GetRValue(c);g=GetGValue(c);b=GetBValue(c);
					r=BOUND(r+dr,0,255);
					g=BOUND(g+dg,0,255);
					b=BOUND(b+db,0,255);
					if(BitsPerPixel()==24)
					{
						c = RGB(r, g, b);
						bSuccess = SetPixel(x, y, c, lpDibBits);
					}
					else if(BitsPerPixel()==8)
					{
						int m_Color=(r*30+g*59+b*11)/100;//转化为灰度处理
						c=RGB(m_Color,m_Color,m_Color);
						bSuccess=SetPixel(x,y,c,lpDibBits);
					}					
				}
				else
					bSuccess = FALSE;
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	
	return bSuccess;
}

BOOL CDibImage::Greyscale()
{
	if (m_hDib == NULL)
		return FALSE;
	m_Proportion=0;

	if(GetBitsPerPixel()==24)
	{
		LPBITMAPINFOHEADER lpDibHdr;//BITMAPINFORHEADER指针
		LPSTR lpDibBits;//DIB象素指针
		lpDibHdr=(LPBITMAPINFOHEADER)::GlobalLock((HGLOBAL)m_hDib);

		HDIB m_hGrayDib;//转换为灰度图后的图象句柄
	    LPBITMAPINFO lpGrayDibdr;
	    LPBITMAPINFOHEADER lpGrayDibHdr;
	    LPSTR lpGrayDibBits;
	    CPalette* m_pGraypalDib;
	    LPLOGPALETTE lpPal;
	    HANDLE hLogPal;
		m_hGrayDib=(HDIB)::GlobalAlloc(GHND,lpDibHdr->biSize+
		               sizeof(RGBQUAD)*256+
					   WIDTHBYTES(Width()*8)*Height());//给灰度图分配空间
	   if(m_hGrayDib==NULL)
	   {
		   ::GlobalUnlock((HGLOBAL)m_hDib);
		   return  FALSE;
	   }
	   lpGrayDibHdr=(LPBITMAPINFOHEADER)::GlobalLock((HGLOBAL)m_hGrayDib);
	   //复制BITMAPINFORHEADER结构
	   memcpy((void*)lpGrayDibHdr,(void*)lpDibHdr,40);
	   lpGrayDibHdr->biBitCount=8;
	   lpGrayDibHdr->biSizeImage=WIDTHBYTES(Width()*8)*Height();

	   //以下创建调色板
       m_pGraypalDib=new CPalette;
	   if(m_pGraypalDib==NULL)
	   {
		   ::GlobalUnlock((HGLOBAL)m_hDib);
		   ::GlobalUnlock((HGLOBAL)m_hGrayDib);
		   ::GlobalFree((HGLOBAL)m_hGrayDib);
		   return  FALSE;
	   }
	   hLogPal=::GlobalAlloc(GHND,sizeof(LOGPALETTE)+sizeof(PALETTEENTRY)*256);
	   if(hLogPal==NULL)
	   {
		   ::GlobalUnlock((HGLOBAL)m_hDib);
		   delete m_pGraypalDib;
		   ::GlobalUnlock((HGLOBAL)m_hGrayDib);
		   ::GlobalFree((HGLOBAL)m_hGrayDib );
		   return  FALSE;
	   }
	   lpPal=(LPLOGPALETTE)::GlobalLock((HGLOBAL)hLogPal);
	   lpPal->palVersion=PALVERSION;
	   lpPal->palNumEntries=256;
	   lpGrayDibdr=(LPBITMAPINFO)lpGrayDibHdr;
	   for(int i=0;i<256;i++)
	   {
		   lpPal->palPalEntry[i].peRed=i;
		   lpPal->palPalEntry[i].peGreen=i;
		   lpPal->palPalEntry[i].peBlue=i;
		   lpPal->palPalEntry[i].peFlags=0;
		   lpGrayDibdr->bmiColors[i].rgbRed=i;
		   lpGrayDibdr->bmiColors[i].rgbGreen=i;
		   lpGrayDibdr->bmiColors[i].rgbBlue=i;
		   lpGrayDibdr->bmiColors[i].rgbReserved=0;
	   }
	   m_pGraypalDib->CreatePalette(lpPal);
	
	   ::GlobalUnlock((HGLOBAL)hLogPal);
	   ::GlobalFree((HGLOBAL)hLogPal);

	   //以下向m_hGrayDIB写入图象数据
	   lpDibBits=::FindDIBBits((LPSTR)lpDibHdr);
       lpGrayDibBits=(LPSTR)lpGrayDibHdr+40+256*sizeof(RGBQUAD);

	   LONG m_GrayWidth=WIDTHBYTES(Width()*8);//灰度图象的宽度(字节数)
	   LONG m_Width=WIDTHBYTES(Width()*24);//彩色图象的宽度(字节数)
	   for(LONG j=0;j<Height();j++)
	   {
		   BYTE* lpSrc=(BYTE*)lpDibBits+m_Width*j;
		   BYTE* lpGrayDes=(BYTE*)lpGrayDibBits+m_GrayWidth*j;
		   for(LONG k=0;k<Width();k++)
		   {
			   BYTE* pbyBlue=lpSrc++;
			   BYTE* pbyGreen=lpSrc++;
		       BYTE* pbyRed=lpSrc++;
			   BYTE r=*pbyRed;
			   BYTE g=*pbyGreen;
			   BYTE b=*pbyBlue;
			   BYTE gray=(BYTE)(((WORD)r*30+(WORD)g*59+(WORD)b*11)/100);
			   *lpGrayDes=gray;
			   lpGrayDes++;
		   }
		   m_Proportion=j/(float)(Height()-1);
	   }
	   ::GlobalUnlock((HGLOBAL)m_hDib);
	   ::GlobalUnlock((HGLOBAL)m_hGrayDib);
	   if(m_hDib!=NULL)
	   {
		   ::GlobalFree((HGLOBAL)m_hDib);
	   }
	   m_hDib=m_hGrayDib;
       m_nBitsPerPixel = GetBitsPerPixel();
       m_nScanWidth = WIDTHBYTES(m_nWidth*m_nBitsPerPixel);
       ::DeleteObject(m_Pal);
       m_Pal = (HPALETTE)m_pGraypalDib->Detach();
	   delete m_pGraypalDib;
	   return  TRUE;
	}
	else
		return FALSE;
}

//迭代法求阈值
BOOL CDibImage::Iteration()	
{
    CRect rt(m_pWorkingArea->BoundingRectangle());
	if(rt.Width()*rt.Height()==0)
		return FALSE;
	long lHistogram[256]={0};					//直方图数组
	unsigned char iThreshold,iNewThreshold,     //阈值，最大与最小灰度值，两个区域的平均灰度值
		iMaxGrayValue,iMinGrayValue,iMean1GrayValue,iMean2GrayValue;
	long lP1,lP2,lS1,lS2;//用于计算区域灰度平均值的中间变量
	int iIterationTimes;//迭代次数
	GetRedHistogram(lHistogram,256);//因为是灰度图像,故其灰度数等于红(篮,绿)色分量
	iMaxGrayValue=255;
	iMinGrayValue=0;
	BOOL bSuccess=FALSE;
	for(int i=0;i<256&&!bSuccess;i++)
	{
		if(lHistogram[i]>0)
		{
			bSuccess=TRUE;
			iMinGrayValue=i;
		}
	}
	bSuccess=FALSE;
	for(i=255;i>=0&&!bSuccess;i--)
	{
		if(lHistogram[i]>0)
		{
			bSuccess=TRUE;
			iMaxGrayValue=i;
		}
	}
	iNewThreshold=(iMaxGrayValue+iMinGrayValue)/2;
	for(iIterationTimes=0;iThreshold!=iNewThreshold&&iIterationTimes<100;iIterationTimes++)
	{
		iThreshold=iNewThreshold;
		lP1=0;lP2=0;lS1=0;lS2=0;
		for(i=iMinGrayValue;i<iThreshold;i++)//求两个区域的灰度平均值
		{
			lP1+=lHistogram[i]*i;
			lS1+=lHistogram[i];
		}
		if(lS1!=0)
			iMean1GrayValue=(unsigned char)(lP1/lS1);
		for(i=iThreshold+1;i<=iMaxGrayValue;i++)
		{
			lP2+=lHistogram[i]*i;
			lS2+=lHistogram[i];
		}
		if(lS2!=0)
			iMean2GrayValue=(unsigned char)(lP2/lS2);
		iNewThreshold=(iMean1GrayValue+iMean2GrayValue)/2;
	}
	TRACE("Threshold %d\n",iThreshold);
	Threshold(m_pWorkingArea,iThreshold);
	return TRUE;
}

//极小值求阈值
BOOL CDibImage::MinValueSegmentation(CWorkingArea* m_pSegArea)
{
	if (m_hDib == NULL)
		return FALSE;
	
	BYTE   m_Thre=-1;//待求的阈值
	long   m_GrayCount[256];//每一个灰度级的数目
	long   maxValueNum=0;
	int    peakGrey=0;
	int    peakGreyT=0;
	int    valleyGrey=0;
	int    total=0;
	int    back=0;

	CWorkingArea* m_pOldWorkingArea=m_pWorkingArea;//保存原有的WorkingArea
	m_pWorkingArea=m_pSegArea;
	GetRedHistogram(m_GrayCount,256);//因为是灰度图像,故其灰度数等于红(篮,绿)色分量
	m_pWorkingArea=m_pOldWorkingArea;//恢复原有的WorkingArea

	for(int i=0;i<255;i++)
	{
		if(m_GrayCount[i]>maxValueNum)
		{
			maxValueNum=m_GrayCount[i];
			peakGrey=i;
		}
		total+=m_GrayCount[i];
	}

	for(i=peakGrey-20;i<256&&i<peakGrey+20;i++)
	{
		back+=m_GrayCount[i];
	}
    float rate=(float)back/total;
	if(rate<0.7)
	{
		Ostu(m_pSegArea);
		return TRUE;
	}

	int critical=1000;
	for( i=peakGrey;i>0&&m_GrayCount[i]>critical;i--)
	{
		if(m_GrayCount[i]<maxValueNum)
		{
			maxValueNum=m_GrayCount[i];
			valleyGrey=i;
		}
	}
	for(i=valleyGrey;m_GrayCount[i]<=m_GrayCount[i+1];i--)
		valleyGrey=i;

	maxValueNum=0;
	for(i=0;i<valleyGrey;i++)
	{
		if(m_GrayCount[i]>maxValueNum)
		{
			maxValueNum=m_GrayCount[i];
			peakGreyT=i;
		}
	}

	for(i=peakGreyT;i<peakGrey;i++)	
	{
		if(m_GrayCount[i]<maxValueNum)
		{
			maxValueNum=m_GrayCount[i];
			valleyGrey=i;
		}
	}

	m_Thre=valleyGrey;
	Threshold(m_pSegArea,m_Thre);
	return TRUE;
}

//最大类间方差法求阈值
BOOL CDibImage::Ostu(CWorkingArea* m_pSegArea)
{
	if (m_hDib == NULL)
		return FALSE;
	
	float m_Pi[256]={0};//灰度级为i的象素的概率
	BYTE m_Thre=-1;//待求的阈值
	double m_W0=0;//灰度级从0-m_Thre的象素的概率总和
    double m_U0=0;//灰度级从0-m_Thre的象素的均值
	double m_W1=0;
	double m_U1=0;
	float m_Utotal=0;//总平均值
	float m_FangCha=0;//类间方差
    long   m_GrayCount[256];//每一个灰度级的数目
	CWorkingArea* m_pOldWorkingArea=m_pWorkingArea;//保存原有的WorkingArea
	m_pWorkingArea=m_pSegArea;
	GetRedHistogram(m_GrayCount,256);//因为是灰度图像,故其灰度数等于红(篮,绿)色分量
	m_pWorkingArea=m_pOldWorkingArea;//恢复原有的WorkingArea
	
	long total=0;//总的象素数
	for(int i=0;i<255;i++)
		total+=m_GrayCount[i];
	for(i=0;i<=255;i++)
	{
		for(int j=0;j<=i;j++)
		{
			m_W0+=m_GrayCount[j];
			m_U0+=j*m_GrayCount[j];
		}
		if(m_W0!=0)
		{
			m_U0=m_U0/m_W0;
			m_W0=m_W0/(double)(total);
		}
		for(j=i+1;j<=255;j++)
		{
			m_W1+=m_GrayCount[j];
			m_U1+=j*m_GrayCount[j];
		}		
		if(m_W1!=0)
		{
			m_U1=m_U1/m_W1;
			m_W1=m_W1/(double)(total);
		}
		m_Utotal=float(m_W0*m_U0+m_W1*m_U1);
		float k=0;
		k=float(m_W0*m_W1*(m_U0-m_U1)*(m_U0-m_U1));
		if(m_FangCha<k)
		{
			m_FangCha=k;
//			if(abs(int(m_U0-m_U1))>20) 
//				m_Thre=i;
			m_Thre=i;
		}
		m_W0=0;m_U0=0;m_W1=0;m_U1=0;
	}
	if(m_Thre-m_KPara>15)
		m_Thre-=m_KPara;
	else
		m_Thre=15;
	Threshold(m_pSegArea,m_Thre);
	return TRUE;
}

BOOL CDibImage::HandThreshold(int m_Threshold)
{
	if (m_hDib == NULL)
		return FALSE;
	Threshold(GetWorkingArea(),m_Threshold);
	return TRUE;
}

//阈值分割
BOOL CDibImage::Threshold(CWorkingArea* m_pSegArea,int m_Threshold)
{
	if (m_hDib == NULL)
		return FALSE;
    m_Proportion=0;
	LPBITMAPINFOHEADER lpDIBHdr;
	lpDIBHdr=(LPBITMAPINFOHEADER)::GlobalLock((HGLOBAL)m_hDib);
	LPSTR lpDIBBits=::FindDIBBits((LPSTR)lpDIBHdr);
	BYTE* lpSrc;

	CRect rect(m_pSegArea->BoundingRectangle());	
	for(LONG i=rect.top;i<rect.bottom;i++)
	{
		for(LONG j=rect.left;j<rect.right;j++)
		{
			if (m_pSegArea->PointInSelection(CPoint(j, i)))
			{
				lpSrc=(BYTE*)lpDIBBits+ScanWidth()*(Height()-1-i)+j;
				if((*lpSrc)<m_Threshold)
					*lpSrc=0;
				else
					*lpSrc=255;
			}
		}
		m_Proportion=(i-rect.top)/(float)(rect.Height()-1);
	}
	::GlobalUnlock((HGLOBAL)m_hDib);
	return TRUE;
}

BOOL CDibImage::Negate()
{
	if (m_hDib == NULL)
		return FALSE;

	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());

	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c;
				if (GetPixel(x, y, c, lpDibBits))
					bSuccess = SetPixel(x, y, RGB(255 - GetRValue(c), 255 - GetGValue(c), 255 - GetBValue(c)), lpDibBits);
				else
					bSuccess = FALSE;
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	return bSuccess;
}

//Sobel水平算子与垂直算子的结合，取二者的较大值
BOOL CDibImage::FindEdgesFilter()
{
	if (m_hDib == NULL)
		return FALSE;

	C3By3Filter Filter1;
	Filter1.m_nValues[0][0] = -1;
	Filter1.m_nValues[0][1] = 0;
	Filter1.m_nValues[0][2] = 1;
	Filter1.m_nValues[1][0] = -2;
	Filter1.m_nValues[1][1] = 0;
	Filter1.m_nValues[1][2] = 2;
	Filter1.m_nValues[2][0] = -1;
	Filter1.m_nValues[2][1] = 0;
	Filter1.m_nValues[2][2] = 1;


	C3By3Filter Filter2;
	Filter2.m_nValues[0][0] = -1;
	Filter2.m_nValues[0][1] = -2;
	Filter2.m_nValues[0][2] = -1;
	Filter2.m_nValues[1][0] = 0;
	Filter2.m_nValues[1][1] = 0;
	Filter2.m_nValues[1][2] = 0;
	Filter2.m_nValues[2][0] = 1;
	Filter2.m_nValues[2][1] = 2;
	Filter2.m_nValues[2][2] = 1;

	m_Proportion=0;

	CDibImage dibCopy;
	CopySelection(dibCopy);	  

	ASSERT(dibCopy.m_pWorkingArea);
	CRect rectSrc(dibCopy.m_pWorkingArea->BoundingRectangle());
	CRect rectDest(m_pWorkingArea->BoundingRectangle());

	BOOL bSuccess = TRUE;
	LPSTR lpDibHdrSrc   = (LPSTR) ::GlobalLock(dibCopy.m_hDib);
	LPSTR lpDibBitsSrc  = ::FindDIBBits(lpDibHdrSrc);
	LPSTR lpDibHdrDest  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBitsDest = ::FindDIBBits(lpDibHdrDest);
	for (int y=0; bSuccess && y<rectSrc.Height(); y++)
	{
		for (int x=0; bSuccess && x<rectSrc.Width(); x++)
		{
			if (dibCopy.m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c1 = Filter1.Filter(dibCopy, lpDibBitsSrc, x, y);
				COLORREF c2 = Filter2.Filter(dibCopy, lpDibBitsSrc, x, y);
				int r = max(GetRValue(c1), GetRValue(c2));
				int g = max(GetGValue(c1), GetGValue(c2));
				int b = max(GetBValue(c1), GetBValue(c2));
				bSuccess = SetPixel(x+rectDest.left, y+rectDest.top, RGB(r, g, b), lpDibBitsDest);
			}
		}
		m_Proportion=y/(float)(rectSrc.Height()-1);	  
	}
	GlobalUnlock((HGLOBAL) m_hDib);	  
	return bSuccess;
}

//Prewitt法进行边缘检测
BOOL CDibImage::Prewitt()  
{
	if (m_hDib == NULL)
		return FALSE;
	
	C3By3Filter Filter1;
	Filter1.m_nValues[0][0] = -1;
	Filter1.m_nValues[0][1] = 0;
	Filter1.m_nValues[0][2] = 1;
	Filter1.m_nValues[1][0] = -1;
	Filter1.m_nValues[1][1] = 0;
	Filter1.m_nValues[1][2] = 1;
	Filter1.m_nValues[2][0] = -1;
	Filter1.m_nValues[2][1] = 0;
	Filter1.m_nValues[2][2] = 1;
	
	
	C3By3Filter Filter2;
	Filter2.m_nValues[0][0] = -1;
	Filter2.m_nValues[0][1] = -1;
	Filter2.m_nValues[0][2] = -1;
	Filter2.m_nValues[1][0] = 0;
	Filter2.m_nValues[1][1] = 0;
	Filter2.m_nValues[1][2] = 0;
	Filter2.m_nValues[2][0] = 1;
	Filter2.m_nValues[2][1] = 1;
	Filter2.m_nValues[2][2] = 1;
	
	
	CDibImage dibCopy;
	CopySelection(dibCopy);	  
	
	ASSERT(dibCopy.m_pWorkingArea);
	CRect rectSrc(dibCopy.m_pWorkingArea->BoundingRectangle());
	CRect rectDest(m_pWorkingArea->BoundingRectangle());
	
	BOOL bSuccess = TRUE;
	LPSTR lpDibHdrSrc   = (LPSTR) ::GlobalLock(dibCopy.m_hDib);
	LPSTR lpDibBitsSrc  = ::FindDIBBits(lpDibHdrSrc);
	LPSTR lpDibHdrDest  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBitsDest = ::FindDIBBits(lpDibHdrDest);
	for (int y=0; bSuccess && y<rectSrc.Height(); y++)
	{
		for (int x=0; bSuccess && x<rectSrc.Width(); x++)
		{
			if (dibCopy.m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c1 = Filter1.Filter(dibCopy, lpDibBitsSrc, x, y);
				COLORREF c2 = Filter2.Filter(dibCopy, lpDibBitsSrc, x, y);
				int r = max(GetRValue(c1), GetRValue(c2));
				int g = max(GetGValue(c1), GetGValue(c2));
				int b = max(GetBValue(c1), GetBValue(c2));
				bSuccess = SetPixel(x+rectDest.left, y+rectDest.top, RGB(r, g, b), lpDibBitsDest);
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);	  
	return bSuccess;	  	
}

//Sobel垂直算子
BOOL CDibImage::FindVerticalEdgesFilter()
{
  C3By3Filter Filter;
  Filter.m_nValues[0][0] = -1;
  Filter.m_nValues[0][1] = -2;
  Filter.m_nValues[0][2] = -1;
  Filter.m_nValues[1][0] = 0;
  Filter.m_nValues[1][1] = 0;
  Filter.m_nValues[1][2] = 0;
  Filter.m_nValues[2][0] = 1;
  Filter.m_nValues[2][1] = 2;
  Filter.m_nValues[2][2] = 1;

  return UserDefinedFilter(Filter);
}

//Sobel水平算子
BOOL CDibImage::FindHorizontalEdgesFilter()
{
  C3By3Filter Filter;
  Filter.m_nValues[0][0] = -1;
  Filter.m_nValues[0][1] = 0;
  Filter.m_nValues[0][2] = 1;
  Filter.m_nValues[1][0] = -2;
  Filter.m_nValues[1][1] = 0;
  Filter.m_nValues[1][2] = 2;
  Filter.m_nValues[2][0] = -1;
  Filter.m_nValues[2][1] = 0;
  Filter.m_nValues[2][2] = 1;

  return UserDefinedFilter(Filter);
}

BOOL CDibImage::Smooth()
{
	C3By3Filter Filter;

	for (int i=0; i<3; i++)
	for (int j=0; j<3; j++)
		Filter.m_nValues[i][j] =1;
	/*  
	Filter.m_nValues[3][3] = -4;
	Filter.m_nValues[2][2] = 1;
	Filter.m_nValues[2][3] = 4;
	Filter.m_nValues[2][4] = 1;
	Filter.m_nValues[3][2] = 4;
	Filter.m_nValues[3][4] = 4;
	Filter.m_nValues[4][2] = 1;
	Filter.m_nValues[4][3] = 4;
	Filter.m_nValues[4][4] = 1;
	*/
	Filter.m_nDivision = 9;

	return UserDefinedFilter(Filter);
}

BOOL CDibImage::Sharp()
{
	C3By3Filter Filter;

	for (int i=0; i<3; i++)
	for (int j=0; j<3; j++)
		Filter.m_nValues[i][j] =0;
	Filter.m_nValues[0][0] = 0;
	Filter.m_nValues[0][1] = -1;
	Filter.m_nValues[0][2] = 0;
	Filter.m_nValues[1][0] = -1;
	Filter.m_nValues[1][1] = 5;
	Filter.m_nValues[1][2] = -1;
	Filter.m_nValues[2][0] = 0;
	Filter.m_nValues[2][1] = -1;
	Filter.m_nValues[2][2] = 0;
	Filter.m_nDivision = 1;

	return UserDefinedFilter(Filter);
}

BOOL CDibImage::AddNoiseFilter(int Percentage)
{
	if (Percentage < 0 || Percentage > 100)
	return FALSE;

	if (m_hDib == NULL)
	return FALSE;

	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());

	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			int test = rand();  
			BOOL bPass = test < (RAND_MAX*Percentage/100);
			if (bPass && m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c;
				int r = rand()*256/RAND_MAX - 128;
				int g = rand()*256/RAND_MAX - 128;
				int b = rand()*256/RAND_MAX - 128;

				if (GetPixel(x, y, c, lpDibBits)&&BitsPerPixel()==24)
				{
					bSuccess = SetPixel(x, y, RGB(GetRValue(c) + r,
						 GetGValue(c) + g, GetBValue(c) + b), lpDibBits);
				}
				else if(GetPixel(x, y, c, lpDibBits)&&BitsPerPixel()==8)
				{
					bSuccess = SetPixel(x, y,RGB(GetRValue(c) + r, 
						 GetRValue(c) + r, GetRValue(c) + r), lpDibBits);
				}
				else
					bSuccess = FALSE;
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	return bSuccess;
}


BOOL CDibImage::MedianFilter()
{
	C3By3MedianFilter Filter;
	return UserDefinedFilter(Filter);
}


BOOL CDibImage::UserDefinedFilter(CUserDefinedFilter& Filter)
{
	if (m_hDib == NULL)
	return FALSE;

	CDibImage dibCopy;
	CopySelection(dibCopy);

	ASSERT(dibCopy.m_pWorkingArea);
	CRect rectSrc(dibCopy.m_pWorkingArea->BoundingRectangle());
	CRect rectDest(m_pWorkingArea->BoundingRectangle());

	BOOL bSuccess = TRUE;
	LPSTR lpDibHdrSrc   = (LPSTR) ::GlobalLock(dibCopy.m_hDib);
	LPSTR lpDibBitsSrc  = ::FindDIBBits(lpDibHdrSrc);
	LPSTR lpDibHdrDest  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBitsDest = ::FindDIBBits(lpDibHdrDest);
	for (int y=0; bSuccess && y<rectSrc.Height(); y++)
	{
		for (int x=0; bSuccess && x<rectSrc.Width(); x++)
		{
			if (dibCopy.m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c = Filter.Filter(dibCopy, lpDibBitsSrc, x, y);
				bSuccess = SetPixel(x+rectDest.left, y+rectDest.top, c, lpDibBitsDest);
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	GlobalUnlock((HGLOBAL) dibCopy.m_hDib);
	return bSuccess;
}


int CDibImage::ColorsUsed() const
{
	return 0;  //for the moment
}


BOOL CDibImage::SplitChannels(CDibImage& RedChannel, CDibImage& GreenChannel, CDibImage& BlueChannel)
{
	if (m_hDib == NULL)
		return FALSE;

	if (!RedChannel.Create(Size(), 24))
		return FALSE;

	if (!GreenChannel.Create(Size(), 24))
		return FALSE;

	if (!BlueChannel.Create(Size(), 24))
		return FALSE;

	BOOL bSuccess = TRUE;

	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	LPSTR lpDibHdrRed  = (LPSTR) ::GlobalLock(RedChannel.m_hDib);
	LPSTR lpDibBitsRed = ::FindDIBBits(lpDibHdrRed);
	LPSTR lpDibHdrGreen  = (LPSTR) ::GlobalLock(GreenChannel.m_hDib);
	LPSTR lpDibBitsGreen = ::FindDIBBits(lpDibHdrGreen);
	LPSTR lpDibHdrBlue  = (LPSTR) ::GlobalLock(BlueChannel.m_hDib);
	LPSTR lpDibBitsBlue = ::FindDIBBits(lpDibHdrBlue);

	CRect r(Rect());
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			COLORREF c;
			if (GetPixel(x, y, c, lpDibBits))
			{
				int r = GetRValue(c);
				int g = GetGValue(c);
				int b = GetBValue(c);
				bSuccess = bSuccess && RedChannel.SetPixel(x, y, RGB(r, r, r), lpDibBitsRed);
				bSuccess = bSuccess && GreenChannel.SetPixel(x, y, RGB(g, g, g), lpDibBitsGreen);
				bSuccess = bSuccess && BlueChannel.SetPixel(x, y, RGB(b, b, b), lpDibBitsBlue);
			}
			else
				bSuccess = FALSE;
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	GlobalUnlock((HGLOBAL) RedChannel.m_hDib);
	GlobalUnlock((HGLOBAL) GreenChannel.m_hDib);
	GlobalUnlock((HGLOBAL) BlueChannel.m_hDib);

	return bSuccess;
}



BOOL CDibImage::GetRedChannel(CDibImage& red)
{
	if (m_hDib == NULL)
		return FALSE;

	if (!red.Create(Size(), 24))
		return FALSE;

	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	LPSTR lpDibHdrRed  = (LPSTR) ::GlobalLock(red.m_hDib);
	LPSTR lpDibBitsRed = ::FindDIBBits(lpDibHdrRed);

	CRect r(Rect());
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			COLORREF c;
			if (GetPixel(x, y, c, lpDibBits))
			{
				int r = GetRValue(c);
				bSuccess = red.SetPixel(x, y, RGB(r, r, r), lpDibBitsRed);
			}
			else
				bSuccess = FALSE;
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	GlobalUnlock((HGLOBAL) red.m_hDib);

	return bSuccess;
}


BOOL CDibImage::GetGreenChannel(CDibImage& green)
{
	if (m_hDib == NULL)
		return FALSE;

	if (!green.Create(Size(), 24))
		return FALSE;

	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	LPSTR lpDibHdrGreen  = (LPSTR) ::GlobalLock(green.m_hDib);
	LPSTR lpDibBitsGreen = ::FindDIBBits(lpDibHdrGreen);

	CRect r(Rect());
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			COLORREF c;
			if (GetPixel(x, y, c, lpDibBits))
			{
				int g = GetGValue(c);
				bSuccess = green.SetPixel(x, y, RGB(g, g, g), lpDibBitsGreen);
			}
			else
				bSuccess = FALSE;
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	GlobalUnlock((HGLOBAL) green.m_hDib);

	return bSuccess;
}


BOOL CDibImage::GetBlueChannel(CDibImage& blue)
{
	if (m_hDib == NULL)
		return FALSE;

	if (!blue.Create(Size(), 24))
		return FALSE;

	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	LPSTR lpDibHdrBlue  = (LPSTR) ::GlobalLock(blue.m_hDib);
	LPSTR lpDibBitsBlue = ::FindDIBBits(lpDibHdrBlue);

	CRect r(Rect());
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			COLORREF c;
			if (GetPixel(x, y, c, lpDibBits))
			{
				int b = GetBValue(c);
				bSuccess = blue.SetPixel(x, y, RGB(b, b, b), lpDibBitsBlue);
			}
			else
			bSuccess = FALSE;
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	GlobalUnlock((HGLOBAL) blue.m_hDib);

	return bSuccess;
}


BOOL CDibImage::CombineChannels(const CDibImage& red, const CDibImage& green, const CDibImage& blue)
{
	if (red.m_hDib == NULL || green.m_hDib == NULL || blue.m_hDib == NULL)
		return FALSE;

	//All the images should be the same size
	if (red.Rect() != green.Rect() || green.Rect() != blue.Rect() || red.Rect() != blue.Rect())
		return FALSE;

	if (!Create(red.Size(), 24))
		return FALSE;


	BOOL bSuccess = TRUE;

	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	LPSTR lpDibHdrRed  = (LPSTR) ::GlobalLock(red.m_hDib);
	LPSTR lpDibBitsRed = ::FindDIBBits(lpDibHdrRed);
	LPSTR lpDibHdrGreen  = (LPSTR) ::GlobalLock(green.m_hDib);
	LPSTR lpDibBitsGreen = ::FindDIBBits(lpDibHdrGreen);
	LPSTR lpDibHdrBlue  = (LPSTR) ::GlobalLock(blue.m_hDib);
	LPSTR lpDibBitsBlue = ::FindDIBBits(lpDibHdrBlue);

	CRect r(Rect());
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			COLORREF r;
			COLORREF g;
			COLORREF b;
			if (red.GetPixel(x, y, r, lpDibBitsRed) &&
				green.GetPixel(x, y, g, lpDibBitsGreen) &&
				blue.GetPixel(x, y, b, lpDibBitsBlue))
			{
				COLORREF c = RGB(GetRValue(r), GetGValue(g), GetBValue(b));
				bSuccess = bSuccess && SetPixel(x, y, c, lpDibBits);
			}
			else
				bSuccess = FALSE;
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	GlobalUnlock((HGLOBAL) red.m_hDib);
	GlobalUnlock((HGLOBAL) green.m_hDib);
	GlobalUnlock((HGLOBAL) blue.m_hDib);

	return bSuccess;
}


BOOL CDibImage::GetRedHistogram(long* RedChannel, int nSize)
{
	if (m_hDib == NULL)
		return FALSE;
	
	for (int i=0; i<nSize; i++)
		RedChannel[i] = 0;
	
	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());
	
    BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);

	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c;
				if (GetPixel(x, y, c, lpDibBits))
				{
					int r = GetRValue(c);
					if (r < nSize)
						RedChannel[r]++;
				}
				else
					bSuccess = FALSE;
			}			
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	return bSuccess;
}


BOOL CDibImage::GetGreenHistogram(int* GreenChannel, int nSize)
{
	if (m_hDib == NULL)
		return FALSE;

	for (int i=0; i<nSize; i++)
		GreenChannel[i] = 0;

	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());

	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c;
				if (GetPixel(x, y, c, lpDibBits))
				{
					int g = GetGValue(c);
					if (g < nSize)
					GreenChannel[g]++;
				}
				else
					bSuccess = FALSE;
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);

	return bSuccess;
}


BOOL CDibImage::GetBlueHistogram(int* BlueChannel, int nSize)
{
	if (m_hDib == NULL)
		return FALSE;

	for (int i=0; i<nSize; i++)
		BlueChannel[i] = 0;

	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());

	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c;
				if (GetPixel(x, y, c, lpDibBits))
				{
					int b = GetBValue(c);
					if (b < nSize)
					BlueChannel[b]++;
				}
				else
					bSuccess = FALSE;
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);

	return bSuccess;
}


BOOL CDibImage::GetHistogram(int* RedChannel, int nRedSize, int* GreenChannel, int nGreenSize, int* BlueChannel, int nBlueSize)
{
	if (m_hDib == NULL)
		return FALSE;

	for (int i=0; i<nRedSize; i++)
		RedChannel[i] = 0;
	for (i=0; i<nGreenSize; i++)
		GreenChannel[i] = 0;
	for (i=0; i<nBlueSize; i++)
		BlueChannel[i] = 0;

	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());

	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				COLORREF c;
				if (GetPixel(x, y, c, lpDibBits))
				{
					int r = GetRValue(c);
					int g = GetGValue(c);
					int b = GetBValue(c);
					if (r < nRedSize)
					RedChannel[r]++;
					if (g < nGreenSize)
					GreenChannel[g]++;
					if (b < nBlueSize)
					BlueChannel[b]++;
				}
				else
					bSuccess = FALSE;
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);

	return bSuccess;
}
CDC* CDibImage::BeginPaint(CDC *pDC)
{
	m_pMemDC = new CDC;
	m_pMemDC->CreateCompatibleDC(pDC);
	m_pPalette=new CPalette;
	m_pPalette->Attach(m_Pal);
	m_pPaletteTmp = m_pMemDC->SelectPalette(m_pPalette, TRUE);
	m_pMemDC->RealizePalette();
	m_pBitmap=new CBitmap;
	m_pBitmap->Attach(m_hBitmap);
	m_pBitmapTmp = (CBitmap *)m_pMemDC->SelectObject(m_pBitmap);
	return m_pMemDC;
}

void CDibImage::EndPaint()
{
	m_pMemDC->SelectObject(m_pBitmapTmp);
	m_pMemDC->SelectPalette(m_pPaletteTmp, TRUE);
	delete m_pMemDC;

	m_pPalette->Detach();
	m_pBitmap->Detach();
	delete m_pPalette;
	delete m_pBitmap;
	m_pPalette=NULL;
	m_pBitmap=NULL;
	Create(m_hBitmap);
}
BOOL CDibImage::Create(HBITMAP hBitmap)		// DIB Section
{
	if (! hBitmap)
        return FALSE; 
	
	HDIB hDib =::DIBSectionToDIB(hBitmap); 
    if (! hDib) 
        return FALSE; 
	
	if (m_hDib)
		::GlobalFree(m_hDib);	
	::DeleteObject(m_Pal); m_Pal=NULL;
	
	::DeleteObject(m_hBitmap); m_hBitmap=NULL;
	
	m_hDib = hDib;
	return UpdateInternal();
}

BOOL CDibImage::UpdateInternal()
{
	CPalette pal;
	::CreateDIBPalette(m_hDib, &pal);	
	m_Pal=(HPALETTE)pal.Detach();
	
	m_hBitmap =::DIBToDIBSection(m_hDib);
	if (m_hBitmap == NULL)
		return FALSE;
	
	return TRUE;
}
void CDibImage::UpdateBitmap()
{
	::DeleteObject(m_hBitmap);
	m_hBitmap=::DIBToDIBSection(m_hDib);
}
//该函数用于对图像进行计数，要求目标图像为只有0和255两个灰度值的灰度图像，
//统计后的结果放在数组m_GraiArea中
//用的是四领域种子点填充算法
BOOL CDibImage::CountNumber()
{
	POINT* seeds;
	int StackPoint;
    BYTE pixel;
	COLORREF m_Color;
	int iCurrentPixelX,iCurrentPixelY;
	m_GrainArea.SetSize(0,5);
	CRect rc(m_pWorkingArea->BoundingRectangle());
	if(rc.Width()*rc.Height()==0)
		return FALSE;
	else
		seeds=new POINT[rc.Width()*rc.Height()];
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
    ::GlobalUnlock((HGLOBAL) m_hDib);		  
	for(int y=rc.top;y<rc.bottom;y++)
	{
		for(int x=rc.left;x<rc.right;x++)
		{			
			GetPixel(x,y,m_Color,lpDibBits);
			pixel=GetRValue(m_Color);
			if(pixel==0)
			{
				CGrainArea grain(x,y);
				m_GrainArea.Add(grain);
				StackPoint=1;
				seeds[1].x=x;
				seeds[1].y=y;
				SetPixel(x,y,RGB(128,128,128),lpDibBits);//做上标记
				while(StackPoint>0)
				{
					//取出种子
					iCurrentPixelX=seeds[StackPoint].x;
					iCurrentPixelY=seeds[StackPoint].y;
					StackPoint--;
					//判断左边的点
					if(iCurrentPixelX>rc.left)
					{
						GetPixel(iCurrentPixelX-1,iCurrentPixelY,m_Color,lpDibBits);
						pixel=GetRValue(m_Color);
						if(pixel==0)
						{
							SetPixel(iCurrentPixelX-1,iCurrentPixelY,RGB(128,128,128),lpDibBits);
							StackPoint++;
							seeds[StackPoint].x=iCurrentPixelX-1;
							seeds[StackPoint].y=iCurrentPixelY;
							int temp=m_GrainArea.GetUpperBound();
							m_GrainArea[temp].m_number++;
							if(m_GrainArea[temp].m_rect.left>iCurrentPixelX-1)
								m_GrainArea[temp].m_rect.left=iCurrentPixelX-1;
						}
					}
					//判断上面的点
					if(iCurrentPixelY>rc.top)
					{
						GetPixel(iCurrentPixelX,iCurrentPixelY-1,m_Color,lpDibBits);
						pixel=GetRValue(m_Color);
						if(pixel==0)
						{
							SetPixel(iCurrentPixelX,iCurrentPixelY-1,RGB(128,128,128),lpDibBits);
							StackPoint++;
							seeds[StackPoint].x=iCurrentPixelX;
							seeds[StackPoint].y=iCurrentPixelY-1;
							int temp=m_GrainArea.GetUpperBound();
							m_GrainArea[temp].m_number++;
							if(m_GrainArea[temp].m_rect.top>iCurrentPixelY-1)
								m_GrainArea[temp].m_rect.top=iCurrentPixelY-1;
						}
					}
					//判断右面的点
					if(iCurrentPixelX<rc.right-1)
					{
						GetPixel(iCurrentPixelX+1,iCurrentPixelY,m_Color,lpDibBits);
						pixel=GetRValue(m_Color);
						if(pixel==0)
						{
							SetPixel(iCurrentPixelX+1,iCurrentPixelY,RGB(128,128,128),lpDibBits);
							StackPoint++;
							seeds[StackPoint].x=iCurrentPixelX+1;
							seeds[StackPoint].y=iCurrentPixelY;
							int temp=m_GrainArea.GetUpperBound();
							m_GrainArea[temp].m_number++;
							if(m_GrainArea[temp].m_rect.right<iCurrentPixelX+1)
								m_GrainArea[temp].m_rect.right=iCurrentPixelX+1;
							
						}
					}
					//判断下面的点
					if(iCurrentPixelY<rc.bottom-1)
					{
						GetPixel(iCurrentPixelX,iCurrentPixelY+1,m_Color,lpDibBits);
						pixel=GetRValue(m_Color);
						if(pixel==0)
						{
							SetPixel(iCurrentPixelX,iCurrentPixelY+1,RGB(128,128,128),lpDibBits);
							StackPoint++;
							seeds[StackPoint].x=iCurrentPixelX;
							seeds[StackPoint].y=iCurrentPixelY+1;
							int temp=m_GrainArea.GetUpperBound();
							m_GrainArea[temp].m_number++;
							if(m_GrainArea[temp].m_rect.bottom<iCurrentPixelY+1)
								m_GrainArea[temp].m_rect.bottom=iCurrentPixelY+1;							
						}
					}					
				}
			}
		}
	}
	TRACE("%d\n",m_GrainArea.GetSize());
	delete []seeds;
	//恢复
	for( y=rc.top;y<rc.bottom;y++)
	{
		for(int x=rc.left;x<rc.right;x++)
		{
			GetPixel(x,y,m_Color,lpDibBits);
			pixel=GetRValue(m_Color);
			if(pixel==128)
			{
				SetPixel(x,y,RGB(0,0,0),lpDibBits);
			}
		}
	}	
	return TRUE;
}

void  CDibImage::CaptureRect(HDC hdcScreen, LPRECT prc, BOOL bFullScreen)
{
	HDC hdc =::CreateCompatibleDC(hdcScreen);
	HBITMAP hbmp = CreateCompatibleBitmap(hdcScreen,prc->right - prc->left,
		prc->bottom - prc->top);
	SelectObject(hdc,hbmp);
	
	BitBlt(hdc,0,0,prc->right - prc->left,prc->bottom - prc->top,
		hdcScreen,prc->left,prc->top,SRCCOPY);
	HPALETTE hPal = GetSystemPalette();
	HDIB hDib = BitmapToDIB(hbmp,hPal);
	Attach(hDib);
	::DeleteDC(hdc);	
}

WORD CDibImage::GetColorNumber()
{
    LPSTR lpBI = (LPSTR)GlobalLock(m_hDib); 
    if (! lpBI) 
	{
		GlobalUnlock(m_hDib);
		return 0;
	}
	
	WORD wColors = DIBNumColors(lpBI);
	GlobalUnlock(m_hDib);
	
	return wColors;
}

HPALETTE CDibImage::GetSystemPalette()
{
	HDC hDC;                // handle to a DC 
    static HPALETTE hPal = NULL;   // handle to a palette 
    HANDLE hLogPal;         // handle to a logical palette 
    LPLOGPALETTE lpLogPal;  // pointer to a logical palette 
    int nColors;            // number of colors 
	
    // Find out how many palette entries we want. 
	
    hDC = GetDC(NULL); 
	
    if (!hDC) 
        return NULL; 
	
    nColors = PalEntriesOnDevice(hDC);   // Number of palette entries 
	
    // Allocate room for the palette and lock it. 
	
    hLogPal = GlobalAlloc(GHND, sizeof(LOGPALETTE) + nColors * 
		sizeof(PALETTEENTRY)); 
	
    // if we didn't get a logical palette, return NULL 
	
    if (!hLogPal) 
        return NULL; 
	
    // get a pointer to the logical palette 
	
    lpLogPal = (LPLOGPALETTE)GlobalLock(hLogPal); 
	
    // set some important fields 
	
    lpLogPal->palVersion = PALVERSION; 
    lpLogPal->palNumEntries = nColors; 
	
    // Copy the current system palette into our logical palette 
	
    GetSystemPaletteEntries(hDC, 0, nColors, 
		(LPPALETTEENTRY)(lpLogPal->palPalEntry)); 
	
    // Go ahead and create the palette.  Once it's created, 
    // we no longer need the LOGPALETTE, so free it.     
	
    hPal = CreatePalette(lpLogPal); 
	
    // clean up 
	
    GlobalUnlock(hLogPal); 
    GlobalFree(hLogPal); 
    ReleaseDC(NULL, hDC); 
	
    return hPal; 
}
int CDibImage::PalEntriesOnDevice(HDC hDC)
{
	int nColors;  // number of colors 
	
    // Find out the number of colors on this device. 
	
    nColors = (1 << (GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES))); 
	
    ASSERT(nColors); 
    return nColors; 
}
BOOL CDibImage::Clear(int nEffect, CDC* pDC, int x, int y, int nDeta, DWORD dwDelayTime)
{
	if (nEffect == EFFECT_FADE)
		return DisplayFadeOut(pDC, x, y, nDeta, dwDelayTime);
	else if (nEffect == EFFECT_MOSAIC)
		return DisplayMosaicOut(pDC, x, y, nDeta, dwDelayTime);

	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	CBitmap bitmap;
	bitmap.Attach(m_hBitmap);
	CBitmap* pOldBmp = MemDC.SelectObject(&bitmap);
	
    CPalette pal;
	pal.Attach(m_Pal);
	CPalette* pOldPal = pDC->SelectPalette(m_pPalette, TRUE);
    pDC->RealizePalette();

	pDC->BitBlt(x,y,Width(),Height(),&MemDC,0,0,SRCCOPY);

	CBrush brush(GetSysColor(COLOR_WINDOW));
	CBrush* oldbrush = pDC->SelectObject(&brush);
	CPen pen(PS_SOLID,1,GetSysColor(COLOR_WINDOW));
    CPen* oldpen = pDC->SelectObject(&pen);

	LONG i = 0;
	BOOL bDone = FALSE;
	while (! bDone)
	{
		switch (nEffect)
		{
		case EFFECT_SCANDOWN:
			if (i > Height())
			{
				i = Height();
				bDone = TRUE;
			}
			pDC->Rectangle(x, 
						   y+i,
						   x+Width(), 
						   y+nDeta);
			i += nDeta;
			break;
		case EFFECT_SCANUP:
			if (i > Height())
			{
				i = Height();
				bDone = TRUE;
			}
			pDC->Rectangle(x, 
						   y+Height()-i,
						   x+Width(), 
						   y+Height()-i+nDeta);
			i += nDeta;
			break;
		case EFFECT_SCANRIGHT:
			if (i > Width())
			{
				i = Width();
				bDone = TRUE;
			}
			pDC->Rectangle(x+i,
						   y,
						   x+nDeta, 
						   y+Height());
			i += nDeta;
			break;
		case EFFECT_SCANLEFT:
			if (i > Width())
			{
				i = Width();
				bDone = TRUE;
			}
			pDC->Rectangle(x+Width()-i, 
						   y,
						   x+Width()-i+nDeta, 
						   y+Height());
			i += nDeta;
			break;
		case EFFECT_VSPLITSCAN:
			if (i > Height()/2)
			{
				i = Height()/2;
				bDone = TRUE;
			}
			pDC->Rectangle(x, 
						   y+i,
						   x+Width(), 
						   y+i+nDeta);
			pDC->Rectangle(x, 
						   y+Height()-i,
						   x+Width(), 
						   y+Height());
			i += nDeta;
			break;
		case EFFECT_HSPLITSCAN:
			if (i > Width()/2)
			{
				i = Width()/2;
				bDone = TRUE;
			}
			pDC->Rectangle(x+i, 
						   y,
						   x+i+nDeta, 
						   y+Height());
			pDC->Rectangle(x+Width()-i,
						   y,
						   x+Width(), 
						   y+Height());
			i += nDeta;
			break;
		case EFFECT_MOVEDOWN:
		{
			i += nDeta;
			if (i > Height())
			{
				i = Height();
				bDone = TRUE;
			}
			pDC->BitBlt(x,  			//x dest
						y+i,		//y dest
      					Width(),		//width
						Height()-i,	//depth
						&MemDC,
						0,				//x source
						0,			//y source
						SRCCOPY);

			pDC->Rectangle(x, y+i-nDeta,
						   x+Width(), y+i);
			break;
		}
		case EFFECT_MOVEUP:
		{
			i += nDeta;
			if (i > Height())
			{
				i = Height();
				bDone = TRUE;
			}
			pDC->BitBlt(x,  			//x dest
						y,		//y dest
	      				Width(),		//width
						Height()-i,	//depth
						&MemDC,
						0,				//x source
						i,			//y source
						SRCCOPY);

			pDC->Rectangle( x,
							y+Height()-i,
							x+Width(),
							y+Height());
			break;
		}
		case EFFECT_MOVERIGHT:
		{
			i += nDeta;
			if (i > Width())
			{
				i = Width();
				bDone = TRUE;
			}
			pDC->BitBlt(x+i,  			//x dest
						y,		//y dest
      					Width()-i,		//width
						Height(),	//depth
						&MemDC,
						0,				//x source
						0,			//y source
						SRCCOPY);

			pDC->Rectangle(x+i-nDeta, y,
						   x+i, y+Height());
			break;
		}
		case EFFECT_MOVELEFT:
		{
			i += nDeta;
			if (i > Width())
			{
				i = Width();
				bDone = TRUE;
			}
			pDC->BitBlt(x,  			//x dest
						y,		//y dest
	      				Width()-i,		//width
						Height(),	//depth
						&MemDC,
						i,			//y source
						0,				//x source
						SRCCOPY);

			pDC->Rectangle( x+Width()-i,
							y,
							x+Width(),
							y+Height());
			break;
		}
		case EFFECT_VCROSSMOVE:
		{
			i += nDeta;
			if (i > Height())
			{
				i = Height();
				bDone = TRUE;
			}
			pDC->BitBlt(x,  			//x dest
						y+i,		//y dest
      					Width()/2,		//width
						Height()-i,	//depth
						&MemDC,
						0,				//x source
						0,			//y source
						SRCCOPY);

			pDC->Rectangle(x, y+i-nDeta,
						   x+Width()/2, y+i);

			pDC->BitBlt(x+Width()/2,  			//x dest
						y,		//y dest
	      				Width(),		//width
						Height()-i,	//depth
						&MemDC,
						Width()/2,				//x source
						i,			//y source
						SRCCOPY);

			pDC->Rectangle( x+Width()/2,
							y+Height()-i,
							x+Width(),
							y+Height());
			break;
		}
		case EFFECT_HCROSSMOVE:
		{
			i += nDeta;
			if (i > Width())
			{
				i = Width();
				bDone = TRUE;
			}
			pDC->BitBlt(x+i,  			//x dest
						y,		//y dest
      					Width()-i,		//width
						Height()/2,	//depth
						&MemDC,
						0,				//x source
						0,			//y source
						SRCCOPY);

			pDC->Rectangle(x+i-nDeta, y,
						   x+i, y+Height()/2);
	
			pDC->BitBlt(x,  			//x dest
						y+Height()/2,	//depth
	      				Width()-i,		//width
						Height(),	//depth
						&MemDC,
						i,			//y source
						Height()/2,	//depth
						SRCCOPY);

			pDC->Rectangle( x+Width()-i,
							y+Height()/2,
							x+Width(),
							y+Height());
			break;
		}
		case EFFECT_VSPLITMOVE:
		{
			i += nDeta;
			if (i > Height()/2)
			{
				i = Height()/2;
				bDone = TRUE;
			}
			pDC->BitBlt(x,  			//x dest
						y,		//y dest
			  			Width(),		//width
						Height()/2-i,	//depth
						&MemDC,
						0,				//x source
						i,			//y source
						SRCCOPY);

			pDC->BitBlt(x,  			//x dest
						y+Height()/2+i,		//y dest
      					Width(),		//width
						Height()/2-i,	//depth
						&MemDC,
						0,				//x source
						Height()/2,			//y source
						SRCCOPY);

			pDC->Rectangle( x,
							y+Height()/2-i,
							x+Width(),
							y+Height()/2+i+1);
			break;
		}
		case EFFECT_HSPLITMOVE:
		{
			i += nDeta;
			if (i > Width()/2)
			{
				i = Width()/2;
				bDone = TRUE;
			}
			pDC->BitBlt(x,  			//x dest
						y,		//y dest
	      				Width()/2-i,		//width
						Height(),	//depth
						&MemDC,
						i,			//y source
						0,				//x source
						SRCCOPY);

			pDC->BitBlt(x+Width()/2+i,  			//x dest
						y,		//y dest
      					Width()/2-i,		//width
						Height(),	//depth
						&MemDC,
						Width()/2,				//x source
						0,			//y source
						SRCCOPY);

			pDC->Rectangle( x+Width()/2-i,
							y,
							x+Width()/2+i+1,
							y+Height());
			break;
		}
		case EFFECT_VRASTER:
		{
			if (i > Height())
			{
				i = Height();
				bDone = TRUE;
			}
			LONG j = 0;
			BOOL bQuitLoop = FALSE;
			while (! bQuitLoop)
			{
				if (j > Width())
				{
					j = Width();
					bQuitLoop = TRUE;
				}
				pDC->BitBlt(x+j,  			//x dest
							y+i,		//y dest
      						nDeta,		//width
							Height()-i,	//depth
							&MemDC,
							j,				//x source
							0,			//y source
							SRCCOPY);

				pDC->Rectangle(x+j, y+i-1,
							   x+j+nDeta, y+i);
				j += nDeta;
				if (j > Width())
				{
					j = Width();
					bQuitLoop = TRUE;
				}
	
				pDC->BitBlt(x+j,  			//x dest
							y,		//y dest
				  			nDeta,		//width
							Height()-i,	//depth
							&MemDC,
							j,			//y source
							i,				//x source
							SRCCOPY);

				pDC->Rectangle( x+j,
								y+Height()-i,
								x+j+nDeta,
								y+Height());
				j += nDeta;
			}
			i++;
			break;
		}
		case EFFECT_HRASTER:
		{
			if (i > Width())
			{
				i = Width();
				bDone = TRUE;
			}
			LONG j = 0;
			BOOL bQuitLoop = FALSE;
			while (! bQuitLoop)
			{
				if (j > Height())
				{
					j = Height();
					bQuitLoop = TRUE;
				}
				pDC->BitBlt(x+i,  			//x dest
							y+j,		//y dest
      						Width()-i,		//width
							nDeta,	//depth
							&MemDC,
							0,				//x source
							j,			//y source
							SRCCOPY);
				pDC->Rectangle(x+i-1, y+j,
							   x+i, y+j+nDeta);
				j += nDeta;
				if (j > Height())
				{
					j = Height();
					bQuitLoop = TRUE;
				}
				pDC->BitBlt(x,  			//x dest
							y+j,		//y dest
				  			Width()-i,		//width
							nDeta,	//depth
							&MemDC,
							i,			//y source
							j,				//x source
							SRCCOPY);
				pDC->Rectangle( x+Width()-i,
								y+j,
								x+Width(),
								y+j+nDeta);
				j += nDeta;
			}
			i++;
			break;
		}
		case EFFECT_HBLIND:
		{
			if (i > nDeta)
			{
				i = nDeta;
				bDone = TRUE;
			}
			for (LONG j=i; j<Height(); j+=nDeta)
				pDC->Rectangle( x,
								y+j,
								x+Width(),
								y+j+1);
			i++;
			break;
		}
		case EFFECT_VBLIND:
			if (i > nDeta)
			{
				i = nDeta;
				bDone = TRUE;
			}
			for (LONG j=i; j<Width(); j+=nDeta)
				pDC->Rectangle( x+j,
								y,
								x+j+1,
								y+Height());
			i++;
			break;
		}	// end of switch
		Delay(dwDelayTime);
	}	// end of while

	MemDC.SelectObject(pOldBmp);
	pDC->SelectPalette(pOldPal, TRUE);
	bitmap.Detach();
	pal.Detach();
	return TRUE;
}
BOOL CDibImage::Display(CDC * pDC, int x, int y, DWORD dwRop)
{
	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	CBitmap bitmap;
	bitmap.Attach(m_hBitmap);
	CBitmap* pOldBmp = MemDC.SelectObject(&bitmap);
	
	CPalette pal;
	pal.Attach(m_Pal);
	CPalette* pOldPal = pDC->SelectPalette(&pal, TRUE);
    pDC->RealizePalette();
	
	BOOL bSuccess = pDC->BitBlt(x, y, 
								Width(), Height(),
								&MemDC, 
								0, 0, 
								dwRop);
	
	MemDC.SelectObject(pOldBmp);
	pDC->SelectPalette(pOldPal, TRUE);
	bitmap.Detach();
	pal.Detach();	
	return bSuccess;
}
BOOL CDibImage::Display(int nEffect, CDC* pDC, int x, int y, int nDeta, DWORD dwDelayTime, DWORD dwRop)
{
	if (nEffect == EFFECT_FADE)
		return DisplayFadeIn(pDC, x, y, nDeta, dwDelayTime);
	else if (nEffect == EFFECT_MOSAIC)
		return DisplayMosaicIn(pDC, x, y, nDeta, dwDelayTime, dwRop);

	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	CBitmap Bitmap;
	Bitmap.Attach(m_hBitmap);
	CBitmap* pOldBmp = MemDC.SelectObject(&Bitmap);
    CPalette Pal;
	Pal.Attach(m_Pal);
	CPalette* pOldPal = pDC->SelectPalette(m_pPalette, TRUE);
    pDC->RealizePalette();

	LONG i = 0;
	BOOL bDone = FALSE;
	while (! bDone)
	{
		switch (nEffect)
		{
		case EFFECT_SCANDOWN:
			if (i > Height())
			{
				i = Height();
				bDone = TRUE;
			}
            pDC->BitBlt(x, 
						y+i,
                        Width(),
						nDeta,
						&MemDC,
                        0,
						i,
						dwRop);
			i += nDeta;
			break;
		case EFFECT_SCANUP:
			if (i > Height())
			{
				i = Height();
				bDone = TRUE;
			}
            pDC->BitBlt(x, 
						y+Height()-i,
                        Width(),
						nDeta,
						&MemDC,
                        0,
						Height()-i,
						dwRop);
			i += nDeta;
			break;
		case EFFECT_SCANRIGHT:
			if (i > Width())
			{
				i = Width();
				bDone = TRUE;
			}
            pDC->BitBlt(x+i, 
						y,
						nDeta,
                        Height(),
						&MemDC,
						i,
                        0,
						dwRop);
			i += nDeta;
			break;
		case EFFECT_SCANLEFT:
			if (i > Width())
			{
				i = Width();
				bDone = TRUE;
			}
            pDC->BitBlt(x+Width()-i, 
						y,
						nDeta,
                        Height(),
						&MemDC,
						Width()-i,
                        0,
						dwRop);
			i += nDeta;
			break;
		case EFFECT_VSPLITSCAN:
			if (i > Height()/2)
			{
				i = Height()/2;
				bDone = TRUE;
			}
			pDC->BitBlt(x,
						y+i,
						Width(),
						nDeta,
						&MemDC,
						0,
						i,
						dwRop);
			pDC->BitBlt(x,
						y+Height()-i,
						Width(),
						i,
						&MemDC,
						0,
						Height()-i,
						dwRop);
			i += nDeta;
			break;
		case EFFECT_HSPLITSCAN:
			if (i > Width()/2)
			{
				i = Width()/2;
				bDone = TRUE;
			}
			pDC->BitBlt(x+i,
						y,
						nDeta,
						Height(),
						&MemDC,
						i,
						0,
						dwRop);
			pDC->BitBlt(x+Width()-i,
						y,
						nDeta,
						Height(),
						&MemDC,
						Width()-i,
						0,
						dwRop);
			i += nDeta;
			break;
		case EFFECT_MOVEDOWN:
			if (i > Height())
			{
				i = Height();
				bDone = TRUE;
			}
			pDC->BitBlt(x,
						y,
						Width(),
						i+nDeta,
						&MemDC,
						0,
						Height()-i,
						dwRop);
			i += nDeta;
			break;
		case EFFECT_MOVEUP:
			if (i > Height())
			{
				i = Height();
				bDone = TRUE;
			}
			pDC->BitBlt(x,
						y+Height()-i,
						Width(),
						i,
						&MemDC,
						0,
						0,
						dwRop);
			i += nDeta;
			break;
		case EFFECT_MOVERIGHT:
			if (i > Width())
			{
				i = Width();
				bDone = TRUE;
			}
			pDC->BitBlt(x,
						y,
						i+nDeta,
						Height(),
						&MemDC,
						Width()-i,
						0,
						dwRop);
			i += nDeta;
			break;
		case EFFECT_MOVELEFT:
			if (i > Width())
			{
				i = Width();
				bDone = TRUE;
			}
			pDC->BitBlt(x+Width()-i,
						y,
						i,
						Height(),
						&MemDC,
						0,
						0,
						dwRop);
			i += nDeta;
			break;
		case EFFECT_VSPLITMOVE:
			if (i > Height()/2)
			{
				i = Height()/2;
				bDone = TRUE;
			}
			pDC->BitBlt(x,
						y,
						Width(),
						i+nDeta,
						&MemDC,
						0,
						Height()/2-i,
						dwRop);
			pDC->BitBlt(x,
						y+Height()-i,
						Width(),
						i,
						&MemDC,
						0,
						Height()/2,
						dwRop);
			i += nDeta;
			break;
		case EFFECT_HSPLITMOVE:
			if (i > Width()/2)
			{
				i = Width()/2;
				bDone = TRUE;
			}
			pDC->BitBlt(x,
						y,
						i+nDeta,
						Height(),
						&MemDC,
						Width()/2-i,
						0,
						dwRop);
			pDC->BitBlt(x+Width()-i,
						y,
						i,
						Height(),
						&MemDC,
						Width()/2,
						0,
						dwRop);
			i += nDeta;
			break;
		case EFFECT_VCROSSMOVE:
			if (i > Height())
			{
				i = Height();
				bDone = TRUE;
			}
			pDC->BitBlt(x,
						y,
						Width()/2,
						i+nDeta,
						&MemDC,
						0,
						Height()-i,
						dwRop);
			pDC->BitBlt(x+Width()/2,
						y+Height()-i,
						Width(),
						i,
						&MemDC,
						Width()/2,
						0,
						dwRop);
			i += nDeta;
			break;
		case EFFECT_HCROSSMOVE:
			if (i > Width())
			{
				i = Width();
				bDone = TRUE;
			}
			pDC->BitBlt(x,
						y,
						i+nDeta,
						Height()/2,
						&MemDC,
						Width()-i,
						0,
						dwRop);
			pDC->BitBlt(x+Width()-i,
						y+Height()/2,
						i,
						Height()/2,
						&MemDC,
						0,
						Height()/2,
						dwRop);
			i += nDeta;
			break;
		case EFFECT_VRASTER:
		{
			if (i > Height())
			{
				i = Height();
				bDone = TRUE;
			}
			LONG j = 0;
			BOOL bQuitLoop = FALSE;
			while (! bQuitLoop)
			{
				if (j > Width())
				{
					j = Width();
					bQuitLoop = TRUE;
				}
				pDC->BitBlt(x+j,
							y,
							nDeta,
							i+1,
							&MemDC,
							j,
							Height()-i,
							dwRop);
				j += nDeta;
				if (j > Width())
				{
					j = Width();
					bQuitLoop = TRUE;
				}
				pDC->BitBlt(x+j,
							y+Height()-i,
							nDeta,
							i,
							&MemDC,
							j,
							0,
							dwRop);
				j += nDeta;
			}
			i++;
			break;
		}
		case EFFECT_HRASTER:
		{
			if (i > Width())
			{
				i = Width();
				bDone = TRUE;
			}
			LONG j = 0;
			BOOL bQuitLoop = FALSE;
			while (! bQuitLoop)
			{
				if (j > Height())
				{
					j = Height();
					bQuitLoop = TRUE;
				}
				pDC->BitBlt(x,
							y+j,
							i+1,
							nDeta,
							&MemDC,
							Width()-i,
							j,
							dwRop);
				j += nDeta;
				if (j > Height())
				{
					j = Height();
					bQuitLoop = TRUE;
				}
				pDC->BitBlt(x+Width()-i,
							y+j,
							i,
							nDeta,
							&MemDC,
							0,
							j,
							dwRop);
				j += nDeta;
			}
			i++;
			break;
		}
		case EFFECT_HBLIND:
		{
			if (i > nDeta)
			{
				i = nDeta;
				bDone = TRUE;
			}
			for (LONG j=i; j<Height(); j+=nDeta)
				pDC->BitBlt(x,
							y+j,
							Width(),
							1,
							&MemDC,
							0,
							j,
							dwRop);
			i++;
			break;
		}
		case EFFECT_VBLIND:
			if (i > nDeta)
			{
				i = nDeta;
				bDone = TRUE;
			}
			for (LONG j=i; j<Width(); j+=nDeta)
				pDC->BitBlt(x+j,
							y,
							1,
							Height(),
							&MemDC,
							j,
							0,
							dwRop);
			i++;
			break;
		}  // end of switch
		Delay(dwDelayTime);
	}	// end of while

	MemDC.SelectObject(pOldBmp);
	pDC->SelectPalette(pOldPal, TRUE);
	Bitmap.Detach();
	Pal.Detach();
	return TRUE;
}

//淡入
#define UPVALUE(x, n)	((x)/((n) ? (n) : 1) + ((x)%((n) ? (n) : 1) ? 1 : 0))
BOOL CDibImage::DisplayFadeIn(CDC* pDC, int x, int y, int nDeta, DWORD dwDelayTime)
{
	// backup DIB
	HDIB hDib =(HDIB)CopyHandle(m_hDib);

	// get color number
	WORD wNumColors = GetColorNumber();

	BITMAPINFO *bmi = (BITMAPINFO *)GlobalLock(m_hDib);
	BITMAPINFO *bmi0 = (BITMAPINFO *)GlobalLock(hDib);
	if (! bmi)
		return FALSE;

	BOOL bDone = FALSE;
	int nDetaR, nDetaG, nDetaB;
	// fade out
	if (wNumColors)		// There is palette
	{
		// build black image
		for (WORD i=0; i<wNumColors; ++i)
		{
			bmi->bmiColors[i].rgbRed = 0 ;
			bmi->bmiColors[i].rgbGreen = 0 ;
			bmi->bmiColors[i].rgbBlue = 0 ;
		}
		UpdateInternal();
		// display black first
		Display(pDC, x, y, SRCCOPY);

		while (! bDone)
		{
			bDone = TRUE;
			for (WORD i=0; i<wNumColors; i++)
			{
				nDetaR = UPVALUE(bmi0->bmiColors[i].rgbRed-bmi->bmiColors[i].rgbRed, nDeta);
				nDetaG = UPVALUE(bmi0->bmiColors[i].rgbGreen-bmi->bmiColors[i].rgbGreen, nDeta);
				nDetaB = UPVALUE(bmi0->bmiColors[i].rgbBlue-bmi->bmiColors[i].rgbBlue, nDeta);
				if (! Fade(nDetaR, nDetaG, nDetaB,
						 bmi0->bmiColors[i].rgbRed,
						 bmi0->bmiColors[i].rgbGreen,
						 bmi0->bmiColors[i].rgbBlue,
						 &(bmi->bmiColors[i].rgbRed),
						 &(bmi->bmiColors[i].rgbGreen),
						 &(bmi->bmiColors[i].rgbBlue)))
					 bDone = FALSE;
			}
			UpdateInternal();
			Display(pDC, x, y, SRCCOPY);
			Delay(dwDelayTime);
			nDeta--;
		}
	}
	else		// No palette
	{
		// should increase the speed!
		dwDelayTime /= 10;
		// bits position
		LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)bmi;
		LPBYTE lpBits = (LPBYTE)lpbi + lpbi->biSize;
		LPBYTE lpBits0 = (LPBYTE)bmi0 + lpbi->biSize;
		int nDelta = WIDTHBYTES(lpbi->biBitCount*lpbi->biWidth) - lpbi->biWidth*lpbi->biBitCount/8;
		int nx, ny;

		// build black image
		for (ny=0; ny<lpbi->biHeight; ny++)
		{
			for (nx=0; nx<lpbi->biWidth; nx++)
			{
				*lpBits++ = 0;
				*lpBits++ = 0;
				*lpBits++ = 0;
			}
			lpBits += nDelta; 
		}
		lpBits = (LPBYTE)lpbi + lpbi->biSize;
		UpdateInternal();
		// display black first
		Display(pDC, x, y, SRCCOPY);

		BYTE r, g, b, r0, g0, b0;
		while (! bDone)
		{
			bDone = TRUE;
			for (ny=0; ny<lpbi->biHeight; ny++)
			{
				for (nx=0; nx<lpbi->biWidth; nx++)
				{
					b0 = (BYTE)*lpBits0++;
					g0 = (BYTE)*lpBits0++;
					r0 = (BYTE)*lpBits0++;
					b = (BYTE)*(lpBits);
					g = (BYTE)*(lpBits+1);
					r = (BYTE)*(lpBits+2);
					nDetaR = UPVALUE(r0-r, nDeta);
					nDetaG = UPVALUE(g0-g, nDeta);
					nDetaB = UPVALUE(b0-b, nDeta);
					if (! Fade(nDetaR, nDetaG, nDetaB, r0, g0, b0, &r, &g, &b))
						bDone = FALSE;
					*lpBits++ = b;
					*lpBits++ = g;
					*lpBits++ = r;
				}
				lpBits += nDelta; 
				lpBits0 += nDelta; 
			}
			lpBits = (LPBYTE)lpbi + lpbi->biSize;
			lpBits0 = (LPBYTE)bmi0 + lpbi->biSize;
			UpdateInternal();
			Display(pDC, x, y, SRCCOPY);
			Delay(dwDelayTime);
			nDeta--;
		}
	}

	// cleanup 
	GlobalUnlock(m_hDib);
	GlobalUnlock(hDib);

	// restore DIB
	m_hDib = (HDIB)CopyHandle(hDib);
	GlobalFree(hDib);

	return TRUE;
}

//淡出
BOOL CDibImage::DisplayFadeOut(CDC* pDC, int x, int y, int nDeta, DWORD dwDelayTime)
{
	// display first
	Display(pDC, x, y, SRCCOPY);
	
	// backup DIB
	HDIB hDib =(HDIB)CopyHandle(m_hDib);

	// get color number
	WORD wNumColors = GetColorNumber();

	BITMAPINFO *bmi = (BITMAPINFO *)GlobalLock(m_hDib);
	if (! bmi)
		return FALSE;

	BOOL bDone = FALSE;
	int nDetaR, nDetaG, nDetaB;
	// fade out
	if (wNumColors)		// There is palette
	{
		while (! bDone)
		{
			bDone = TRUE;
			for (WORD i=0; i<wNumColors; i++)
			{
				nDetaR = -1*UPVALUE(bmi->bmiColors[i].rgbRed, nDeta);
				nDetaG = -1*UPVALUE(bmi->bmiColors[i].rgbGreen, nDeta);
				nDetaB = -1*UPVALUE(bmi->bmiColors[i].rgbBlue, nDeta);
				if (! Fade(nDetaR, nDetaG, nDetaB,
						 0, 0, 0,
						 &(bmi->bmiColors[i].rgbRed),
						 &(bmi->bmiColors[i].rgbGreen),
						 &(bmi->bmiColors[i].rgbBlue)))
					 bDone = FALSE;
			}
			UpdateInternal();
			Display(pDC, x, y, SRCCOPY);
			Delay(dwDelayTime);
			nDeta--;
		}
	}
	else		// No palette
	{
		// should increase the speed!
		dwDelayTime /= 10;
		// bits position
		LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)bmi;
		LPBYTE lpBits = (LPBYTE)lpbi + lpbi->biSize;
		int nDelta = WIDTHBYTES(lpbi->biBitCount*lpbi->biWidth) - lpbi->biWidth*lpbi->biBitCount/8;

		BYTE r, g, b;
		int nx, ny;
		while (! bDone)
		{
			bDone = TRUE;
			for (ny=0; ny<lpbi->biHeight; ny++)
			{
				for (nx=0; nx<lpbi->biWidth; nx++)
				{
					b = (BYTE)*(lpBits);
					g = (BYTE)*(lpBits+1);
					r = (BYTE)*(lpBits+2);
					nDetaR = -1*UPVALUE(r, nDeta);
					nDetaG = -1*UPVALUE(g, nDeta);
					nDetaB = -1*UPVALUE(b, nDeta);
					if (! Fade(nDetaR, nDetaG, nDetaB, 0, 0, 0, &r, &g, &b))
						bDone = FALSE;
					*lpBits++ = b;
					*lpBits++ = g;
					*lpBits++ = r;
				}
				lpBits += nDelta; 
			}
			lpBits = (LPBYTE)lpbi + lpbi->biSize;
			UpdateInternal();
			Display(pDC, x, y, SRCCOPY);
			Delay(dwDelayTime);
			nDeta--;
		}
	}

	// cleanup 
	GlobalUnlock(m_hDib);

	// restore DIB
	m_hDib =(HDIB) CopyHandle(hDib);
	GlobalFree(hDib);

	return TRUE;
}
//马赛克
BOOL CDibImage::DisplayMosaicIn(CDC *pDC, int xx, int yy, int nTileSize, DWORD dwDelayTime, DWORD dwRop)
{
    // count the number of dwTileNum
	DWORD dwTileNum=(Width()/nTileSize + ((Width() % nTileSize) ? 1:0))*
					(Height()/nTileSize + ((Height() % nTileSize) ? 1:0));
	// allocate tile array
	POINT *point = new POINT[dwTileNum];
	if (point == NULL)
		return FALSE;

	// initialize the tile array
	LONG x=0, y=0;
	for (DWORD i=0;i<dwTileNum;++i) 
	{
		point[i].x=x;
		point[i].y=y;

		x+=nTileSize;
		if (x >= Width()) 
		{
			x=0;
            y+=nTileSize;
		}
	}

	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	CBitmap  bitmap;
	bitmap.Attach(m_hBitmap);
	CBitmap* pOldBmp = MemDC.SelectObject(&bitmap);

	CPalette pal;
	pal.Attach(m_Pal);
	CPalette* pOldPal = pDC->SelectPalette(&pal, TRUE);
    pDC->RealizePalette();
	
	double fMax = RAND_MAX;
	BOOL  bDone = FALSE;
	while (! bDone)
	{
		DWORD n = (DWORD)((double)dwTileNum*(double)((double)rand()/fMax));
		x = point[n].x;
		y = point[n].y;

		pDC->BitBlt(xx+x,
					yy+y,
					nTileSize,
					nTileSize,
					&MemDC,
					x,
					y,
					dwRop);

		dwTileNum--;
		bDone = !dwTileNum;

		point[n].x = point[dwTileNum].x;
		point[n].y = point[dwTileNum].y;
		Delay(dwDelayTime);
	}

	MemDC.SelectObject(pOldBmp);
	pDC->SelectPalette(pOldPal, TRUE);

	bitmap.Detach();
	pal.Detach();
	delete[] point;

	return TRUE;
}
//马赛克
BOOL CDibImage::DisplayMosaicOut(CDC *pDC, int xx, int yy, int nTileSize, DWORD dwDelayTime)
{
	// display dib first
	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	CBitmap bitmap;
	bitmap.Attach(m_hBitmap);
	CBitmap* pOldBmp = MemDC.SelectObject(&bitmap);

	CPalette pal;
	pal.Attach(m_Pal);
	CPalette* pOldPal = pDC->SelectPalette(&pal, TRUE);
    pDC->RealizePalette();

	pDC->BitBlt(xx,	yy,
				Width(),	Height(),
				&MemDC,
				0, 0,
				SRCCOPY);

    // count the number of dwTileNum
	DWORD dwTileNum=(Width()/nTileSize + ((Width() % nTileSize) ? 1:0))*
					(Height()/nTileSize + ((Height() % nTileSize) ? 1:0));
	// allocate tile array
	POINT *point = new POINT[dwTileNum];
	if (point == NULL)
		return FALSE;

	// initialize the tile array
	LONG x=0, y=0;
	for (DWORD i=0;i<dwTileNum;++i) 
	{
		point[i].x=x;
		point[i].y=y;

		x+=nTileSize;
		if (x >=Width()) 
		{
			x=0;
            y+=nTileSize;
		}
	}

	CBrush brush(GetSysColor(COLOR_WINDOW));
	CBrush* oldbrush = pDC->SelectObject(&brush);
	CPen pen(PS_SOLID,1,GetSysColor(COLOR_WINDOW));
    CPen* oldpen = pDC->SelectObject(&pen);
	
	double fMax = RAND_MAX;
	BOOL bDone = FALSE;
	while (! bDone)
	{
		DWORD n = (DWORD)((double)dwTileNum*(double)((double)rand()/fMax));
		x = point[n].x;
		y = point[n].y;

		pDC->Rectangle(	xx+x,
						yy+y,
						xx+x+nTileSize+1,
						yy+y+nTileSize+1);

		dwTileNum--;
		bDone = !dwTileNum;

		point[n].x = point[dwTileNum].x;
		point[n].y = point[dwTileNum].y;
		Delay(dwDelayTime);
	}

	MemDC.SelectObject(pOldBmp);
	pDC->SelectPalette(pOldPal, TRUE);
	pDC->SelectObject(oldbrush);
    pDC->SelectObject(oldpen);
	bitmap.Detach();
	pal.Detach();

	delete[] point;

	return TRUE;
}
void CDibImage::MakeThumbnail(HBITMAP& hBitmap,int cx,int cy)
{
	HPALETTE    hPal = NULL;
	
	int			nWidth, nHeight;
	// borrow our dib header to create our thumbnail bitmap
	nWidth = Width();
	nHeight = Height();
	LPBITMAPINFO lpDIBHdr=(LPBITMAPINFO)::GlobalLock((HGLOBAL)m_hDib);
	LPSTR lpDIBBits=FindDIBBits((LPSTR)lpDIBHdr);
	lpDIBHdr->bmiHeader.biWidth=cx;
	lpDIBHdr->bmiHeader.biHeight=cy;
	// create thumbnail bitmap section
	hBitmap = ::CreateDIBSection(NULL, 
		lpDIBHdr, 
		DIB_RGB_COLORS,
		NULL, 
		NULL, 
		0); 
	// restore dib header
	lpDIBHdr->bmiHeader.biWidth = nWidth;
	lpDIBHdr->bmiHeader.biHeight = nHeight;
	// select thumbnail bitmap into screen dc
	HDC	 hDCDest=::CreateCompatibleDC(NULL);
	HDC  hDCSour=::CreateCompatibleDC(NULL);
	
	HGDIOBJ hOldObj1 = ::SelectObject(hDCDest, hBitmap);
	HGDIOBJ hOldObj2=::SelectObject(hDCSour,m_hBitmap);
	// grayscale image, need palette
	if(m_Pal!= NULL)
	{
		hPal =::SelectPalette(hDCDest, m_Pal, FALSE);
		::RealizePalette(hDCDest);
	}
	// set stretch mode
	::SetStretchBltMode(hDCDest, COLORONCOLOR);
	CRect rc(GetWorkingArea()->BoundingRectangle());
	// populate the thumbnail bitmap bits
	::StretchBlt(hDCDest,0,0,cx,cy,hDCSour,rc.left,rc.top,rc.Width(),rc.Height(),SRCCOPY);
	::GlobalUnlock((HGLOBAL)m_hDib);
	// restore DC object
	::SelectObject(hDCDest, hOldObj1);
	::SelectObject(hDCSour,hOldObj2);
	// restore DC palette
	if(m_Pal != NULL)
		::SelectPalette(hDCDest, (HPALETTE)hPal, FALSE);
	// clean up
	::DeleteObject(hDCDest);
	::DeleteObject(hDCSour);
}
BOOL CDibImage::DeleteWorkingArea()
{
	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());
	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
				bSuccess=SetPixel(x,y,RGB(255,255,255));
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	return bSuccess;
}
/*
 *	函数名称：TraceContour()
 *  返回值：BOOL  运算成功返回TRUE，否则返回FALSE
 *  说明： 该函数用与对图像进行轮廓提取运算
 *  要求目标图像为只有0和255两个灰度值的灰度图像
 *  作者：赵中明，修改自《Visual c++图像处理》P437
 *  日期：2011年8月13日
 */
BOOL CDibImage::TraceContour(BOOL bAdd)     //bAdd表示是否是增加孔洞
{
	BOOL bFindStartPoint=FALSE;				//是否找到起始点和回到起始点
	BOOL bFindPoint;					    //是否找到孔洞的边界点
	CPoint StartPoint,CurrentPoint;		    //起始边界点和当前边界点
	//八个扫描方向和起始扫描方向
	int Direction[8][2]={{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0}};
	int BeginDirec;
	
	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	CRect r(m_pWorkingArea->BoundingRectangle());	
	//先找到最左下方的边界点,workingarea不包括bottom和right，但包括top和left
	int i;							//循环变量
	CContour* pContour=NULL;
	for(int y=r.bottom-1;y>=r.top&&bSuccess;y--)
	{
		for(int x=r.left;x<r.right&&bSuccess;x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{				
				COLORREF c;
				if(!bFindStartPoint)
				{
					bSuccess=GetPixel(x,y,c,lpDibBits);
					if(c==0)
					{
						bFindStartPoint=TRUE;
						StartPoint.x=x;
						StartPoint.y=y;
						
						pContour=new CContour(bAdd);
						pContour->m_pointArray.Add(CPoint(x,y)); 
						pContour->m_rect.left=pContour->m_rect.right=x;
						pContour->m_rect.top=pContour->m_rect.bottom=y;
						m_ContourList.AddTail(pContour);
						
						BOOL bSingle=TRUE;				
						for(i=0;i<8&&bSingle;i++)		//判断是否为单象素孔洞
						{
							CurrentPoint.x=StartPoint.x+Direction[i][0];
							CurrentPoint.y=StartPoint.y+Direction[i][1];
							if(m_pWorkingArea->PointInSelection(CurrentPoint))
							{
								GetPixel(CurrentPoint.x,CurrentPoint.y,c,lpDibBits);
								if(c==0)
									bSingle=FALSE;
							}
						}
						if(!bSingle)
						{
							//由于起始点是在左下方，故扫描方向沿左上方向
							BeginDirec=0;
							bFindStartPoint=FALSE;//跟踪边界
							CurrentPoint.x=StartPoint.x;//从初始点开始扫描
							CurrentPoint.y=StartPoint.y;
							while(!bFindStartPoint)
							{
								bFindPoint=FALSE;
								while(!bFindPoint)
								{
									//沿扫描方向查看一个象素
									CPoint  tempPoint;
									tempPoint.x=CurrentPoint.x+Direction[BeginDirec][0];
									tempPoint.y=CurrentPoint.y+Direction[BeginDirec][1];
									if(m_pWorkingArea->PointInSelection(tempPoint))
									{
										GetPixel(tempPoint.x,tempPoint.y,c,lpDibBits);
										if(c==0)
										{
											bFindPoint=TRUE;
											CurrentPoint.x=CurrentPoint.x+Direction[BeginDirec][0];
											CurrentPoint.y=CurrentPoint.y+Direction[BeginDirec][1];
											if(CurrentPoint.y==StartPoint.y&&CurrentPoint.x==StartPoint.x)
											{
												bFindStartPoint=TRUE;
											}
											pContour->m_pointArray.Add(CurrentPoint);
											if(CurrentPoint.x<pContour->m_rect.left)
												pContour->m_rect.left=CurrentPoint.x;
											if(CurrentPoint.x>pContour->m_rect.right)
												pContour->m_rect.right=CurrentPoint.x;
											if(CurrentPoint.y<pContour->m_rect.top)
												pContour->m_rect.top=CurrentPoint.y;
											if(CurrentPoint.y>pContour->m_rect.bottom)
												pContour->m_rect.bottom=CurrentPoint.y;
											//扫描方向逆时针旋转两格
											
											BeginDirec--;
											if(BeginDirec==-1)
												BeginDirec=7;
											BeginDirec--;
											if(BeginDirec==-1)
												BeginDirec=7;
											
										}
										else
										{
											//扫描方向顺时针旋转一格
											BeginDirec++;
											if(BeginDirec==8)
												BeginDirec=0;										
										}
									}
									else
									{
										//扫描方向顺时针旋转一格
										BeginDirec++;
										if(BeginDirec==8)
											BeginDirec=0;
									}								
								}					
							}
						}
						pContour->EraserGrain(*this);								
					}
				}			
				bFindStartPoint=FALSE;
			}
		}
	}
	return TRUE;
}

/*
 *	函数名称：MagicWnd(int radius)
 *  参数：int radius  灰度的差值
 *        CPoint pt   当前鼠标点的坐标
 *  返回值 成功返回TRUE，否则返回FALSE
 *  说明   求出同pt点的灰度差值在radius内的区域
 *  要求   目标图像为灰度图像
 */
BOOL CDibImage::MagicWand(CPoint pt,int radius,BOOL bAdd)
{
	CRect rect(0,0,Width(),Height());
	if(!rect.PtInRect(pt))
		return FALSE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	COLORREF currentPixel;
    GetPixel(pt.x,pt.y,currentPixel,lpDibBits);
	int value=GetRValue(currentPixel);
	for(int y=0;y<Height();y++)
	{
		for(int x=0;x<Width();x++)
		{
			GetPixel(x,y,currentPixel,lpDibBits);
			int tempValue=GetRValue(currentPixel);
			if(abs(value-tempValue)<=radius)
			{
				SetPixel(x,y,RGB(0,0,0),lpDibBits);
			}
			else
				SetPixel(x,y,RGB(255,255,255),lpDibBits);
		}
	}
	::GlobalUnlock((HGLOBAL) m_hDib);	
	CWorkingArea *pOldWorkingArea=m_pWorkingArea;
	m_pWorkingArea=new CRectWorkingArea(Rect());
	if(!TraceContour(bAdd))
		return FALSE;
	TRACE("m_CountList %d,%d,%d\n",m_ContourList.GetCount(),Width(),Height());
	CContour* pContour=NULL;
	POSITION pos=m_ContourList.GetHeadPosition();
	while(pos!=NULL)
	{
		POSITION temp=pos;
		pContour=(CContour*)m_ContourList.GetNext(pos);
        if(!pContour->InRegion(pt))
		{
			m_ContourList.RemoveAt(temp);
			delete pContour;
		}
	}
	delete m_pWorkingArea;
	m_pWorkingArea=pOldWorkingArea;
	return TRUE;
}

BOOL CDibImage::ColorMedianFilter(LPSTR lpNewDibBits,LPSTR lpDibBits)
{
	CRect r(m_pWorkingArea->BoundingRectangle());
	int x,y,Red[9],Green[9],Blue[9],RMedian,GMedian,BMedian,iFilterLen=9;
	COLORREF c;
	for(y=r.top;y<r.bottom;y++)
	{
		for(x=r.left;x<r.right;x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{
				for(int k=0;k<3;k++)
				{
					for(int l=0;l<3;l++)
					{
						GetPixel(x+1-l,y+1-k,c,lpNewDibBits);
						Red[k*3+l]=GetRValue(c);
						Green[k*3+l]=GetGValue(c);
						Blue[k*3+l]=GetBValue(c);
					}                  
				}
				RMedian=Median(Red,iFilterLen);
				GMedian=Median(Green,iFilterLen);
				BMedian=Median(Blue,iFilterLen);
				c=RGB(RMedian,GMedian,BMedian);
				SetPixel(x,y,c,lpDibBits);
			}
		}
	}
    return TRUE;
}

int CDibImage::Median(int* Median,int iFilterLen)
{
	int bTemp;
	
	for(int i=0;i<iFilterLen-1;i++)
	{
		for(int j=0;j<iFilterLen-1-i;j++)
		{
			if(Median[j]>Median[j+1])
			{
				bTemp=Median[j];
				Median[j]=Median[j+1];
				Median[j+1]=bTemp;
			}
		}
	}
	if((iFilterLen&1)>0)
	{
		bTemp=Median[(iFilterLen+1)/2];
	}
	else
	{
		bTemp=(Median[iFilterLen/2]+Median[iFilterLen/2+1])/2;
	}
	return bTemp;
}

BOOL CDibImage::GetHistogram1(int* RedChannel, int nRedSize, int* GreenChannel, int nGreenSize, int* BlueChannel, int nBlueSize,int R1,int R2)
{
	if (m_hDib == NULL)
		return FALSE;
	
	for (int i=0; i<nRedSize; i++)
		RedChannel[i] = 0;
	for (i=0; i<nGreenSize; i++)
		GreenChannel[i] = 0;
	for (i=0; i<nBlueSize; i++)
		BlueChannel[i] = 0;
	
	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());
	
	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
	for (int y=r.top; bSuccess && y<r.bottom; y++)
	{
		for (int x=r.left; bSuccess && x<r.right; x++)
		{
			if (m_pWorkingArea->PointCircleInSelection(CPoint(x, y),R1,R2))
			{
				COLORREF c;
				if (GetPixel(x, y, c, lpDibBits))
				{
					int r = GetRValue(c);
					int g = GetGValue(c);
					int b = GetBValue(c);
					if (r < nRedSize)
						RedChannel[r]++;
					if (g < nGreenSize)
						GreenChannel[g]++;
					if (b < nBlueSize)
						BlueChannel[b]++;
				}
				else
					bSuccess = FALSE;
			}
		}
	}
	GlobalUnlock((HGLOBAL) m_hDib);
	
	return bSuccess;
}
BOOL CDibImage::GetHistogram2(int* RedChannel1,int* RedChannel, 
							  int* GreenChannel1,int* GreenChannel,int* BlueChannel1,
							  int* BlueChannel, int nBiasSize,int nSize)
{
	if (m_hDib == NULL)
		return FALSE;
	
	for (int i=0; i<nSize; i++)
	{
		RedChannel1[i] = 0;
				
		GreenChannel1[i] = 0;
	
		BlueChannel1[i] = 0;
	}
	
	int j;
	for (i=0;i<nBiasSize; i++)
	{
		j=0;
		do
		{
			RedChannel1[i]+= RedChannel[i+j];
			GreenChannel1[i]+= GreenChannel[i+j];
			BlueChannel1[i]+= BlueChannel[i+j];
		}while(j++<(nSize-nBiasSize));
	}
	
	return TRUE;
	
}
BOOL CDibImage::DisplayBack(int R_value,int G_value,int B_value,int nBiasSize,int nSize,int R1,int R2)
{
    LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
    BOOL bSuccess = TRUE;
	CRect r(m_pWorkingArea->BoundingRectangle());	
    
	for(int y=r.top;y<r.bottom&&bSuccess;y++)
	{
		for(int x=r.left;x<r.right&&bSuccess;x++)
		{
			if (m_pWorkingArea->PointCircleInSelection(CPoint(x, y),R1,R2))
			{	
				COLORREF color;
				bSuccess=GetPixel(x,y,color,lpDibBits);
				int r = GetRValue(color);
				int g = GetGValue(color);
				int b = GetBValue(color);
				if((r>=(R_value-(nSize-nBiasSize)/2)&&r<=(R_value+(nSize-nBiasSize)/2)&&
					g>=(G_value-(nSize-nBiasSize)/2)&&g<=(G_value+(nSize-nBiasSize)/2)&&
					b>=(B_value-(nSize-nBiasSize)/2)&&b<=(B_value+(nSize-nBiasSize)/2)))
				
				{
					color=RGB(255,255,255);
					SetPixel(x,y,color);
				}
				
			}
		}
	}
				
	return TRUE;
}
BOOL CDibImage::Eraseimpurity(BOOL bAdd)     //bAdd表示是否是增加孔洞
{
	while(!m_ContourList.IsEmpty())
	{
		delete m_ContourList.RemoveHead();
	}	
	BOOL bFindStartPoint=FALSE;				//是否找到起始点和回到起始点
	BOOL bFindPoint;					//是否找到孔洞的边界点
	CPoint StartPoint,CurrentPoint;		//起始边界点和当前边界点
	      
	//八个扫描方向和起始扫描方向
	int Direction[8][2]={{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0}};
	int BeginDirec;

	BOOL bSuccess = TRUE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);
    LONG lHeight=Height();
    LONG lWidth=Width();
    LONG lLineBytes=WIDTHBYTES(lWidth*24);
    LONG s=lLineBytes*lHeight;
    HLOCAL hNewDibBits=(HDIB)LocalAlloc(LHND,s);
    if(hNewDibBits==NULL)
    {
		return FALSE;
    }
    LPSTR lpNewDibBits=(char*)LocalLock(hNewDibBits);
	
    memcpy(lpNewDibBits,lpDibBits,s);
	
	CRect r(m_pWorkingArea->BoundingRectangle());	
	//先找到最左下方的边界点,workingarea不包括bottom和right，但包括top和left
	int i;	//循环变量
	int x;
	int y;
	COLORREF c;
	CContour* pContour=NULL;
	for(y=r.top;y<r.bottom&&bSuccess;y++)
	{
		for(x=r.left;x<r.right&&bSuccess;x++)
		{
			if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
			{				
			if(!bFindStartPoint)
			{
                bSuccess=GetPixel(x,y,c,lpDibBits);
			
				if(c!=RGB(255,255,255))
				{
					bFindStartPoint=TRUE;
					StartPoint.x=x;
					StartPoint.y=y;
					pContour=new CContour(bAdd);
					pContour->m_pointArray.Add(CPoint(x,y)); 
					pContour->m_rect.left=pContour->m_rect.right=x;
					pContour->m_rect.top=pContour->m_rect.bottom=y;

					m_ContourList.AddTail(pContour);

					BOOL bSingle=TRUE;				
					for(i=0;i<8&&bSingle;i++)		//判断是否为单象素孔洞
					{
						CurrentPoint.x=StartPoint.x+Direction[i][0];
						CurrentPoint.y=StartPoint.y+Direction[i][1];
						if(m_pWorkingArea->PointInSelection(CurrentPoint))
						{
							GetPixel(CurrentPoint.x,CurrentPoint.y,c,lpDibBits);
					
							if(c!=RGB(255,255,255))
								bSingle=FALSE;
						}
					}
					if(!bSingle)
					{
						//由于起始点是在左下方，故扫描方向沿左上方向
						BeginDirec=0;
						bFindStartPoint=FALSE;//跟踪边界
						CurrentPoint.x=StartPoint.x;//从初始点开始扫描
						CurrentPoint.y=StartPoint.y;
						while(!bFindStartPoint)
						{
							bFindPoint=FALSE;
							while(!bFindPoint)
							{
								//沿扫描方向查看一个象素
								CPoint  tempPoint;
								tempPoint.x=CurrentPoint.x+Direction[BeginDirec][0];
								tempPoint.y=CurrentPoint.y+Direction[BeginDirec][1];
								if(m_pWorkingArea->PointInSelection(tempPoint))
								{
									GetPixel(tempPoint.x,tempPoint.y,c,lpDibBits);
							
							        if(c!=RGB(255,255,255))
									{
										bFindPoint=TRUE;
										CurrentPoint.x=CurrentPoint.x+Direction[BeginDirec][0];
										CurrentPoint.y=CurrentPoint.y+Direction[BeginDirec][1];
										if(CurrentPoint.y==StartPoint.y&&CurrentPoint.x==StartPoint.x)
										{
											bFindStartPoint=TRUE;
										}
										pContour->m_pointArray.Add(CurrentPoint);
										if(CurrentPoint.x<pContour->m_rect.left)
											pContour->m_rect.left=CurrentPoint.x;
										if(CurrentPoint.x>pContour->m_rect.right)
											pContour->m_rect.right=CurrentPoint.x;
										if(CurrentPoint.y<pContour->m_rect.top)
											pContour->m_rect.top=CurrentPoint.y;
										if(CurrentPoint.y>pContour->m_rect.bottom)
											pContour->m_rect.bottom=CurrentPoint.y;
										//扫描方向逆时针旋转两格
										BeginDirec--;
										if(BeginDirec==-1)
											BeginDirec=7;
										BeginDirec--;
										if(BeginDirec==-1)
											BeginDirec=7;	
									}
									else
									{
										//扫描方向顺时针旋转一格
										BeginDirec++;
										if(BeginDirec==8)
											BeginDirec=0;										
									}
								}
								else
								{
									//扫描方向顺时针旋转一格
									BeginDirec++;
									if(BeginDirec==8)
										BeginDirec=0;
								}
							}
						}					
					}
					pContour->EraserColorGrain(*this);					
				}
			}
			bFindStartPoint=FALSE;
		}
		}
	}
    memcpy(lpDibBits,lpNewDibBits,s);

    LocalUnlock(hNewDibBits);
	LocalFree(hNewDibBits);

    i=0;
    POSITION pos=m_ContourList.GetHeadPosition();
	POSITION pos1=NULL;
	while (pos!=NULL)
	{
          pos1=pos;
		  CContour* pContour=(CContour*)m_ContourList.GetNext(pos);
		  i=pContour->ComputeArea();
          if(i<2)
		  {
		    pContour->EraserColorGrain(*this);
            m_ContourList.RemoveAt(pos1);
		  }

	}
	
	return TRUE;
}

BOOL CDibImage::EraseFaultGrain(int* Red,int* Green,int* Blue,int* R,int nSubSize,int* nBiasSize,int nSize)
{
	CRect r(m_pWorkingArea->BoundingRectangle());
	int x=(r.left+r.right)/2;
	int y=(r.top+r.bottom)/2;
	int r1,r2;
	BOOL bSuccess=TRUE;
	CContour* pContour=NULL;
    POSITION pos=m_ContourList.GetHeadPosition();
	POSITION pos1=NULL;
	while (pos!=NULL)
	{
		bSuccess=FALSE;
        pos1=pos; 
		CContour* pContour=(CContour*)m_ContourList.GetNext(pos);
        bSuccess=pContour->FaultGrainRadius(CPoint(x,y),r1,r2,*this);
		int maxsum,minsum;
		for(int i=0;i<nSubSize;i++)
		{
			if(r1>R[i]&&r1<=R[i+1])
				minsum=i;
			if(r2>R[i]&&r2<=R[i+1])
				maxsum=i+1;
		}
		int r=maxsum-minsum;
		int Red_sum=0;
		int Green_sum=0;
		int Blue_sum=0;
		int nCurrentBiasSize=0;
		for(i=minsum;i<maxsum;i++)
		{
			Red_sum+=Red[i];
			Green_sum+=Green[i];
			Blue_sum+=Blue[i];
			nCurrentBiasSize=max(nCurrentBiasSize,nBiasSize[i]);
		}
        int RedValue=Red_sum/r;
		int GreenValue=Green_sum/r;
		int BlueValue=Blue_sum/r;
		
		bSuccess=bSuccess&&pContour->GrainMaxValue(RedValue,GreenValue,BlueValue,*this,nCurrentBiasSize,nSize);
		if(bSuccess==TRUE)
		{
			pContour->EraserColorGrain(*this);
			m_ContourList.RemoveAt(pos1);
		}
	}   
	return TRUE;
}
//腐蚀
BOOL CDibImage::Erosion(int nMode,int structure[3][3])
{
	ASSERT(m_pWorkingArea);
	CRect r(m_pWorkingArea->BoundingRectangle());
	
	if (m_hDib == NULL)
		return FALSE;
	LPSTR lpDibHdr  = (LPSTR) ::GlobalLock(m_hDib);
	LPSTR lpDibBits = ::FindDIBBits(lpDibHdr);

	LPSTR lpSrc;//原图
	LPSTR lpDst;//缓存

	LPSTR lpNewDIBBits;
	HLOCAL hNewDIBBits;

	long i;
	long j;
	int n; 
	int m;

	unsigned char pixel;

	hNewDIBBits=LocalAlloc(LHND,m_nScanWidth*m_nHeight);

	if(hNewDIBBits==NULL)
	{
		return FALSE;
	}

	lpNewDIBBits=(char*)LocalLock(hNewDIBBits);

	lpDst=(char*)lpNewDIBBits;
	memset(lpDst,(BYTE)255,m_nScanWidth*m_nHeight);

	if(nMode==0)
	{
		for(j=0;j<m_nHeight;j++)
		{
			for(i=1;i<m_nScanWidth-1;i++)
			{
				int y=r.top+j;
				int x=r.left+i;
				if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
				{
					lpSrc=(char*)lpDibBits+m_nScanWidth*(m_nHeight-1-j)+i;
					lpDst=(char*)lpNewDIBBits+m_nScanWidth*(m_nHeight-1-j)+i;
					
					pixel=(unsigned char)*lpSrc;
					
					if(pixel!=255&&*lpSrc!=0)
						return FALSE;
					
					*lpDst=(unsigned char)0;
					
					for(n=0;n<3;n++)
					{
						pixel=*(lpSrc+n-1);
						if(pixel==255)
						{
							*lpDst=(unsigned char)255;
							break;
						}
					}
				}
				
			}
		}
	}
	else if(nMode==1)
	{
		for(j=1;j<m_nHeight-1;j++)
		{
			for(i=0;i<m_nScanWidth;i++)
			{
				int y=r.top+j;
				int x=r.left+i;
				if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
				{
					lpSrc=(char*)lpDibBits+m_nScanWidth*j+i;
					lpDst=(char*)lpNewDIBBits+m_nScanWidth*j+i;
					pixel=(unsigned char)*lpSrc;
					
					if(pixel!=255&&*lpSrc!=0)
						return FALSE;
					
					*lpDst=(unsigned char)0;
					
					for(n=0;n<3;n++)
					{
						pixel=*(lpSrc+(n-1)*m_nScanWidth);
						if(pixel==255)
						{
							*lpDst=(unsigned char)255;
							break;
						}
					}
				}
				
			}
		}
	}
	else
	{
		for(j=0;j<m_nHeight-1;j++)
		{
			for(i=0;i<m_nScanWidth;i++)
			{
				int y=r.top+j;
				int x=r.left+i;
				if (m_pWorkingArea->PointInSelection(CPoint(x, y)))
				{
					lpSrc=(char*)lpDibBits+m_nScanWidth*(m_nHeight-1-y)+x;
					lpDst=(char*)lpNewDIBBits+m_nScanWidth*(m_nHeight-1-y)+x;
					pixel=(unsigned char)*lpSrc;
					
					if(pixel!=255&&*lpSrc!=0)
						return FALSE;
					
					*lpDst=(unsigned char)0;
					for(m=0;m<3;m++)
					{
						for(n=0;n<3;n++)
						{
							if(structure[m][n]==-1)
								continue;
							pixel=*(lpSrc+((2-m)-1)*m_nScanWidth+(n-1));
							if(pixel==255)
							{
								*lpDst=(unsigned char)255;
								break;
							}
						}
					}
				}				
			}
		}
	}
	memcpy(lpDibBits,lpNewDIBBits,m_nScanWidth*m_nHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	return TRUE;
}
void CContour::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
	{	// storing code	
		ar<<m_Radius;				
		ar<<m_rect;		            //存放外接此孔洞的矩形
		ar<<m_bAdd;                 //m_bAdd表示是否是增加孔洞
	}
	else
	{	
		ar>>m_Radius;
		ar>>m_rect;
		ar>>m_bAdd;
		//if(m_Ver!=1.f)
		//{
		//	ar>>m_Ver;							//对象版本号
		//	ar>>m_Radius;						//孔洞半径
		//	ar>>m_Round;						//孔洞圆度
		//	ar>>m_Rx;							//孔洞长轴长度
		//	ar>>m_Ry;							//孔洞短轴长度
		//	ar>>m_Phi;							//孔洞长轴角度
		//	
		//	ar>>m_Cx;							//孔洞中心位置(m_Cx, m_Cy)
		//	ar>>m_Cy;
		//	ar>>m_Value;						//平均电导率值
		//}
	}
	m_pointArray.Serialize(ar);    //存放孔洞轮廓的坐标
}

#include "stdafx.h"
#include "MultiScale.h"
#include <math.h>
BOOL MUSICA_Decomposition(BYTE* pPixelsGrey,BYTE* pDecomposition,int * matrixInfo)
{
	int i;
	int nLevel = *matrixInfo;
	if (nLevel<2)
		return FALSE;

	memcpy(pDecomposition,pPixelsGrey,matrixInfo[2]*matrixInfo[3]);

	int windowWidth = 5;
	double filter[5*5] = {
		0.0025,	0.0125,	0.02,	0.0125,	0.0025,
			0.0125,	0.0625,	0.1,	0.0625,	0.0125,
			0.02,	0.1,	0.16,	0.1,	0.02,
			0.0125,	0.0625,	0.1,	0.0625,	0.0125,
			0.0025,	0.0125,	0.02,	0.0125,	0.0025
	};
	double filterMul4[5*5];
	for(i=0;i<5*5;i++)
		filterMul4[i] = filter[i]*4.0;

	BYTE* buffer = new BYTE[matrixInfo[2]*matrixInfo[3]];
	BYTE* X = pDecomposition;
	BYTE* X_1 = NULL;

	for (i=0;i<nLevel;i++)
	{
		int X_w = matrixInfo[i*3+1];
		int X_h = matrixInfo[i*3+2];
		int X_stride = matrixInfo[i*3+3];
		int X_1_w = matrixInfo[(i+1)*3+1];
		int X_1_h = matrixInfo[(i+1)*3+2];
		int X_1_stride = matrixInfo[(i+1)*3+3];
		int X_size = X_h*X_stride;
//		int X_1_size = X_1_h*X_1_stride;
		//X+1
		X_1 = X + X_size;
		//X -> buffer
		memcpy(buffer, X, X_size);
		//X filter
		ImageFilter8( filter,5,X, X_w , X_h, X_stride);
		//X Subsample by a factor of 2 -> X+1
		MUSICA_ImageSubsample(X, X_w, X_h, X_stride, X_1, X_1_w , X_1_h, X_1_stride);
		//X+1 Interpolate zero -> X
		MUSICA_ImageInterpolator(X_1, X_1_w, X_1_h, X_1_stride, X, X_w , X_h, X_stride);
		//X filter by 4 compensate
		ImageFilter8( filterMul4, 5, X, X_w , X_h, X_stride);
		//buffer-X -> X
		MUSICA_ImageSub8(buffer, X, X, X_w , X_h, X_stride);
		//(X+1) -> (X)
		X = X_1;
	}

	delete buffer;

	return TRUE;
}
void MUSICA_Mapping(BYTE* pDecomposition,int * matrixInfo)
{
	int i;
	int nLevel = *matrixInfo;
	if (nLevel<2)
	{
		return ;
	}

	int m = nLevel;
	double power = 0.65;
	BYTE map[256];
	for (i=0;i<256;i++)
	{
		double detail = i - 128;

		detail = m*(detail/m);
		if (detail>=0) {
			detail = m*pow(detail/m,power);
		}
		else{
			detail = -1*m*pow(-1*detail/m,power);
		}

		detail = detail + 128;
		detail = max(detail,0);
		detail = min(detail,255);

		map[i] = (BYTE)detail;
	}

	int totleLength = 0;
	for(i=0;i<nLevel;i++)
	{
		totleLength += matrixInfo[2+i*3]*matrixInfo[3+i*3];
	}

	BYTE* _pixels = pDecomposition;
	for (i=0;i<totleLength;i++)
	{
		*_pixels = map[*_pixels];
		_pixels++;
	}
}
BOOL MUSICA_Reconstruction(BYTE* pPixelsGrey,BYTE* pDecomposition,int * matrixInfo)
{
	int i;
	int nLevel = *matrixInfo;
	if (nLevel<2)
		return FALSE;

	int windowWidth = 5;
	double filter[5*5] = {
		0.0025,	0.0125,	0.02,	0.0125,	0.0025,
			0.0125,	0.0625,	0.1,	0.0625,	0.0125,
			0.02,	0.1,	0.16,	0.1,	0.02,
			0.0125,	0.0625,	0.1,	0.0625,	0.0125,
			0.0025,	0.0125,	0.02,	0.0125,	0.0025
	};
	double filterMul4[5*5];
	for (i=0;i<5*5;i++)
		filterMul4[i] = filter[i]*4.0;

	int totleLength = 0;
	for(i=0;i<nLevel;i++)
		totleLength += matrixInfo[2+i*3]*matrixInfo[3+i*3];

	BYTE* buffer = new BYTE[matrixInfo[2]*matrixInfo[3]];
	BYTE* X = NULL;
	BYTE* X_1 = pDecomposition + totleLength;

	for (i=nLevel-1;i>=0;i--)
	{
		int X_w = matrixInfo[i*3+1];
		int X_h = matrixInfo[i*3+2];
		int X_stride = matrixInfo[i*3+3];
		int X_1_w = matrixInfo[(i+1)*3+1];
		int X_1_h = matrixInfo[(i+1)*3+2];
		int X_1_stride = matrixInfo[(i+1)*3+3];
		int X_size = X_h*X_stride;
		int X_1_size = X_1_h*X_1_stride;

		//X
		X = X_1 - X_size;

		//X+1 Interpolate zero -> buffer
		MUSICA_ImageInterpolator(X_1, X_1_w, X_1_h, X_1_stride, buffer, X_w , X_h, X_stride);

		//buffer filter
		ImageFilter8(filterMul4,5,buffer, X_w , X_h, X_stride);

		//X + buffer -> X
		MUSICA_ImageAdd8(buffer, X, X, X_w , X_h, X_stride);

		//next
		X_1 = X;
	}

	delete buffer;

	memcpy(pPixelsGrey,pDecomposition,matrixInfo[2]*matrixInfo[3]);

	return TRUE;
}

BOOL MUSICA_ImageSubsample(BYTE* X,int X_w,int X_h,int X_stride,BYTE* X_1,int X_1_w,int X_1_h,int X_1_stride)
{
	if ( 2*(X_1_w-1)>X_w-1 || 2*(X_1_h-1)>X_h-1 )
	{
		return FALSE;
	}

	int pixelByte8 = 1 ;
	BYTE* _pixels_X = X;
	BYTE* _pixels_X_1 = X_1;
	int _strideoff_X = X_stride - pixelByte8 * 2 * X_1_w;
	int _strideoff_X_1 = X_1_stride - pixelByte8 * X_1_w;
	unsigned short _letf=0;
	unsigned short _top=0;
	unsigned short _right= X_1_w;
	unsigned short _bottom=X_1_h;

	int x,y;
	for(y=_top;y<_bottom;y++)
	{
		for(x=_letf;x<_right;x++)
		{
			*_pixels_X_1 = *_pixels_X;
			_pixels_X_1++;
			_pixels_X += 2;
		}
		_pixels_X_1 += _strideoff_X_1;
		_pixels_X += _strideoff_X;
		_pixels_X += X_stride;
	}

	return TRUE;
}

BOOL MUSICA_ImageInterpolator(BYTE* X_1,int X_1_w,int X_1_h,int X_1_stride,BYTE* X,int X_w,int X_h,int X_stride)
{
	if ( 2*(X_1_w-1)>X_w-1 || 2*(X_1_h-1)>X_h-1 )
	{
		return FALSE;
	}

	int pixelByte8 = 1 ;
	BYTE* _pixels_X = X;
	BYTE* _pixels_X_1 = X_1;
	int _strideoff_X = X_stride - pixelByte8 * 2 * X_1_w;
	int _strideoff_X_1 = X_1_stride - pixelByte8 * X_1_w;
	unsigned short _letf=0;
	unsigned short _top=0;
	unsigned short _right= X_1_w;
	unsigned short _bottom=X_1_h;

	memset(_pixels_X,0,X_h*X_stride);

	int x,y;
	for(y=_top;y<_bottom;y++)
	{
		for(x=_letf;x<_right;x++)
		{
			*_pixels_X = *_pixels_X_1;
			_pixels_X_1++;
			_pixels_X += 2;
		}
		_pixels_X_1 += _strideoff_X_1;
		_pixels_X += _strideoff_X;
		_pixels_X += X_stride;
	}

	return TRUE;
}

void ImageFilter8(double *fliter,int windowWidth,BYTE* pixels,int w,int h,int stride)
{
	int i,j;
	int ww = windowWidth*windowWidth;
	int hw = windowWidth/2;
	int pixelByte = 1 ; //bitCount / 8;
	BYTE* _pixels0 = pixels;
	BYTE* _pixels1 = new BYTE[stride*h];
	BYTE** _pixels = new BYTE*[windowWidth*windowWidth];
	int _strideoff = stride - pixelByte * w + 2*hw*pixelByte;
	unsigned short _letf=0;
	unsigned short _top=0;
	unsigned short _right= w;
	unsigned short _bottom=h;
	memcpy((void*)_pixels1,(void*)pixels,stride*h);
	for(i=0;i<windowWidth;i++)
	{
		for(j=0;j<windowWidth;j++)
		{
			_pixels[i*windowWidth+j] = _pixels1 + i*stride + j*pixelByte;
		}
	}
	_pixels0 = pixels+ hw *stride + hw*pixelByte ;

	int x,y;
	int hww = (ww+1)/2;
	for(y=_top+hw;y<_bottom-hw;y++)
	{
		for(x=_letf+hw;x<_right-hw;x++)
		{
//			BYTE* _pixels_b;//b
			BYTE* _pixels_g;//g
//			BYTE* _pixels_r;//r
//			double b=0;
			double g=0;
//			double r=0;
			for(i=0;i<ww;i++)
			{
//				_pixels_b = _pixels[i]++;//b
				_pixels_g = _pixels[i]++;//g
//				_pixels_r = _pixels[i]++;//r
//				b = b + *_pixels_b * fliter[i];
				g = g + *_pixels_g * fliter[i];
//				r = r + *_pixels_r * fliter[i];
			}

//			*_pixels0 = (BYTE)b;// - *(_pixels[hww]-3) + 128;
//			_pixels0++;
			*_pixels0 = (BYTE)g;// - *(_pixels[hww]-2) + 128;
			_pixels0++;
//			*_pixels0 = (BYTE)r;// - *(_pixels[hww]-1) + 128;
//			_pixels0++;
		}

		_pixels0 += _strideoff;
		for(i=0;i<ww;i++)
		{
			_pixels[i] += _strideoff;
		}
	}
	delete[] _pixels;
	delete _pixels1;
	return;
}

//Ê¹ÓÃ0~255¼ÇÂ¼ -128~127
void MUSICA_ImageSub8(BYTE* pMinuend,BYTE* pSubtrahend,BYTE* pResult,int w,int h,int stride)
{
	int pixelByte8 = 1 ;
	BYTE* _pixels_M = pMinuend;
	BYTE* _pixels_S = pSubtrahend;
	BYTE* _pixels_R = pResult;
	int _strideoff = stride - pixelByte8  * w;
	unsigned short _letf=0;
	unsigned short _top=0;
	unsigned short _right= w;
	unsigned short _bottom=h;

	int x,y;
	for(y=_top;y<_bottom;y++)
	{
		for(x=_letf;x<_right;x++)
		{
			//*_pixels_X = *_pixels_X_1;
			int nResult = *_pixels_M - *_pixels_S + 128;
			nResult = max(nResult,0);
			nResult = min(nResult,255);
			*_pixels_R = (BYTE)nResult;

			_pixels_M++;
			_pixels_S++;
			_pixels_R++;
		}
		_pixels_M += _strideoff;
		_pixels_S += _strideoff;
		_pixels_R += _strideoff;
	}

	return;
}

void MUSICA_ImageAdd8(BYTE* pOriginal,BYTE* pDetail,BYTE* pResult,int w,int h,int stride)
{
	int pixelByte8 = 1 ;
	BYTE* _pixels_O = pOriginal;
	BYTE* _pixels_D = pDetail;
	BYTE* _pixels_R = pResult;
	int _strideoff = stride - pixelByte8  * w;
	unsigned short _letf=0;
	unsigned short _top=0;
	unsigned short _right= w;
	unsigned short _bottom=h;

	int x,y;
	for(y=_top;y<_bottom;y++)
	{
		for(x=_letf;x<_right;x++)
		{
			//*_pixels_X = *_pixels_X_1;
			int nResult = *_pixels_O + *_pixels_D - 128;
			nResult = max(nResult,0);
			nResult = min(nResult,255);
			*_pixels_R = (BYTE)nResult;

			_pixels_O++;
			_pixels_D++;
			_pixels_R++;
		}
		_pixels_O += _strideoff;
		_pixels_D += _strideoff;
		_pixels_R += _strideoff;
	}

	return;
}
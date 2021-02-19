#if !defined _IMAGEPROCESSORS_H__ 
#define  _IMAGEPROCESSORS_H__ 

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ExBitmap.h"

//////////////////////////////////////////////////////////////////////////////////////
// derived image processors

class AFX_EXT_CLASS CImageRotator : public C32BitImageProcessor
{
public:
	CImageRotator(int nDegrees, BOOL bEnableWeighting = TRUE);
	CImageRotator(double dRadians);
	virtual ~CImageRotator();

	virtual CSize CalcDestSize(CSize sizeSrc);
	virtual BOOL ProcessPixels(RGBX* pSrcPixels, CSize sizeSrc, RGBX* pDestPixels, CSize sizeDest);

protected:
	double m_dRadians;
};

class AFX_EXT_CLASS CImageShearer : public C32BitImageProcessor
{
public:
	CImageShearer(int nHorz, int nVert, BOOL bEnableWeighting = TRUE);
	virtual ~CImageShearer();

	virtual CSize CalcDestSize(CSize sizeSrc);
	virtual BOOL ProcessPixels(RGBX* pSrcPixels, CSize sizeSrc, RGBX* pDestPixels, CSize sizeDest);

protected:
	int m_nHorz, m_nVert;
};

class AFX_EXT_CLASS CImageGrayer : public C32BitImageProcessor
{
public:
	CImageGrayer();
	virtual ~CImageGrayer();

	virtual BOOL ProcessPixels(RGBX* pSrcPixels, CSize sizeSrc, RGBX* pDestPixels, CSize sizeDest);
};

class AFX_EXT_CLASS CImageBlurrer : public C32BitImageProcessor
{
public:
	CImageBlurrer(int nAmount = 5); // 1 - 10 (10 is very blurred)
	virtual ~CImageBlurrer();

	virtual BOOL ProcessPixels(RGBX* pSrcPixels, CSize sizeSrc, RGBX* pDestPixels, CSize sizeDest);

protected:
	int m_nAmount;
};

class AFX_EXT_CLASS CImageSharpener : public C32BitImageProcessor
{
public:
	CImageSharpener(int nAmount = 5); // 1 - 10 (10 is very sharp)
	virtual ~CImageSharpener();

	virtual BOOL ProcessPixels(RGBX* pSrcPixels, CSize sizeSrc, RGBX* pDestPixels, CSize sizeDest);

protected:
	int m_nAmount;
};

class AFX_EXT_CLASS CImageResizer : public C32BitImageProcessor
{
public:
	CImageResizer(double dFactor);
	virtual ~CImageResizer();

	virtual CSize CalcDestSize(CSize sizeSrc);
	virtual BOOL ProcessPixels(RGBX* pSrcPixels, CSize sizeSrc, RGBX* pDestPixels, CSize sizeDest);

protected:
	double m_dFactor;

protected:
	BOOL Enlarge(RGBX* pSrcPixels, CSize sizeSrc, RGBX* pDestPixels, CSize sizeDest);
	BOOL Shrink(RGBX* pSrcPixels, CSize sizeSrc, RGBX* pDestPixels, CSize sizeDest);
};

class AFX_EXT_CLASS CImageNegator : public C32BitImageProcessor
{
public:
	CImageNegator();
	virtual ~CImageNegator();

	virtual BOOL ProcessPixels(RGBX* pSrcPixels, CSize sizeSrc, RGBX* pDestPixels, CSize sizeDest);
};

class AFX_EXT_CLASS CImageFlipper : public C32BitImageProcessor
{
public:
	CImageFlipper(BOOL bHorz, BOOL bVert = 0); 
	virtual ~CImageFlipper();

	virtual BOOL ProcessPixels(RGBX* pSrcPixels, CSize sizeSrc, RGBX* pDestPixels, CSize sizeDest);

protected:
	BOOL m_bHorz, m_bVert;
};

class AFX_EXT_CLASS CColorReplacer : public C32BitImageProcessor
{
public:
	CColorReplacer(COLORREF crFrom, COLORREF crTo);
	virtual ~CColorReplacer();

	virtual BOOL ProcessPixels(RGBX* pSrcPixels, CSize sizeSrc, RGBX* pDestPixels, CSize sizeDest);

protected:
	COLORREF m_crFrom, m_crTo;
};


#endif
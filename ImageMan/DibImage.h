////////////////////////////////// Macros ///////////////////////////

#ifndef __DIBIMAGE_H__
#define __DIBIMAGE_H__

////////////////////////////////// Includes /////////////////////////
#include "dibapi.h"   // The "C/SDK" style functions provided by MS to 
                      // handle DIBs which are used internally by CDibImage
/////////////////////////// Classes /////////////////////////////////
//forward declaration
class CDibImage;
//Implements a generic selection object (pure virtual)
class CWorkingArea
{
public:
	virtual BOOL PointInSelection(const CPoint& point) = 0;
	virtual BOOL PointCircleInSelection(const CPoint& point,int R1,int R2)=0;
	virtual CRect BoundingRectangle() = 0;
	virtual CWorkingArea* Clone() = 0;
	BOOL m_bRound;
	BOOL m_bPoly;
};

//A rectangular selection
class CRectWorkingArea : public CWorkingArea
{
public: 
	CRectWorkingArea(CRect rect) 
	{ 
		m_Rect=rect;
		m_bRound=FALSE;
		m_bPoly=FALSE;
		m_Rgn.CreateEllipticRgnIndirect(&m_Rect);
	}
	CRectWorkingArea(CRgn& rgn) 
	{
		if(!m_Rgn.CopyRgn(&rgn))
		{
			AfxMessageBox("Sorry!");
		}
		m_Rect.left=0;
		m_Rect.top=0;
		m_Rect.right=0;
		m_Rect.bottom=0;
		m_bRound=FALSE;
		m_bPoly=TRUE;
		
	}
	CRectWorkingArea() 
	{
		m_Rect.left=0;
		m_Rect.top=0;
		m_Rect.right=0;
		m_Rect.bottom=0;
		m_bRound=FALSE;
		m_bPoly=FALSE;
		m_Rgn.CreateEllipticRgnIndirect(&m_Rect);  
	}
	virtual BOOL CreateWorkingArea(CRgn& rgn,CRect rc) 
	{
		if(!m_Rgn.CopyRgn(&rgn))
		{
			AfxMessageBox("Sorry!");
			return FALSE;
		}
		m_Rect=rc;
		m_bRound=FALSE;
		m_bPoly=TRUE;
		return TRUE;
	}

	virtual BOOL PointInSelection(const CPoint& point) 
	{ 
		if(!m_bRound&&!m_bPoly)
			return m_Rect.PtInRect(point);
		else if (m_bPoly)
		{
			return m_Rgn.PtInRegion(point);
		}
		else
		{
			int height=m_Rect.Height();
			CPoint pt(m_Rect.left+height/2,m_Rect.top+height/2);
			if(4*((point.x-pt.x)*(point.x-pt.x)+(point.y-pt.y)*(point.y-pt.y))
				<=height*height)
				return TRUE;
			else 
				return FALSE;
		}
	}

	virtual BOOL PointCircleInSelection(const CPoint& point,int R1,int R2) 
	{ 	 
		int height=m_Rect.Height();

		CPoint pt(m_Rect.left+height/2,m_Rect.top+height/2);
		int r=int((point.x-pt.x)*(point.x-pt.x)+(point.y-pt.y)*(point.y-pt.y));
		if(R1<=R2)
		{
			if(r<R2*R2&&r>=R1*R1)
			{    
				return TRUE;
			}
			else 
				return FALSE;
		}
		else
			return FALSE;
			
	}
	virtual CRect BoundingRectangle() { return m_Rect; }
	virtual CWorkingArea* Clone() 
	{
		CWorkingArea* pWorkingArea=new CRectWorkingArea(m_Rect);
		pWorkingArea->m_bRound=m_bRound;
		return pWorkingArea;
	}
protected:
	CRect m_Rect;
	CRgn  m_Rgn;
};

//A filter which can be used in call to CDibImage::UserDefinedFilter(..)
class CUserDefinedFilter
{
public:
	virtual COLORREF Filter(CDibImage& dibImage, LPSTR lpDibBits, int x, int y)=0;
};

//a concrete implementation of a user defined filter
class C3By3Filter : public CUserDefinedFilter
{
public:
	C3By3Filter();
	int m_nValues[3][3];
	int m_nDivision;
	int m_nBias;

	virtual COLORREF Filter(CDibImage& dibImage, LPSTR lpDibBits, int x, int y);
};


//a concrete implementation of a user defined filter
class C5By5Filter : public CUserDefinedFilter
{
public:
	C5By5Filter();
	int m_nValues[5][5];
	int m_nDivision;
	int m_nBias;

	virtual COLORREF Filter(CDibImage& dibImage, LPSTR lpDibBits, int x, int y);
};


//a concrete implementation of a user defined filter
class C3By3MedianFilter : public CUserDefinedFilter
{
public:
	C3By3MedianFilter();
	virtual COLORREF Filter(CDibImage& dibImage, LPSTR lpDibBits, int x, int y);

protected:
	static int CompareFunc(const void *elem1, const void *elem2);

	COLORREF m_Ordered[9];
};

//a concrete implementation of a user defined filter
class C7By7Filter : public CUserDefinedFilter
{
public:
	C7By7Filter();
	int m_nValues[7][7];
	int m_nDivision;
	int m_nBias;

	virtual COLORREF Filter(CDibImage& dibImage, LPSTR lpDibBits, int x, int y);
};

//孔洞面积类
class CGrainArea          
{
public:
	CGrainArea(int number,CRect rect) {m_number=number;m_rect=rect;};
	CGrainArea(int x,int y)
	{
		m_number=1;
		m_rect.left=m_rect.right=x;
		m_rect.top=m_rect.bottom=y;
	}
	CGrainArea()
	{
		m_number=0;
		m_rect.left=0;
		m_rect.top=0;
		m_rect.right=0;
		m_rect.bottom=0;
	}
	~CGrainArea(){ };
	int GetNumber() {return m_number;};
	CRect BoundingRectangle() {return m_rect;};
public:
	int m_number;//孔洞中的象素的数目
	CRect m_rect;//孔洞的四个顶点位置
};

class CContour : public CObject  //孔洞轮廓类
{
public:
	CContour(BOOL bAdd);
	CContour();
	virtual ~CContour();
protected:
	DECLARE_SERIAL(CContour);
public:
	BOOL EraserGrain(CDibImage& ds);
    BOOL EraserColorGrain(CDibImage& ds);
	void DrawContour(CDC* pDC,int nWidth,COLORREF c,float zoom);
    BOOL GrainMaxValue(int Red,int Green,int Blue,CDibImage& ds,int nBiasSize,int nSize);
    BOOL FaultGrainRadius(const CPoint& point,int& r1,int& r2,CDibImage& ds);
	//计算孔洞的面积(区域内总点数)
	long ComputeArea();
	//计算孔洞的圆度(椭圆长短轴百分比)
	float ComputeRound(float Hor,float Ver);
	//判断此孔洞区域是否与矩形rt相交
	BOOL IsIntersect(CRect rt);
	//判断pt点是否在此孔洞区域内
	BOOL InRegion(CPoint pt);
	//求出孔洞的R,G,B的平均值
	COLORREF GetAvgRGB(int Red,int Green,int Blue,CDibImage& ds);
	//求出孔洞的R,G,B的平均值
	COLORREF GetTgtPtAvgRGB(CDibImage& ds);
public:
	CArray<CPoint,CPoint&>m_pointArray; //存放孔洞轮廓的坐标
	CRect m_rect;						//存放外接此孔洞的矩形
	BOOL  m_bAdd;                       //m_bAdd表示是否是增加孔洞
	float m_Ver;                        //对象版本号
	float m_Radius;						//孔洞半径
	float m_Round;						//孔洞圆度
	float m_Rx;							//孔洞长轴长度
	float m_Ry;							//孔洞短轴长度
	float m_Phi;						//孔洞长轴角度
	//孔洞中心位置(m_Cx, m_Cy)
	float m_Cx, m_Cy;
	float m_Value;						//平均电导率值
	virtual void Serialize(CArchive& ar);
};


#define EFFECT_SCANDOWN		1
#define EFFECT_SCANUP		2
#define EFFECT_SCANRIGHT	3
#define EFFECT_SCANLEFT		4
#define EFFECT_VSPLITSCAN	5
#define EFFECT_HSPLITSCAN	6
#define EFFECT_MOVEDOWN		7
#define EFFECT_MOVEUP		8
#define EFFECT_MOVERIGHT	9
#define EFFECT_MOVELEFT		10
#define EFFECT_VSPLITMOVE	11
#define EFFECT_HSPLITMOVE	12
#define EFFECT_VCROSSMOVE	13
#define EFFECT_HCROSSMOVE	14
#define EFFECT_VRASTER		15
#define EFFECT_HRASTER		16
#define EFFECT_VBLIND		17
#define EFFECT_HBLIND		18
#define EFFECT_MOSAIC		19
#define EFFECT_FADE			20

//The DIB class itself
class CDibImage
{
public:
	//Creation & Destruction
	CDibImage();
	~CDibImage();
	BOOL	Attach(HGLOBAL hGlobal);
	void	Destroy();
	BOOL	Create(CSize size, WORD nBitCount);
	BOOL	Create(HBITMAP hBitmap);	// DIBSection
	friend BOOL CompareNear20(CDibImage *pDib, int *pArray, int x, int y);
	//这样CompareNear20就可以访问CDibImage里面的私有变量了,因为CompareNear20是CDibImage
	//类的友元函数
	friend BOOL CompareNear8(CDibImage *pDib, int *pArray, int x, int y, int &mark);
	friend BOOL CompareNear48(CDibImage *pDib, int *pArray, int x, int y, int &mark);
	friend BOOL Compare8(CDibImage *pDib, int *pArray, int x, int y, int &mark);
//分类控制参数
public:
	// 特征值聚类数
	int m_KPara;
	float m_HoleMinAcr;
	//仅拾取小于m_HoleMaxAcr的孔洞
	BOOL  m_bHoleMaxAcr;
	//仅拾取大于m_HoleMinAcr的孔洞
	float m_HoleMaxAcr;
	BOOL  m_bHoleMinAcr;

	//仅拾取圆度大于m_HoleMinRound的孔洞
	BOOL m_bMinRound;
	float m_HoleMinRound;
	BOOL m_bMaxRound;
	float m_HoleMaxRound;  
	//图像纵横向比例 仅计算园度时使用
	float m_Hor,m_Ver;
	//对面积标准井线性刻度参数s=s*m_fA+m_fB 仅在以直经为条件去除某孔洞时使用;
	float m_fA,m_fB;

public:
	static WORD     GetVersion();
	static void     RGBtoHSL(COLORREF rgb, double* H, double* S, double* L);
	static COLORREF HLStoRGB(const double& H, const double& L, const double& S);

//Loading & Saving (File & Resource)
	BOOL	Load(LPCTSTR lpszPathName);
	BOOL	Load(HINSTANCE hInst, LPCTSTR lpResourceName);
	BOOL	Load(HINSTANCE hInst, UINT uID) { return Load(hInst, MAKEINTRESOURCE(uID)); };
    BOOL	Save(LPCTSTR lpszPathName);
    BOOL	ColorMedianFilter(LPSTR lpNewDibBits,LPSTR lpDibBits);
    int		Median(int *Median,int iFilterLen);
	BOOL	GetHistogram1(int* RedChannel, int nRedSize ,int* GreenChannel, 
				int nGreenSize, int* BlueChannel, int nBlueSize,int R1,int R2);
	BOOL	GetHistogram2(int* RedChannel1, int*RedChannel, int* GreenChannel1,
				int* GreenChannel, int* BlueChannel1,int* BlueChannel, int nBias,int nSize);
    BOOL	DisplayBack(int R_value,int G_value,int B_value,int nBiasSize,int nSize,int R1,int R2);
	BOOL	Eraseimpurity(BOOL bAdd);

	BOOL	CountContour(BOOL bAdd);
	BOOL	ReverseGray();
	BOOL	DoubleGray();
	void	DistanceTransform();
	unsigned char FindMaxGray();
    BOOL	WaterSheds(unsigned char level);
    BOOL	EraseNoise();
	BOOL	EraseEllipse();
	BOOL	GetContour(BOOL bAdd);
	void	DeleteContour();
    BOOL	EraseFaultGrain(int* Red,int* Green,int* Blue,int* R,int nSubSize,int* nBiasSize,int nSize);
	CWorkingArea* GetWorkArea() const{return m_pWorkingArea;};
	//Clipboard support
	BOOL	CopyToClipboard();
	BOOL	PasteFromClipboard();
	static BOOL PasteAvailable();

//Copying
	CDibImage(const CDibImage& ds);
	CDibImage& operator=(const CDibImage& ds);
	BOOL	CopySelection(CDibImage& dib);
	BOOL	DeleteWorkingArea();
  
//Selection / Working Area support
	CWorkingArea* GetWorkingArea();
	void	SetWorkingArea (CWorkingArea* pWorkingArea);

//Misc functions
	int		ColorsUsed() const;
	LPSTR	GetDIBBits();

//Area Image processing support
	BOOL	SetColor(COLORREF color);
	BOOL	Flip();
	BOOL	Mirror();

//Color Image Processing support
	BOOL	AdjustBrightness(int Percentage);
	BOOL	AdjustContrast(int Percentage);
	BOOL	AdjustGammaCorrection(float Value);
	BOOL	AdjustHighLight(int Percentage);
	BOOL	AdjustMidtone(int Percentage);
	BOOL	AdjustShadow(int Percentage);
	BOOL	AdjustHue(int Percentage);
	BOOL	AdjustSaturation(int Percentage);
	BOOL	AdjustHSL(int PercentHue, int PercentSaturation, int PercentLuminosity);
	BOOL	AdjustRed(int Percentage);
	BOOL	AdjustGreen(int Percentage);
	BOOL	AdjustBlue(int Percentage);
	BOOL	AdjustColor(int dr, int dg, int db);	  
	BOOL	Greyscale();
	BOOL	Iteration();
	BOOL	Ostu(CWorkingArea* m_pSegArea);
	BOOL	HandThreshold(int m_Threshold);
	BOOL	Threshold(CWorkingArea* m_pSegArea,int m_Threshold);
	BOOL	Negate();
	BOOL	AdjustFrame(int left,int top,int right,int bottom);
	void	MakeThumbnail(HBITMAP& hBitmap,int cx,int cy);
	BOOL	MinValueSegmentation(CWorkingArea* m_pSegArea);//my own
	BOOL	Erosion(int nMode,int structure[3][3]);//my own

// effect operation
	BOOL	Display(CDC * pDC, int x, int y, DWORD dwRop);	  
	BOOL	Display(int nEffect, CDC* pDC, int x, int y, int nDeta, DWORD dwDelayTime, DWORD dwRop=SRCCOPY);
	BOOL	Clear(int nEffect, CDC* pDC, int x, int y, int nDeta, DWORD dwDelayTime);
  
//Filter Image processing support
	BOOL	Prewitt();
	BOOL	FindEdgesFilter();
	BOOL	FindVerticalEdgesFilter();
	BOOL	FindHorizontalEdgesFilter();
	BOOL	Smooth();
	BOOL	Sharp();
	BOOL	AddNoiseFilter(int Percentage);
	BOOL	MedianFilter();
	BOOL	UserDefinedFilter(CUserDefinedFilter& Filter);

//Channel processing support
	BOOL	SplitChannels(CDibImage& red, CDibImage& green, CDibImage& blue);
	BOOL	CombineChannels(const CDibImage& red, const CDibImage& green, const CDibImage& blue);
	BOOL	GetRedChannel(CDibImage& red);
	BOOL	GetGreenChannel(CDibImage& green);
	BOOL	GetBlueChannel(CDibImage& blue);
  

//Drawing support
	BOOL	Draw(CDC& dc, const CRect* rcDst=NULL, const CRect* rcSrc=NULL, CPalette* pPal=NULL);

//Data accessors
	CSize	Size() const          { return CSize(m_nWidth, m_nHeight); };
	CRect	Rect() const          { return CRect(CPoint(0, 0), Size()); };
	int		Width() const         { return m_nWidth; };
	int		Height() const        { return m_nHeight; };
	int		ScanWidth() const     { return m_nScanWidth; };
	int		BitsPerPixel() const  { return m_nBitsPerPixel; };
	WORD	GetColorNumber();
	  
//Direct Pixel access
	inline BOOL GetPixel(int x, int y, COLORREF& value, LPSTR lpDibBits = NULL) const;
	inline BOOL SetPixel(int x, int y, const COLORREF& value, LPSTR lpDibBits = NULL);
	BOOL	CountNumber();
	BOOL	TraceContour(BOOL bAdd);
	BOOL	MagicWand(CPoint pt,int radius,BOOL bAdd);

//Channel access support
	BOOL	GetRedHistogram(long* RedChannel, int nSize);
	BOOL	GetGreenHistogram(int* GreenChannel, int nSize);
	BOOL	GetBlueHistogram(int* BlueChannel, int nSize);
	BOOL	GetHistogram(int* RedChannel, int nRedSize, int* GreenChannel, int nGreenSize, int* BlueChannel, int nBlueSize);
// DC for modify DIB
	CDC*	BeginPaint(CDC *pDC);
	void	EndPaint();
	void	UpdateBitmap();
  
//If you really must muck around with the internals 
//of the class
	HDIB	GetHDIB() const { return m_hDib; };
	HPALETTE GetHPALLETTE() const { return m_Pal; };
	void	CaptureRect(HDC hdcScreen, LPRECT prc, BOOL bFullScreen=TRUE);
	HPALETTE GetSystemPalette();
	int		PalEntriesOnDevice(HDC hDC);
	float	GetProportion() {return m_Proportion;};
  	  
protected:
//Internal functions
	int		GetBitsPerPixel() const;
	int		ComputePaletteSize(DWORD nBitCount);
	static double HuetoRGB(double m1, double m2, double h);

private:
	BOOL	UpdateInternal();
	BOOL	DisplayFadeIn(CDC* pDC, int x, int y, int nDeta, DWORD dwDelayTime);
	BOOL	DisplayFadeOut(CDC* pDC, int x, int y, int nDeta, DWORD dwDelayTime);
	BOOL	DisplayMosaicIn(CDC *pDC, int x, int y, int nTileSize, DWORD dwDelayTime, DWORD dwRop=SRCCOPY);
	BOOL	DisplayMosaicOut(CDC *pDC, int x, int y, int nTileSize, DWORD dwDelayTime);
	
//member variables
	HDIB          m_hDib;
	HPALETTE      m_Pal;
	int           m_nWidth;
	int           m_nHeight;
	int           m_nScanWidth;
	int           m_nBitsPerPixel;
	CWorkingArea* m_pWorkingArea;
	DWORD         m_dwChannel;
	int           m_nUndoSize;
  
public:
	HBITMAP		m_hBitmap;	// handle of DIBSection
	CPalette* 	m_pPalette;
	CBitmap*	m_pBitmap;
	// private member data
private:
	// for drawing in DIB
	CDC *		m_pMemDC;
	CBitmap*	m_pBitmapTmp;
	CPalette*	m_pPaletteTmp;
	float       m_Proportion;	
public:
	CArray<CGrainArea,CGrainArea&>m_GrainArea;
	CTypedPtrList<CObList,CContour*>m_ContourList;
};

#endif //__DIBIMAGE_H__





















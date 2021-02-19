#if !defined(AFX_CLOUDS_H__308163D1_9A07_408B_8D97_F595742A8E29__INCLUDED_)
#define AFX_CLOUDS_H__308163D1_9A07_408B_8D97_F595742A8E29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Clouds.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCloudsCtrl window

class CCloudsCtrl : public CStatic
{
// Construction
public:
	CCloudsCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCloudsCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetExponentialClouds(BOOL bFlag = TRUE);
	void SetCredits(LPCTSTR lpszCredits);
	void SetSpeed(int nValue);
	void SetInteractive(BOOL bFlag = TRUE);
	void SetDensity(int nValue);
	virtual ~CCloudsCtrl();

protected:
	void LoadCredits(CDC *pDC);
	CDC m_CreditsDC;
	CString m_strCredits;
	int m_nCounter;

	CDC     m_MainDC;
	CBitmap m_MainBitmap;
	CBitmap *m_pOldBitmap;

	void DrawCtrl();

	int *m_pCloudPlasma1;
	int *m_pCloudPlasma2;
	int *m_pCloudPlasma3;

	CPoint m_ptOffset1;
	CPoint m_ptOffset2;
	CPoint m_ptOffset3;

	CPoint m_ptMove;
	CPoint m_ptDirection;

	CPoint m_ptCursor;

	COLORREF m_BlueColorMap[255];
	double CloudExpCurve(double value);
	void GeneratePlasma(int byteArray[]);

	void UpdateCloudImage();

	int	m_nDensity;
	int m_bInterActive;
	int m_nSpeed;

	int m_nWidth;
	int m_nHeight;

	BITMAPINFO m_bmpInfo;
	int* m_pPixels;
	int m_nSizeOfBits;
	HBITMAP m_hBitmap;

	//{{AFX_MSG(CCloudsCtrl)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
protected:
	int GetRandomValue(int nMin, int nMax);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLOUDS_H__308163D1_9A07_408B_8D97_F595742A8E29__INCLUDED_)

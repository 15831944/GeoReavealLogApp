#if !defined(AFX_HOLEANALYSIS_H__1D5BB5C0_BD02_44DF_BC81_E6E7A70BE1FE__INCLUDED_)
#define AFX_HOLEANALYSIS_H__1D5BB5C0_BD02_44DF_BC81_E6E7A70BE1FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HoleAnalysis.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHoleAnalysis dialog
#include "Grippie.h"
class CHoleAnalysis : public CDialog
{
// Construction
public:
	BOOL GetSlipPoint();
	float ComputeAzi(CPoint OrDot,CPoint Dot);
	void ClearHoleMark();
	void Arrow(CDC *pDC,CPoint m_One, CPoint m_Two,COLORREF Color,BOOL bSolid=TRUE);
	void DrawArrow(CDC *pDC);
	BOOL GetDevAzm();
	BOOL GetPoint();
	void DrawHole(CDC* pDC,BOOL bBmp=FALSE);
	void SendToClip();
	CHoleAnalysis(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHoleAnalysis)
	enum { IDD = IDD_HOLE_ANALYSIS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	BOOL m_bHandMouse,m_bBoxAnisys;
	CRect m_HandRect;
	int m_nNum;CString m_szUnit;
	CPoint *m_points,m_OrDot,m_AziDot;
	//¼ôÇÐ»¬¶¯
	float m_Azi0,m_Azi1;
	double m_fCenterX1,m_fCenterY1,m_fRadius1; 

	float m_Dep,m_MaxValue,m_MinValue,m_fDev,m_fDaz;              	
	//                   ¡ý"¼ü²ÛÄ¥Ëð";"¾®ÑÛ±ÀÂä";"¼ôÇÐ»¬¶¯"
	float m_fHRad,m_fHAzi,m_fHRad1,m_fHAzi1,m_fHRad2,m_fHAzi2,m_fHRad3,m_fHAzi3;
	CString m_szAradFileName,m_szTyle,m_sDEVName,m_sDAZName,m_sAZ1Name;
	double m_fCenterX,m_fCenterY,m_fRadius,m_fScale; 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHoleAnalysis)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHoleAnalysis)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CGrippie m_grippie;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOLEANALYSIS_H__1D5BB5C0_BD02_44DF_BC81_E6E7A70BE1FE__INCLUDED_)

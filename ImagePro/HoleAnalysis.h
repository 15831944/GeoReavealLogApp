#if !defined(AFX_HOLEANALYSIS_H__1D5BB5C0_BD02_44DF_BC81_E6E7A70BE1FE__INCLUDED_)
#define AFX_HOLEANALYSIS_H__1D5BB5C0_BD02_44DF_BC81_E6E7A70BE1FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HoleAnalysis.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHoleAnalysis dialog

class CHoleAnalysis : public CDialog
{
// Construction
public:
	BOOL GetPoint();
	void DrawHole(CDC* pDC);
	CHoleAnalysis(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHoleAnalysis)
	enum { IDD = IDD_HOLE_ANALYSIS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CRect m_HandRect;
	int m_nNum;
	CPoint *m_points;
	CString m_DepStr,m_szAradFileName;
	float m_Dep,m_ComputeRadius,m_ScaleRadius,m_ScaleK;
	float m_MaxValue,m_MinValue;              	
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
	afx_msg void OnPaint();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOLEANALYSIS_H__1D5BB5C0_BD02_44DF_BC81_E6E7A70BE1FE__INCLUDED_)

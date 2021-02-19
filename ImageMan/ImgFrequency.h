#if !defined(AFX_IMGFREQUENCY_H__06570109_6F16_49FB_9004_CDC3B9B19921__INCLUDED_)
#define AFX_IMGFREQUENCY_H__06570109_6F16_49FB_9004_CDC3B9B19921__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImgFrequency.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CImgFrequency dialog

class CImgFrequency : public CDialog
{
// Construction
public:
	void SendToClip();
	BOOL GetFrequency();
	void DrawFrequency(CDC *pDC);
	void DrawRibbon(CDC *pDC);
	void CreateImagePalette();
	CImgFrequency(CWnd* pParent = NULL);   // standard constructor

	BOOL m_bSelect;
	CRect m_rectselect;  //鼠村拖动区区域
// Dialog Data
	//{{AFX_DATA(CImgFrequency)
	enum { IDD = IDD_IMG_FREQUENCY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	int m_Palsize;
	float m_MorMax,m_LitMin;
	BOOL m_bColor,m_bDraw;
	CPalette m_Palette;
	CString m_WellName,m_CurveName;
	float m_ProEnDep,m_ProStDep,m_Rlev;
	float m_MaxValue,m_MinValue,m_MaxValue0,m_MinValue0;
	int m_freq[300];
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImgFrequency)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImgFrequency)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMGFREQUENCY_H__06570109_6F16_49FB_9004_CDC3B9B19921__INCLUDED_)

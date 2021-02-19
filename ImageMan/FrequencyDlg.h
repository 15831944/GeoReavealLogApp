#if !defined(AFX_IMGFREQUENCY_H__06570109_6F16_49FB_9004_CDC3B9B19921__INCLUDED_)
#define AFX_IMGFREQUENCY_H__06570109_6F16_49FB_9004_CDC3B9B19921__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImgFrequency.h : header file
//

class CFrequencyDlg : public CDialog
{
// Construction
public:
	void DrawFrequency(CDC *pDC,BOOL bClip=FALSE);
	CFrequencyDlg(CWnd* pParent = NULL);   // standard constructor
	
	BOOL m_bThinDraw;
	BOOL m_bDrawShape;
	BOOL m_bBlack;

	COLORREF m_ShapeColor;
	COLORREF m_FreColor;

// Dialog Data
	//{{AFX_DATA(CFrequencyDlg)
	enum { IDD = IDD_FREQUENCY_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	//用于刻度标尺
	float m_MaxValue,m_MinValue;
	//用于直方图颜色显示
	float m_ImageMin,m_ImageMax;
	int m_freq[210],m_LineLog,m_NumScale,m_iThinScale,m_iThickScale,m_Point;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrequencyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFrequencyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSendToClip();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMGFREQUENCY_H__06570109_6F16_49FB_9004_CDC3B9B19921__INCLUDED_)

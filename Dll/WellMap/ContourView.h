#if !defined(AFX_CONTOURVIEW_H__F1D75CCE_6DF2_4DB6_96EE_F9207A771C9D__INCLUDED_)
#define AFX_CONTOURVIEW_H__F1D75CCE_6DF2_4DB6_96EE_F9207A771C9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ContourView.h : header file
//

#include "InputReader.h"
#include "Dib.h"

/////////////////////////////////////////////////////////////////////////////
// CContourView view

class AFX_EXT_CLASS CContourView : public CScrollView
{
	DECLARE_DYNCREATE(CContourView)
public:
	CContourView();           // 动态创建所使用的受保护的构造函数
// Attributes
public:
	CInputReader m_ir;

	CString m_XTitle,m_YTitle,m_ZTitle;
	double	m_XMin,m_XMax;int m_XScale;	//X轴刻度
	double	m_YMin,m_YMax;int m_YScale;	//Y轴刻度
	int 	m_ZMin,m_ZMax,m_ZScaleNum;	//Z轴刻度
	int		m_zDataMin,m_zDataMax;		//记录数据表文件中Z轴最小最大值
	int		m_nDiameter;				//图形宽度
	int		m_nDiameter3D;				//3D图形宽度
	int		m_nInterpolater;			//插值方案
	int		m_nLegend;
	int		m_PointNum,m_vecNum;

	BOOL	m_bZAuto;
	CString m_SchemeFile;
	
// Operations
public:
	void Init(LPCTSTR RootName,DWORD RootID);
	CString GetWorkPath(CString flag);
	BOOL GetLogTopRegString(CString szSubKey,LPCTSTR lpszEntry,CString& strValue);
// Operations
public:
	CImage* m_pImage;
	LOGFONT m_Logfont;
	int		m_nNumOfLevels;
	int		m_nBaseWidth;
	int		m_nBaseHeight;
private:
	bool	m_bImageReady;
	bool	m_bLegend;
	bool	m_bValues;
	int		m_nZoom;

	double	m_dThickMax;
	double	m_dThickMin;

	void DrawValues(CDC* pDC);
	void DrawPoints(CDC* pDC);
	void DrawLegend(CDC* pDC);
	void DrawScale(CDC* pDC);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContourView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CContourView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CContourView)
	afx_msg void OnShowvalues();
	afx_msg void OnShowlegend();
	afx_msg void OnUpdateShowvalues(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowlegend(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnZoomLittle();
	afx_msg void OnZoomBig();
	//}}AFX_MSG
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTOURVIEW_H__F1D75CCE_6DF2_4DB6_96EE_F9207A771C9D__INCLUDED_)

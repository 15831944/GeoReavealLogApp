#if !defined(AFX_3DCONTOURVIEW_H__F84F20AB_C0FB_435B_A938_E48E8B1141F6__INCLUDED_)
#define AFX_3DCONTOURVIEW_H__F84F20AB_C0FB_435B_A938_E48E8B1141F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 3DContourView.h : header file
//

#include "GLSurfaceView.h"
#include "InputReader.h"
/////////////////////////////////////////////////////////////////////////////
// C3DContourView view

class AFX_EXT_CLASS C3DContourView : public CGLSurfaceView
{	
	DECLARE_DYNCREATE(C3DContourView)
public:
	C3DContourView();
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

// Operations
public:
	double m_dThickMin;
	double m_dThickMax;
	bool   m_bSmooth;
	void	UpdateIption();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C3DContourView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~C3DContourView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(C3DContourView)
	afx_msg void OnShowlegend();
	afx_msg void OnUpdateShowlegend(CCmdUI* pCmdUI);
	afx_msg void OnCullface();
	afx_msg void OnUpdateCullface(CCmdUI* pCmdUI);
	afx_msg void OnSmooth();
	afx_msg void OnUpdateSmooth(CCmdUI* pCmdUI);
	afx_msg void OnBgcolor();
	afx_msg void OnEditCopy();
	afx_msg void OnShowwall();
	afx_msg void OnUpdateShowwall(CCmdUI* pCmdUI);
	afx_msg void OnShowboundary();
	afx_msg void OnUpdateShowboundary(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_3DCONTOURVIEW_H__F84F20AB_C0FB_435B_A938_E48E8B1141F6__INCLUDED_)

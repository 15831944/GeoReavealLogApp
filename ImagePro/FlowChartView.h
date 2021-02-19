// FlowChartView.h : interface of the CFlowChartView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLOWCHARTVIEW_H__59026E5F_9D8F_4011_9F49_86E5F2906BB1__INCLUDED_)
#define AFX_FLOWCHARTVIEW_H__59026E5F_9D8F_4011_9F49_86E5F2906BB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VerticalTree.h"

class CFlowChartView : public CFormView
{
protected: // create from serialization only
	CFlowChartView();
	DECLARE_DYNCREATE(CFlowChartView)

public:
	//{{AFX_DATA(CFlowChartView)
	enum { IDD = IDD_FLOWCHART_FORM };
	CVerticalTree	m_lnkTreeCtrl;
	//}}AFX_DATA
	BOOL m_bDataBufRun;//"初始化数据缓冲区"
// Attributes
public:
	CFlowChartDoc* GetDocument();
	CFlowChartDoc* pDoc;
// Operations
public:
	void SetFlowChart();
	BOOL RemoveFlowAt(CString ItemName);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlowChartView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	void DelDirAllFile(CString DirName);
	BOOL FindItemRunResut(CString ItemName);
	void UpdateItemEable();
	BOOL InitNextItemPara(CString ItemName);
	virtual ~CFlowChartView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CFlowChartView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDel();
	afx_msg void OnRun();
	afx_msg void OnUpdateRun(CCmdUI* pCmdUI);
	afx_msg void OnDelDatabuf();
	afx_msg void OnUpdateDelDatabuf(CCmdUI* pCmdUI);
	afx_msg void OnRunAll();
	afx_msg void OnUpdateRunAll(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnFlowDbClick();
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in FlowChartView.cpp
inline CFlowChartDoc* CFlowChartView::GetDocument()
   { return (CFlowChartDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLOWCHARTVIEW_H__59026E5F_9D8F_4011_9F49_86E5F2906BB1__INCLUDED_)

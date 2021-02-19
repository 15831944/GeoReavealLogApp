// FlowChartDoc.h : interface of the CFlowChartDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLOWCHARTDOC_H__9283EDBA_EF5F_46F4_A93D_CE5677AD2F1D__INCLUDED_)
#define AFX_FLOWCHARTDOC_H__9283EDBA_EF5F_46F4_A93D_CE5677AD2F1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CFlowChartDoc : public CDocument
{
protected: // create from serialization only
	CFlowChartDoc();
	DECLARE_DYNCREATE(CFlowChartDoc)

// Attributes
public:
	BOOL m_bAunAll;
	char m_VersionNo[10];
	char m_Series[20];
	CObList m_ParentList;
	CObList m_ChildList0;
	CObList m_ChildList1;
	CObList m_ChildList2;
	CString m_szOption;
	CString m_DocFileName;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlowChartDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL
	BOOL FindFlowNode(CString ItemName);
// Implementation
public:
	void DeleteObjList();
	void InitPara();
	virtual ~CFlowChartDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CFlowChartDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
//	afx_msg void OnFileSaveAs();
	afx_msg void OnFileSaveAs();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLOWCHARTDOC_H__9283EDBA_EF5F_46F4_A93D_CE5677AD2F1D__INCLUDED_)

#if !defined(AFX_DLISCURDLG_H_INCLUDED_)
#define AFX_DLISCURDLG_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlisCurDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDlisCurDlg dialog
#include "Resource.h"
class CDlisCurDlg : public CDialog
{
// Construction
public:
	CDlisCurDlg(CWnd* pParent = NULL);   // standard constructor
	bool bSucceed;
	int nTransMode; // 转换模式
	int m_nMaxLen;
	void OnCheckBtn();
	void FreeArrayMem();
	void GetArrayMem();

public:
	CStringArray m_ObjectName; //输出曲线名
	CStringArray m_SourceName; //输入曲线名
	CStringArray m_Origin;     //通道中源
	CStringArray m_CopyNum;    //通道中拷贝数
	CStringArray m_IndexInfo;  //起止索引
	CStringArray m_StepInfo;   //采样间隔
	CStringArray m_FrameNum;   //帧名（用帧对象号代替）
	CStringArray m_Dimension;  //维定义
	CString m_sModuleName;     //模块名称

// Dialog Data
	//{{AFX_DATA(CDlisCurDlg)
	enum { IDD = IDD_EDITDLISCURVES_DLG };
	CListCtrl	m_ctlDlisCurves;
	int		m_nMaxLog;
	int		m_NumLog;
	CString	m_sCurveStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlisCurDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlisCurDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkDliscurvesList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickDliscurvesList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditDliscurvesList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCurvenameAddframeno();
	afx_msg void OnCurvenameAddcopynum();
	afx_msg void OnCurvenameAddorigin();
	afx_msg void OnCurvenameDelbad();
	afx_msg void OnCurvenameDelsame();
	afx_msg void OnCurvenameDeltoolen();
	afx_msg void OnCurvenameDelselect();
	afx_msg void OnCurvenameClearerror();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLISCURDLG_H_INCLUDED_)

#if !defined(AFX_MAINDIALOG_H__7B8222F2_15BD_41F1_B271_706357EB7428__INCLUDED_)
#define AFX_MAINDIALOG_H__7B8222F2_15BD_41F1_B271_706357EB7428__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CurveView.h : header file
//

class CLogPlotView;
class CLogPlotDoc;
class CCurveView;	//单条曲线预览的滚动视
#define IDD_MAIN_DIALOG                 102
class AFX_EXT_CLASS CMainDialog : public CDialog
{
// Construction
public:
	CMainDialog(CWnd* pParent = NULL);   // standard constructor
	
	//数据为链接字符串
	//如：10.132.204.16;sa;log;LogBase
	CString		m_szServer;	
	CString		m_szRole;			//用户角色
	DWORD		m_StorageID;		//存储体ID号
	DWORD		m_ProjectID;		//项目ID号
	DWORD		m_ProductionID;		//成果ID号
	DWORD		m_CurveID;			//曲线ID号
public:
	CLogPlotDoc		*m_pDoc;
	CLogPlotView	*m_pView;
	CCurveView		*m_pCurveView;
	BOOL InitialScrollView();
// Dialog Data
	//{{AFX_DATA(CMainDialog)
	enum { IDD = IDD_MAIN_DIALOG };
	//}}AFX_DATA
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMainDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL DestroyWindow();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CURVEVIEW_H__7B8222F2_15BD_41F1_B271_706357EB7428__INCLUDED_)

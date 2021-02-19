#if !defined(AFX_MAINDIALOG_H__7B8222F2_15BD_41F1_B271_706357EB7428__INCLUDED_)
#define AFX_MAINDIALOG_H__7B8222F2_15BD_41F1_B271_706357EB7428__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CurveView.h : header file
//

class CLogPlotView;
class CLogPlotDoc;
class CCurveView;	//��������Ԥ���Ĺ�����
#define IDD_MAIN_DIALOG                 102
class AFX_EXT_CLASS CMainDialog : public CDialog
{
// Construction
public:
	CMainDialog(CWnd* pParent = NULL);   // standard constructor
	
	//����Ϊ�����ַ���
	//�磺10.132.204.16;sa;log;LogBase
	CString		m_szServer;	
	CString		m_szRole;			//�û���ɫ
	DWORD		m_StorageID;		//�洢��ID��
	DWORD		m_ProjectID;		//��ĿID��
	DWORD		m_ProductionID;		//�ɹ�ID��
	DWORD		m_CurveID;			//����ID��
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

#if !defined(AFX_RUNTIMEDLG_H__AC34D7F7_D4E4_45E3_A746_0CC018F717F1__INCLUDED_)
#define AFX_RUNTIMEDLG_H__AC34D7F7_D4E4_45E3_A746_0CC018F717F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RuntimeDlg.h : header file
//

#include "rcctrlparser.h"

/////////////////////////////////////////////////////////////////////////////
// CRuntimeDlg window

enum RT_CTRLSTATE // for SetCtrlState
{
	RTCS_ENABLED,
	RTCS_DISABLED,
	RTCS_READONLY,
};

class CRuntimeDlg : public CDialog
{
// Construction
public:
	CRuntimeDlg();

	// do not use
	int DoModal() { ASSERT(0); return -1; } 
	BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL) { ASSERT(0); return FALSE; } 
	BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL) { ASSERT(0); return FALSE; } 

// Operations
protected: // intended to be used only from a derived class
	enum 
	{
		RTD_DEFSTYLE = WS_VISIBLE | WS_POPUPWINDOW  | WS_CAPTION | DS_CENTER,
		RTD_DEFEXSTYLE = WS_EX_DLGMODALFRAME,
	};

	virtual int DoModal(LPCTSTR szCaption, 
						DWORD dwStyle = RTD_DEFSTYLE, 
						DWORD dwExStyle = RTD_DEFEXSTYLE, 
						const CRect& rect = rectAuto, 
						CWnd* pParentWnd = NULL, 
						UINT nID = -1);

	virtual BOOL Create(LPCTSTR szCaption, 
						DWORD dwStyle = RTD_DEFSTYLE, 
						DWORD dwExStyle = RTD_DEFEXSTYLE, 
						const CRect& rect = rectAuto, 
						CWnd* pParentWnd = NULL, 
						UINT nID = -1);

	virtual void PostCreate() {}

	// default borders are 7 DLU
	void SetBorders(int nAll) { SetBorders(nAll, nAll, nAll, nAll); }
	void SetBordersDLU(int nAll) { SetBordersDLU(nAll, nAll, nAll, nAll); }
	void SetBorders(int nLeft, int nTop, int nRight, int nBottom);
	void SetBordersDLU(int nLeft, int nTop, int nRight, int nBottom);

	// use these when you do _not_ want to subclass the control
	virtual BOOL AddControl(LPCTSTR szClass, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle, 
							const CRect& rect, UINT nID); // pos in pixels
	virtual BOOL AddControl(LPCTSTR szClass, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle, 
							int x, int y, int cx, int cy, UINT nID); // pos in pixels

	// use these when you _do_ want to subclass via MFC
	virtual BOOL AddControl(CWnd* pWnd, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle, 
							const CRect& rect, UINT nID); // pos in pixels
	virtual BOOL AddControl(CWnd* pWnd, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle, 
							int x, int y, int cx, int cy, UINT nID); // pos in pixels

	// individual RC controls
	virtual BOOL AddRCControl(LPCTSTR szRCType, LPCTSTR szClass, LPCTSTR szCaption, DWORD dwStyle, 
							DWORD dwExStyle, int x, int y, int cx, int cy, UINT nID, UINT uIconID = 0); // pos in DLUs

	virtual BOOL AddRCControl(CWnd* pWnd, LPCTSTR szCaption, DWORD dwStyle, 
							DWORD dwExStyle, int x, int y, int cx, int cy, UINT nID, UINT uIconID = 0); // pos in DLUs

	// 'doctored' text straight from an RC file
	virtual int AddRCControls(const CString& sRCControls);

	// call to size the dialog to fit the controls
	// will be called automatically during the create process if 'rectAuto' specified
	void AutoFit();

	// static helpers
	static CRect OffsetCtrl(CWnd* pParent, UINT nCtrlID, int x = 0, int y = 0);
	static CRect ResizeCtrl(CWnd* pParent, UINT nCtrlID, int cx = 0, int cy = 0);

	static void SetControlState(CWnd* pParent, UINT nCtrlID, RT_CTRLSTATE nState);
	static void SetControlState(HWND hCtrl, RT_CTRLSTATE nState);

	static void SetControlsReadOnly(CWnd* pParent, UINT nCtrlIDFrom, UINT nCtrlIDTo, BOOL bReadOnly = TRUE);
	static void SetControlReadOnly(HWND hCtrl, BOOL bReadOnly = TRUE);

	static void EnableControls(CWnd* pParent, UINT nCtrlIDFrom, UINT nCtrlIDTo, BOOL bEnable = TRUE);
	static void ShowControls(CWnd* pParent, UINT nCtrlIDFrom, UINT nCtrlIDTo, BOOL bShow = TRUE);
	static void ExcludeControls(CWnd* pParent, CDC* pDC, UINT nCtrlIDFrom, UINT nCtrlIDTo);

	static BOOL IsEdit(CWnd* pParent, UINT nCtrlID);
	static BOOL IsEdit(HWND hCtrl);

	static CString GetControlClassName(CWnd* pWnd); // works up the derivation chain till it finds a 'standard' windows control class
	static CString GetClassName(CWnd* pWnd); // returns whatever ::GetClassName() returns

// Attributes
public:
	static const CRect rectAuto;

protected:
	CRTCtrlList m_lstControls;
	CRect m_rBorders;
	HICON m_hILarge, m_hISmall;

	static CMapStringToString s_mapClasses;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRuntimeDlg)
	//}}AFX_VIRTUAL
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
public:
	virtual ~CRuntimeDlg();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRuntimeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	virtual HWND CreateControl(LPCTSTR szClass, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle, 
								int x, int y, int cx, int cy, UINT nID, BOOL bDLU, UINT nIconID = 0);

	// use this when you _do_ want to subclass
	virtual BOOL CreateControl(CWnd* pWnd, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle, 
								int x, int y, int cx, int cy, UINT nID, BOOL bDLU, UINT nIconID = 0);

	BOOL CreateControl(const RTCONTROL& ctrl);
	void CreateControls();
	BOOL AddRCControl(const CString& sRCCtrl);
	CRect OffsetCtrl(UINT nCtrlID, int x = 0, int y = 0);
	CRect ResizeCtrl(UINT nCtrlID, int cx = 0, int cy = 0);
	void EnableControls(UINT uCtrlIDFrom, UINT nCtrlIDTo, BOOL bEnable = TRUE);
	void ShowControls(UINT nCtrlIDFrom, UINT nCtrlIDTo, BOOL bShow = TRUE);
	void ExcludeControls(CDC* pDC, UINT nCtrlIDFrom, UINT nCtrlIDTo);

	inline BOOL IsEdit(UINT nCtrlID) { return IsEdit(this, nCtrlID); }

	static void BuildClassMap();

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RUNTIMEDLG_H__AC34D7F7_D4E4_45E3_A746_0CC018F717F1__INCLUDED_)

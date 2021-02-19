// PrintDlg.h : header file
//

#ifndef __PRINTDLG_H__
#define __PRINTDLG_H__
/////////////////////////////////////////////////////////////////////////////
// CMyPrintDlg dialog

class CMyPrintDlg : public CDialog
{
// Construction
public:
	CMyPrintDlg(CImageManDoc* pDoc1,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMyPrintDlg)
	enum { IDD = IDD_PRINT_DLG };
	BOOL	m_bMaxPageSize;
	BOOL	m_bPlotBottomRem;
	BOOL	m_bPlotHead;
	BOOL	m_bPlotTopRem;
	BOOL	m_bPlotTrack;
	BOOL	m_bReserveSpace;
	BOOL	m_bPlotResultList;
	//}}AFX_DATA
	
	CImageManDoc* pDoc;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyPrintDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMyPrintDlg)
	afx_msg void OnPrint();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // __PRINTDLG_H__

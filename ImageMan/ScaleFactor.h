#if !defined(AFX_SCALEFACTOR_H__92762298_6D20_4930_AE3A_9164E0B55D23__INCLUDED_)
#define AFX_SCALEFACTOR_H__92762298_6D20_4930_AE3A_9164E0B55D23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScaleFactor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScaleFactor dialog

class CScaleFactor : public CDialog
{
// Construction
public:
	CScaleFactor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScaleFactor)
	enum { IDD = IDD_SCALEFACTOR };
	int		m_ScaleFactor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScaleFactor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScaleFactor)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCALEFACTOR_H__92762298_6D20_4930_AE3A_9164E0B55D23__INCLUDED_)

#ifndef __DEPTHDIALOG_H__
#define __DEPTHDIALOG_H__

#include "PlotRes.h"

/////////////////////////////////////////////////////////////////////////////
// CPlotDepthDialog dialog

class CPlotDepthDialog : public CDialog
{
// Construction
public:
	CPlotDepthDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPlotDepthDialog)
	enum { IDD = IDD_PLOT_DEPTH };
	double	m_PlotSdep;
	double	m_PlotEdep;
	//}}AFX_DATA

//operation
public:
	void SetDepth(double Sdep, double Edep);
	void GetDepth(double& Sdep, double& Edep);

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPlotDepthDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDepthScaleDialog dialog

class CDepthScaleDialog : public CDialog
{
// Construction
public:
	CDepthScaleDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDepthScaleDialog)
	enum { IDD = IDD_DEPTH_SCALE };
	double	m_DepthScale;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDepthScaleDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDepthScaleDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //__DEPTHDIALOG_H__

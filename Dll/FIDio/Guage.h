// Guage.h : header file
// CG: This file was added by the Guage Dialog component

/////////////////////////////////////////////////////////////////////////////
// CGuage dialog
#ifndef __PROGDLG_H__
#define __PROGDLG_H__
//#include "WndShadow.h"
#include "FIDRes.h"
class AFX_EXT_CLASS CGuage : public CDialog
{
// Construction / Destruction
public:
    CGuage(float sdep, float edep);   // standard constructor
    ~CGuage();

    BOOL Create(CWnd *pParent=NULL);

    CString m_szGuage;    
// Dialog Data
    //{{AFX_DATA(CGuage)
	enum { IDD = FID_IDD_PROGRESS };
	CStatic	m_PercentCtrl;
	CStatic	m_CurDepCtrl;
    CProgressCtrl	m_Progress;
	//}}AFX_DATA

// Attributes
protected:
    int m_nLower,m_nUpper,m_nStep,m_NextPos,m_CurPos;
	float m_Sdep,m_Edep;
    
    BOOL m_bCancel;
    BOOL m_bParentDisabled;

// Operations
public:
	int SetTitle(const char *Title);
    BOOL CheckCancelButton();
    void SetDepthRange(float sdep, float edep);
    void SetDepth(float dep);
    void SetRange(int nLower,int nUpper);
    int  SetStep(int nStep);
    int  SetPos(int nPos);
protected:
    int  OffsetPos(int nPos);
    int  StepIt();
    void ReEnableParent();
    void UpdatePercent(int nCurrent);
    void PumpMessages();
//	CWndShadow m_Shadow;
// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGuage)
    public:
    virtual BOOL DestroyWindow();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
    // Generated message map functions
    //{{AFX_MSG(CGuage)
    virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // __PROGDLG_H__

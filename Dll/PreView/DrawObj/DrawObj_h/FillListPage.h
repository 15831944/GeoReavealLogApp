// FillListPage.h : header file
//

#ifndef __FILLLISTPAGR_H__
#define __FILLLISTPAGR_H__

/////////////////////////////////////////////////////////////////////////////
// CFillListPage dialog

class CFillListPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFillListPage)

// Construction
public:
	CFillListPage();

// Dialog Data
	//{{AFX_DATA(CFillListPage)
	enum { IDD = IDD_FILL_LIST };
	int		m_Style;
	int		m_ItemNo;
	int		m_PatternType;
	//}}AFX_DATA

protected:
	CCJColorPicker m_Color;
	CFillPatternBox m_StyleBox;
	LOGBRUSH* m_pLogBrush;
	BOOL m_bWindowsPattern;
	int m_FillNum;
	LOGBRUSH m_FillList[20];
	CStringArray m_DescList;
	LOGBRUSH* m_PtrList[20];

	void SetPatternIndirect(LOGBRUSH *m_LogPen);
	void GetPatternIndirect(LOGBRUSH *m_LogPen);  
	void Preview();
public:
	void AddFill(LOGBRUSH* pLogBrush,CString str);

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFillListPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFillListPage)
	afx_msg void OnPaint();
	afx_msg void OnSelchangeStyle();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFillItem();
	afx_msg void OnPatternLth();
	afx_msg void OnPatternSystem();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // __FILLLISTPAGR_H__

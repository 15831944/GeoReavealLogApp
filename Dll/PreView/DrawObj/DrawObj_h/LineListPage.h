// LineListPage.h : header file
//

#ifndef __LINELISTPAGE_H__
#define __LINELISTPAGE_H__

/////////////////////////////////////////////////////////////////////////////
// CLineListPage dialog
class CLineListPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CLineListPage)

// Construction
public:
	CLineListPage();
	~CLineListPage();

// Dialog Data
	//{{AFX_DATA(CLineListPage)
	enum { IDD = IDD_LINE_LIST };
	int		m_Width;
	int		m_Style;
	CSpinButtonCtrl	m_WidthSpinCtrl;
	int		m_ItemNo;
	//}}AFX_DATA

protected:
	CCJColorPicker m_Color;
	CLineStyleBox m_StyleBox;

	int m_LineNum;
	LOGPEN m_LineList[20];
	CStringArray m_DescList;
	LOGPEN* m_PtrList[20];

	void TransferData(BOOL bOutPut);
	void SetLineIndirect(LOGPEN *m_LogPen);
	void GetLineIndirect(LOGPEN *m_LogPen);
	void Preview();

public:
	void AddPen(LOGPEN* pLogPen,CString string);

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLineListPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLineListPage)
	afx_msg void OnChangeWidth();
	afx_msg void OnSelchangeStyle();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSelchangeLineItem();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif  // __LINELISTPAGR_H__

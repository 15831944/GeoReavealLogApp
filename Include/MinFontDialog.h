// FontDialog.h : header file
//

#ifndef __MINFONTDIALOG_H__
#define __MINFONTDIALOG_H__

#include "afxtempl.h"
#include "plotres.h"
#include "chicdial.h"

typedef struct tagTEXTFONT
{
	LOGFONT lf;
	CString m_ItemName;
	CString m_Text;
	int m_BackMode;
	COLORREF m_ForeGround;
	COLORREF m_BackGround;
}TEXTFONT;


/////////////////////////////////////////////////////////////////////////////
// CLocalComboBox

class CLocalComboBox : public CComboBox
{
public:

//Attributes
	CPtrArray m_arrayFontDesc;
	static int m_nFontHeight;
	int m_nLimitText;
	BOOL HasFocus()
	{
		HWND hWnd = ::GetFocus();
		return (hWnd == m_hWnd || ::IsChild(m_hWnd, hWnd));
	}
	void GetTheText(CString& str);
	void SetTheText(LPCTSTR lpszText,BOOL bMatchExact = FALSE);

//Operations
	BOOL LimitText(int nMaxChars);

// Implementation
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// Generated message map functions
	//{{AFX_MSG(CLocalComboBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CFontComboBox : public CLocalComboBox
{
public:
	CFontComboBox();

//Attributes
	CBitmap m_bmFontType;
	BOOL m_bScreen;
	DWORD m_Style;

//Operations
	void EnumFontFamiliesEx(DWORD dwStyle);
	void AddFont(ENUMLOGFONT* pelf, int nType, LPCTSTR lpszScript = NULL);
	void MatchFont(LPCTSTR lpszName, BYTE nCharSet);
	void EmptyContents();

//Overridables
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCIS);
	// Generated message map functions
	//{{AFX_MSG(CFontComboBox)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CSizeComboBox : public CLocalComboBox
{
public:
	CSizeComboBox();

//Attributes
	int m_nLogVert;
	int m_nTwipsLast;
public:
	void EnumFontSizes(CDC& dc, LPCTSTR pFontName);
	static BOOL FAR PASCAL EnumSizeCallBack(LOGFONT FAR* lplf, 
		LPNEWTEXTMETRIC lpntm,int FontType, LPVOID lpv);
	void TwipsToPointString(LPTSTR lpszBuf, int nTwips);
	void SetTwipSize(int nSize);
	int GetTwipSize();
	void InsertSize(int nSize);
};

/////////////////////////////////////////////////////////////////////////////
// CFontPage dialog

class CMinFontPage : public CCSPropertyPage
{
// Construction
public:
	CMinFontPage();
	~CMinFontPage();

protected:
	CSizeComboBox m_FontSizeBox;
	CFontComboBox m_FontBox;
	CCJColorPicker m_ForeGround;
	LOGFONT lf;

// Dialog Data
	//{{AFX_DATA(CMinFontPage)
	enum { IDD = IDD_MIN_FONT };
	CButton	m_Ratate;
	int		m_ItemNo;
	BOOL	m_bRotate;
	CString	m_Style;
	CString	m_Text;
	//}}AFX_DATA

// Attribute
	BOOL bScreen;

// Operations
	void Preview();
	void TransferData(BOOL bOutput);
	void FillFontBox();
	void SetLogFont(LOGFONT* pLogFont);
	void GetLogFont(LOGFONT* pLogFont);
	int m_FontNum;
	TEXTFONT m_FontList[20];
	TEXTFONT* m_PtrList[20];

public:
	void AddFont(TEXTFONT* pTextFont);

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMinFontPage)
	public:
	virtual void OnOK();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static const DWORD m_nHelpIDs[];
	virtual const DWORD* GetHelpIDs() {return m_nHelpIDs;}
	// Generated message map functions
	//{{AFX_MSG(CMinFontPage)
	afx_msg void OnRotate();
	afx_msg void OnChangeText();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnKillfocusFont();
	afx_msg void OnKillfocusFontSize();
	afx_msg void OnDropdownFontSize();
	afx_msg void OnComboBoxCloseup();
	afx_msg void OnSelchangeItem();
	afx_msg void OnSelchangeFontStyle();
	afx_msg void OnSelchangeForeground();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif //__MINFONTDIALOG_H__
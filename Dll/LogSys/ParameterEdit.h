// ParameterEdit.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDefConCtrl window
#ifndef __EDITOR_H__
#define __EDITOR_H__

#include "FIDIO.h"
#include "LogProcess.h"
#define AttNor	0
#define AttTop	1
#define AttBtm	2

class CRuler;
class CTabParaObj : public CObject
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CTabParaObj)
#else
	DECLARE_SERIAL(CTabParaObj);
#endif
public:
	CTabParaObj();
	void Serialize(CArchive& ar);
public:
	CString			szTabName;
	CStringArray	szParaNameArry;
	CStringArray	szParaDspArry;
};
/////////////////////////////////////////////////////////////////////////////
// CRuler window
class CRulerItem
{
public:
	CRulerItem(UINT nBitmapID=0, CPoint pt=CPoint(0, 0), double dep=0, CRuler* pRuler = NULL);
	~CRulerItem();
	virtual BOOL HitTestPix(CPoint pt) { return GetHitRectPix().PtInRect(pt); }
	virtual void Draw(CDC& dc, BOOL bTop, int SliderPos);
	virtual void SetHorzPosPix(int nXPos); 
	virtual void TrackHorzPosPix(int nXPos);
	void Invalidate();
	int GetHorzPosPix() { return m_nPos.x;}
	CRect GetHitRectPix();
	void DrawFocusLine();
	void SetProperties(int aAtt) {nAtt = aAtt;}
	int GetProperties() {return nAtt;}
	CRect GetDepRect(CDC& dc);

	HBITMAP m_hbm;
	HBITMAP m_hbmMask;
	CSize m_size;	// size of item in pixels
	double Depth;
	CPoint m_nPos;
// Operations
	BOOL LoadMaskedBitmap(LPCTSTR lpszResourceName);

protected:
	int nAtt;
	CRuler* m_pRuler;
	BOOL bDepTop;
};

class CRuler : public CStatic
{
// Construction
public:
	CRuler();

// Attributes
public:
	int			m_ZoomNum;
	double		m_SDep;
	double		m_EDep;
	double		m_Rlev;
	int			m_nMin, m_nMax;
	int			CenterH;
	CRulerItem* m_pSelItem;
	CRulerItem* m_pPrevItem;
	CRulerItem* m_pNextItem;
	CObList		m_ObList;
	LOGFONT		m_DepthFont;
	int			m_SelZoon;
	CSliderCtrl* pDepSlider;
private:
	double		AA,  BB;
	double		AA1, BB1;
	CRulerItem* m_pRItem;
	UINT bSplitDep;
	UINT bDelDep;
	UINT bChangeDep;
	// Operations
public:
	void PtDpToLp(LPPOINT point);
	CRulerItem* HitTestPix(CPoint point);
	void SetMinMax(double SDep, double EDep, double Rlev, CSliderCtrl* m_pDepSlider);
	void RecalcRuler();
	void AddTBRulerItem();
	void AddRulerItem(double Dep, int Att);
	double PixToDepth(int nPos);
	int DepthToPix(double dep);
	void ContextMenu(CPoint point, int nMenu);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRuler)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRuler();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRuler)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDepthChange();
	afx_msg void OnDepthDelete();
	afx_msg void OnDepthSplit();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

class CDefConCtrl : public CListCtrl
{
// Construction
public:
	CDefConCtrl();
	void InitCtrl(int No, INPNAME* pNameC, float* pValueC);

// Attributes
public:
	BOOL			m_bDragging;
	int				m_iItemDrag;
	CString			szName, szValue;
	CPoint			m_ptHotSpot;
	CPoint			m_ptOrigin;
	CSize			m_sizeDelta;
	CImageList		*m_pimageListDrag;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefConCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDefConCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDefConCtrl)
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CEditCtrl window

class CEditCtrl : public CListCtrl
{
// Construction
public:
	CEditCtrl();
	void InitCtrl(CWnd* pEdit);

// Attributes
public:
	CWnd*	m_Edit;
	int		col;
	int		ColNum;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditCtrl();
	virtual void PositionEdit();
	virtual int HitTest( CPoint point, UINT* pFlags = NULL );
	virtual BOOL AnyKey();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CInCtrl window

class CInCtrl : public CEditCtrl
{
// Construction
public:
	CInCtrl();
	void InitCtrl(int No, INCURVE* pInCurve, CWnd* pEdit);

// Attributes
public:

// Operations
public:
	BOOL AddInCurve(CString szCurve, CString szCurve1);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// COutCtrl window

class COutCtrl : public CEditCtrl
{
// Construction
public:
	COutCtrl();
	void InitCtrl(int No, OUTCURVE* pOutCurve, CWnd* pEdit);
// Attributes
public:
// Operations
	BOOL AddOutCurve(CString szCurve, CString szCurve1);
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COutCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(COutCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CConCtrl window

class CConCtrl : public CEditCtrl
{
// Construction
public:
	CConCtrl();
	void InitCtrl(CWnd* pEdit);
	void AddCon(CString szName, CString szValue, CPoint point);
	void MoveItem(int m_iItemDrag, int m_iItemDragTo);
	void DelItem(int m_iItemDrag);
	void FillCon(CTabParaObj* pTabParaVar);
// Attributes
public:
	CTabParaObj*	pTabPara;
	BOOL			m_bDragging;
	int				m_iItemDrag;
	int				m_iItemDragTo;
	CPoint			m_ptHotSpot;
	CPoint			m_ptOrigin;
	CSize			m_sizeDelta;
	CImageList		*m_pimageListDrag;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CConCtrl();
	virtual BOOL AnyKey();

	// Generated message map functions
protected:
	//{{AFX_MSG(CConCtrl)
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CDCEdit window

class CDCEdit : public CEdit
{
// Construction
public:
	CDCEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDCEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDCEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDCEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CParameterEdit dialog


class CParameterEdit : public CDialog
{
// Construction
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CParameterEdit(CWnd* pParent, 
					int NNI, int NNO, int NNC,
					INCURVE* m_InCurve, OUTCURVE* m_OutCurve, 
					INPNAME* m_InpName, float* m_InpValue);   // standard constructor
	CObList			m_TabList;
	CObList			m_ValueList;
	CStringArray*	pParaValue;
	CStringArray	szParaNameArry;
	CMenu m_Menu;

	BOOL		bTplModified;
	BOOL		bParaModified;
	CString		m_InpFileName;
	CString		m_ParaExt;
	CString		m_MethodName;
	CString		szTemplateName;
	INCURVE		*pInCurve;
	OUTCURVE	*pOutCurve;       
	INPNAME		*pNameC;
	float		*pValueC;
	double		m_SDep;
	double		m_EDep;
	double		m_Rlev;
	CRulerItem	m_ZoonItem;
	CToolTipCtrl m_ToolTip;

// Dialog Data
	//{{AFX_DATA(CParameterEdit)
	enum { IDD = IDD_ParameterEdit };
	CButton	m_ZoomOut;
	CButton	m_ZoomIn;
	CSliderCtrl	m_DepSlider;
	CComboBox	m_Combo;
	CDCEdit		m_Edit;
	CRuler		m_Ruler;
	COutCtrl	m_OutCtrl;
	CInCtrl		m_InCtrl;
	CDefConCtrl	m_DefConCtrl;
	CTabCtrl	m_ConTab;
	CConCtrl	m_ConCtrl;
	int		NoC;
	int		NoI;
	int		NoO;
	//}}AFX_DATA

// Implementation
public:
	virtual ~CParameterEdit();
	void InitCombo();
	BOOL Is2DCurve(CString szCN);
	BOOL ReadInpData();
	void SavePara(BOOL bDefPara);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParameterEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CParameterEdit)
	afx_msg void OnEdit();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEdit();
	afx_msg void OnAddtab();
	afx_msg void OnChtab();
	afx_msg void OnDeltab();
	afx_msg void OnSavetab();
	afx_msg void OnSelchangeContab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSavepara();
	afx_msg void OnHelp();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	afx_msg void OnSelchangeCombo();
	afx_msg void OnSavedefaultpara();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDepZoomin();
	afx_msg void OnDepZoomout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CTabNameDlg dialog

class CTabNameDlg : public CDialog
{
// Construction
public:
	CTabNameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTabNameDlg)
	enum { IDD = IDD_INPUT_TABNAME };
	CString	m_TabName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTabNameDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
#endif //__EDITOR_H__
/////////////////////////////////////////////////////////////////////////////
// CDepthDlg dialog

class CDepthDlg : public CDialog
{
// Construction
public:
	CDepthDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDepthDlg)
	enum { IDD = IDD_CHANGEDEPTH };
	double	m_Dep;
	double	m_EDep;
	double	m_SDep;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDepthDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDepthDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

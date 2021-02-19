#ifndef __STYLE_H__
#define __STYLE_H__
#define BS_OWNER 100

#define FILLLEFT 0
#define FILLRIGHT 1
#define FILLLEFTRIGHT 2

// CCJColorPopup messages
#define CPN_SELCHANGE        WM_USER + 1001        // Color Picker Selection change
#define CPN_DROPDOWN         WM_USER + 1002        // Color Picker drop down
#define CPN_CLOSEUP          WM_USER + 1003        // Color Picker close up
#define CPN_SELENDOK         WM_USER + 1004        // Color Picker end OK
#define CPN_SELENDCANCEL     WM_USER + 1005        // Color Picker end (cancelled)

// forward declaration
class CColorPicker;

// To hold the Colors and their names
typedef struct {
    COLORREF crColor;
    TCHAR    *szName;
} ColorTableEntry;

/////////////////////////////////////////////////////////////////////////////
// CCJColorPopup window

class CCJColorPopup : public CWnd
{
// Construction
public:
    CCJColorPopup();
    CCJColorPopup(CPoint p, COLORREF crColor, CWnd* pParentWnd,
                 LPCTSTR szDefaultText = NULL, LPCTSTR szCustomText = NULL);
    void Initialise();

// Attributes
public:

// Operations
public:
    BOOL Create(CPoint p, COLORREF crColor, CWnd* pParentWnd, 
                LPCTSTR szDefaultText = NULL, LPCTSTR szCustomText = NULL);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCJColorPopup)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CCJColorPopup();

protected:
    BOOL GetCellRect(int nIndex, const LPRECT& rect);
    void FindCellFromColor(COLORREF crColor);
    void SetWindowSize();
    void CreateToolTips();
    void ChangeSelection(int nIndex);
    void EndSelection(int nMessage);
    void DrawCell(CDC* pDC, int nIndex);

    COLORREF GetColor(int nIndex)              { return m_crColors[nIndex].crColor; }
    LPCTSTR GetColorName(int nIndex)           { return m_crColors[nIndex].szName; }
    int  GetIndex(int row, int col) const;
    int  GetRow(int nIndex) const;
    int  GetColumn(int nIndex) const;

// protected attributes
protected:
    static ColorTableEntry m_crColors[];
    int            m_nNumColors;
    int            m_nNumColumns, m_nNumRows;
    int            m_nBoxSize, m_nMargin;
    int            m_nCurrentSel;
    int            m_nChosenColorSel;
    CString        m_strDefaultText;
    CString        m_strCustomText;
    CRect          m_CustomTextRect, m_DefaultTextRect, m_WindowRect;
    CFont          m_Font;
    CPalette       m_Palette;
    COLORREF       m_crInitialColor, m_crColor;
    CToolTipCtrl   m_ToolTip;
    CWnd*          m_pParent;

    // Generated message map functions
protected:
    //{{AFX_MSG(CCJColorPopup)
    afx_msg void OnNcDestroy();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg BOOL OnQueryNewPalette();
    afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker window

void AFXAPI DDX_CJColorPicker(CDataExchange *pDX, int nIDC, COLORREF& crColor);

/////////////////////////////////////////////////////////////////////////////
// CCJColorPicker window

#define CP_MODE_TEXT 1  // edit text Color
#define CP_MODE_BK   2  // edit background Color (default)

class AFX_EXT_CLASS CCJColorPicker : public CButton
{
// Construction
public:
    CCJColorPicker();
    DECLARE_DYNCREATE(CCJColorPicker);

// Attributes
public:
    COLORREF GetColor();
    void     SetColor(COLORREF crColor); 

    void     SetDefaultText(LPCTSTR szDefaultText);
    void     SetCustomText(LPCTSTR szCustomText);

    void     SetTrackSelection(BOOL bTracking = TRUE)  { m_bTrackSelection = bTracking; }
    BOOL     GetTrackSelection()                       { return m_bTrackSelection; }

    void     SetSelectionMode(UINT nMode)              { m_nSelectionMode = nMode; }
    UINT     GetSelectionMode()                        { return m_nSelectionMode; };

    void     SetBkColor(COLORREF crColorBk);
    COLORREF GetBkColor()                             { return m_crColorBk; }
    
    void     SetTextColor(COLORREF crColorText);
    COLORREF GetTextColor()                           { return m_crColorText;}

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCJColorPicker)
    public:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CCJColorPicker();

protected:
    void SetWindowSize();

// protected attributes
protected:
    BOOL     m_bActive,                // Is the dropdown active?
             m_bTrackSelection;        // track Color changes?
    COLORREF m_crColorBk;
    COLORREF m_crColorText;
    UINT     m_nSelectionMode;
    CRect    m_ArrowRect;
    CString  m_strDefaultText;
    CString  m_strCustomText;
	bool	 m_bLBtnDown;

    // Generated message map functions
protected:
    //{{AFX_MSG(CCJColorPicker)
    afx_msg BOOL OnClicked();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
    afx_msg LONG OnSelEndOK(UINT lParam, LONG wParam);
    afx_msg LONG OnSelEndCancel(UINT lParam, LONG wParam);
    afx_msg LONG OnSelChange(UINT lParam, LONG wParam);

    DECLARE_MESSAGE_MAP()
};             

class CLineStyleBox : public CComboBox
{
public:
	CLineStyleBox(BOOL bWindows = 1);

// Attributes
protected:
	int m_bWindows;

// Operations           
public:
    void SetStyleType(BOOL bWindows);
	void AddItem();

// Implementation
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
};                    

class CFillPatternBox : public CComboBox
{
public:           
	CFillPatternBox(BOOL bWindows = 1);

// Attributes
protected:
	int m_bWindows;
public:

// Operations
	void AddItem();
    void SetStyleType(BOOL bWindows);

// Implementation
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
};                    

class CBitmapPen
{     
public:
	CBitmapPen();
	CBitmapPen(CBitmap* pBitmap,int nWidth,COLORREF nColor);

// Attributes
public:
	COLORREF color;
	CPoint org;
	int len;
	int width;
	int m_LineNo;
	long style;

// Operations
public:
	COLORREF GetColor();
	void SetColor(COLORREF nColor);
	CPoint GetPenOrg();
	void SetPenOrg(const CPoint& point);
	int GetWidth();
	void SetWidth(int nWidth);	
	void CreatePen(CBitmap* pBitmap, int nWidth, COLORREF nColor);
	friend void DrawHorLine(int x1, int x2, int y, CBitmapPen* pPen,CDC* pDC);
};

class CLinePen : public CBitmapPen
{
public:
	CLinePen();
	CLinePen(int nLineNo,int nWidth,COLORREF nColor);

// Attributes
protected:

// Operations
public:
	void CreateLinePen(int nLineNo,int nWidth,COLORREF nColor);
	int GetLineNo();
	void SetLineNo(int nLineNo);
};

extern void DrawHorLine(int x1, int x2, int y, CBitmapPen* pPen, CDC* pDC);
extern BOOL _declspec(dllexport) _cdecl FileExist(const char* s);
extern void _declspec(dllexport) _cdecl HWFillCurve(LPPOINT point1, LPPOINT point2, int nPointNum,int nLthNo,COLORREF nColor,
													CPoint org, CDC* pDC, int nFillMode,COLORREF nBkColor=0xffffff);
extern void _declspec(dllexport) _cdecl FillCurve(LPPOINT point1, LPPOINT point2, int nPointNum,int nLthNo,COLORREF nColor,
												  CPoint org, CDC* pDC, int nFillMode,COLORREF nBkColor=0xffffff);
extern void _declspec(dllexport) _cdecl FillCurve(int x, LPPOINT point, int nPointNum, int nLthNo,COLORREF nColor,
												  CPoint org, CDC* pDC,int nFillMode,COLORREF nBkColor=0xffffff);
extern void _declspec(dllexport) _cdecl FillRoundRect(CRect rect, CPoint roundNess,int nLthNo,COLORREF nColor,
													  CPoint org, CDC* pDC,COLORREF nBkColor=0xffffff);
extern void _declspec(dllexport) _cdecl FillRoundRect(int xl, int yt, int xr, int yb, CPoint roundNess, int nLthNo,COLORREF nColor,
													  CPoint org,CDC* pDC,COLORREF nBkColor=0xffffff);
extern void _declspec(dllexport) _cdecl LithFillBox(int xl, int yt, int xr, int yb, int nLthNo,COLORREF nColor,
													CPoint org,CDC* pDC1,COLORREF nBkColor=0xffffff);
extern void _declspec(dllexport) _cdecl LithFillBox(CRect rect, int nLthNo,COLORREF nColor,
													CPoint org,CDC* pDC1,COLORREF nBkColor=0xffffff);
extern void _declspec(dllexport) _cdecl FillEllipse(CRect rect, int nLthNo,COLORREF nColor,
													CPoint org, CDC* pDC,COLORREF nBkColor=0xffffff);
extern void _declspec(dllexport) _cdecl FillEllipse(int xl, int yt, int xr, int yb,int nLthNo,COLORREF nColor,
													CPoint org, CDC* pDC,COLORREF nBkColor=0xffffff);
extern void _declspec(dllexport) _cdecl LithFillPoly(LPPOINT point, int nPointNum,int nLthNo,COLORREF nColor,
													 CPoint org,CDC* pDC1,COLORREF nBkColor=0xffffff);
extern CSize GetCoreOGSize(int no);
extern CSize GetCoreLthSize(int no);
extern CSize GetCoreSize(int m_OG, int m_Lth);
extern void DrawCoreOG(CDC* pDC,CPoint pt,int no);
extern void DrawCoreLth(CDC* pDC,CPoint pt,int no);
extern void DrawCore(CDC* pDC,CPoint pt,int m_OG,int m_Lth);
extern void SerializeLogFont(CArchive& ar,LOGFONT& lf);
extern void SerializeLogPen(CArchive& ar,LOGPEN& lopn);
extern void SerializeLogBrush(CArchive& ar,LOGBRUSH& lb);
extern void SerializePoint(CArchive& ar,LPPOINT pt, int num);


#endif // __STYLE_H__


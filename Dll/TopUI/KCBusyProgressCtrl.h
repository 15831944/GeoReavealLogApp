/****************************************************************************
 *	class		:	CKCBusyProgressCtrl
 *	author		:	Peter Mares / kinkycode.com (gui@ch.co.za)
 *	base class	:	CStatic (MFC)
 *	notes		:	Control to be used for progress indication when there are
 *					no lower and upper bounds available for a progress bar.
 *					Can also be used as a normal progress bar
 *
 *	Disclaimer	:	Its free, it feels good and its from South Africa :)
 ****************************************************************************/
#if !defined(AFX_BUSYPROGRESSCTRL_H__BEE47141_AAF5_44A5_8408_2C4452BD764A__INCLUDED_)
#define AFX_BUSYPROGRESSCTRL_H__BEE47141_AAF5_44A5_8408_2C4452BD764A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BusyProgressCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKCBusyProgressCtrl window

#define	BPC_MODE_BUSY		0x1
#define	BPC_MODE_PROGRESS	0x2

#define BPM_SETNUMSTEPS		(WM_USER+1)
#define	BPM_SETCURPOS		(WM_USER+2)
#define	BPM_SETIBPAD		(WM_USER+3)
#define	BPM_SETSPEED		(WM_USER+4)
#define	BPM_SETRANGE		(WM_USER+5)
#define	BPM_SETMODE			(WM_USER+6)
#define BPM_STARTBUSY		(WM_USER+7)
#define BPM_ENDBUSY			(WM_USER+8)
#define	BPM_STEPIT			(WM_USER+9)

class AFX_EXT_CLASS CKCBusyProgressCtrl : public CStatic
{
// Construction
public:
	CKCBusyProgressCtrl(int nNumSteps = 10, int nCurPos = 0);
	virtual ~CKCBusyProgressCtrl();

	//
	// Methods
	void				Recalc();
	void				Reset();
	void				Start();
	void				End();
	void				StepIt();

	void				SetNumSteps(int nNumSteps)					{ m_nNumSteps = nNumSteps; Recalc(); }
	int					GetNumSteps()								{ return m_nNumSteps; }
	void				SetCurPos(int nCurPos);	
	int					GetCurPos()									{ return m_nCurPos; }
	void				SetInterBlockPadding(int nPadding)			{ m_nIBPadding = nPadding; Recalc(); }
	int					GetInterBlockPadding()						{ return m_nIBPadding; }
	void				SetSpeed(int nSpeed)						{ m_nSpeed = nSpeed; }
	int					GetSpeed()									{ return m_nSpeed; }
	bool				IsRunning()									{ return (m_pThrd != NULL); }
	void				SetMode(int nMode = BPC_MODE_BUSY);
	int					GetMode()									{ return m_nMode; }
	void				SetRange(int nLower, int nUpper)			{ m_nLower = nLower; m_nUpper = nUpper; }
	void				GetRange(int& nLower, int& nUpper)			{ nLower = m_nLower; nUpper = m_nUpper; }
	
	COLORREF			GetColBkg()									{ return m_colBkg; }
	void				SetColBkg(COLORREF col)						{ m_colBkg = col; }
	COLORREF			GetColBlockFace()							{ return m_colBlockFace; }
	void				SetColBlockFace(COLORREF col)				{ m_colBlockFace = col; }
	COLORREF			GetColBlockEdge()							{ return m_colBlockEdge; }
	void				SetColBlockEdge(COLORREF col)				{ m_colBlockEdge = col; }
	COLORREF			GetColBlockFaceHi()							{ return m_colBlockFaceHi; }
	void				SetColBlockFaceHi(COLORREF col)				{ m_colBlockFaceHi = col; }
	COLORREF			GetColBlockEdgeHi()							{ return m_colBlockEdgeHi; }
	void				SetColBlockEdgeHi(COLORREF col)				{ m_colBlockEdgeHi = col; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKCBusyProgressCtrl)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

protected:
	void PumpMessages();
	//
	// Methods
	virtual void		DrawBackground(CDC& dc, CRect& rect);
	void				DrawBlocks(CDC& dc, CRect& rect);
	virtual void		DrawBlock(CDC& dc, CRect& rect);
	virtual void		DrawHiliteBlock(CDC& dc, CRect& rect);
	virtual void		DrawPartialBlock(CDC& dc, CRect& rect, int nPartial);

	// thread
	static UINT			thrdBusy(LPVOID pParam);

	//
	// Attributes
	int					m_nNumSteps;
	int					m_nStep;
	int					m_nCurPos;
	CRect				m_rect;
	int					m_nIBPadding;
	double				m_dBlockHeight;
	double				m_dBlockWidth;
	CWinThread*			m_pThrd;
	bool				m_bBusyThrd;
	int					m_nSpeed;
	int					m_nMode;
	int					m_nLower;
	int					m_nUpper;

	// colours
	COLORREF			m_colBkg;
	COLORREF			m_colBlockFace;
	COLORREF			m_colBlockEdge;
	COLORREF			m_colBlockFaceHi;
	COLORREF			m_colBlockEdgeHi;

	// drawing stuff
	CDC					m_memDC;
	CBitmap				m_memBmp;
	CBitmap*			m_pOldBmp;

	// Generated message map functions
	//{{AFX_MSG(CKCBusyProgressCtrl)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg LRESULT OnSetNumSteps(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetCurPos(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetIBPad(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetSpeed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetRange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetMode(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStartBusy(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEndBusy(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStepIt(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUSYPROGRESSCTRL_H__BEE47141_AAF5_44A5_8408_2C4452BD764A__INCLUDED_)

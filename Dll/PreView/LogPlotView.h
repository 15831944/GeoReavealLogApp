#pragma once

#include "DrawObj.h"
class CLogPlotDoc;
DECLARE_HANDLE(HDIB);
#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')
/* DIB constants */
#define PALVERSION   0x300
/* DIB Macros*/
#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))
#define RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top)
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

class AFX_EXT_CLASS CLogPlotView : public CScrollView
{
	DECLARE_DYNCREATE(CLogPlotView)
private:
	QGdiPlus m_GdiPlus;
public:
	CLogPlotDoc* m_pDoc;

	CPoint m_LocalPt;
	BOOL m_bActive;          //可交互操作
	BOOL bFirstUpdate;

	BOOL bObjectLink;
	CDrawObj* m_pSelectObj;
	int m_LeftSpace,m_TopSpace;
	BOOL bToolTipActive;
	CString TipText;
	CToolTipCtrl m_tooltip;

public:
	CLogPlotView();           // 动态创建所使用的受保护的构造函数
//protected:	
	virtual ~CLogPlotView();
// Operations
protected:
	CLogPlotDoc* GetDocument()
		{ return m_pDoc;}

	CRect GetInitialPosition();
	void SetScrollSizes(int nMapMode, SIZE sizeTotal);
	void SelectObj(CDrawObj* pObj);
	void UpdateActiveItem();
	void DeSelectObj();
	void GetCenterPoint(LPPOINT pt);

public:
	virtual void DeleteContents();
	virtual void OnInitialUpdate();     // 构造后的第一次
public:
	//保存BMP文件到本地
	void OnPrepareSaveBmp(CDC* pDrawDC);
	HDIB BitmapToDIB(HBITMAP hBitmap, WORD ColorNum);
	DWORD DIBNumColors(LPSTR lpbi);
	DWORD PaletteSize (LPSTR lpbi);
	//下载数据到本地保存为FID格式
	BOOL	DownCurve(CString SaveName,int ProjectID);
	BOOL	DownProduction(CString SaveName,int ProjectID);
// Implementation
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll);
	virtual void OnDraw(CDC* pDC);      // 重写以绘制此视图
	
	virtual void OnUpdate(CView* , LPARAM lHint, CObject* pHint);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActiveView,	CView* pDeactiveView);

	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnEditCut();
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnObjectPropterties();
	afx_msg void OnUpdateObjectPropterties(CCmdUI* pCmdUI);
	
	afx_msg void SaveDocToBmpfile();
	afx_msg void OnUpdateDocSaveBmp(CCmdUI* pCmdUI);
	afx_msg void SaveDocToDatafile();
	afx_msg void OnUpdateDocSaveData(CCmdUI* pCmdUI);

	afx_msg void OnSetupPage();
	afx_msg void OnSetDepth();
	afx_msg void OnUpdateSetDepth(CCmdUI* pCmdUI);
	afx_msg void OnDocdataUpdate();
	afx_msg void OnMatchAllobject();
	afx_msg void OnAddCurve();
	afx_msg void OnUpdateAddCurve(CCmdUI *pCmdUI);
	afx_msg void OnAddTrack();
	afx_msg void OnUpdateAddTrack(CCmdUI *pCmdUI);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
};



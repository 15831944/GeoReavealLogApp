/**
 * Copyright (C) 2005
 * Doga Arinir
 * 
 * Author: Doga Arinir
 * E-Mail: arinir@gmx.de
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the author or the company be held liable 
 * for any damages arising from the use of this software. EXPECT BUGS!
 * 
 * You may use this software in compiled form in any way you desire PROVIDING it is
 * not sold for profit without the authors written permission, and providing that this
 * notice and the authors name is included. If the source code in this file is used in 
 * any commercial application then acknowledgement must be made to the author of this file.
 */


#if !defined(AFX_VERTICALTREE_H__D9ACD532_0DEB_422B_9037_2EC4CCD72144__INCLUDED_)
#define AFX_VERTICALTREE_H__D9ACD532_0DEB_422B_9037_2EC4CCD72144__INCLUDED_

#pragma warning(disable:4786)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VerticalTree1.h : Header-Datei
//
#include <map>
#include "MyBitmap.h"
/////////////////////////////////////////////////////////////////////////////
// Fenster CVerticalTree 
#if !defined BUTTON_SIZE
	#define BUTTON_SIZE 9
#endif

#if !defined ID_FLOW_DBCLICK
#define ID_FLOW_DBCLICK   32775
#endif

class AFX_EXT_CLASS CVerticalTree : public CTreeCtrl
{
	struct ItemViewport
	{
		ItemViewport(HTREEITEM i) 
			{this->item = i;}
		HTREEITEM item;
		int cx;
		int cy;
		int x;
		int y;
		BOOL m_bEnable;
	};
	typedef std::map<HTREEITEM, ItemViewport*> HTREE2VPMAP;
// Konstruktion
public:
	CVerticalTree();
	CMyBitmap   m_NodeButton,m_NodeButton0;
	CMyBitmap   m_NodeButtonL,m_NodeButton0L;
	CMyBitmap   m_NodeButtonR,m_NodeButton0R;
// Attribute
public:
	ItemViewport *m_SelItemView;
	CRect m_SelRect;
protected:
	BOOL m_bHasLines;
	BOOL m_bHasButtons;
	BOOL m_bCustomDrawn;
	int m_ItemSpaceX;
	int m_ItemSpaceY;

	CRect m_Rect;
	int   m_OffsetX;
	int   m_OffsetY;
	int   m_XMax;
	int   m_YMax; 

	COLORREF m_LeafTextColor;
	COLORREF m_LeafBkColor;
	COLORREF m_ItemTextColor;
	COLORREF m_ItemBkColor;

	HTREE2VPMAP m_HTree2VP;
	CPen	m_TreeLinePen;
// Operationen
public:
	void SetCustomDrawn(BOOL);
	BOOL GetCustomDrawn();
	void CleanAllBuf();
	BOOL IsEnable(HTREEITEM nItem);
	void EnableItem(HTREEITEM nItem);
	void AllDisable();
	//These operations are overwritten. The Problem is that those are non-virtual
	//functions...
	HTREEITEM InsertItem(LPTVINSERTSTRUCT lpInsertStruct);
	HTREEITEM InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage, int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam, HTREEITEM hParent, HTREEITEM hInsertAfter);
	HTREEITEM InsertItem(LPCTSTR lpszItem, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
	HTREEITEM InsertItem(LPCTSTR lpszItem, int nImage, int nSelectedImage, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
	BOOL DeleteItem(HTREEITEM hItem);
	void ExpandAllItems();
	
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CVerticalTree)
	//}}AFX_VIRTUAL

// Implementierung
public:
	void DrawSelItem();
	int GetItemSpaceX();
	void SetItemSpaceX(int);
	int GetItemSpaceY();
	void SetItemSpaceY(int);
	virtual ~CVerticalTree();
	void ResetScrollbars();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	
	__forceinline ItemViewport* GetViewport(HTREEITEM hItem);
	void DeleteAllViewportItems();
	void DrawItemText(CDC *pDC, HTREEITEM item, const char *text, int x, int y, int cx, int cy, COLORREF bkcolor,COLORREF txtcolor);
	int DrawItem(CDC *pDC, HTREEITEM, int x, int level = 0);
	void UpdateDimensions();
	//{{AFX_MSG(CVerticalTree)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_VERTICALTREE1_H__D9ACD532_0DEB_422B_9037_2EC4CCD72144__INCLUDED_

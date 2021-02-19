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

#include "stdafx.h"
#include "VerticalTree.h"
#include "FIDio.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVerticalTree

CVerticalTree::CVerticalTree() : CTreeCtrl(), m_bCustomDrawn(TRUE), m_ItemSpaceX(10), m_ItemSpaceY(300)
{
	m_TreeLinePen.CreatePen(PS_DOT,1,0xFF9326);//PS_SOLID

	m_LeafTextColor = 0x008080;//0xF0F0F0;//::GetSysColor(COLOR_3DSHADOW);//0xFFFFFF;
	m_LeafBkColor = ::GetSysColor(COLOR_3DLIGHT);//::GetSysColor(COLOR_3DFACE);//

	m_ItemTextColor =0x00D7FF;//0x00FFFF;//0x0000FF;
	m_ItemBkColor =0xFF9326;
	m_SelItemView=NULL;
	m_SelRect.left =0;m_SelRect.top =0;
	m_SelRect.bottom =0;m_SelRect.right =0;
	
	CString Path=GetWorkPath("SoftSkin");
	m_NodeButton.LoadBitmap(Path+"Node44.bmp");
	m_NodeButton0.LoadBitmap(Path+"Node00.bmp");
	m_NodeButtonL.LoadBitmap(Path+"Node44L.bmp");
	m_NodeButton0L.LoadBitmap(Path+"Node00L.bmp");
	m_NodeButtonR.LoadBitmap(Path+"Node44R.bmp");
	m_NodeButton0R.LoadBitmap(Path+"Node00R.bmp");
}

CVerticalTree::~CVerticalTree()
{
	CleanAllBuf();
	m_NodeButton.DeleteObject();
	m_NodeButton0.DeleteObject();
	m_NodeButtonL.DeleteObject();
	m_NodeButton0L.DeleteObject();
	m_NodeButtonR.DeleteObject();
	m_NodeButton0R.DeleteObject();

}
void CVerticalTree::SetCustomDrawn(BOOL c)
{
	m_bCustomDrawn = c;
}
BOOL CVerticalTree::GetCustomDrawn()
{
	return m_bCustomDrawn;
}

BEGIN_MESSAGE_MAP(CVerticalTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CVerticalTree)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CVerticalTree 

void CVerticalTree::OnPaint() 
{
	if(m_bCustomDrawn)
	{
		CPaintDC dc(this);
		//Double Buffering...
		CDC dc_backbuffer;
		CBitmap bb_image;

		//Calculate scoll position and control dimensions
		UpdateDimensions();

		//Create device context for the backbuffer
		dc_backbuffer.CreateCompatibleDC(&dc);
		//Create the image we are going to draw in
		bb_image.CreateCompatibleBitmap(&dc, m_Rect.Width(), m_Rect.Height());
		dc_backbuffer.SelectObject(&bb_image);

		//We use the same Font which was configured in the main device context
		dc_backbuffer.SelectObject(GetFont());
		dc_backbuffer.SetBkMode(TRANSPARENT);

		//General Info
		DWORD style = GetStyle();
		m_bHasButtons = (style & TVS_HASBUTTONS) != 0;
		m_bHasLines = (style & TVS_HASLINES) != 0;

		//Now draw the items...0xF0F0F0);//
		m_XMax = 0;
		m_YMax = 0;
		dc_backbuffer.FillSolidRect(m_Rect,dc_backbuffer.GetBkColor()); 
		int xoffset = 12;
		for (HTREEITEM root = GetRootItem(); root != NULL; root = GetNextItem(root, TVGN_NEXT))
			xoffset = DrawItem(&dc_backbuffer, root, xoffset);
		
		//Copy the backbuffer to the main device context...
		dc.BitBlt( m_Rect.left, m_Rect.top, m_Rect.Width(), m_Rect.Height(), &dc_backbuffer, 0, 0, SRCCOPY);

		ResetScrollbars();
		DrawSelItem();
	}
	else
		CTreeCtrl::OnPaint();
}

BOOL CVerticalTree::OnEraseBkgnd(CDC* pDC) 
{
	if (m_bCustomDrawn)
		return TRUE;
	else
		return CTreeCtrl::OnEraseBkgnd(pDC);
}

void CVerticalTree::UpdateDimensions()
{
	GetClientRect(&m_Rect);
	SCROLLINFO scroll_info;
	if (GetScrollInfo(SB_HORZ, &scroll_info, SIF_POS | SIF_RANGE))
		m_OffsetX = -scroll_info.nPos;
	else
		m_OffsetX = m_Rect.left;
	if (GetScrollInfo(SB_VERT, &scroll_info, SIF_POS | SIF_RANGE))
	{
		if ( scroll_info.nMin == 0 && scroll_info.nMax == 100) 
			scroll_info.nMax = 0;
		m_OffsetY = -scroll_info.nPos;
	}
	else
		m_OffsetY = m_Rect.top;
}


void CVerticalTree::DrawItemText(CDC *pDC, HTREEITEM item, const char *text, int x, int y, int cx, int cy, COLORREF bkcolor,COLORREF txtcolor)
{
	CRect rect;
	int xx,yy;
	xx=x+m_OffsetX-4;
	yy=y+m_OffsetY-4;
	if(bkcolor==m_LeafBkColor)
	{		
		rect=CRect(xx-5,yy, 15, cy+19);
		m_NodeButton0L.StretchDraw(pDC,&rect);
		rect=CRect(xx+10,yy, cx, cy+19);
		m_NodeButton0.StretchDraw(pDC,&rect);
		rect=CRect(xx+cx,yy, 15, cy+19);
		m_NodeButton0R.StretchDraw(pDC,&rect);
	}
	else
	{
		rect=CRect(xx-5,yy, 15, cy+19);
		m_NodeButtonL.StretchDraw(pDC,&rect);
		rect=CRect(xx+10,yy, cx, cy+19);
		m_NodeButton.StretchDraw(pDC,&rect);
		rect=CRect(xx+cx,yy, 15, cy+19);
		m_NodeButtonR.StretchDraw(pDC,&rect);
	}

	pDC->SetTextColor(txtcolor);
	pDC->TextOut(x+m_OffsetX+m_ItemSpaceX/2,y+m_OffsetY+4,text);

	ItemViewport *nvp = GetViewport(item);
	
	nvp->x = x;
	nvp->y = y;
	nvp->cx = cx;
	nvp->cy = cy;

	if (x + cx > m_XMax)
		m_XMax = x + cx;
	if (y + cy > m_YMax)
		m_YMax = y + cy;
}


int CVerticalTree::DrawItem(CDC *pDC, HTREEITEM item, int x, int level)
{
	CString name = GetItemText(item);
	CSize text_size = pDC->GetTextExtent(name);
	text_size.cx+= m_ItemSpaceX;

	int state = GetItemState(item, TVIF_STATE);
	BOOL selected = IsEnable(item);

	if(ItemHasChildren(item))
	{
		int left = x;
		int right = 0;
		int childcount = 0;
		if (state & TVIS_EXPANDED)
			for (HTREEITEM childitem = GetChildItem(item); childitem != NULL; childitem = GetNextItem(childitem, TVGN_NEXT))
			{
				right = DrawItem(pDC, childitem, x, level + 1);
				x = right;
				childcount++;
			}

		right = right - m_ItemSpaceX;
		int width =  right - left;
		x = left + width / 2 - text_size.cx / 2;
		if (x < left)//)
		   x = left;

		int y = level*m_ItemSpaceY+20;
		if(selected)
			DrawItemText(pDC, item, name, x, level*m_ItemSpaceY+9, text_size.cx, text_size.cy, m_ItemBkColor, m_ItemTextColor);
		else
			DrawItemText(pDC, item, name, x, level*m_ItemSpaceY+9, text_size.cx, text_size.cy, m_LeafBkColor, m_LeafTextColor);
		
		if(m_SelItemView!=NULL&&item==m_SelItemView->item )
		{
			m_SelItemView->x=x;
			m_SelItemView->y=level*m_ItemSpaceY+9;
			m_SelItemView->cx=text_size.cx;
			m_SelItemView->cy=text_size.cy;
		}
		//Draw lines...
		if(m_bHasLines && (state & TVIS_EXPANDED))
		{
			int xstart = x + text_size.cx / 2;
			int ystart = y + text_size.cy + BUTTON_SIZE+4;
			CGdiObject *oldPen = pDC->SelectObject(&m_TreeLinePen);
			for(HTREEITEM childitem = GetChildItem(item); childitem != NULL; childitem = GetNextItem(childitem, TVGN_NEXT))
			{
				ItemViewport *current = GetViewport(childitem);
				pDC->MoveTo(xstart+m_OffsetX,ystart+m_OffsetY);
				pDC->LineTo(current->x +m_OffsetX + current->cx / 2 , current->y + m_OffsetY-5);
			}
			pDC->SelectObject(oldPen);
		}
		if(m_bHasButtons)
		{
			pDC->Draw3dRect(x+m_OffsetX+text_size.cx / 2 - BUTTON_SIZE/ 2, y+m_OffsetY + text_size.cy+5, 
						BUTTON_SIZE, BUTTON_SIZE, ::GetSysColor(COLOR_3DFACE), ::GetSysColor(COLOR_3DSHADOW));
			pDC->MoveTo(x+m_OffsetX+text_size.cx / 2 - BUTTON_SIZE/ 2 + 2, y+m_OffsetY + text_size.cy + BUTTON_SIZE/ 2+5);
			pDC->LineTo(x+m_OffsetX+text_size.cx / 2 + BUTTON_SIZE/ 2 - 1, y+m_OffsetY + text_size.cy + BUTTON_SIZE/ 2+5);
			if ((state & TVIS_EXPANDED) == 0)
			{
				pDC->MoveTo(x+m_OffsetX+text_size.cx / 2, y+m_OffsetY + text_size.cy + 7);
				pDC->LineTo(x+m_OffsetX+text_size.cx / 2, y+m_OffsetY + text_size.cy + BUTTON_SIZE +3);
			}
		}
		if (right > x + text_size.cx)
		   return right + m_ItemSpaceX;
		else
		   return x + text_size.cx + m_ItemSpaceX;
	}
	else
	{
		if(selected)
			DrawItemText(pDC, item, name, x, level*m_ItemSpaceY+9, text_size.cx, text_size.cy,m_ItemBkColor , m_ItemTextColor);
		else
			DrawItemText(pDC, item, name, x, level*m_ItemSpaceY+9, text_size.cx, text_size.cy,m_LeafBkColor , m_LeafTextColor);
		return x + text_size.cx + m_ItemSpaceX;
	}
}

void CVerticalTree::SetItemSpaceX(int space)
{
	m_ItemSpaceX = space;
}

int CVerticalTree::GetItemSpaceX()
{
	return m_ItemSpaceX;
}
void CVerticalTree::SetItemSpaceY(int space)
{
	m_ItemSpaceY = space;
}

int CVerticalTree::GetItemSpaceY()
{
	return m_ItemSpaceY;
}

void CVerticalTree::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (m_bCustomDrawn)
	{
		int iScrollBarPos = GetScrollPos( SB_HORZ );

		switch( nSBCode )
		{
			case SB_LINEUP:
				iScrollBarPos = max( iScrollBarPos - 1, 0 );
			break;

			case SB_LINEDOWN:
				iScrollBarPos = min( iScrollBarPos + 1, GetScrollLimit( SB_HORZ ) );
			break;

			case SB_PAGEUP:
				iScrollBarPos = max( iScrollBarPos - m_Rect.Width(), 0 );
			break;

			case SB_PAGEDOWN:
				iScrollBarPos = min( iScrollBarPos + m_Rect.Width(), GetScrollLimit( SB_HORZ ) );
			break;

			case SB_THUMBTRACK:
			case SB_THUMBPOSITION:
				iScrollBarPos = nPos;
				break;
		}		
		SetScrollPos( SB_HORZ, iScrollBarPos );		
		Invalidate();
	}
	else
		CTreeCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CVerticalTree::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if (m_bCustomDrawn)
	{
		int iScrollBarPos = GetScrollPos( SB_VERT )-zDelta;
		SetScrollPos( SB_VERT, iScrollBarPos );		
		Invalidate();
		return TRUE;
	}
	else	
		return CTreeCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

void CVerticalTree::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (m_bCustomDrawn)
	{
		int iScrollBarPos = GetScrollPos( SB_VERT );
		switch( nSBCode )
		{
			case SB_LINEUP:
				iScrollBarPos = max( iScrollBarPos - 1, 0 );
			break;

			case SB_LINEDOWN:
				iScrollBarPos = min( iScrollBarPos + 1, GetScrollLimit( SB_VERT ) );
			break;

			case SB_PAGEUP:
				iScrollBarPos = max( iScrollBarPos - m_Rect.Height(), 0 );
			break;

			case SB_PAGEDOWN:
				iScrollBarPos = min( iScrollBarPos + m_Rect.Height(), GetScrollLimit( SB_VERT ) );
			break;

			case SB_THUMBTRACK:
			case SB_THUMBPOSITION:
				iScrollBarPos = nPos;
				break;
		}		
		SetScrollPos( SB_VERT, iScrollBarPos );
		Invalidate();
	}
	else
		CTreeCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CVerticalTree::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_bCustomDrawn)
	{
		CPoint pt=point;
		HTREE2VPMAP::iterator it = m_HTree2VP.begin();
		pt.x -= m_OffsetX;
		pt.y -= m_OffsetY;
		while (it != m_HTree2VP.end())
		{
			ItemViewport *vp = (*it).second;
			if (vp->x <= pt.x && ((vp->x+vp->cx) >= pt.x)
				&& vp->y <= pt.y && ((vp->y+vp->cy+10) >= pt.y))
			{
				CDC* dc=this->GetDC ();
				int DrawMode = dc->SetROP2(R2_XORPEN);
				CPen AxialPen;
				AxialPen.CreatePen(PS_SOLID, 2, 0xffff00);
				CPen *OldPen = (CPen *)dc->SelectObject(&AxialPen);
				CBrush* pOldBrush=(CBrush*)dc->SelectStockObject (NULL_BRUSH);
				dc->Rectangle (m_SelRect);

				m_SelRect.left =vp->x-10;
				m_SelRect.top =vp->y-5;
				m_SelRect.right =vp->x+vp->cx+11;
				m_SelRect.bottom =vp->y+vp->cy+15;
				m_SelRect.OffsetRect (m_OffsetX,m_OffsetY);
				dc->Rectangle (m_SelRect);

				dc->SetROP2( DrawMode );
				dc->SelectObject( OldPen );
				dc->SelectObject(pOldBrush);
				m_SelItemView=vp;
				return;
			}
			else if ((vp->x+vp->cx/2-10) <= pt.x && (vp->x+vp->cx/2+10) >= pt.x
				&& (vp->y+vp->cy+20) <= pt.y && (vp->y+vp->cy+10) >= pt.y)
			{
				Expand(vp->item, TVE_TOGGLE);
				Invalidate();
				return;
			}
			it++;
		}
	}
	else	
		CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CVerticalTree::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_bCustomDrawn)
	{
	}
	else	
		CTreeCtrl::OnMouseMove(nFlags, point);
}


HTREEITEM CVerticalTree::InsertItem(LPTVINSERTSTRUCT lpInsertStruct)
{
	HTREEITEM htreeitem = CTreeCtrl::InsertItem(lpInsertStruct);
	//I presume that CTreeCtrl::InsertItem won't create same handles,
	//if the key is in use this will lead to memory leaks...
	m_HTree2VP.insert(HTREE2VPMAP::value_type(htreeitem, new ItemViewport(htreeitem)));
	return htreeitem;
}
HTREEITEM CVerticalTree::InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage, int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	HTREEITEM htreeitem = CTreeCtrl::InsertItem(nMask, lpszItem, nImage, nSelectedImage, nState, nStateMask, lParam, hParent, hInsertAfter);
	//I presume that CTreeCtrl::InsertItem won't create same handles,
	//if the key is in use this will lead to memory leaks...
	m_HTree2VP.insert(HTREE2VPMAP::value_type(htreeitem, new ItemViewport(htreeitem)));
	return htreeitem;
}
HTREEITEM CVerticalTree::InsertItem(LPCTSTR lpszItem, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	HTREEITEM htreeitem = CTreeCtrl::InsertItem(lpszItem, hParent, hInsertAfter);
	//I presume that CTreeCtrl::InsertItem won't create same handles,
	//if the key is in use this will lead to memory leaks...
	m_HTree2VP.insert(HTREE2VPMAP::value_type(htreeitem, new ItemViewport(htreeitem)));
	return htreeitem;
}
HTREEITEM CVerticalTree::InsertItem(LPCTSTR lpszItem, int nImage, int nSelectedImage, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	HTREEITEM htreeitem = CTreeCtrl::InsertItem(lpszItem, nImage, nSelectedImage, hParent, hInsertAfter);
	//I presume that CTreeCtrl::InsertItem won't create same handles,
	//if the key is in use this will lead to memory leaks...
	m_HTree2VP.insert(HTREE2VPMAP::value_type(htreeitem, new ItemViewport(htreeitem)));
	return htreeitem;
}
BOOL CVerticalTree::DeleteItem(HTREEITEM hItem)
{
	BOOL ret = CTreeCtrl::DeleteItem(hItem);
	HTREE2VPMAP::iterator it = m_HTree2VP.find(hItem);
	if (it != m_HTree2VP.end())
	{
		delete (*it).second;
		m_HTree2VP.erase(it);
	}
	return ret;
}

void CVerticalTree::DeleteAllViewportItems()
{
	HTREE2VPMAP::iterator it = m_HTree2VP.begin();
	while (it != m_HTree2VP.end())
	{
		delete (*it).second;
		m_HTree2VP.erase(it);
		it++;
	}
}

void CVerticalTree::ExpandAllItems()
{
	HTREE2VPMAP::iterator it = m_HTree2VP.begin();
	while (it != m_HTree2VP.end())
	{
		ItemViewport *vp = (*it).second;
		Expand(vp->item, TVE_TOGGLE);		
		it++;
	}
	Invalidate();
}

__forceinline CVerticalTree::ItemViewport* CVerticalTree::GetViewport(HTREEITEM hItem)
{
	HTREE2VPMAP::iterator it = m_HTree2VP.find(hItem);
	if (it != m_HTree2VP.end())
		return (*it).second;
	return NULL;
}

void CVerticalTree::ResetScrollbars()
{
	CRect rect;
	GetClientRect(rect);

	if(rect.Height() > m_YMax + 16 )
	{
		ShowScrollBar( SB_VERT, FALSE );	// Hide it
		SetScrollPos( SB_VERT, 0 );
	}
	else
	{
		SCROLLINFO	si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE | SIF_RANGE;
		si.nPage = rect.Height();
		si.nMax = m_YMax + 16;
		si.nMin = 0 ;

		SetScrollInfo( SB_VERT, &si );
		EnableScrollBarCtrl( SB_VERT, TRUE );
	}

	if(rect.Width() > m_XMax + 8 )
	{
		ShowScrollBar( SB_HORZ, FALSE );	// Hide it
		SetScrollPos( SB_HORZ, 0 );
	}
	else
	{
		SCROLLINFO	si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE | SIF_RANGE;
		si.nPage = rect.Width();
		si.nMax = m_XMax + 8;
		si.nMin = 0 ;

		SetScrollInfo( SB_HORZ, &si );
		EnableScrollBarCtrl( SB_HORZ, TRUE );
	}
}

void CVerticalTree::OnSizing(UINT fwSide, LPRECT pRect) 
{
	if (!m_bCustomDrawn)
		CTreeCtrl::OnSizing(fwSide, pRect);
}

void CVerticalTree::OnSize(UINT nType, int cx, int cy) 
{
	if (!m_bCustomDrawn)
		CTreeCtrl::OnSize(nType, cx, cy);
	else
	{
		ResetScrollbars();
		Invalidate();
	}	
}

void CVerticalTree::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_bCustomDrawn)
	{
		CPoint pt=point;		
		HTREE2VPMAP::iterator it = m_HTree2VP.begin();
		pt.x -= m_OffsetX;
		pt.y -= m_OffsetY;
		while (it != m_HTree2VP.end())
		{
			ItemViewport *vp = (*it).second;
			if (vp->x <= pt.x && ((vp->x+vp->cx) >= pt.x)
				&& vp->y <= pt.y && ((vp->y+vp->cy+10) >= pt.y))
			{			
				HTREEITEM item=vp->item ;
				if(IsEnable(vp->item))
				{Invalidate();
					HWND hwnd = this->GetParent ()->m_hWnd ;
					if(hwnd!=NULL)
						::SendMessage(hwnd, WM_COMMAND,ID_FLOW_DBCLICK,0);
					Invalidate();
				}
				
				return;
			}
			it++;
		}
	}
	else
		CTreeCtrl::OnLButtonDblClk(nFlags, point);
}

void CVerticalTree::DrawSelItem()
{
	if(m_SelItemView!=NULL&&m_bCustomDrawn)
	{
		CDC* dc=this->GetDC ();
		int DrawMode = dc->SetROP2(R2_XORPEN);
		CPen AxialPen;
		AxialPen.CreatePen(PS_SOLID, 2, 0xffff00);
		CPen *OldPen = (CPen *)dc->SelectObject(&AxialPen);
		CBrush* pOldBrush=(CBrush*)dc->SelectStockObject (NULL_BRUSH);
 
		m_SelRect.left =m_SelItemView->x-10;
		m_SelRect.top =m_SelItemView->y-5;
		m_SelRect.right =m_SelItemView->x+m_SelItemView->cx+11;
		m_SelRect.bottom =m_SelItemView->y+m_SelItemView->cy+15;
		m_SelRect.OffsetRect (m_OffsetX,m_OffsetY);
		dc->Rectangle (m_SelRect);

		dc->SetROP2( DrawMode );
		dc->SelectObject( OldPen );
		dc->SelectObject(pOldBrush);
	}
}

void CVerticalTree::CleanAllBuf()
{
	m_HTree2VP.clear();
	DeleteAllItems();
	m_SelItemView=NULL;
	m_SelRect.left =0;m_SelRect.top =0;
	m_SelRect.bottom =0;m_SelRect.right =0;	
}

BOOL CVerticalTree::IsEnable(HTREEITEM nItem)
{
	HTREE2VPMAP::iterator it = m_HTree2VP.begin();
	while (it != m_HTree2VP.end())
	{
		ItemViewport *vp = (*it).second;
		if(vp->item==nItem)
			return vp->m_bEnable;
		it++;
	}
	return FALSE;
}

void CVerticalTree::AllDisable()
{
	HTREE2VPMAP::iterator it = m_HTree2VP.begin();
	while (it != m_HTree2VP.end())
	{
		ItemViewport *vp = (*it).second;
		vp->m_bEnable=FALSE;
		it++;
	}
	Invalidate();
}

void CVerticalTree::EnableItem(HTREEITEM nItem)
{
	HTREE2VPMAP::iterator it = m_HTree2VP.begin();
	while (it != m_HTree2VP.end())
	{
		ItemViewport *vp = (*it).second;
		if(vp->item==nItem)
		{
			vp->m_bEnable=TRUE;
			return;
		}
		it++;
	}
	Invalidate();
}
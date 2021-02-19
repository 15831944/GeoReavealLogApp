// ListEditCtrl.cpp : implementation file
//

#include "stdafx.h"
//#include "topui.h"
#include "ListEditCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListEditCtrl

CListEditCtrl::CListEditCtrl()
{
	m_StartCol=0;
	m_EndCol = 2;
	m_SelCol=-1;
}

CListEditCtrl::~CListEditCtrl()
{
}


BEGIN_MESSAGE_MAP(CListEditCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CListEditCtrl)
	ON_WM_VSCROLL()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListEditCtrl message handlers
void CListEditCtrl::InitCtrl(CWnd* pEdit)
{
	m_Edit = pEdit;
}
void CListEditCtrl::PositionEdit() 
{
	int row;
	row = GetNextItem( -1, LVNI_SELECTED );
	if ( row == -1) {
		m_Edit->ShowWindow(SW_HIDE);
		return;	
	}
	CPoint point;
	GetCursorPos(&point) ;
	ScreenToClient(&point) ;
	HitTest(point);

	::SetParent(m_Edit->GetSafeHwnd(), this->GetSafeHwnd());
	CRect rect;
	GetItemRect( row, &rect, LVIR_BOUNDS );
	int i;
	rect.left=0;
	if(m_SelCol>-1)
	{
		for (i=0; i<m_SelCol; i++)
			rect.left +=GetColumnWidth(i);
		rect.right =rect.left+GetColumnWidth(m_SelCol);
	}
	else
	{
		for (i=0; i<m_StartCol; i++)
			rect.left +=GetColumnWidth(i);
		rect.right =rect.left+GetColumnWidth(m_StartCol);
	}
	rect.left -= GetScrollPos(SB_HORZ);
	rect.right -= GetScrollPos(SB_HORZ);
	CRect rect1;
	GetClientRect(&rect1);
	if (rect.right > rect1.right) 
		rect.right=rect1.right;
	CString szText;
	if(m_SelCol>-1)
		szText = GetItemText( row, m_SelCol);
	else
		szText = GetItemText( row, m_StartCol);
	m_Edit->SetWindowText(szText);
	m_Edit->MoveWindow(rect.left, rect.top, rect.Width(), rect.Height(), FALSE);
	m_Edit->ShowWindow(SW_SHOW);
	m_Edit->SetFocus();
}

int CListEditCtrl::HitTest( CPoint point, UINT* pFlags )
{
	int row = -1;
	m_SelCol=-1;
	CRect rect;
	int i;
	for(i=0;i<GetItemCount();i++)
	{
		GetItemRect( i, &rect, LVIR_BOUNDS );
		if (rect.PtInRect(point))
		{
			row = i;
			break;
		}
	}
	if(row>-1)
	{
		rect.left=0;
		for (i=0; i<m_StartCol; i++)
			rect.left +=GetColumnWidth(i);
		for(i=m_StartCol;i<m_EndCol;i++)
		{
			rect.right =rect.left+GetColumnWidth(i);
			if(rect.PtInRect(point))
			{
				m_SelCol=i;
				break;
			}
			rect.left=rect.right;
		}
	}
	return row;
}

void CListEditCtrl::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	PositionEdit();
	*pResult = 0;
}
void CListEditCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
	PositionEdit();
}
void CListEditCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
	PositionEdit();
}

void CListEditCtrl::OnPaint() 
{
	CListCtrl::OnPaint();
	if (m_Edit->GetParent() != this) return;
	if (!m_Edit->IsWindowVisible()) return;
	int row;
	row = GetNextItem( -1, LVNI_SELECTED );
	if ( row == -1) return;
	if ( row <GetTopIndex()) {
		m_Edit->ShowWindow(SW_HIDE);
		return;
	}
    CRect rect1;
	m_Edit->GetWindowRect(&rect1);
	CRect rect2;
	GetItemRect( row, &rect2, LVIR_BOUNDS );
	ClientToScreen(&rect2);
	if (abs(rect2.top-rect1.top) >=2)
     PositionEdit();
}

void CListEditCtrl::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	PositionEdit();
	*pResult = 0;
}


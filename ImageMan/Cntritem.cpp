/******************************************************************
*   Cntritem.cpp - implementation of the cntritem class           *           
*******************************************************************/

#include "stdafx.h"
#include "afxpriv.h"
#include "ImageMan.h"

#include "ImageManDoc.h"
#include "OleDraw.h"
#include "CntrItem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawItem implementation

IMPLEMENT_SERIAL(CDrawItem, COleClientItem, 0)

CDrawItem::CDrawItem(CImageManDoc* pContainer, COleObj* pDrawObj)
	: COleClientItem(pContainer)
{
	m_pDrawObj = pDrawObj;
}

CDrawItem::~CDrawItem()
{
	// TODO: add cleanup code here
}

void CDrawItem::OnChange(OLE_NOTIFICATION nCode, DWORD dwParam)
{
	ASSERT_VALID(this);

	COleClientItem::OnChange(nCode, dwParam);
	CView* pView = GetActiveView();

	switch(nCode)
	{
	case OLE_CHANGED_STATE:
	case OLE_CHANGED_ASPECT:
		m_pDrawObj->Invalidate(pView);
		break;
	case OLE_CHANGED:
		UpdateExtent(); // extent may have changed
		m_pDrawObj->Invalidate(pView);
		break;
	}
}

BOOL CDrawItem::OnChangeItemPosition(const CRect& rectPos)
{
	ASSERT_VALID(this);

	CView* pView = GetActiveView();
	ASSERT_VALID(pView);
	CRect rect = rectPos;
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc,NULL);
	dc.DPtoLP(rect);

	if (rect != m_pDrawObj->m_Rect)
	{
		// invalidate old rectangle
		m_pDrawObj->Invalidate(pView);

		// update to new rectangle
		m_pDrawObj->m_Rect = rect;
		GetExtent(&m_pDrawObj->m_extent);

		// and invalidate new rectangle
		m_pDrawObj->Invalidate(pView);
	}
	return COleClientItem::OnChangeItemPosition(rectPos);
}

void CDrawItem::OnGetItemPosition(CRect& rect)
{
	ASSERT_VALID(this);

	// update to extent of item if m_position is not initialized
	if (m_pDrawObj->m_Rect.IsRectEmpty())
		UpdateExtent();

	// copy m_position, which is in document coordinates
	CView* pView = GetActiveView();
	ASSERT_VALID(pView);
	rect = m_pDrawObj->m_Rect;
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc,NULL);
	dc.DPtoLP(rect);
}

void CDrawItem::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	// Call base class first to read in COleClientItem data.
	// Note: this sets up the m_pDocument pointer returned from
	//  CDrawItem::GetDocument, therefore it is a good idea
	//  to call the base class Serialize first.
	COleClientItem::Serialize(ar);
}

BOOL CDrawItem::UpdateExtent()
{
	CSize size;
	if (!GetExtent(&size) || size == m_pDrawObj->m_extent)
		return FALSE;       // blank

	// if new object (i.e. m_extent is empty) setup position
	if (m_pDrawObj->m_extent == CSize(0, 0))
	{
		m_pDrawObj->m_Rect.right =
			m_pDrawObj->m_Rect.left + MulDiv(size.cx, 10, 254);
		m_pDrawObj->m_Rect.bottom =
			m_pDrawObj->m_Rect.top + MulDiv(size.cy, 10, 254);
	}
	// else data changed so scale up rect as well
	else if (!IsInPlaceActive() && size != m_pDrawObj->m_extent)
	{
		m_pDrawObj->m_Rect.right = m_pDrawObj->m_Rect.left +
			MulDiv(m_pDrawObj->m_Rect.Width(), size.cx, m_pDrawObj->m_extent.cx);
		m_pDrawObj->m_Rect.bottom = m_pDrawObj->m_Rect.top +
			MulDiv(m_pDrawObj->m_Rect.Height(), size.cy, m_pDrawObj->m_extent.cy);
	}

	m_pDrawObj->m_extent = size;
	CView* pView = GetActiveView();
	m_pDrawObj->Invalidate(pView);   // redraw to the new size/position
	return TRUE;
}

void CDrawItem::OnActivate()
{
	// allow only one inplace active item per frame
	CView* pView = GetActiveView();
	ASSERT_VALID(pView);
	COleClientItem* pItem = GetDocument()->GetInPlaceActiveItem(pView);
	if (pItem != NULL && pItem != this)
		pItem->Close();

	COleClientItem::OnActivate();
}

void CDrawItem::OnDeactivateUI(BOOL bUndoable)
{
	COleClientItem::OnDeactivateUI(bUndoable);

	// hide the object if it is not an outside-in object
	DWORD dwMisc = 0;
	m_lpObject->GetMiscStatus(GetDrawAspect(), &dwMisc);
	if (dwMisc & OLEMISC_INSIDEOUT)
		DoVerb(OLEIVERB_HIDE, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CDrawItem diagnostics

#ifdef _DEBUG
void CDrawItem::AssertValid() const
{
	COleClientItem::AssertValid();
}

void CDrawItem::Dump(CDumpContext& dc) const
{
	COleClientItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////

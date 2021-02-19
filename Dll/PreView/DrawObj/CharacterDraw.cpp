/******************************************************************
*   RectDrw.cpp - implementation of Character draw object class   *           
*******************************************************************/

#include "stdafx.h" 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "TrackDraw.h"
#include "CharacterDraw.h"            
//#include "HeadPlotView.h"
//#include "MinFontDialog.h"
//#include "TextAlignPage.h"

IMPLEMENT_SERIAL(CharacterObj, CTextObj, 0)

CharacterObj::CharacterObj()
{
	m_ShapeName = "Character";
}

CharacterObj::CharacterObj(const CRect& rect,CDrawObj* pObj,CLogPlotDoc* pDocument) 
		: CTextObj(rect,pObj,pDocument)
{
	m_ShapeName = "Character";
	m_TextVertAlignMode = IDM_TOP;
	m_TextHoriAlignMode = IDM_LEFT;	
}

void CharacterObj::Serialize(CArchive& ar)
{
	CTextObj::Serialize(ar);
}

void CharacterObj::Draw(CView* pView,CDC* pDC)
{
	CRect rect = m_Rect;	
	pDoc->LPtoPP(rect);
	if (!pDC->RectVisible(rect)) return;
	CPen BoxPen(PS_NULL,1,RGB(255,255,255));
	CBrush BoxBrush(RGB(255,255,255));
	CPen* OldPen=pDC->SelectObject(&BoxPen);
	int DrawModel=pDC->SetROP2 (R2_NOTXORPEN);
	pDC->Rectangle (rect);
	pDC->SetROP2(DrawModel);
	pDC->SelectObject(OldPen);
	BoxPen.DeleteObject();
	CTextObj::Draw(pView,pDC);
	CDrawObj::Draw(pView,pDC);
}


int CharacterObj::HitTest(CView* pView,const CPoint& point)
{
	ASSERT_VALID(this);
	ASSERT(pView != NULL);
	int nHandle =  CTextObj::HitTest(pView,point);
	if (nHandle >=0 ) return nHandle;
	//if ((pView->IsKindOf(RUNTIME_CLASS(CHeadPlotView))) && (!((CHeadPlotView*)pView)->m_bEditEnable)) return -1;
	CRgn rgn,rgn1,rgn2;
	CRect rect (point.x-3,point.y-3,point.x+3,point.y+3);
	rgn1.CreateRectRgnIndirect(rect);
	CRect rect1 = m_Rect;
	if (bInput || (m_bSelected && (!IsTextEmpty()))) 
		rect1.InflateRect(3,3);
    rgn2.CreateRectRgnIndirect(rect1);
    rgn.CreateRectRgn(0,0,0,0);
    if (rgn.CombineRgn(&rgn1,&rgn2,RGN_AND) != NULLREGION) 
    {
	 	if (m_LogBrush.lbStyle != BS_NULL) return 0;
	 	if (!rgn.EqualRgn(&rgn1)) return 0;
    }                          
    return -1;
}

void CharacterObj::CreateFocusObj(CView* pView)
{
	m_pFocusObj = new CharacterObj(m_Rect,m_pParentObj,pDoc);
	m_pFocusObj->pDoc = pDoc;     
	m_pFocusObj->m_TextRect = m_TextRect;     
	m_pFocusObj->m_TextHoriAlignMode = m_TextHoriAlignMode;     
	m_pFocusObj->m_TextVertAlignMode = m_TextVertAlignMode;     
	m_pFocusObj->DrawFocus(pView);
}

void CharacterObj::DeleteFocusObj(CView* pView)
{
	ASSERT((m_pFocusObj != NULL) && (pView != NULL));
	if (m_pFocusObj == NULL) return;
	if (pView != NULL) m_pFocusObj->DrawFocus(pView);
 	delete m_pFocusObj;
	m_pFocusObj = NULL;
}   

void CharacterObj::DrawFocusObj(CView* pView)
{
	ASSERT((m_pFocusObj != NULL) && (pView != NULL));
	if ((m_pFocusObj == NULL) || (pView == NULL)) return;
	m_pFocusObj->DrawFocus(pView);
	if (m_SelectMode != move) return;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if (pObj != NULL) pObj->DrawFocusObj(pView);
	}                  
}

void CharacterObj::DrawFocus(CView* pView)
{
	ASSERT(pView != NULL);
	if (pView == NULL) return;
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc,NULL);
	dc.SetROP2(R2_XORPEN);
    CPen m_Pen(PS_DOT,1,RGB(0,0,0));
    CPen *pOldPen = dc.SelectObject(&m_Pen);
    CBrush* pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	CRect rect = m_Rect;	
	pDoc->LPtoPP(rect);
	dc.Rectangle(rect);
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
	dc.SetROP2(R2_COPYPEN);
	if (m_SelectMode == move) CTextObj::DrawFocus(pView);
}   

void CharacterObj::OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point)
{
    if (m_bSelected)
    {
		
		if (c_ToolShape == texttool)
		{
			m_SelectMode = size;
			nDragHandle = 5;
		}              
		else
		{
			nDragHandle = HitTest(pView,point); 
			if (nDragHandle == 0) 
			{
				CTextObj::OnLButtonDown(pView,nFlags,point);
				if (m_SelectMode != move) return;
			}	
			else if (nDragHandle > 0) m_SelectMode = size; 
			else return;
			SetCursor(GetHandleCursor(nDragHandle));
		}	
	    CDrawObj::OnLButtonDown(pView,nFlags,point);
	}	
	if (m_SelectMode == move)
	{
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if (pObj != NULL) pObj->OnLButtonDown(pView, nFlags, point);
		}                  
	}	
	CreateFocusObj(pView);
}

void CharacterObj::OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point)
{
	if (bInput) return;
	if (m_SelectMode == move)
	{
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if (pObj != NULL) pObj->OnLButtonUp(pView, nFlags, point);
		}                  
		if (!m_bSelected)
		{
			DeleteFocusObj(pView);
			return;
		}	
	}	
	if (m_bSelected)
	{
		CRect m_FocusRect = m_pFocusObj->m_Rect;
		DeleteFocusObj(pView);  
		m_FocusRect.NormalizeRect();
		if (m_FocusRect.Width() < 5) m_FocusRect.right = m_FocusRect.left + 5;
		if (m_FocusRect.Height() < 5) m_FocusRect.bottom = m_FocusRect.top + 5;
		if (m_Rect == m_FocusRect)
		{
		}
		else if (m_SelectMode == move)
		{
	    	SetRect(m_FocusRect);
			if (m_pParentObj == NULL) pDoc->OnChildPosChange(this);
			else m_pParentObj->OnChildPosChange(this);
		}   
		else if (m_SelectMode == size)
		{
			CRect rect = m_Rect;
			m_Rect = m_FocusRect;
			if ((m_Rect.Width() < 5) && (m_Rect.Height() < 5))
				m_Rect.InflateRect(5,5);
			if (rect.Width() != m_Rect.Width())
			{
				if (m_pParentObj == NULL) pDoc->OnChildWidthChange(this);
				else m_pParentObj->OnChildWidthChange(this);
			}
			if (rect.Height() != m_Rect.Height())
			{
				if (m_pParentObj == NULL) pDoc->OnChildHeightChange(this);
				else m_pParentObj->OnChildHeightChange(this);
			}
		}
	}
	CDrawObj::OnLButtonUp(pView,nFlags,point);
}

void CharacterObj::OnMouseMove(CView* pView, UINT nFlags, const CPoint& point)
{
	CPoint delta = (CPoint)(point - c_last);
	m_pFocusObj->DrawFocus(pView);
	if (m_SelectMode == move)
	{
		CPoint point1 = point;
		if (m_bSelected)
		{
			CRect rect = m_pFocusObj->m_Rect;
			rect.NormalizeRect();
			if ((rect.right + delta.x > m_xMax) && (delta.x > 0)) 
				point1.x = m_xMax - rect.right + c_last.x;
			if ((rect.left + delta.x < m_xMin) && (delta.x < 0)) 
				point1.x = m_xMin - rect.left + c_last.x;
			if ((rect.bottom + delta.y > m_yMax) && (delta.y > 0)) 
				point1.y = m_yMax - rect.bottom + c_last.y;
			if ((rect.top + delta.y < m_yMin) && (delta.y < 0)) 
				point1.y = m_yMin - rect.top + c_last.y;
			if (point1.x != point.x) delta.x = point1.x - c_last.x;
			if (point1.y != point.y) delta.y = point1.y - c_last.y;
		}
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if (pObj != NULL) pObj->OnMouseMove(pView, nFlags, point1);
		}
		m_pFocusObj->MoveHandle(pView,nDragHandle, delta);
	}	
	if (m_SelectMode == size)
		m_pFocusObj->MoveHandle(pView,nDragHandle, delta);
	m_pFocusObj->DrawFocus(pView);
	if (m_bSelected) 
	{
		DoScroll((CScrollView*)pView);
		CDrawObj::OnMouseMove(pView,nFlags,point);
	}
}

BOOL CharacterObj::Properties(CView* pView)
{
	/*if (bShowCaret) pView->HideCaret();
	if (bCaretExist) ::DestroyCaret();
	bCaretExist = FALSE;
	bShowCaret = FALSE; 
	if (bInput)
		SetInput(pView,FALSE,CPoint(0,0));
	else
	{
		bInput = FALSE;
		InvalObj(pView);
	}
	CString szHT;
	CPropertySheet m_Sheet("∂‘œÛ Ù–‘");
	TEXTFONT m_FontItem;
	int Num=m_Text.GetSize ();
	int i;
	CString str="";
	for(i=0;i<Num-1;i++)
	{
		str+=m_Text.GetAt(i);
		str+=char(13);
		str+=char(10);
	}
	str+=m_Text.GetAt(i);
	if(str.IsEmpty ()) 
	{
		szHT.LoadString(IDS_NOTESTEXT);	
		szHT.ReleaseBuffer();
	}
	else
		szHT=str;
	
	m_FontItem.m_Text = szHT;
	szHT.LoadString(IDS_NOTESTEXT);
	m_FontItem.m_ItemName = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	m_FontItem.m_ForeGround = m_TextColor;
	m_FontItem.m_BackGround = RGB(255,255,255);
	m_FontItem.lf = m_TextFont;
	CMinFontPage m_FontPage;
	m_FontPage.AddFont(&m_FontItem);
	m_Sheet.AddPage(&m_FontPage);

	CTextALignPage m_AlignPage;
	m_AlignPage.m_bClip = m_bClip;
	m_AlignPage.m_bRotate = m_bRotate;
	m_AlignPage.m_TextHoriAlignMode = m_TextHoriAlignMode;
	m_AlignPage.m_TextVertAlignMode = m_TextVertAlignMode;
	m_Sheet.AddPage(&m_AlignPage);

	if(m_Sheet.DoModal() == IDOK)
	{
		m_FontPage.OnOK();
		str=m_FontItem.m_Text;
		m_Text.RemoveAll();
		while(str!="")
		{
			Num=str.Find(char(13));
			if(Num>0)
			{
				szHT=str.Left(Num);
				str=str.Mid(Num+1);
			}
			else
			{
				szHT=str;
				str="";
			}
			m_Text.Add(szHT);
		}
		m_TextHoriAlignMode = m_AlignPage.m_TextHoriAlignMode;
		m_TextVertAlignMode = m_AlignPage.m_TextVertAlignMode;

		m_TextColor = m_FontItem.m_ForeGround;
		m_TextFont = m_FontItem.lf;
		m_CharSize.cy = 0;
		m_bClip = m_AlignPage.m_bClip;
		m_bRotate = m_AlignPage.m_bRotate;
		if(m_bRotate)
			m_TextFont.lfEscapement = 2700;
		else
			m_TextFont.lfEscapement = 0;
		pDoc->SetModifiedFlag();
		Invalidate(pView);
		return TRUE;
	}*/
	return FALSE;
}

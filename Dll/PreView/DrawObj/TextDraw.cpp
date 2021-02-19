/******************************************************************
*   Textdrw.cpp - implementation of text draw object base class   *           
*******************************************************************/

#include "stdafx.h" 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "LogPlotView.h"
#include "TextDraw.h"

//#include "HeadPlotView.h"
//#include "MinFontDialog.h"
//#include "TextAlignPage.h"
//#include "LineListPage.h"
//#include "FillListPage.h"
//#include "styledialog.h"

IMPLEMENT_SERIAL(CTextObj, CRemDrawObj, 0)

CTextObj::CTextObj()
{
 	m_DrawShape = textobj;
	WORD hatchPattern[8];
	WORD wPattern = 0x11;
	for (int i = 0; i < 4; i++)
	{
		hatchPattern[i] = wPattern;
		hatchPattern[i+4] = wPattern;
		wPattern <<= 1;
	}
	CBitmap hatchBitmap;
	hatchBitmap.CreateBitmap(8, 8, 1, 1, &hatchPattern);
	hatchBrush.CreatePatternBrush(&hatchBitmap);
	bCaretExist = false; bShowCaret = false; bInput = false;
	m_CharSize.cy = 0; 
	m_Text.Add("Text");
	m_TextRect = m_Rect; m_TextRect.right = m_TextRect.left;
	m_TextRect.top = m_TextRect.bottom;m_Rect0 = m_Rect;

	LOGFONT textFont=
	{
		-14,				 	    //lfHeight
		0,							//lfWidth
		0,							//lfEscapement
		0,							//lfOrientation
		FW_NORMAL,	//NORMAL	BOLD			//lfWeight
		0,							//lfItalic
		0,							//lfUnderLine;
		0,							//lfStrikeOut;
		ANSI_CHARSET,			    //lfCharSet
		OUT_TT_ONLY_PRECIS,			//lfOutPrecision
		CLIP_DEFAULT_PRECIS,		//lfClipPrecision
		DEFAULT_QUALITY,			//lfQuality
		VARIABLE_PITCH | FF_ROMAN,	//lfPitchAndFamily
		"Times New Roman"			//lfFaceName
	};      
	m_TextFont =textFont;
	m_bClip = FALSE;
	m_bRotate = FALSE;
	m_TextColor = 0x0;
	m_TextVertAlignMode = IDM_CENTER;
	m_TextHoriAlignMode = IDM_CENTER;

}              

CTextObj::CTextObj(const CRect& rect,CDrawObj* pObj,CLogPlotDoc* pDocument) 
		: CRemDrawObj(rect,pObj,pDocument)
{
 	m_DrawShape = textobj;
	WORD hatchPattern[8];
	WORD wPattern = 0x11;
	for (int i = 0; i < 4; i++)
	{
		hatchPattern[i] = wPattern;
		hatchPattern[i+4] = wPattern;
		wPattern <<= 1;
	}
	CBitmap hatchBitmap;
	hatchBitmap.CreateBitmap(8, 8, 1, 1, &hatchPattern);
	hatchBrush.CreatePatternBrush(&hatchBitmap);
	bCaretExist = FALSE; bShowCaret = FALSE;bInput = FALSE; 
	m_Text.Add(""); 
	m_CharSize.cy = 0;
	m_TextRect = m_Rect; m_TextRect.right = m_TextRect.left;
	m_TextRect.top = m_TextRect.bottom;	

	LOGFONT textFont=
	{
		-14,				 	    //lfHeight
		0,							//lfWidth
		0,							//lfEscapement
		0,							//lfOrientation
		FW_NORMAL,	//NORMAL	BOLD			//lfWeight
		0,							//lfItalic
		0,							//lfUnderLine;
		0,							//lfStrikeOut;
		ANSI_CHARSET,			    //lfCharSet
		OUT_TT_ONLY_PRECIS,			//lfOutPrecision
		CLIP_DEFAULT_PRECIS,		//lfClipPrecision
		DEFAULT_QUALITY,			//lfQuality
		VARIABLE_PITCH | FF_ROMAN,	//lfPitchAndFamily
		"Times New Roman"			//lfFaceName
	};      
	m_TextFont =textFont;
	m_bClip = FALSE;
	m_bRotate = FALSE;
	m_TextColor = 0x0;
	m_TextVertAlignMode = IDM_CENTER;
	m_TextHoriAlignMode = IDM_CENTER;
}

CTextObj::~CTextObj()
{
	if (bCaretExist) ::DestroyCaret();
	if (bInput) SetInput(NULL,FALSE,0);
}

void CTextObj::Serialize(CArchive& ar)
{
	LONG l;
	CRemDrawObj::Serialize(ar);
	if (ar.IsStoring())
	{
		SerializeLogFont(ar,m_TextFont);
		ar << (long) m_TextColor;
		ar << m_TextHoriAlignMode;
		ar << m_TextVertAlignMode;
		ar << m_bClip;
		ar << m_bRotate;
	}
	else
	{
		pDoc = (CLogPlotDoc*)ar.m_pDocument;
		ASSERT_VALID(pDoc);
		ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CLogPlotDoc)));
		SerializeLogFont(ar,m_TextFont);
		ar >> l; m_TextColor = l;
		ar >> m_TextHoriAlignMode;
		ar >> m_TextVertAlignMode;
		ar >> m_bClip;
		ar >> m_bRotate;
		m_Rect0 = m_Rect;
	}
	m_Text.Serialize(ar);
}

BOOL CTextObj::Properties(CView* pView)
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
 	CLineListPage m_LinePage;
	m_LinePage.AddPen(&m_LogPen,m_ShapeName);
 	CFillListPage m_FillPage;
	m_FillPage.AddFill(&m_LogBrush,m_ShapeName);
	m_Sheet.AddPage(&m_FillPage);
	m_Sheet.AddPage(&m_LinePage);
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
		if (m_bRotate)
			m_TextFont.lfEscapement = 2700;
		else
			m_TextFont.lfEscapement = 0;
		pDoc->SetModifiedFlag();
		Invalidate(pView);
		return TRUE;
	}*/
	return FALSE;
}

CPoint CTextObj::GetHandlePoint(int nHandle)
{
	ASSERT_VALID(this);
	CRect rect = m_Rect;
	if (bInput || (m_bSelected && (!IsTextEmpty())))
		m_Rect.InflateRect(7,7);
	CPoint point = CRemDrawObj::GetHandlePoint(nHandle);
	m_Rect = rect;
	return point;
}

int CTextObj::HitTest(CView* pView,const CPoint& point)
{
	ASSERT_VALID(this);
	ASSERT(pView != NULL);

	/*if ((pView->IsKindOf(RUNTIME_CLASS(CHeadPlotView))) && (!((CHeadPlotView*)pView)->m_bEditEnable))
	{
		if (m_Rect.PtInRect(point)) return 0;
		else return -1;
	}*/
	if ((m_bSelected) && (!bInput))
	{
		int nHandleCount = GetHandleCount();
		for (int nHandle = nHandleCount; nHandle >= 1; nHandle -= 1)
		{
			// GetHandleRect returns in logical coords
			CRect rc = GetHandleRect(pView,nHandle);
			if (rc.PtInRect(point)) return nHandle;
		}
	}
    CRect rect1 = m_TextRect;
	if (!bInput && m_bClip) rect1 = m_TextRect & m_Rect;
	if ((!IsTextEmpty()) && (rect1.PtInRect(point))) return 0;

	if (bInput && m_bRotate) return -1;
	CRgn rgn,rgn1,rgn2;
    rgn.CreateRectRgn(0,0,0,0);
	CRect rect (point.x-3,point.y-3,point.x+3,point.y+3);
    rgn1.CreateRectRgnIndirect(rect);
	if (bInput || (m_bSelected && (!IsTextEmpty()))) 
	{
		CRect rect1 = m_Rect;
		rect1.InflateRect(3,3);
    	rgn2.CreateRectRgnIndirect(rect1);
    	if ((rgn.CombineRgn(&rgn1,&rgn2,RGN_AND) != NULLREGION) 
	 		&& (!rgn.EqualRgn(&rgn1))) return 0;
	}
	return -1;
}

void CTextObj::DrawTracker(CView* pView, CDC* pDC, TrackerState state)
{
	if ((!bInput) && (c_ToolShape == none)) CRemDrawObj::DrawTracker(pView,pDC,state);
}

BOOL CTextObj::IsTextEmpty()
{
	for (int i=0; i<m_Text.GetSize(); i++)
		if (m_Text[i] != "") return FALSE;
	return TRUE;
}

void CTextObj::Draw(CView* pView,CDC* pDC)
{
	CRect saveTextRect;
	CRect saveRect;
	saveTextRect = m_TextRect;
	saveRect = m_Rect;
	pDoc->LPtoPP(m_Rect);
	pDoc->LPtoPP(m_TextRect);

	CPoint m_CaretPoint;
	CRect m_ClipRect;
	pDC->GetClipBox(m_ClipRect);

	BOOL bActiveView = (pView == pView->GetFocus());
	if (bShowCaret && bActiveView)  
	{
		bShowCaret = FALSE; 
		pView->HideCaret();
	}
	//if (! ( (pView->IsKindOf(RUNTIME_CLASS(CHeadPlotView))) && (!((CHeadPlotView*)pView)->m_bEditEnable) ) )
		if ((bInput || (m_bSelected && (!IsTextEmpty()))) && bActiveView)
		{
			hatchBrush.UnrealizeObject();
			CBrush* pOldBrush = pDC->SelectObject(&hatchBrush);
			pDC->SetBkMode(TRANSPARENT);
			CRect rectTrue = m_Rect;     
			CRect rect = m_Rect;
			rectTrue.InflateRect(4,4); 
			pDC->PatBlt(rectTrue.left, rectTrue.top, rectTrue.Width(),
			rect.top-rectTrue.top, 0x000F0001 /* Pn */);
			pDC->PatBlt(rectTrue.left, rect.bottom,
			rectTrue.Width(), rectTrue.bottom-rect.bottom, 0x000F0001 /* Pn */);
			pDC->PatBlt(rectTrue.left, rect.top, rect.left-rectTrue.left,
			rect.Height(), 0x000F0001 /* Pn */);
			pDC->PatBlt(rect.right, rect.top, rectTrue.right-rect.right,
			rect.Height(), 0x000F0001 /* Pn */);
			pDC->SelectObject(pOldBrush);
		}    
	if (bInput && bActiveView)
	{
	 	CBrush br;
 		if (m_LogBrush.lbStyle == BS_HATCHED) 
		 	br.CreateHatchBrush((int)m_LogBrush.lbHatch,m_LogBrush.lbColor);
   		else
		{
			if (m_LogBrush.lbStyle == BS_NULL)
				br.CreateSolidBrush(0xc0c0c0);
			else br.CreateBrushIndirect(&m_LogBrush);
		}
		pDC->FillRect(m_Rect,&br);
	}	
	CFont m_Font;
	if ((m_bRotate) && (!bInput)) m_TextFont.lfEscapement = 2700;
	else m_TextFont.lfEscapement = 0;
	if (m_Font.CreateFontIndirect(&m_TextFont))
	{
		CFont* pOldFont = pDC->SelectObject(&m_Font);
   		pDC->SetTextAlign(TA_LEFT | TA_TOP);
		pDC->SetTextColor(m_TextColor);
		pDC->SetBkMode(TRANSPARENT);
		if (m_CharSize.cy == 0) 
			m_CharSize = pDC->GetTextExtent("A",1);
		ComputerTextRect(pDC);
		saveTextRect = m_TextRect;
		pDoc->PPtoLP(saveTextRect);
		BOOL bClip = FALSE;
		CRect rect1,rect2;
		if (rect1.IntersectRect(m_ClipRect,m_TextRect)) 
		{
			CSize size;        
		 	int y = m_TextRect.top;                                   
			int x = m_TextRect.right;
	 	
			if ((m_bClip) && (!bInput || !bActiveView))
			{
				if (rect2.IntersectRect(m_Rect,rect1) && (rect1 != rect2))
				{
					bClip = TRUE;
					if (pDC->m_hDC != NULL) ::LPtoDP(pDC->m_hDC,(LPPOINT)(LPRECT)rect2,2);
					CRgn rgn; rgn.CreateRectRgn(rect2.left,rect2.top,rect2.right,rect2.bottom);
					pDC->SelectClipRgn(&rgn);
				}
			}
			
			for (int i=0; i<m_Text.GetSize(); i++)
			{
				size = pDC->GetTextExtent(m_Text[i],m_Text[i].GetLength());
				if(m_bRotate && (!bInput))
				{
					if (m_TextVertAlignMode == IDM_TOP)
  						pDC->TextOut(x,m_TextRect.top,m_Text[i]); 
					else if (m_TextVertAlignMode == IDM_BOTTOM)
  						pDC->TextOut(x,m_TextRect.bottom-size.cx,m_Text[i]); 
					else
						pDC->TextOut(x,(m_TextRect.top + m_TextRect.bottom - size.cx) / 2,m_Text[i]); 
		 			x -= m_CharSize.cy; 
				}
				else
				{
					if (m_TextHoriAlignMode == IDM_LEFT)
  						pDC->TextOut(m_TextRect.left,y,m_Text[i]); 
					else if (m_TextHoriAlignMode == IDM_RIGHT)
  						pDC->TextOut(m_TextRect.right-size.cx,y,m_Text[i]); 
					else
						pDC->TextOut((m_TextRect.left + m_TextRect.right - size.cx) / 2,y,m_Text[i]); 
		 			y += m_CharSize.cy; 
				}
			}                                       
		}
		if (bInput && bActiveView)
		{
			bShowCaret = TRUE;
			if ((m_Row < 0) || (m_Row >= m_Text.GetSize())) SetRow(pDC,m_CaretPos.y);	
			if ((m_Col < 0) || (m_Col > m_Text[m_Row].GetLength())) SetCol(pDC,m_CaretPos.x);	
			GetCaretPos(pDC); 
			m_CaretPoint = m_CaretPos;
			CClientDC dc(pView);
			pView->OnPrepareDC(&dc,NULL);
			dc.LPtoDP(&m_CaretPoint,1);
			if (!bCaretExist) 
			{
				int x = ::GetSystemMetrics(SM_CXBORDER)*2;
				pView->CreateSolidCaret(x,m_CharSize.cy);
				bCaretExist = TRUE;
			}
			pView->SetCaretPos(m_CaretPoint);
			pView->ShowCaret();
		}
		if (bClip)
		{
			CRect rect = m_ClipRect;
			if (pDC->m_hDC != NULL) ::LPtoDP(pDC->m_hDC,(LPPOINT)(LPRECT)rect,2);
			CRgn rgn; rgn.CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);
			pDC->SelectClipRgn(&rgn);
		}
		pDC->SelectObject(pOldFont);
		m_Font.DeleteObject();
	}	
	m_TextRect = saveTextRect;
	m_Rect = saveRect;
}

int CTextObj::SetRow(CDC* pDC,int y)
{
	m_Row = (y - m_TextRect.top)  / m_CharSize.cy;
	if (m_Row < 0) m_Row = 0;
	if (m_Row > (int)m_Text.GetUpperBound()) 
		m_Row = (int)m_Text.GetUpperBound();
	return m_Row;
}

int CTextObj::SetCol(CDC* pDC,int x)
{
	CSize size;
	int x0;
	int len = m_Text[m_Row].GetLength();
	if (len < 1) m_Col = 0;
	size = pDC->GetTextExtent(m_Text[m_Row],len);
	if (m_TextHoriAlignMode == IDM_LEFT) x0 = m_TextRect.left;
	else if (m_TextHoriAlignMode == IDM_RIGHT) x0 = m_TextRect.right-size.cx;
	else x0 = m_TextRect.left + (m_TextRect.Width() - size.cx) / 2;
	int x1 = x - x0;
	if (x1 <= 0) m_Col = 0;
	else if (x1 >= size.cx) m_Col = len;
	else for (m_Col=1; m_Col<=len; m_Col++)
	{
		size = pDC->GetTextExtent(m_Text[m_Row],m_Col);
		if (x1 <= size.cx) break; 
	}	  
	m_Col++;
	if (m_Col > len) m_Col = len;
	if (m_Col < 0) m_Col = 0;
	return m_Col;
}

void CTextObj::GetCaretPos(CDC* pDC)
{
	CSize size = pDC->GetTextExtent(m_Text[m_Row],m_Text[m_Row].GetLength());
	if (m_TextHoriAlignMode == IDM_LEFT)
		m_CaretPos.x = m_TextRect.left;
	else if (m_TextHoriAlignMode == IDM_RIGHT)
		m_CaretPos.x = m_TextRect.right-size.cx;
	else m_CaretPos.x = (m_TextRect.right + m_TextRect.left - size.cx) / 2 ;
	size = pDC->GetTextExtent(m_Text[m_Row],m_Col);
	m_CaretPos.x += size.cx;
	m_CaretPos.y = m_TextRect.top + m_Row * m_CharSize.cy;
}   


void CTextObj::SetCaretPos(CView* pView,CPoint point)
{                                       
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc,NULL);
	CFont m_Font; 
	m_Font.CreateFontIndirect(&m_TextFont);
	CFont* pOldFont = dc.SelectObject(&m_Font);
	m_CharSize = dc.GetTextExtent("A",1);

	SetRow(&dc,point.y);	
	SetCol(&dc,point.x);	

	GetCaretPos(&dc);
	
	if (!bCaretExist) 
	{
		int x = ::GetSystemMetrics(SM_CXBORDER)*2;
		pView->CreateSolidCaret(x,m_CharSize.cy);
		bCaretExist = TRUE;
	}

	CPoint m_CaretPoint = m_CaretPos;
	dc.LPtoDP(&m_CaretPoint,1);
	pView->SetCaretPos(m_CaretPoint);
	dc.SelectObject(pOldFont);
}

void CTextObj::ComputerTextRect(CDC* pDC)
{
	CSize size;
	m_CharSize = pDC->GetTextExtent("A",1);
	if ((m_bRotate) && (!bInput))
	{
		int x = (int)(m_Text.GetSize() * m_CharSize.cy);
		if (m_TextHoriAlignMode == IDM_LEFT)
		{
			m_TextRect.left = m_Rect.left + 4;
			m_TextRect.right = m_TextRect.left + x;
		}
		else if (m_TextHoriAlignMode == IDM_RIGHT)
		{
			m_TextRect.right = m_Rect.right - 4;
			m_TextRect.left = m_TextRect.right - x;
		}
		else
		{
			m_TextRect.left = (m_Rect.left + m_Rect.right - x) / 2; 
			m_TextRect.right = m_TextRect.left + x;
		}

		if (m_TextVertAlignMode == IDM_TOP)
			m_TextRect.top = m_TextRect.bottom = m_Rect.top + 4;
		else if (m_TextHoriAlignMode == IDM_BOTTOM)
			m_TextRect.top = m_TextRect.bottom = m_Rect.bottom - 4;
		else
			m_TextRect.top = m_TextRect.bottom = (m_Rect.top + m_Rect.bottom) / 2; 
	
		for (int i=0; i<m_Text.GetSize(); i++)
		{
			size = pDC->GetTextExtent(m_Text[i],m_Text[i].GetLength());
			if (m_TextRect.Height() < size.cx)
			{
				if (m_TextVertAlignMode == IDM_TOP)
					m_TextRect.bottom = m_TextRect.top + size.cx;
				else if (m_TextVertAlignMode == IDM_BOTTOM)
					m_TextRect.top = m_TextRect.bottom - size.cx;
				else 
				{
					m_TextRect.top = (m_Rect.top + m_Rect.bottom - size.cx) / 2;
					m_TextRect.bottom = m_TextRect.top + size.cx;
				}
			}
		}
	}
	else
	{
		int y = (int)(m_Text.GetSize() * m_CharSize.cy);
		if (m_TextVertAlignMode == IDM_TOP)
		{
			m_TextRect.top = m_Rect.top + 4;
			m_TextRect.bottom = m_TextRect.top + y;
		}
		else if (m_TextVertAlignMode == IDM_BOTTOM)
		{
			m_TextRect.bottom = m_Rect.bottom - 4;
			m_TextRect.top = m_TextRect.bottom - y;
		}
		else
		{
			m_TextRect.top = (m_Rect.top + m_Rect.bottom - y) / 2; 
			m_TextRect.bottom = m_TextRect.top + y;
		}

		if (m_TextHoriAlignMode == IDM_LEFT)
			m_TextRect.right = m_TextRect.left = m_Rect.left + 4;
		else if (m_TextHoriAlignMode == IDM_RIGHT)
			m_TextRect.right = m_TextRect.left = m_Rect.right - 4;
		else
			m_TextRect.left = m_TextRect.right = (m_Rect.left + m_Rect.right) / 2; 
	
		for (int i=0; i<m_Text.GetSize(); i++)
		{
			size = pDC->GetTextExtent(m_Text[i],m_Text[i].GetLength());
			if (m_TextRect.Width() < size.cx)
			{
				if (m_TextHoriAlignMode == IDM_LEFT)
					m_TextRect.right = m_TextRect.left + size.cx;
				else if (m_TextHoriAlignMode == IDM_RIGHT)
					m_TextRect.left = m_TextRect.right - size.cx;
				else 
				{
					m_TextRect.left = (m_Rect.left + m_Rect.right - size.cx) / 2;
					m_TextRect.right = m_TextRect.left + size.cx;
				}
			}
		}
	}
}

void CTextObj::KillFocus(CView* pView)
{
	if (bShowCaret) pView->HideCaret();
	if (bCaretExist) ::DestroyCaret();
	bCaretExist = FALSE;
	bShowCaret = FALSE; 
}

void CTextObj::SetSelect(CView* pView,BOOL bf)
{              
	m_bSelected = bf;
	if (!bf)
	{
		if (bShowCaret) pView->HideCaret();
		if (bCaretExist) ::DestroyCaret();
		bCaretExist = FALSE;
		bShowCaret = FALSE; 
		SetInput(pView,FALSE,0);
    }
}

void CTextObj::DrawFocus(CView* pView)
{
	ASSERT(pView != NULL);
	if (pView == NULL) return;
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc,NULL);
	if ((m_TextRect.Width() > 0) && (m_TextRect.Height() > 0) && (m_SelectMode == move))
	{
		int w = m_TextRect.Width();    
		int h = m_TextRect.Height();
		if (m_TextHoriAlignMode == IDM_LEFT)
			m_TextRect.left = m_Rect.left + 4;
		else if (m_TextHoriAlignMode == IDM_RIGHT)
			m_TextRect.left = m_Rect.right - w - 4;
		else
			m_TextRect.left = (m_Rect.left + m_Rect.right - w) / 2;

		if (m_TextVertAlignMode == IDM_TOP)
			m_TextRect.top = m_Rect.top + 4;
		else if (m_TextVertAlignMode == IDM_BOTTOM)
			m_TextRect.top = m_Rect.bottom - h - 4;
		else
			m_TextRect.top = (m_Rect.top + m_Rect.bottom - h) / 2;
		m_TextRect.right = m_TextRect.left + w;
		m_TextRect.bottom = m_TextRect.top + h;
		dc.SetROP2(R2_XORPEN);
    	CPen m_Pen(PS_DOT,1,RGB(0,0,0));
	    CPen *pOldPen = dc.SelectObject(&m_Pen);
    	CBrush* pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
		CRect rect = m_TextRect;
		pDoc->LPtoPP(rect);
		dc.Rectangle(rect);
		dc.SelectObject(pOldPen);
		dc.SelectObject(pOldBrush);
	}	
}

BOOL CTextObj::OnKeyDown(CView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!bInput) return FALSE;
	switch (nChar)
	{
		case VK_LEFT: KeyLeft();
			break;
		case VK_RIGHT: KeyRight();
			break;
		case VK_UP: KeyUp();
			break;
		case VK_DOWN: KeyDown();
			break;
		case VK_DELETE: KeyDelete();
			break;
		case VK_HOME: KeyHome();
			break;
		case VK_END: KeyEnd();
			break;
	}                   
	InvalText(pView);
	return FALSE;
}

BOOL CTextObj::KeyDown()
{
	if (m_Row < m_Text.GetUpperBound()) 
	{
		m_Row ++;
		m_Col = -1;
		return TRUE;
	}	
	return FALSE;
}

BOOL CTextObj::KeyUp()
{    
	if (m_Row > 0)
	{            
		m_Row --;
		m_Col = -1;
		return TRUE;
	}   
	return FALSE;
}                     

BOOL CTextObj::KeyHome()
{
	if (m_Col != 0) 
	{
		m_Col = 0;
		return TRUE;
	}
	return FALSE;	
}

BOOL CTextObj::KeyEnd()
{
	if (m_Col != m_Text[m_Row].GetLength())
	{
		m_Col = m_Text[m_Row].GetLength();
		return TRUE;
	}
	return FALSE;
}

BOOL CTextObj::KeyLeft()
{
	if (m_Col > 0) m_Col--;
	else if (m_Row > 0)
	{
		m_Row--;
		m_Col = m_Text[m_Row].GetLength();
	} 
	else return FALSE;
	return TRUE;
}

BOOL CTextObj::KeyRight()
{
	if (m_Col < m_Text[m_Row].GetLength()) m_Col++;
	else if (m_Row < m_Text.GetUpperBound())
	{
		m_Col = 0; m_Row++;
	}
	else return FALSE;
	return TRUE;
}

BOOL CTextObj::KeyDelete()
{
	if (m_Col == m_Text[m_Row].GetLength()) 
	{
		if (m_Row < m_Text.GetUpperBound()) 
		{
			m_Text[m_Row] += m_Text[m_Row+1];
			m_Text.RemoveAt(m_Row+1);
		}	
		else return FALSE;
	}
	else 
	{
		DeleteChar(m_Row,m_Col);
		pDoc->SetModifiedFlag();
	}
		
	return TRUE;
}

void CTextObj::DeleteChar(int row,int col)
{
	int l = m_Text[m_Row].GetLength();
	m_Text[m_Row] = m_Text[m_Row].Left(col-1) + m_Text[m_Row].Right(l-col-1);
}

BOOL CTextObj::OnChar(CView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{   
	BOOL bf = FALSE;  
	if (!bInput) 
	{
		m_Row = (int)m_Text.GetSize() - 1;
		m_Col = m_Text[m_Row].GetLength();
		bf = IsTextEmpty();
	}	
	switch (nChar)
	{
	case '\b':                  
		{
			if (m_Col == 0) 
			{
				if (m_Row > 0) 
				{
					m_Col = m_Text[m_Row -1].GetLength();
					m_Text[m_Row-1] += m_Text[m_Row];
					m_Text.RemoveAt(m_Row);
					m_Row --; 
				}	
				else return FALSE;
			}
			else 
			{
				m_Col --;			
				DeleteChar(m_Row,m_Col);
				m_Col --;				
			}	
		} 
		break;	    
	case '\r':
		{
			if (IsTextEmpty()) break;
			CString m_TextLine = "";
			if (m_Col == 0) 
				m_Text.InsertAt(m_Row,m_TextLine);
			else 
			{
				if (m_Col < m_Text[m_Row].GetLength())
				{
					m_TextLine = m_Text[m_Row].Right(m_Text[m_Row].GetLength() - m_Col);
					m_Text[m_Row] = m_Text[m_Row].Left(m_Col);
				}
				if (m_Row == m_Text.GetUpperBound()) m_Text.Add(m_TextLine);
				else m_Text.InsertAt(m_Row+1,m_TextLine); 
			}	
			m_Row ++; m_Col = 0;
		}
		break;
	default:	
		if (nChar>= 32) 
		{
			int l = m_Text[m_Row].GetLength();
			char s = nChar;
			m_Text[m_Row] = m_Text[m_Row].Left(m_Col) + s + m_Text[m_Row].Right(l-m_Col);
			if (m_TextHoriAlignMode != IDM_RIGHT) m_Col ++;
		}	
		else return FALSE;
		break;
	}	
	if (!bInput) SetInput(pView,TRUE,CPoint(0,0));
	if (bf) Invalidate(pView);
	else InvalText(pView);
	return TRUE;
}       

void CTextObj::InvalText(CView* pView)
{            
	CClientDC dc(pView);              
	pView->OnPrepareDC(&dc);
	CRect rect,rect1;
	rect1 = m_TextRect;
	CFont m_Font;
	m_Font.CreateFontIndirect(&m_TextFont);
	CFont* pOldFont = dc.SelectObject(&m_Font);
	ComputerTextRect(&dc);
	dc.SelectObject(pOldFont);
	rect = rect1 | m_TextRect;
	rect.InflateRect(5,m_CharSize.cx+5);
	pDoc->LPtoPP(rect);
	dc.LPtoDP(rect);
	pView->InvalidateRect(rect,FALSE);
	pView->UpdateWindow();
}

BOOL CTextObj::OnSetCursor(CView* pView,const CPoint& point)
{
	CRect rect;               
	int nHandle = HitTest(pView,point);
	if (nHandle>=0) 
	{                                                                               
		//if ((pView->IsKindOf(RUNTIME_CLASS(CHeadPlotView))) && (!((CHeadPlotView*)pView)->m_bEditEnable))
		//	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_IBEAM));	
		//else 
		if (nHandle == 0)
		{
		    CRect rect1 = m_TextRect;
			if (!bInput && m_bClip) rect1 = m_TextRect & m_Rect;
			if ((!IsTextEmpty()) && (rect1.PtInRect(point))) 
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_IBEAM));
			else SetCursor(GetHandleCursor(nHandle));
		}
		else SetCursor(GetHandleCursor(nHandle));
		return TRUE;
	}	
	POSITION pos = m_ObjList.GetTailPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_ObjList.GetPrev(pos);
		if (pObj->OnSetCursor(pView,point)) return TRUE;
	}                  
	return FALSE;
}

void CTextObj::InvalObj(CView* pView)
{
	CClientDC dc(pView);              
	pView->OnPrepareDC(&dc);
	if (m_CharSize.cy == 0) ComputerTextRect(&dc);
	CRect rect = m_TextRect;
	if (rect.left > m_Rect.left) rect.left = m_Rect.left;
	if (rect.right < m_Rect.right) rect.right = m_Rect.right;
	if (rect.top > m_Rect.top) rect.top = m_Rect.top;
	if (rect.bottom < m_Rect.bottom) rect.bottom = m_Rect.bottom;
	rect.InflateRect(12,12);
	pDoc->LPtoPP(rect);
	dc.LPtoDP(rect);
	pView->InvalidateRect(rect,FALSE);
	pView->UpdateWindow();
}

void CTextObj::SetInput(CView* pView,BOOL bf, CPoint pt)
{
	CClientDC dc(pView); 
	CFont m_Font,*pOldFont;
	if (pView != NULL)
	{
		pView->OnPrepareDC(&dc);
		m_Font.CreateFontIndirect(&m_TextFont);
		pOldFont = dc.SelectObject(&m_Font);
	}
	CRect m_InvalRect;
	if (bf)
	{
		if (pView == NULL) return;
		if (bInput == FALSE)
		{
			bInput = TRUE;
			if (m_bRotate)
			{
				int x,y;
				m_Rect0 = m_Rect;
				x = (m_Rect0.left + m_Rect0.right) / 2;
				y = (m_Rect0.top + m_Rect0.bottom) / 2;
				m_Rect.left = x - (y - m_Rect0.top);
				m_Rect.right = m_Rect.left + m_Rect0.Height();
				m_Rect.top = y - (x-m_Rect0.left);
				m_Rect.bottom = m_Rect.top + m_Rect0.Width();
				CRect m_Rect1;
				pView->GetClientRect(m_Rect1);
				dc.DPtoLP(m_Rect1);
				pDoc->LPtoPP(m_Rect1);
				/*if (pView->IsKindOf(RUNTIME_CLASS(CHeadPlotView)))
					m_Rect1 &= pDoc->m_HeadRect;
				else*/
					m_Rect1 &= pDoc->m_Rect;
				m_Rect1.InflateRect(-3,-3);
				if (m_Rect.top < m_Rect1.top)
				{
					m_Rect.bottom = m_Rect.bottom + m_Rect1.top - m_Rect.top; 
					m_Rect.top = m_Rect1.top;
				}
				if (m_Rect.bottom > m_Rect1.bottom)
				{
					m_Rect.top = m_Rect.top + m_Rect1.bottom - m_Rect.bottom; 
					m_Rect.bottom = m_Rect1.bottom;
				}
				if (m_Rect.left < m_Rect1.left)
				{
					m_Rect.right = m_Rect.right + m_Rect1.left - m_Rect.left; 
					m_Rect.left = m_Rect1.left;
				}
				if (m_Rect.right > m_Rect1.right)
				{
					m_Rect.left = m_Rect.left + m_Rect1.right - m_Rect.right; 
					m_Rect.right = m_Rect1.right;
				}
				m_TextRect0 = m_TextRect;
				int m_TextVertAlignMode1 = m_TextVertAlignMode;
				if (m_TextHoriAlignMode == IDM_LEFT)
					m_TextVertAlignMode = IDM_BOTTOM;
				else if (m_TextHoriAlignMode == IDM_RIGHT)
					m_TextVertAlignMode = IDM_TOP;
				else m_TextVertAlignMode = IDM_CENTER;
				if (m_TextVertAlignMode1 == IDM_TOP)
					m_TextHoriAlignMode = IDM_LEFT;
				else if (m_TextVertAlignMode1 == IDM_BOTTOM)
					m_TextHoriAlignMode = IDM_RIGHT;
				else m_TextHoriAlignMode = IDM_CENTER;
				ComputerTextRect(&dc);
				m_InvalRect = m_TextRect | m_Rect | m_TextRect0 | m_Rect0;
			}
			else
			{
				ComputerTextRect(&dc);
				m_InvalRect = m_TextRect | m_Rect;
			}
			bInput = TRUE;
			m_InvalRect.InflateRect(12,12);
			pDoc->LPtoPP(m_InvalRect);
			if (pt != CPoint(0,0))
			{
				CRect saveRect = m_TextRect;
				pDoc->LPtoPP(m_TextRect);
				SetCaretPos(pView,pt);
				m_TextRect = saveRect;
			}
			dc.LPtoDP(m_InvalRect);
			pView->InvalidateRect(m_InvalRect,FALSE);
		}
		else
		{
			if (pt != CPoint(0,0))
			{
				CRect saveRect = m_TextRect;
				pDoc->LPtoPP(m_TextRect);
				SetCaretPos(pView,pt);
				m_TextRect = saveRect;
			}
		}
	}
	else if (bInput)
	{
		bInput = FALSE;
		if (pView != NULL)
		{
			CClientDC dc(pView);              
			pView->OnPrepareDC(&dc);
		}
		if (m_bRotate)
		{
			CRect m_TextRect1 = m_TextRect;
			CRect m_Rect1 = m_Rect;
			m_Rect = m_Rect0;
			int m_TextVertAlignMode1 = m_TextVertAlignMode;
			
			if (m_TextHoriAlignMode == IDM_RIGHT)
				m_TextVertAlignMode = IDM_BOTTOM;
			else if (m_TextHoriAlignMode == IDM_LEFT)
				m_TextVertAlignMode = IDM_TOP;
			else m_TextVertAlignMode = IDM_CENTER;
			
			if (m_TextVertAlignMode1 == IDM_TOP)
				m_TextHoriAlignMode = IDM_RIGHT;
			else if (m_TextVertAlignMode1 == IDM_BOTTOM)
				m_TextHoriAlignMode = IDM_LEFT;
			else m_TextHoriAlignMode = IDM_CENTER;
			
			if (pView == NULL) return;
			ComputerTextRect(&dc);
			m_InvalRect = m_TextRect | m_Rect | m_TextRect1 | m_Rect1;
		}
		else
		{
			if (pView == NULL) return;
			ComputerTextRect(&dc);
			m_InvalRect = m_TextRect | m_Rect;
		}
		m_InvalRect.InflateRect(12,12);
		pDoc->LPtoPP(m_InvalRect);
		dc.LPtoDP(m_InvalRect);
		pView->InvalidateRect(m_InvalRect,FALSE);
	}
	if (pView != NULL) dc.SelectObject(&m_Font);
}

void CTextObj::OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point)
{
	/*if ((pView->IsKindOf(RUNTIME_CLASS(CHeadPlotView))) && (!((CHeadPlotView*)pView)->m_bEditEnable))
	{
		if (m_Rect.PtInRect(point)) 
			SetInput(pView,TRUE,point);
		else SetInput(pView,FALSE,point);
		return;
	}*/	
	if (m_TextRect.PtInRect(point)) 
	{
		SetInput(pView,TRUE,point);
		return;
	}	
	if (m_bRotate && bInput) 
	{
		SetInput(pView,FALSE,point);
		return;
	}
	SetInput(pView,FALSE,point);
	if (nDragHandle == 0) 
	{
		m_SelectMode = move;
		CRect rect;
		if (pView->IsKindOf(RUNTIME_CLASS(CLogPlotView)))
			rect = pDoc->GetRect(); 
		else
			rect = pDoc->GetHeadRect();
		m_xMin = rect.left;	m_xMax = rect.right;
		m_yMin = rect.top;  m_yMax = rect.bottom;	
	}	
}


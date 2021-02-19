/*************************************************************************
*   StratumDraw.cpp - implementation of the Stratum draw object class      *           
*************************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "TrackDraw.h"
#include "FIDIO.h"
#include "StratumDraw.h"
#include "MinFontDialog.h"
#include "LineListPage.h"
#include "FillListPage.h"
#include "HandWorkPage.h"
#include "StratumDraw.h"
#include "ImageDraw.h"
#include "ModifyHandWorkDlg.h"

IMPLEMENT_SERIAL(CSinPoint, CRemDrawObj, 0)

CSinPoint::CSinPoint()
{
	m_DrawShape = sinobj;
}

CSinPoint::CSinPoint(const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDocument)
		   : CRemDrawObj(rect,pParentObj,pDocument)
{
	m_DrawShape = sinobj;
	m_Points = NULL;
	m_nPoints = 0;
	m_nAllocPoints = 0;
	m_ShapeName = "foldtool";
	m_bSave=FALSE;
}

CSinPoint::~CSinPoint()
{
}

void CSinPoint::Draw(CView* pView,CDC* pDC)
{
	pDoc->LPtoPP(m_Points,m_nPoints);
	
	Graphics *pic=NULL;
	pic=new Graphics(pDC->m_hDC);
	pic->SetSmoothingMode(SmoothingModeHighQuality);
	Color CurveColor(GetRValue(m_Color),GetGValue(m_Color),GetBValue(m_Color));
	Pen *pn= new Pen(CurveColor,(float)m_PenWidth);
	for(int i=1;i<m_nPoints;i++)
		pic->DrawLine(pn,m_Points[i-1].x,m_Points[i-1].y,m_Points[i].x,m_Points[i].y);

	pic->ReleaseHDC(pDC->m_hDC);
	delete pn;
	delete pic;
	
	pDoc->PPtoLP(m_Points,m_nPoints);
	CDrawObj::Draw(pView,pDC);
}

CPoint* CSinPoint::NewPoints(int nPoints)
{
	return (CPoint*)new BYTE[nPoints * sizeof(CPoint)];
}

BOOL CSinPoint::RecalcBounds()
{
	ASSERT_VALID(this);

	if (m_nPoints == 0)
		return FALSE;

	CRect bounds(m_Points[0], CSize(0, 0));
	for (int i = 1; i < m_nPoints; ++i)
	{
		if (m_Points[i].x < bounds.left)
			bounds.left = m_Points[i].x;
		if (m_Points[i].x > bounds.right)
			bounds.right = m_Points[i].x;
		if (m_Points[i].y < bounds.top)
			bounds.top = m_Points[i].y;
		if (m_Points[i].y > bounds.bottom)
			bounds.bottom = m_Points[i].y;
	}                       
	m_Rect = bounds;
	return TRUE;
}

HCURSOR CSinPoint::GetHandleCursor(int nHandle)
{
	ASSERT_VALID(this);
	if(nHandle == 0) 
		return AfxGetApp()->LoadCursor(IDC_SELECT);
	return CRemDrawObj::GetHandleCursor(nHandle);
}   

int CSinPoint::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CRemDrawObj::HitTest(pView,point);
	if (nHandle > 0 ) return nHandle;
	if (m_bSelected)
	{
		int nHandleCount = GetHandleCount();
		for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
		{
			CRect rc = GetHandleRect(pView,nHandle);
			if (rc.PtInRect(point)) return nHandle;
		}
	}                     
	CRect rect = m_Rect;
	if (rect.Width() <= 2) rect.InflateRect(1,0);
	if (rect.Height() <= 2) rect.InflateRect(0,1);
	if (rect.PtInRect(point))
		return 0;
	return -1;
}

void CSinPoint::MoveHandle(CView* pView,int nHandle, const CPoint& point)
{
	ASSERT_VALID(this);
	ASSERT_VALID(this);
	if ((point.x == 0) && (point.y == 0))
		return;            
	if (m_SelectMode == size)
	{	
		CRemDrawObj::MoveHandle(pView,nHandle,point);
		return;
	}	
	if (nHandle == 0)
	{
		for (int i = 0; i < m_nPoints; i += 1)
		{
			m_Points[i].x += point.x;
			m_Points[i].y += point.y;
		}
		m_Rect += point;
	}	
	else 
	{
		ASSERT(nHandle >= 0 && nHandle <= m_nPoints);
	  	CPoint point1 = m_Points[nHandle-1];
		point1.x += point.x;
		point1.y += point.y;
		CRect rect;
		rect = pDoc->GetRect(); 
		if (!rect.PtInRect(point1)) return;
	  	m_Points[nHandle-1] = point1;
		RecalcBounds();
	}  	               
}

void CSinPoint::InitStatusBar()
{
	CDrawObj::InitStatusBar();
	if (m_StatusBar.m_hWnd == NULL) return;
	if (m_SelectMode == move) return;
	CString szHT, szHT1;
	szHT.LoadString(IDS_ADDPOINT);
	szHT1.LoadString(IDS_EDITPOINT);
	if (c_ToolShape == foldtool)
	{
		m_StatusBar.SetPaneText(0,szHT);
	}
	ShowStatus();
}

void CSinPoint::ShowStatus()
{
	if (m_StatusBar.m_hWnd == NULL) return;
	if (m_SelectMode == move)
	{
		CDrawObj::ShowStatus();
		return;
	}
	
	char strX[20],strY[20];
	int nHandle;
	if (c_ToolShape == foldtool) nHandle = m_nPoints;
	else nHandle = nDragHandle;
	
	CString text;
	if (c_ToolShape == foldtool) text.LoadString(IDS_NUMBER);
	else text.LoadString(IDS_NO);
	sprintf(strX,"%3i",nHandle);
	text = text + strX;
	m_StatusBar.SetPaneText(1,text);
	
	CPoint point = GetHandlePoint(nHandle);
	LPtoHIMM(&point,1);

	sprintf(strX,"%7.2f",point.x*0.01);
	sprintf(strY,"%7.2f",point.y*0.01);
	text.LoadString(IDS_LOCATION1);
	text = text + strX + "," + strY;
	m_StatusBar.SetPaneText(2,text);
}

void CSinPoint::CreateFocusObj(CView* pView)
{
	m_pFocusObj = new CSinPoint(m_Rect,m_pParentObj,pDoc);
	m_pFocusObj->pDoc = pDoc;     
	if (m_SelectMode == move)
	{
		m_pFocusObj->m_Points = NewPoints(m_nAllocPoints);
		memcpy(m_pFocusObj->m_Points, m_Points, sizeof(CPoint) * m_nPoints);
		m_pFocusObj->m_nAllocPoints = m_nAllocPoints;
		m_pFocusObj->m_nPoints = m_nPoints;
	}
	m_pFocusObj->DrawFocus(pView);
}

void CSinPoint::SetRect(const CRect& rect)
{
	int x=0,y=0;
	CPoint point;
	point.x = rect.left - m_Rect.left;
	point.y = rect.top - m_Rect.top;
	CRect m_Rect1 = m_Rect;
	if ((rect.Height() != m_Rect.Height()) && (m_Rect.Width() != 0))
    {
	    y = 1;
		double dtor=rect.Height ()/2.;
		double pos=(rect.top+rect.bottom)/2.;
		for (int i=0; i<m_nPoints; i++)
			m_Points[i].y=(int)(pos+cos((i*5.-m_Dir)*PI/180.)*dtor);

    }                         
    else if (point.y != 0) 
    {
  	    for (int i=0; i<m_nPoints; i++)
    		m_Points[i].y += point.y;
    }
    m_Rect = m_Rect1;
    if (m_SelectMode == move)
		CRemDrawObj::SetRect(rect);
	if (y != 0) RecalcBounds();
	m_bSave=TRUE;
}

void CSinPoint::DeleteFocusObj(CView* pView)
{
	ASSERT((m_pFocusObj != NULL) && (pView != NULL));
	if (m_pFocusObj == NULL) return;
	if ((pView != NULL) && (m_pFocusObj->m_nPoints > 1)) 
		m_pFocusObj->DrawFocus(pView);
	delete m_pFocusObj;
	m_pFocusObj = NULL;
}   

void CSinPoint::DrawFocusObj(CView* pView)
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

void CSinPoint::DrawFocus(CView* pView)
{
	ASSERT((m_pFocusObj != NULL) && (pView != NULL));
	if (pView == NULL) return;
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc,NULL);
	CRect rect = m_Rect;
	pDoc->LPtoPP(rect);
	if (!dc.RectVisible(rect)) return;
	dc.SetROP2(R2_XORPEN);
	CPen Pen(PS_DOT,1,RGB(0,0,0));
	CPen *pOldPen= dc.SelectObject(&Pen);
	CBrush* pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);	
	if (m_SelectMode == size)
		dc.Rectangle(rect);
	pDoc->LPtoPP(m_Points,m_nPoints);
	dc.Polyline(m_Points, m_nPoints);
	pDoc->PPtoLP(m_Points,m_nPoints);
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
	dc.SetROP2(R2_COPYPEN);
}   

void CSinPoint::OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point)
{
    if (m_bSelected)
    {
		nDragHandle = HitTest(pView,point); 
		if (nDragHandle == 0) 
		{
			m_SelectMode = move;
			CRect rect;
			rect = pDoc->GetRect(); 
			m_xMin = rect.left;	m_xMax = rect.right;
			m_yMin = rect.top;  m_yMax = rect.bottom;	
		}	
		else if (nDragHandle > 0) m_SelectMode = size; 
		else return;
		SetCursor(GetHandleCursor(nDragHandle));
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
	if (m_bSelected) 
		m_pFocusObj->InitStatusBar();
}

void CSinPoint::OnLButtonDblClk(CView* pView, UINT nFlags, const CPoint& point)
{       
	int i;
    if(c_ToolShape == foldtool) 
	{
		ReleaseStatusBar();
		ReleaseCapture();
	    c_ToolShape = nonetool;
   		m_SelectMode = none;
		i=m_nPoints;
		if(i > 2 &&
			(m_Points[i-1] == m_Points[i-2] ||
			 m_Points[i-1].x - 1 == m_Points[i-2].x &&
			 m_Points[i-1].y == m_Points[i-2].y))
			m_nPoints -= 1;
		Invalidate(pView);
		return;	
	}
	else
	{
		CString Title[6]={"≤„¿Ì","≥‰ÃÓ∑Ï","’≈ø™∑Ï","—π¡—∑Ï","∞Î≥‰ÃÓ∑Ï","≤„º‰∑Ï"};
		COLORREF Color[6]={0x009300,0x00FF00,0xFF0000,0x800080,0xFF8000,0xFFFF00};
		CModifyHandWorkDlg Dlg;
		for(i=0;i<6;i++)
		{
			if(m_Flag==Title[i])
			{
				Dlg.m_TypeSin=i;
				break;
			}
		}
		if(Dlg.DoModal ()==IDOK)
		{
			m_Color=Color[Dlg.m_TypeSin];
			m_Flag=Title[Dlg.m_TypeSin];
			m_bSave=TRUE;
			Invalidate();
		}
	}
}

void CSinPoint::OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point)
{
    if (c_ToolShape == foldtool) return;
	if (m_bSelected)
	{
		m_pFocusObj->ReleaseStatusBar();
		CRect m_FocusRect = m_pFocusObj->m_Rect;
		if((c_last == c_down)||((nFlags & MK_CONTROL) != 0)) 
			DeleteFocusObj(pView);     
		else if (m_SelectMode == move)
		{	
			DeleteFocusObj(pView);     	
	    	SetRect(m_FocusRect);	
			if (m_pParentObj == NULL) pDoc->OnChildPosChange(this);
			else m_pParentObj->OnChildPosChange(this);
		}   
		else if (m_SelectMode == size)
		{
			DeleteFocusObj(pView);   
			SetRect(m_FocusRect);
		}
	}
	CDrawObj::OnLButtonUp(pView,nFlags,point);
}

void CSinPoint::OnMouseMove(CView* pView, UINT nFlags, const CPoint& point)
{     
	int i;
	CPoint point0;	
	if ((m_SelectMode == move)
		&&((nFlags & MK_SHIFT) != 0))//CONTROL
	{
		double dtor=m_Rect.Height ()/2.;
		double pos=(m_Rect.top+m_Rect.bottom)/2.;
		if(point.x>c_last.x)
			m_Dir+=5;
		else if(point.x<c_last.x)
			m_Dir-=5;
		if(m_Dir>360.f) m_Dir-=360.f;
		if(m_Dir<0.f) m_Dir+=360.f;
		for(i=0; i<m_nPoints; i++)
			m_Points[i].y=(int)(pos+cos((i*5.-m_Dir)*PI/180.)*dtor);

		m_pFocusObj->DrawFocus(pView);
		for (i=0; i<m_nPoints; i++)
			m_pFocusObj->m_Points[i]=m_Points[i];
		m_pFocusObj->DrawFocus(pView);
		if (m_bSelected) 
		{
			DoScroll((CScrollView*)pView);
			point0=point;
			point0.y=c_last.y;
			CDrawObj::OnMouseMove(pView,nFlags,point0);
			if ((c_ToolShape == polytool)||(c_ToolShape == foldtool))
				ShowStatus();
			else m_pFocusObj->ShowStatus();
		}
		return;
	}
	CRect rect;
	point0=point;
	point0.x=c_last.x;
	if (point0 == c_last) return;
	CPoint delta = (CPoint)(point0 - c_last);
	if (m_SelectMode == move)
	{
		CPoint point1 = point0;
		if (m_bSelected)
		{
			CRect rect = m_pFocusObj->m_Rect;
			rect.NormalizeRect();
			if (point1.x != point.x) 
				delta.x = point1.x - c_last.x;
			if (point1.y != point.y) 
				delta.y = point1.y - c_last.y;
		}
		m_pFocusObj->DrawFocus(pView);
		m_pFocusObj->MoveHandle(pView,nDragHandle, delta);
		m_pFocusObj->DrawFocus(pView);
	}	
	if (m_SelectMode == size)
	{		
		m_pFocusObj->DrawFocus(pView);
		m_pFocusObj->MoveHandle(pView,nDragHandle, delta);
		m_pFocusObj->DrawFocus(pView);
	}
	if (m_bSelected) 
	{
		DoScroll((CScrollView*)pView);
		CDrawObj::OnMouseMove(pView,nFlags,point0);
		if ((c_ToolShape == polytool)||(c_ToolShape == foldtool))
			ShowStatus();
		else m_pFocusObj->ShowStatus();
	}
}

void CSinPoint::AddPartPoint(const CPoint& point)
{
	ASSERT_VALID(this);
	if (m_nPoints == m_nAllocPoints)
	{
		CPoint* newPoints = NewPoints(m_nAllocPoints + 10);
		if (m_Points != NULL)
		{
			memcpy(newPoints, m_Points, sizeof(CPoint) * m_nAllocPoints);
			delete m_Points;
		}
		m_Points = newPoints;
		m_nAllocPoints ++;
	}

	if (m_nPoints == 0 || m_Points[m_nPoints - 1] != point)
	{
		m_Points[m_nPoints++] = point;
		RecalcBounds();
	}	
}

////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CStratumObj, CFIDDrawObj, 0)

CStratumObj::CStratumObj()
{
	m_bFlush=TRUE;
}
    
CStratumObj::CStratumObj(CString szPathName, const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDoc)
			:CFIDDrawObj(szPathName,rect,pParentObj,pDoc)
{
	m_DrawShape = stratumobj;
	m_AliasName = "Ωªª•Ω‚ Õ";
	m_ScaleFont = m_NameFont; 
	m_ScaleFont.lfHeight = -10;
	m_ScaleFont.lfWeight = 300;
	m_ScaleColor = m_NameColor;
	m_PenWidth=1;
	m_bFlush=TRUE;
}

CStratumObj::~CStratumObj()
{
}    

void CStratumObj::Serialize( CArchive& ar )
{
	long l;
	CFIDDrawObj::Serialize( ar );
	if(!ar.IsStoring() )
	{
		WORD m_DepLevel;
		WORD m_TailLength,m_CircleRadius,m_TailWidth;
		COLORREF m_CircleColor,m_TailColor;

		ar >> m_DepLevel;
		ar >> m_PenWidth;
		ar >> m_CircleRadius;
		ar >> l; m_CircleColor = (COLORREF) l;
		ar >> m_TailWidth;
		ar >> m_TailLength;
		ar >> l; m_TailColor = (COLORREF) l;
		ar >> l; m_ScaleColor = (COLORREF) l;
		ar >> m_PlotContent;
		SerializeLogFont(ar,m_ScaleFont);
		m_bFlush=TRUE;
	}	
}

void CStratumObj::SetRect(const CRect& rect)
{
	if (m_Rect == rect) return;
	CFIDDrawObj::SetRect(rect);
	m_bFlush=TRUE;
}

void CStratumObj::CreatePoint()
{
	CRect rect=m_Rect;
	pDoc->DelSinObjList();
	if(DetectFileExist(m_strPathName))
	{
		m_bFlush=FALSE;
		DWORD Pos,High;
		CString Flag;
		float Dep,Dip,Dir,Dip0,Dir0;double q;
		double tmp = YCoorToDepth (1000+m_Rect.Width())-YCoorToDepth (1000); 
		double amp=(DepthToYCoor(1000.f+tmp)-DepthToYCoor(1000.f))*0.5/PI;
		double HVr=0.5;
		CTrackObj* pTrack=(CTrackObj*)GetParentObj();
		if(pTrack!=NULL)
		{
			int HorScale=0;
			CFIDDrawObj *pObj=NULL;
			POSITION pos = pTrack->m_ObjList.GetHeadPosition();
			while(pos != NULL)
			{
				pObj = (CFIDDrawObj*)pTrack->m_ObjList.GetNext(pos);
				if(pObj!=NULL&&pObj->IsKindOf(RUNTIME_CLASS(CImageObj)))
				{
					HorScale=(int)(((CImageObj*)(pObj))->m_Bit*PI/(((CImageObj*)(pObj))->m_Rect.Width ()*0.254));
					HVr=HorScale/pDoc->m_DepthScale;
					break;
				}
			} 
		}
		amp=amp*HVr;
		TCHAR *token, *end;
		TCHAR buffer[1024];
		CStdioFile File;
		CFileException ex;
		if (!File.Open(m_strPathName, CFile::modeRead | CFile::typeText)) 
			return;
		try 
		{
			File.ReadString(buffer, 1024);	// Read Header off file	
			while (File.ReadString(buffer, 1024)) 
			{
				// Get first token
				for (token=buffer, end=buffer; 
					 *end && (*end != _T(',')) && (*end != _T('\n')); end++);
				if ((*end == _T('\0')) && (token == end)) token = NULL;
				*end = _T('\0');
				
				int i=0;CString str;
				while (token) 
				{
					str=token;
					while(str.Left(1)==" ")
						str.TrimLeft (" ");
					while(str.Right(1)==" ")
						str.TrimRight (" ");
					if(i==1)
						Dep=(float)atof(str);
					else if(i==2)
						Dip=(float)atof(str);
					else if(i==3)
						Dir=(float)atof(str);
					else if(i==4)
						Flag=str;
					else if(i==5)
						Dip0=(float)atof(str);
					else if(i==6)
						Dir0=(float)atof(str);
					// Get next token
					for (token=++end; *end && (*end != _T(',')) && (*end != _T('\n'));
						 end++);
					if ((*end == _T('\0')) && (token == end)) token = NULL;
					*end = _T('\0');
					i++;
				}
				if(Dep<m_PlotSdep)
					continue;
				if(Dep>m_PlotEdep)
					continue;
				if(m_PlotContent.Find(Flag)>-1)
				{	
					Pos= DepthToYCoor(Dep);
					q=Dip*PI/180.;
					High =(DWORD)(amp*tan(q));
					rect.top=Pos-High;
					rect.bottom=Pos+High;
					CSinPoint* pObj=new CSinPoint(rect,m_pParentObj,pDoc);
					pObj->m_Dep=Dep;
					pObj->m_Dip=Dip;
					pObj->m_Dir=Dir;
					pObj->m_Flag=Flag;
					pObj->m_Rect = rect;
					pObj->m_Dip0=Dip0;
					pObj->m_Dir0=Dir0;
					if(Flag=="≤„¿Ì")         //¬Ã…´--≤„¿Ì
						pObj->m_Color=0x009300;
					else if(Flag=="≥‰ÃÓ∑Ï")  //«‡¿∂…´--≥‰ÃÓ∑Ï
						pObj->m_Color=0x00FF00;
					else if(Flag=="’≈ø™∑Ï")  //¿∂…´--’≈ø™∑Ï
						pObj->m_Color=0xFF0000;
					else if(Flag=="∞Î≥‰ÃÓ∑Ï")//ÃÏ¿∂…´--∞Î≥‰ÃÓ∑Ï
						pObj->m_Color=0xFF8000;
					else if(Flag=="≤„º‰∑Ï")  //«‡…´--∞Î≥‰ÃÓ∑Ï
						pObj->m_Color=0xFFFF00;
					else                     //◊œ…´--—π¡—∑Ï
						pObj->m_Color=0x800080;
					pObj->m_PenWidth=m_PenWidth;
					CPoint point;
					double dtor=rect.Width ()*1./360.;
					for(int i=0;i<=360;i+=5)
					{
						point.x=(int)(rect.left +dtor*i*1.);
						if(Dip<0.001f)
							point.y=Pos;
						else
							point.y=(int)(Pos+cos((i*1.-Dir)*PI/180.)*High);
						pObj->AddPartPoint(point);
					}		
					pDoc->AddObj(pObj);
				}
			}
			File.Close();
		}
		catch (CFileException* e)
		{
			AfxMessageBox(_T("Unable to load grid data"));
			e->Delete();
		}		
	}
}

void CStratumObj::AddPoint(float Dep,float Dip,float Dir,
						   float Dip0,float Dir0,int Flag)
{
	CRect rect=m_Rect;
	DWORD Pos,High;
	Pos= DepthToYCoor(Dep);
	double q=Dip*PI/180.;
	double tmp = YCoorToDepth (1000+m_Rect.Width())-YCoorToDepth (1000); 
	double amp=(DepthToYCoor(1000.f+tmp)-DepthToYCoor(1000.f))*0.5/PI;
	double HVr=0.5;
	
	CTrackObj* pTrack=(CTrackObj*)GetParentObj();
	if(pTrack!=NULL)
	{
		int HorScale=0;
		CFIDDrawObj *pObj=NULL;
		POSITION pos = pTrack->m_ObjList.GetHeadPosition();
		while(pos != NULL)
		{
			pObj = (CFIDDrawObj*)pTrack->m_ObjList.GetNext(pos);
			if(pObj!=NULL&&pObj->IsKindOf(RUNTIME_CLASS(CImageObj)))
			{
				HorScale=(int)(((CImageObj*)(pObj))->m_Bit*PI/(((CImageObj*)(pObj))->m_Rect.Width ()*0.254));
				HVr=HorScale/pDoc->m_DepthScale;
				break;
			}
		} 
	}
	amp*=HVr;

	High =(DWORD)(amp*tan(q));
	rect.top=Pos-High;
	rect.bottom=Pos+High;
	
	CString Title[7]={"≤„¿Ì","≥‰ÃÓ∑Ï","’≈ø™∑Ï","—π¡—∑Ï","∞Î≥‰ÃÓ∑Ï","≤„º‰∑Ï","π‹¥ÆΩ”πø"};
	COLORREF Color[7]={0x009300,0x00FF00,0xFF0000,0x800080,0xFF8000,0xFFFF00,0x0101FF};
	CSinPoint* pObj=new CSinPoint(rect,m_pParentObj,pDoc);
	pObj->m_Dep=Dep;
	pObj->m_Dip=Dip;
	pObj->m_Dir=Dir;
	pObj->m_Dip0=Dip0;
	pObj->m_Dir0=Dir0;
	pObj->m_Rect = rect;
	pObj->m_Color=Color[Flag/10-1];
	pObj->m_Flag=Title[Flag/10-1];
	pObj->m_PenWidth=m_PenWidth;
	CPoint point;
	double dtor=rect.Width ()*1./360.;
	for(int i=0;i<=360;i+=5)
	{
		point.x=(int)(rect.left +dtor*i*1.);
		point.y=(int)(Pos+cos((i*1.-Dir)*PI/180.)*High);
		pObj->AddPartPoint(point);
	}	
	pDoc->AddObj(pObj);
}

void CStratumObj::DrawSin(CView* pView,CDC* pDC)
{  
	if(m_bFlush) CreatePoint();
	CSinPoint* pObj;
	POSITION pos = pDoc->m_SinObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CSinPoint*) pDoc->m_SinObjList.GetNext(pos);
		CRect rect = pObj->m_Rect;
		pDoc->LPtoPP(&rect);
		if (pDC->RectVisible(rect))
			pObj->Draw(pView,pDC);
	}
}

void CStratumObj::Draw(CView* pView,CDC* pDC)
{
	DrawHead(pDC);
	DrawClient(pDC);
	if (pDoc->m_DrawFIDHeadMode == 1)
	{
		CRect m_SaveRect = m_HeadRect;
		CPoint pt(0,0);
		pt.y = m_Rect.bottom - m_HeadRect.top + m_Rect.top - m_HeadRect.bottom;
		m_HeadRect += pt;
		DrawHead(pDC);
		m_HeadRect = m_SaveRect;
	}
	DrawFrame(pDC);
	DrawSin(pView,pDC);
}

void CStratumObj::DrawClient(CDC* pDC)
{    
	if ((m_pParentObj == NULL) || (m_ParentType != 0))
		CFIDDrawObj::DrawClient(pDC);	
}

void CStratumObj::DrawHead(CDC* pDC)
{
	CString Title[7]={"≤„¿Ì","≥‰ÃÓ∑Ï","’≈ø™∑Ï","—π¡—∑Ï","∞Î≥‰ÃÓ∑Ï","≤„º‰∑Ï","π‹¥ÆΩ”πø"};
	COLORREF Color[7]={0x009300,0x00FF00,0xFF0000,0x800080,0xFF8000,0xFFFF00,0x0101FF};
	CRect rect = m_HeadRect;
	pDoc->LPtoPP(rect);
	if (pDC->RectVisible(rect))
	{
		CFIDDrawObj::DrawHead(pDC);
		pDC->SetTextColor(m_NameColor);
		pDC->SetBkMode(TRANSPARENT); 
		pDC->SetTextAlign(TA_CENTER|TA_TOP); 
		CFont m_Font1;
		m_Font1.CreateFontIndirect(&m_NameFont);
		CFont* pOldFont = pDC->SelectObject(&m_Font1);
   		CSize size = pDC->GetTextExtent("Œƒ±æ");
		pDC->TextOut((rect.left + rect.right)/2, rect.top + size.cy -5, m_AliasName,strlen(m_AliasName));
		pDC->SelectObject(pOldFont);  
		
		CFont m_Font;
		m_Font.CreateFontIndirect(&m_ScaleFont);
	    pOldFont = pDC->SelectObject(&m_Font);
		pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
		int k=0;
		for(int j=0;j<7;j++)
		{
			if(m_PlotContent.Find(Title[j])>-1)
			{
				pDC->SetTextColor(Color[j]);  
				pDC->TextOut((rect.left + rect.right)/2, rect.bottom - 30*k-20, Title[j], strlen(Title[j]));	
				CPen pen;
				pen.CreatePen (PS_SOLID,5,Color[j]);
				CPen* Oldpen=pDC->SelectObject (&pen);
				pDC->MoveTo (rect.left+30,rect.bottom - 30*k- 15);
				pDC->LineTo (rect.right-30,rect.bottom - 30*k- 15);
				pDC->SelectObject (Oldpen);
				k++;
			}
		}
		pDC->SelectObject(pOldFont); 
	}
}                              

BOOL CStratumObj::Properties(CView* pView)
{
	CString szHT;
	CPropertySheet m_Sheet("Ωªª•Ω‚ ÕΩ·¬€");
		
	CHandWorkPage m_HandWorkPage;
	m_HandWorkPage.m_FileName =m_strPathName;
	m_HandWorkPage.m_PlotContent =m_PlotContent;
	m_Sheet.AddPage(&m_HandWorkPage);

	CLineListPage m_LinePage;
	LOGPEN m_LogPen;
	m_LogPen.lopnColor =RGB(0,0,255);
	m_LogPen.lopnWidth.x=m_PenWidth;
	m_LogPen.lopnWidth.y =m_PenWidth;
	m_LogPen.lopnStyle =BS_SOLID;
	m_LinePage.AddPen(&m_LogPen,"Sin-line");
	m_Sheet.AddPage(&m_LinePage);

	CMinFontPage m_FontPage;
	TEXTFONT m_TextFont;
	szHT.LoadString(IDS_ALIASFONT);
	m_TextFont.m_ItemName = m_AliasName;
	szHT.ReleaseBuffer();
	m_TextFont.m_Text = m_AliasName;
	m_TextFont.m_ForeGround = m_NameColor;
	m_TextFont.lf = m_NameFont;
	m_FontPage.AddFont(&m_TextFont);
	TEXTFONT m_TextFont1;
	szHT.LoadString(IDS_ZERFONT);
	m_TextFont1.m_ItemName = "Ω‚ ÕÕº¿˝";
	szHT.ReleaseBuffer();
	m_TextFont1.m_Text = "≥‰ÃÓ∑Ï";
	m_TextFont1.m_ForeGround = m_ScaleColor;
	m_TextFont1.lf = m_ScaleFont;
	m_FontPage.AddFont(&m_TextFont1);

	m_Sheet.AddPage(&m_FontPage);
	if (m_Sheet.DoModal() == IDOK)
	{
		m_PenWidth=(WORD)m_LogPen.lopnWidth .y ;
		m_FontPage.OnOK();
		m_NameColor = m_TextFont.m_ForeGround;
		m_NameFont = m_TextFont.lf;
		m_ScaleFont = m_TextFont1.lf;
		m_NameFont.lfEscapement = 0;
		m_ScaleFont.lfEscapement = 0;
		CRect SaveRect = m_Rect;
		SaveRect.top = m_HeadRect.top;
		m_HandWorkPage.OnOK();
		m_strPathName=m_HandWorkPage.m_FileName;
		m_PlotContent =m_HandWorkPage.m_PlotContent;
		m_bFlush=TRUE;
		if (m_ParentType != 0) 
			SaveRect.bottom = m_Rect.bottom;
		InvalidateRect(SaveRect);
		return TRUE;
	}
	return FALSE;
}

void CStratumObj::UpdatePoint()
{
	m_bFlush=TRUE;
	CRect SaveRect = m_Rect;
	SaveRect.top = m_HeadRect.top;
	InvalidateRect(SaveRect);
}
/******************************************************************
*   ResulDrw.cpp - implementation of the Result draw object class *           
*******************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "GeoAdo.h"
#include "GeoFIDio.h"
#include "TrackDraw.h"
#include "ResultDraw.h"

#include "MinFontDialog.h"
#include "LineListPage.h"
#include "FillListPage.h"
#include "ResultPage.h"

CResultPoint::CResultPoint(CRect rect, COLORREF ResultColor,LOGFONT ResultFont,CString nResultNo,
						   CString NoStr,int nDirect,CLogPlotDoc* pDocument)
{
	pDoc = pDocument;
	m_ResultNo = nResultNo;
	m_NoStr=NoStr;
	m_Direct = nDirect;
	m_Rect = rect;
	m_ResultColor=ResultColor;
	m_ResultFont=ResultFont;
}

void CResultPoint::SetClipRect(CDC* pDC , CRect rect)
{
	if (pDC->m_hDC != NULL) ::LPtoDP(pDC->m_hDC,(LPPOINT)(LPRECT)rect,2);
	CRgn rgn; rgn.CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);
	pDC->SelectClipRgn(&rgn);
}

void CResultPoint::Draw(CDC* pDC)
{
	CRect rect = m_Rect;		
	pDoc->LPtoPP(&rect);
	if(!pDC->RectVisible(rect)) return;
	int x,y;         	
	       
	CPen m_Pen(0,1,m_ResultColor);
	CPen* pOldPen = pDC->SelectObject(&m_Pen);
	CFont m_Font;CSize size;
	LOGFONT NoLogFont= {
		12,					        //lfHeight
		0,							//lfWidth
		0,						    //lfEscapement
		0, 							//lfOrientation
		FW_BOLD,					//lfWeight
		0,							//lfItalic
		0,							//lfUnderLine;
		0,							//lfStrikeOut;
		ANSI_CHARSET,				//lfCharSet
		OUT_TT_ONLY_PRECIS,			//lfOutPrecision
		CLIP_DEFAULT_PRECIS,		//lfClipPrecision
		DEFAULT_QUALITY,			//lfQuality
		VARIABLE_PITCH | FF_SWISS,	//lfPitchAndFamily
		"Times New Roman"			//lfFaceName
	}; 
	if(m_ResultFont.lfEscapement==2700)
		NoLogFont.lfEscapement=2700;
	m_Font.CreateFontIndirect(&NoLogFont);
	CFont* pOldFont = pDC->SelectObject(&m_Font);	
  	pDC->SetTextColor(m_ResultColor);
	pDC->SetBkMode(TRANSPARENT); 
	size = pDC->GetTextExtent(m_NoStr);
	if(m_Direct == IDM_LEFT) 
	{
		rect.left = m_Rect.left;
		if(m_ResultFont.lfEscapement==2700)
			rect.right = m_Rect.right - size.cy;
		else
			rect.right = m_Rect.right - size.cx-4;
	}	
	else 
	{	
		if(m_ResultFont.lfEscapement==2700)
			rect.left = m_Rect.left+ size.cy;
		else
			rect.left = m_Rect.left+size.cx+4;
		rect.right = m_Rect.right;
	}
	CRect FrameRect = rect;
	pDC->Rectangle(FrameRect);
	int yy=(FrameRect.top+FrameRect.bottom)/2;
	if (m_Direct == IDM_LEFT)
	{                 
		pDC->SetTextAlign(TA_LEFT| TA_TOP);
		if(m_ResultFont.lfEscapement==2700)
			pDC->TextOut(FrameRect.right+size.cy-1,yy-size.cx/2,m_NoStr, (int)strlen(m_NoStr));
		else
			pDC->TextOut(FrameRect.right+1,yy-size.cy/2,m_NoStr, (int)strlen(m_NoStr));
	}
	else
	{
		pDC->SetTextAlign(TA_RIGHT | TA_TOP);
		if(m_ResultFont.lfEscapement==2700)
			pDC->TextOut(FrameRect.left+1,yy+size.cy/2,m_NoStr, (int)strlen(m_NoStr));
		else
			pDC->TextOut(FrameRect.left-1,yy-size.cx/2, m_NoStr, (int)strlen(m_NoStr));
	}
	m_Font.CreateFontIndirect(&m_ResultFont);
	pDC->SelectObject(&m_Font);
	size = pDC->GetTextExtent("国");
	if (m_ResultFont.lfEscapement == 2700)
	{	
		CFont m_FontR;
		size.cx = m_ResultFont.lfHeight; 
		size.cy = m_ResultFont.lfWidth;
		CSize size1 = size;
		pDC->LPtoDP(&size);
		m_ResultFont.lfHeight = size.cx;
		m_ResultFont.lfWidth = size.cy;
		m_FontR.CreateFontIndirect(&m_ResultFont);
		m_ResultFont.lfHeight = size1.cx;
		m_ResultFont.lfWidth = size1.cy;
		pDC->SelectObject(&m_FontR);
	}
	size = pDC->GetTextExtent("国");
	pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
	int Remark=atoi(m_ResultNo);
	if((Remark>39)||(Remark<1))
	{
		if (m_ResultFont.lfEscapement == 2700)
		{
			CPoint pt;
			pt.x = (rect.left+rect.right) / 2;
			pt.y =  (rect.top + rect.bottom) / 2;
			pDC->LPtoDP(&pt);
			int nSaveDC = pDC->SaveDC();
			pDC->SetMapMode(MM_TEXT);
			pDC->DPtoLP(&pt);
			pDC->TextOut(pt.x-size.cy/2, pt.y,m_ResultNo);
			pDC->RestoreDC(nSaveDC);
		}
		else
		{
			CPoint pt;
			pt.x = (rect.left+rect.right) / 2;
			pt.y =  (rect.top + rect.bottom) / 2;
			int AliasLen=m_ResultNo.GetLength ();
			CString Str;
			for(int i=0;i<AliasLen;i+=2)
			{
				Str=m_ResultNo.Mid (i,2);
				pDC->TextOut(pt.x,pt.y+(i-1)*size.cy/2,Str);
			}
		}
	}
	else
	{
		CBrush c_brush;
		c_brush.CreateSolidBrush(0xFFFFFF);
		CBrush brush;
		brush.CreateSolidBrush(m_ResultColor);
		CPen Pen(0,1,m_ResultColor);
		pDC->SelectObject(&Pen);
		CRect m_ClipRect;
		pDC->GetClipBox(m_ClipRect);
		SetClipRect(pDC,rect);
		switch (Remark)
		{
			case 1: //油
			 {
				brush.CreateSolidBrush(0x0101FF);
				CBrush* pOldBrush = pDC->SelectObject(&brush);
				pDC->Rectangle(rect);
				pDC->SelectObject(pOldBrush);
			 }; break;
			case 2: //气                        
			 {
				if(pDoc->m_PlotResultModel!=3&&pDoc->m_PlotResultModel!=5)
				{
					brush.CreateSolidBrush(0x01FFFF);
					CBrush* pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(rect);
					pDC->SelectObject(pOldBrush);
				}
				x=rect.left+(rect.right-rect.left)/2;
				y=rect.top+(rect.bottom-rect.top)/2-2;
				int daltaY=(rect.bottom-rect.top)/2;
				if(daltaY>14)
				{
					POINT point2[6]={{x,y-10},
									{x-1,y-8},
									{x-2,y-6},
									{x-2,y-4},
									{x-1,y-2},
									{x+1,y}};
					pDC->Polyline(point2,6);
					pDC->Ellipse(x-6,y,x+8,y+14);
				}
				else
				{
					POINT point22[3]={{x,y-daltaY+1},
									{x-1,y-daltaY/2},
									{x,y}};
					pDC->Polyline(point22,3);
					int daltaY1=daltaY/2;
					if(daltaY1<4)	daltaY1=4;
					pDC->Ellipse(x-daltaY1,y,x+daltaY1+1,y+daltaY);
				}
			 }; break;
			case 3: //水                       
			 {	
				if(pDoc->m_PlotResultModel!=3&&pDoc->m_PlotResultModel!=5)
				{
					brush.CreateSolidBrush(0x80FF00);//0xFF8000
					CBrush* pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(rect);
					pDC->SelectObject(pOldBrush);
				}
				x=rect.left+(rect.right-rect.left)/2;
				y=rect.top+(rect.bottom-rect.top)/2;
				if (m_ResultFont.lfEscapement == 2700)
				{
					POINT point3[5]={
						{x,y-6},
						{x-2,y-3},
						{x,y},
						{x+2,y+3},
						{x,y+6}};
					pDC->Polyline(point3,5);
				}
				else
				{
					POINT point3[5]={
						{x-5,y},
						{x-3,y-2},
						{x,y},
						{x+3,y+2},
						{x+6,y}};
					pDC->Polyline(point3,5);
				}
			 }; break;
			case 4: //产层
			 {
				x=int((rect.right -rect.left )*0.5);
				y=0;
				for(;;)
				{
					if(rect.top+y*x>rect.bottom ) break;
					pDC->MoveTo(rect.left +x,rect.top +y*x);
					pDC->LineTo(rect.left ,rect.top +int((y+1.)*x));
					pDC->MoveTo(rect.left +int(0.5*x)+2,rect.top +int((y+0.5)*x));
					pDC->LineTo(rect.right ,rect.top +int((y+1.5)*x));
					y++;
				}
			 }; break; 
			case 5: //油水
			 {
				if(pDoc->m_PlotResultModel!=3&&pDoc->m_PlotResultModel!=5)
					brush.CreateSolidBrush(0x0101FF);
				CBrush* pOldBrush = pDC->SelectObject(&brush);
				POINT point[3]={{rect.left,rect.top},{rect.right,rect.top},{rect.left,rect.bottom}};
				pDC->Polygon(point,3);
				if(pDoc->m_PlotResultModel!=3&&pDoc->m_PlotResultModel!=5)
				{
					brush.CreateSolidBrush(0x80FF00);
					pDC->SelectObject(&brush);
					POINT point1[3]={{rect.left,rect.bottom-1},{rect.right-1,rect.top},{rect.right-1,rect.bottom-1}};
					pDC->Polygon(point1,3);
				}
				pDC->SelectObject(pOldBrush);
				if (m_ResultFont.lfEscapement == 2700)
				{
					x=rect.right -rect.Width ()/2+2;
					y=rect.bottom-10;
					POINT point5[5]={
						{x,y-6},
						{x-2,y-3},
						{x,y},
						{x+2,y+3},
						{x,y+6}};
					pDC->Polyline(point5,5);
				}
				else
				{
					x=rect.right -rect.Width ()/2+2;
					y=rect.bottom-8;
					POINT point5[5]={
						{x-5,y},
						{x-3,y-2},
						{x,y},
						{x+3,y+2},
						{x+5,y}};
					pDC->Polyline(point5,5);
				}
			 }; break;
			case 6: //气水
			 {
				POINT point[3]={{rect.left,rect.top},{rect.right,rect.top},{rect.left,rect.bottom}};
				if(pDoc->m_PlotResultModel!=3&&pDoc->m_PlotResultModel!=5)
				{
					brush.CreateSolidBrush(0x01FFFF);
					CBrush* pOldBrush = pDC->SelectObject(&brush);
					pDC->Polygon(point,3);
					brush.CreateSolidBrush(0x80FF00);
					pDC->SelectObject(&brush);
					POINT point1[3]={{rect.left,rect.bottom-1},{rect.right-1,rect.top},{rect.right-1,rect.bottom-1}};
					pDC->Polygon(point1,3);
					pDC->SelectObject(pOldBrush);
				}
				else
					pDC->Polygon(point,3);
				x=rect.left+(rect.right-rect.left)/2-2;
				y=rect.top+10;
				POINT point0[5]={
					{x-1,y-8},
					{x-2,y-6},
					{x-2,y-4},
					{x-1,y-2},
					{x+1,y}
				};	
				pDC->Polyline(point0,5);
				pDC->Ellipse(x-5,y,x+7,y+13);
			
				if(m_ResultFont.lfEscapement == 2700)
				{
					x=rect.right-rect.Width ()/2+2;
					y=rect.bottom-10;
					POINT point6[5]={
						{x,y-6},
						{x-2,y-3},
						{x,y},
						{x+2,y+3},
						{x,y+6}};
					pDC->Polyline(point6,5);
				}
				else
				{
					x=rect.right-rect.Width ()/2+2;
					y=rect.bottom-8;
					POINT point6[5]={
						{x-5,y},
						{x-3,y-2},
						{x,y},
						{x+3,y+2},
						{x+5,y}};
					pDC->Polyline(point6,5);
				}
			 }; break;
			case 7: //可能油气层                       
			 {
				x=rect.left+(rect.right-rect.left)/2;
				y=rect.top+(rect.bottom-rect.top)/2;
				pDC->Arc (x-5,y-10,x+6,y,x+2,y-1,x-4,y-2);
				POINT point7[4]={
					{x+2,y},
					{x,y+2},
					{x,y+5},
					{x,y+7}};
				pDC->Polyline(point7,4);
				pDC->MoveTo(x-1,y+11);pDC->LineTo (x+2,y+11);
				pDC->MoveTo(x-1,y+12);pDC->LineTo (x+2,y+12);
				pDC->MoveTo(x-1,y+13);pDC->LineTo (x+2,y+13);


				pDC->MoveTo (rect.left ,y+1);
				pDC->LineTo (x-4,y+1);
				pDC->MoveTo (x+6,y+1);
				pDC->LineTo (rect.right ,y+1);

				pDC->MoveTo (rect.left ,y+5);
				pDC->LineTo (x-3,y+5);
				pDC->MoveTo (x+4,y+5);
				pDC->LineTo (rect.right ,y+5);
			 }; break;
			case 8: //射孔标注
			{	
				int X;
				int Y=rect.top;
				int RectY=rect.Height ();
				if (m_Direct== IDM_LEFT)
				{
					X=rect.left;
					for(int i=0;i<RectY-7;i+=6)
					{
					pDC->MoveTo (X+5,Y+2+i);
					pDC->LineTo (X+2,Y+6+i);
					pDC->MoveTo (X+2,Y+5+i);
					pDC->LineTo (X+5,Y+8+i);
					pDC->MoveTo (X+2,Y+5+i);
					pDC->LineTo (rect.right -4,Y+5+i);
					}
				}
				else
				{
					X=rect.right-5;
					for(int i=0;i<RectY-7;i+=6)
					{
					pDC->MoveTo (X-5,Y+2+i);
					pDC->LineTo (X-2,Y+6+i);
					pDC->MoveTo (X-2,Y+5+i);
					pDC->LineTo (X-5,Y+8+i);
					pDC->MoveTo (X-2,Y+5+i);
					pDC->LineTo (rect.left +4,Y+5+i);
					}
				}
			};break;
			case 9: //差油层
			 {
				int x=rect.left+rect.Width ()/2-3;
				for(int m=0;m<6;m++)
				{
					pDC->MoveTo(x+m,rect.top);
					pDC->LineTo(x+m,rect.bottom);
				}
			 }; break;
			case 10://含油水层
			 {	
				POINT point1[3]={{rect.left,rect.top},{rect.right,rect.top},{rect.left,rect.top+rect.Width()}};
				CBrush* pOldBrush;
				if(pDoc->m_PlotResultModel!=3&&pDoc->m_PlotResultModel!=5)
				{
					brush.CreateSolidBrush(0x80FF00);
					pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(rect);
					pDC->SelectObject(pOldBrush);	
				}
				brush.CreateSolidBrush(0x0101FF);
				pOldBrush =pDC->SelectObject(&brush);
				pDC->Polygon(point1,3);
				pDC->SelectObject(pOldBrush);

				x=rect.left+(rect.right-rect.left)/2;
				y=rect.top+(rect.bottom-rect.top)/2;
				if (m_ResultFont.lfEscapement == 2700)
				{
					POINT point3[5]={
						{x,y-6},
						{x-2,y-3},
						{x,y},
						{x+2,y+3},
						{x,y+6}};
					pDC->Polyline(point3,5);
				}
				else
				{
					POINT point3[5]={
						{x-5,y},
						{x-3,y-2},
						{x,y},
						{x+3,y+2},
						{x+6,y}};
					pDC->Polyline(point3,5);
				}
			 }; break;
			case 11://干层
			 {
				int x=rect.left+rect.Width ()/2;
				pDC->MoveTo (x,rect.top );pDC->LineTo (x,rect.bottom );
				pDC->MoveTo (x-3,rect.top );pDC->LineTo (x-3,rect.bottom );
				pDC->MoveTo (x+3,rect.top );pDC->LineTo (x+3,rect.bottom );
			 }; break;
			case 12://差油水
			 {
				int x=rect.left+rect.Width ()/2-3;
				for(int m=0;m<6;m++)
				{
					pDC->MoveTo(x+m,rect.top);
					pDC->LineTo(x+m,rect.bottom);
				}
				if(pDoc->m_PlotResultModel!=3&&pDoc->m_PlotResultModel!=5)
					brush.CreateSolidBrush(0x80FF00);
				else
					brush.CreateSolidBrush(0xFFFFFF);
				CBrush* pOldBrush= pDC->SelectObject(&brush);
				POINT c_point[3]={{rect.left,rect.bottom-1},
				                  {rect.right-1,rect.bottom-1},
				                  {rect.right-1,rect.top+1}};
				pDC->Polygon(c_point,3);
				pDC->SelectObject(pOldBrush);
				if (m_ResultFont.lfEscapement == 2700)
				{
					x=rect.right -rect.Width ()/2+2;
					y=rect.bottom-10;
					POINT point5[5]={
						{x,y-6},
						{x-2,y-3},
						{x,y},
						{x+2,y+3},
						{x,y+6}};
					pDC->Polyline(point5,5);
				}
				else
				{
					x=rect.right -rect.Width ()/2+2;
					y=rect.bottom-8;
					POINT point5[5]={
						{x-5,y},
						{x-3,y-2},
						{x,y},
						{x+3,y+2},
						{x+5,y}};
					pDC->Polyline(point5,5);
				}
			 }; break;
			case 13://差油气
			 {
				int x=rect.left+rect.Width ()/2-3;
				for(int m=0;m<6;m++)
				{
					pDC->MoveTo(x+m,rect.top);
					pDC->LineTo(x+m,rect.bottom);
				}
				if(pDoc->m_PlotResultModel!=3&&pDoc->m_PlotResultModel!=5)
					brush.CreateSolidBrush(0x01FFFF);
				else
					brush.CreateSolidBrush(0xFFFFFF);
				CBrush* pOldBrush= pDC->SelectObject(&brush);
				POINT point1[3]={{rect.left,rect.top},
				                 {rect.right-1,rect.top},
				                 {rect.left,rect.bottom}};
				pDC->Polygon(point1,3);
				pDC->SelectObject(pOldBrush);
				x=rect.left+(rect.right-rect.left)/2-2;
				y=rect.top+10;
				POINT point0[5]={
					{x-1,y-8},
					{x-2,y-6},
					{x-2,y-4},
					{x-1,y-2},
					{x+1,y}
				};	
				pDC->Polyline(point0,5);
				pDC->Ellipse(x-5,y,x+7,y+13);
			 }; break;
			case 14://油气层
			 {
				POINT point[3]={{rect.left,rect.top},{rect.right,rect.top},{rect.left,rect.bottom}};
				if(pDoc->m_PlotResultModel!=3&&pDoc->m_PlotResultModel!=5)
					brush.CreateSolidBrush(0x01FFFF);
				else
					brush.CreateSolidBrush(0xFFFFFF);
				CBrush* pOldBrush = pDC->SelectObject(&brush);
				pDC->Polygon(point,3);
				brush.CreateSolidBrush(0x0101FF);
				pDC->SelectObject(&brush);
				POINT point1[3]={{rect.left,rect.bottom},{rect.right,rect.top},{rect.right,rect.bottom}};
				pDC->Polygon(point1,3);
				pDC->SelectObject(pOldBrush);
				x=rect.left+(rect.right-rect.left)/2-2;
				y=rect.top+10;
				POINT point0[5]={
					{x-1,y-8},
					{x-2,y-6},
					{x-2,y-4},
					{x-1,y-2},
					{x+1,y}
				};	
				pDC->Polyline(point0,5);
				pDC->Ellipse(x-5,y,x+7,y+13);
			 }; break;
			case 15://含气水层
			 {	
				POINT point1[3]={{rect.left,rect.top},{rect.right,rect.top},{rect.left,rect.top+rect.Width()}};
				if(pDoc->m_PlotResultModel!=3&&pDoc->m_PlotResultModel!=5)
				{
					brush.CreateSolidBrush(0x80FF00);
					CBrush* pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(rect);
					brush.CreateSolidBrush(0x01FFFF);
					pDC->SelectObject(&brush);
					pDC->Polygon(point1,3);
					pDC->SelectObject(pOldBrush);
				}
				else
				{
					brush.CreateSolidBrush(0xFFFFFF);
					CBrush* pOldBrush = pDC->SelectObject(&brush);
					pDC->Polygon(point1,3);
					pDC->SelectObject(pOldBrush);
				}
				x=rect.left+(rect.right-rect.left)/2;
				y=rect.top+(rect.bottom-rect.top)/2;
				if (m_ResultFont.lfEscapement == 2700)
				{
					POINT point3[5]={
						{x,y-6},
						{x-2,y-3},
						{x,y},
						{x+2,y+3},
						{x,y+6}};
					pDC->Polyline(point3,5);
				}
				else
				{
					POINT point3[5]={
						{x-5,y},
						{x-3,y-2},
						{x,y},
						{x+3,y+2},
						{x+6,y}};
					pDC->Polyline(point3,5);
				}
				y=rect.top+1;
				x=rect.left+1;
				pDC->MoveTo (x+3,y+1);pDC->LineTo (x+3,y+4);
				pDC->Ellipse(x+1,y+4,x+6,y+9);
			 }; break;			 
			case 16://含水油层
			 {
				brush.CreateSolidBrush(0x0101FF);
				CBrush* pOldBrush = pDC->SelectObject(&brush);
				pDC->Rectangle(rect);
				POINT point1[3]={{rect.left,rect.bottom},{rect.right,rect.bottom},{rect.right,rect.bottom-rect.Width()}};
				if(pDoc->m_PlotResultModel!=3&&pDoc->m_PlotResultModel!=5)
				{
					brush.CreateSolidBrush(0x80FF00);
					pDC->SelectObject(&brush);
					pDC->Polygon(point1,3);
					pDC->SelectObject(pOldBrush);
				}
				else
				{
					pDC->SelectObject(pOldBrush);
					pDC->Polygon(point1,3);
				}

				x=rect.right -6;
				y=rect.bottom-4;
				POINT point6[5]={
					{x-5,y},
					{x-3,y-2},
					{x,y},
					{x+3,y+2},
					{x+5,y}};
				pDC->Polyline(point6,5);
			 }; break;
			case 17://含水气层                       
			 {
				POINT point1[3]={{rect.left,rect.bottom},{rect.right,rect.bottom},{rect.right,rect.bottom-rect.Width()}};
				if(pDoc->m_PlotResultModel!=3&&pDoc->m_PlotResultModel!=5)
				{
					brush.CreateSolidBrush(0x01FFFF);
					CBrush* pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(rect);
					brush.CreateSolidBrush(0x80FF00);
					pDC->SelectObject(&brush);
					pDC->Polygon(point1,3);
					pDC->SelectObject(pOldBrush);
				}
				else
					pDC->Polygon(point1,3);
				x=rect.left+(rect.right-rect.left)/2;
				y=rect.top+(rect.bottom-rect.top)/2-2;
				int daltaY=(rect.bottom-rect.top)/2;
				if(daltaY>14)
				{
					POINT point2[6]={{x,y-10},
									{x-1,y-8},
									{x-2,y-6},
									{x-2,y-4},
									{x-1,y-2},
									{x+1,y}};
					pDC->Polyline(point2,6);
					pDC->Ellipse(x-6,y,x+8,y+14);
				}
				else
				{
					POINT point22[3]={{x,y-daltaY+1},
									{x-1,y-daltaY/2},
									{x,y}};
					pDC->Polyline(point22,3);
					int daltaY1=daltaY/2;
					if(daltaY1<4)	daltaY1=4;
					pDC->Ellipse(x-daltaY1,y,x+daltaY1+1,y+daltaY);
				}

				x=rect.right -6;
				y=rect.bottom-4;
				POINT point6[5]={
					{x-5,y},
					{x-3,y-2},
					{x,y},
					{x+3,y+2},
					{x+5,y}};
				pDC->Polyline(point6,5);
			 }; break;
			case 18://含气层
			 {
				x=rect.left+(rect.right-rect.left)/2;
				y=rect.top+(rect.bottom-rect.top)/2-2;
				pDC->MoveTo (x-3,rect.top );pDC->LineTo (x-3,rect.bottom );
				pDC->MoveTo (x+3,rect.top );pDC->LineTo (x+3,rect.bottom );
				
				POINT point1[3]={{rect.left,rect.top},{rect.right,rect.top},{rect.left,rect.top+rect.Width()}};
				if(pDoc->m_PlotResultModel!=3&&pDoc->m_PlotResultModel!=5)
				{
					brush.CreateSolidBrush(0x01FFFF);
					CBrush* pOldBrush = pDC->SelectObject(&brush);
					pDC->Polygon(point1,3);
					pDC->SelectObject(pOldBrush);
				}
				else
				{
					brush.CreateSolidBrush(0xFFFFFF);
					CBrush* pOldBrush = pDC->SelectObject(&brush);
					pDC->Polygon(point1,3);
					pDC->SelectObject(pOldBrush);
				}
				y=rect.top+1;
				x=rect.left+1;
				pDC->MoveTo (x+3,y+1);pDC->LineTo (x+3,y+4);
				pDC->Ellipse(x+1,y+4,x+6,y+9);		 
			}; break;
			case 19://含水层
			{
				x=rect.left+(rect.right-rect.left)/2;
				y=rect.top+(rect.bottom-rect.top)/2-2;
				pDC->MoveTo (x-3,rect.top );pDC->LineTo (x-3,rect.bottom );
				pDC->MoveTo (x+3,rect.top );pDC->LineTo (x+3,rect.bottom );
				
				POINT point1[3]={{rect.left,rect.bottom},{rect.right,rect.bottom},{rect.right,rect.bottom-rect.Width()}};
				if(pDoc->m_PlotResultModel!=3&&pDoc->m_PlotResultModel!=5)
				{
					brush.CreateSolidBrush(0x80FF00);
					CBrush *pOldBrush=pDC->SelectObject(&brush);
					pDC->Polygon(point1,3);
					pDC->SelectObject(pOldBrush);
				}
				else
					pDC->Polygon(point1,3); 
				x=rect.right -6;
				y=rect.bottom-4;
				POINT point6[5]={
					{x-5,y},
					{x-3,y-2},
					{x,y},
					{x+3,y+2},
					{x+5,y}};
				pDC->Polyline(point6,5);
			}; break;
			case 20://低含水层        长庆  
			 {	
				brush.CreateSolidBrush(0x0101FF);
				CBrush* pOldBrush = pDC->SelectObject(&brush);
				pDC->Rectangle(rect);

				POINT point1[3]={{rect.right,rect.top},{rect.left,rect.top+rect.Height()/2},{rect.right,rect.bottom}};
				brush.CreateSolidBrush(0xFFFFFF);
				pDC->SelectObject(&brush);
				pDC->Polygon(point1,3);
				pDC->SelectObject(pOldBrush);
				if (m_ResultFont.lfEscapement == 2700)
				{
					CPoint pt;
					pt.x = (rect.left+rect.right) / 2;
					pt.y =  (rect.top + rect.bottom) / 2;
					pDC->LPtoDP(&pt);
					int nSaveDC = pDC->SaveDC();
					pDC->SetMapMode(MM_TEXT);
					pDC->DPtoLP(&pt);
					pDC->TextOut(pt.x-size.cy/2+2, pt.y,"D");
					pDC->RestoreDC(nSaveDC);
				}
				else
				{
					CPoint pt;
					pt.x = (rect.left+rect.right) / 2;
					pt.y =  (rect.top + rect.bottom) / 2;
					pDC->TextOut(pt.x+2,pt.y+size.cy/2,"D");
				}
			 }; break;
			case 21://中含水层        长庆  
			 {	
				brush.CreateSolidBrush(0x0101FF);
				CBrush* pOldBrush = pDC->SelectObject(&brush);
				pDC->Rectangle(rect);

				POINT point1[3]={{rect.right,rect.top},{rect.left,rect.top+rect.Height()/2},{rect.right,rect.bottom}};
				brush.CreateSolidBrush(0xFFFFFF);
				pDC->SelectObject(&brush);
				pDC->Polygon(point1,3);
				pDC->SelectObject(pOldBrush);
				if (m_ResultFont.lfEscapement == 2700)
				{
					CPoint pt;
					pt.x = (rect.left+rect.right) / 2;
					pt.y =  (rect.top + rect.bottom) / 2;
					pDC->LPtoDP(&pt);
					int nSaveDC = pDC->SaveDC();
					pDC->SetMapMode(MM_TEXT);
					pDC->DPtoLP(&pt);
					pDC->TextOut(pt.x-size.cy/2+2, pt.y,"Z");
					pDC->RestoreDC(nSaveDC);
				}
				else
				{
					CPoint pt;
					pt.x = (rect.left+rect.right) / 2;
					pt.y =  (rect.top + rect.bottom) / 2;
					pDC->TextOut(pt.x+2,pt.y+size.cy/2,"Z");
				}
			 }; break;
			case 22://高含水层        长庆  
			 {	
				brush.CreateSolidBrush(0x0101FF);
				CBrush* pOldBrush = pDC->SelectObject(&brush);
				pDC->Rectangle(rect);

				POINT point1[3]={{rect.right,rect.top},{rect.left,rect.top+rect.Height()/2},{rect.right,rect.bottom}};
				brush.CreateSolidBrush(0xFFFFFF);
				pDC->SelectObject(&brush);
				pDC->Polygon(point1,3);
				pDC->SelectObject(pOldBrush);
				if (m_ResultFont.lfEscapement == 2700)
				{
					CPoint pt;
					pt.x = (rect.left+rect.right) / 2;
					pt.y =  (rect.top + rect.bottom) / 2;
					pDC->LPtoDP(&pt);
					int nSaveDC = pDC->SaveDC();
					pDC->SetMapMode(MM_TEXT);
					pDC->DPtoLP(&pt);
					pDC->TextOut(pt.x-size.cy/2+2, pt.y,"G");
					pDC->RestoreDC(nSaveDC);
				}
				else
				{
					CPoint pt;
					pt.x = (rect.left+rect.right) / 2;
					pt.y =  (rect.top + rect.bottom) / 2;
					pDC->TextOut(pt.x+2,pt.y+size.cy/2,"G");
				}
			 }; break;
			case 23://含油水层        长庆                                   
			 {
				x=rect.left+(rect.right-rect.left)/2;
				y=rect.top+(rect.bottom-rect.top)/2;
				POINT point3[5];

				point3[0].x=x-9;point3[0].y=y;
				point3[1].x=x-4;point3[1].y=y-2;
				point3[2].x=x;  point3[2].y=y;
				point3[3].x=x+4;point3[3].y=y+2;
				point3[4].x=x+9;point3[4].y=y;
				pDC->Polyline(point3,5);
				pDC->MoveTo(x+2,y-3);pDC->LineTo (x+5,y-3);
				pDC->MoveTo(x+2,y-4);pDC->LineTo (x+5,y-4);
				pDC->MoveTo(x+2,y-5);pDC->LineTo (x+5,y-5);
				while(y-20-rect.top>15)
				{
					y-=20;
					point3[0].x=x-9;point3[0].y=y;
					point3[1].x=x-4;point3[1].y=y-2;
					point3[2].x=x;  point3[2].y=y;
					point3[3].x=x+4;point3[3].y=y+2;
					point3[4].x=x+9;point3[4].y=y;
					pDC->Polyline(point3,5);
					pDC->MoveTo(x+2,y-3);pDC->LineTo (x+5,y-3);
					pDC->MoveTo(x+2,y-4);pDC->LineTo (x+5,y-4);
					pDC->MoveTo(x+2,y-5);pDC->LineTo (x+5,y-5);
				}
				y=rect.top+(rect.bottom-rect.top)/2;
				while(rect.bottom-y-20>15)
				{
					y+=20;
					point3[0].x=x-9;point3[0].y=y;
					point3[1].x=x-4;point3[1].y=y-2;
					point3[2].x=x;  point3[2].y=y;
					point3[3].x=x+4;point3[3].y=y+2;
					point3[4].x=x+9;point3[4].y=y;
					pDC->Polyline(point3,5);
					pDC->MoveTo(x+2,y-3);pDC->LineTo (x+5,y-3);
					pDC->MoveTo(x+2,y-4);pDC->LineTo (x+5,y-4);
					pDC->MoveTo(x+2,y-5);pDC->LineTo (x+5,y-5);
				}
			 }; break;
			case 24://含气水层        长庆
			{
				x=rect.left+(rect.right-rect.left)/2;
				y=rect.top+(rect.bottom-rect.top)/2;
				POINT point3[5];
				point3[0].x=x-9;point3[0].y=y;
				point3[1].x=x-4;point3[1].y=y-2;
				point3[2].x=x;  point3[2].y=y;
				point3[3].x=x+4;point3[3].y=y;
				point3[4].x=x+9;point3[4].y=y;
				pDC->Polyline(point3,5);
				pDC->MoveTo (x+3,y-9);pDC->LineTo (x+3,y-6);
				pDC->Ellipse(x+1,y-1,x+6,y-6);
				while(y-20-rect.top>15)
				{
					y-=20;
					point3[0].x=x-9;point3[0].y=y;
					point3[1].x=x-4;point3[1].y=y-2;
					point3[2].x=x;  point3[2].y=y;
					point3[3].x=x+4;point3[3].y=y;
					point3[4].x=x+9;point3[4].y=y;
					pDC->Polyline(point3,5);
					pDC->MoveTo (x+3,y-9);pDC->LineTo (x+3,y-6);
					pDC->Ellipse(x+1,y-1,x+6,y-6);
				}
				y=rect.top+(rect.bottom-rect.top)/2;
				while(rect.bottom-y-20>15)
				{
					y+=20;
					point3[0].x=x-9;point3[0].y=y;
					point3[1].x=x-4;point3[1].y=y-2;
					point3[2].x=x;  point3[2].y=y;
					point3[3].x=x+4;point3[3].y=y;
					point3[4].x=x+9;point3[4].y=y;
					pDC->Polyline(point3,5);
					pDC->MoveTo (x+3,y-9);pDC->LineTo (x+3,y-6);
					pDC->Ellipse(x+1,y-1,x+6,y-6);
				}
			 }; break;
			case 25://差油水          长庆
			 {
				int x=rect.left+rect.Width ()/2;
				pDC->MoveTo (x-2,rect.top );pDC->LineTo (x-2,rect.bottom );
				pDC->MoveTo (x-3,rect.top );pDC->LineTo (x-3,rect.bottom );
				pDC->MoveTo (x-4,rect.top );pDC->LineTo (x-4,rect.bottom );
				pDC->MoveTo (x+2,rect.top );pDC->LineTo (x+2,rect.bottom );
				pDC->MoveTo (x+3,rect.top );pDC->LineTo (x+3,rect.bottom );
				pDC->MoveTo (x+4,rect.top );pDC->LineTo (x+4,rect.bottom );
				
				POINT c_point[3]={{rect.left,rect.bottom-1},
				                  {rect.right-1,rect.bottom-1},
				                  {rect.right-1,rect.top+1}};
				CBrush* pOldBrush =pDC->SelectObject (&c_brush);
				pDC->Polygon(c_point,3);
				pDC->SelectObject(pOldBrush);
				x=rect.right -rect.Width ()/2+2;
				y=rect.bottom-8;
				POINT point5[5]={
					{x-5,y},
					{x-3,y-2},
					{x,y},
					{x+3,y+2},
					{x+5,y}};
				pDC->Polyline(point5,5);
			 }; break;
			case 26://差油气          长庆
			 {
				int x=rect.left+rect.Width ()/2;
				pDC->MoveTo (x-2,rect.top );pDC->LineTo (x-2,rect.bottom );
				pDC->MoveTo (x-3,rect.top );pDC->LineTo (x-3,rect.bottom );
				pDC->MoveTo (x-4,rect.top );pDC->LineTo (x-4,rect.bottom );
				pDC->MoveTo (x+2,rect.top );pDC->LineTo (x+2,rect.bottom );
				pDC->MoveTo (x+3,rect.top );pDC->LineTo (x+3,rect.bottom );
				pDC->MoveTo (x+4,rect.top );pDC->LineTo (x+4,rect.bottom );

				POINT point1[3]={{rect.left,rect.top},
				                 {rect.right-1,rect.top},
				                 {rect.left,rect.bottom}};
				CBrush* pOldBrush =pDC->SelectObject (&c_brush);
				pDC->Polygon(point1,3);
				pDC->SelectObject(pOldBrush);
				x=rect.left+(rect.right-rect.left)/2-2;
				y=rect.top+10;
				POINT point0[5]={
					{x-1,y-8},
					{x-2,y-6},
					{x-2,y-4},
					{x-1,y-2},
					{x+1,y}
				};	
				pDC->Polyline(point0,5);
				pDC->Ellipse(x-5,y,x+7,y+13);
			 }; break;
			case 27://气             长庆                        
			 {
				x=rect.left+(rect.right-rect.left)/2;
				y=rect.top+(rect.bottom-rect.top)/2-2;
				int daltaY=(rect.bottom-rect.top)/2;
				if(daltaY>24)
				{
					POINT point2[6];
					point2[0].x=x;  point2[0].y=y-10;
					point2[1].x=x-1;point2[1].y=y-8;
					point2[2].x=x-2;point2[2].y=y-6;
					point2[3].x=x-2;point2[3].y=y-4;
					point2[4].x=x-1;point2[4].y=y-2;
					point2[5].x=x+1;point2[5].y=y;
					pDC->Polyline(point2,6);
					pDC->Ellipse(x-6,y,x+8,y+14);
					while(y-40-rect.top>20)
					{
						y-=40;
						point2[0].x=x;  point2[0].y=y-10;
						point2[1].x=x-1;point2[1].y=y-8;
						point2[2].x=x-2;point2[2].y=y-6;
						point2[3].x=x-2;point2[3].y=y-4;
						point2[4].x=x-1;point2[4].y=y-2;
						point2[5].x=x+1;point2[5].y=y;
						pDC->Polyline(point2,6);
						pDC->Ellipse(x-6,y,x+8,y+14);
					}
					y=rect.top+(rect.bottom-rect.top)/2;
					while(rect.bottom-y-40>30)
					{
						y+=40;
						point2[0].x=x;  point2[0].y=y-10;
						point2[1].x=x-1;point2[1].y=y-8;
						point2[2].x=x-2;point2[2].y=y-6;
						point2[3].x=x-2;point2[3].y=y-4;
						point2[4].x=x-1;point2[4].y=y-2;
						point2[5].x=x+1;point2[5].y=y;
						pDC->Polyline(point2,6);
						pDC->Ellipse(x-6,y,x+8,y+14);
					}
				}
				else
				{
					POINT point22[3]={{x,y-daltaY+1},
									{x-1,y-daltaY/2},
									{x,y}};
					pDC->Polyline(point22,3);
					int daltaY1=daltaY/2;
					if(daltaY1<4)	daltaY1=4;
					pDC->Ellipse(x-daltaY1,y,x+daltaY1+1,y+daltaY);
				}
			 }; break;
			case 28: //水             长庆                       
			 {	
				x=rect.left+(rect.right-rect.left)/2;
				y=rect.top+(rect.bottom-rect.top)/2;
				POINT point3[5];
				point3[0].x=x-5;point3[0].y=y;
				point3[1].x=x-3;point3[1].y=y-2;
				point3[2].x=x;  point3[2].y=y;
				point3[3].x=x+3;point3[3].y=y+2;
				point3[4].x=x+6;point3[4].y=y;
				pDC->Polyline(point3,5);				
				while(y-20-rect.top>15)
				{
					y-=20;
					point3[0].x=x-5;point3[0].y=y;
					point3[1].x=x-3;point3[1].y=y-2;
					point3[2].x=x;  point3[2].y=y;
					point3[3].x=x+3;point3[3].y=y+2;
					point3[4].x=x+6;point3[4].y=y;
					pDC->Polyline(point3,5);
				}
				y=rect.top+(rect.bottom-rect.top)/2;
				while(rect.bottom-y-20>15)
				{
					y+=20;
					point3[0].x=x-5;point3[0].y=y;
					point3[1].x=x-3;point3[1].y=y-2;
					point3[2].x=x;  point3[2].y=y;
					point3[3].x=x+3;point3[3].y=y+2;
					point3[4].x=x+6;point3[4].y=y;
					pDC->Polyline(point3,5);
				}
			 }; break;
			case 29://油水           长庆  
			 {
				CBrush* pOldBrush = pDC->SelectObject(&brush);
				POINT point[3]={{rect.left,rect.top},{rect.right,rect.top},{rect.left,rect.bottom}};
				pDC->Polygon(point,3);
				pDC->SelectObject(pOldBrush);
				x=rect.right -rect.Width ()/2+2;
				y=rect.bottom-8;
				POINT point5[5]={
					{x-5,y},
					{x-3,y-2},
					{x,y},
					{x+3,y+2},
					{x+5,y}};
				pDC->Polyline(point5,5);
			 }; break;
			case 30://气水            长庆  
			 {
				POINT point[3]={{rect.left,rect.top},{rect.right,rect.top},{rect.left,rect.bottom}};
				pDC->Polygon(point,3);
				x=rect.left+(rect.right-rect.left)/2-1;
				y=rect.top+10;
				POINT point0[5]={
					{x-1,y-8},
					{x-2,y-6},
					{x-2,y-4},
					{x-1,y-2},
					{x+1,y}
				};	
				pDC->Polyline(point0,5);
				pDC->Ellipse(x-5,y,x+7,y+13);
				
				x=rect.right -rect.Width ()/2+2;
				y=rect.bottom-8;
				POINT point6[5]={
					{x-5,y},
					{x-3,y-2},
					{x,y},
					{x+3,y+2},
					{x+5,y}};
				pDC->Polyline(point6,5);
			 }; break;
			 case 31://可能油气层     长庆                   
			 {
				x=rect.left+(rect.right-rect.left)/2;
				y=rect.top+(rect.bottom-rect.top)/2;
				pDC->Arc (x-5,y-10,x+6,y,x+2,y-1,x-4,y-2);
				POINT point7[4]={
					{x+2,y},
					{x,y+2},
					{x,y+5},
					{x,y+7}};
				pDC->Polyline(point7,4);
				pDC->MoveTo(x-1,y+11);pDC->LineTo (x+2,y+11);
				pDC->MoveTo(x-1,y+12);pDC->LineTo (x+2,y+12);
				pDC->MoveTo(x-1,y+13);pDC->LineTo (x+2,y+13);

				pDC->MoveTo (x-3,rect.top);
				pDC->LineTo (x-3,y-13);
				pDC->MoveTo (x+3,rect.top);
				pDC->LineTo (x+3,y-13);
				
				pDC->MoveTo (x-3,rect.bottom);
				pDC->LineTo (x-3,y+16);
				pDC->MoveTo (x+3,rect.bottom);
				pDC->LineTo (x+3,y+16);
			 }; break;
			 case 32://油气层         长庆  
			 {
				POINT point[3]={{rect.left,rect.top},
				                {rect.right,rect.top},
				                {rect.left,rect.bottom}};
				brush.CreateSolidBrush(0xFFFFFF);
				CBrush* pOldBrush = pDC->SelectObject(&brush);
				pDC->Polygon(point,3);
				brush.CreateSolidBrush(0x0101FF);
				pDC->SelectObject(&brush);
				POINT point1[3]={{rect.left,rect.bottom},
				                 {rect.right,rect.top},
				                 {rect.right,rect.bottom}};
				pDC->Polygon(point1,3);
				pDC->SelectObject(pOldBrush);
				x=rect.left+(rect.right-rect.left)/2-2;
				y=rect.top+10;
				POINT point0[5]={
					{x-1,y-8},
					{x-2,y-6},
					{x-2,y-4},
					{x-1,y-2},
					{x+1,y}
				};	
				pDC->Polyline(point0,5);
				pDC->Ellipse(x-5,y,x+7,y+13);
			 }; break;
			case 33: //差油层         长庆
			 {
				int x=rect.left+rect.Width ()/2;
				pDC->MoveTo (x-2,rect.top );pDC->LineTo (x-2,rect.bottom );
				pDC->MoveTo (x-3,rect.top );pDC->LineTo (x-3,rect.bottom );
				pDC->MoveTo (x-4,rect.top );pDC->LineTo (x-4,rect.bottom );
				pDC->MoveTo (x+2,rect.top );pDC->LineTo (x+2,rect.bottom );
				pDC->MoveTo (x+3,rect.top );pDC->LineTo (x+3,rect.bottom );
				pDC->MoveTo (x+4,rect.top );pDC->LineTo (x+4,rect.bottom );
			 }; break;
			 case 34://差油水         长庆
			 {
			    int x=rect.left+rect.Width ()/2;
				pDC->MoveTo (x-2,rect.top );pDC->LineTo (x-2,rect.bottom );
				pDC->MoveTo (x-3,rect.top );pDC->LineTo (x-3,rect.bottom );
				pDC->MoveTo (x-4,rect.top );pDC->LineTo (x-4,rect.bottom );
				pDC->MoveTo (x+2,rect.top );pDC->LineTo (x+2,rect.bottom );
				pDC->MoveTo (x+3,rect.top );pDC->LineTo (x+3,rect.bottom );
				pDC->MoveTo (x+4,rect.top );pDC->LineTo (x+4,rect.bottom );

				brush.CreateSolidBrush(0xFFFFFF);
				CBrush* pOldBrush= pDC->SelectObject(&brush);
				POINT c_point[3]={{rect.left,rect.bottom-1},
				                  {rect.right-1,rect.bottom-1},
				                  {rect.right-1,rect.top+1}};
				pDC->Polygon(c_point,3);
				pDC->SelectObject(pOldBrush);
				if (m_ResultFont.lfEscapement == 2700)
				{
					x=rect.right -rect.Width ()/2+2;
					y=rect.bottom-10;
					POINT point5[5]={
						{x,y-6},
						{x-2,y-3},
						{x,y},
						{x+2,y+3},
						{x,y+6}};
					pDC->Polyline(point5,5);
				}
				else
				{
					x=rect.right -rect.Width ()/2+2;
					y=rect.bottom-8;
					POINT point5[5]={
						{x-5,y},
						{x-3,y-2},
						{x,y},
						{x+3,y+2},
						{x+5,y}};
					pDC->Polyline(point5,5);
				}
			 }; break;
			case 35://差油气         长庆
			{
				int x=rect.left+rect.Width ()/2;
				pDC->MoveTo (x-2,rect.top );pDC->LineTo (x-2,rect.bottom );
				pDC->MoveTo (x-3,rect.top );pDC->LineTo (x-3,rect.bottom );
				pDC->MoveTo (x-4,rect.top );pDC->LineTo (x-4,rect.bottom );
				pDC->MoveTo (x+2,rect.top );pDC->LineTo (x+2,rect.bottom );
				pDC->MoveTo (x+3,rect.top );pDC->LineTo (x+3,rect.bottom );
				pDC->MoveTo (x+4,rect.top );pDC->LineTo (x+4,rect.bottom );

				brush.CreateSolidBrush(0xFFFFFF);
				CBrush* pOldBrush= pDC->SelectObject(&brush);
				POINT point1[3]={{rect.left,rect.top},
									{rect.right-1,rect.top},
									{rect.left,rect.bottom}};
				pDC->Polygon(point1,3);
				pDC->SelectObject(pOldBrush);
				x=rect.left+(rect.right-rect.left)/2-2;
				y=rect.top+10;
				POINT point0[5]={
					{x-1,y-8},
					{x-2,y-6},
					{x-2,y-4},
					{x-1,y-2},
					{x+1,y}
				};	
				pDC->Polyline(point0,5);
				pDC->Ellipse(x-5,y,x+7,y+13);
			}; break;
			case 36://裂缝型储层
			{
				if(pDoc->m_PlotResultModel==3||pDoc->m_PlotResultModel==6)
					LithFillBox(rect,46,m_ResultColor,rect.TopLeft(),pDC);
				else
					LithFillBox(rect,46,m_ResultColor,rect.TopLeft(),pDC,0xD3D3D3);
				CBrush* pOldBrush =(CBrush*)pDC->SelectStockObject(NULL_BRUSH);
				pDC->Rectangle(rect);
				pDC->SelectObject(pOldBrush);
			};break;
			case 37://溶孔型储层
			{
				if(pDoc->m_PlotResultModel==3||pDoc->m_PlotResultModel==6)
					LithFillBox(rect,95,m_ResultColor,rect.TopLeft(),pDC);
				else
					LithFillBox(rect,95,m_ResultColor,rect.TopLeft(),pDC,0xD3D3D3);
				CBrush* pOldBrush =(CBrush*)pDC->SelectStockObject(NULL_BRUSH);
				pDC->Rectangle(rect);
				pDC->SelectObject(pOldBrush);
			};break;
			case 38://差气层
			{
				x=rect.left+(rect.right-rect.left)/2;
				y=rect.top+(rect.bottom-rect.top)/2;
				if(pDoc->m_PlotResultModel!=3&&pDoc->m_PlotResultModel!=5)
					brush.CreateSolidBrush(0x01FFFF);
				else
					brush.CreateSolidBrush(0xFFFFFF);
				CBrush* pOldBrush =(CBrush*)pDC->SelectObject(&brush);
				pDC->Rectangle(x-4,rect.top,x+4,rect.bottom);
				pDC->SelectObject(pOldBrush);
				pDC->MoveTo (x,y-4);pDC->LineTo (x,y);
				pDC->Ellipse(x-3,y,x+3,y+6);		 
			};break;
			case 39://裂缝溶孔型储层
			{
				if(pDoc->m_PlotResultModel==3||pDoc->m_PlotResultModel==6)
					LithFillBox(rect,99,m_ResultColor,rect.TopLeft(),pDC);
				else
					LithFillBox(rect,99,m_ResultColor,rect.TopLeft(),pDC,0xD3D3D3);
				CBrush* pOldBrush =(CBrush*)pDC->SelectStockObject(NULL_BRUSH);
				pDC->Rectangle(rect);
				pDC->SelectObject(pOldBrush);
			};break;
		default:break;
		}
		SetClipRect(pDC,m_ClipRect);
	}
	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldPen);
}

IMPLEMENT_SERIAL(CResultObj, CFIDDrawObj, 0)

CResultObj::CResultObj()
{
	m_DrawShape = resultobj;
	bRButtonDown = FALSE;
	bFlush = TRUE;
}

CResultObj::CResultObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDoc)
		:CFIDDrawObj(strPathName,rect,pParentObj,pDoc)
{
	bRButtonDown = FALSE;
	m_HeadRect = rect;
	m_HeadRect.bottom = m_HeadRect.top + 100;
	m_HeadRect.right = m_HeadRect.left + 40;
	m_ResultLR= IDM_LEFT;
	m_Rect = m_HeadRect;
	m_Rect.top = m_HeadRect.bottom;
	m_DrawDirect = VERDIR;
	m_DrawShape = resultobj;
    m_NameFont.lfHeight = -20;
    m_ResultOrderFont.lfHeight = -14;
	strcpy(m_ResultOrderFont.lfFaceName,"宋体"); 
	m_ResultOrderFont=m_NameFont;
	
	m_ResultOrderColor = RGB(0,0,0);
	m_AliasName = "解释结论";
	bFlush = TRUE;
	m_FramePen.lopnWidth = CPoint(2,2); 
	m_FramePen.lopnColor = RGB(0,0x80,0); 
	m_FramePen.lopnStyle = 0;
}

CResultObj::~CResultObj()
{
	POSITION pos = m_ResultPoint.GetHeadPosition();
	while (pos != NULL) delete m_ResultPoint.GetNext(pos);
	m_ResultPoint.RemoveAll();
}    

void CResultObj::CreatePoint()
{
	CRect rect;
	rect.left = m_Rect.left;
	rect.right = m_Rect.right;

	POSITION pos = m_ResultPoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_ResultPoint.GetNext(pos);
	m_ResultPoint.RemoveAll();
	
	CGeoAdo	ado;
	if(!ado.InitData(pDoc->m_szSever)) return;

	CString WellName=m_strPathName.Mid(m_strPathName.Find("\\")+1);
	if(WellName.Find("\\")>0)
		WellName=WellName.Left(WellName.Find("\\"));

	_RecordsetPtr pRS = ado.GetRS("SELECT * FROM Storage WHERE StorageName='" + WellName + "'");
	if(pRS->adoEOF)
	{
		bFlush = FALSE;
		ado.Disconnect();
		return;
	}
	else
	{
		DWORD WellID=atol((LPCTSTR)(_bstr_t)pRS->GetCollect("StorageID"));
		if(WellID>0)
		{
			int Remark;
			CString str,NoStr,Resultstr;
			double DepSt,DepEn;
			CString cID;
			cID.Format("%lu",WellID);
			pRS = ado.GetRS("SELECT * FROM Result WHERE UpperID=" + cID);
			while(pRS->adoEOF==0)
			{					
				DepSt=atof((LPCTSTR)(_bstr_t)pRS->GetCollect("DepSt"));
				DepEn=atof((LPCTSTR)(_bstr_t)pRS->GetCollect("DepEn"));
				if(DepSt>=m_PlotEdep)
				{
					pRS->MoveNext();
					continue;
				}
				if(DepEn<m_PlotSdep)
				{
					pRS->MoveNext();
					continue;
				}
				if(DepSt<m_PlotSdep) 
					DepSt=m_PlotSdep;
				if(DepEn>m_PlotEdep) 
					DepEn=m_PlotEdep;

				Remark=-1;
				NoStr=(LPCTSTR)(_bstr_t)pRS->GetCollect("Number");
				str=(LPCTSTR)(_bstr_t)pRS->GetCollect("Conclusion");
				str.Trim();
				
				if(str=="油层") Remark=1;
				else if(str=="气层") Remark=2;
				else if(str=="水层") Remark=3;
				else if(str=="产层"
					||str=="渗透层") Remark=4;
				else if(str=="油水层") Remark=5;
				else if(str=="气水层") Remark=6;
				else if((str=="可能性油气层")
					||(str=="可能油气层")
					||(str=="可能油层")
					||(str=="可能气层")
					||(str=="可疑气层")
					||(str=="可疑油层")
					||(str=="可疑油气层")) Remark=7;
				else if(str=="射孔") Remark=8;
				else if(str=="差油层") Remark=9;
				else if(str=="干层") Remark=11;						
				else if(str=="油气层") Remark=14;
				else if(pDoc->m_PlotResultModel==2||pDoc->m_PlotResultModel==3)
				{
					if(str=="差油水层") Remark=12;
					if(str=="差油气层") Remark=13;
					if(str=="含油水层") Remark=10;
					if(str=="含气水层") Remark=15;
					if(str=="含水油层") Remark=16;
					if(str=="含水气层") Remark=17;
					if(str=="含气层") Remark=18;
					if(str=="含水层") Remark=19;
					if(str=="裂缝型储层") Remark=36;
					if(str=="溶孔型储层") Remark=37;
					if(str=="差气层") Remark=38;
					if(str=="裂缝溶孔型储层") Remark=39;
				}
				if(pDoc->m_PlotResultModel==6)   //长庆解释符号
				{							
					if(str=="低含水层") Remark=20;
					if(str=="中含水层") Remark=21;
					if(str=="高含水层") Remark=22;
					if(str=="含油水层") Remark=23;
					if(str=="含气水层") Remark=24;
					if(str=="差油层") Remark=33;
					if(str=="差油水层") Remark=34;
					if(str=="差油气层") Remark=35;
					if(str=="气层") Remark=27;
					if(str=="水层") Remark=28;
					if(str=="油水层") Remark=29;
					if(str=="气水层") Remark=30;
					if((str=="可能性油气层")
					||(str=="可能油气层")
					||(str=="可能油层")
					||(str=="可能气层")
					||(str=="可疑气层")
					||(str=="可疑油层")
					||(str=="可疑油气层")) Remark=31;						
					if(str=="油气层") Remark=32;
					if(str=="裂缝型储层") Remark=36;
					if(str=="溶孔型储层") Remark=37;
				}
				Resultstr=str;
				
				rect.top = DepthToYCoor(DepSt);
				rect.bottom = DepthToYCoor(DepEn);
				if(Remark==-1)
					str=Resultstr;
				else
					str.Format("%d",Remark);
				CResultPoint* pObj=NULL;
				pObj = new CResultPoint(rect,m_ResultOrderColor,m_ResultOrderFont,str,NoStr,m_ResultLR,pDoc);
				m_ResultPoint.AddTail(pObj);
				pRS->MoveNext();
			}
		}
	}
	bFlush = FALSE;
	ado.Disconnect();
}

void CResultObj::Serialize(CArchive& ar)
{                      
	WORD w;
	CFIDDrawObj::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << (WORD) m_ResultLR;
		ar << (LONG) m_ResultOrderColor;
		SerializeLogFont(ar,m_ResultOrderFont);
	}
	else
	{
		long l;
		ar >> w; m_ResultLR = (int) w;
		ar >> l; m_ResultOrderColor = (COLORREF) l;
		SerializeLogFont(ar,m_ResultOrderFont);
		bFlush = TRUE;
	}	
}

void CResultObj::SetRect(const CRect& rect)
{
	CPoint delta;    
	CResultPoint* pObj;
	CRect rect1;
	delta.x = rect.right - m_Rect.right;
	delta.y = rect.top - m_Rect.top;
	int cx =20;
	POSITION pos = m_ResultPoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CResultPoint*) m_ResultPoint.GetNext(pos);
		rect1 = pObj->GetRect() + delta;
		rect1.left = rect.left;
		rect1.right = rect.right;
		pObj->SetRect(rect1);
	}          
	CFIDDrawObj::SetRect(rect);
}       

BOOL CResultObj::Properties(CView* pView)
{
	CString szHT;
	CPropertySheet m_Sheet("储层解释");
	CResultPage m_ResultPage;
	m_ResultPage.m_ResultLR = m_ResultLR;
	m_ResultPage.m_FileName=m_strPathName;
	m_ResultPage.m_szSever=pDoc->m_szSever;
	m_Sheet.AddPage(&m_ResultPage);
	CLineListPage m_LineList;
	CFillListPage m_FillList;
 	if((m_ParentType != 0) || (m_DrawDirect == VERDIR))
	{
		szHT.LoadString(IDS_FRAMELINE);
		m_LineList.AddPen(&m_FramePen,szHT);
		szHT.LoadString(IDS_FRAMEFILL);
		m_FillList.AddFill(&m_Brush,szHT);
		m_Sheet.AddPage(&m_LineList);
		m_Sheet.AddPage(&m_FillList);
	}
	CMinFontPage m_FontPage;
	TEXTFONT m_TextFont[2];
	if(m_AliasName=="") m_AliasName="标题";
	m_TextFont[0].m_ItemName = m_AliasName;
	m_TextFont[0].m_Text = m_AliasName;
	m_TextFont[0].m_ForeGround = m_NameColor;
	m_TextFont[0].m_BackGround = RGB(255,255,255);
	m_TextFont[0].lf = m_NameFont;
	m_FontPage.AddFont(&m_TextFont[0]);
	szHT.LoadString(IDS_RESULTFONT);
	m_TextFont[1].m_ItemName = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	m_TextFont[1].m_Text = "12345";
	m_TextFont[1].m_ForeGround = m_ResultOrderColor;
	m_TextFont[1].m_BackGround = RGB(255,255,255);
	m_TextFont[1].lf = m_ResultOrderFont;
	m_FontPage.AddFont(&m_TextFont[1]);
	m_Sheet.AddPage(&m_FontPage);
	double m_PlotSdep1 = m_PlotSdep;
	double m_PlotEdep1 = m_PlotEdep;
	if (m_Sheet.DoModal() == IDOK)
	{
		m_FontPage.OnOK();
		m_AliasName=m_TextFont[0].m_Text ;
		m_NameColor = m_TextFont[0].m_ForeGround;
		m_NameFont = m_TextFont[0].lf;
		m_ResultOrderFont = m_TextFont[1].lf;
		m_ResultOrderColor = m_TextFont[1].m_ForeGround;
		if (m_ResultLR != m_ResultPage.m_ResultLR)
			m_ResultLR = m_ResultPage.m_ResultLR;
		if (m_strPathName != m_ResultPage.m_FileName)
			m_strPathName = m_ResultPage.m_FileName;
		CRect SaveRect = m_Rect;
		SaveRect.top = m_HeadRect.top;
		bFlush=TRUE;
		if (m_ParentType != 0) 
			SaveRect.bottom = m_Rect.bottom;
		InvalidateRect(SaveRect);
		pDoc->SetModifiedFlag();
		return TRUE;
	}
	return FALSE;
}                

void CResultObj::DrawHead(CDC* pDC)
{
	CRect rect = m_HeadRect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;

	CFIDDrawObj::DrawHead(pDC);   
	CFont m_Font;
	if (m_NameFont.lfEscapement == 2700)
	{
		CSize size;
		size.cx = m_NameFont.lfHeight; size.cy = m_NameFont.lfWidth;
		CSize size1 = size;
		pDC->LPtoDP(&size);
		m_NameFont.lfHeight = size.cx;
		m_NameFont.lfWidth = size.cy;
		m_Font.CreateFontIndirect(&m_NameFont);
		m_NameFont.lfHeight = size1.cx;
		m_NameFont.lfWidth = size1.cy;
	}
	else m_Font.CreateFontIndirect(&m_NameFont);
	
	CFont* pOldFont = pDC->SelectObject(&m_Font);
	CSize size = pDC->GetTextExtent("文本");
   	pDC->SetTextColor(m_NameColor);  
	pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
	pDC->SetBkMode(TRANSPARENT); 
	CPoint pt;
	if (m_NameFont.lfEscapement == 2700)
	{		
		pt.x = (rect.left+rect.right) / 2;
		pt.y =  (rect.top + rect.bottom) / 2;
		pDC->LPtoDP(&pt);
		int nSaveDC = pDC->SaveDC();
		pDC->SetMapMode(MM_TEXT);
		pDC->DPtoLP(&pt);
		pDC->TextOut(pt.x-size.cy/2, pt.y,m_AliasName);
		pDC->RestoreDC(nSaveDC);
	}
	else
	{
		int AliasLen=m_AliasName.GetLength ();
		pt.x = (rect.left+rect.right)/2;
		pt.y =  (rect.top + rect.bottom-size.cy*AliasLen/2)/2+size.cy;
		CString Str;
		for(int i=0;i<AliasLen;i+=2)
		{
			Str=m_AliasName.Mid(i,2);
			pDC->TextOut(pt.x,pt.y+i*size.cy/2,Str);
		}
	}
	pDC->SelectObject(pOldFont);
}   

void CResultObj::DrawClient(CDC* pDC)
{
	CRect rect = m_Rect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;
	if(bFlush) CreatePoint();
	CResultPoint* pObj;
	POSITION pos = m_ResultPoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CResultPoint*) m_ResultPoint.GetNext(pos);
		CRect rect = pObj->m_Rect;
		pDoc->LPtoPP(&rect);
		CRect rect1 =m_ClientRect;
		pDoc->LPtoPP(&rect1);
		rect|=rect1;
		if (pDC->RectVisible(rect))
			pObj->Draw(pDC);
	}
	CFIDDrawObj::DrawClient(pDC);
}

int CResultObj::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CFIDDrawObj::HitTest(pView,point); 
	if ((nHandle >= 0) || (!m_Rect.PtInRect(point))) return nHandle;
	if(m_Rect.PtInRect(point)) 
		return 9;
	else return -1;
}

// ContourView.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ContourView.h"
#include "InverseDist.h"
#include "Kriging.h"
#include <algorithm>
#include <fstream>
#include "Gradient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int LEGEND_WIDTH = 120;

/////////////////////////////////////////////////////////////////////////////
// CContourView

IMPLEMENT_DYNCREATE(CContourView, CScrollView)

CContourView::CContourView() : m_pImage(NULL), m_bImageReady(false), m_nZoom(2), m_nBaseWidth(600), m_nBaseHeight(600),
m_nNumOfLevels(256), m_bLegend(true), m_bValues(true), m_dThickMax(0), m_dThickMin(0)
{
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0));    
	memcpy(&m_Logfont, &(ncm.lfMessageFont), sizeof(LOGFONT));
	m_XTitle="纵坐标",m_YTitle="横坐标",m_ZTitle="地面海拔";
	m_XMin=0.;m_XMax=0.;m_XScale=5000;//X轴刻度
	m_YMin=0.;m_YMax=0.;m_YScale=5000;//Y轴刻度
	m_ZMin=0;m_ZMax=0;m_ZScaleNum=24;//Z轴刻度
	m_nDiameter=600;              //图形宽度
	m_nDiameter3D=300;              //图形宽度
	m_nInterpolater=1;  
	m_bZAuto=TRUE;
	m_SchemeFile="Wave-STC";
	m_nLegend=1;
}

CContourView::~CContourView()
{
	if(m_pImage)
		delete m_pImage;
}


BEGIN_MESSAGE_MAP(CContourView, CScrollView)
	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(CContourView)
	ON_COMMAND(ID_SHOWVALUES, OnShowvalues)
	ON_COMMAND(ID_SHOWLEGEND, OnShowlegend)
	ON_UPDATE_COMMAND_UI(ID_SHOWVALUES, OnUpdateShowvalues)
	ON_UPDATE_COMMAND_UI(ID_SHOWLEGEND, OnUpdateShowlegend)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_ZOOM_LITTLE, OnZoomLittle)
	ON_COMMAND(ID_ZOOM_BIG, OnZoomBig)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CContourView drawing

void CContourView::OnDraw(CDC* pDC)
{
	DrawScale(pDC);
	if(m_bImageReady) 
	{
		m_pImage->Draw(pDC, 0, 0, m_nZoom);
		if(m_bValues)
			DrawValues(pDC);
		if(m_bLegend)
			DrawLegend(pDC);
	}
	if(m_bValues)
		DrawPoints(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CContourView diagnostics

#ifdef _DEBUG
void CContourView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CContourView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CContourView message handlers
void CContourView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	// TODO: Add your specialized code here and/or call the base class
	CSize sizeTotal;
	sizeTotal.cx =m_nDiameter*m_nZoom+LEGEND_WIDTH+140; 
	sizeTotal.cy =m_nDiameter*m_nZoom+80;
	SetScrollSizes(MM_TEXT, sizeTotal);

	int nInterpolater =m_nInterpolater;
	if(m_vecNum>110)
	{
		nInterpolater=0;
		m_nInterpolater=nInterpolater;
	}
	if(m_vecNum<2)
		return;
	m_nNumOfLevels=m_ZScaleNum;
	CWaitCursor wait;
	vector<double> vecZs;
	int nRadius=m_nDiameter/2;
	int i,j;double z;
	vector<Point3D>& input = const_cast<vector<Point3D>&>(m_ir.Get3DPoints());
	Interpolater* pInterpolater = NULL;
	if(nInterpolater==0)
		pInterpolater = new InverseDist(m_nDiameter,4.);
	else if(nInterpolater==1)
		pInterpolater = new Kriging((Point3D*)&(*input.begin()),(Point3D*)&(*input.end()),4.);
	for(i=0;i<m_nDiameter; i++) 
	{
		for(j=0;j<m_nDiameter; j++)
		{
			z=pInterpolater->GetInterpolatedZ(j-nRadius,i-nRadius,(Point3D*)&(*input.begin()),(Point3D*)&(*input.end()));
			vecZs.push_back(z);			
		}
	}
	delete pInterpolater;
	CGradient m_Gradient;
	CString ColorFile=GetWorkPath("Color")+m_SchemeFile+".cip";
	CFile file(ColorFile, CFile::modeRead | CFile::shareDenyWrite );
	CArchive ar(&file, CArchive::load);	
	m_Gradient.Serialize(ar);

	RGBTRIPLE *pal = new RGBTRIPLE[m_nNumOfLevels], *entry;
	m_Gradient.SetQuantization (m_nNumOfLevels);
	m_Gradient.MakeEntries(pal,m_nNumOfLevels);

	vector<double>::iterator iter;
	iter = max_element(vecZs.begin(), vecZs.end());
	m_dThickMax = *iter;
	iter = min_element(vecZs.begin(), vecZs.end());
	m_dThickMin = *iter;
	double interval =(m_dThickMax-m_dThickMin)/m_nNumOfLevels;
	double correct=(m_dThickMax-m_dThickMin)/(m_ZMax-m_ZMin);
	// fill in CDib structure
	int size = m_nDiameter * m_nDiameter;
	int iWidthBytes = ((m_nDiameter * 24 + 15) & ~15) >> 3 ;
	DWORD dwImgSize = iWidthBytes * m_nDiameter;

	// Data bits
	LPBYTE pDibBits;
	pDibBits = new BYTE[dwImgSize];
	LPBYTE pBit = pDibBits;
	int index;
	for(i=0;i<(int)vecZs.size();i++) 
	{
		z = vecZs[i];
		index= (int)(((z-m_dThickMin)-(m_ZMin-m_zDataMin))/interval*correct);
		if(index<0) index=0;
		if(index>m_nNumOfLevels-1)
			index=m_nNumOfLevels-1;
		entry = &pal[index];
		*pBit = entry->rgbtBlue;
		pBit++;
		*pBit = entry->rgbtGreen;
		pBit++;
		*pBit = entry->rgbtRed;
		pBit++;
	}
	delete[] pal;

	if(m_pImage!=NULL)
		delete m_pImage;
	m_pImage = new CDib;
	try
	{
		m_pImage->Create(m_nDiameter,m_nDiameter,pDibBits);
	} 
	catch(CImageException& e) 
	{
		::AfxMessageBox(e.what());
		delete[] pDibBits;
		return;
	}
	m_bImageReady = true;
}

void CContourView::DrawValues(CDC* pDC)
{
	vector<Point3D>& input = const_cast<vector<Point3D>&>(m_ir.Get3DPoints());
	CFont font;
	font.CreateFontIndirect(&m_Logfont);
	CFont* poldFont = pDC->SelectObject(&font);
	COLORREF OldColor=pDC->SetTextColor(0x000000);
	pDC->SetBkMode(TRANSPARENT);
	DWORD i;CSize size;
	for(i=0; i<input.size(); i++) 
	{
		int x = (int)(m_nZoom*(input[i].x+m_nBaseWidth/2));
		int y = (int)(m_nZoom*(0.5 * m_nBaseHeight-input[i].y));
		size=pDC->GetTextExtent("●");
		pDC->TextOut(x-size.cx/2,y-size.cy/2,"●");
		y=y+size.cy/2;
		size=pDC->GetTextExtent(input[i].szStr); 
		if(x>size.cx/2)
			pDC->TextOut(x-size.cx/2,y, input[i].szStr);
		else
			pDC->TextOut(x, y,input[i].szStr);
	}
	pDC->SelectObject(poldFont);
	pDC->SetTextColor(OldColor);
}

void CContourView::DrawPoints(CDC* pDC)
{
	vector<Point3D>& input = const_cast<vector<Point3D>&>(m_ir.GetPoints());
	CFont font;
	font.CreateFontIndirect(&m_Logfont);
	CFont* poldFont = pDC->SelectObject(&font);
	COLORREF OldColor=pDC->SetTextColor(0x0000FF);
	pDC->SetBkMode(TRANSPARENT);
	DWORD i;CSize size;
	for(i=0; i<input.size(); i++) 
	{
		int x = (int)(m_nZoom*(input[i].x+m_nBaseWidth/2));
		int y = (int)(m_nZoom*(0.5 * m_nBaseHeight-input[i].y));
		pDC->SetTextColor(0x0000FF);
		size=pDC->GetTextExtent("★");
		pDC->TextOut(x-size.cx/2,y-size.cy/2,"★");//☆★○◎◆□△※＃
		y=y+size.cy/2;
		size=pDC->GetTextExtent(input[i].szStr); 
		pDC->SetTextColor(0xFF0000);
		if(x>size.cx/2)
			pDC->TextOut(x-size.cx/2,y, input[i].szStr);
		else
			pDC->TextOut(x, y,input[i].szStr);
	}
	pDC->SelectObject(poldFont);
	pDC->SetTextColor(OldColor);
}

void CContourView::DrawScale(CDC* pDC)
{
	CFont font;
	font.CreateFontIndirect(&m_Logfont);
	CFont* poldFont = pDC->SelectObject(&font);
	int mw=m_nDiameter*m_nZoom;
	pDC->TextOut(2,mw+25,m_XTitle);
	int flage=mw;	
	if(!m_bImageReady)
		flage=0;
	pDC->MoveTo(0,mw+20);
	pDC->LineTo(mw,mw+20);
	pDC->MoveTo(mw+20,0);
	pDC->LineTo(mw+20,mw);
	double x_min=m_XMin;
	double x_Scale=(double)m_XScale;
	double x_ScaleMin=x_Scale/10.;
	while(x_min<m_XMax)
	{
		if(((int)x_min)%m_XScale==0)
			break;
		x_min+=1.;
	}
	int x0=(int)((x_min-m_XMin)/(m_XMax-m_XMin)*mw);
	pDC->MoveTo(x0,flage);
	pDC->LineTo(x0,mw+25);
	double dlta=x_min-x_ScaleMin;
	int x,xx;
	while(dlta>m_XMin)
	{
		xx=(int)((dlta-m_XMin)/(m_XMax-m_XMin)*mw);
		pDC->MoveTo(xx,mw+15);
		pDC->LineTo(xx,mw+20);
		dlta-=x_ScaleMin;
	}
	x_min+=x_Scale;
	CString str;CSize size;
	while(x_min<m_XMax)
	{
		x=(int)((x_min-m_XMin)/(m_XMax-m_XMin)*mw);
		pDC->MoveTo(x,flage);
		pDC->LineTo(x,mw+25);
		str.Format("%d",(int)x_min);
		size=pDC->GetTextExtent(str); 
		pDC->TextOut(x-size.cx/2,mw+25,str);
		dlta=x_min-x_ScaleMin;
		while(dlta>x_min-x_Scale)
		{
			xx=(int)((dlta-m_XMin)/(m_XMax-m_XMin)*mw);
			pDC->MoveTo(xx,mw+15);
			pDC->LineTo(xx,mw+20);
			dlta-=x_ScaleMin;
		}
		x0=x;
		x_min+=x_Scale;
	}
	dlta=x_min-(x_Scale-x_ScaleMin);
	while(dlta<m_XMax)
	{
		xx=(int)((dlta-m_XMin)/(m_XMax-m_XMin)*mw);
		pDC->MoveTo(xx,mw+15);
		pDC->LineTo(xx,mw+20);
		dlta+=x_ScaleMin;
	}

	pDC->TextOut(mw+25,2,m_YTitle);

	int y;
	double y_min=m_YMin;
	double y_Scale=(double)m_YScale;
	double y_ScaleMin=y_Scale/10.;
	while(y_min<m_YMax)
	{
		if(((int)y_min)%m_YScale==0)
			break;
		y_min+=1.;
	}
	int y0=(int)((y_min-m_YMin)/(m_YMax-m_YMin)*mw);
	pDC->MoveTo(flage,mw-y0);
	pDC->LineTo(mw+25,mw-y0);
	
	int yy;
	dlta=y_min-y_ScaleMin;
	while(dlta>m_YMin)
	{
		yy=(int)((dlta-m_YMin)/(m_YMax-m_YMin)*mw);
		pDC->MoveTo(mw+15,mw-yy);
		pDC->LineTo(mw+20,mw-yy);
		dlta-=y_ScaleMin;
	}
	y_min+=y_Scale;
	size=pDC->GetTextExtent("3210"); 
	while(y_min<m_YMax)
	{
		y=(int)((y_min-m_YMin)/(m_YMax-m_YMin)*mw);
		pDC->MoveTo(flage,mw-y);
		pDC->LineTo(mw+25,mw-y);
		str.Format("%d",(int)y_min);
		pDC->TextOut(mw+25,mw-y-size.cy/2,str);
		dlta=y_min-y_ScaleMin;
		while(dlta>y_min-y_Scale)
		{
			yy=(int)((dlta-m_YMin)/(m_YMax-m_YMin)*mw);
			pDC->MoveTo(mw+15,mw-yy);
			pDC->LineTo(mw+20,mw-yy);
			dlta-=y_ScaleMin;
		}
		y0=y;
		y_min+=y_Scale;
	}
	dlta=y_min-(y_Scale-y_ScaleMin);
	while(dlta<m_YMax)
	{
		yy=(int)((dlta-m_YMin)/(m_YMax-m_YMin)*mw);
		pDC->MoveTo(mw+15,mw-yy);
		pDC->LineTo(mw+20,mw-yy);
		dlta+=y_ScaleMin;
	}
	pDC->SelectObject(poldFont);
}

void CContourView::DrawLegend(CDC* pDC)
{
	int RECT_HEIGHT = m_nDiameter*m_nZoom/m_nNumOfLevels;
	int RECT_WIDTH = 20;

	CFont font;
	font.CreateFontIndirect(&m_Logfont);
	CFont* poldFont = pDC->SelectObject(&font);

	CGradient m_Gradient;
	CString ColorFile=GetWorkPath("Color")+m_SchemeFile+".cip";
	CFile file(ColorFile, CFile::modeRead | CFile::shareDenyWrite );
	CArchive ar(&file, CArchive::load);	
	m_Gradient.Serialize(ar);
	RGBTRIPLE *pal = new RGBTRIPLE[m_nNumOfLevels], *entry;
	m_Gradient.SetQuantization (m_nNumOfLevels);
	m_Gradient.MakeEntries(pal,m_nNumOfLevels);

	int x1 = m_nZoom * m_nBaseWidth + 80;
	CString str;
	double interval =(m_ZMax-m_ZMin) / m_nNumOfLevels;
	CBrush br;CRect rect;
	int i,x=0;
	CSize size;
	for(i=0; i<m_nNumOfLevels; i++) 
	{
		entry = &pal[i];
		br.CreateSolidBrush(RGB(entry->rgbtRed,entry->rgbtGreen,entry->rgbtBlue));
		rect.left=x1+3;rect.top=i*RECT_HEIGHT+2;
		rect.right=x1+RECT_WIDTH;rect.bottom=(i+1)*RECT_HEIGHT+2;
		pDC->FillRect(rect,&br);
		if(i%m_nLegend==0)
		{
			str.Format(_T("%.1f-%.1f"), m_ZMin+(i*interval), m_ZMin+((i+1)*interval));
			pDC->TextOut(x1+RECT_WIDTH+4, i*RECT_HEIGHT,str);
		}
		br.DeleteObject();
	}
	delete[] pal;
	pDC->TextOut(x1+4, rect.bottom+10,m_ZTitle);
	pDC->SelectObject(poldFont);
}

void CContourView::OnShowvalues() 
{
	// TODO: Add your command handler code here
	m_bValues = !m_bValues;
	Invalidate();
}

void CContourView::OnShowlegend() 
{
	// TODO: Add your command handler code here
	m_bLegend = !m_bLegend;
	Invalidate();
}

void CContourView::OnUpdateShowvalues(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_bValues);
}

void CContourView::OnUpdateShowlegend(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_bLegend);
}

void CContourView::OnZoomLittle()
{
	m_nZoom--;
	if(m_nZoom<1)
		m_nZoom=1;
	CSize sizeTotal;
	sizeTotal.cx =m_nDiameter*m_nZoom+LEGEND_WIDTH+140; 
	sizeTotal.cy =m_nDiameter*m_nZoom+80;
	SetScrollSizes(MM_TEXT, sizeTotal);
	Invalidate();
}

void CContourView::OnZoomBig() 
{
	m_nZoom++;
	CSize sizeTotal;
	sizeTotal.cx =m_nDiameter*m_nZoom+LEGEND_WIDTH+140; 
	sizeTotal.cy =m_nDiameter*m_nZoom+80;
	SetScrollSizes(MM_TEXT, sizeTotal);
	Invalidate();
}
void CContourView::OnEditCopy() 
{
	// TODO: Add your command handler code here
	if(!m_bImageReady)
		return;
	CWaitCursor wait;

	CClientDC scrDC(this);
	CDC memDC;           // screen DC and memory DC
	CBitmap bitmap;
	CBitmap* pOldBitmap;  // handles to deice-dependent bitmaps

	memDC.CreateCompatibleDC(&scrDC);
	/* create a bitmap compatible with the screen DC */
	CSize sizeTotal;
	sizeTotal.cx =m_nDiameter*m_nZoom+LEGEND_WIDTH+140; 
	sizeTotal.cy =m_nDiameter*m_nZoom+80;

	bitmap.CreateCompatibleBitmap(&scrDC, sizeTotal.cx, sizeTotal.cy);

	pOldBitmap = memDC.SelectObject(&bitmap);// select new bitmap into memory DC

	memDC.FillSolidRect(0,0,sizeTotal.cx, sizeTotal.cy,0xFFFFFF);
	DrawScale(&memDC);
	if(m_bImageReady) 
	{
		m_pImage->Draw(&memDC, 0, 0, m_nZoom);
		if(m_bValues)
			DrawValues(&memDC);
		if(m_bLegend)
			DrawLegend(&memDC);
	}
	if(m_bValues)
		DrawPoints(&memDC);


	HBITMAP hBitmap = (HBITMAP)bitmap.Detach();
	memDC.SelectObject(pOldBitmap);

	// Place the copy on the clipboard.
	::OpenClipboard(m_hWnd);
    ::EmptyClipboard();
    ::SetClipboardData(CF_BITMAP, hBitmap);
    ::CloseClipboard();

	// clean up 
	memDC.DeleteDC();
}

void CContourView::Init(LPCTSTR RootName,DWORD RootID)
{
	m_ir.m_nDiameter=m_nDiameter; //图形宽度
	m_ir.m_nDiameter3D=m_nDiameter3D;//图形宽度
	m_ir.Read(RootName,RootID,false);

	m_XMin=m_ir.m_XMin;					
	m_XMax=m_ir.m_XMax;
	m_YMin=m_ir.m_YMin;
	m_YMax=m_ir.m_YMax;
	m_ZMin=m_ir.m_ZMin;
	m_ZMax=m_ir.m_ZMax;
	m_nDiameter=m_ir.m_nDiameter;              
	m_nDiameter3D=m_ir.m_nDiameter3D;
	m_PointNum=m_ir.m_PointNum;
	m_vecNum=m_ir.m_vecNum;
	m_zDataMin=m_ir.m_ZMin;
	m_zDataMax=m_ir.m_ZMax;
}

void CContourView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu menu;
	if(menu.LoadMenu(IDR_POP_MENU))
	{
		CMenu* pPopup = menu.GetSubMenu(0);
		pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN,
								point.x, point.y,
								this->GetWindow(0)); 
	}
}
void CContourView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(GetKeyState(VK_CONTROL) & ~1)
		OnZoomLittle();
	else
		OnZoomBig();
	CScrollView::OnLButtonDblClk(nFlags, point);
}

CString CContourView::GetWorkPath(CString flag)
{
	CString WorkPath;
	if(!GetLogTopRegString("SOFTWARE\\四川石油管理局测井公司\\测井解释平台\\User","WorkDisk",WorkPath))
		WorkPath="";
	else
		WorkPath+="\\"+flag+"\\";
	return WorkPath;
}

BOOL CContourView::GetLogTopRegString(CString szSubKey,LPCTSTR lpszEntry,CString& strValue)
{
	ASSERT(lpszEntry != NULL);
	HKEY hSecKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSubKey, 0, KEY_QUERY_VALUE,
		&hSecKey) == ERROR_SUCCESS)
	{
		DWORD dwType, dwCount;
		LONG lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
			NULL, &dwCount);
		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_BINARY);
			LONG lResult = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
				(LPBYTE)strValue.GetBuffer(dwCount/sizeof(TCHAR)), &dwCount);
			strValue.ReleaseBuffer();
		}
		RegCloseKey(hSecKey);
		if (lResult == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_BINARY);
			return TRUE;
		}
	}
	return FALSE;
}
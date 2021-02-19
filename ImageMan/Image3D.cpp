/******************************************************************
*   Image3D.cpp - implementation of the Wf draw object class     *           
*******************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "FIDIO.h"
#include "styledialog.h"
#include "TrackDraw.h"
#include "Image3D.h"
#include "Image3DPage.h"
#include "LineListPage.h"
#include "FillListPage.h"
#include "MinFontDialog.h"
#include "MainFrm.h"
#include "HandWorkPage.h"

CPlanePoint::CPlanePoint(CRect rect,float Dep,float Dip,float Dir,CString Flag,
					   WORD PenWidth,COLORREF Color,float LookAngle,double HVr,BOOL bDrawBox)
{
	m_Dep=Dep;
	m_Dip=Dip;
	m_Dir=Dir;
	m_Flag=Flag;
	m_Rect = rect;
	m_Color=Color;
	m_PenWidth=PenWidth;
	m_LookAngle=LookAngle;
	m_HVr=HVr;
	m_bDrawBox=bDrawBox;
}
float CPlanePoint::Dir360(float dir)
{
	while(dir>360.f) 
		dir-=360.f;
	while(dir<0.f) 
		dir+=360.f;
	return dir;
}
void CPlanePoint::DrawEllipse(CDC* pDC,CImageManDoc* pDoc,int StartPos)
{
	CRect rect=m_Rect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;
	
	double r=rect.Width()/4.; 
	int StartX=rect.left+rect.Width()/2-(int)r;
	double dtor=r/90.;
	double High=r*tan(m_Dip*PI/180.)*m_HVr;
	double hh=sin(m_LookAngle*PI/180.0);
	int x0,y0,x,y;
	
	int i,k;
	int *pData=new int[80];
	for(i=0;i<=360;i+=5)
	{
		k=i/5;
		pData[k]=(rect.top+rect.bottom)/2+(int)(cos(i*PI/180.)*High);
	}

	if(High<4&&m_bDrawBox) m_HVr=0;
	//里
	Graphics *pic=new Graphics(pDC->m_hDC);
	pic->SetSmoothingMode(SmoothingModeHighQuality);
	Color CurveColor(GetRValue(m_Color),GetGValue(m_Color),GetBValue(m_Color));
	Pen pn(CurveColor,(float)m_PenWidth);

	x0=StartX;
	float dir=Dir360(StartPos-m_Dir);

	k=(int)(dir/5.f);
	y0=pData[k];

	Point *pt=new Point[130];
	pt[0].X=x0;pt[0].Y=y0;
	int Num=1;
	for(i=5;i<180;i+=5)
	{
		x=StartX+(int)(i*dtor);
		
		dir=Dir360(StartPos-m_Dir+i);
		k=(int)(dir/5.f);
		y=pData[k]+(int)(sqrt(r*r-(dtor*i-r)*(dtor*i-r))*hh);
		pic->DrawLine(&pn,x0,y0,x,y);
		x0=x;y0=y;

		pt[Num].X=x;pt[Num].Y=y;
		Num++;
	}
	if(m_bDrawBox)
	{
		///////////////////////////////////////////////////////////
		//改变半径后重新赋值
		//
		r=rect.Width()/2.;
		dtor=r/90.;
		High =r*tan(m_Dip*PI/180.)*m_HVr;
		for(i=0;i<=360;i+=5)
		{
			k=i/5;
			pData[k]=(rect.top+rect.bottom)/2+(int)(cos(i*PI/180.)*High);
		}
		//右边点
		int ix=rect.Width()/4.;
		for(i=0;i<=180;i+=5)
		{
			x=rect.left+(int)(i*dtor);
			dir=Dir360(StartPos-m_Dir+i+180.f);
			k=(int)(dir/5.f);
			y=pData[k]-(int)(sqrt(r*r-(dtor*i-r)*(dtor*i-r))*hh);
			if(x>=rect.right-ix-2)
			{
				pt[Num].X=x;pt[Num].Y=y;
				Num++;
			}
		}

		//外
		for(i=0;i<=180;i+=5)
		{
			x=rect.right-(int)(i*dtor);
			dir=Dir360(StartPos-m_Dir+180.f-i);
			k=(int)(dir/5.f);
			y=pData[k]+(int)(sqrt(r*r-(dtor*(180-i)-r)*(dtor*(180-i)-r))*hh);
			pt[Num].X=x;pt[Num].Y=y;
			Num++;
		}

		//左边点
		for(i=0;i<=180;i+=5)
		{
			x=rect.left+(int)(i*dtor);
			dir=Dir360(StartPos-m_Dir+i+180.f);
			k=(int)(dir/5.f);
			y=pData[k]-(int)(sqrt(r*r-(dtor*i-r)*(dtor*i-r))*hh);
			if(x<=rect.left+ix)
			{
				pt[Num].X=x;pt[Num].Y=y;
				Num++;
			}
		}

		SolidBrush transBrush(Color(120, GetRValue(m_Color),GetGValue(m_Color),GetBValue(m_Color)));  
		pic->FillClosedCurve(&transBrush, pt, Num);
	}
	pic->ReleaseHDC(pDC->m_hDC);
	delete pt;delete pic;
	delete pData;
}
void CPlanePoint::Draw(CDC* pDC,CImageManDoc* pDoc,int StartPos)
{
	CRect rect=m_Rect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;
	
	double r=rect.Width()/4.; 
	double High=r*tan(m_Dip*PI/180.)*m_HVr;
	if(High<4||!m_bDrawBox)
	{
		DrawEllipse(pDC,pDoc,StartPos);
		return;
	}
	
	int x0=(rect.left+rect.right)/2;			// 绘图原点x
	int y0=(rect.top+rect.bottom)/2;			// 绘图原点y
	double x=0;									// 长方体位置x
	double y=0;									// 长方体位置y
	double z=0;									// 长方体位置z
	double cx=rect.Width();						// 长
	double cy=0;								// 宽
	double cz=cx;								// 高
	
	double ax=m_Dip*PI/180.;					// x轴旋转角度
	double ay=(StartPos-m_Dir+90.)*PI/180.;		// y轴旋转角度
	double az=m_LookAngle*PI/180.;				// z轴旋转角度
	double f=1000;								// 视距
	
	int czMin=(int)sqrt(4.*High*High+r*r);
	if(cz<czMin)
		cz=czMin;

	int i;
    double pts[8][3];
    double l, sina, sinb, cosa, cosb;
    CPoint points[4];
    int face[6][4] =
    {
        {0, 1, 2, 3},
        {4, 7, 6, 5},
        {1, 0, 4, 5},
        {2, 1, 5, 6},
        {3, 2, 6, 7},
        {0, 3, 7, 4}
    };
    // 初始化坐标
    pts[0][0] = x-cx/2;
    pts[0][1] = y;
    pts[0][2] = z-cz/2;
    pts[1][0] = x-cx/2;
    pts[1][1] = y;
    pts[1][2] = z + cz/2;
    pts[2][0] = x + cx/2;
    pts[2][1] = y;
    pts[2][2] = z + cz/2;
    pts[3][0] = x + cx/2;
    pts[3][1] = y;
    pts[3][2] = z-cz/2;
    for(i = 0; i < 4; i++)
    {
        pts[i + 4][0] = pts[i][0];
        pts[i + 4][1] = pts[i][1] + cy;
        pts[i + 4][2] = pts[i][2];
    }
    // 旋转变换
	for(i = 0; i < 8; i++)
    {
        // 绕x轴旋转
        l = sqrt(pts[i][1] * pts[i][1] + pts[i][2] * pts[i][2]);
        sina = sin(ax);
        cosa = cos(ax);
        sinb = pts[i][2] / l;
        cosb = pts[i][1] / l;
        pts[i][1] = l * (cosa * cosb - sina * sinb);
        pts[i][2] = l * (cosa * sinb + sina * cosb);
        // 绕y轴旋转
        l = sqrt(pts[i][2] * pts[i][2] + pts[i][0] * pts[i][0]);
        sina = sin(ay);
        cosa = cos(ay);
        sinb = pts[i][0] / l;
        cosb = pts[i][2] / l;
        pts[i][2] = l * (cosa * cosb - sina * sinb);
        pts[i][0] = l * (cosa * sinb + sina * cosb);
        // 绕z轴旋转
        l = sqrt(pts[i][0] * pts[i][0] + pts[i][1] * pts[i][1]);
        sina = sin(az);
        cosa = cos(az);
        sinb = pts[i][1] / l;
        cosb = pts[i][0] / l;
        pts[i][0] = l * (cosa * cosb - sina * sinb);
        pts[i][1] = l * (cosa * sinb + sina * cosb);
    }
    // 透视变换
    for(i = 0; i < 8; i++)
    {
        l = f / (pts[i][2] + f);
        pts[i][0] *= l;
        pts[i][1] *= l;
    }
    // 绘制
	Point pt[40];
	pt[0].X = int(x0 + pts[face[0][0]][0]);
	pt[0].Y = int(y0 + pts[face[0][0]][1]);
	pt[1].X = int(x0 + pts[face[0][1]][0]);
	pt[1].Y = int(y0 + pts[face[0][1]][1]);
	pt[2].X = int(x0 + pts[face[0][2]][0]);
	pt[2].Y = int(y0 + pts[face[0][2]][1]);
	pt[3].X = int(x0 + pts[face[0][3]][0]);
	pt[3].Y = int(y0 + pts[face[0][3]][1]);
	
	Graphics *pic=new Graphics(pDC->m_hDC);
	pic->SetSmoothingMode(SmoothingModeHighQuality);
	//获取平面区域
	GraphicsPath myPath;
	myPath.AddPolygon(pt,4);
	Region patchRgn(&myPath);
	//用于区域操作
	Region tmpRgn(&myPath);

	double dtor=r/90.;
	double hh=sin(m_LookAngle*PI/180.0);

	int xx,yy,j=1;
	for(i=0;i<=180;i+=5)
	{
		xx=x0+(int)(dtor*i-r);
		yy=y0+(int)(cos((i+StartPos-m_Dir)*PI/180.)*High);
		yy=yy+(int)(sqrt(r*r-(dtor*i-r)*(dtor*i-r))*hh);
	
		pt[j].X=xx;pt[j].Y=yy;
		j++;
	}
	if(pt[19].Y<y0)
	{
		pt[0].X=(int)(x0-r);pt[0].Y=y0+czMin*2;
		pt[j].X=(int)(x0+r);pt[j].Y=y0+czMin*2;
	}
	else
	{
		pt[0].X=(int)(x0-r);pt[0].Y=y0-czMin*2;
		pt[j].X=(int)(x0+r);pt[j].Y=y0-czMin*2;
	}
	//获取遮挡区域
	GraphicsPath myPath1;
	myPath1.AddPolygon(pt,j+1);
	Region patchRgn1(&myPath1);
	//获取平面及遮挡区域的交集
	tmpRgn.Intersect(&myPath1);
	//异并集获取填充区域
	patchRgn.Xor(&tmpRgn);
	//定义半透明画刷
	SolidBrush transBrush(Color(170,GetRValue(m_Color),GetGValue(m_Color),GetBValue(m_Color)));
	pic->FillRegion(&transBrush,&patchRgn);

	Color CurveColor(GetRValue(m_Color),GetGValue(m_Color),GetBValue(m_Color));
	Pen pn(CurveColor,(float)m_PenWidth);
	int xx0,yy0;
	for(i=0;i<=180;i+=5)
	{
		xx=x0+(int)(dtor*i-r);
		yy=y0+(int)(cos((i+StartPos-m_Dir)*PI/180.)*High);
		yy=yy+(int)(sqrt(r*r-(dtor*i-r)*(dtor*i-r))*hh);
		if(i>0) pic->DrawLine(&pn,xx0,yy0,xx,yy);
		xx0=xx;yy0=yy;
	}

	pic->ReleaseHDC(pDC->m_hDC);
	delete pic;
}

IMPLEMENT_SERIAL(CImage3D, CFIDDrawObj, 0)
        
CImage3D::CImage3D()
{
	m_DrawShape = image3D;
	m_BkColor = RGB(0,0,0);
	m_ColorGrade = 32;
	m_bColor=FALSE;
	m_LeftValue =0;
	m_RightValue = 1024;
	m_PenWidth = 1;
	m_StartPos=0;
	m_LookAngle=30.f;
	m_Bit=216;
	m_HorScale=10;
	m_SchemeFile="Wood";
	m_CtrlVer = 4;
}
    
CImage3D::CImage3D(CString szName, const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDoc)
			:CFIDDrawObj(szName,rect,pParentObj,pDoc)
{
	m_DrawShape = image3D;
	m_BkColor = RGB(0,0,0);
	m_bColor=FALSE;
	m_LeftValue =0;
	m_RightValue = 1024;
	m_ColorGrade = 32;
	m_PenWidth = 1;
	m_StartPos=0;
	m_Bit=216;
	m_HorScale=(int)(m_Bit*PI/(m_Rect.Width ()*0.254));
	m_LookAngle=30.f;
	m_HeadRect.top -= 40;
	m_Rect.top = m_HeadRect.top;
	//获取系统默认字体
	LOGFONT lf;
	CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	font->GetLogFont(&lf);
	m_ScaleFont = lf; 
	m_SchemeFile="Wood";
	m_CtrlVer = 2;
	if(szName.ReverseFind('\\')>0)
		m_PlaneName=szName.Left(szName.ReverseFind('\\')+1)+"HandWork.List";
}

CImage3D::~CImage3D()
{
	POSITION pos = m_PlanePoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_PlanePoint.GetNext(pos);
	m_PlanePoint.RemoveAll();
}    

void CImage3D::Serialize( CArchive& ar )
{
	CFIDDrawObj::Serialize( ar );
	if(ar.IsStoring() )
	{
		m_CtrlVer=4;
		ar << m_CtrlVer;
		ar << m_PenWidth;
		ar << m_LeftVal;
		ar << m_RightVal;
		ar << m_BkColor;
		ar << m_bColor;
		ar << m_StartPos;
		ar << m_LookAngle;
		ar << m_Bit;
		ar << m_HorScale;
		SerializeLogFont(ar,m_ScaleFont);
		ar << m_SchemeFile;
		ar << m_PlaneName;
		ar << m_PlotContent;
	}
	else
	{
		ar >> m_CtrlVer;
		ar >> m_PenWidth;
		if(m_CtrlVer>3)
		{
			ar >> m_LeftVal;
			ar >> m_RightVal;
		}
		else
		{
			ar >> m_LeftValue;
			ar >> m_RightValue;
			m_LeftVal = m_LeftValue; 
			m_RightVal = m_RightValue;
		}
		ar >> m_BkColor;
		ar >> m_bColor;
		ar >> m_StartPos;
		ar >> m_LookAngle;
		ar >> m_Bit;
		if(m_CtrlVer>1)
			ar >> m_HorScale;
		SerializeLogFont(ar,m_ScaleFont);
		if(m_CtrlVer>1)
			ar >> m_SchemeFile;
		else
			m_ColorGrade=32;
		if(m_CtrlVer>2)
		{
			ar >> m_PlaneName;
			ar >> m_PlotContent;
		}

		CreateImagePalette();
		m_bFlush=TRUE;
	}
}

void CImage3D::OnScaleFont()
{
	int lfEscapement = m_ScaleFont.lfEscapement;
	CFontDialog m_FontDialog((LPLOGFONT)&m_ScaleFont); 
	if(m_FontDialog.DoModal() == IDOK)
	{
		m_ScaleFont.lfEscapement = lfEscapement;
		Invalidate();
	}	
}                        

BOOL CImage3D::Properties(CView* pView)
{
	// 1英寸=100像素点 m_Bit 单位 mm
	// 道宽的2/3绘制井周的一半 
	m_HorScale=(int)(m_Bit*PI*0.5/(m_Rect.Width ()*0.254*2./3.));
	/*m_HorScale=(int)(m_Bit*PI*0.5/(m_Rect.Width ()*0.254*2./3.)*1000);*/
	CPropertySheet m_Sheet("成像图属性");
	CImage3DBase m_Base(this);
	m_Sheet.AddPage(&m_Base);
	CHandWorkPage m_HandWorkPage;
	m_HandWorkPage.m_FileName =m_PlaneName;
	m_HandWorkPage.m_PlotContent=m_PlotContent;
	m_Sheet.AddPage(&m_HandWorkPage);
	CImage3DPage m_Page(this);
	m_Page.m_Bit=m_Bit;
	m_Sheet.AddPage(&m_Page);
	
	CLineListPage m_LinePage;
	LOGPEN m_LogPen;
	m_LogPen.lopnColor =RGB(0,0,255);
	m_LogPen.lopnWidth.x=m_PenWidth;
	m_LogPen.lopnWidth.y =m_PenWidth;
	m_LogPen.lopnStyle =BS_SOLID;
	m_LinePage.AddPen(&m_LogPen,"Sin-line");
	m_Sheet.AddPage(&m_LinePage);

	CMinFontPage m_FontPage;
	TEXTFONT m_TextFont[2];
	CString szHT;
	szHT.LoadString(IDS_ALIASFONT);
	m_TextFont[0].m_ItemName = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	m_TextFont[0].m_Text = m_AliasName;
	m_TextFont[0].m_ForeGround = m_NameColor;
	m_TextFont[0].m_BackGround = RGB(255,255,255);
	m_TextFont[0].lf = m_NameFont;
	int nHeight = m_NameFont.lfHeight;
	m_NameFont.lfHeight = (int) (nHeight * 0.72 - 0.5);
	m_FontPage.AddFont(&m_TextFont[0]);
	m_NameFont.lfHeight = nHeight;
	szHT.LoadString(IDS_ZERFONT);
	m_TextFont[1].m_ItemName = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	m_TextFont[1].m_Text = "012345";
	m_TextFont[1].m_ForeGround = m_NameColor;
	m_TextFont[1].m_BackGround = RGB(255,255,255);
	m_TextFont[1].lf = m_ScaleFont;
	m_FontPage.AddFont(&m_TextFont[1]);
	m_Sheet.AddPage(&m_FontPage);
	if (m_Sheet.DoModal() == IDOK)
	{
		m_PenWidth=(WORD)m_LogPen.lopnWidth.y;
		int w =(int)(m_Bit*PI*0.5/m_HorScale/0.254*3./2);
		CTrackObj* pObj=(CTrackObj*) this->GetParentObj ();
		if(pObj!=NULL)
		{
			CRect rect;
			rect=pObj->m_Rect ;
			rect.right = rect.left + w;
			pObj->SetRect(rect);
		}
		
		m_PlaneName=m_HandWorkPage.m_FileName;
		m_PlotContent=m_HandWorkPage.m_PlotContent;	

		m_FontPage.OnOK();
		m_NameColor = m_TextFont[0].m_ForeGround;
		m_NameFont = m_TextFont[0].lf;
		m_ScaleFont = m_TextFont[1].lf;
		m_NameFont.lfEscapement = 0;
		m_ScaleFont.lfEscapement = 0;
		CreateImagePalette();
		CreatePoint();
		CRect m_SaveRect = m_Rect;
		m_SaveRect.top = m_HeadRect.top;
		if(m_ParentType != 0) 
			m_SaveRect.bottom = m_Rect.bottom;
		InvalidateRect(m_SaveRect);
		return TRUE;
	}
	return FALSE;
}

void CImage3D::DrawHead(CDC *pDC)
{
	CFIDDrawObj::DrawHead(pDC);
	CRect saveHeadRect;
	saveHeadRect = m_HeadRect;
	pDoc->LPtoPP(m_HeadRect);
	if (!pDC->RectVisible(m_HeadRect)) 
	{
		m_HeadRect = saveHeadRect;
		return;
	}
	char ss[10];      
	
	CFont m_Font1;
	m_Font1.CreateFontIndirect(&m_NameFont);
	CFont* pOldFont =  pDC->SelectObject(&m_Font1);
    pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_NameColor);
	CSize textsize = pDC->GetTextExtent("文本"); 
    pDC->TextOut((m_HeadRect.left + m_HeadRect.right)/2, m_HeadRect.bottom - textsize.cy -10, m_AliasName,strlen(m_AliasName));
	
	CRect rect = m_HeadRect;
	rect.bottom = rect.top + 30;
	if (rect.bottom > m_HeadRect.bottom) rect.bottom = m_HeadRect.bottom;
	rect.InflateRect(-2,-2);
	CRect rect0=rect;
	int i;double dx;
	dx = (m_HeadRect.Width()-4.0) / m_ColorGrade;
	rect.left = m_HeadRect.left + 2;
	CPalette* pOldPalette = (CPalette*)pDC->SelectPalette(&m_Palette, FALSE);
	pDC->RealizePalette();	
	for (i=0; i<m_ColorGrade; i++)
	{
		rect.right = (int) (m_HeadRect.left + (i+1)*dx) + 2;
		pDC->FillSolidRect(rect,PALETTEINDEX(i));
		rect.left = rect.right;
	}
	pDC->SelectObject(pOldPalette);
	
	CFont m_Font;
	m_Font.CreateFontIndirect(&m_ScaleFont);
    pDC->SelectObject(&m_Font);
	pDC->MoveTo(m_Rect.left,m_HeadRect.bottom-5);
	pDC->LineTo(m_HeadRect.right,m_HeadRect.bottom - 5);
//	if(m_Daz)
	{
		if(m_StartPos+1>360) m_StartPos=0;
		dtos(m_StartPos,ss);
		pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
		pDC->TextOut(m_HeadRect.left+2 ,m_HeadRect.bottom - 5,ss);
		int aa=m_StartPos+180;
		if(aa>360) aa-=360;
		dtos(aa,ss);
		pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
		pDC->TextOut(m_HeadRect.right-2 ,m_HeadRect.bottom - 5,ss);
	}
    pDC->SetTextAlign(TA_CENTER | TA_TOP);//BOTTOM
    pDC->TextOut((m_HeadRect.left + m_HeadRect.right)/2, rect.bottom, m_UnitText);
    pDC->SetTextAlign(TA_LEFT | TA_TOP);
	dtos(m_LeftVal,ss);
    pDC->TextOut(m_HeadRect.left+2,rect.bottom,ss,strlen(ss));
	dtos(m_RightVal,ss);
    pDC->SetTextAlign(TA_RIGHT | TA_TOP);
    pDC->TextOut(m_HeadRect.right-2,rect.bottom,ss,strlen(ss));  
	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldFont);

	m_HeadRect = saveHeadRect;
}

void CImage3D::SetClipRect(CDC* pDC , CRect rect)
{
	if (pDC->m_hDC != NULL) ::LPtoDP(pDC->m_hDC,(LPPOINT)(LPRECT)rect,2);
	CRgn rgn; rgn.CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);
	pDC->SelectClipRgn(&rgn);
}

void CImage3D::CreateImagePalette()
{
	m_Palette.DeleteObject();		
	LPLOGPALETTE pLogPal = (LPLOGPALETTE) new BYTE[sizeof(LOGPALETTE)+m_ColorGrade * sizeof(PALETTEENTRY)];
	pLogPal->palVersion = 0x300; 
	pLogPal->palNumEntries = m_ColorGrade;	
	CGradient m_Gradient;
	CString ColorFile=GetWorkPath("Color")+m_SchemeFile+".cip";
	CFile file(ColorFile, CFile::modeRead | CFile::shareDenyWrite );
	CArchive ar(&file, CArchive::load);	
	m_Gradient.Serialize(ar);

	RGBTRIPLE *pal = new RGBTRIPLE[m_ColorGrade], *entry;
	m_Gradient.SetQuantization (m_ColorGrade);
	m_Gradient.MakeEntries(pal,m_ColorGrade);
	for (int i=0; i<m_ColorGrade; i++) 
	{
		if(m_bColor)
			entry = &pal[i];
		else
			entry = &pal[m_ColorGrade-i-1];
		pLogPal->palPalEntry[i].peRed = entry->rgbtRed;
		pLogPal->palPalEntry[i].peGreen = entry->rgbtGreen;
		pLogPal->palPalEntry[i].peBlue =  entry->rgbtBlue;
		pLogPal->palPalEntry[i].peFlags = PC_RESERVED; 
	}
	delete[] pal;
	m_Palette.CreatePalette(pLogPal);
	delete [] (BYTE *)pLogPal;
	ar.Close();file.Close();
}

void CImage3D::DrawPlane(CDC* pDC)
{			
	if(m_bFlush) CreatePoint();
	CPlanePoint* pObj=NULL;
	POSITION pos = m_PlanePoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CPlanePoint*) m_PlanePoint.GetNext(pos);
		CRect rect = pObj->GetRect();
		pDoc->LPtoPP(&rect);
		if(pDC->RectVisible(rect))
			pObj->Draw(pDC,pDoc,m_StartPos);
	}
}
void CImage3D::DrawClient(CDC *pDC)
{
	CRect rect = m_Rect;
	pDoc->LPtoPP(rect);
	if(!pDC->RectVisible(rect)) return;
	if((m_pParentObj == NULL) || (m_ParentType != 0)) 
		CFIDDrawObj::DrawClient(pDC);

	double r=rect.Width()/4;
	int StartX=rect.Width()/2-(int)r;
	int EndX=rect.Width()/2+(int)r;
	int Amp=(int)r;
	double sdep,edep,DrawElipseDep;
	double DataDep=YCoorToDepth(m_Rect.top +(int)(r*sin(m_LookAngle*PI/180.0))+2)-m_PlotSdep;
	CFIDIO *pFIDIO = new CFIDIO(m_strPathName);
	if(!pFIDIO->Open()) 
		{delete pFIDIO; return;}

	m_Rlev = pFIDIO->GetDepthLevel();
	m_SDep = m_PlotSdep+DataDep; 
	DrawElipseDep=m_SDep;
	m_EDep = m_PlotEdep;
	if((m_EDep < pFIDIO->GetStartDepth()) 
		|| (m_SDep > pFIDIO->GetEndDepth()))
		{delete pFIDIO; return;}
	if(m_SDep<pFIDIO->GetStartDepth()) 
	{
		m_SDep = ((int)(pFIDIO->GetStartDepth() / m_Rlev) + 1) * m_Rlev;
		DrawElipseDep=m_SDep;
	}
	if (m_EDep> pFIDIO->GetEndDepth()) 
		m_EDep = ((int)(pFIDIO->GetEndDepth() / m_Rlev) -1 ) * m_Rlev;

	CRect rect1 = rect;
	CRect m_ClipRect;
	pDC->GetClipBox(m_ClipRect);
	rect1 &= m_ClipRect;
	rect1.InflateRect(0,Amp);
	pDoc->PPtoLP(rect1);

	CRect rc=m_ClipRect;
	rc&= rect;
	rc.InflateRect(0,2);

	int Offset=0;
	if(rect.top>m_ClipRect.top)
		Offset=rect.top-m_ClipRect.top;
	if(m_ClipRect.bottom>rect.bottom)
		m_ClipRect.bottom=rect.bottom;
	CRect DrawRect(0,0,rect.Width(),m_ClipRect.Height());

	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC,DrawRect.Width(),DrawRect.Height());
	CBitmap* pOldBitmap =MemDC.SelectObject(&bmp);

	CBrush brush;
	CTrackObj* pT=(CTrackObj*)this->GetParentObj ();
	if(pT!=NULL)
		brush.CreateSolidBrush (pT->m_TrackClientBrush.lbColor );
	else
		brush.CreateSolidBrush (RGB(255,255,255));
	brush.UnrealizeObject();     
	MemDC.FillRect(DrawRect, &brush);	

	int i,cr;
	CPalette* pOldPalette;
	pOldPalette = (CPalette*)MemDC.SelectPalette(&m_Palette, FALSE);
	MemDC.RealizePalette();
	sdep = YCoorToDepth(rect1.top);
	edep = YCoorToDepth(rect1.bottom);
	m_EDep -=DataDep;
	if ((sdep > m_EDep) ||(edep<m_SDep)) 
		{delete pFIDIO; return;}
	if (sdep < m_SDep) sdep = m_SDep;
	if (edep > m_EDep) edep = m_EDep;		
	edep-=0.01;

	int y = DepthToYCoor(sdep)-pDoc->m_WinOrg.y;
	float* fwf = NULL;	 
	FIDCONTENT* Content = pFIDIO->GetContent(3);
	int nps = Content->nps;
	fwf = new float[2*nps+2];
	short* iwf = NULL;
	iwf = new short[nps+2];
	char* cwf = NULL;
	cwf = new char[nps+2];

	int xx,yy;
	int mmm;
	double hh=sin(m_LookAngle*PI/180.0);
	double WW=nps*0.5/(m_Rect.Width()/2.);
	int Ps=m_ColorGrade-1;
	float Dalta=m_RightVal-m_LeftVal;
	float LeftVal=m_LeftVal;
	float Grade=(float)m_ColorGrade;
	if(Content->length<2)
	{
		while((y<rect.bottom)&&(sdep<edep))
		{
			sdep=this->YCoorToDepth (y+pDoc->m_WinOrg.y);
			pFIDIO->SeekToDepth((float)sdep);
			pFIDIO->Read(3,nps,cwf);
			for(i=0;i<nps;i++)
			{
				fwf[i+nps]=(float)cwf[i];
				fwf[i+nps]=fwf[i];
			}
			if(m_StartPos>360) m_StartPos-=360;
			for(xx=StartX; xx<EndX; xx++)
			{
				i=(int)((xx-StartX)*WW);
				mmm=i+m_StartPos*nps/360;
				if((fwf[mmm]==-9999.f)||(fwf[mmm]==-999.25f))
				{
					yy=y+(int)(sqrt(r*r-(xx-StartX-r)*(xx-StartX-r))*hh);
					MemDC.SetPixel (xx,yy-m_ClipRect.top,m_BkColor);
					continue;
				}
				cr=(int)((fwf[mmm]-LeftVal)/Dalta*Grade);
				if(cr>Ps) cr=Ps;
				yy=y+(int)(sqrt(r*r-(xx-StartX-r)*(xx-StartX-r))*hh);
				MemDC.SetPixel (xx,yy-m_ClipRect.top,PALETTEINDEX(cr));
			}
			y++;
		}
	}
	else if(Content->length==2)
	{
		while((y<rect.bottom)&&(sdep<edep))
		{
			sdep=this->YCoorToDepth (y+pDoc->m_WinOrg.y);
			pFIDIO->SeekToDepth((float)sdep);
			pFIDIO->Read(3,nps,iwf);
			for(i=0;i<nps;i++)
			{
				fwf[i]=(float)iwf[i];
				fwf[i+nps]=fwf[i];
			}
			if(m_StartPos>360) 
				m_StartPos-=360;
			for(xx=StartX; xx<EndX; xx++)
			{
				i=(int)((xx-StartX)*WW);
				mmm=i+m_StartPos*nps/360;
				if((fwf[mmm]==-9999.f)||(fwf[mmm]==-999.25f))
				{
					yy=y+(int)(sqrt(r*r-(xx-StartX-r)*(xx-StartX-r))*hh);
					MemDC.SetPixel (xx,yy-m_ClipRect.top,m_BkColor);
					continue;
				}
				cr=(int)((fwf[mmm]-LeftVal)/Dalta*Grade);
				if(cr>Ps) cr=Ps;
				yy=y+(int)(sqrt(r*r-(xx-StartX-r)*(xx-StartX-r))*hh);
				MemDC.SetPixel (xx,yy-m_ClipRect.top,PALETTEINDEX(cr));
			}
			y++;
		}
	}
	else
	{
		while((y<rect.bottom)&&(sdep<edep))
		{
			sdep=this->YCoorToDepth (y+pDoc->m_WinOrg.y);
			pFIDIO->SeekToDepth((float)sdep);
			pFIDIO->Read(3,nps,fwf);
			for(i=0;i<nps;i++)
				fwf[i+nps]=fwf[i];
			if(m_StartPos>360) 
				m_StartPos-=360;
			for(xx=StartX; xx<EndX; xx++)
			{
				i=(int)((xx-StartX)*WW);
				mmm=i+m_StartPos*nps/360;
				if((fwf[mmm]==-9999.f)||(fwf[mmm]==-999.25f))
				{
					yy=y+(int)(sqrt(r*r-(xx-StartX-r)*(xx-StartX-r))*hh);
					MemDC.SetPixel (xx,yy-m_ClipRect.top,m_BkColor);
					continue;
				}
				cr=(int)((fwf[mmm]-LeftVal)/Dalta*Grade);
				if(cr>Ps) cr=Ps;
				yy=y+(int)(sqrt(r*r-(xx-StartX-r)*(xx-StartX-r))*hh);
				MemDC.SetPixel (xx,yy-m_ClipRect.top,PALETTEINDEX(cr));
			}
			y++;
		}
	}
	
	if(m_SDep ==DrawElipseDep)
	{
		CBrush Brush2(RGB(255,225,122));
		CPen Pen2;
		Pen2.CreatePen (PS_SOLID,1,RGB(255,225,122));
		CPen* OldPen=MemDC.SelectObject (&Pen2);
		CBrush* pOldBrush2 = MemDC.SelectObject(&Brush2); 
		int StartY = DepthToYCoor(DrawElipseDep)-pDoc->m_WinOrg.y-m_ClipRect.top+2;
		MemDC.Ellipse(StartX,StartY-(int)(r*sin(m_LookAngle*PI/180.0)),StartX+(int)(2.0*r),StartY+(int)(r*sin(m_LookAngle*PI/180.0)));
		CBrush Brush3;
		CTrackObj* pTrack=(CTrackObj*)this->GetParentObj ();
		if(pTrack!=NULL)
			Brush3.CreateSolidBrush (pTrack->m_TrackClientBrush .lbColor );
		else
			Brush3.CreateSolidBrush (RGB(255,255,255));
		MemDC.SelectObject(&Brush3);				
		MemDC.Ellipse(StartX+1,StartY-(int)(r*sin(m_LookAngle*PI/180.0))+1,StartX+(int)(2.0*r)-1,StartY+(int)(r*sin(m_LookAngle*PI/180.0))-1);
		MemDC.SelectObject(pOldBrush2);
		MemDC.SelectObject (OldPen);
	}	
	delete pFIDIO;
	delete fwf;delete iwf;delete cwf;
	pDC->BitBlt(rect.left, m_ClipRect.top+Offset, rect.Width(),m_ClipRect.Height()-Offset,&MemDC, 0, Offset,SRCCOPY);
	MemDC.SelectObject(pOldPalette);
	MemDC.SelectObject(pOldBitmap);
	MemDC.DeleteDC ();
	bmp.DeleteObject ();

	SetClipRect(pDC,rc);
	DrawPlane(pDC);
	SetClipRect(pDC,m_ClipRect);
}

int CImage3D::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CFIDDrawObj::HitTest(pView,point); 
	if ((nHandle >= 0) || (!m_Rect.PtInRect(point))) return nHandle;
	if(m_Rect.PtInRect(point)) 
		return 9;
	else return -1;
}

void CImage3D::CreatePoint()
{
	CRect rect=m_Rect;
	POSITION pos = m_PlanePoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_PlanePoint.GetNext(pos);
	m_PlanePoint.RemoveAll();
	m_bFlush=FALSE;
	if(DetectFileExist(m_PlaneName))
	{
		BOOL bDrawBox=FALSE;
		if(m_PlotContent.Find("绘截面图")>-1)
			bDrawBox=TRUE;
		
		DWORD Pos;
		CString Flag;COLORREF Color;
		float Dep,Dip,Dir;

		double amp=rect.Width()/3;
		double HVr=m_Bit/(m_Rect.Width()*0.254*2/3.);
		HVr=HVr/pDoc->m_DepthScale;
		int High;
		TCHAR *token, *end;
		TCHAR buffer[1024];
		CStdioFile File;
		if (File.Open(m_PlaneName, CFile::modeRead | CFile::typeText)) 
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
					High =(int)(amp*tan(Dip*PI/180.));
					rect.top=Pos-High;
					rect.bottom=Pos+High;
					if(Flag=="层理")     //绿色--层理
						Color=0x009300;
					else if(Flag=="充填缝")//青蓝色--充填缝
						Color=0x00FF00;
					else if(Flag=="张开缝")//蓝色--张开缝
						Color=0xFF0000;
					else if(Flag=="半充填缝")//天蓝色--半充填缝
						Color=0xFF8000;
					else if(Flag=="层间缝")//青色--半充填缝
						Color=0xFFFF00;
					else               //紫色--压裂缝
						Color=0x800080;
					CPlanePoint* pObj=new CPlanePoint(rect,Dep,Dip,Dir,Flag,m_PenWidth,Color,m_LookAngle,HVr,bDrawBox);
					m_PlanePoint.AddTail(pObj);
				}
			}
			File.Close();
		}
	}
}
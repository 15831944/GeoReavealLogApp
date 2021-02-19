/*************************************************************************
*   StratumDraw.cpp - implementation of the Stratum draw object class      *           
*************************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "TrackDraw.h"
#include "GeoFIDio.h"
#include "PickupHoleDraw.h"

//#include "MinFontDialog.h"
//#include "LineListPage.h"
//#include "PickupPage.h"


IMPLEMENT_SERIAL(CContour, CObject, 0)
CContour::CContour(BOOL bAdd)
{
	m_pointArray.SetSize(0,10);
	m_bAdd=bAdd;
	m_Radius=-999.25f;						//溶洞半径
	m_Round=-999.25f;
}
CContour::CContour()
{
	m_Radius=-999.25f;						//溶洞半径
	m_Round=-999.25f;
}
CContour::~CContour()
{
	m_pointArray.RemoveAll();
	m_pointArray.SetSize(0);	
}

/*
 *	说明：该函数的作用是计算颗粒的面积
 */
long CContour::ComputeArea()
{
	CRgn rgn;
	long number=0;
	if(m_pointArray.GetSize()<=1)
	{
		number=(long)m_pointArray.GetSize();
	}
	else
	{
		if(rgn.CreatePolygonRgn(&m_pointArray[0],(int)m_pointArray.GetSize(),ALTERNATE))
		{
//			TRACE("Cannot Create PolygonRgn\n");
			for (int y=m_rect.top; y<=m_rect.bottom; y++)//计算区域内部的面积
			{
				for (int x=m_rect.left; x<=m_rect.right; x++)
				{
					if(rgn.PtInRegion(CPoint(x,y)))
					{
						number++;
					}
				}
			}
			rgn.DeleteObject();
		}
		number=number+m_pointArray.GetSize()/2;
	}
	return number;	
}
/*
 *	判断此颗粒区域是否与矩形rt相交
 */
BOOL CContour::IsIntersect(CRect rt)
{
	BOOL bIntersect=FALSE;
	for(int i=0;i<m_pointArray.GetSize()&&!bIntersect;i++)
	{
		bIntersect=rt.PtInRect(CPoint(m_pointArray[i].x,m_pointArray[i].y));		
	}
	CRgn rgn;
	BOOL bSuccess=rgn.CreatePolygonRgn(&m_pointArray[0],(int)m_pointArray.GetSize(),ALTERNATE);
	for(int y=rt.top;bSuccess&&y<rt.bottom&&!bIntersect;y++)
	{
		for(int x=rt.left;bSuccess&&x<rt.right&&!bIntersect;x++)
		{
			bIntersect=rgn.PtInRegion(CPoint(x,y));
		}
	}
	if(bSuccess) rgn.DeleteObject();
	return bIntersect;
}
/*
 *	判断pt点是否在此颗粒区域内
 */
BOOL CContour::InRegion(CPoint pt)
{
	BOOL bIn=FALSE;
	for(int i=0;i<m_pointArray.GetSize()&&!bIn;i++)
	{
		if(pt==m_pointArray[i])
			bIn=TRUE;
	}
	if(!bIn)
	{
		CRgn rgn;
		BOOL bSuccess=rgn.CreatePolygonRgn(&m_pointArray[0],(int)m_pointArray.GetSize(),ALTERNATE);
		if(bSuccess)
		{
			bIn=rgn.PtInRegion(pt);	
			rgn.DeleteObject();
		}
	}
	return bIn;
}

void CContour::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
	{	// storing code	
		ar<<m_Radius;				
		ar<<m_rect;		            //存放外接此颗粒的矩形
		ar<<m_bAdd;                 //m_bAdd表示是否是增加颗粒
	}
	else
	{	// loading code
		ar>>m_Radius;
		ar>>m_rect;
		ar>>m_bAdd;
	}
	m_pointArray.Serialize(ar);    //存放颗粒轮廓的坐标
}

IMPLEMENT_SERIAL(CPickupHoleObj, CFIDDrawObj, 0)

CPickupHoleObj::CPickupHoleObj()
{
	m_DrawShape = pickupholeobj;
	m_FillColor = 0x000000;
	m_bFill=FALSE;
	bFlush=TRUE;
	m_bDrawRadius=FALSE;
}
    
CPickupHoleObj::CPickupHoleObj(CString szPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDoc)
			:CFIDDrawObj(szPathName,rect,pParentObj,pDoc)
{
	m_DrawShape = pickupholeobj;
	m_AliasName = "颗粒孔洞分析";
	m_ScaleFont = m_NameFont; 
	m_ScaleFont.lfHeight = -10;
	m_ScaleFont.lfWeight = 300;
	m_FillColor = 0x000000;
	m_bFill=0;
	m_PenWidth=1;
	m_PenColor=0xD4FF7F;
	m_LeftVal=0.f;
	m_RightVal=360.f;
	bFlush=TRUE;
	m_bDrawRadius=FALSE;
}

CPickupHoleObj::~CPickupHoleObj()
{
	////删除已有的颗粒或孔洞
	POSITION temp;
	POSITION pos=m_ContourList.GetHeadPosition();	 
	while(pos!=NULL)
	{
		temp=pos;
		m_ContourList.GetNext(pos);
		m_ContourList.RemoveAt(temp);
	}
}    

void CPickupHoleObj::Serialize( CArchive& ar )
{
	CFIDDrawObj::Serialize( ar );
	if(ar.IsStoring())
	{
		ar << m_PenWidth;
		ar << m_PenColor;
		ar << m_FillColor;
		ar << m_bFill;
		ar << m_bDrawRadius;
		SerializeLogFont(ar,m_ScaleFont);
	}
	else
	{
		ar >> m_PenWidth;
		ar >> m_PenColor;
		ar >> m_FillColor;
		ar >> m_bFill;
		if(pDoc->m_VersionNo>3.2f)
			ar >> m_bDrawRadius;
		SerializeLogFont(ar,m_ScaleFont);
		bFlush=TRUE;
	}		
}

void CPickupHoleObj::CreatePoint()
{
	bFlush=FALSE;
	char DataType[] = "IMAGEPRO-GHA    ";
	//创建文件
	
	CFile fp;
	if(!fp.Open(m_strPathName,CFile::modeRead))
	{
		fp.Close();
		return;
	}
	//创建一个档案对象
	CArchive ar(&fp,CArchive::load);
	char type[16];
	ar.Read(type,16);
	type[16] = '\0';
	float fVer;
	if(!strncmp(type,"IMAGEPRO-GHA    ",16))
	{
		//读取图像相关控制参数
		ar >> fVer;                          //文件格式版本号
		ar >> m_PickupFIDName;               //曲线全名
		ar >> m_PickupAliasName;             //曲线别名
		ar >> m_PickupBit;                   //钻头直径
		ar >> m_PickupHorScale;              //横向绘图比例
		ar >> m_PickupRect;                  //新建时的图像大小 
		ar >> m_PickupStDep;                 //新建时的起始深度
		ar >> m_PickupEnDep;                 //新建时的结束深度
		ar >> m_PickupVerScale;              //纵向绘图比例
		ar >> m_CoverageRate;                //自动拾取覆盖率
		////删除已有的颗粒或孔洞
		POSITION temp;
		POSITION pos=m_ContourList.GetHeadPosition();	 
		while(pos!=NULL)
		{
			temp=pos;
			m_ContourList.GetNext(pos);
			m_ContourList.RemoveAt(temp);
		}
		//读取文件中的颗粒或孔洞
		m_ContourList.Serialize(ar);
	}
	ar.Close();//关闭档案
	fp.Close();//关闭文件
}

void CPickupHoleObj::DrawClient(CDC* pDC)
{    
	if((m_pParentObj == NULL)||(m_ParentType != 0))
		CFIDDrawObj::DrawClient(pDC);
	if(bFlush) CreatePoint();
	//绘制自动识取的溶蚀孔洞边界
	if(!m_ContourList.IsEmpty())
	{
		CContour* pContour=NULL;
		POSITION pos=m_ContourList.GetHeadPosition();
		CPoint pt,OffsetPt;
		OffsetPt.x=m_Rect.left-m_PickupRect.left;
		OffsetPt.y=pDoc->DepthToYCoor(m_PickupStDep)-pDoc->DepthToYCoor(pDoc->m_PlotSdep);
		OffsetPt.y+=m_Rect.top-m_PickupRect.top;
		
		//纵横向不同绘图比例校正系数
		double Her,Ver;
		Her=m_Rect.Width()*1.0/m_PickupRect.Width();
		Ver=m_PickupVerScale/pDoc->m_DepthScale;

		CPen* pOldPen;
		CPen pen1(PS_SOLID,m_PenWidth,0xFFFFFF);//白色画笔
		CPen pen2(PS_SOLID,m_PenWidth,0x240000);//黑色画笔
		pOldPen=pDC->SelectObject(&pen1);

		CArray<CPoint,CPoint&>pointArray;
		CString str;
		int i,k,m,n;CRect rect;
		CBrush brush,*pOldBrush;
		pOldBrush =(CBrush*)pDC->SelectStockObject(NULL_BRUSH);
		//绘制立体效果
		if(m_bFill==1||m_bFill==2)
		{
			n=m_PenWidth;
			if(n<1) n=1;
			while(pos!=NULL)
			{	
				pContour=(CContour*)m_ContourList.GetNext(pos);
				rect = pContour->m_rect;
				rect.left=m_PickupRect.left+(int)((rect.left-m_PickupRect.left)*Her);
				rect.right=rect.left+(int)(rect.Width()*Her);
				rect.top=m_PickupRect.top+(int)((rect.top-m_PickupRect.top)*Ver);
				rect.bottom=rect.top+(int)(rect.Height()*Ver);
				rect.OffsetRect(OffsetPt);
				rect&=m_Rect;
				pDoc->LPtoPP(&rect);
		//		if(pDC->RectVisible(rect))
				{
					m=0;
					k=(int)pContour->m_pointArray.GetSize();
					if(k>1)
					{
						pointArray.SetSize(k,5);
						for(i=0;i<k;i++)
						{
							pt=pContour->m_pointArray[i];
							//进行纵横向不同绘图比例校正
							pt.x=m_PickupRect.left+(int)((pt.x-m_PickupRect.left)*Her);
							pt.y=m_PickupRect.top+(int)((pt.y-m_PickupRect.top)*Ver);
							//进行整体偏移
							pt+=OffsetPt;
							if(pt.y>m_Rect.top&&pt.y<m_Rect.bottom)
							{
								pDoc->LPtoPP(&pt);pt.Offset(-1*n,-1*n);
								pointArray[m]=pt;
								m++;
							}
						}
						if(m_bFill==1)
							pDC->SelectObject(&pen2);
						else
							pDC->SelectObject(&pen1);
						pDC->Polygon(&pointArray[0],m);
	
						for(i=0;i<m;i++)
							pointArray[i].Offset(2*n,2*n);
						if(m_bFill==1)
							pDC->SelectObject(&pen1);
						else
							pDC->SelectObject(&pen2);
						pDC->Polygon(&pointArray[0],m);

						pointArray.SetSize(0);	
					}
				}
			}
		}
		CPen pen(PS_SOLID,m_PenWidth,m_PenColor);
		pDC->SelectObject(&pen);
		pos=m_ContourList.GetHeadPosition();
		if(m_bFill!=0)
		{
			brush.CreateSolidBrush(m_FillColor);
			pDC->SelectObject(&brush);
		}
		else
			pDC->SelectStockObject(NULL_BRUSH);
		while(pos!=NULL)
		{	
			pContour=(CContour*)m_ContourList.GetNext(pos);
			rect = pContour->m_rect;
			rect.left=m_PickupRect.left+(int)((rect.left-m_PickupRect.left)*Her);
			rect.right=rect.left+(int)(rect.Width()*Her);
			rect.top=m_PickupRect.top+(int)((rect.top-m_PickupRect.top)*Ver);
			rect.bottom=rect.top+(int)(rect.Height()*Ver);
			rect.OffsetRect(OffsetPt);
			rect&=m_Rect;
			pDoc->LPtoPP(&rect);
		//	if(pDC->RectVisible(rect))
			{
				m=0;
				k=(int)pContour->m_pointArray.GetSize();
				if(k>1)
				{
					pointArray.SetSize(k,5);
					for(i=0;i<k;i++)
					{
						pt=pContour->m_pointArray[i];
						//进行纵横向不同绘图比例校正
						pt.x=m_PickupRect.left+(int)((pt.x-m_PickupRect.left)*Her);
						pt.y=m_PickupRect.top+(int)((pt.y-m_PickupRect.top)*Ver);
						//进行整体偏移
						pt+=OffsetPt;
						if(pt.y>m_Rect.top&&pt.y<m_Rect.bottom)
						{
							pDoc->LPtoPP(&pt);
							pointArray[m]=pt;
							m++;
						}
					}
					pDC->Polygon(&pointArray[0],m);

					if(m_bDrawRadius&&pContour->m_Radius>0.0025f)
					{
						str.Format("%.1f",pContour->m_Radius);
						pDC->TextOut(pointArray[0].x,pointArray[0].y,str);
					}
					pointArray.SetSize(0);	
				}
			}
		}

		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}
}

void CPickupHoleObj::DrawHead(CDC* pDC)
{			
	CRect rect = m_HeadRect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;
	char ss[10];      
	CFont* pOldFont;
	pDC->SetTextColor(m_NameColor);
	if(!m_bFill&&m_LeftVal!=m_RightVal)
	{
		CFont m_Font;
		m_Font.CreateFontIndirect(&m_ScaleFont);
		pOldFont = pDC->SelectObject(&m_Font);
		pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
		pDC->TextOut((rect.left + rect.right)/2, rect.bottom - 7, m_UnitText, (int)strlen(m_UnitText));

		pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
		dtos(m_LeftVal,ss);
		pDC->TextOut(rect.left+2,rect.bottom-7,ss,(int)strlen(ss));
		dtos(m_RightVal,ss);
		pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
		pDC->TextOut(rect.right-2,rect.bottom-7,ss,(int)strlen(ss));
		pDC->SelectObject(pOldFont);
	}

	CFont Font1;
	Font1.CreateFontIndirect(&m_NameFont);
    pOldFont = pDC->SelectObject(&Font1);
    pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
    CSize textsize=pDC->GetTextExtent(m_AliasName); 

	CPen pen(0,m_PenWidth,m_PenColor);
	CPen *Oldpen=(CPen *)pDC->SelectObject(&pen);
	if(m_bFill)
	{
		CBrush brush(m_FillColor);
		pDC->FillRect(&rect,&brush);
		CRect rect1;
		rect1.top = rect.top+(rect.Height()-textsize.cy)/2-3;
		rect1.bottom = rect.bottom-(rect.Height()-textsize.cy)/2+3;
		rect1.left = rect.left+(rect.Width()-textsize.cx)/2-10;
		rect1.right = rect.right-(rect.Width()-textsize.cx)/2+10;

		CBrush TextBoxBrush(0xFFFFFF);
		CBrush* pOldBrush=pDC->SelectObject (&TextBoxBrush);
		pDC->RoundRect(rect1.left,rect1.top,rect1.right,rect1.bottom ,7,7);
		pDC->SelectObject(pOldBrush);
	}
	else
	{
		pDC->MoveTo(rect.left,rect.bottom-5);
		pDC->LineTo(rect.right,rect.bottom-5);
	}
	pDC->SelectObject(Oldpen);
	pDC->TextOut((rect.left+rect.right)/2+1, (rect.top+rect.bottom+textsize.cy)/2, m_AliasName,(int)strlen(m_AliasName));
	pDC->SelectObject(pOldFont);
}                              

BOOL CPickupHoleObj::Properties(CView* pView)
{
	/*CString szHT;
	CPropertySheet Sheet("颗粒孔洞分析");
		
	CPickupPage PickupPage;
	PickupPage.m_FileName =m_strPathName;
	PickupPage.m_LeftValue=m_LeftVal;
	PickupPage.m_RightValue=m_RightVal;
	PickupPage.m_PenWidth=m_PenWidth;
	PickupPage.m_PenColor=m_PenColor;
	PickupPage.m_FillColor=m_FillColor;
	PickupPage.m_bFill=m_bFill;
	PickupPage.m_bDrawRadius=m_bDrawRadius;
	Sheet.AddPage(&PickupPage);

	CMinFontPage FontPage;
	TEXTFONT TextFont;
	szHT.LoadString(IDS_ALIASFONT);
	TextFont.m_ItemName = m_AliasName;
	szHT.ReleaseBuffer();
	TextFont.m_Text = m_AliasName;
	TextFont.m_ForeGround = m_NameColor;
	TextFont.lf = m_NameFont;
	FontPage.AddFont(&TextFont);
	TEXTFONT TextFont1;
	szHT.LoadString(IDS_ZERFONT);
	TextFont1.m_ItemName = "刻度字体";
	szHT.ReleaseBuffer();
	TextFont1.m_Text = "刻度字体";
	TextFont1.m_ForeGround = m_NameColor;
	TextFont1.lf = m_ScaleFont;
	FontPage.AddFont(&TextFont1);
	Sheet.AddPage(&FontPage);
	if(Sheet.DoModal() == IDOK)
	{
		FontPage.OnOK();
		m_AliasName=TextFont.m_Text;
		m_NameColor = TextFont.m_ForeGround;
		m_NameFont = TextFont.lf;
		m_NameFont.lfEscapement = 0;
		m_ScaleFont = TextFont1.lf;
		m_ScaleFont.lfEscapement = 0;
		PickupPage.OnOK();
		m_LeftVal=PickupPage.m_LeftValue;
		m_RightVal=PickupPage.m_RightValue;
		m_PenWidth=PickupPage.m_PenWidth;
		m_PenColor=PickupPage.m_PenColor;
		m_FillColor=PickupPage.m_FillColor;
		m_bDrawRadius=PickupPage.m_bDrawRadius;
		m_bFill=PickupPage.m_bFill;
		if(m_strPathName!=PickupPage.m_FileName)
		{
			m_strPathName=PickupPage.m_FileName;
			bFlush=TRUE;
		}
		pDoc->SetModifiedFlag();
		CRect SaveRect = m_Rect;
		SaveRect.top = m_HeadRect.top;
		if (m_ParentType != 0) 
			SaveRect.bottom = m_Rect.bottom;
		InvalidateRect(SaveRect);
		return TRUE;
	}*/
	return FALSE;
}

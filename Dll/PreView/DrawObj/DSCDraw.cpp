/******************************************************************
*   ResulDrw.cpp - implementation of the DSC draw object class *           
*******************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "GeoFIDio.h"
#include "TrackDraw.h"
#include "DSCDraw.h"
#include "DataBase.h"

//#include "StyleDialog.h"
//#include "MinFontDialog.h"
//#include "LineListPage.h"
//#include "FillListPage.h"
//#include "FIDBasePage.h"
//#include "DSCPage.h"
//#include "DSCParaDlg.h"


CDSCPoint::CDSCPoint(CRect rect,CDSCObj* pObj,CLogPlotDoc* pDocument)
{
	pDoc =pDocument;
	m_Rect = rect;
	m_pObj=pObj;
}

void CDSCPoint::DrawList(CDC *pDC)
{
	CString str;
	str.Format("测点深度:%.3f  ",m_Dep);
	CString msg=str;
//	if(m_Flux>0.f)
	{
		if(m_sDBFFluid.Find("上水流")>-1)
			str.Format("上水流量:%.2f",m_Flux);
		else
			str.Format("下水流量:%.2f",m_Flux);
		msg+=str;
	}
	CFont Font,*pOldFont;
	LOGFONT LogFont =m_pObj->m_LegendFont;
	Font.CreateFontIndirect(&LogFont);
	pOldFont = (CFont*)pDC->SelectObject(&Font);
	pDC->SetTextColor(m_pObj->m_LegendColor);
	pDC->SetTextAlign(TA_CENTER|TA_TOP);
	pDC->SetBkMode(TRANSPARENT);
	CSize size=pDC->GetTextExtent(msg);

	m_ListRect=m_GridRect;
	m_ListRect.left=m_GridRect.right-size.cx-10;
	m_ListRect.right=m_GridRect.right-5;
	m_ListRect.top=m_GridRect.top+5;
	m_ListRect.bottom=m_ListRect.top+size.cy+5;	
	pDC->TextOut((m_ListRect.left+m_ListRect.right)/2,m_ListRect.top+5,msg);
	pDC->SelectObject(pOldFont);
}

void CDSCPoint::DrawGrid(CDC *pDC)
{
	CRect rect=m_Rect;
	pDoc->LPtoPP(&rect);
	CBrush br1(0xffffff);
	m_GridRect=rect;
	m_GridRect.left=rect.left+40;
	m_GridRect.right=rect.right-5;
	m_GridRect.top =rect.top;
	m_GridRect.bottom -=30;
	

	CBrush *OldBrush=(CBrush *)pDC->SelectObject(&br1);
	CPen* pOldPen =(CPen*)pDC->SelectStockObject(NULL_PEN);
	pDC->Rectangle(rect);
	CPen pen;
	pen.CreatePen(0,2,RGB(0,0,0));
	pDC->SelectObject(&pen);
	pDC->Rectangle(m_GridRect);
	
	pen.DeleteObject();
	pen.CreatePen(0,1,m_pObj->m_Color);
	pDC->SelectObject(&pen);
	int i,x,y;

	//纵刻度线
	for(i=1;i<50;i++)
	{
		y=m_GridRect.bottom -i*m_GridRect.Height()/50 ;
		pDC->MoveTo (m_GridRect.left ,y);
		pDC->LineTo (m_GridRect.left-4,y);
	}
	
	//横刻度线
	{
		for(i=1;i<50;i++)
		{
			x=m_GridRect.left+i*m_GridRect.Width ()/50;
			pDC->MoveTo (x,m_GridRect.bottom );
			pDC->LineTo (x,m_GridRect.bottom+3);
		}
	}

	//绘X轴名称
	CSize size;
	CFont Font,*pOldFont;
	LOGFONT LogFont =m_pObj->m_AxiasTextFont;
	Font.CreateFontIndirect(&LogFont);
	pOldFont = (CFont*)pDC->SelectObject(&Font);
	pDC->SetTextColor(m_pObj->m_AxiasTextColor);
	pDC->SetTextAlign(TA_CENTER|TA_TOP);
	pDC->SetBkMode(TRANSPARENT);
	size=pDC->GetTextExtent(m_pObj->m_XText);
//	pDC->TextOut((m_GridRect.left+m_GridRect.right)/2,m_Rect.bottom-size.cy/2,m_pObj->m_XText);

	//绘Y轴名称
	Font.DeleteObject();
	LogFont.lfEscapement = -2700;
	Font.CreateFontIndirect(&LogFont);
	pDC->SelectObject(&Font);
	size=pDC->GetTextExtent(m_pObj->m_YText);
//	pDC->TextOut(m_Rect.left+size.cy/2,(m_GridRect.top+m_GridRect.bottom+size.cy)/2,m_pObj->m_YText);
	//设置刻度字体
	Font.DeleteObject();
	Font.CreateFontIndirect(&m_pObj->m_ScaleTextFont);
	pDC->SelectObject(&Font);
	pDC->SetTextColor(m_pObj->m_ScaleTextColor);
	size=pDC->GetTextExtent("D1");
	//绘谱线说明
	CString str;
	CPen Pen2;
	Pen2.CreatePen(PS_DOT,1,m_pObj->m_Color);
	pDC->SelectObject(&Pen2);
	//纵刻度值
	for(i=1;i<10;i++)
	{
		y=m_GridRect.bottom -i*m_GridRect.Height ()/10;
		pDC->MoveTo (m_GridRect.left-6,y);
		pDC->LineTo (m_GridRect.right,y);
		if((i+1)%2==0)
		{
			str.Format ("%g",m_pObj->m_YMin+i*(m_pObj->m_YMax-m_pObj->m_YMin)/10.f);
			size=pDC->GetTextExtent(str);
			pDC->TextOut (m_GridRect.left-size.cx,y-size.cy/2,str);
		}
	}

	//横刻度值
	{
		for(i=1;i<10;i++)
		{
			x=m_GridRect.left +i*m_GridRect.Width ()/10;
			pDC->MoveTo (x,m_GridRect.top );
			pDC->LineTo (x,m_GridRect.bottom+8);
			if((i+1)%2==0)
			{
				str.Format ("%g",m_pObj->m_XMin+i*(m_pObj->m_XMax-m_pObj->m_XMin)/10.f);
				size=pDC->GetTextExtent(str);
				pDC->TextOut (x-size.cx/2,m_GridRect.bottom+6,str);
			}
		}	
		x=m_GridRect.right-1;
		pDC->MoveTo (x,m_GridRect.bottom );
		pDC->LineTo (x,m_GridRect.bottom+6);	
	}
	pDC->SelectObject (OldBrush);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldFont);
}

BOOL CDSCPoint::InitDrawObj()
{
	m_Num=-9999;
	BOOL flag=FALSE;
	CGeoFIDIO* pFIDIO = new CGeoFIDIO(m_sCurveName);
	if(pFIDIO->Open(pDoc->m_szSever))
	{
		m_Rlev = pFIDIO->GetDepthLevel();
		m_StDep=pFIDIO->GetStartDepth();
		m_EnDep=pFIDIO->GetEndDepth();
		FIDCONTENT* Content = pFIDIO->GetContent(2);
		if(Content != NULL)
		{
			m_nps = Content->nps;
			m_Num=(int)((m_pObj->m_XMax-m_pObj->m_XMin)/m_Rlev);
			if(m_Num>1200) 
				m_Num=1200;
			flag=TRUE;
		}
	}
	delete pFIDIO;
	return flag;
}

void CDSCPoint::DrawLine(CDC *pDC)
{
	if(m_Num<1) return;
	char s[256],fn[256];
	strcpy(s,m_sCurveName);
	CGeoFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(fp.Open (fn,CFile::modeRead|CFile::typeBinary))
	{
		int i,j,k;
		float *m_pData=NULL;
		m_pData=new float[m_Num*4+10];
		unsigned long Block;
		int Num0=(int)((m_pObj->m_XMin-m_StDep)/m_Rlev);
		float data[4];
		for(i=0;i<m_Num;i++)
		{
			Block=(Num0+i)*m_nps*4;
			fp.Seek (Block,CFile::begin);
			fp.Read (data,16L);
			for(j=0;j<4;j++)
				m_pData[i*4+j]=data[j];
		}
		//五点钟形滤波         0.11(Ti-2+Ti+2)+0.24(Ti-1+Ti+1)+0.3Ti
		for(k=0;k<m_FilterNum;k++)
			for(j=0;j<4;j++)
				for(i=2;i<m_Num;i++)
					m_pData[j+i*4]=0.11f*(m_pData[j+(i-2)*4]+m_pData[j+(i+2)*4])
								+0.24f*(m_pData[j+(i-1)*4]+m_pData[j+(i+1)*4])+0.3f*m_pData[j+i*4];
	
		int x=m_GridRect.left;
		int y=m_GridRect.bottom;
		int w=m_GridRect.Width();
		int h=m_GridRect.Height();	
		CPoint pt;
		//绘谱线D1....
		CPen pen;	
		pen.CreatePen(0,1,m_pObj->m_ColorPen[0]);
		CPen* pOldPen =(CPen*)pDC->SelectObject(&pen);
		for(j=0;j<4;j++)
		{	
			if(!m_pObj->m_bD[j])
				continue;
			pt.x=x;
			pt.y=y-(int)(h*(m_pData[j]-m_pObj->m_YMin)/(m_pObj->m_YMax-m_pObj->m_YMin));
			if(pt.y<m_GridRect.top)
				pt.y=m_GridRect.top;
			if(pt.y>m_GridRect.bottom)
				pt.y=m_GridRect.bottom;
			pDC->MoveTo(pt);
			for(i=1;i<m_Num;i++)
			{
				pt.x=x+i*w/m_Num;
				pt.y=y-(int)(h*(m_pData[j+i*4]-m_pObj->m_YMin)/(m_pObj->m_YMax-m_pObj->m_YMin));
				if(pt.y<m_GridRect.top)
					pt.y=m_GridRect.top;
				if(pt.y>m_GridRect.bottom)
					pt.y=m_GridRect.bottom;
				pDC->LineTo(pt);
			}
			pen.DeleteObject();
			pen.CreatePen(0,1,m_pObj->m_ColorPen[j+1]);
			pDC->SelectObject(&pen);
		}
		
		if(m_DetectNo>0
			&&m_DetectNo<5
			&&m_Sttime>=m_pObj->m_XMin
			&&m_Sttime<m_pObj->m_XMax)
		{
			//加粗绘制积分曲线
			j=m_DetectNo-1;
			pen.DeleteObject();
			pen.CreatePen(0,2,m_pObj->m_ColorPen[j]);
			pDC->SelectObject(&pen);
			pt.x=x;
			pt.y=y-(int)(h*(m_pData[j]-m_pObj->m_YMin)/(m_pObj->m_YMax-m_pObj->m_YMin));
			if(pt.y<m_GridRect.top)
				pt.y=m_GridRect.top;
			if(pt.y>m_GridRect.bottom)
				pt.y=m_GridRect.bottom;
			pDC->MoveTo(pt);
			for(i=1;i<m_Num;i++)
			{
				pt.x=x+i*w/m_Num;
				pt.y=y-(int)(h*(m_pData[j+i*4]-m_pObj->m_YMin)/(m_pObj->m_YMax-m_pObj->m_YMin));
				if(pt.y<m_GridRect.top)
					pt.y=m_GridRect.top;
				if(pt.y>m_GridRect.bottom)
					pt.y=m_GridRect.bottom;
				pDC->LineTo(pt);
			}
			//绘制积分图形
			pen.DeleteObject();
			pen.CreatePen(0,1,m_pObj->m_ColorPen[j]);
			pDC->SelectObject(&pen);
			k=(int)((m_Sttime-m_pObj->m_XMin)/(m_pObj->m_XMax-m_pObj->m_XMin)*m_Num);
			int y0=y-(int)(h*(m_pData[j+k*4]-m_pObj->m_YMin)/(m_pObj->m_YMax-m_pObj->m_YMin));
			int x0=k*w/m_Num+2;
			for(k=x0;k<w;k+=2)
			{
				pt.x=x+k;
				i=(int)(k*m_Num*1./w);
				if(i>m_Num-1) break;
				pt.y=y-(int)(h*(m_pData[j+i*4]-m_pObj->m_YMin)/(m_pObj->m_YMax-m_pObj->m_YMin));
				if(pt.y<m_GridRect.top)
					pt.y=m_GridRect.top;
				if(pt.y>m_GridRect.bottom)
					pt.y=m_GridRect.bottom;
				if(pt.y>y0) break;
				pDC->MoveTo(pt.x,y0);
				pDC->LineTo(pt);
			}
		}
		pDC->SelectObject(pOldPen);
		delete m_pData;
	}
	fp.Close();
}

void CDSCPoint::Draw(CDC* pDC)
{
	CRect rect=m_Rect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) 
		return;	
	DrawGrid(pDC);
	DrawLine(pDC);
	DrawList(pDC);
}

IMPLEMENT_SERIAL(CDSCObj, CFIDDrawObj, 0)

CDSCObj::CDSCObj()
{
	m_DrawShape = DSCobj;
	bFlush = FALSE;
	m_Color=0xC0C0C0;
}

CDSCObj::CDSCObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDoc)
		:CFIDDrawObj(strPathName,rect,pParentObj,pDoc)
{
	m_DrawDirect = HORDIR;
	m_HeadRect = rect;
	m_HeadRect.bottom = m_HeadRect.top + 130;
	m_HeadRect.right = m_HeadRect.left + 140;
	m_Rect = m_HeadRect;
	m_Rect.top = m_HeadRect.bottom;
	m_DrawShape = DSCobj;
    m_NameFont.lfHeight = -20;
	m_AliasName = "氧活化谱线";
	bFlush = FALSE;
	m_FramePen.lopnWidth = CPoint(2,2); 
	m_FramePen.lopnColor = RGB(0,0x80,0); 
	m_FramePen.lopnStyle = 0;
	m_XMin=0.f;m_XMax=40.f;
	m_YMin=0.f;m_YMax=250.f;
	m_bD[0]=m_bD[1]=m_bD[2]=m_bD[3]=TRUE;

	m_XText = "时间(秒)"; m_YText = "计数(累加10次)";
	m_TitleTextColor = m_AxiasTextColor = m_ScaleTextColor =m_LegendColor=0x08000;
	m_ColorPen[0]=0x0000FF;m_ColorPen[1]= 0xFFFF00;m_ColorPen[2]= 0xFF0000;m_ColorPen[3]= 0x0;
	LOGFONT m_LogFont=
	{
		22,					    //lfHeight
		0,							//lfWidth
		0,							//lfEscapement
		0,							//lfOrientation
		FW_BOLD,					//lfWeight
		0,							//lfItalic
		0,							//lfUnderLine;
		0,							//lfStrikeOut;
		DEFAULT_CHARSET,			//lfCharSet
		OUT_TT_ONLY_PRECIS,			//lfOutPrecision
		CLIP_DEFAULT_PRECIS,		//lfClipPrecision
		DEFAULT_QUALITY,			//lfQuality
		VARIABLE_PITCH | FF_SWISS,	//lfPitchAndFamily
		"Times New Roman"	    	//lfFaceName
	}; 
	m_TitleTextFont =m_AxiasTextFont=m_ScaleTextFont=m_LegendFont=m_LogFont;
	m_TitleTextFont.lfHeight = 40;
	strcpy(m_ScaleTextFont.lfFaceName,"Times New Roman");
	m_ScaleTextFont.lfHeight = -14;

	m_LegendFont.lfWeight =FW_NORMAL;
	strcpy(m_LegendFont.lfFaceName,"Times New Roman");
	m_LegendFont.lfHeight =-14;
	m_Color=0xC0C0C0;
}

CDSCObj::~CDSCObj()
{
	POSITION pos = m_DSCPoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_DSCPoint.GetNext(pos);
	m_DSCPoint.RemoveAll();
}    

void CDSCObj::CreatePoint()
{
	CRect rect;
	rect.left = m_Rect.left;
	rect.right = m_Rect.right;
	int h=rect.Width()-45;
	h=h/2+30;
	POSITION pos = m_DSCPoint.GetHeadPosition();
	while (pos!= NULL) 
		delete m_DSCPoint.GetNext(pos);
	m_DSCPoint.RemoveAll();
	m_strPathName.MakeUpper();
	bFlush = FALSE;
	float dalta=(float)(YCoorToDepth(500+h)-YCoorToDepth(500));
	int xyz=m_strPathName.Find (".DBF");
	if((xyz>0)&&DetectFileExist(m_strPathName))
	{
		CString WellPath=m_strPathName.Left(m_strPathName.ReverseFind('\\'));
		WellPath=WellPath+"\\"+WellPath.Mid(WellPath.ReverseFind('#')+1);
		CString CurveName;
		CDbaseFile Base;
		if (Base.Open(m_strPathName) != DBASE_SUCCESS)
			return;
		char sData[256];
		// show all records
		for(int rc=Base.GetFirstRecord(); rc==DBASE_SUCCESS; rc=Base.GetNextRecord())
  		{
			// do not show deleted records
			if (Base.IsRecordDeleted())
				continue;
			float dep;
			if(Base.GetField("测点深度",sData))
				dep=(float)atof(sData);
			if(dep<pDoc->m_PlotSdep||dep>pDoc->m_PlotEdep)
				continue;
			rect.bottom = DepthToYCoor(dep)+30;
			rect.top=rect.bottom-h;

			CDSCPoint* pObj0=NULL;
			pos = m_DSCPoint.GetHeadPosition();
			while (pos != NULL) 
			{
				pObj0= (CDSCPoint*)m_DSCPoint.GetNext(pos);
				if((rect.top<pObj0->m_Rect.bottom)&&(rect.bottom>pObj0->m_Rect.bottom))
					rect.OffsetRect(0,pObj0->m_Rect.bottom-rect.top);
				if(abs(rect.top-pObj0->m_Rect.top)<2)
					rect.OffsetRect(0,pObj0->m_Rect.Height());
			}
			CDSCPoint* pObj=NULL;
			pObj = new CDSCPoint(rect,this,pDoc);
			pObj->m_Dep=dep;
			if(Base.GetField("井名",sData))
				pObj->m_sDBFWell=sData;
			pObj->m_sDBFWell.TrimLeft();
			pObj->m_sDBFWell.TrimRight();
			if(Base.GetField("测井日期",sData))
				pObj->m_sDBFDate=sData;
			pObj->m_sDBFDate.TrimLeft();
			pObj->m_sDBFDate.TrimRight();
			if(Base.GetField("水流方式",sData))
				pObj->m_sDBFFluid=sData;
			pObj->m_sDBFFluid.TrimLeft();
			pObj->m_sDBFFluid.TrimRight();

			if(Base.GetField("初始峰位",sData))
				pObj->m_Sttime=(float)atof(sData);
			if(Base.GetField("滤波器",sData))
				pObj->m_FilterNum=atoi(sData);
			if(Base.GetField("峰位",sData))
				pObj->m_Velocity=(float)atof(sData);
			if(Base.GetField("流量",sData))
				pObj->m_Flux=(float)atof(sData);
			if(Base.GetField("探头号",sData))
				pObj->m_DetectNo=atoi(sData);
			CurveName.Format("%.3f",dep);
			int ix=CurveName.ReverseFind('.');
			if(ix>-1) 
				CurveName.SetAt(ix,'_');
			if(pObj->m_sDBFFluid.Find("上水流")>-1)
				CurveName+="_U";
			else
				CurveName+="_D";
			pObj->m_sCurveName=WellPath+"."+CurveName;
			if(pObj->InitDrawObj())
				m_DSCPoint.AddTail(pObj);
		}
		Base.Close();
	}
}

void CDSCObj::Serialize(CArchive& ar)
{                      
	CFIDDrawObj::Serialize(ar);
	if (ar.IsStoring())
	{
		ar<<m_XMin;
		ar<<m_XMax;
		ar<<m_YMin;
		ar<<m_YMax;
		ar<<m_bD[0];
		ar<<m_bD[1];
		ar<<m_bD[2];
		ar<<m_bD[3];
		ar<<m_XText;
		ar<<m_YText;
		SerializeLogFont(ar,m_TitleTextFont);
		SerializeLogFont(ar,m_AxiasTextFont);
		SerializeLogFont(ar,m_ScaleTextFont);
		SerializeLogFont(ar,m_LegendFont);
		ar<<(LONG)m_TitleTextColor;
		ar<<(LONG)m_AxiasTextColor;
		ar<<(LONG)m_ScaleTextColor;
		ar<<(LONG)m_LegendColor;
		ar<<(LONG)m_ColorPen[0];
		ar<<(LONG)m_ColorPen[1];
		ar<<(LONG)m_ColorPen[2];
		ar<<(LONG)m_ColorPen[3];
	}
	else
	{
		long l;
		ar>>m_XMin;
		ar>>m_XMax;
		ar>>m_YMin;
		ar>>m_YMax;
		ar>>m_bD[0];
		ar>>m_bD[1];
		ar>>m_bD[2];
		ar>>m_bD[3];
		ar>>m_XText;
		ar>>m_YText;
		SerializeLogFont(ar,m_TitleTextFont);
		SerializeLogFont(ar,m_AxiasTextFont);
		SerializeLogFont(ar,m_ScaleTextFont);
		SerializeLogFont(ar,m_LegendFont);
		ar >> l;m_TitleTextColor = (COLORREF)l;
		ar >> l; m_AxiasTextColor= (COLORREF) l;
		ar >> l; m_ScaleTextColor= (COLORREF) l;
		ar >> l; m_LegendColor= (COLORREF) l;
		ar >> l; m_ColorPen[0]= (COLORREF) l;
		ar >> l; m_ColorPen[1]= (COLORREF) l;
		ar >> l; m_ColorPen[2]= (COLORREF) l;
		ar >> l; m_ColorPen[3]= (COLORREF) l;
		bFlush = TRUE;
	}
}

void CDSCObj::SetRect(const CRect& rect)
{
	CPoint delta;    
	CDSCPoint* pObj;
	CRect rect1;
	delta.x = rect.right - m_Rect.right;
	delta.y = rect.top - m_Rect.top;
	int cx =20;
	POSITION pos = m_DSCPoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDSCPoint*) m_DSCPoint.GetNext(pos);
		rect1 = pObj->GetRect() + delta;
		rect1.left = rect.left;
		rect1.right = rect.right;
		pObj->SetRect(rect1);
	}          
	CFIDDrawObj::SetRect(rect);
}       

BOOL CDSCObj::Properties(CView* pView)
{
	/*CString szHT;
	szHT.LoadString(IDS_DRAWINGPROPERTIES);
	CPropertySheet m_Sheet("氧活化谱线属性");
	CDSCPage m_DSCPage;
	m_DSCPage.m_FileName=m_strPathName;
	m_Sheet.AddPage(&m_DSCPage);
	CDSCParaDlg DSCParaDlg;
	DSCParaDlg.m_bD1=m_bD[0];
	DSCParaDlg.m_bD2=m_bD[1];
	DSCParaDlg.m_bD3=m_bD[2];
	DSCParaDlg.m_bD4=m_bD[3];
	DSCParaDlg.m_XMax=m_XMax;
	DSCParaDlg.m_XMin=m_XMin;
	DSCParaDlg.m_YMax=m_YMax;
	DSCParaDlg.m_YMin=m_YMin;
	m_Sheet.AddPage(&DSCParaDlg);

	CLineListPage m_LineList;
	CFillListPage m_FillList;
 	if(m_ParentType != 0)
	{
		szHT.LoadString(IDS_FRAMELINE);
		m_LineList.AddPen(&m_FramePen,szHT);
		szHT.LoadString(IDS_FRAMEFILL);
		m_FillList.AddFill(&m_Brush,szHT);
		m_Sheet.AddPage(&m_LineList);
		m_Sheet.AddPage(&m_FillList);
	}
	CMinFontPage m_FontPage;
	TEXTFONT m_TextFont[3];
	if(m_AliasName=="") m_AliasName="标题";
	m_TextFont[0].m_ItemName = m_AliasName;
	m_TextFont[0].m_Text = m_AliasName;
	m_TextFont[0].m_ForeGround = m_NameColor;
	m_TextFont[0].m_BackGround = RGB(255,255,255);
	m_TextFont[0].lf = m_NameFont;
	m_FontPage.AddFont(&m_TextFont[0]);

	m_TextFont[1].m_ItemName = "刻度字体";
	m_TextFont[1].m_Text = "12345";
	m_TextFont[1].m_ForeGround = m_ScaleTextColor;
	m_TextFont[1].m_BackGround = RGB(255,255,255);
	m_TextFont[1].lf = m_ScaleTextFont;
	m_FontPage.AddFont(&m_TextFont[1]);

	m_TextFont[2].m_ItemName = "注释字体";
	m_TextFont[2].m_Text = "12345";
	m_TextFont[2].m_ForeGround = m_LegendColor;
	m_TextFont[2].m_BackGround = RGB(255,255,255);
	m_TextFont[2].lf = m_LegendFont;
	m_FontPage.AddFont(&m_TextFont[2]);
	m_Sheet.AddPage(&m_FontPage);
	double m_PlotSdep1 = m_PlotSdep;
	double m_PlotEdep1 = m_PlotEdep;
	if (m_Sheet.DoModal() == IDOK)
	{
		m_FontPage.OnOK();
		m_AliasName=m_TextFont[0].m_Text ;
		m_NameColor = m_TextFont[0].m_ForeGround;
		m_NameFont = m_TextFont[0].lf;
		m_ScaleTextFont = m_TextFont[1].lf;
		m_ScaleTextColor = m_TextFont[1].m_ForeGround;
		m_LegendFont = m_TextFont[2].lf;
		m_LegendColor = m_TextFont[2].m_ForeGround;

		m_DSCPage.m_FileName.MakeUpper();
		bFlush=TRUE;
		m_strPathName = m_DSCPage.m_FileName;
		m_bD[0]=DSCParaDlg.m_bD1;
		m_bD[1]=DSCParaDlg.m_bD2;
		m_bD[2]=DSCParaDlg.m_bD3;
		m_bD[3]=DSCParaDlg.m_bD4;
		m_XMax=DSCParaDlg.m_XMax;
		m_XMin=DSCParaDlg.m_XMin;
		m_YMax=DSCParaDlg.m_YMax;
		m_YMin=DSCParaDlg.m_YMin;
		CRect SaveRect = m_Rect;
		SaveRect.top = m_HeadRect.top;
		if (m_ParentType != 0) 
			SaveRect.bottom = m_Rect.bottom;
		InvalidateRect(SaveRect);
		pDoc->SetModifiedFlag();
		return TRUE;
	}*/
	return FALSE;
}                

void CDSCObj::DrawHead(CDC* pDC)
{
	CRect rect = m_HeadRect;
	pDoc->LPtoPP(rect);
	if (pDC->RectVisible(rect))
	{
		CFIDDrawObj::DrawHead(pDC);
		pDC->SetTextColor(m_NameColor);
		pDC->SetBkMode(TRANSPARENT); 
		pDC->SetTextAlign(TA_CENTER | TA_TOP); 
		CFont m_Font1;
		m_Font1.CreateFontIndirect(&m_NameFont);
		CFont* pOldFont = pDC->SelectObject(&m_Font1);
   		CSize size = pDC->GetTextExtent("文本");
		pDC->TextOut((rect.left + rect.right)/2, rect.top + size.cy -5, m_AliasName,(int)strlen(m_AliasName));
		pDC->SelectObject(pOldFont);  
		
		CFont m_Font;
		m_Font.CreateFontIndirect(&m_ScaleTextFont);
	    pOldFont = pDC->SelectObject(&m_Font);
		pDC->SetTextAlign(TA_CENTER|TA_CENTER);
		int k=0;CString str;
		for(int j=0;j<4;j++)
		{
			if(m_bD[j])
			{		
				CPen pen;
				pDC->SetTextColor(m_ColorPen[j]); 
				pen.CreatePen (PS_SOLID,1,m_ColorPen[j]);
				str.Format("D%d",j+1);
				size = pDC->GetTextExtent(str); 
				pDC->TextOut(rect.right-30-size.cx/2,rect.bottom-20*k-15-size.cy/2,str);
				CPen* Oldpen=pDC->SelectObject (&pen);
				pDC->MoveTo (rect.left+30,rect.bottom - 20*k- 15);
				pDC->LineTo (rect.right-30-size.cx,rect.bottom - 20*k- 15);
				pDC->SelectObject (Oldpen);
				k++;
			}
		}
		pDC->SelectObject(pOldFont); 
	}
}   

void CDSCObj::DrawClient(CDC* pDC)
{
	CFIDDrawObj::DrawClient(pDC);
	CRect rect = m_Rect;
	pDoc->LPtoPP(&rect);
	if(!pDC->RectVisible(rect)) return;
	if(bFlush) CreatePoint();
	CDSCPoint* pObj;
	POSITION pos = m_DSCPoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDSCPoint*) m_DSCPoint.GetNext(pos);
		CRect rect = pObj->m_Rect;
		pDoc->LPtoPP(&rect);
		CRect rect1 =m_ClientRect;
		pDoc->LPtoPP(&rect1);
		rect|=rect1;
		if (pDC->RectVisible(rect))
			pObj->Draw(pDC);
	}
}

int CDSCObj::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CFIDDrawObj::HitTest(pView,point); 
	if ((nHandle >= 0) || (!m_Rect.PtInRect(point))) return nHandle;
	if(m_Rect.PtInRect(point)) 
		return 9;
	else return -1;
}

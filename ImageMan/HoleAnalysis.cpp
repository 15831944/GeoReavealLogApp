// HoleAnalysis.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "HoleAnalysis.h"
#include "FIDio.h"
#include "FitCircle.h"
#include "InputBox.h"
#include <math.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHoleAnalysis dialog


CHoleAnalysis::CHoleAnalysis(CWnd* pParent /*=NULL*/)
	: CDialog(CHoleAnalysis::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHoleAnalysis)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_MaxValue=0.f;
	m_MinValue=0.f;
	m_bHandMouse=FALSE;
	m_fHRad=-9999.f;
	m_fHAzi=-9999.f;
	m_fHRad1=-9999.f;
	m_fHAzi1=-9999.f;
	m_fHRad2=-9999.f;
	m_fHAzi2=-9999.f;
	m_fHRad3=-9999.f;
	m_fHAzi3=-9999.f;
	m_szUnit="";
	m_bBoxAnisys=TRUE;
	m_points=NULL;
}

void CHoleAnalysis::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHoleAnalysis)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHoleAnalysis, CDialog)
	//{{AFX_MSG_MAP(CHoleAnalysis)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHoleAnalysis message handlers

BOOL CHoleAnalysis::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	ASSERT((IDM_DLGBMP_CLIPBOARD & 0xFFF0) == IDM_DLGBMP_CLIPBOARD);
	ASSERT(IDM_DLGBMP_CLIPBOARD < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString str;
		pSysMenu->AppendMenu(MF_SEPARATOR);
		if(m_bBoxAnisys) str="¾Ö²¿¸¯Ê´";
		else str.LoadString(IDS_HOLE_KEYSEAT);
		if(!str.IsEmpty())
			pSysMenu->AppendMenu(MF_STRING, IDM_HOLE_KEYSEAT, str);
		if(m_bBoxAnisys) str="ÍÖÔ²±äÐÎ";
		else str.LoadString(IDS_HOLE_BREAKOUT);
		if(!str.IsEmpty())
			pSysMenu->AppendMenu(MF_STRING, IDM_HOLE_BREAKOUT, str);
		if(m_bBoxAnisys) str="¸¯Ê´ÆÆÁÑ";
		else str.LoadString(IDS_HOLE_SLIP);
		if(!str.IsEmpty())
			pSysMenu->AppendMenu(MF_STRING, IDM_HOLE_SLIP, str);
		str.LoadString(IDS_HOLE_CLEAR);
		if(!str.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_HOLE_CLEAR, str);
		}
		pSysMenu->AppendMenu(MF_STRING, IDM_HOLE_SCALE, "ÉèÖÃ×ø±ê¿Ì¶È");
		str.LoadString(IDS_DLGBMP_CLIPBOARD);
		if (!str.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_DLGBMP_CLIPBOARD, str);
		}	
	}

	m_szTyle="";
	m_MaxValue=-5.f;
	GetPoint();
	GetDevAzm();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHoleAnalysis::OnSysCommand(UINT nID, LPARAM lParam) 
{
	// TODO: Add your message handler code here and/or call default
	if (nID== IDM_DLGBMP_CLIPBOARD)
		SendToClip();
	else
	{
		if(nID== IDM_HOLE_KEYSEAT)
		{
			m_szTyle="¼ü²ÛÄ¥Ëð";
			SetWindowText ("¾®ÑÛÐÎ×´·ÖÎö");
		}
		else if(nID== IDM_HOLE_BREAKOUT)
		{
			m_szTyle="¾®ÑÛ±ÀÂä";
			SetWindowText ("¾®ÑÛÐÎ×´·ÖÎö");
		}
		else if(nID == IDM_HOLE_SLIP)
		{
			m_szTyle="¼ôÇÐ»¬¶¯";
			SetWindowText ("¾®ÑÛÐÎ×´·ÖÎö");
		}
		else if(nID == IDM_HOLE_SCALE)
		{
			SetWindowText ("¾®ÑÛÐÎ×´·ÖÎö");
			CInputBox InBox;
			InBox.m_Value=m_MaxValue;
			InBox.m_Title ="×ø±ê¿Ì¶È";
			if(InBox.DoModal ()==IDOK)
			{
				m_MaxValue=InBox.m_Value;
				GetPoint();
				Invalidate();
			}
		}
		else if(nID == IDM_HOLE_CLEAR)
		{
			ClearHoleMark();
			SetWindowText ("¾®ÑÛÐÎ×´·ÖÎö");
		}
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CHoleAnalysis::SendToClip()
{
	CClientDC dc(this);
	CRect EmfRect;
	GetClientRect(&EmfRect);	//»ñµÃ¿Í»§ÇøµÄ¾ØÐÎ´óÐ¡
	EmfRect.bottom-=1;
	EmfRect.right-=1;
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bm;
	CSize sz(EmfRect.Width(), EmfRect.Height());
	bm.CreateCompatibleBitmap(&dc, sz.cx, sz.cy);
	CBitmap * oldbm = memDC.SelectObject(&bm);
	DrawHole(&memDC,TRUE);
	if(OpenClipboard() )
    {
		::EmptyClipboard();
		::SetClipboardData(CF_BITMAP, bm.m_hObject);
		::CloseClipboard();
		AfxMessageBox("·¢ËÍBMPÍ¼Æ¬µ½¼ôÌù°å³É¹¦       ");
	}
	//»Ö¸´Ô­Ê¼»·¾³
	dc.DeleteDC ();
	memDC.SelectObject(oldbm);
	bm.DeleteObject (); 
}

void CHoleAnalysis::DrawHole(CDC *pDC,BOOL bBmp)
{
	CRect clientrect;
	GetClientRect(&clientrect);	//»ñµÃ¿Í»§ÇøµÄ¾ØÐÎ´óÐ¡	
	COLORREF Color=0x0;//0xB75B30;;
	CPen* pOldPen;
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(Color);
	CString str;

	CRect rectRib=clientrect;
	rectRib.left=45;
	rectRib.top=20;
	rectRib.bottom-=45;
	rectRib.right=rectRib.left+rectRib.Height ();
	//ºá¿Ì¶ÈÏ¸Ïß
	CPen m_Pen2(0,1,Color);
	pOldPen = pDC->SelectObject(&m_Pen2);
	CBrush *pOldBrush=NULL;
	if(bBmp)
	{
		clientrect.right -=1;
		clientrect.bottom -=1;
		CBrush brush(RGB(255,255,255));
		pOldBrush=pDC->SelectObject(&brush);
		pDC->Rectangle(&clientrect);
		pDC->SelectObject(pOldBrush);
	}
	pOldBrush=(CBrush *)pDC->SelectStockObject (NULL_BRUSH);
	pDC->Rectangle(&rectRib);
	int i,x,y;
	//ºá¿Ì¶ÈÖµ
	pDC->SetTextAlign(TA_RIGHT| TA_TOP);
	for(i=1;i<10;i++)
	{
		x=rectRib.left +i*rectRib.Width ()/10;
		if((i+1)%2==0)
		{
			if(i==5)
			{
				pDC->MoveTo (x,rectRib.top );
				pDC->LineTo (x,rectRib.bottom);
				pDC->MoveTo (x+1,rectRib.top );
				pDC->LineTo (x+1,rectRib.bottom);
			}
			else
			{
				pDC->MoveTo (x,rectRib.top-6 );
				pDC->LineTo (x,rectRib.bottom+6);
			}
		}
		else
		{
			pDC->MoveTo (x,rectRib.top );
			pDC->LineTo (x,rectRib.bottom);
		}
		y=rectRib.bottom -i*rectRib.Height()/10;
		if((i+1)%2==0)
		{
			if(i==5)
			{
				pDC->MoveTo (rectRib.left,y);
				pDC->LineTo (rectRib.right,y);
				pDC->MoveTo (rectRib.left,y+1);
				pDC->LineTo (rectRib.right,y+1);
			}
			else
			{
				pDC->MoveTo (rectRib.left-6,y);
				pDC->LineTo (rectRib.right+6,y);
			}
		}
		else
		{
			pDC->MoveTo (rectRib.left,y);
			pDC->LineTo (rectRib.right,y);	
		}		
		if((i+1)%2==0&&i!=5)
		{
			str.Format ("%g",m_MinValue+i*(m_MaxValue-m_MinValue)/10);
			pDC->TextOut (x+str.GetLength ()*4,rectRib.bottom+6,str);
			pDC->TextOut (rectRib.left-6,y-8,str);
		}
	}
	CRect rect;
	if(m_nNum>3)
	{
		CBrush br2(RGB(0,0,0));
		//»æÊý¾ÝµãÍ¼Àý
		rect.left =rectRib.right+50;
		rect.top =rectRib.bottom-45;
		rect.right =rect.left+4;
		rect.bottom =rect.top+4;
		pDC->FillRect(rect,&br2);
		
		//»æÄâºÏÔ²
		CPen Pen3(0,1,RGB(0,255,0));
		pDC->SelectObject(&Pen3);
		if(m_points!=NULL)
		{
			rect.left =(int)(m_fCenterX-m_fRadius);
			rect.top =(int)(m_fCenterY-m_fRadius);
			rect.right =(int)(m_fCenterX+m_fRadius);
			rect.bottom =(int)(m_fCenterY+m_fRadius);
			pDC->Ellipse(rect);
			//»æÄâºÏÔ²ÐÄ
			rect.left =(int)(m_fCenterX-3);
			rect.top =(int)(m_fCenterY-3);
			rect.right =(int)(m_fCenterX+3);
			rect.bottom =(int)(m_fCenterY+3);
			pDC->Ellipse(rect);
	
			//»æÊµÀý°ë¾¶Êý¾Ýµã
			for(i=0;i<m_nNum;i++)
			{
				rect.left =m_points[i].x-1;
				rect.top =m_points[i].y-1;
				rect.right =m_points[i].x+1;
				rect.bottom =m_points[i].y+1;
				pDC->FillRect(rect,&br2);
			}
		}
		//»æÄâºÏÔ²Í¼Àý
		pDC->MoveTo(rectRib.right+40,rectRib.bottom-20);
		pDC->LineTo(rectRib.right+70,rectRib.bottom-20);
		if(m_fHAzi3>=0.f)
		{
			//»æ¼ôÇÐ»¬¶¯ÄâºÏÔ²ÐÄ
			CPen Pen4(0,1,RGB(255,0,0));
			pDC->SelectObject(&Pen4);
			rect.left =(int)(m_fCenterX1-3);
			rect.top =(int)(m_fCenterY1-3);
			rect.right =(int)(m_fCenterX1+3);
			rect.bottom =(int)(m_fCenterY1+3);
			pDC->Ellipse(rect);
			//»æ¼ôÇÐ»¬¶¯ÄâºÏÔ²Í¼Àý
			pDC->MoveTo(rectRib.right+40,rectRib.bottom);
			pDC->LineTo(rectRib.right+70,rectRib.bottom);
			//»æ¼ôÇÐ»¬¶¯ÄâºÏÔ²
			rect.left =(int)(m_fCenterX1-m_fRadius1);
			rect.top =(int)(m_fCenterY1-m_fRadius1);
			rect.right =(int)(m_fCenterX1+m_fRadius1);
			rect.bottom =(int)(m_fCenterY1+m_fRadius1);
			CPoint Stpt,Enpt;
			Stpt.x=(int)m_fCenterX1+(int)(sin(6.2831853*m_Azi0/360.f)*m_fRadius1);
			Stpt.y=(int)m_fCenterY1-(int)(cos(6.2831853*m_Azi0/360.f)*m_fRadius1);
			
			Enpt.x=(int)m_fCenterX1+(int)(sin(6.2831853*m_Azi1/360.f)*m_fRadius1);
			Enpt.y=(int)m_fCenterY1-(int)(cos(6.2831853*m_Azi1/360.f)*m_fRadius1);
			if(m_Azi0>m_Azi1)
				pDC->Arc(rect,Stpt,Enpt);
			else
				pDC->Arc(rect,Enpt,Stpt);
		}
		//»æ¾®ÑÛ±äÐÎ·½Î»
		if(m_fHAzi1>=0.f||m_fHAzi2>=0.f||m_fHAzi3>=0.f)
			DrawArrow(pDC);
	}
	pDC->SetTextAlign(TA_LEFT| TA_TOP);
	str.Format ("%.1f m",m_Dep);
	pDC->TextOut (rectRib.right+110,rectRib.top,str);
	if(m_fDev>=0.f)
	{
		str.Format ("%.1f degrees",m_fDev);
		pDC->TextOut (rectRib.right+110,rectRib.top+20,str);
	}
	if(m_fDaz>=0.f)
	{
		str.Format ("%.1f degrees",m_fDaz);
		pDC->TextOut (rectRib.right+110,rectRib.top+40,str);
	}
	int yy=80;
	if(m_fHAzi1>=0.f)
	{
		str.Format ("%.1f degrees",m_fHAzi1);
		pDC->TextOut (rectRib.right+110,rectRib.top+yy,str);
		yy+=20;
		str.Format ("%.1f ",m_fHRad1);
		pDC->TextOut (rectRib.right+110,rectRib.top+yy,str+m_szUnit);
		yy+=20;
	}
	if(m_fHAzi2>=0.f)
	{
		str.Format ("%.1f degrees",m_fHAzi2);
		pDC->TextOut (rectRib.right+110,rectRib.top+yy,str);
		yy+=20;
		str.Format ("%.1f ",m_fHRad2);
		pDC->TextOut (rectRib.right+110,rectRib.top+yy,str+m_szUnit);
		yy+=20;
	}
	if(m_fHAzi3>=0.f)
	{
		str.Format ("%.1f degrees",m_fHAzi3);
		pDC->TextOut (rectRib.right+110,rectRib.top+yy,str);
		yy+=20;
		str.Format ("%.1f ",m_fHRad3);
		pDC->TextOut (rectRib.right+110,rectRib.top+yy,str+m_szUnit);
		yy+=20;
	}
	//ÄâºÏÔ²°ë¾¶
	str.Format ("= %.2f ",m_fRadius/m_fScale);
	pDC->TextOut (rectRib.right+185,rectRib.bottom-30,str+m_szUnit);
	//¼ôÇÐ»¬¶¯ÄâºÏÔ²°ë¾¶
	if(m_fHAzi3>=0.f)
	{
		str.Format ("= %.2f ",m_fRadius1/m_fScale);
		pDC->TextOut (rectRib.right+185,rectRib.bottom-10,str+m_szUnit);
	}
	CFont* pOldFont;
	CFont Font;
	Font.CreateFont(16, 0,0,0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET,
                             OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
                             DEFAULT_PITCH | FF_DONTCARE, "Times New Roman");
	pOldFont = pDC->SelectObject(&Font);

	str="¾®ÑÛ°ë¾¶";
	CSize size = pDC->GetTextExtent(str);
	pDC->TextOut ((rectRib.left+rectRib.right-size.cx)/2,rectRib.bottom+25 ,str);
	pDC->TextOut ((rectRib.left+rectRib.right)/2-8,rectRib.top-16,"±±");
	pDC->TextOut ((rectRib.left+rectRib.right)/2-8,rectRib.bottom +2,"ÄÏ");
	pDC->TextOut (rectRib.left-16,(rectRib.top+rectRib.bottom)/2-8,"Î÷");
	pDC->TextOut (rectRib.right+2,(rectRib.top+rectRib.bottom)/2-8,"¶«");
	
	pDC->TextOut (rectRib.right+40,rectRib.top,"Éî¶È");
	pDC->TextOut (rectRib.right+40,rectRib.top+20,"¾®Ð±");
	pDC->TextOut (rectRib.right+40,rectRib.top+40,"·½Î»");

	yy=80;//"¼ü²ÛÄ¥Ëð";"¾®ÑÛ±ÀÂä";"¼ôÇÐ»¬¶¯"
	CString strAn;
	if(m_fHAzi1>=0.f)
	{
		if(m_bBoxAnisys) strAn="¾Ö²¿¸¯Ê´";
		else             strAn="¼ü²ÛÄ¥Ëð";
		pDC->TextOut (rectRib.right+40,rectRib.top+yy,strAn);
		Arrow(pDC,CPoint(rectRib.right+50,rectRib.top+yy+30),
			CPoint(rectRib.right+70,rectRib.top+yy+30),0x0);
		yy+=40;
	}
	if(m_fHAzi2>=0.f)
	{
		if(m_bBoxAnisys) strAn="ÍÖÔ²±äÐÎ";
		else             strAn="¾®ÑÛ±ÀÂä";
		pDC->TextOut (rectRib.right+40,rectRib.top+yy,strAn);
		Arrow(pDC,CPoint(rectRib.right+50,rectRib.top+yy+30),
			CPoint(rectRib.right+70,rectRib.top+yy+30),RGB(0,255,0));
		yy+=40;
	}
	if(m_fHAzi3>=0.f)
	{
		if(m_bBoxAnisys) strAn="¸¯Ê´ÆÆÁÑ";
		else             strAn="¼ôÇÐ»¬¶¯";
		pDC->TextOut (rectRib.right+40,rectRib.top+yy,strAn);
		Arrow(pDC,CPoint(rectRib.right+50,rectRib.top+yy+30),
			CPoint(rectRib.right+70,rectRib.top+yy+30),RGB(255,0,0));
		yy+=40;
	}
	pDC->TextOut (rectRib.right+80,rectRib.bottom-50,"Êý¾Ýµã");
	pDC->TextOut (rectRib.right+80,rectRib.bottom-30,"ÄâºÏÔ²    °ë¾¶");
	if(m_fHAzi3>=0.f)
		pDC->TextOut (rectRib.right+80,rectRib.bottom-10,"¼ôÇÐ»¬¶¯ °ë¾¶");

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldFont);
	pDC->DeleteDC ();
}

void CHoleAnalysis::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	DrawHole(&dc);
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
}

BOOL CHoleAnalysis::GetPoint()
{
	float Offset=0.f;
	CFIDIO* pAZ1=new CFIDIO(m_sAZ1Name);
	if(pAZ1->Open())
	{
		pAZ1->SeekToDepth(m_Dep);
		pAZ1->Read(2,1,&Offset);
		Offset=Offset*3.1415926/180.f;
	}
	delete pAZ1;
	
	BOOL bBreak=FALSE;
	CFIDIO* PadI=new CFIDIO(m_szAradFileName);
	if(PadI->Open())
	{
		FIDCONTENT* Content = PadI->GetContent(3);
		int Num = Content->nps;
		if(Num<3||m_Dep<PadI->GetStartDepth()
			||m_Dep>PadI->GetEndDepth()) 
		{
			delete PadI;
			return FALSE;
		}		
		float *Value=new float[Num];
		if(m_points!=NULL)
			delete m_points;
		m_points=new CPoint[Num+10];
		
		PadI->SeekToDepth(m_Dep);
		PadI->Read(3,Num,Value);

		if(m_MaxValue<=0.f)
		{
			m_MaxValue=1.f;
			for(int i=0;i<Num;i++)
			{
				if(Value[i]<=0.f) continue;
				if(Value[i]>m_MaxValue) 
					m_MaxValue=Value[i];
			}
			int Max=(int)(m_MaxValue/5.f);
			float Max1=(float)Max*5.f;
			if(Max1<m_MaxValue)
				Max1+=5.f;
			Max1+=5.f;
			m_MaxValue=Max1;
		}
		m_MinValue=m_MaxValue*-1;
		CRect rectRib;
		GetClientRect(&rectRib);
		rectRib.left=45;
		rectRib.top=20;
		rectRib.bottom-=45;
		rectRib.right=rectRib.left+rectRib.Height ();

		CPoint pt;
		pt.x=(rectRib.left+rectRib.right)/2;
		pt.y=(rectRib.top +rectRib.bottom )/2;
		m_fScale=rectRib.Height ()/(m_MaxValue-m_MinValue);
		m_nNum=0;
		int nFitNum=0;
		CFitCircle FitCircle(Num);//×îÐ¡¶þ³Ë·¨ÄâºÏÔ²
		for(int i=0;i<Num;i++)
		{
			if(Value[i]>1.f)
			{
				m_points[m_nNum].x=(int)pt.x+(int)(sin(6.2831853*i/Num+Offset)*m_fScale*Value[i]);
				m_points[m_nNum].y=(int)pt.y-(int)(cos(6.2831853*i/Num+Offset)*m_fScale*Value[i]);
//				if(Value[i]<4.7f)
				{
					FitCircle.m_points[nFitNum].x=m_points[m_nNum].x;
					FitCircle.m_points[nFitNum].y=m_points[m_nNum].y;
					nFitNum++;
				}
				m_nNum++;
			}
		}
		delete Value;
		if(nFitNum>3)
		{
			FitCircle.m_nNum=nFitNum;
			FitCircle.DoCircleFit();
			m_fCenterX = FitCircle.m_fCenterX;
			m_fCenterY = FitCircle.m_fCenterY;
			m_fRadius = FitCircle.m_fRadius;
		}
	}
	else bBreak=TRUE;
	delete PadI;
	return !bBreak;
}

BOOL CHoleAnalysis::GetSlipPoint()
{
	CFIDIO* PadI=new CFIDIO(m_szAradFileName);
	if(PadI->Open())
	{
		FIDCONTENT* Content = PadI->GetContent(3);
		int Num = Content->nps;
		if(Num<3||
			m_Dep<PadI->GetStartDepth()||
			m_Dep>PadI->GetEndDepth()) 
		{
			delete PadI;
			return FALSE;
		}		
		float *Value=new float[Num];
		CFitCircle FitCircle(Num);//×îÐ¡¶þ³Ë·¨ÄâºÏÔ²
		PadI->SeekToDepth(m_Dep);
		PadI->Read(3,Num,Value);

		CRect rectRib;
		GetClientRect(&rectRib);
		rectRib.left=45;
		rectRib.top=20;
		rectRib.bottom-=45;
		rectRib.right=rectRib.left+rectRib.Height ();

		CPoint pt;
		pt.x=(rectRib.left+rectRib.right)/2;
		pt.y=(rectRib.top +rectRib.bottom )/2;
		int a0=(int)(m_Azi0/360.f*Num);
		int a1=(int)(m_Azi1/360.f*Num);
		int iSt,iEn;
		if(a0>a1)
		{
			iSt=a1;
			iEn=a0;
		}
		else
		{
			iSt=a0;
			iEn=a1;
		}
		int i;
		int nSlipNum=0;
		for(i=iSt;i<iEn;i++)
		{
			if(Value[i]>0.f)
			{
			//	if(Value[i]<4.7f)
				{
					FitCircle.m_points[nSlipNum].x=(int)pt.x+(int)(sin(6.2831853*i/Num)*m_fScale*Value[i]);
					FitCircle.m_points[nSlipNum].y=(int)pt.y-(int)(cos(6.2831853*i/Num)*m_fScale*Value[i]);
					nSlipNum++;
				}
			}
		}
		delete Value;
		if(nSlipNum>3)
		{
			FitCircle.m_nNum=nSlipNum;
			FitCircle.DoCircleFit();
			m_fCenterX1 = FitCircle.m_fCenterX;
			m_fCenterY1 = FitCircle.m_fCenterY;
			m_fRadius1 = FitCircle.m_fRadius;
		}
	}
	delete PadI;
	return TRUE;
}

BOOL CHoleAnalysis::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_points!=NULL) 
		delete m_points;
		
	return CDialog::DestroyWindow();
}

BOOL CHoleAnalysis::GetDevAzm()
{
	m_fDaz=-9999.f;
	m_fDev=-9999.f;
	CFIDIO* PadDev=new CFIDIO(m_sDEVName);
	if(PadDev->Open())
	{
		if(m_Dep<PadDev->GetStartDepth()||m_Dep>PadDev->GetEndDepth()) 
		{
			delete PadDev;
			return FALSE;
		}		
		PadDev->SeekToDepth(m_Dep);
		PadDev->Read(2,1,&m_fDev);
	}
	delete PadDev;
	CFIDIO* PadAzm=new CFIDIO(m_sDAZName);
	if(PadAzm->Open())
	{
		PadAzm->SeekToDepth(m_Dep);
		PadAzm->Read(2,1,&m_fDaz);
	}
	delete PadAzm;
	return TRUE;
}

void CHoleAnalysis::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnMouseMove(nFlags, point);
	if(m_bHandMouse)
	{
		CClientDC dc(this);
		int nSaveDC = dc.SaveDC();
		dc.SetROP2(R2_NOT);
		dc.SelectStockObject(NULL_BRUSH);
		if(m_szTyle!="¼ôÇÐ»¬¶¯")
			dc.Ellipse(m_HandRect);
		dc.MoveTo (m_OrDot);
		dc.LineTo (m_AziDot);
		int w=abs(m_OrDot.x-point.x);
		int h=abs(m_OrDot.y-point.y);
		int r=(int)sqrt(w*w*1.+h*h*1.);
		m_HandRect.left =m_OrDot.x-r;
		m_HandRect.top =m_OrDot.y-r;
		m_HandRect.right =m_OrDot.x+r;
		m_HandRect.bottom =m_OrDot.y+r;
		if(m_szTyle!="¼ôÇÐ»¬¶¯")
			dc.Ellipse(m_HandRect);
		m_AziDot=point;
		dc.MoveTo (m_OrDot);
		dc.LineTo (m_AziDot);
		dc.RestoreDC(nSaveDC);
	}
}

void CHoleAnalysis::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnLButtonUp(nFlags, point);
	if(m_bHandMouse)
	{
		m_bHandMouse=FALSE;
		int w=abs(m_OrDot.x-m_AziDot.x);
		int h=abs(m_OrDot.y-m_AziDot.y);
		double r=sqrt(w*w*1.+h*h*1.);
		m_fHRad=(float)((r-m_fRadius)/m_fScale);	
		if(m_szTyle=="¼ü²ÛÄ¥Ëð")
		{
			m_fHRad1=m_fHRad;
			m_fHAzi1=ComputeAzi(m_OrDot,m_AziDot);
		}
		else if(m_szTyle=="¾®ÑÛ±ÀÂä")
		{
			m_fHRad2=m_fHRad;
			m_fHAzi2=ComputeAzi(m_OrDot,m_AziDot);
		}
		else
		{
			CRect rectRib;
			GetClientRect(&rectRib);
			rectRib.left=45;
			rectRib.top=20;
			rectRib.bottom-=45;
			rectRib.right=rectRib.left+rectRib.Height ();

			CPoint pt;
			pt.x=(rectRib.left+rectRib.right)/2;
			pt.y=(rectRib.top +rectRib.bottom )/2;
			m_Azi1=ComputeAzi(pt,point);
			if(GetSlipPoint())
			{
				pt.x=(int)m_fCenterX1;
				pt.y=(int)m_fCenterY1;
				m_fHAzi3=ComputeAzi(m_OrDot,pt);
				w=abs((int)(m_fCenterX1-m_fCenterX));
				h=abs((int)(m_fCenterY1-m_fCenterY));
				m_fHRad3=(float)(sqrt(w*w*1.+h*h*1.)/m_fScale);
			}
		}
		this->Invalidate();
	}
}

void CHoleAnalysis::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnLButtonDown(nFlags, point);
	if(m_szTyle!="")
	{
		m_bHandMouse=TRUE;
		CRect rectRib;
		GetClientRect(&rectRib);
		rectRib.left=45;
		rectRib.top=20;
		rectRib.bottom-=45;
		rectRib.right=rectRib.left+rectRib.Height ();

		CPoint pt;
		pt.x=(rectRib.left+rectRib.right)/2;
		pt.y=(rectRib.top +rectRib.bottom )/2;
		m_OrDot=pt;
		CPoint pp;
		pp.x=(int)m_fCenterX;
		pp.y=(int)m_fCenterY;
		CRect rr;
		rr.left=pt.x-rectRib.Width()/4;
		rr.top=pt.y -rectRib.Height()/4;
		rr.right=pt.x+rectRib.Width()/4;
		rr.bottom=pt.y +rectRib.Height()/4;
		if(rr.PtInRect(pp))
			m_OrDot=pp;
		m_AziDot=m_OrDot;
		m_HandRect.left =0;
		m_HandRect.top =0;
		m_HandRect.right =0;
		m_HandRect.bottom =0;
		if(m_szTyle=="¼ôÇÐ»¬¶¯")
		{
			m_Azi0=ComputeAzi(pt,point);
			pt.x=(int)m_fCenterX+(int)(sin(6.2831853*m_Azi0/360.f)*m_fRadius);
			pt.y=(int)m_fCenterY-(int)(cos(6.2831853*m_Azi0/360.f)*m_fRadius);
			CClientDC dc(this);
			CPen pen(0,1,RGB(255,0,0));
			CPen *OldPen=(CPen *)dc.SelectObject (&pen);
			dc.MoveTo (m_OrDot);
			dc.LineTo (pt);
			dc.SelectObject (OldPen);	
		}
	}
}

void CHoleAnalysis::DrawArrow(CDC* pDC)
{
	// m_szTyle "¼ü²ÛÄ¥Ëð";"¾®ÑÛ±ÀÂä";"¼ôÇÐ»¬¶¯"
	int x,y,x1,y1;
	int len = 20;
	double dd;
	//"¼ü²ÛÄ¥Ëð"
	if(m_fHAzi1>0.f)
	{
		dd = m_fHAzi1 * 0.01745329;
		x=(int)(m_fCenterX+m_fRadius*sin(dd)*0.8);
		y=(int)(m_fCenterY-m_fRadius*cos(dd)*0.8);
		x1 = x + Round(len * sin(dd));
		y1 = y - Round(len * cos(dd));
		Arrow(pDC,CPoint(x,y),CPoint(x1,y1),0x0);
	}
	//"¾®ÑÛ±ÀÂä"
	if(m_fHAzi2>0.f)
	{
		dd = m_fHAzi2 * 0.01745329;
		x=(int)(m_fCenterX+m_fRadius*sin(dd)*0.8);
		y=(int)(m_fCenterY-m_fRadius*cos(dd)*0.8);
		x1 = x + Round(len * sin(dd));
		y1 = y - Round(len * cos(dd));
		Arrow(pDC,CPoint(x,y),CPoint(x1,y1),RGB(0,255,0));
		dd = (m_fHAzi2+180) * 0.01745329;
		x=(int)(m_fCenterX+m_fRadius*sin(dd)*0.8);
		y=(int)(m_fCenterY-m_fRadius*cos(dd)*0.8);
		x1 = x + Round(len * sin(dd));
		y1 = y - Round(len * cos(dd));
		Arrow(pDC,CPoint(x,y),CPoint(x1,y1),RGB(0,255,0));
	}
	//"¼ôÇÐ»¬¶¯"
	if(m_fHAzi3>0.f)
	{
		dd = m_fHAzi3 * 0.01745329;
		x=(int)(m_fCenterX+m_fRadius*sin(dd)*0.8);
		y=(int)(m_fCenterY-m_fRadius*cos(dd)*0.8);
		x1 = x + Round(len * sin(dd));
		y1 = y - Round(len * cos(dd));
		Arrow(pDC,CPoint(x,y),CPoint(x1,y1),RGB(255,0,0));
	}
}

void CHoleAnalysis::Arrow(CDC *pDC, CPoint m_One, CPoint m_Two,COLORREF Color,BOOL bSolid)
{
	double slopy , cosy , siny;
	double Par = 8.0;//length of Arrow (>)
	slopy = atan2(( m_One.y - m_Two.y )*1.,
		( m_One.x - m_Two.x )*1. );
	cosy = cos( slopy );
	siny = sin( slopy );

	CPen Pen1;
	Pen1.CreatePen(0,2,Color);
	CPen* pOldPen = pDC->SelectObject(&Pen1); 

	//draw a line between the 2 endpoint
	pDC->MoveTo( m_One );
	pDC->LineTo( m_Two );

	CPen Pen2;
	Pen2.CreatePen(0,1,Color);
	pDC->SelectObject(&Pen2); 
	
	if(!bSolid)                //»æ¿ÕÐÄ¼ýÍ·
	{
		pDC->MoveTo( m_Two );
		pDC->LineTo( m_Two.x + int( Par * cosy - ( Par / 2.0 * siny ) ),
			m_Two.y + int( Par * siny + ( Par / 2.0 * cosy ) ) );
		pDC->LineTo( m_Two.x + int( Par * cosy + Par / 2.0 * siny ),
			m_Two.y - int( Par / 2.0 * cosy - Par * siny ) );
		pDC->LineTo( m_Two );
	}
	else                       //»æÊµÐÄ¼ýÍ·
	{
		CBrush Brush(Color);
		CBrush* pOldBrush = pDC->SelectObject(&Brush);   
		POINT point[3]={{m_Two.x,m_Two.y},
						{m_Two.x + int( Par * cosy - ( Par / 2.0 * siny ))
						,m_Two.y + int( Par * siny + ( Par / 2.0 * cosy ))},
						{m_Two.x + int( Par * cosy + Par / 2.0 * siny )
						,m_Two.y - int( Par / 2.0 * cosy - Par * siny )}};
					pDC->Polygon(point,3);
		pDC->SelectObject(pOldBrush);
	}
	pDC->SelectObject(pOldPen);	
}

void CHoleAnalysis::ClearHoleMark()
{
	m_szTyle="";
	m_bHandMouse=FALSE;
	m_fHRad=-9999.f;
	m_fHAzi=-9999.f;
	m_fHRad1=-9999.f;
	m_fHAzi1=-9999.f;
	m_fHRad2=-9999.f;
	m_fHAzi2=-9999.f;
	m_fHRad3=-9999.f;
	m_fHAzi3=-9999.f;
	this->Invalidate();
}

float CHoleAnalysis::ComputeAzi(CPoint OrDot,CPoint Dot)
{
	int w=abs(OrDot.x-Dot.x);
	int h=abs(OrDot.y-Dot.y);
	double r=sqrt(w*w*1.+h*h*1.);
	float Azi=(float)(acos(w/r)*57.29577957855);
	if(Dot.x>OrDot.x&&Dot.y<OrDot.y)
	{		
		Azi=90.f-Azi;
	}
	if(Dot.x>OrDot.x&&Dot.y>OrDot.y)
	{
		Azi+=90.f;
	}
	if(Dot.x<OrDot.x&&Dot.y>OrDot.y)
	{
		Azi=270.f-Azi;
	}
	if(Dot.x<OrDot.x&&Dot.y<OrDot.y)
	{
		Azi+=270.f;
	}
	if(w==0&&Dot.y<OrDot.y)
		Azi=0.f;
	if(w==0&&Dot.y>OrDot.y)
		Azi=180.f;
	if(h==0&&Dot.x<OrDot.x)
		Azi=270.f;
	if(h==0&&Dot.x>OrDot.x)
		Azi=90.f;
	if(Azi>360) Azi-=360;
	if(Azi<0) Azi+=360;
	return Azi;
}

void CHoleAnalysis::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	m_grippie.OnParentSize();
	// TODO: Add your message handler code here
	CPaintDC dc(this); // device context for painting
	GetPoint();
	Invalidate();
}

int CHoleAnalysis::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_grippie.Create(this);
	return 0;
}
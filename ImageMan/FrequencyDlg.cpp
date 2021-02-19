// ImgFrequency.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "FrequencyDlg.h"
#include "FIDio.h"
#include "Gradient.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFrequencyDlg::CFrequencyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFrequencyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFrequencyDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_MinValue=0.f;
	m_MaxValue=100.f;
	m_ImageMin=-999.25f;
	m_ImageMax=-999.25f;

	m_LineLog=0;
	m_iThinScale=10;
	m_iThickScale=10;

	m_bBlack=FALSE;
	m_bThinDraw=TRUE;
	m_bDrawShape=FALSE;

	m_ShapeColor=0x40FF00;//0x0000FF;
	m_FreColor=0x008000;
	m_NumScale=100;
}


void CFrequencyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrequencyDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrequencyDlg, CDialog)
	//{{AFX_MSG_MAP(CFrequencyDlg)
	ON_WM_PAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrequencyDlg message handlers

BOOL CFrequencyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,WS_EX_TOOLWINDOW);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, 1000, "发送到剪贴板");
		pSysMenu->AppendMenu(MF_STRING, IDOK, "确认");
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFrequencyDlg::OnPaint() 
{
	// TODO: Add your message handler code here
	CPaintDC dc(this);
	if(m_Point>0)
		DrawFrequency(&dc);
	// Do not call CDialog::OnPaint() for painting messages
}

void CFrequencyDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CClientDC dc(this);
	if(m_Point>0) 
		DrawFrequency(&dc);
}

void CFrequencyDlg::DrawFrequency(CDC *pDC,BOOL bClip)
{
	if(m_Point<1) return;
	int i,j,k,Max=1;
	for(i=0;i<m_NumScale;i++)
		if(m_freq[i]>Max) 
			Max=m_freq[i];
	if(Max<=1) return;
	/*COLORREF reColor[6]={0xFFFF00,0x008000,0xFF0000,0x008CFF,0x00D7FF,0x0000FF};*/
	COLORREF reColor[3]={0xFF0000,0x00D7FF,0x0000FF};

	CRect clientrect;
	GetClientRect(&clientrect);	//获得客户区的矩形大小
	//if(bClip)
	//	clientrect.bottom-=40;
	//else
	//	clientrect.top+=40;

	CRect rect=clientrect;
	COLORREF gColor,sColor=0x0;
	if(m_bBlack)
		gColor=0xffffff;
	else
		gColor=0xF0F0F0;
	if(bClip)
		gColor=0xffffff;
	
	CRect rectRib=clientrect;
	rect.top =1;
	pDC->FillSolidRect(clientrect,gColor);

	rectRib.right -=50;
	rectRib.bottom -=20;

	CPen Pen1(0,1,sColor);
	CPen* pOldPen = pDC->SelectObject(&Pen1);

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(sColor);

	float fMax;	CString str="%";
	fMax=Max*100.f/m_Point;

	//纵刻度线
	pDC->MoveTo (rectRib.left,rectRib.top);
	pDC->LineTo (rectRib.left,rectRib.bottom );
	pDC->MoveTo (rectRib.right+2,rectRib.top);
	pDC->LineTo (rectRib.right+2,rectRib.bottom );
	pDC->TextOut (rectRib.right+8,rectRib.bottom-10,str);
	int y;	
	for(i=1;i<50;i++)
	{
		y=rectRib.bottom -i*rectRib.Height()/50 ;
		pDC->MoveTo (rectRib.right+2,y);
		pDC->LineTo (rectRib.right+5,y);
	}
	//纵刻度值
	for(i=0;i<11;i++)
	{
		y=rectRib.bottom -i*rectRib.Height ()/10;
		pDC->MoveTo (rectRib.right+2,y);
		pDC->LineTo (rectRib.right+8,y);
		if(i%2==0&&i>1&&i<10)
		{
			str.Format ("%d",i*10);
			pDC->TextOut (rectRib.right+8,y-8,str);
			if(m_bThinDraw)
			{
				pDC->MoveTo (rectRib.left,y);
				pDC->LineTo (rectRib.right+2,y);
			}
		}
	}

	//横刻度值
	int x;
	pDC->MoveTo (rectRib.left,rectRib.top );
	pDC->LineTo (rectRib.right+2,rectRib.top );
	pDC->MoveTo (rectRib.left,rectRib.bottom );
	pDC->LineTo (rectRib.right+2,rectRib.bottom );
	if(m_LineLog==0)
	{
		for(i=1;i<m_NumScale;i++)
		{
			x=rectRib.left+i*rectRib.Width ()/m_NumScale ;
			pDC->MoveTo (x,rectRib.bottom );
			pDC->LineTo (x,rectRib.bottom+3);
		}
		
		for(i=0;i<m_iThickScale;i++)
		{
			x=rectRib.left +i*rectRib.Width ()/m_iThickScale;
			pDC->MoveTo (x,rectRib.bottom );
			pDC->LineTo (x,rectRib.bottom+6);
			if((i+1)%2==0)
			{
				str.Format ("%.2f",m_MinValue+i*(m_MaxValue-m_MinValue)/m_iThickScale);
				pDC->TextOut (x-str.GetLength ()*4+4,rectRib.bottom+5,str);
				if(m_bThinDraw)//线性刻度
				{
					pDC->MoveTo (x,rectRib.top );
					pDC->LineTo (x,rectRib.bottom);
				}
			}
		}	
		x=rectRib.right-1;
		pDC->MoveTo (x,rectRib.bottom );
		pDC->LineTo (x,rectRib.bottom+6);	
	}
	else
	{
		int ZerY;
		int xinc = 1;
		double xv,xvl,xvr;
		if (m_MinValue < 0.0001f) m_MinValue = 0.0001f;
		if (m_MaxValue < 0.0001f) m_MaxValue = 0.0001f;
		xv = (int)log10(m_MinValue);
		xvl = log10(m_MinValue);	
		xvr = log10(m_MaxValue);
		if (xinc*(xvl-xv) < 0) 
			xv -= xinc;
		k = 1;   
		x = Round(rectRib.left + (xv - xvl) / (xvr - xvl) * rectRib.Width());
		while (x < rectRib.right)
		{            
			x = Round(rectRib.left + (xv - xvl) / (xvr - xvl) * rectRib.Width());
			if (x > rectRib.left)
			{
				if ((k == 1) || (k == 10))
				{
					ZerY =rectRib.bottom+6;
					str.Format ("%g",pow(10.,xv));
					pDC->TextOut (x-str.GetLength ()*4,rectRib.bottom+5,str);
					if(m_bThinDraw)
					{
						pDC->MoveTo (x,rectRib.bottom );
						pDC->LineTo (x,rectRib.top);
					}
				}
				else
				{
					ZerY = rectRib.bottom+3;
					pDC->SelectObject(&Pen1);
				}
				pDC->MoveTo (x,rectRib.bottom );
				pDC->LineTo (x,ZerY);
			}
			k += xinc;
			xv += (log10((double)k) - log10((double)k-1)) * xinc;
			if (k == 10) k = 1;					
		}
	}
	rect=rectRib;

	int wide=rectRib.Width ();
	int Height=rectRib.Height()-2;

	int cr,ii,li=0;
	
	COLORREF leftCr=GetSysColor(COLOR_3DLIGHT);
	COLORREF bottomCr=GetSysColor(COLOR_3DSHADOW);

	int maxNum=0;
	for(i=0;i<m_NumScale;i++)
		maxNum+=m_freq[i];
	int ptNum=0;

	x=0;
	COLORREF color=m_FreColor;

	Graphics *pic=NULL;
	pic=new Graphics(pDC->m_hDC);
	pic->SetSmoothingMode(SmoothingModeHighQuality);

	Color cr0(GetRValue(GetSysColor(COLOR_3DLIGHT)),GetGValue(GetSysColor(COLOR_3DLIGHT)),GetBValue(GetSysColor(COLOR_3DLIGHT)));
	Pen *pPen0 = new Pen(cr0,1.f);

	Color cr1(GetRValue(m_ShapeColor),GetGValue(m_ShapeColor),GetBValue(m_ShapeColor));
	Pen *pPen1 = new Pen(cr1,2.f);

	Color cr2(GetRValue(GetSysColor(COLOR_3DSHADOW)),GetGValue(GetSysColor(COLOR_3DSHADOW)),GetBValue(GetSysColor(COLOR_3DSHADOW)));
	Pen *pPen2 = new Pen(cr2,2.f);

	Point *ppt=new Point[100];
	ppt[0]=Point(rectRib.left,rectRib.bottom);
	int ptN=1;

	float MinValue=m_MinValue;
	float MaxValue=m_MaxValue;
	float ImageMin=m_ImageMin;
	float ImageMax=m_ImageMax;
	if(m_LineLog>0)
	{
		MinValue=log10(MinValue);
		MaxValue=log10(MaxValue);
		ImageMin=log10(ImageMin);
		ImageMax=log10(ImageMax);
	}
	x=1;
	for(i=0;i<wide; i+=8)
	{
		rect.left=i;
		rect.right = i+6;
		ii=(i+4)*m_NumScale/wide;
		if(ii>m_NumScale-1) 
			ii=m_NumScale-1;
		rect.top=rectRib.bottom -Height*m_freq[ii]/Max;
		//绘直方图
		if(rect.Height()>1)
		{
			if(m_ImageMax<0.f)
				color=reColor[0];
			else
			{
				cr=(int)((MinValue+i*((MaxValue-MinValue)/wide)-ImageMin)*3/(ImageMax-ImageMin));
				//if(m_LineLog==0)
				//	cr=(int)((m_MinValue+i*((m_MaxValue-m_MinValue)/wide)-m_ImageMin)*3/(m_ImageMax-m_ImageMin));
				//else
				//	cr=(int)((log10(m_MinValue)+i*((log10(m_MaxValue)-log10(m_MinValue))/wide)-log10(m_ImageMin))*3/(log10(m_ImageMax)-log10(m_ImageMin)));
				if(cr<0) cr=0;
				if(cr>2) cr=2;
				color=reColor[cr];
			}
			pDC->Draw3dRect(rect,leftCr,bottomCr);
			pDC->FillSolidRect(i+x,rect.top+x,rect.Width()-1-x,rect.Height()-1-x,color);
		}
		//绘累计频率线
		if(m_bDrawShape&&li!=ii)
		{
			for(j=li;j<ii;j++)
				ptNum+=m_freq[j];
			ppt[ptN]=Point((rect.left+rect.right)/2,rect.bottom-ptNum*Height/maxNum);
			ptN++;
			li=ii;
		}
	}
	if(m_bDrawShape)
	{
		for(j=0;j<ptN;j++)
			ppt[j].Y-=1;
		pic->DrawLines(pPen0,ppt,ptN);

		for(j=0;j<ptN;j++)
			ppt[j].Y+=2;
		pic->DrawLines(pPen2,ppt,ptN);

		for(j=0;j<ptN;j++)
			ppt[j].Y-=1;
		pic->DrawLines(pPen1,ppt,ptN);
	}
	if(!bClip&&m_ImageMax>0.f)
	{
		ptN=5;
		ppt[0]=Point((int)((ImageMin-MinValue)/(MaxValue-MinValue)*wide),rectRib.bottom-4);
		ppt[1]=Point((int)((ImageMin-MinValue)/(MaxValue-MinValue)*wide),rectRib.bottom -Height/2);
		ppt[2]=Point((int)((ImageMax-MinValue)/(MaxValue-MinValue)*wide),rectRib.bottom -Height/2);
		ppt[3]=Point((int)((ImageMax-MinValue)/(MaxValue-MinValue)*wide),rectRib.bottom-4);
		ppt[4]=ppt[0];
		pic->DrawLines(pPen0,ppt,ptN);
		for(j=0;j<ptN;j++)
			ppt[j].Y+=2;
		pic->DrawLines(pPen2,ppt,ptN);
		for(j=0;j<ptN;j++)
			ppt[j].Y-=1;
		pic->DrawLines(pPen1,ppt,ptN);
	}
	delete ppt;
	pic->ReleaseHDC(pDC->m_hDC);
	delete pPen0;delete pPen1;delete pPen2;
	delete pic;

	pDC->SelectObject(pOldPen);
}

void CFrequencyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(nID == 1000)
		OnSendToClip();
	else if(nID == IDOK)
		OnOK();
	else
		CDialog::OnSysCommand(nID, lParam);
}

void CFrequencyDlg::OnSendToClip()
{
	CClientDC dc(this);
	CRect EmfRect;
	GetClientRect(&EmfRect);	//获得客户区的矩形大小
	//EmfRect.bottom-=40;

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bm;
	CSize sz(EmfRect.Width(), EmfRect.Height());
	bm.CreateCompatibleBitmap(&dc, sz.cx, sz.cy);
	CBitmap * oldbm = memDC.SelectObject(&bm);
	memDC.FillSolidRect(EmfRect,0xffffff);

	if(m_Point>0) 
		DrawFrequency(&memDC,TRUE);
	if(OpenClipboard() )
    {
		::EmptyClipboard();
		::SetClipboardData(CF_BITMAP, bm.m_hObject);
		::CloseClipboard();
	}
	//恢复原始环境
	dc.DeleteDC ();
	memDC.SelectObject(oldbm);
	bm.DeleteObject ();
}

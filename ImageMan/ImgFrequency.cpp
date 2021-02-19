// ImgFrequency.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "ImgFrequency.h"
#include "FIDio.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImgFrequency dialog


CImgFrequency::CImgFrequency(CWnd* pParent /*=NULL*/)
	: CDialog(CImgFrequency::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImgFrequency)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bColor=TRUE;
	m_Palsize=64;
	m_MinValue=0.f;
	m_MaxValue=0.f;
	m_bSelect=FALSE;
}


void CImgFrequency::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImgFrequency)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImgFrequency, CDialog)
	//{{AFX_MSG_MAP(CImgFrequency)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImgFrequency message handlers

void CImgFrequency::CreateImagePalette()
{
	m_Palette.DeleteObject();
	LPLOGPALETTE pLogPal = (LPLOGPALETTE) new BYTE[sizeof(LOGPALETTE)+m_Palsize * sizeof(PALETTEENTRY)];
	pLogPal->palVersion = 0x300; /* bloody mysterious */
	pLogPal->palNumEntries = m_Palsize;
	int i;	
	//斯仑贝谢配色方案
	COLORREF Color0[64] = {
	0xFFFFFF,0xE7FFFF,0xC7FFFF,0xAFFFFF,0x97F7FF,0x77EFFF,0x5FE7FF,0x57DFFF,
	0x5FD7FF,0x5FCFFF,0x5FC7FF,0x5FC7FF,0x67BFFF,0x67BFFF,0x3FB7FF,0x07B7FF,
	0x00AFFF,0x00AFFF,0x00A7FF,0x00A7FF,0x00A7FF,0x009FFF,0x009FFF,0x0097FF,
	0x0097F7,0x0097F7,0x008FEF,0x008FEF,0x0087E7,0x0087E7,0x0087DF,0x007FDF,
	0x007FD7,0x0077D7,0x0077CF,0x0077CF,0x006FC7,0x006FC7,0x0067BF,0x0067BF,
	0x0067B7,0x005FB7,0x005FAF,0x0057AF,0x0057A7,0x0057A7,0x004F9F,0x004F9F,
	0x004797,0x004797,0x003F8F,0x003F8F,0x003F87,0x00377F,0x00377F,0x002F77,
	0x002F6F,0x002767,0x001F5F,0x001757,0x00174F,0x000F47,0x000737,0x00002F};
	if(m_bColor)
	{
		for (i=0; i<64; i++) 
		{
			pLogPal->palPalEntry[i].peRed = GetRValue(Color0[i]);
			pLogPal->palPalEntry[i].peGreen = GetGValue(Color0[i]);
			pLogPal->palPalEntry[i].peBlue =  GetBValue(Color0[i]);
			pLogPal->palPalEntry[i].peFlags = PC_RESERVED; 
		}
	}
	else
	{
		for (i=0; i<64; i++) 
		{
			pLogPal->palPalEntry[i].peRed = GetRValue(Color0[63-i]);
			pLogPal->palPalEntry[i].peGreen = GetGValue(Color0[63-i]);
			pLogPal->palPalEntry[i].peBlue =  GetBValue(Color0[63-i]);
			pLogPal->palPalEntry[i].peFlags = PC_RESERVED; 
		}
	}
	m_Palette.CreatePalette(pLogPal);
	delete [] (BYTE *)pLogPal;
}

BOOL CImgFrequency::OnInitDialog() 
{
	CDialog::OnInitDialog();
	ASSERT((IDM_DLGBMP_CLIPBOARD & 0xFFF0) == IDM_DLGBMP_CLIPBOARD);
	ASSERT(IDM_DLGBMP_CLIPBOARD < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_DLGBMP_CLIPBOARD);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_DLGBMP_CLIPBOARD, strAboutMenu);
		}
	}
	// TODO: Add extra initialization here
	m_MaxValue0=m_MaxValue;
	m_MinValue0=m_MinValue;
	CreateImagePalette();
	m_bDraw=GetFrequency();
	m_bSelect=FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CImgFrequency::DrawRibbon(CDC *pDC)
{
	CRect clientrect;
	GetClientRect(&clientrect);	//获得客户区的矩形大小
	
	CRect rectRib=clientrect;
	rectRib.bottom=rectRib.top+15;
	CRect rect=rectRib;
	CPalette* pOldPalette = (CPalette*)pDC->SelectPalette(&m_Palette, FALSE);
	pDC->RealizePalette();
	
	for (int i=0; i<m_Palsize; i++)
	{
		rect.right = rectRib.left+(i+1)*rectRib.Width()/m_Palsize+2;
		CBrush br(PALETTEINDEX(i));
		pDC->FillRect(rect,&br);
		rect.left = rect.right;
	}
	pDC->SelectObject(pOldPalette);
}

void CImgFrequency::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	DrawRibbon(&dc);
	if(m_bDraw) DrawFrequency(&dc);
	// Do not call CDialog::OnPaint() for painting messages
}

void CImgFrequency::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CPaintDC dc(this);
	DrawRibbon(&dc);
	if(m_bDraw) DrawFrequency(&dc);
}

void CImgFrequency::DrawFrequency(CDC *pDC)
{
	int i,Max=1;
	for(i=0;i<300;i++)
		if(m_freq[i]>Max) 
			Max=m_freq[i];
	if(Max<=1) 
		return;	
	CRect clientrect;
	GetClientRect(&clientrect);	//获得客户区的矩形大小	
	CRect rect=clientrect;
	rect.top =15;
	CBrush br1(0xB75B30);
	pDC->FillRect(rect,&br1);

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,255));
	CString str;
	str.Format ("<-%.1f",m_LitMin);
	pDC->TextOut (2,17,str);
	str.Format ("%.1f->",m_MorMax);
	pDC->TextOut (rect.right -str.GetLength ()*8+8,17,str);

	CRect rectRib=clientrect;
	rectRib.top=35;
	rectRib.bottom-=20;
	//横刻度线
	pDC->MoveTo (rectRib.left ,rectRib.bottom );
	pDC->LineTo (rectRib.right ,rectRib.bottom );
	int x;
	for(i=1;i<128;i++)
	{
		x=rectRib.left+i*rectRib.Width ()/128 ;
		pDC->MoveTo (x,rectRib.bottom );
		pDC->LineTo (x,rectRib.bottom+3);
	}
	CPen m_Pen2(0,1,RGB(255,255,255));
	CPen* pOldPen = pDC->SelectObject(&m_Pen2);
	//横刻度值
	for(i=0;i<16;i++)
	{
		x=rectRib.left +i*rectRib.Width ()/16;
		pDC->MoveTo (x,rectRib.bottom );
		pDC->LineTo (x,rectRib.bottom+6);
		if((i+1)%2==0)
		{
			str.Format ("%g",m_MinValue+i*(m_MaxValue-m_MinValue)/16.f);
			pDC->TextOut (x-str.GetLength ()*4+2,rectRib.bottom+5,str);
		}
	}
	x=rectRib.right-1;
	pDC->MoveTo (x,rectRib.bottom);
	pDC->LineTo (x,rectRib.bottom+6);	
	pDC->SelectObject(pOldPen);

	rect=rectRib;
	CPalette* pOldPalette = (CPalette*)pDC->SelectPalette(&m_Palette, FALSE);
	pDC->RealizePalette();
	int wide=rectRib.Width ();
	int Height=rectRib.Height ();
	int Ps=m_Palsize-1;
	int cr,ii;
	for (i=0;i<wide;i++)
	{
		rect.left=i;
		rect.right = i+1;
		ii=i*300/wide;
		if(ii>299) ii=299;
		rect.top=rectRib.bottom -Height*m_freq[ii]/Max;
		cr=i*m_Palsize/wide;
		if(cr>Ps) cr=Ps;
		CBrush br(PALETTEINDEX(cr));
		pDC->FillRect(rect,&br);		
	}
	pDC->SelectObject(pOldPalette);
}

BOOL CImgFrequency::GetFrequency()
{
	if(fabs(m_MinValue-m_MaxValue)<5.f) 
		return FALSE;
	CFIDIO* pFIDR = NULL;
	pFIDR = new CFIDIO(m_WellName+"."+m_CurveName);
	if(!pFIDR->Open())
	{
		AfxMessageBox(m_WellName+"."+m_CurveName+"打开失败    ");
		delete pFIDR;return FALSE;
	}
	float Rlev=m_Rlev;
	float Sdep = m_ProStDep;
	float Edep = m_ProEnDep;	
	int dim=pFIDR->GetDimenSion();
	FIDCONTENT* Content;
	Content = pFIDR->GetContent(dim);
	if (Content == NULL)
	{
		delete pFIDR; return FALSE;
	}
	BeginWaitCursor();
	int nps= Content->nps;
	short* ibuf = NULL;
	float* Value = NULL;	
	char* cbuf = NULL;
	Value = new float[nps];
	if(Content->type != REPR_FLOAT)
	{
		if(Content->type == REPR_CHAR) 
			cbuf = new char[nps];
		else ibuf = new short[nps];
	}
	int i,m;
	float fdata,dep;
	fdata=m_MaxValue-m_MinValue;
	for(i=0;i<300;i++)
		m_freq[i]=0;
	int MorMax=0,LitMin=0;
	dep=Sdep;
	DWORD Point=0;
	while(dep<Edep)
	{
		pFIDR->SeekToDepth(dep);
		if (Content->type == REPR_FLOAT)
			pFIDR->Read(dim,nps,Value);
		else if (Content->type == REPR_CHAR)
		{
			pFIDR->Read(dim,nps,cbuf);
			for(i=0;i<nps;i++)
				Value[i]=(float)cbuf[i];
		}
		else 
		{
			pFIDR->Read(dim,nps,ibuf);
			for(i=0;i<nps;i++)
				Value[i]=(float)ibuf[i];
		}
		dep+=Rlev;
		for(i=0;i<nps;i++)
		{
			if((Value[i]!=-9999.f)&&(Value[i]!=-999.f))
			{
				m=(int)((Value[i]-m_MinValue)/fdata*300.f);
				if(m>300)
					MorMax++;
				else if(m<0)
					LitMin++;
				else
					m_freq[m]++;
				Point++;
			}
		}
	}
	if(Point>0)
	{
		m_MorMax=MorMax*100.f/Point;
		m_LitMin=LitMin*100.f/Point;
	}
	else
	{
		m_MorMax=0.f;
		m_LitMin=0.f;
	}
	delete pFIDR;delete Value;
	if (cbuf != NULL)	delete cbuf;
	if (ibuf != NULL)	delete ibuf;
	EndWaitCursor();
	return TRUE;
}

void CImgFrequency::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnLButtonDown(nFlags, point);	
	m_bSelect=TRUE;
	m_rectselect.left=point.x;
	m_rectselect.top=point.y;
	m_rectselect.right=point.x;
	m_rectselect.bottom=point.y;
	SetCapture();
}

void CImgFrequency::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnLButtonUp(nFlags, point);
	if(m_bSelect)
	{	
		ReleaseCapture();
		m_rectselect.right=point.x;
		m_rectselect.bottom=point.y;
		
		CClientDC dc(this);
		dc.SetROP2(R2_NOT);
		dc.SelectStockObject(NULL_BRUSH);
		dc.Rectangle(m_rectselect);
		
		CRect clientrect;
		GetClientRect(&clientrect);
		float fdata=m_MaxValue-m_MinValue;
		int r,l;
		if(m_rectselect.right>m_rectselect.left)
		{
			r=m_rectselect.right;
			l=m_rectselect.left;
		}
		else
		{
			r=m_rectselect.left;
			l=m_rectselect.right;
		}
		if(r-l>20)
		{
			m_MaxValue=m_MinValue+r*fdata/clientrect.Width ();
			m_MinValue=m_MinValue+l*fdata/clientrect.Width ();
			CClientDC dc(this);
			m_bDraw=GetFrequency();
			if(m_bDraw) DrawFrequency(&dc);
		}
		m_bSelect=FALSE;
	}
}

void CImgFrequency::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnMouseMove(nFlags, point);
	if(m_bSelect)
	{
		CClientDC dc(this);
		dc.SetROP2(R2_NOT);
		dc.SelectStockObject(NULL_BRUSH);
		dc.Rectangle(m_rectselect);
		m_rectselect.right=point.x;
		m_rectselect.bottom=point.y;
		dc.Rectangle(m_rectselect);
	}
}

void CImgFrequency::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnLButtonDblClk(nFlags, point);
	m_MaxValue=m_MaxValue0;
	m_MinValue=m_MinValue0;
	m_bDraw=GetFrequency();
	if(m_bDraw) 
	{
		CClientDC dc(this);
		DrawFrequency(&dc);
	}
}

void CImgFrequency::OnSysCommand(UINT nID, LPARAM lParam) 
{
	// TODO: Add your message handler code here and/or call default
// TODO: Add your message handler code here and/or call default
	if ((nID & 0xFFF0) == IDM_DLGBMP_CLIPBOARD)
	{
		SendToClip();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CImgFrequency::SendToClip()
{
	CClientDC dc(this);
	CRect EmfRect;
	GetClientRect(&EmfRect);	//获得客户区的矩形大小	

	CMetaFileDC mfdc;
    if( mfdc.CreateEnhanced(NULL,NULL,NULL,"HWPro") )
    {
		mfdc.SetAttribDC(dc.m_hAttribDC);
		CBrush brush;
		brush.CreateSolidBrush(RGB(255,255,255));
		mfdc.FillRect(EmfRect, &brush);
		brush.DeleteObject ();

		DrawRibbon(&mfdc);
		if(m_bDraw) DrawFrequency(&mfdc);
		
		HENHMETAFILE hmf;
        if( (hmf = mfdc.CloseEnhanced()) )
        {
            if(OpenClipboard() )
            {
                EmptyClipboard();
                SetClipboardData(CF_ENHMETAFILE, hmf);
                CloseClipboard();
				AfxMessageBox("发送图元文件到剪贴板成功       ");
            }
            ::DeleteEnhMetaFile(hmf);
        }
   }
}

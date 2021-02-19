// CrackDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FlowChart.h"
#include "CrackCompute.h"
#include "FIDio.h"
#include <math.h>
#include <process.h>
#include ".\CrackDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCrackDlg dialog


CCrackDlg::CCrackDlg(CCrackCompute *pObj,CWnd* pParent /*=NULL*/)
	: CDialog(CCrackDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCrackDlg)
	//}}AFX_DATA_INIT
	m_bColor=FALSE;
	m_Palsize=64;
	m_WinWide=0.0254f;
	m_flagValue=200.f;
	m_FlagFre=0.f;

	m_ParA=0.35f;
	m_ParB=0.4f;
	m_pObj=pObj;
	m_MaxValue=1024.f;m_MinValue=0.f;
}


void CCrackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCrackDlg)
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT1, m_WinWide);
	DDX_Text(pDX, IDC_EDIT9, m_flagValue);
	DDX_Text(pDX, IDC_EDIT10, m_ParA);
	DDX_Text(pDX, IDC_EDIT11, m_ParB);
	DDX_Text(pDX, IDC_EDIT12,m_FlagFre);
	DDX_Text(pDX, IDC_EDIT13,m_Win);
	DDX_Text(pDX, IDC_EDIT14,m_WinRlev);
	DDX_Check(pDX, IDC_CHECK1, m_Check1);
	DDX_Check(pDX, IDC_CHECK2, m_Check2);
	DDX_Check(pDX, IDC_CHECK3, m_Check3);
	DDX_Check(pDX, IDC_CHECK4, m_Check4);
}


BEGIN_MESSAGE_MAP(CCrackDlg, CDialog)
	//{{AFX_MSG_MAP(CCrackDlg)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_PAINT()
//	ON_EN_CHANGE(IDC_EDIT9, OnEnChangeEdit9)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCrackDlg message handlers

void CCrackDlg::CreateImagePalette()
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

BOOL CCrackDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Check1=m_Check2=m_Check3=m_Check4=FALSE;
	if(m_szCp.Find("充填缝")>-1)
		m_Check1=TRUE;
	if(m_szCp.Find("张开缝")>-1)
		m_Check2=TRUE;
	if(m_szCp.Find("层间缝")>-1)
		m_Check3=TRUE;
	if(m_szCp.Find("半充填缝")>-1)
		m_Check4=TRUE;
	this->UpdateData(FALSE);
	// TODO: Add extra initialization here
	CreateImagePalette();
	m_bSelect=FALSE;
	m_Shadow.Create(GetSafeHwnd());
	m_bDraw=GetFrequency();

	CRect rect;
	// get the current window size and position
	GetWindowRect( rect );
	int h=rect.Height ();
	int w=rect.Width ();
	rect.left =(GetSystemMetrics(SM_CXSCREEN)-w)/2;
	rect.top =(GetSystemMetrics(SM_CYSCREEN)-h)/2;
	rect.bottom =rect.top+h;
	rect.right =rect.left+w;
	MoveWindow(&rect);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCrackDlg::DrawRibbon()
{
	CRect clientrect;
	GetClientRect(&clientrect);	//获得客户区的矩形大小
	
	CRect rectRib=clientrect;
	rectRib.top=80;
	rectRib.bottom=rectRib.top+95;
	CRect rect=rectRib;
	CClientDC dc(this);
	CPalette* pOldPalette = (CPalette*)dc.SelectPalette(&m_Palette, FALSE);
	dc.RealizePalette();
	
	for (int i=0; i<m_Palsize; i++)
	{
		rect.right = rectRib.left+(i+1)*rectRib.Width()/m_Palsize+2;
		CBrush br(PALETTEINDEX(i));
		dc.FillRect(rect,&br);
		rect.left = rect.right;
	}
	dc.SelectObject(pOldPalette);
}

void CCrackDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	if(m_bDraw) DrawFrequency();
}

void CCrackDlg::DrawFrequency()
{
	DrawRibbon();
	int i,Max=1;
	for(i=0;i<300;i++)
		if(m_freq[i]>Max) 
			Max=m_freq[i];
	if(Max<=1) 
		return;	
	CClientDC dc(this);
	CRect clientrect;
	GetClientRect(&clientrect);	//获得客户区的矩形大小	
	CRect rect=clientrect;
	rect.top =95;
	CBrush br1(0xB75B30);//0xFFFFFF);//
	dc.FillRect(rect,&br1);

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(0xffffff);//0x0);
	CString str;
	str.Format ("<-%.1f",m_LitMin);
	dc.TextOut (2,97,str);
	str.Format ("%.1f->",m_MorMax);
	dc.TextOut (rect.right -str.GetLength ()*8+8,97,str);

	CRect rectRib=clientrect;
	rectRib.top=95;
	rectRib.bottom-=20;
	//横刻度线
	dc.MoveTo (rectRib.left ,rectRib.bottom );
	dc.LineTo (rectRib.right ,rectRib.bottom );
	int x;
	for(i=1;i<128;i++)
	{
		x=rectRib.left+i*rectRib.Width ()/128 ;
		dc.MoveTo (x,rectRib.bottom );
		dc.LineTo (x,rectRib.bottom+3);
	}
	CPen m_Pen2(0,1,RGB(255,255,255));
	CPen* pOldPen = dc.SelectObject(&m_Pen2);
	//横刻度值
	for(i=0;i<16;i++)
	{
		x=rectRib.left +i*rectRib.Width ()/16;
		dc.MoveTo (x,rectRib.bottom );
		dc.LineTo (x,rectRib.bottom+6);
		if((i+1)%2==0)
		{
			str.Format ("%g",m_MinValue+i*(m_MaxValue-m_MinValue)/16.f);
			dc.TextOut (x-str.GetLength ()*4+2,rectRib.bottom+5,str);
		}
	}
	x=rectRib.right-1;
	dc.MoveTo (x,rectRib.bottom);
	dc.LineTo (x,rectRib.bottom+6);	

	rect=rectRib;
	CPalette* pOldPalette = (CPalette*)dc.SelectPalette(&m_Palette, FALSE);
	dc.RealizePalette();
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
		dc.FillRect(rect,&br);		
	}
	dc.SelectObject(pOldPalette);
	m_Rect=rectRib;
	m_MouseRect=rectRib;
	m_Pt=(int)((m_flagValue-m_MinValue)/(m_MaxValue-m_MinValue)*wide);
	m_MouseRect.left=m_Pt-2;
	m_MouseRect.right=m_Pt+2;
	int DrawMode = dc.SetROP2(R2_XORPEN);
	CPen Pen(0,2,0xffff00);
	dc.SelectObject(&Pen);
	dc.MoveTo (m_Pt,m_Rect.top);
	dc.LineTo (m_Pt,m_Rect.bottom);
	dc.SetROP2( DrawMode );
	dc.SelectObject( pOldPen );
}

BOOL CCrackDlg::GetFrequency()
{
	POSITION pos = m_pObj->m_SinPoint.GetHeadPosition();
	if(pos== NULL) 
		return FALSE;
	int Off=(int)(m_WinWide/m_Rlev);
	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");
	strcat(s,m_CurveName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;	
	if(!fp.Open(fn,CFile::modeRead|CFile::typeBinary))
		return FALSE;
	BeginWaitCursor();
	int nps= m_pObj->m_PadNum*m_pObj->m_ButtonNum;
	int i,j,y,m,Num;
	int RecordNum,Block;
	float fdata=m_MaxValue-m_MinValue;
	if(fdata<1.f) 
	{
		fp.Close ();
		return FALSE;
	}

	for(i=0;i<300;i++)
		m_freq[i]=0;
	int MorMax=0,LitMin=0;
	AllNum=0;
	double dtor=360./nps;
	while (pos!= NULL) 
	{
		CSinPoint *pSin=(CSinPoint *)m_pObj->m_SinPoint.GetNext(pos);
		if(pSin!=NULL)
		{
			if(m_szCp.Find(pSin->m_Flag)>-1)
			{
				RecordNum=(unsigned)((pSin->m_Dep-m_ProStDep)/m_Rlev)-pSin->m_Hight-Off;
				if(RecordNum<0) RecordNum=0;
				Block=nps*(pSin->m_Hight+Off)*2;
				float* Value = NULL;	
				Value = new float[Block];
				fp.Seek (RecordNum*nps*4,CFile::begin);
				fp.Read (Value,Block*4);
				for(j=pSin->m_Hight-Off;j<pSin->m_Hight+Off;j++)
				{
					for(i=0;i<nps;i++)
					{
						y=j+(int)(cos((i*dtor-pSin->m_Dir)*PI/180.)*pSin->m_Hight);
						Num=y*nps+i;
						if(Num<0) Num=0;
						if(Num>Block-1) Num=Block-1;
						if((Value[Num]!=-9999.f)&&(Value[Num]!=-999.f))
						{
							m=(int)((Value[Num]-m_MinValue)/fdata*300.f);
							if(m>300)
								MorMax++;
							else if(m<0)
								LitMin++;
							else
								m_freq[m]++;
							AllNum++;
						}
					}
				}
				delete Value;
			}
		}
	}
	if(AllNum>0)
	{
		m_MorMax=MorMax*100.f/AllNum;
		m_LitMin=LitMin*100.f/AllNum;
	}
	else
	{
		m_MorMax=0.f;
		m_LitMin=0.f;
	}
	fp.Close ();
	EndWaitCursor();
	return TRUE;
}

void CCrackDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnLButtonDown(nFlags, point);	
	if(m_MouseRect.PtInRect(point))
	{
		m_bSelect=TRUE;
		CClientDC dc(this);
		int DrawMode = dc.SetROP2(R2_XORPEN);
		CPen Pen(0,2,0xffff00);
		CPen *pOldPen=(CPen *)dc.SelectObject(&Pen);
		dc.MoveTo (m_Pt,m_Rect.top);
		dc.LineTo (m_Pt,m_Rect.bottom);
		dc.MoveTo (point.x,m_Rect.top);
		dc.LineTo (point.x,m_Rect.bottom);
		dc.SetROP2( DrawMode );
		dc.SelectObject( pOldPen );
		m_Pt = point.x;
		m_MouseRect.left=m_Pt-2;
		m_MouseRect.right=m_Pt+2;
	}
}

void CCrackDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnLButtonUp(nFlags, point);
	if(m_bSelect)
	{	
		CClientDC dc(this);
		int DrawMode = dc.SetROP2(R2_XORPEN);
		CPen Pen(0,2,0xffff00);
		CPen *pOldPen=(CPen *)dc.SelectObject(&Pen);
		dc.MoveTo (m_Pt,m_Rect.top);
		dc.LineTo (m_Pt,m_Rect.bottom);
		dc.MoveTo (point.x,m_Rect.top);
		dc.LineTo (point.x,m_Rect.bottom);
		m_Pt = point.x;		
		// 更改阈值
		m_flagValue=m_MinValue+(m_Pt-m_Rect.left)*(m_MaxValue-m_MinValue)/m_Rect.Width();
		int pp=(m_Pt-m_Rect.left)*300/m_Rect.Width();
		if(pp>0&&pp<300)
		{
			int pNum=0;
			for(int i=0;i<pp;i++)
				pNum+=m_freq[i];
			m_FlagFre=pNum*100.f/AllNum;
		}
		UpdateData(FALSE);
		m_MouseRect.left=m_Pt-2;
		m_MouseRect.right=m_Pt+2;
		dc.SetROP2( DrawMode );
		dc.SelectObject( pOldPen );
		m_bSelect=FALSE;
	}
}

void CCrackDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnMouseMove(nFlags, point);
	// 判断是否正在拖动
	if (m_bSelect)
	{
		if(m_Pt!= point.x)
		{
			CClientDC dc(this);
			int DrawMode = dc.SetROP2(R2_XORPEN);
			CPen Pen(0,2,0xffff00);
			CPen *pOldPen=(CPen *)dc.SelectObject(&Pen);
			dc.MoveTo (m_Pt,m_Rect.top);
			dc.LineTo (m_Pt,m_Rect.bottom);
			dc.MoveTo (point.x,m_Rect.top);
			dc.LineTo (point.x,m_Rect.bottom);
			dc.SetROP2( DrawMode );
			dc.SelectObject( pOldPen );
			m_Pt = point.x;
			// 更改阈值
			m_flagValue=m_MinValue+(m_Pt-m_Rect.left)*(m_MaxValue-m_MinValue)/m_Rect.Width();
			int pp=(m_Pt-m_Rect.left)*300/m_Rect.Width();
			if(pp>0&&pp<300)
			{
				int pNum=0;
				for(int i=0;i<pp;i++)
					pNum+=m_freq[i];
				m_FlagFre=pNum*100.f/AllNum;
			}
			UpdateData(FALSE);		
			m_MouseRect.left=m_Pt-2;
			m_MouseRect.right=m_Pt+2;
		}
	}
	else if(m_MouseRect.PtInRect(point))
		// 更改光标
		::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
}

void CCrackDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
	m_szCp="";
	if(m_Check1)
		m_szCp+="充填缝";
	if(m_Check2)
		m_szCp+="张开缝";
	if(m_Check3)
		m_szCp+="层间缝";
	if(m_Check4)
		m_szCp+="半充填缝";
}

void CCrackDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	if(m_bDraw) DrawFrequency();
}

void CCrackDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	m_szCp="";
	if(m_Check1)
		m_szCp+="充填缝";
	if(m_Check2)
		m_szCp+="张开缝";
	if(m_Check3)
		m_szCp+="层间缝";
	if(m_Check4)
		m_szCp+="半充填缝";

	m_bDraw=GetFrequency();
	if(m_bDraw) DrawFrequency();
}

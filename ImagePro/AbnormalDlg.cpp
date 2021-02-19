// CrackDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FlowChart.h"
#include "FIDio.h"
#include "Guage.h"
#include <math.h>
#include <process.h>
#include "AbnormalDlg.h"
#include "FitCircle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbnormalDlg dialog


CAbnormalDlg::CAbnormalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAbnormalDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAbnormalDlg)
	//}}AFX_DATA_INIT
	m_bColor=TRUE;
	m_Palsize=64;
	m_flagValue=756.f;
	m_FlagFre=0.f;
	m_Win=0.6069f;
	m_WinRlev=0.1524f;
	m_MinValue=0.f;
	m_MaxValue=1024.f;
	m_EdgeType=0;
	m_bCreatImg=TRUE;
	m_szEdgeName="动态图像";
	m_szOutImgName="EDIMG0";
	m_szOutStaName="EDSTA0";
}


void CAbnormalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbnormalDlg)
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT9, m_flagValue);
	DDX_Text(pDX, IDC_EDIT12,m_FlagFre);
	DDX_Text(pDX, IDC_EDIT13,m_Win);
	DDX_Text(pDX, IDC_EDIT14,m_WinRlev);
	DDX_Text(pDX, IDC_EDIT16,m_szOutImgName);
	DDX_Text(pDX, IDC_EDIT17,m_szOutStaName);
	DDX_CBString(pDX, IDC_COMBO1, m_szEdgeName);
	DDX_Control(pDX, IDC_COMBO1, m_NameComCtrl);
	DDX_Radio(pDX, IDC_RADIO1, m_EdgeType);
	DDX_Check(pDX, IDC_CHECK1, m_bCreatImg);
}


BEGIN_MESSAGE_MAP(CAbnormalDlg, CDialog)
	//{{AFX_MSG_MAP(CAbnormalDlg)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
	ON_CBN_CLOSEUP(IDC_COMBO1, OnCbnCloseupCombo1)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnBnClickedRadio3)
//	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbnormalDlg message handlers

void CAbnormalDlg::CreateImagePalette()
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

BOOL CAbnormalDlg::OnInitDialog() 
{
	m_bCreatImg=FALSE;
	if(m_EdgeType==0)
	{
		m_szOutImgName="EDIMG0";
		m_szOutStaName="EDSTA0";
	}
	else if(m_EdgeType==1)
	{
		m_szOutImgName="EDIMG1";
		m_szOutStaName="EDSTA1";
	}
	else
	{
		m_bCreatImg=TRUE;
		m_szOutImgName="EDIMG";
		m_szOutStaName="EDSTA";
	}
	CDialog::OnInitDialog();
	if(m_bCreatImg)
		this->GetDlgItem(IDC_EDIT16)->EnableWindow();
	else
		this->GetDlgItem(IDC_EDIT16)->EnableWindow(FALSE);
	// TODO: Add extra initialization here
	CreateImagePalette();
	m_bSelect=FALSE;
	m_Shadow.Create(GetSafeHwnd());
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

	CString CurveName;
	if(m_szEdgeName=="静态图像")
		CurveName="DATAST";
	else if(m_szEdgeName=="动态图像")
		CurveName="DATADY";
	else
		CurveName="DATAPS";
	CreatEdgeData(CurveName,"EDGE");
	m_bDraw=GetFrequency("EDGE");
	if(m_bDraw) 
		DrawFrequency();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAbnormalDlg::DrawRibbon()
{
	CRect clientrect;
	GetClientRect(&clientrect);	//获得客户区的矩形大小
	
	CRect rectRib=clientrect;
	rectRib.top=80;
	rectRib.bottom=rectRib.top+90;
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

void CAbnormalDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	if(m_bDraw) DrawFrequency();
}

void CAbnormalDlg::DrawFrequency()
{
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
	rect.top =100;
	CBrush br1(GetSysColor(COLOR_BTNFACE));
	dc.FillRect(rect,&br1);

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(0x0);
	CString str;
	if(m_LitMin>0.1f)
	{
		str.Format ("< %.1f",m_LitMin);
		dc.TextOut (2,100,str);
	}
	if(m_MorMax>0.1f)
	{
		str.Format ("%.1f >",m_MorMax);
		dc.TextOut (rect.right -str.GetLength ()*8+8,100,str);
	}
	CRect rectRib=clientrect;
	rectRib.top=100;
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
	CPen m_Pen2(0,2,0x0000FF);
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

	if(m_EdgeType==1)
		x=m_Rect.left;
	else
		x=m_Rect.right;
	dc.MoveTo (x,m_Rect.top+1);
	dc.LineTo (m_Pt,m_Rect.top+1);

	dc.SetROP2( DrawMode );
	dc.SelectObject( pOldPen );
}

//区域扩展处理
BOOL CAbnormalDlg::AllDataExtentPro(CString Path,CString Name)
{
	int RecordNum;
	char s[256],fn[256];
	CFIDIO* pFIDI = new CFIDIO(Path+"."+Name);
	if(!pFIDI->Open())
	{
		delete pFIDI;return FALSE;
	}
	FIDCONTENT* Content=NULL;
	Content = pFIDI->GetContent(3);
	int DataNum=Content->nps;
	delete pFIDI;

	int Block=DataNum*4;
	RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	BOOL bUserBreak=FALSE;
	strcpy(s,Path);
	strcat(s,".");strcat(s,Name);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		//定义进度指示器
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->ShowWindow(SW_SHOW);
		CString ss;
		ss.Format("统计 %s 井周静态刻度区间...",Name);
		pGuage->SetTitle(ss);
		float dep=m_StProDep;

		float *Value;
		Value=new float[DataNum+10];
		int i,k,NNN,pos;

		int freq[101];
		//寻找窗口范围内的频率最大位置的值
		for(i=0;i<100;i++)
			freq[i]=0;
		k=0;

		while(k<RecordNum)
		{
			fp.Seek (k*Block,CFile::begin);
			fp.Read (Value,Block);
			for(i=0;i<DataNum;i++)
			{
				pos=(int)(Value[i]/1024.f*100.f);
				if(pos>99) pos=99;
				if(pos<0) pos=0;
				freq[pos]++;
			}
			k++;
			dep=m_StProDep+m_Rlev*k;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) 
			{
				bUserBreak=TRUE;
				break;
			}
		}
		if(!bUserBreak)
		{
			NNN=0;pos=0;
			for(i=0;i<100;i++)
			{
				if(freq[i]>NNN)
				{
					NNN=freq[i];
					pos=i;
				}
			}
			for(i=1;i<100;i++)
				freq[i]=freq[i]*1000/(RecordNum*DataNum);

			float Min=0.f,Max=0.f,xxx;
			if(pos>98)
			{
				//寻找最小值
				for(i=pos;i>0;i--)
					if(freq[i]>1)
					{
						Min=i*10.24f;
						Max=1024.f;
					}
			}
			else if(pos<1)
			{
				//寻找最大值
				for(i=pos;i<99;i++)
					if(freq[i]>1)
					{
						Max=i*10.24f;
						Min=0.f;
					}
			}
			else
			{
				//寻找最大值
				for(i=pos;i<98;i++)
				{
					if(freq[i]>1)
						Max=i*10.24f;
				}
				//寻找最小值
				for(i=pos;i>1;i--)
				{
					if(freq[i]>1)
						Min=i*10.24f;
				}
			}
			if(Max>0.f&&Max>Min)
			{
				xxx=Max-Min;
				k=0;
				ss=Name+" 井周静态刻度处理...";
				pGuage->SetTitle(ss);
				dep=m_StProDep;
				while(k<RecordNum)
				{
					fp.Seek (k*Block,CFile::begin);
					fp.Read (Value,Block);
						
					for(i=0;i<DataNum;i++)
						Value[i]=(Value[i]-Min)/xxx*1024.f;
					fp.Seek (k*Block,CFile::begin);
					fp.Write (Value,Block);
					k++;
					dep=m_StProDep+m_Rlev*k;
					pGuage->SetDepth(dep);
					if (pGuage->CheckCancelButton()) 
					{
						bUserBreak=TRUE;
						break;
					}
				}
			}
		}
		delete Value;
		delete pGuage;
	}
	else bUserBreak=TRUE;
	fp.Close ();
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

BOOL CAbnormalDlg::CopyCurve(CString InName,CString OutName)
{
	BOOL bUserBreak=FALSE;
	CFIDIO* pFIDR = NULL;
	pFIDR = new CFIDIO(m_szBufPath+"."+InName);
	if(!pFIDR->Open())
	{
		delete pFIDR;
		return FALSE;
	}

	float Sdep = pFIDR->GetStartDepth();
	float Edep = pFIDR->GetEndDepth();
	float Rlev=pFIDR->GetDepthLevel();
	int dim=pFIDR->GetDimenSion();	
	int RecordNum=(int)((Edep-Sdep)/Rlev);

	//定义进度指示器
	CGuage* pGuage = new CGuage(Sdep,Edep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);

	CString ss="从缓冲区复制 "+InName+" 到目标区 "+OutName+"...";
	pGuage->SetTitle(ss);
	float dep=Sdep;
	
	int i;
	FIDCONTENT* Content;
	FIDCONTENT ContentW[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
								{"","none",REPR_FLOAT,4,1,1,0.f,100.f,0.f},
								{"","none",REPR_FLOAT,4,1,1,0.f,100.f,0.f}};
	ContentW[0].min = Sdep;
	ContentW[0].max = Edep;
	ContentW[0].rlev =Rlev ;
	for(i=1;i<dim;i++)
	{
		Content = pFIDR->GetContent(i+1);
		strcpy(ContentW[i].name,OutName);
		strcpy(ContentW[i].unit,Content->unit);
		ContentW[i].type=Content->type;
		ContentW[i].length=Content->length;
		ContentW[i].nps=Content->nps;
		ContentW[i].npw=Content->npw;
		ContentW[i].min=Content->min;
		ContentW[i].max=Content->max;
		ContentW[i].rlev=Rlev;
	}
	int nps;
	short* ibuf = NULL;
	float* fbuf = NULL;
	char* cbuf = NULL;
	if (Content != NULL)
	{
		nps = Content->nps;
		if (Content->type == REPR_FLOAT) fbuf = new float[nps];
		else if (Content->type == REPR_CHAR) cbuf = new char[nps];
		else ibuf = new short[nps];
	}

	FIDINDEX IndexW;
	IndexW.dimension = dim;
	ContentW[0].rlev =Rlev ;
	IndexW.Contentlist = ContentW;
	char adi[] = "==END==";
  	IndexW.adi = adi;

	CFIDIO* pFIDW = NULL;
	pFIDW= new CFIDIO(m_szBufPath+"."+OutName,&IndexW);
	if(pFIDW->Open())
	{
		int k=0;
		while(k<RecordNum)
		{
			if (Content->type == REPR_FLOAT)
			{
				pFIDR->Read(dim,nps,fbuf);
				pFIDW->Write(dim,nps,fbuf);
			}
			else if (Content->type == REPR_CHAR)
			{
				pFIDR->Read(dim,nps,cbuf);
				pFIDW->Write(dim,nps,cbuf);
			}
			else 
			{
				pFIDR->Read(dim,nps,ibuf);
				pFIDW->Write(dim,nps,ibuf);
			}
			pFIDR->Next();	
			pFIDW->Next();
			k++;
			dep=m_StProDep+m_Rlev*k;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) 
			{
				bUserBreak=TRUE;
				break;
			}
		}
	}
	else bUserBreak=TRUE;
	delete pFIDR;delete pFIDW;
	delete pGuage;
	if (cbuf != NULL)	delete cbuf;
	if (ibuf != NULL)	delete ibuf;
	if (fbuf != NULL)	delete fbuf;
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

BOOL CAbnormalDlg::CreatEdgeData(CString InName,CString OutName)
{
	if(!CopyCurve(InName,OutName)) return FALSE;
	if(m_EdgeType==1) 
	{
		GetDataMaxMin(OutName);
		return TRUE;
	}
	float *fwf0,*fwf1,*fwf2,*fwf3;
	int i,j,k,m,RecordNum,nLoop=3;
	char s[256],fn[256];
	float radius=0.25f;
	RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	strcpy(s,m_szBufPath);
	strcat(s,".");strcat(s,OutName);
	CFIDIO::GetFileName(s,fn,'D');	
	BOOL bUserBreak=FALSE;
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		int Block=m_DataNum*4;
		
		//定义进度指示器
		float dep=m_StProDep;
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->ShowWindow(SW_SHOW);
		pGuage->SetTitle(m_szEdgeName+"图像边沿检测...");

		CPoint *points=NULL;
		points=new CPoint[m_DataNum];
		CFitCircle FitCircle(m_DataNum);//最小二乘法拟合圆
		float fX,fY,Radius,fScale;
		int nFitNum;
		
		fwf0=new float[m_DataNum];
		fwf1=new float[m_DataNum];
		fwf2=new float[m_DataNum];
		fwf3=new float[m_DataNum];
		k=0;
		fp.Seek (k*Block,CFile::begin);
		fp.Read (fwf0,Block);
		k++;
		fp.Seek (k*Block,CFile::begin);
		fp.Read (fwf1,Block);
		k++;
		float ydat,xdat;
		while(k<RecordNum)
		{
			fp.Seek (k*Block,CFile::begin);
			fp.Read (fwf2,Block);	
			//用拟合圆的方法求取图像底值，消除图像本底
			nFitNum=0;
			fScale=1.f;
			for(i=0;i<m_DataNum;i++)
			{
				if(fwf2[i]>fScale) 
					fScale=fwf2[i];
			}
			fScale=30000.f/fScale;
			for(i=0;i<m_DataNum;i++)
			{
				if(fwf2[i]>1024.f) 
					fwf2[i]=1024.f;
				if(fwf2[i]<0.f) 
					fwf2[i]=0.f;
				points[nFitNum].x=(int)(sin(6.2831853*i/m_DataNum)*fScale*fwf2[i]);
				points[nFitNum].y=(int)(cos(6.2831853*i/m_DataNum)*fScale*fwf2[i]);
				FitCircle.m_points[nFitNum].x=points[nFitNum].x;
				FitCircle.m_points[nFitNum].y=points[nFitNum].y;
				nFitNum++;
			}
			if(nFitNum>3)
			{
				FitCircle.m_nNum=nFitNum;
				FitCircle.DoCircleFit();
				fX = (float)FitCircle.m_fCenterX;
				fY = (float)FitCircle.m_fCenterY;
				Radius=(float)FitCircle.m_fRadius/fScale;
				for(i=0;i<m_DataNum;i++)
				{
					fwf2[i]=(float)(sqrt((points[i].x-fX)*(points[i].x-fX)+
							(points[i].y-fY)*(points[i].y-fY)))/fScale;
					fwf2[i]-=Radius;
				}
			}
			fp.Seek (k*Block,CFile::begin);
			if(m_EdgeType==0)
				fp.Write (fwf2,Block);
			else//图像边沿检测
			{
				for(i=0;i<m_PadNum;i++)
				{
					fwf3[i*m_ButtonNum]=0.f;
					for(m=1;m<m_ButtonNum-1;m++)
					{
						j=i*m_ButtonNum+m;
						xdat=fwf0[j+1]+fwf1[j+1]*2.f+fwf2[j+1]
							-fwf0[j-1]-fwf1[j-1]*2.f-fwf2[j-1];
						ydat=fwf2[j-1]+fwf2[j]*2.f+fwf2[j+1]
							-fwf0[j-1]-fwf0[j]*2.f-fwf0[j+1];
						fwf3[j]=sqrt(xdat*xdat+ydat*ydat);
					}
					fwf3[i*m_ButtonNum+m_ButtonNum-1]=0.f;
				}
				fp.Write (fwf3,Block);
			}
			memcpy(fwf0,fwf1,Block);
			memcpy(fwf1,fwf2,Block);
			k++;
			dep=m_StProDep+m_Rlev*k;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) break;
		}
		delete points;
		delete fwf0;delete fwf1;
		delete fwf2;delete fwf3;
		delete pGuage;
	}
	else bUserBreak=TRUE;
	fp.Close ();
	GetDataMaxMin(OutName);
	if(bUserBreak) return FALSE;
	else return TRUE;
}

BOOL CAbnormalDlg::GetFrequency(CString CurveName)
{
	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");
	strcat(s,CurveName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;	
	if(!fp.Open(fn,CFile::modeRead|CFile::typeBinary))
		return FALSE;
	BeginWaitCursor();

	float* Value = NULL;	
	Value = new float[m_DataNum];
	int i,m;
	
	int RecordNum=(unsigned)((m_EnProDep-m_StProDep)/m_Rlev);
	int  Block=m_DataNum*4;
	int Num=0;
	for(i=0;i<300;i++)
		m_freq[i]=0;
	int MorMax=0,LitMin=0;
	
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);
	CString ss=CurveName+" 数据检索...";
	pGuage->SetTitle(ss);
	float dep,fdata=m_MaxValue-m_MinValue;
	AllNum=0;
	while(Num<RecordNum)
	{
		fp.Seek (Num*Block,CFile::begin);
		fp.Read (Value,Block);	
		for(i=0;i<m_DataNum;i++)
		{
			m=(int)(Value[i]*300.f/fdata);
			if(m>299)
				MorMax++;
			else if(m<0)
				LitMin++;
			else
				m_freq[m]++;
			AllNum++;
		}
		Num++;
		dep=m_StProDep+m_Rlev*Num;
		pGuage->SetDepth(dep);
		if(pGuage->CheckCancelButton()) 
			break;
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
	delete pGuage;delete Value;
	EndWaitCursor();
	return TRUE;
}

void CAbnormalDlg::OnLButtonDown(UINT nFlags, CPoint point) 
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

		int x;
		if(m_EdgeType==1)
			x=m_Rect.left;
		else
			x=m_Rect.right;
		dc.MoveTo (x,m_Rect.top+1);
		dc.LineTo (m_Pt,m_Rect.top+1);
		dc.MoveTo (x,m_Rect.top+1);
		dc.LineTo (point.x,m_Rect.top+1);
		dc.SetROP2( DrawMode );
		dc.SelectObject( pOldPen );
		m_Pt = point.x;
		m_MouseRect.left=m_Pt-2;
		m_MouseRect.right=m_Pt+2;
		// 更改阈值
		m_flagValue=m_MinValue+(m_Pt-m_Rect.left)*(m_MaxValue-m_MinValue)/m_Rect.Width();
		int pp=(m_Pt-m_Rect.left)*300/m_Rect.Width();
		if(pp>0&&pp<300)
		{
			int pNum=0;
			for(int i=0;i<pp;i++)
				pNum+=m_freq[i];
			m_FlagFre=pNum*100.f/AllNum+m_LitMin;
			if(m_EdgeType!=1)
				m_FlagFre=100.f-m_FlagFre;
		}
		UpdateData(FALSE);
	}
}

void CAbnormalDlg::OnLButtonUp(UINT nFlags, CPoint point) 
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

		int x;
		if(m_EdgeType==1)
			x=m_Rect.left;
		else
			x=m_Rect.right;
		dc.MoveTo (x,m_Rect.top+1);
		dc.LineTo (m_Pt,m_Rect.top+1);
		dc.MoveTo (x,m_Rect.top+1);
		dc.LineTo (point.x,m_Rect.top+1);
		m_Pt = point.x;		
		// 更改阈值
		m_flagValue=m_MinValue+(m_Pt-m_Rect.left)*(m_MaxValue-m_MinValue)/m_Rect.Width();
		int pp=(m_Pt-m_Rect.left)*300/m_Rect.Width();
		if(pp>0&&pp<300)
		{
			int pNum=0;
			for(int i=0;i<pp;i++)
				pNum+=m_freq[i];
			m_FlagFre=pNum*100.f/AllNum+m_LitMin;
			if(m_EdgeType!=1)
				m_FlagFre=100.f-m_FlagFre;
		}
		UpdateData(FALSE);
		m_MouseRect.left=m_Pt-2;
		m_MouseRect.right=m_Pt+2;
		dc.SetROP2( DrawMode );
		dc.SelectObject( pOldPen );
		m_bSelect=FALSE;
	}
}

void CAbnormalDlg::OnMouseMove(UINT nFlags, CPoint point) 
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
			
			int x;
			if(m_EdgeType==1)
				x=m_Rect.left;
			else
				x=m_Rect.right;
			dc.MoveTo (x,m_Rect.top+1);
			dc.LineTo (m_Pt,m_Rect.top+1);
			dc.MoveTo (x,m_Rect.top+1);
			dc.LineTo (point.x,m_Rect.top+1);

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
				m_FlagFre=pNum*100.f/AllNum+m_LitMin;
				if(m_EdgeType!=1)
					m_FlagFre=100.f-m_FlagFre;
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

void CAbnormalDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	if(m_bDraw) DrawFrequency();
}

void CAbnormalDlg::OnCbnCloseupCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	int nIndex = m_NameComCtrl.GetCurSel();
	if (nIndex == CB_ERR)
	{
		m_NameComCtrl.GetWindowText(str);
		return;
	}
	else
		m_NameComCtrl.GetLBText(nIndex, str);
	str.MakeUpper();
	if (m_szEdgeName != str)
	{
		m_szEdgeName = str;
		GetDlgItem(IDC_COMBO1)->SetWindowText(m_szEdgeName);
		CString CurveName;
		if(m_szEdgeName=="静态图像")
			CurveName="DATAST";
		else if(m_szEdgeName=="动态图像")
			CurveName="DATADY";
		else
			CurveName="DATAPS";
		m_bDraw=CreatEdgeData(CurveName,"EDGE");
		if(m_bDraw)
		{
			m_bDraw=GetFrequency("EDGE");
			if(m_bDraw) 
				DrawFrequency();
		}
	}
}

void CAbnormalDlg::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	if(m_bCreatImg)
		this->GetDlgItem(IDC_EDIT16)->EnableWindow();
	else
		this->GetDlgItem(IDC_EDIT16)->EnableWindow(FALSE);
}

void CAbnormalDlg::UpdateEdgeData()
{
	UpdateData();
	CString CurveName;
	if(m_szEdgeName=="静态图像")
		CurveName="DATAST";
	else if(m_szEdgeName=="动态图像")
		CurveName="DATADY";
	else
		CurveName="DATAPS";
	m_bDraw=CreatEdgeData(CurveName,"EDGE");
	if(m_bDraw)
	{
		m_bDraw=GetFrequency("EDGE");
		if(m_bDraw) 
			DrawFrequency();
	}
	
	m_bCreatImg=FALSE;
	if(m_EdgeType==0)
	{
		m_szOutImgName="EDIMG0";
		m_szOutStaName="EDSTA0";
	}
	else if(m_EdgeType==1)
	{
		m_szOutImgName="EDIMG1";
		m_szOutStaName="EDSTA1";
	}
	else
	{
		m_bCreatImg=TRUE;
		m_szOutImgName="EDIMG";
		m_szOutStaName="EDSTA";
	}
	if(m_bCreatImg)
		this->GetDlgItem(IDC_EDIT16)->EnableWindow();
	else
		this->GetDlgItem(IDC_EDIT16)->EnableWindow(FALSE);
	UpdateData(FALSE);
}

void CAbnormalDlg::OnBnClickedRadio1()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_EdgeType!=0)
	{
		m_EdgeType=0;
		UpdateEdgeData();
	}
}

void CAbnormalDlg::OnBnClickedRadio2()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_EdgeType!=1)
	{
		m_EdgeType=1;
		UpdateEdgeData();
	}
}

void CAbnormalDlg::OnBnClickedRadio3()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_EdgeType!=2)
	{
		m_EdgeType=2;
		UpdateEdgeData();
	}
}

BOOL CAbnormalDlg::GetDataMaxMin(CString InName)
{
	BOOL bUserBreak=FALSE;
	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");
	strcat(s,InName);
	CFIDIO::GetFileName(s,fn,'D');

	float* Value = NULL;	
	Value = new float[m_DataNum];
	int i,k;
	int Block=m_DataNum*4;
	int RecordNum=(unsigned)((m_EnProDep-m_StProDep)/m_Rlev);

	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);
	pGuage->SetTitle("检索数据区间...");
	float ScaleMin=327600.f;
	float ScaleMax=0.f;
	CFile fp;	
	if(fp.Open(fn,CFile::modeRead|CFile::typeBinary))
	{
		k=0;
		float dep=m_StProDep;
		while(k<RecordNum)
		{
			fp.Seek (k*Block,CFile::begin);
			fp.Read (Value,Block);
			for(i=0;i<m_DataNum;i++)
			{
				if(Value[i]>0.f) 
				{
					if(Value[i]<ScaleMin) 
						ScaleMin=Value[i];
					if(Value[i]>ScaleMax) 
						ScaleMax=Value[i];
				}
			}
			k++;
			dep=m_StProDep+m_Rlev*k;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) 
			{
				bUserBreak=TRUE;
				break;
			}
		}
	}
	fp.Close();
	delete Value;
	delete pGuage;
	if(bUserBreak) return FALSE;
	else    
	{
		m_MinValue=ScaleMin;
		m_MaxValue=ScaleMax;
		return TRUE;
	}
}
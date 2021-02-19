// DataPoisDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FlowChart.h"
#include "DataPoisePro.h"
#include "FIDio.h"
#include <math.h>
#include <process.h>
#include "DataPoisDlg.h"
#include "Gradient.h"
#include "Guage.h"
#include "Clustering.h"
#include "FitCircle.h"
#include ".\datapoisdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataPoisDlg dialog


CDataPoisDlg::CDataPoisDlg(CDataPoisePro *pObj,CWnd* pParent /*=NULL*/)
	: CDialog(CDataPoisDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDataPoisDlg)
	m_szPadNo = _T("");
	m_szButNo = _T("");
	//}}AFX_DATA_INIT
	m_bColor=TRUE;
	m_Palsize=64;
	m_bSelect=FALSE;
	m_pObj=pObj;
	m_MaxFre=60.f;
	
	m_ButMin=0.01f;			//������С���ֵ
	m_bFilter =TRUE;
	m_CurveName="DPOIS";
	m_bDataPro=TRUE;

	m_bAutoBut=FALSE;		//��۾��⴦��
	m_bAutoPad =FALSE;		//������⴦��
}


void CDataPoisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataPoisDlg)
	DDX_Control(pDX, IDC_COMBO2, m_ButComCtrl);
	DDX_Control(pDX, IDC_COMBO1, m_PadComCtrl);
	DDX_Text(pDX, IDC_EDIT1, m_szPadNo);
	DDX_Text(pDX, IDC_EDIT8, m_szButNo);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT9, m_MaxFre);
	DDX_Text(pDX, IDC_EDIT10, m_ScaleMax);
	DDX_Text(pDX, IDC_EDIT11, m_ScaleMin);
	DDX_Text(pDX, IDC_EDIT12, m_ButMin);
	DDX_Text(pDX, IDC_EDIT_PROSTDEP, m_StProDep);
	DDX_Text(pDX, IDC_EDIT_PROENDEP, m_EnProDep);
	DDX_Check(pDX, IDC_CHECK1, m_bFilter);
	DDX_Check(pDX, IDC_CHECK2,m_bLogScal);
}


BEGIN_MESSAGE_MAP(CDataPoisDlg, CDialog)
	//{{AFX_MSG_MAP(CDataPoisDlg)
	ON_WM_PAINT()
	ON_CBN_CLOSEUP(IDC_COMBO1, OnCloseupCombo1)
	ON_CBN_CLOSEUP(IDC_COMBO2, OnCloseupCombo2)
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_CREATE()

	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, GetScalMaxMinValue)
	ON_BN_CLICKED(IDC_CHECK7, OnBnClickedCheck7)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataPoisDlg message handlers

void CDataPoisDlg::CreateImagePalette()
{
	m_Palette.DeleteObject();
	LPLOGPALETTE pLogPal = (LPLOGPALETTE) new BYTE[sizeof(LOGPALETTE)+m_Palsize * sizeof(PALETTEENTRY)];
	pLogPal->palVersion = 0x300; 
	pLogPal->palNumEntries = m_Palsize;	
	CGradient m_Gradient;
	CString ColorFile=GetWorkPath("Color")+"ImagePro.cip";
	CFile file(ColorFile, CFile::modeRead | CFile::shareDenyWrite );
	CArchive ar(&file, CArchive::load);	
	m_Gradient.Serialize(ar);

	RGBTRIPLE *pal = new RGBTRIPLE[m_Palsize], *entry;
	m_Gradient.SetQuantization (m_Palsize);
	m_Gradient.MakeEntries(pal,m_Palsize);
	for (int i=0; i<m_Palsize; i++) 
	{
		if(m_bColor)
			entry = &pal[i];
		else
			entry = &pal[m_Palsize-i-1];
		pLogPal->palPalEntry[i].peRed = entry->rgbtRed;
		pLogPal->palPalEntry[i].peGreen = entry->rgbtGreen;
		pLogPal->palPalEntry[i].peBlue =  entry->rgbtBlue;
		pLogPal->palPalEntry[i].peFlags = PC_RESERVED; 
	}
	delete[] pal;
	m_Palette.CreatePalette(pLogPal);
	delete [] (BYTE *)pLogPal;
}

BOOL CDataPoisDlg::OnInitDialog() 
{
	m_StProDep=m_StDep;
	m_EnProDep=m_EnDep;
	CDialog::OnInitDialog();
	
	CenterWindow();
	// TODO: Add extra initialization here
	CreateImagePalette();
	m_bSelect=FALSE;
	CString str;int i;
	for(i=0;i<m_pObj->m_PadNum;i++)
	{
		str.Format ("%d�ż���",i+1);
		m_PadComCtrl.AddString(str);
	}
	m_PadComCtrl.AddString("���м���");
	m_SelPadNo=0;
	m_PadComCtrl.SetWindowText("1�ż���");
	for(i=0;i<m_pObj->m_ButtonNum;i++)
	{
		str.Format ("%d�ŵ��",i+1);
		m_ButComCtrl.AddString(str);
	}
	m_ButComCtrl.AddString("���е��");
	m_SelButNo=m_pObj->m_ButtonNum;
	m_ButComCtrl.SetWindowText("���е��");	
	m_DataNum=m_pObj->m_PadNum*m_pObj->m_ButtonNum;
	
	str.Format("��ʼ:%.1f",m_StDep);
	GetDlgItem(IDC_STATIC_SDEP)->SetWindowText(str);
	str.Format("����:%.1f",m_EnDep);
	GetDlgItem(IDC_STATIC_EDEP)->SetWindowText(str);

	//if(DataPro())
	//	m_bDraw=GetFrequency();
	m_bDraw=FALSE;
	GetDlgItem(IDC_EDIT12)->EnableWindow(m_bFilter);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDataPoisDlg::DrawRibbon()
{
	CRect clientrect;
	GetClientRect(&clientrect);	//��ÿͻ����ľ��δ�С
	
	CRect rectRib=clientrect;
	rectRib.top=73;
	rectRib.bottom=rectRib.top+15;
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

void CDataPoisDlg::GetScaleNum()
{
	int xinc = 1;
	double xv,xvl,xvr;
	if(m_ScaleMax<m_ScaleMin)
	{
		float fdata=m_ScaleMax;
		m_ScaleMax=m_ScaleMin;
		m_ScaleMin=fdata;
	}
	xv = (int)log10(m_ScaleMin);
	xvl = log10(m_ScaleMin);	
	xvr = log10(m_ScaleMax);
	if (xinc*(xvl-xv) < 0)  
		xv -= xinc;
	int k = 1;   
	m_iThickScale=1;
	m_NumScale=1;
	while (xv <xvr)
	{            
		if (xv >xvl)
		{
			if ((k == 1) || (k == 10))
				m_iThickScale++;
			m_NumScale++;
		}
		k += xinc;
		xv += (log10((double)k) - log10((double)k-1)) * xinc;
		if (k == 10) k = 1;
	}
}

void CDataPoisDlg::DrawFrequency()
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
	GetClientRect(&clientrect);	//��ÿͻ����ľ��δ�С	
	CRect rect=clientrect;
	rect.top =95;
	CBrush br1(GetSysColor(COLOR_BTNFACE));
	dc.FillRect(rect,&br1);

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
	CString str;
	if(m_LitMin>0.01f)
	{
		str.Format ("<%.2f",m_LitMin);
		dc.TextOut (2,95,str);
	}
	if(m_MorMax>0.01f)
	{
		str.Format ("%.2f>",m_MorMax);
		dc.TextOut (rect.right -str.GetLength ()*8+8,95,str);
	}
	CPen Pen(0,1,0x26FE17);
	CPen* pOldPen = dc.SelectObject(&Pen);
	CRect rectRib=clientrect;
	rectRib.top=95;
	rectRib.bottom-=20;
	//��̶���
	dc.MoveTo (rectRib.left ,rectRib.bottom );
	dc.LineTo (rectRib.right ,rectRib.bottom );
	int x;
	BOOL bThinDraw=TRUE;
	if(m_bLogScal)
	{
		GetScaleNum();
		int Height=rectRib.Height ();
		int ZerY;
		int xinc = 1;
		double xv,xvl,xvr;
		xv = (int)log10(m_ScaleMin);
		xvl = log10(m_ScaleMin);	
		xvr = log10(m_ScaleMax);
		if (xinc*(xvl-xv) < 0) 
			xv -= xinc;
		int k = 1;   
		x = Round(rectRib.left + (xv - xvl) / (xvr - xvl) * rectRib.Width());
		CPen Pen1(0,1,0x0000FF);
		int x0=x,ii=0,ii0=0;
		while(x < rectRib.right)
		{            
			x = Round(rectRib.left + (xv - xvl) / (xvr - xvl) * rectRib.Width());
			if(x>=rectRib.left)
			{
				if((k == 1)||(k == 10))
				{
					ZerY =rectRib.bottom+6;
					dc.SelectObject(&Pen1);
					str.Format ("%g",pow(10.,xv));
					if(x-str.GetLength()*4<rectRib.left)
						dc.TextOut (x,rectRib.bottom+5,str);
					else if(x+str.GetLength()*4>rectRib.right)
						dc.TextOut (x-str.GetLength()*8,rectRib.bottom+5,str);
					else
						dc.TextOut (x-str.GetLength()*4,rectRib.bottom+5,str);
				}
				else
				{
					ZerY = rectRib.bottom+4;
					dc.SelectObject(&Pen);
				}
				dc.MoveTo (x,rectRib.bottom );
				dc.LineTo (x,ZerY);

				rect.left=x0;
				rect.right = x;
				if(ii>m_NumScale-1) ii=m_NumScale-1;
				rect.top=rectRib.bottom -Height*m_freq[ii]/Max;
				x0=x;
				ii++;
			}
			k += xinc;
			xv+=(log10((double)k)-log10((double)k-1))*xinc;
			if(k==10) k = 1;					
		}
	}
	else
	{
		for(i=1;i<128;i++)
		{
			x=rectRib.left+i*rectRib.Width ()/128 ;
			dc.MoveTo (x,rectRib.bottom );
			dc.LineTo (x,rectRib.bottom+4);
		}
		//��̶�ֵ
		CPen Pen2(0,1,0x0000FF);
		dc.SelectObject(&Pen2);
		for(i=0;i<16;i++)
		{
			x=rectRib.left +i*rectRib.Width ()/16;
			dc.MoveTo (x,rectRib.bottom );
			dc.LineTo (x,rectRib.bottom+6);
			if((i+1)%2==0)
			{
				str.Format ("%g",m_ScaleMin+i*(m_ScaleMax-m_ScaleMin)/16.f);
				dc.TextOut (x-str.GetLength ()*4+2,rectRib.bottom+5,str);
			}
		}
	}
	x=rectRib.right-1;
	dc.MoveTo (x,rectRib.bottom);
	dc.LineTo (x,rectRib.bottom+6);	
	dc.SelectObject(pOldPen);

	rect=rectRib;
	CPalette* pOldPalette = (CPalette*)dc.SelectPalette(&m_Palette, FALSE);
	dc.RealizePalette();
	int wide=rectRib.Width ();
	int Height=rectRib.Height ();
	int Ps=m_Palsize-1;
	int cr,ii;
	for(i=0;i<wide;i++)
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
	m_grippie.Invalidate();
}

void CDataPoisDlg::GetScalMaxMinValue()
{
	UpdateData();
	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");
	strcat(s,m_CurveName0);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;	
	if(!fp.Open(fn,CFile::modeRead|CFile::typeBinary))
		return;
	
	int nps= m_pObj->m_PadNum*m_pObj->m_ButtonNum;
	float* Value = new float[nps];
	int i;
	int StPt,EnPt;
	if(m_SelPadNo>=m_pObj->m_PadNum)
	{
		StPt=0;EnPt=nps;
	}
	else
	{
		StPt=m_SelPadNo*m_pObj->m_ButtonNum;
		EnPt=StPt+m_pObj->m_ButtonNum;
	}
	unsigned RecordNum=(unsigned)((m_EnProDep-m_StProDep)/m_Rlev);
	unsigned Num=0;
	//Ѱ����� ��Сֵ m_ScaleMax m_ScaleMin;
	
	m_ScaleMin=FLT_MAX;//300000.f;
	m_ScaleMax=FLT_MIN;//-300000.f;
	int Block=nps*4;
	BeginWaitCursor();
	if((m_SelButNo>=m_pObj->m_ButtonNum)||(m_SelPadNo>=m_pObj->m_PadNum))
	{
		while(Num<RecordNum)
		{
			fp.Seek (Num*Block,CFile::begin);
			fp.Read (Value,Block);
			Num++;
			for(i=StPt;i<EnPt;i++)
			{
				if(Value[i]==-999.25f||Value[i]==-9999.f)
					continue;
				if(Value[i]<m_ScaleMin)
				{
					if(m_bLogScal)
					{
						if(Value[i]>0.f)
							m_ScaleMin=Value[i];
					}
					else
						m_ScaleMin=Value[i];
				}
				if(Value[i]>m_ScaleMax) 
					m_ScaleMax=Value[i];
			}
		}
	}
	else
	{
		while(Num<RecordNum)
		{
			fp.Seek (Num*Block,CFile::begin);
			fp.Read (Value,Block);
			Num++;
			i=m_SelPadNo*m_pObj->m_ButtonNum+m_SelButNo;
			if(Value[i]==-999.25f||Value[i]==-9999.f)
				continue;
			if(Value[i]<m_ScaleMin) 
				m_ScaleMin=Value[i];
			if(Value[i]>m_ScaleMax) 
				m_ScaleMax=Value[i];
		}
	}
	fp.Close ();delete Value;
	EndWaitCursor();
	UpdateData(FALSE);
}

BOOL CDataPoisDlg::GetFrequency()
{
	if(m_bLogScal)
		if(m_ScaleMin<0.0000001f||m_ScaleMax<0.0000001f)
			return FALSE;
	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");
	strcat(s,m_CurveName);
	CFIDIO::GetFileName(s,fn,'D');
	int nps= m_pObj->m_PadNum*m_pObj->m_ButtonNum;
	CFile fp;	
	if(!fp.Open(fn,CFile::modeRead|CFile::typeBinary))
		return FALSE;
	BeginWaitCursor();
	
	float* Value = NULL;	
	Value = new float[nps];
	int i,m;
	int StPt,EnPt;
	if(m_SelPadNo>=m_pObj->m_PadNum)
	{
		StPt=0;EnPt=nps;
	}
	else
	{
		StPt=m_SelPadNo*m_pObj->m_ButtonNum;
		EnPt=StPt+m_pObj->m_ButtonNum;
	}
	unsigned RecordNum=(unsigned)((m_EnProDep-m_StProDep)/m_Rlev);
	unsigned Num=0;

	for(i=0;i<301;i++)
		m_freq[i]=0;
	int MorMax=0,LitMin=0;
	DWORD Point=0;Num=0;
	int Block=nps*4;
	float fdata,lgMin,lgMax;
	BOOL flag=FALSE;
	if(m_bLogScal)
	{
		lgMin=log10(m_ScaleMin);
		lgMax=log10(m_ScaleMax);
		fdata=lgMax-lgMin;
		if((m_SelButNo>=m_pObj->m_ButtonNum)||(m_SelPadNo>=m_pObj->m_PadNum))
		{
			while(Num<RecordNum)
			{
				fp.Seek (Num*Block,CFile::begin);
				fp.Read (Value,Block);
				Num++;
				
				for(i=StPt;i<EnPt;i++)
				{
					Value[i]=(float)((log10(Value[i])-lgMin)/fdata*1024.);
					m=(int)(Value[i]*300.f/1024.f);
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
		else
		{
			while(Num<RecordNum)
			{
				fp.Seek (Num*Block,CFile::begin);
				fp.Read (Value,Block);
				Num++;
				i=m_SelPadNo*m_pObj->m_ButtonNum+m_SelButNo;
				Value[i]=(float)((log10(Value[i])-lgMin)/fdata*1024.);
				m=(int)(Value[i]*300.f/1024.f);
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
	else
	{
		lgMin=m_ScaleMin;
		lgMax=m_ScaleMax;
		fdata=lgMax-lgMin;
		if((m_SelButNo>=m_pObj->m_ButtonNum)||(m_SelPadNo>=m_pObj->m_PadNum))
		{
			while(Num<RecordNum)
			{
				fp.Seek (Num*Block,CFile::begin);
				fp.Read (Value,Block);
				Num++;
				for(i=StPt;i<EnPt;i++)
				{
					Value[i]=(float)((Value[i]-lgMin)/fdata*1024.);
					m=(int)(Value[i]*300.f/1024.f);
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
		else
		{
			while(Num<RecordNum)
			{
				fp.Seek (Num*Block,CFile::begin);
				fp.Read (Value,Block);
				Num++;
				i=m_SelPadNo*m_pObj->m_ButtonNum+m_SelButNo;
				Value[i]=(float)((Value[i]-lgMin)/fdata*1024.);
				m=(int)(Value[i]*300.f/1024.f);
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
		flag=TRUE;
	}
	else
	{
		m_MorMax=0.f;
		m_LitMin=0.f;
	}
	fp.Close ();delete Value;
	EndWaitCursor();
	return flag;
}

void CDataPoisDlg::OnCloseupCombo1() 
{
	// TODO: Add your control notification handler code here
	int nIndex = m_PadComCtrl.GetCurSel();
	if (nIndex == CB_ERR)
		return;
	else
	{
		if(m_SelPadNo==nIndex)
			return;
		else
		{	
			m_SelPadNo=nIndex;
			m_bDraw=GetFrequency();
			if(m_bDraw) DrawFrequency();
		}
	}
}

void CDataPoisDlg::OnCloseupCombo2() 
{
	// TODO: Add your control notification handler code here
	int nIndex = m_ButComCtrl.GetCurSel();
	if (nIndex == CB_ERR)
		return;
	else
	{
		if(m_SelButNo==nIndex)
			return;
		else
		{	
			m_SelButNo=nIndex;
			m_bDraw=GetFrequency();
			if(m_bDraw) DrawFrequency();
		}
	}
}

//�Զ�����쳣���
void CDataPoisDlg::OnBnClickedButton2()
{
	UpdateData();
	int SelButNo=m_SelButNo;
	int SelPadNo=m_SelPadNo;

	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");
	strcat(s,m_CurveName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(!fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
		return;

	int Num=m_pObj->m_PadNum*m_pObj->m_ButtonNum;
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	float *Value=new float[Num*RecordNum];
	//��ȡ������������
	fp.Seek (0L,CFile::begin);
	fp.Read (Value,Num*RecordNum*4);
	fp.Close ();

	//�������ָʾ��
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->m_szGuage="���";
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);
	pGuage->SetTitle("��ۼ��...");		

	m_szButNo="";
	CString szStr;
	float va0,va1;
	int i,j;
	int StPt,EnPt;
	if(m_SelPadNo>=m_pObj->m_PadNum)
	{
		StPt=0;EnPt=Num;
	}
	else
	{
		StPt=m_SelPadNo*m_pObj->m_ButtonNum;
		EnPt=StPt+m_pObj->m_ButtonNum;
	}
	pGuage->SetDepthRange(StPt*1.f,EnPt*1.f);
	for(j=StPt;j<EnPt;j++)
	{
		//�����ֵ
		va0=0.f;
		for(i=0;i<RecordNum;i++)
			va0+=Value[Num*i+j];
		va0/=RecordNum;
		//���������
		va1=0.f;
		for(i=0;i<RecordNum;i++)
			va1+=pow((Value[Num*i+j]-va0)*(Value[Num*i+j]-va0),0.5f);
		va1/=RecordNum;
		if(va1<m_MaxFre)
		{
			szStr.Format("%d",j);
			if(m_szButNo!="")
				m_szButNo+=",";
			m_szButNo+=szStr;
		}
		pGuage->SetDepth(j*1.f);
		if(pGuage->CheckCancelButton())
			break;
	}
	delete Value;
	delete pGuage;
	m_SelButNo=SelButNo;
	m_SelPadNo=SelPadNo;
	UpdateData(FALSE);
}
//�Զ���⼫��
void CDataPoisDlg::OnBnClickedButton4()
{
	UpdateData();
	int SelButNo=m_SelButNo;
	int SelPadNo=m_SelPadNo;
	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");strcat(s,m_CurveName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		int Num=m_pObj->m_PadNum*m_pObj->m_ButtonNum;
		//�������ָʾ��
		CGuage* pGuage = new CGuage(0.f,Num*1.f);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->ShowWindow(SW_SHOW);
		pGuage->SetTitle("������...");

		int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
		float *Value=new float[Num*RecordNum];
		//��ȡ������������
		fp.Seek (0L,CFile::begin);
		fp.Read (Value,Num*RecordNum*4);

		//Ѱ��ȫ����ÿ����������Сֵ
		int i,j,k;

		// K-Means������ȡÿ����������Сֵ
		int kmk=12;					//��������
		int kmp=2;					//���ݵ�ά��
		float **xMeans=new float *[kmp];
		for(i=0;i<kmp;i++)
			xMeans[i]=new float[RecordNum];

		float **cMeans = new float *[kmp];
		for(i=0;i<kmp;i++)
			cMeans[i]=new float[kmk];

		int *zMeans = new int[RecordNum];
		float *pWork = new float[RecordNum];

		for(j=0;j<RecordNum;j++)
			xMeans[1][j]=1.f;

		int StPt,EnPt;
		if(m_SelPadNo>=m_pObj->m_PadNum)
		{
			StPt=0;EnPt=Num;
		}
		else
		{
			StPt=m_SelPadNo*m_pObj->m_ButtonNum;
			EnPt=StPt+m_pObj->m_ButtonNum;
		}
		pGuage->SetDepthRange(StPt*1.f,EnPt*1.f);
		m_szButNo=_T("");
		float Min=FLT_MAX;
		float Max=FLT_MIN;

		for(k=StPt;k<EnPt;k++)
		{
			for(j=0;j<RecordNum;j++)
				xMeans[0][j]=Value[Num*j+k];

			if(Kmeans(xMeans,kmp,RecordNum, kmk, cMeans, zMeans, pWork)==0)
			{
				for(j=0;j<kmk;j++)
				{
					if(cMeans[0][j]>Max) 
						Max=cMeans[0][j];
					if(cMeans[0][j]<Min)
						Min=cMeans[0][j];
				}
			}

			pGuage->SetDepth(k*1.f);
			if(pGuage->CheckCancelButton()) 
				break;
		}

		for(i=0;i<kmp;i++)
			delete xMeans[i];
		delete xMeans;
		for(i=0;i<kmp;i++)
			delete cMeans[i];
		delete cMeans;
		delete zMeans;
		delete pWork;

		delete Value;
		delete pGuage;
		
		m_ScaleMin=Min;
		m_ScaleMax=Max;
		m_SelButNo=SelButNo;
		m_SelPadNo=SelPadNo;
		UpdateData(FALSE);
	}
	fp.Close ();
}

void CDataPoisDlg::OnPaint()
{
	CDialog::OnPaint();
	if(m_bDraw) 
	{
		BeginWaitCursor();
		DrawFrequency();
		EndWaitCursor();
	}
}

void CDataPoisDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	if(DataPro())
	{
		m_bDraw=GetFrequency();
		if(m_bDraw) 
			DrawFrequency();
	}
}

BOOL CDataPoisDlg::DataPro()
{
	BeginWaitCursor();
	if(DataCopy(m_CurveName0,m_CurveName))
	{
		ButBalancePro(m_szBufPath,m_CurveName);
		if(m_bFilter) 
			DataFilterPro(m_szBufPath,m_CurveName);
		return TRUE;
	}
	EndWaitCursor();
	return FALSE;
}

//������������Ԥ����
BOOL CDataPoisDlg::DataCopy(CString InName,CString OutName)
{
	BOOL bOK=FALSE;
	CFIDIO* pFIDR = new CFIDIO(m_szBufPath+"."+InName);
	if(pFIDR->Open())
	{
		if(pFIDR->Copy(m_szBufPath+"."+OutName)!=-1)
			bOK=TRUE;
	}
	delete pFIDR;
	return bOK;
}

//�����ݽ���ƽ���˲�����
BOOL CDataPoisDlg::DataFilterPro(CString Path,CString Name)
{	
	BOOL bBreak=FALSE;
	char s[256],fn[256];
	strcpy(s,Path);
	strcat(s,".");strcat(s,Name);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
		float *Value=new float[m_DataNum*RecordNum];
		//��ȡ������������
		fp.Seek (0L,CFile::begin);
		fp.Read (Value,m_DataNum*RecordNum*4);

		//����ƽ���˲�����
		//�ȴ���С�������������ݣ���3�������˲�
		int j,k;
		for(k=0;k<m_DataNum;k++)
		{
			for(j=1;j<RecordNum-1;j++)
			{
				if(Value[m_DataNum*j+k]<m_ButMin)
					Value[m_DataNum*j+k]=(Value[m_DataNum*(j-1)+k]+Value[m_DataNum*(j+1)+k])/2.f;
				Value[m_DataNum*j+k]=0.212f*Value[m_DataNum*(j-1)+k]+0.576f*Value[m_DataNum*j+k]+0.212f*Value[m_DataNum*(j+1)+k];
			}
		}
		//����ƽ���˲�
		if(!bBreak)
		{
			for(j=0;j<RecordNum;j++)
			{
				for(k=1;k<m_DataNum-1;k++)
				{
					if(Value[m_DataNum*j+k]<m_ButMin)
						Value[m_DataNum*j+k]=(Value[m_DataNum*j+k-1]+Value[m_DataNum*j+k+1])/2.f;
					Value[m_DataNum*j+k]=0.212f*Value[m_DataNum*j+k-1]+0.576f*Value[m_DataNum*j+k]+0.212f*Value[m_DataNum*j+k+1];
				}
			}
			fp.Seek (0L,CFile::begin);
			fp.Write(Value,m_DataNum*RecordNum*4);
		}
		delete Value;
	}
	else bBreak=TRUE;
	fp.Close ();
	return !bBreak;
}

//ָ������쳣����
BOOL CDataPoisDlg::ButBalancePro(CString Path,CString Name)
{
	if(m_szButNo=="") return FALSE;
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	char s[256],fn[256];
	strcpy(s,Path);
	strcat(s,".");strcat(s,Name);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	BOOL bUserBreak=FALSE;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		//�������ָʾ��
		CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
		pGuage->Create(CWnd::GetActiveWindow());	
		pGuage->ShowWindow(SW_SHOW);
		pGuage->SetTitle(Name+" ����쳣ֵ����..");	
		int Block=m_DataNum*4;
		float *Value=NULL;
		Value=new float[m_DataNum+10];

		//���ָ���ļ����ۺ��Ƿ�Ϸ�
		float *pBut=NULL;
		pBut=new float[m_DataNum+10];
		int i;
		for(i=0;i<m_DataNum+10;i++)
			pBut[i]=-999.25f;
			
		int BNo,xxx;
		CString s1,s2;
		s1=m_szButNo;
		s1.TrimLeft ();
		s1.TrimRight ();
		while(s1!="")
		{		
			xxx=s1.Find(",");
			if(xxx>0)
			{
				s2=s1.Left(xxx);
				s1=s1.Mid(xxx+1);
			}
			else
			{
				s2=s1;
				s1="";
			}
			BNo=atoi(s2)-1;		
			if(BNo<0)  continue;
			if(BNo>m_DataNum-1)  continue;
			pBut[BNo]=999.25f;
		}
		
		int lp,rp,n,k=0;	
		float dep=m_StProDep;
		while(k<RecordNum)
		{
			fp.Seek (k*Block,CFile::begin);
			fp.Read (Value,Block);
			//�����쳣���ֵ				
			for(i=0;i<m_DataNum;i++)
			{
				if(pBut[i]>0.f)
				{
					lp=i;n=0;
					do{
						lp--;
						if(lp<0) 
							lp=m_DataNum+lp;
						n++;
					}while(n<m_DataNum&&pBut[lp]>0.f);
					rp=i;n=0;
					do{
						rp++;
						if(rp>m_DataNum-1) 
							rp=rp%m_DataNum;
						n++;
					}while(n<m_DataNum&&pBut[rp]>0.f);
					if(n<m_DataNum)
						Value[i]=(Value[lp]+Value[rp])/2.f;
				}
			}
			fp.Seek (k*Block,CFile::begin);
			fp.Write (Value,Block);	

			k++;
			dep=m_StProDep+m_Rlev*k;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton())
			{
				bUserBreak=TRUE;
				break;
			}
		}
		delete pGuage;delete pBut;delete Value;
	}
	else bUserBreak=TRUE;
	fp.Close ();
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

void CDataPoisDlg::OnBnClickedCheck7()
{
	UpdateData();
	m_bDataPro=TRUE;
}

void CDataPoisDlg::OnBnClickedCheck1()
{
	UpdateData();
	m_bDataPro=TRUE;
	GetDlgItem(IDC_EDIT12)->EnableWindow(m_bFilter);
}

void CDataPoisDlg::OnEnChangeEdit1()
{
	UpdateData();
	m_bDataPro=TRUE;
}

void CDataPoisDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	m_grippie.OnParentSize();
}

int CDataPoisDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_grippie.Create(this);
	return 0;
}

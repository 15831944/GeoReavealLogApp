// HoleAnalysis.cpp : implementation file
//

#include "stdafx.h"
#include "FlowChart.h"
#include "HoleAnalysis.h"
#include "FIDio.h"
#include "FitCircle.h"
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
	m_points=NULL;
}


void CHoleAnalysis::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHoleAnalysis)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Text(pDX, IDC_STATIC_DEP, m_DepStr);
	DDX_Text(pDX, IDC_EDIT1, m_Dep);
	DDX_Text(pDX, IDC_EDIT8, m_MaxValue);
	DDX_Text(pDX, IDC_EDIT9, m_ComputeRadius);
	DDX_Text(pDX, IDC_EDIT10, m_ScaleRadius);
	DDX_Text(pDX, IDC_EDIT11, m_ScaleK);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHoleAnalysis, CDialog)
	//{{AFX_MSG_MAP(CHoleAnalysis)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHoleAnalysis message handlers

BOOL CHoleAnalysis::OnInitDialog() 
{
	m_MaxValue=-5.f;
	GetPoint();
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHoleAnalysis::DrawHole(CDC *pDC)
{
	CRect clientrect;
	GetClientRect(&clientrect);	//获得客户区的矩形大小	
	COLORREF Color=RGB(255,255,255);
	CPen* pOldPen;
	Color=0x0;//0xB75B30;
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(Color);
	CString str;

	CRect rectRib=clientrect;
	rectRib.left=40;
	rectRib.top=10;
	rectRib.bottom-=25;
	rectRib.right=rectRib.left+rectRib.Height ();
	//横刻度细线
	CBrush *pOldBrush=(CBrush *)pDC->SelectStockObject (NULL_BRUSH);
	pDC->Rectangle(&rectRib);
	int i,x,y;

	CPen m_Pen2(0,1,Color);
	pOldPen = pDC->SelectObject(&m_Pen2);
	//横刻度值
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
		//绘拟合圆
		CPen Pen3(0,1,RGB(0,255,0));
		pDC->SelectObject(&Pen3);
		if(m_points!=NULL)
		{
			rect.left =(int)(m_fCenterX-m_fRadius);
			rect.top =(int)(m_fCenterY-m_fRadius);
			rect.right =(int)(m_fCenterX+m_fRadius);
			rect.bottom =(int)(m_fCenterY+m_fRadius);
			pDC->Ellipse(rect);
			//绘拟合圆心
			rect.left =(int)(m_fCenterX-3);
			rect.top =(int)(m_fCenterY-3);
			rect.right =(int)(m_fCenterX+3);
			rect.bottom =(int)(m_fCenterY+3);
			pDC->Ellipse(rect);
	
			//绘实例半径数据点
			for(i=0;i<m_nNum;i++)
			{
				rect.left =m_points[i].x-1;
				rect.top =m_points[i].y-1;
				rect.right =m_points[i].x+1;
				rect.bottom =m_points[i].y+1;
				pDC->FillRect(rect,&br2);
			}
		}
	}
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
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
	CFIDIO* PadI= NULL;
	PadI=new CFIDIO(m_szAradFileName);
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
		float *Value=NULL;
		Value=new float[Num];
		if(m_points!=NULL) delete m_points;
		m_points=new CPoint[Num+10];
		CFitCircle FitCircle(Num);//最小二乘法拟合圆
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
		rectRib.left=40;
		rectRib.top=10;
		rectRib.bottom-=25;
		rectRib.right=rectRib.left+rectRib.Height ();

		CPoint pt;
		pt.x=(rectRib.left+rectRib.right)/2;
		pt.y=(rectRib.top +rectRib.bottom )/2;
		m_fScale=rectRib.Height ()/(m_MaxValue-m_MinValue);
		m_nNum=0;
		int nFitNum=0;
		for(int i=0;i<Num;i++)
		{
			if(Value[i]>1.f)
			{
				m_points[m_nNum].x=pt.x+(int)(sin(6.2831853*i/Num)*m_fScale*Value[i]);
				m_points[m_nNum].y=pt.y-(int)(cos(6.2831853*i/Num)*m_fScale*Value[i]);
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
			m_ComputeRadius=(float)(m_fRadius/m_fScale);
			m_ScaleK=(float)(m_ScaleRadius/m_ComputeRadius);
		//	m_ComputeRadius=((int)(m_ComputeRadius*1000.f))/1000.f;
		//	m_ScaleK=((int)(m_ScaleK*1000.f))/1000.f;
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

void CHoleAnalysis::OnButton1()
{
	UpdateData();
	GetPoint();
	Invalidate();
	UpdateData(FALSE);
}
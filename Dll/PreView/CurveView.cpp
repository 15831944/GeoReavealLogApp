// CurveView.cpp : 实现文件
//

#include "stdafx.h"
#include "PreView.h"
#include "CurveView.h"
#include "math.h"
#include "GeoCurve.h"
#include "CurveAttrDlg.h"
#include ".\curveview.h"

// CCurveView

IMPLEMENT_DYNCREATE(CCurveView, CScrollView)

CCurveView::CCurveView()
{
	m_CurveID=2399;
	m_WellPath="";
	m_CurveName="GR";
	MyBottom=0;
	MyLeft=0;
	TopInsert=18;
	LineLog=0;
	bReadCurve=false;
	LineColor=1;
	LineWidth=1;
	
	m_nScrollSize.cx = 10; 
    m_nScrollSize.cy = 10; 
}

CCurveView::~CCurveView()
{
	delete CurveData;
	CurveData=0;
}


BEGIN_MESSAGE_MAP(CCurveView, CScrollView)
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()


// CCurveView 绘图

void CCurveView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	InitData();
	SetScrollSizes(MM_TEXT, m_nScrollSize);	
}

void CCurveView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
	DrawCurve(pDC);
}

void CCurveView::DrawCurve(CDC *pDC)
{
	if(!bReadCurve) return;
	int rColor=0,gColor=0,bColor=0;	
	CString cs;
	CRect clientrect(0,0,m_nScrollSize.cx,m_nScrollSize.cy);

	clientrect.top-=2 ;
	clientrect.left +=1;
	CPen RedPen,GreenPen,BluePen;	//定义三支新笔
	GreenPen.CreatePen(PS_SOLID,1,RGB(0xC0, 0xC0, 0xC0)); //创建一支新笔(绿色)
	BluePen.CreatePen(PS_SOLID,1,0x008080); //创建一支新笔(兰色)

	switch(LineColor)
	{
		case 1:
			rColor=255; 
			break;
		case 2:
			gColor=255;
			break;
		case 3:
			bColor=255;
			break;
		default:
			break;
	}
	RedPen.CreatePen(PS_SOLID,LineWidth,RGB(rColor,gColor,bColor)); //创建一支新笔(红色),用于画曲线
	
	CPen* pOriginalPen;  //定义一个指向新笔的指针
	CPen Penx;
	Penx.CreatePen(PS_SOLID,2,0x848284);
	pOriginalPen=pDC->SelectObject(&Penx); //选择兰色笔
	MyRect.left=clientrect.left+MyLeft;
	MyRect.top=clientrect.top+2;
	MyRect.right=clientrect.right;
	MyRect.bottom=clientrect.bottom-MyBottom;
	pDC->Rectangle(&MyRect); //画一矩形,矩形内填充白色背景
	pDC->SelectStockObject(NULL_BRUSH); //设置空画刷
	pDC->SelectObject(&pOriginalPen);
	pOriginalPen=pDC->SelectObject(&RedPen); //选择红色笔
	MyRect.left++;
	CPoint current;
	//画横向比
	cs.Format("%s",m_CurveName);
	pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	pDC->TextOut(MyRect.left+(MyRect.right-MyRect.left)/2,TopInsert-1,cs);  //曲线名

	pDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
	cs.Format("%g",CurveMin);
	pDC->TextOut(MyRect.left+3,TopInsert-1,cs);  //左刻度
	
	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
	cs.Format("%g",CurveMax);
	pDC->TextOut(MyRect.right-3,TopInsert-1,cs);  //右刻度

	pDC->MoveTo(MyLeft,TopInsert);
	pDC->LineTo(MyRect.right,TopInsert);

	//画竖线
	pDC->SelectObject(&GreenPen); //选择绿色笔
	if(LineLog==0)
	{
		//线性，分为10等分
		for(int i=1;i<10;i++)
		{
			int x=MyRect.left+i*(MyRect.right-MyRect.left)/10;
			pDC->MoveTo(x,TopInsert);
			pDC->LineTo(x,MyRect.bottom);
		}
	}
	else
	{
		//只有当左刻度小于右刻度，下面程序才能运行正常
		for(int i=1;;i++)
		{
			double tp=CurveMin*pow(10.0,(double)i);
			if(tp > CurveMax) break;
			int ktp=(int)log10(tp);
			if(tp<1.0) ktp--;
			tp=pow(10.0,(double)ktp);
			int x=(int)(MyRect.left+(log10(tp)-log10(CurveMin))*(MyRect.right-MyRect.left)/(log10(CurveMax)-log10(CurveMin)));
			pDC->MoveTo(x,TopInsert);
			pDC->LineTo(x,MyRect.bottom);
		}
	}
	//画水平线
	pDC->SelectObject(&GreenPen); //选择绿色笔
	int ktp=(int)log10(Edep-Sdep);
	pDC->SetBkMode(TRANSPARENT);
	if(ktp > -1) //1米以上
	{
		double step=10.;//pow(10.0,(double)ktp);
		double sdep1=Sdep+step;
		sdep1=(int)(sdep1/step)*step;
		for(int i=0;;i++)
		{
			double dep=sdep1+i*step;
			if(dep>=Edep) break;
			int y=(int)(MyRect.top+(dep-Sdep)*(MyRect.bottom-MyRect.top)/(Edep-Sdep));
			pDC->MoveTo(MyRect.left,y);
			pDC->LineTo(MyRect.right,y);
			//写起止深度
			cs.Format("%g",dep);
			pDC->SetTextAlign(TA_CENTER | TA_BASELINE);
			pDC->TextOut(MyRect.right/2,y+6,cs); 
		}
	}

	MyRect.top+=TopInsert;
	MyRect.bottom-=1;
	MyRect.left-=1;
	MyRect.right-=1;
	int n1=(int)((Sdep-Stdep)/Rlev);
	int n2=(int)((Edep-Stdep)/Rlev)+1;
	//绘主曲线
	int x;
	pDC->SelectObject(&RedPen);
	float tp=CurveData[n1]; 
	if(LineLog)
	{
		if(tp<=1.1e-38) 
			tp=1.1e-38f;
		x=MyRect.left+(int)((log10(tp)-log10(CurveMin))*(MyRect.right-MyRect.left)/(log10(CurveMax)-log10(CurveMin)));
	}
	else
		x=MyRect.left+(int)((tp-CurveMin)*(MyRect.right-MyRect.left)/(CurveMax-CurveMin));
	int y=MyRect.top;
	pDC->MoveTo(x,y);
	for(int i=n1+1;i<n2;i++)
	{
		tp=CurveData[i];  //临时变量，保证buf变量不变
		if(LineLog)
		{
			if(tp<=1.1e-38) 
				tp=1.1e-38f;
			current.x=MyRect.left+(int)((log10(tp)-log10(CurveMin))*(MyRect.right-MyRect.left)/(log10(CurveMax)-log10(CurveMin)));
		}
		else
			current.x=MyRect.left+(int)((tp-CurveMin)*(MyRect.right-MyRect.left)/(CurveMax-CurveMin));
		current.y=MyRect.top+(i-n1)*(MyRect.bottom-MyRect.top)/(n2-n1-1);
		if(current.x<MyRect.left)
			current.x=MyRect.left;
		if(current.x>MyRect.right)
			current.x=MyRect.right;
		pDC->LineTo(current);
	}
	pDC->SelectObject(pOriginalPen);
}

// CCurveView 诊断

#ifdef _DEBUG
void CCurveView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CCurveView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

BOOL CCurveView::InitData()
{
	int i;
	if(m_CurveID>0)
	{
		CGeoCurve Curve;
		if(!Curve.InitData(m_szServer)) 
			return FALSE;
		if(!Curve.GetData(m_CurveID))
			return FALSE;
		m_CurveName=Curve.m_CurveName;
		Stdep=Curve.m_DepSt;
		Endep=Curve.m_DepEn;
		Rlev=Curve.m_DepRlev; 
		NumSam=(int)((Endep-Stdep+0.5*Rlev)/Rlev)+1;
		CurveData=new float[NumSam];
		
		if(Curve.m_DataType==2)
		{
			short *pData=new short[NumSam];
			Curve.GetDataByte(m_CurveID,NumSam*2,(char *)pData,"DataByte1");
			for(i=0;i<NumSam;i++)
				CurveData[i]=(float)pData[i];
			delete pData;
		}
		else
			Curve.GetDataByte(m_CurveID,NumSam*4,(char *)CurveData,"DataByte1");
		Curve.Disconnect();
	}
	
	Sdep=Stdep;
	Edep=Endep;
	bReadCurve=true;
	CRect rect;
	GetClientRect(&rect);
	m_nScrollSize.cx=rect.Width();
	m_nScrollSize.cy=(int)(Endep-Stdep)*10;

	typedef struct 
	{
		char CurveName[5];
		float left,right;
		int line_log;
	}CurAttrib;
	//预定曲线的左右刻度
	const NUMCUR=40;  //具有预定刻度的曲线数
	CurAttrib cur[NUMCUR]={
			{"CAL",4.0f,14.0f,0},
			{"CALI",4.0f,14.0f,0},
			{"GR",0.0f,150.0f,0},
			{"CGR",0.0f,150.0f,0},
			{"AC",90.0f,40.0f,0},
			{"DT",90.0f,40.0f,0},
			{"DTC",90.0f,40.0f,0},
			{"CNL",27.0f,-3.0f,0},
			{"NPHI",27.0f,-3.0f,0},
			{"APLC",27.0f,-3.0f,0},
			{"DEN",3.0f,2.0f,0},
			{"RHOB",3.0f,2.0f,0},
			{"RHOM",3.0f,2.0f,0},
			{"RT",2.0f,10000.0f,1},
			{"LLD",2.0f,10000.0f,1},
			{"RXO",2.0f,10000.0f,1},
			{"LLS",2.0f,10000.0f,1},
			{"SH",0.0f,100.0f,0},
			{"VSH",0.0f,100.0f,0},
			{"POR",0.0f,25.0f,0},
			{"PORT",0.0f,25.0f,0},
			{"CPOR",0.0f,25.0f,0},
			{"PORA",0.0f,25.0f,0},
			{"PORB",0.0f,25.0f,0},
			{"PORD",0.0f,25.0f,0},
			{"PORN",0.0f,25.0f,0},
			{"PORW",0.0f,25.0f,0},
			{"PERM",0.01f,1000.0f,1},
			{"CPER",0.01f,1000.0f,0},
			{"SW",0.0f,100.0f,0},
			{"CSW",0.0f,100.0f,0},
			{"SXO",0.0f,100.0f,0},
			{"U",0.0f,50.0f,0},
			{"TH",0.0f,50.0f,0},
			{"K",0.0f,10.0f,0},
			{"HAZI",0.0f,360.0f,0},
			{"AZIM",0.0f,360.0f,0},
			{"DEVI",0.0f,90.0f,0},
			{"DEV",0.0f,90.0f,0},
	};
	//从隐含的左右刻度值得到绘图的左右刻度值，如果没有则用最大和最小值
	bool bLeft_Right=FALSE;
	CString cs;
	for(i=0;i<NUMCUR;i++)
	{
		cs=cur[i].CurveName;
		if(m_CurveName==cs)
		{
			CurveMin=cur[i].left;
			CurveMax=cur[i].right;
			LineLog=cur[i].line_log;
			bLeft_Right=true;
			break;
		}
	}
	if(!bLeft_Right)
	{
		//找出最小和最大值，作为横向刻度比例
		float varmin=3.4e38f;  //取最小值变量为正的最大浮点数
		float varmax=-3.4e38f; //取最大值变量为负的最大浮点数
		int Num=0;
		for(i=0;i<NumSam;i++)
		{
			if(CurveData[i]==-9999.0)continue;
			if(CurveData[i]==-99999.0)continue;
			if(CurveData[i]==-999.25)continue;
			Num++;
			if(Num==1)
			{
				CurveMin=CurveData[i];
				CurveMax=CurveData[i];
			}
			else
			{
				CurveMin=__min(CurveMin,CurveData[i]);
				CurveMax=__max(CurveMax,CurveData[i]);
			}
		}
	}
	return TRUE;
}

void CCurveView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CScrollView::OnRButtonUp(nFlags, point);

	CCurveAttrDlg dlg;
 	dlg.m_Sdep=Sdep;
 	dlg.m_Edep=Edep;
 	dlg.m_CurveMin=CurveMin;
 	dlg.m_CurveMax=CurveMax;
 	dlg.LineWidth=LineWidth;
 	dlg.LineColor=LineColor;
 	dlg.m_LineLog=LineLog;
 
 	if(dlg.DoModal()==IDOK)
 	{
 		Sdep=dlg.m_Sdep;
 		Edep=dlg.m_Edep;
 		CurveMin=dlg.m_CurveMin;
 		CurveMax=dlg.m_CurveMax;
 		LineWidth=dlg.LineWidth;
 		LineColor=dlg.LineColor;
 		LineLog=dlg.m_LineLog;
		Invalidate();	//重绘
 	}
}

BOOL CCurveView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int iScrollBarPos = GetScrollPos( SB_VERT )-zDelta;
	SetScrollPos( SB_VERT, iScrollBarPos );
	Invalidate();
	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}

void CCurveView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);
	m_nScrollSize.cx=cx;
	SetScrollSizes(MM_TEXT, m_nScrollSize);	
	Invalidate();
}

int CCurveView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

// RossDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageMan.h"
#include "CrossDlg.h"
#include "FrequencyDlg.h"
// CCrossDlg 对话框

IMPLEMENT_DYNAMIC(CCrossDlg, CDialog)
CCrossDlg::CCrossDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCrossDlg::IDD, pParent)
{
	m_szAliaseX = "电导率"; 
	m_szAliaseY = "图像值";
	m_AxiasTextColor =0x08000; 
	m_ScaleTextColor=0x08000;
	m_LogPen1.lopnWidth = CPoint(2,2); m_LogPen1.lopnColor  = 0x08000; m_LogPen1.lopnStyle  = 0;
	m_LogPen2.lopnWidth = CPoint(1,1); m_LogPen2.lopnColor = 0x08000;  m_LogPen2.lopnStyle = 0;
	m_LogPen3.lopnWidth = CPoint(1,1); m_LogPen3.lopnColor = 0xc0c0c0;  m_LogPen3.lopnStyle = 0;
	m_LogPen4.lopnWidth = CPoint(2,2); m_LogPen4.lopnColor = 0x0ff;  m_LogPen4.lopnStyle = 0;

	m_ScaleIncX = 1.f;
	m_ScaleTypeX = SCALE_LOG;
	m_StartX = 0.00001f;
	m_ScaleNumX = 4;

	m_ScaleIncY = 1.f;
	m_ScaleTypeY = SCALE_LOG;
	m_StartY = 2.f;
	m_ScaleNumY = 3;

	m_bkColor = RGB(255, 255, 255);

	MarginX	=60;
	MarginY	=60;
	
	LengthX = 600;
	LengthY = 450;
}

CCrossDlg::~CCrossDlg()
{
}

void CCrossDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCrossDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


void CCrossDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(nID == 1000)
		OnSendToClip();
	else if(nID == 1100)
		XFrequency();
	else if(nID == 1200)
		YFrequency();
	else if(nID == IDOK)
		OnOK();
	else
		CDialog::OnSysCommand(nID, lParam);
}

void CCrossDlg::OnPaint()
{
	CPaintDC dc(this);
	dc.FillSolidRect(MarginX,MarginY,LengthX,LengthY,m_bkColor);
	DrawAxias(&dc);
	DrawCrossData(&dc);
}

BOOL CCrossDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,WS_EX_TOOLWINDOW);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, 1100, "X轴直方图");
		pSysMenu->AppendMenu(MF_STRING, 1200, "Y轴直方图");
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, 1000, "发送到剪贴板");
		pSysMenu->AppendMenu(MF_STRING, IDOK, "确认");
	}

	m_nPos = CPoint(MarginX, LengthY + MarginY);
	m_nPosLT = CPoint(MarginX, MarginY);
	m_nPosRB = CPoint(MarginX + LengthX, MarginY+ LengthY);	

	LOGFONT LogFont=
	{
		-11,					    //lfHeight
		0,							//lfWidth
		0,							//lfEscapement
		0,							//lfOrientation
		FW_BOLD,					//lfWeight
		0,							//lfItalic
		0,							//lfUnderLine;
		0,							//lfStrikeOut;	
		ANSI_CHARSET,				//lfCharSet
		OUT_TT_ONLY_PRECIS,			//lfOutPrecision
		CLIP_DEFAULT_PRECIS,		//lfClipPrecision
		DEFAULT_QUALITY,			//lfQuality
		VARIABLE_PITCH | FF_SWISS,	//lfPitchAndFamily
		"Times New Roman"           //"MS Serif" "Courier New""Times New Roman"
	};
	m_ScaleTextFont=LogFont;
	strcpy(LogFont.lfFaceName,"微软雅黑");
	LogFont.lfWeight =FW_NORMAL;
	LogFont.lfHeight =-14;
	m_AxiasTextFont=LogFont;

	return TRUE;
}

void CCrossDlg::dtos(double value, char* ss)
{
	int i,j,k;
	sprintf(ss,"%10.4f",value);
	k = strlen(ss);
	j = 0;
	for (i=0; i<k; i++)
		if (ss[i] != ' ')
		{
			ss[j] = ss[i];
			j++;
		}
	ss[j] = '\0';
	while (ss[j-1] == '0') j--;
	if (ss[j-1] == '.') j--;
	ss[j] = 0;
}

void CCrossDlg::DrawAxias(CDC* pDC)
{
	int MaxX=m_nPosRB.x;
	int MinY=m_nPosLT.y;
	int zerH=30;			//直方图最大高度

    char ZerText [16];
	double xStart, xInc, xDx, xStart0, xDInc;
	xInc = m_ScaleIncX;
	if (m_ScaleTypeX ==SCALE_LOG) 
		xStart = log10(m_StartX);	
	else 
		xStart = m_StartX;	
	xDx = (double)LengthX / m_ScaleNumX;
	if (fabs(xStart/xInc)>0) 
		xStart0 =xInc* ((int)(xStart/xInc));
	else 
		xStart0 =xInc*((int)(xStart/xInc)) - xInc;
	xDInc = 0.1*xInc;
	
	double xv = xStart;  
	int x = m_nPos.x + (int) (xDx/xInc*(xv-xStart));
	
	CFont m_Font,*pOldFont;
	CPen m_Pen1,m_Pen2,m_Pen3,*pOldPen;
	m_Pen1.CreatePenIndirect(&m_LogPen1);
	m_Pen2.CreatePenIndirect(&m_LogPen2);
	m_Pen3.CreatePenIndirect(&m_LogPen3);

	LOGFONT LogFont=m_ScaleTextFont;
	m_Font.CreateFontIndirect(&LogFont);
	pOldFont = pDC->SelectObject(&m_Font);
	pDC->SetTextColor(m_ScaleTextColor);
	pDC->SetTextAlign(TA_CENTER|TA_TOP);			
	pDC->SetBkMode(TRANSPARENT);

    CSize textsize;
	textsize = pDC->GetTextExtent("文本");
	int space = 4;
	int x1,y1,i,xmax,ymax,y2;
    double xk = xv,yk,xxd,yj,xj,yyd;
	/*COLORREF Color[6]={0xFFFF00,0x008000,0xFF0000,0x008CFF,0x00D7FF,0x0000FF};*/
	COLORREF reColor[3]={0xFF0000,0x00D7FF,0x0000FF};
	//绘X轴直方图

	xmax=0;
	for(i=0;i<100;i++)
		if(m_CrossX[i]>xmax)
			xmax=m_CrossX[i];
	if(xmax>0)
	{
		xk=0;
		xxd=xDInc*m_ScaleNumX/10;
		pOldPen = pDC->SelectObject(&m_Pen1);
		for(i=0;i<100;i++)
		{
			x1=m_nPos.x +(int)(xDx/xInc*xk);
			xk += xxd;
			yj=(float)m_CrossX[i]/xmax*zerH;			
			pDC->MoveTo(x1,m_nPosLT.y - 5);
			if(yj>0)
			{
				y2=int (yj);
				pDC->LineTo(x1,m_nPosLT.y-5-y2);
				if(MinY>m_nPosLT.y-5-y2)
					MinY=m_nPosLT.y-5-y2;
			}	
		}
		pDC->SelectObject(pOldPen);
	}
	//绘X轴刻度线
	if (m_ScaleTypeX ==SCALE_LOG)
	{
		if(m_ScaleIncX<0)
			xDInc = 10;
		else
			xDInc = 1;
	}
	while (x<=m_nPosRB.x) 
	{
		if (x>=m_nPos.x) 
		{
			if(fabs((((int)(fabs(xv/xInc)+fabs(xInc/1000.0)))*fabs(xInc) - fabs(xv)))<fabs(xInc/100)) 
			{
				if (m_ScaleTypeX ==SCALE_LOG)	dtos(pow(10.0,xv), ZerText);
				else dtos(xv, ZerText);
	  			pDC->TextOut(x, m_nPos.y + space, ZerText); 
				if(x<m_nPosRB.x && x>m_nPos.x)
				{
					pOldPen = pDC->SelectObject(&m_Pen2);
					pDC->MoveTo(x, m_nPos.y);
					pDC->LineTo(x, m_nPosLT.y);
					pDC->SelectObject(pOldPen);
				}
			}
			else if (x>m_nPos.x) 
			{
				pOldPen = pDC->SelectObject(&m_Pen3);
			    if (x<m_nPosRB.x) 
				{
					pDC->MoveTo(x, m_nPos.y);
					pDC->LineTo(x, m_nPosLT.y);
				}
				else if ((m_ScaleTypeX ==SCALE_LOG)&& (x<m_nPosRB.x))
				{
					pDC->MoveTo(x, m_nPos.y);
					pDC->LineTo(x, m_nPosLT.y);
				}
				else
				{
					pDC->MoveTo(x, m_nPos.y);
					pDC->LineTo(x, m_nPos.y);
					pDC->MoveTo(x, m_nPosLT.y);
					pDC->LineTo(x, m_nPosLT.y);
				}
				pDC->SelectObject(pOldPen);
			}
		}
		if (m_ScaleTypeX ==SCALE_LOG)	
		{
			if(m_ScaleIncX<0)
			{
				xDInc--;
				xv += (log10(xDInc+1) - log10(xDInc))*xInc;
				if (xDInc == 1) xDInc = 10;
			}
			else
			{
				xDInc++;
				xv += (log10(xDInc) - log10(xDInc-1))*xInc;
				if (xDInc == 10) xDInc = 1;
			}
		}
		else xv += xDInc*2;  
		x = m_nPos.x + (int) (xDx/xInc*(xv-xStart));
	}
		
	double yStart, yInc, yDx, yStart0, yDInc;
	yInc = m_ScaleIncY;	
	if(m_ScaleTypeY ==SCALE_LOG) 
		yStart = log10(m_StartY);	
	else 
		yStart = m_StartY;	
	yDx = (double)LengthY / m_ScaleNumY;
	if((yStart/yInc)>0) 
		yStart0 =yInc* ((int)(yStart/yInc+0.3));
	else 
		yStart0 =yInc*((int)(yStart/yInc)) - yInc;
	yDInc = 0.1*yInc;
	double yv = yStart0;     
	int y = m_nPos.y - (int) (yDx/yInc*(yv-yStart));
	//绘Y轴直方图
	ymax=0;
	for(i=0;i<100;i++)
		if(m_CrossY[i]>ymax)
			ymax=m_CrossY[i];
	if(ymax>0)
	{	
		yk=0;
		yyd=yDInc*m_ScaleNumY/10;
		pOldPen = pDC->SelectObject(&m_Pen1);
		for(i=0;i<100;i++)
		{	
			y1 = m_nPos.y - (int) (yDx/yInc*yk);
			yk += yyd;
			xj=(float)m_CrossY[i]/ymax*zerH;
			pDC->MoveTo(m_nPosRB.x+5,y1);
			if(xj>0)
			{
				x1=int (xj);
				pDC->LineTo(m_nPosRB.x+x1+5,y1);
				if(m_nPosRB.x+x1+5>MaxX)
					MaxX=m_nPosRB.x+x1+5;
			}				
		}
		pDC->SelectObject(pOldPen);
	}
	//绘Y轴刻度线
	if(m_ScaleTypeY ==SCALE_LOG)
	{
		if(m_ScaleIncY<0)
			yDInc = 10;
		else 
			yDInc = 1;
	}
	m_Font.DeleteObject();
	LogFont.lfEscapement = -2700;
	m_Font.CreateFontIndirect(&LogFont);
	pDC->SelectObject(&m_Font);
	pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	while (y>=m_nPosLT.y) 
	{
		if (y<=m_nPos.y) 
		{
			if (fabs((((int)(fabs(yv/yInc)+fabs(yInc/1000.)))*fabs(yInc) - fabs(yv)))<fabs(yInc/100)) 
			{
				if (m_ScaleTypeY ==SCALE_LOG)
					dtos(pow(10.0,yv), ZerText);
				else dtos(yv, ZerText);
	  			pDC->TextOut(m_nPos.x - space, y, ZerText);
				if(y>m_nPosLT.y&&y<m_nPos.y) 
				{
					pOldPen = pDC->SelectObject(&m_Pen2);
					pDC->MoveTo(m_nPos.x, y);
					pDC->LineTo(m_nPosRB.x, y);
					pDC->SelectObject(pOldPen);
				}
			}
			else if (y<m_nPos.y)
			{
				pOldPen = pDC->SelectObject(&m_Pen3);
			    if(y>m_nPosLT.y)
				{
					pDC->MoveTo(m_nPos.x, y);
					pDC->LineTo(m_nPosRB.x, y);
				}
				else if((m_ScaleTypeY ==SCALE_LOG)&& (y>m_nPosLT.y))
				{
					pDC->MoveTo(m_nPos.x, y);
					pDC->LineTo(m_nPosRB.x, y);
				}
				else
				{
					pDC->MoveTo(m_nPos.x, y);
					pDC->LineTo(m_nPos.x, y);
					pDC->MoveTo(m_nPosRB.x, y);
					pDC->LineTo(m_nPosRB.x, y);
				}
				pDC->SelectObject(pOldPen);
			}
		}
		if (m_ScaleTypeY ==SCALE_LOG)	
		{
			if(m_ScaleIncY<0)
			{
				yDInc--;
				yv += (log10(yDInc+1) - log10(yDInc))*yInc;
				if (yDInc == 1) yDInc = 10;
			}
			else
			{
				yDInc++;
				yv += (log10(yDInc) - log10(yDInc-1))*yInc;
				if (yDInc == 10) yDInc = 1;
			}
		}
		else yv += yDInc*2;  
		y = m_nPos.y - (int) (yDx/yInc*(yv-yStart));
	}
	
	//绘边框线
	space += textsize.cy + 8;
	pDC->SelectStockObject(NULL_BRUSH);
	pOldPen = pDC->SelectObject(&m_Pen1);
	CPoint nPos1 = m_nPosRB;
	nPos1.x += m_LogPen1.lopnWidth.x;
	nPos1.y += m_LogPen1.lopnWidth.y;
	pDC->MoveTo(m_nPosLT);
	pDC->LineTo(m_nPosLT.x,nPos1.y);
	pDC->LineTo(nPos1);
	pDC->LineTo(nPos1.x,m_nPosLT.y);
	pDC->LineTo(m_nPosLT);
	pDC->SelectObject(pOldPen);
	//绘X轴别名
	m_Font.DeleteObject();
	LogFont=m_AxiasTextFont;
	m_Font.CreateFontIndirect(&LogFont);
	pOldFont = pDC->SelectObject(&m_Font);
	textsize = pDC->GetTextExtent("文本");
	pDC->SetTextColor(m_AxiasTextColor);
	pDC->SetTextAlign(TA_CENTER|TA_TOP);
	pDC->TextOut((m_nPosLT.x + m_nPosRB.x)/2, m_nPosRB.y+space, m_szAliaseX);				 	
	//绘Y轴别名
	m_Font.DeleteObject();
	LogFont.lfEscapement = -2700;
	m_Font.CreateFontIndirect(&LogFont);
	pDC->SelectObject(&m_Font);
	pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	pDC->TextOut(m_nPosLT.x - space, (m_nPosLT.y + m_nPosRB.y) / 2, m_szAliaseY);

	m_Font.DeleteObject();
	pDC->SelectObject(pOldFont);
}
void CCrossDlg::DrawCrossData(CDC* pDC)
{	
	/*COLORREF Color[6]={0xFFFF00,0x008000,0xFF0000,0x008CFF,0x00D7FF,0x0000FF};*/
	COLORREF Color[3]={0xFF0000,0x00D7FF,0x0000FF};
	double dx = 0.01*(m_nPosRB.x-m_nPosLT.x);
	double dy = 0.01*(m_nPosRB.y-m_nPosLT.y);
	int i,j,k;
	int x1, y1,x2,y2;
	COLORREF leftCr=GetSysColor(COLOR_3DLIGHT);
	COLORREF bottomCr=GetSysColor(COLOR_3DSHADOW);
	for(k=0;k<3; k++) 
	{
		for(i=0; i<100; i++) 
		{
			x1 = int(m_nPosLT.x + i*dx);
			x2 = int(x1+dx);
			if (x2 == x1) x2 = x1+1;
			for(j=0; j<100; j++) 
			{
				y1 = (int) (m_nPosRB.y - j*dy); 
				y2 = int(y1 - dy);
				if (y2 == y1) y2= y1-1;
			    if(m_CrossData[k][i][j]>0) 
				{
				/*	pDC->Draw3dRect(x1-2,y1-2,5,5,leftCr,bottomCr);*/
					pDC->FillSolidRect(x1-1,y1-1,3,3,Color[k]);
				}
			}
		}
	}
}

void CCrossDlg::OnSendToClip()
{
	CClientDC dc(this);
	CRect EmfRect;
	GetClientRect(&EmfRect);	//获得客户区的矩形大小

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bm;
	CSize sz(EmfRect.Width(), EmfRect.Height());
	bm.CreateCompatibleBitmap(&dc, sz.cx, sz.cy);
	CBitmap * oldbm = memDC.SelectObject(&bm);
	memDC.FillSolidRect(EmfRect,0xffffff);

	DrawAxias(&memDC);
	DrawCrossData(&memDC);

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
void CCrossDlg::XFrequency()
{
	CFrequencyDlg dlg;
	dlg.m_MinValue=m_StartX;
	if(m_ScaleTypeX==SCALE_LOG) 
		dlg.m_MaxValue= m_StartX*pow(10.f,m_ScaleIncX*m_ScaleNumX);
	else 
		dlg.m_MaxValue= m_StartX+m_ScaleIncX*m_ScaleNumX;
	dlg.m_LineLog=m_ScaleTypeX;
	dlg.m_Point=0;
	for(int i=0;i<100;i++)
	{
		dlg.m_freq[i]=m_CrossX[i];
		dlg.m_Point+=m_CrossX[i];
	}
	dlg.DoModal();
}
void CCrossDlg::YFrequency()
{
	CFrequencyDlg dlg;
	dlg.m_MinValue=m_StartY;
	if(m_ScaleTypeY==SCALE_LOG) 
		dlg.m_MaxValue= m_StartY*pow(10.f,m_ScaleIncY*m_ScaleNumY);
	else 
		dlg.m_MaxValue= m_StartY+m_ScaleIncY*m_ScaleNumY;
	dlg.m_LineLog=m_ScaleTypeY;
	dlg.m_ImageMin=m_ImageMin;
	dlg.m_ImageMax=m_ImageMax;
	dlg.m_Point=0;
	for(int i=0;i<100;i++)
	{
		dlg.m_freq[i]=m_CrossY[i];
		dlg.m_Point+=m_CrossY[i];
	}
	dlg.DoModal();
}
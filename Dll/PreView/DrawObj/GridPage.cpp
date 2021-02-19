#include "stdafx.h"
#include "PreView.h"
#include <math.h>  
#include "GridPage.h" 
 
/////////////////////////////////////////////////////////////////////////////
// CGridPage dialog

static long Round(double x)
{
	return (long) (x+0.5);
}

CGridPage::CGridPage(CGridObj* pGridObj)
	: CPropertyPage(CGridPage::IDD)
{
	ASSERT(pGridObj != NULL);
	pObj = pGridObj;
	m_ZerX = NULL;
	//{{AFX_DATA_INIT(CGridPage)
	m_LineNum = 0;
	m_SelLineNo = 0;
	m_LineWidth = 0;
	m_bDrawHorLine = FALSE;
	m_bDrawVerLine = FALSE;
	m_LeftValue = 1.0;
	m_RightValue = 100.0;
	m_GridType = -1;
	m_bHoriOpaque = FALSE;
	m_bVertOpaque = FALSE;
	//}}AFX_DATA_INIT
}

void CGridPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);  		
	m_LineNum = m_Num[m_SelLineNo];
	m_LineWidth = m_Width[m_SelLineNo];
	m_LineColor.SetColor(m_Color[m_SelLineNo]);
	//{{AFX_DATA_MAP(CGridPage)
	DDX_Control(pDX, IDC_GRID_SPIN_LINENUM, m_LineNum_SpinCtrl);
	DDX_Control(pDX, IDC_GRID_SPIN_LINEWIDTH, m_LineWidth_SpinCtrl);
	DDX_Control(pDX, IDC_GRID_LINENUM, m_LineNumCtrl);
	DDX_Control(pDX, IDC_GRID_LINE_WIDTH, m_LineWidthCtrl);
	DDX_Control(pDX, IDC_GRID_LEFT_VALUE, m_LeftValueCtrl);
	DDX_Control(pDX, IDC_GRID_RIGHT_VALUE, m_RightValueCtrl);
	DDX_Text(pDX, IDC_GRID_LINENUM, m_LineNum);
	DDX_CBIndex(pDX, IDC_GRID_SELECT_ITEM, m_SelLineNo);
	DDX_Text(pDX, IDC_GRID_LINE_WIDTH, m_LineWidth);
	DDX_Check(pDX, IDC_GRID_HORLINE, m_bDrawHorLine);
	DDX_Check(pDX, IDC_GRID_VERLINE, m_bDrawVerLine);
	DDX_Text(pDX, IDC_GRID_LEFT_VALUE, m_LeftValue);
	DDX_Text(pDX, IDC_GRID_RIGHT_VALUE, m_RightValue);
	DDX_Radio(pDX, IDC_GRID_LINE_TYPE, m_GridType);
	DDX_Control(pDX, IDC_GRID_LINE_COLOR, m_LineColor);
	DDX_Check(pDX, IDC_HORI_OPAQUE, m_bHoriOpaque);
	DDX_Check(pDX, IDC_VERT_OPAQUE, m_bVertOpaque);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGridPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGridPage)
	ON_BN_CLICKED(IDC_GRID_LOG_TYPE, OnLogType)
	ON_BN_CLICKED(IDC_GRID_LINE_TYPE, OnLineType)
	ON_BN_CLICKED(IDC_GRID_DIP_TYPE, OnDipType)
	ON_EN_CHANGE(IDC_GRID_LINE_WIDTH, OnChangeLineWidth)
	ON_EN_CHANGE(IDC_GRID_LINENUM, OnChangeLinenum)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_GRID_VERLINE, OnVerline)
	ON_BN_CLICKED(IDC_GRID_HORLINE, OnHorline)
	ON_CBN_SELCHANGE(IDC_GRID_SELECT_ITEM, OnSelchangeSelectItem)
	//}}AFX_MSG_MAP
	ON_MESSAGE( CPN_SELENDOK, OnSelEndOK )
END_MESSAGE_MAP()                                    

/////////////////////////////////////////////////////////////////////////////
// CGridPage message handlers

LRESULT CGridPage::OnSelEndOK(UINT lParam, LONG wParam)
{
	// a color selection was made, update the 
	// appropriate member data.
	m_Color[m_SelLineNo] = m_LineColor.GetColor();
	Preview();
	return 0;
}

void CGridPage::OnSelchangeSelectItem()
{
    CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_GRID_SELECT_ITEM);
    int i = pBox->GetCurSel();
    if ((i != m_SelLineNo) && (i<5))
    {
		m_SelLineNo = i;
		if ((m_GridType != LINEGRID) && (i > 2))
		{
			m_LineNumCtrl.EnableWindow(FALSE);
			m_LineNum_SpinCtrl.EnableWindow(FALSE);
		}
		else
		{
			m_LineNumCtrl.EnableWindow(TRUE);
			m_LineNum_SpinCtrl.EnableWindow(TRUE);
			SetNumRange();
		}
		SetWidthRange();
		UpdateData(FALSE);
    	Preview();
    }	
}

void CGridPage::OnLogType()
{
	if(m_GridType == LOGGRID)
	{
		UpdateData(TRUE);
		Preview();
		return;
	}
	if (m_LeftValueCtrl.m_hWnd != NULL) 
		m_LeftValueCtrl.EnableWindow(TRUE);
	if (m_RightValueCtrl.m_hWnd != NULL) 
		m_RightValueCtrl.EnableWindow(TRUE);
	if (m_SelLineNo > 2)
	{
		if (m_LineNumCtrl.m_hWnd != NULL) 
			m_LineNumCtrl.EnableWindow(FALSE);
		if (m_LineNum_SpinCtrl.m_hWnd != NULL) 
			m_LineNum_SpinCtrl.EnableWindow(FALSE);
	}
	m_GridType = LOGGRID;
	UpdateData(FALSE);
	Preview();
}

void CGridPage::OnLineType()
{
	if (m_GridType == LINEGRID)
	{
		UpdateData(TRUE);
		Preview();
		return;
	}
	if (m_LeftValueCtrl.m_hWnd != NULL) 
		m_LeftValueCtrl.EnableWindow(FALSE);
	if (m_RightValueCtrl.m_hWnd != NULL) 
		m_RightValueCtrl.EnableWindow(FALSE);
	if (m_SelLineNo > 2)
	{
		if (m_LineNumCtrl.m_hWnd != NULL) 
			m_LineNumCtrl.EnableWindow(TRUE);
		if (m_LineNum_SpinCtrl.m_hWnd != NULL) 
			m_LineNum_SpinCtrl.EnableWindow(TRUE);
	}
	m_GridType = LINEGRID;
	UpdateData(FALSE);
	Preview();
}

void CGridPage::OnDipType()
{
	if(m_GridType == DIPGRID)
	{
		UpdateData(TRUE);
		Preview();
		return;
	}
	if (m_LeftValueCtrl.m_hWnd != NULL) 
		m_LeftValueCtrl.EnableWindow(FALSE);
	if (m_RightValueCtrl.m_hWnd != NULL) 
		m_RightValueCtrl.EnableWindow(FALSE);
	if(m_SelLineNo > 2)
	{
		if (m_LineNumCtrl.m_hWnd != NULL) 
			m_LineNumCtrl.EnableWindow(FALSE);
		if (m_LineNum_SpinCtrl.m_hWnd != NULL) 
			m_LineNum_SpinCtrl.EnableWindow(FALSE);
	}
	m_GridType = DIPGRID;
	UpdateData(FALSE);
	Preview();
}

void CGridPage::OnChangeLineWidth()
{
	int i = GetDlgItemInt(IDC_GRID_LINE_WIDTH);
	if (m_Width[m_SelLineNo] == i) return;
	m_Width[m_SelLineNo] = i;
	Preview();
}

void CGridPage::SetWidthRange()
{
	int m_Min = 0; int m_Max = 10;
    if ((m_SelLineNo == 1) || (m_SelLineNo == 2) || (m_SelLineNo == 4))
    	m_Min = m_Width[m_SelLineNo-1];
    if ((m_SelLineNo == 0) || (m_SelLineNo == 1) || (m_SelLineNo == 3))
    	m_Max = m_Width[m_SelLineNo+1];
	if (m_LineWidth_SpinCtrl.m_hWnd != NULL) 
		m_LineWidth_SpinCtrl.SetRange(m_Min,m_Max);
}

void CGridPage::SetNumRange()
{
	int m_Min,m_Max;
	m_Min = 1; m_Max = 1000;
	if ((m_SelLineNo > 2) && (m_GridType != LINEGRID)) return;
	if ((m_SelLineNo == 1) || (m_SelLineNo == 2)) 
		m_Min = m_Num[m_SelLineNo-1];
    if ((m_SelLineNo == 0) || (m_SelLineNo == 1)) 
		m_Max = m_Num[m_SelLineNo+1];       
	if (m_SelLineNo == 3) m_Min = m_Num[4];
	if (m_SelLineNo == 4) m_Max = m_Num[3];
	if (m_LineNum_SpinCtrl.m_hWnd != NULL) 
		m_LineNum_SpinCtrl.SetRange(m_Min,m_Max);
}

void CGridPage::OnChangeLinenum()
{
	int i = GetDlgItemInt(IDC_GRID_LINENUM);
	if (m_Num[m_SelLineNo] == i) return;
	m_Num[m_SelLineNo] = i;
	Preview();
}

int CGridPage::HitTest(CPoint& point)
{
	ClientToScreen(&point);
	CWnd* pWnd = GetDlgItem(IDC_TRACK_GRID);
	pWnd->Invalidate();                           
	pWnd->UpdateWindow();
	CRect rect ;
	pWnd->GetClientRect(&rect);                   
	rect.top += 20;	rect.left += 10; rect.right -= 10; rect.bottom  -= 10;
	pWnd->ScreenToClient(&point);
	int lineno;
	int a = 3;
	if (rect.PtInRect(point))
	{     
    	if (m_bDrawHorLine)
    	{
			int y;
			float dy = ((float) rect.Height()) / (m_Num[2] + 2);
	    	for (int i=1; i<m_Num[2]+2; i++)
		    {
				y = rect.top + (int)(i*dy+0.5);
				if ((point.y < y+a) && (point.y > y-a)) 
	            {
					if ((i % m_Num[2]) == 0) {lineno = 2; return 2;}
					else if ((i % m_Num[1]) == 0) {lineno = 1; return 1;}
					else if ((i % m_Num[0]) == 0) {lineno =0; return 0;}
	            }   
    	    }    
   		}
		if (m_bDrawVerLine)
		{                                                     
			for (int i=1;i<m_VerLine-1; i++)
			{               
				int x = m_ZerX[i];
				if (x < 0) x = -m_ZerX[i];
				if ((point.x < x+a) && (point.x > x-a))
				{
					if (m_ZerX[i] < 0) lineno = 4;
					else lineno = 3;
					return lineno;
				}	
			}
		}
	}                    
	return -1;
}

void CGridPage::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPropertyPage::OnMouseMove(nFlags, point);
	int i = HitTest(point);
	if (i != -1) 
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_UPARROW));
}

void CGridPage::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	CPropertyPage::OnLButtonDown(nFlags, point);
	c_down = point;
}

void CGridPage::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	CPropertyPage::OnLButtonUp(nFlags, point);
	lineno = HitTest(point);
	if ((lineno != m_SelLineNo) && (lineno != -1))
	{
		m_SelLineNo = lineno;
		if ((m_GridType != LINEGRID) && (m_SelLineNo > 2))
		{
			if (m_LineNumCtrl.m_hWnd != NULL) m_LineNumCtrl.EnableWindow(FALSE);
			if (m_LineNum_SpinCtrl.m_hWnd != NULL) m_LineNum_SpinCtrl.EnableWindow(FALSE);
		}
		else
		{
			if (m_LineNumCtrl.m_hWnd != NULL) m_LineNumCtrl.EnableWindow(TRUE);
			if (m_LineNum_SpinCtrl.m_hWnd != NULL) m_LineNum_SpinCtrl.EnableWindow(TRUE);
			SetNumRange();
		}
		SetWidthRange();
		UpdateData(FALSE);
		Preview();
	}
}

BOOL CGridPage::OnInitDialog()
{	
	m_Num[0] = pObj->m_DepLine1;
	m_Num[1] = pObj->m_DepLine2;      
	m_Num[2] = pObj->m_DepLine3;
	m_Num[3] = pObj->m_VerLine1;
	m_Num[4] = pObj->m_VerLine2;
	m_bDrawHorLine = pObj->m_bDrawDepLine;
	m_bDrawVerLine = pObj->m_bDrawVerLine;
	m_bHoriOpaque = pObj->m_bHoriOpaque;
	m_bVertOpaque = pObj->m_bVertOpaque;
	m_Width[0]= pObj->m_DepPen1.lopnWidth.x;
	m_Color[0] = pObj->m_DepPen1.lopnColor;
	m_Width[1]= pObj->m_DepPen2.lopnWidth.x;
	m_Color[1] = pObj->m_DepPen2.lopnColor;
	m_Width[2]= pObj->m_DepPen3.lopnWidth.x;
	m_Color[2] = pObj->m_DepPen3.lopnColor;
	m_Width[3]= pObj->m_VerPen1.lopnWidth.x;
	m_Color[3] = pObj->m_VerPen1.lopnColor;
	m_Width[4]= pObj->m_VerPen2.lopnWidth.x;
	m_Color[4] = pObj->m_VerPen2.lopnColor;

	m_SelLineNo = 0;
	m_LeftValue = pObj->m_LeftVal;
	m_RightValue = pObj->m_RightVal;
	m_GridType = pObj->m_GridType; 
	CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_GRID_SELECT_ITEM);
	CString szHT;
	szHT.LoadString(IDS_DEPTHTHINLINE);
	pBox->AddString(szHT);
	szHT.LoadString(IDS_DEPTHMIDLINE);
	pBox->AddString(szHT);
	szHT.LoadString(IDS_DEPTHBOLDLINE);
	pBox->AddString(szHT);
	szHT.LoadString(IDS_ZERTHINLINE);
	pBox->AddString(szHT);
	szHT.LoadString(IDS_ZERBOLDLINE);
	pBox->AddString(szHT);	
	CPropertyPage::OnInitDialog();
	if(m_GridType != LINEGRID)
	{
		if(m_GridType == DIPGRID)
		{
			m_LeftValueCtrl.EnableWindow(FALSE);
			m_RightValueCtrl.EnableWindow(FALSE);
		}
		else
		{
			if (m_LeftValueCtrl.m_hWnd != NULL) 
				m_LeftValueCtrl.EnableWindow(TRUE);
			if (m_RightValueCtrl.m_hWnd != NULL) 
				m_RightValueCtrl.EnableWindow(TRUE);
		}
	}
	else
	{
		if (m_LeftValueCtrl.m_hWnd != NULL) 
			m_LeftValueCtrl.EnableWindow(FALSE);
		if (m_RightValueCtrl.m_hWnd != NULL) 
			m_RightValueCtrl.EnableWindow(FALSE);
	}	
	if (m_LineWidth_SpinCtrl.m_hWnd != NULL) 
		m_LineWidth_SpinCtrl.SetRange(0,m_Width[1]);
	if (m_LineNum_SpinCtrl.m_hWnd != NULL) 
		m_LineNum_SpinCtrl.SetRange(1,m_Num[1]);

	return TRUE;
}

void CGridPage::CreateVerLine(CRect m_Rect)
{
	if (!m_bDrawVerLine) return;
	if (m_ZerX != NULL) delete m_ZerX;
	m_ZerX = NULL;
	if(m_GridType == LOGGRID)
	{
		int xinc = 1;
		int x;  
		double xv,xvl,xvr;
		m_ZerX = new int[100];
		if (m_LeftValue > m_RightValue) xinc = -1;
		if (m_LeftValue < 0.0001) m_LeftValue = 0.0001;
		if (m_RightValue < 0.0001) m_RightValue = 0.0001;
		xv = (int)log10(m_LeftValue);
		xvl = log10(m_LeftValue);	xvr = log10(m_RightValue);
		if (xinc*(xvl-xv) < 0)  xv -= xinc;
		int k = 1;   
		if (xinc < 0) k = 10;
		x = Round(m_Rect.left + (xv - xvl) / (xvr - xvl) * m_Rect.Width());
		m_ZerX[0] = -m_Rect.left; 
		m_VerLine = 1;
		while (x < m_Rect.right)
		{            
			x = Round(m_Rect.left + (xv - xvl) / (xvr - xvl) * m_Rect.Width());
			if (x > m_Rect.left)
			{
				if ((k == 1) || (k == 10))
					m_ZerX[m_VerLine] = -x;   
				else
					m_ZerX[m_VerLine] = x;
				m_VerLine++;
			}
			if (xinc > 0)
			{
				k += xinc;
				xv += (log10((double)k) - log10((double)k-1)) * xinc;
				if (k == 10) k = 1;
			}
			else
			{	
				xv += (log10((double)k) - log10((double)k-1)) * xinc;
				k += xinc;
				if (k == 1) k = 10;
			}
		}
		m_ZerX[m_VerLine] = -m_Rect.right;
		m_VerLine++;
	}
	else if (m_GridType == DIPGRID)
	{
		div_t x;
		int i,j,k,n;
		int m_ScalePara = 234;
	 	int m_ScaleArray[10];
		i = m_ScalePara;
		int m_ScaleNum = 0;
		m_VerLine = 0; n = 0;
		while (1)
		{
			x = div(i,10);
	    	i = x.quot;
		    if(x.rem == 0) break;
			m_ScaleArray[m_ScaleNum] = x.rem;
			n += x.rem;
			m_ScaleNum ++;
			if (x.quot == 0) m_VerLine += x.rem*5; else m_VerLine += x.rem*2;
		}
		if (n==0) return;                     
		m_VerLine += 1;
		m_ZerX = new int[m_VerLine];
		m_ZerX[0] = -m_Rect.left; m_ZerX[m_VerLine-1] = -m_Rect.right;
		int n1 = 0;		
		for (i=0; i<m_ScaleNum; i++)
		{
			n1 += Round(m_ScaleArray[i]*pow(2,i));			
		}                           
		float xAdd;
		float xx = (float) -m_ZerX[0];       
		int divNum;   
		n = 1;
		for (i=m_ScaleNum-1; i>=0; i--)
		{
			if (i == m_ScaleNum-1) divNum = 5; else divNum = 2;
			xAdd = (float)(pow(2,i) * m_Rect.Width() / n1 /divNum);
			for (j=0; j<m_ScaleArray[i]; j++)
			for (k=0; k<divNum; k++)	
			{              
				xx += xAdd;
				m_ZerX[n] = Round(xx);
				if (k == divNum-1) m_ZerX[n] = -m_ZerX[n];
				n ++;
			}
		}
	}
	else
	{
		m_VerLine = m_Num[3] + 1;
		m_ZerX = new int[m_VerLine];
		m_ZerX[0] = -m_Rect.left; m_ZerX[m_VerLine-1] = -m_Rect.right;
		for (int i=1;i<m_VerLine-1;i++)
		{               
			int xx = m_Rect.left + Round(((float)m_Rect.Width())/(m_VerLine-1)*i);
			if ((i % m_Num[4]) == 0) m_ZerX[i] = -xx;
			else m_ZerX[i] = xx;
		}
	}
}

void CGridPage::Preview()
{
	CPen m_Pen1,m_Pen2,m_Pen3;
	int x,y,k;
	CWnd* pWnd = GetDlgItem(IDC_TRACK_GRID);
	if (pWnd == NULL) return;
	pWnd->Invalidate();                           
	pWnd->UpdateWindow();
	CRect rect,rect1 ;
	pWnd->GetClientRect(&rect);                   
	CDC* pDC = pWnd->GetDC();
    CBrush br(0xffffff);
    rect1 = rect; rect1.top = rect.top + 10; rect1.bottom = rect.bottom-1; 
    rect1.right = rect.right -1; rect1.left = rect.left + 1;
    pDC->FillRect(rect1,&br);
	rect.top += 20;	rect.left += 10; rect.right -= 10; rect.bottom  -= 10;
	CPen* pOldPen,m_Pen;
	m_Pen.CreatePen(0,1,RGB(0,0,0));
	pOldPen = pDC->SelectObject(&m_Pen);
	pDC->MoveTo(rect.left,rect.top);
	pDC->LineTo(rect.left,rect.bottom); pDC->LineTo(rect.right,rect.bottom);
	pDC->LineTo(rect.right,rect.top); pDC->LineTo(rect.left,rect.top);
	     
//draw Horline                  
	if(m_bDrawHorLine)
	{               
		m_Pen1.CreatePen(0,m_Width[0],m_Color[0]);
		m_Pen2.CreatePen(0,m_Width[1],m_Color[1]);
		m_Pen3.CreatePen(0,m_Width[2],m_Color[2]);
		float dy = ((float) rect.Height()) / (m_Num[2] + 2);
    	for (int i=1; i<m_Num[2]+2; i++)
	    {
			if ((i % m_Num[2]) == 0) {pDC->SelectObject(&m_Pen3);k=8;}
			else if ((i % m_Num[1]) == 0) {pDC->SelectObject(&m_Pen2);k=4;}
			else if ((i % m_Num[0]) == 0) {pDC->SelectObject(&m_Pen1);k=0;}
			else continue;
			y = rect.top + (int)(i*dy+0.5);
			pDC->MoveTo(rect.left-k,y);
			pDC->LineTo(rect.right+k,y);
    	}
		m_Pen1.DeleteObject();
		m_Pen2.DeleteObject();
		m_Pen3.DeleteObject();
    }	
// Draw VerLine
	if (m_bDrawVerLine)
	{
		CreateVerLine(rect);
		m_Pen1.CreatePen(0,m_Width[3],m_Color[3]);
		m_Pen2.CreatePen(0,m_Width[4],m_Color[4]);
		for(int i=1;i<m_VerLine-1;i++)
		{               
			if (m_ZerX[i] < 0) 
			{ 
				x = -m_ZerX[i]; 				
				if(m_Width[4]>0)
				{
					pDC->SelectObject(&m_Pen2);
					pDC->MoveTo(x,rect.top);
					pDC->LineTo(x,rect.bottom);
				}
			}
			else 
			{
				x = m_ZerX[i]; 
				if(m_Width[3]>0)
				{
					pDC->SelectObject(&m_Pen1);
					pDC->MoveTo(x,rect.top);
					pDC->LineTo(x,rect.bottom);
				}
			}
		}
		m_Pen1.DeleteObject();
		m_Pen2.DeleteObject();
	}	
	pDC->SelectObject(pOldPen);
	m_Pen.DeleteObject();
	pWnd->ReleaseDC(pDC);
} 

void CGridPage::OnPaint()
{
	CPaintDC dc(this); // device context for painting
    Preview();
}


void CGridPage::OnVerline()
{
	// TODO: Add your control notification handler code here
	m_bDrawVerLine = !m_bDrawVerLine;
	UpdateData(FALSE);
	Preview();
}

void CGridPage::OnHorline()
{
	// TODO: Add your control notification handler code here
	m_bDrawHorLine = !m_bDrawHorLine;
	UpdateData(FALSE);
	Preview();
}

void CGridPage::OnOK()
{
	CPropertyPage::OnOK();
	pObj->m_DepLine1 = m_Num[0];
	pObj->m_DepLine2 = m_Num[1];      
	pObj->m_DepLine3 = m_Num[2];      
	pObj->m_VerLine1 = m_Num[3];      
	pObj->m_VerLine2 = m_Num[4];      
	pObj->m_bDrawDepLine = m_bDrawHorLine;
	pObj->m_bDrawVerLine = m_bDrawVerLine;
	pObj->m_DepPen1.lopnWidth.x = m_Width[0];
	pObj->m_DepPen1.lopnColor = m_Color[0];
	pObj->m_DepPen2.lopnWidth.x = m_Width[1];
	pObj->m_DepPen2.lopnColor = m_Color[1];
	pObj->m_DepPen3.lopnWidth.x = m_Width[2];
	pObj->m_DepPen3.lopnColor = m_Color[2];
	pObj->m_VerPen1.lopnWidth.x = m_Width[3];
	pObj->m_VerPen1.lopnColor = m_Color[3];
	pObj->m_VerPen2.lopnWidth.x = m_Width[4];
	pObj->m_VerPen2.lopnColor = m_Color[4];
	pObj->m_bHoriOpaque = m_bHoriOpaque;
	pObj->m_bVertOpaque = m_bVertOpaque;
	pObj->m_GridType = m_GridType; 
	pObj->m_LeftVal = m_LeftValue; 
	pObj->m_RightVal = m_RightValue; 
	delete m_ZerX;
	pObj->CreateVerLine();
}


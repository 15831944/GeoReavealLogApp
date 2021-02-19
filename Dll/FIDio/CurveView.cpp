// CurveView.cpp : implementation file
//

#include "stdafx.h"
#include "CurveView.h"
#include "math.h"
#include "AttrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCurveView dialog


CCurveView::CCurveView(char* CurverName,CWnd* pParent /*=NULL*/)
	: CDragDialog(CCurveView::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCurveView)
	//}}AFX_DATA_INIT
	CString FileName=CurverName;
	int xxx=FileName.ReverseFind ('>');
	if(xxx>0)
	{
		m_WellPath=FileName.Left (xxx);
		m_CurveName=FileName.Mid (xxx+1);
		int m_WPL=m_WellPath.GetLength();
		int m1=m_WPL;
		for(int i=m_WPL-1;i>=0;i--)
		{
			if(m_WellPath.GetAt(i)=='\\')
			{
				m1=i;
				break;
			}
		}
		m_WellName=m_WellPath.Mid(m1+1);
		m_IndexFile=m_WellPath.Left(m1)+"\\I"+m_WellName+"."+m_CurveName;
		m_DataFile=m_WellPath.Left(m1)+"\\D"+m_WellName+"."+m_CurveName;
	}
	else
	{
		m_WellPath="";
		m_CurveName="";
		m_WellName="";
		m_IndexFile="";
		m_DataFile="";
	}
	MyBottom=4;
	MyLeft=4;
	TopInsert=45;
	LineLog=0;
	bReadCurve=false;
	LineColor=1;
	LineWidth=1;
	m_Drawing=FALSE;  //��ʾ�ͻ�������������û�а���
}

void CCurveView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCurveView)
	DDX_Control(pDX, IDC_VALUE, m_Value);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCurveView, CDragDialog)
	//{{AFX_MSG_MAP(CCurveView)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_STYLE, OnStyle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCurveView message handlers


BOOL CCurveView::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CWndShadow::Initialize(AfxGetInstanceHandle());
	m_Shadow.Create(GetSafeHwnd());

	SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,WS_EX_TOOLWINDOW);
	CRect rect;
	// get the current window size and position
	GetWindowRect( rect );
	// now change the size, position, and Z order 
	// of the window.
	int h=rect.Height ();
	int w=rect.Width ();
	rect.left =GetSystemMetrics(SM_CXSCREEN)-w-10;
	rect.top =GetSystemMetrics(SM_CYSCREEN)-h-30;
	rect.bottom =GetSystemMetrics(SM_CYSCREEN)-30;
	rect.right =GetSystemMetrics(SM_CXSCREEN)-10;
	MoveWindow(&rect);
	// TODO: Add extra initialization here
	SetFadeWhileDragging(100);
	SetDragCursor(FID_IDC_FINGER);
	//��ȡ�����������ļ�
	CString cs;
	char s[1024],str1[1024],str2[1024],str3[1024];;
	FILE *fdIndex,*fdData;
	char IndexType[21],DataType[21];
	int IndexLen,DataLen;
	fdIndex=fopen(m_IndexFile,"r");
	if(fdIndex==NULL)
	{
		cs.Format("���ܴ������ļ�:%s",m_IndexFile);
		AfxMessageBox(cs);
		return TRUE;
	}
	fgets(str1,1024,fdIndex);
	fgets(str2,1024,fdIndex);
	fgets(str3,1024,fdIndex);
	fclose(fdIndex);
	sscanf(str1,"%s%d",s,&Dimension);
	sscanf(str2,"%s%s%s%s%s%s%s%d%s%s%s%s%s%f%s%f%s%f",
		  s,s,s,DeptUnits,s,IndexType,s,&DataLen,s,s,s,s,s,&Stdep,s,&Endep,s,&Rlev);
	sscanf(str3,"%s%s%s%s%s%s%s%d%s%s%s%s%s%f%s%f",
		  s,CurveName,s,CurveUnits,s,DataType,s,&IndexLen,s,s,s,s,s,&CurveMin,s,&CurveMax);
	
	if(stricmp(DataType,"float")!=0)
		return FALSE;
	NumSam=(int)((Endep-Stdep+0.5*Rlev)/Rlev)+1;
	CurveData=new float[NumSam];
	
	fdData=fopen(m_DataFile,"rb");
	if(fdData==NULL)
	{
		fclose(fdIndex);
		cs.Format("���ܴ������ļ�:%s",m_DataFile);
		AfxMessageBox(cs);
		return TRUE;
	}
	fread(CurveData,NumSam*sizeof(float),1,fdData);
	fclose(fdData);
	Sdep=Stdep;
	Edep=Endep;
	bReadCurve=true;

	typedef struct 
	{
		char CurveName[5];
		float left,right;
		int line_log;
	}CurAttrib;
	//Ԥ�����ߵ����ҿ̶�
	const NUMCUR=40;  //����Ԥ���̶ȵ�������
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
	//�����������ҿ̶�ֵ�õ���ͼ�����ҿ̶�ֵ�����û������������Сֵ
	bool bLeft_Right=FALSE;
	for(int i=0;i<NUMCUR;i++)
	{
		cs=cur[i].CurveName;
		if(CurveName==cs)
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
		//�ҳ���С�����ֵ����Ϊ����̶ȱ���
		float varmin=3.4e38f;  //ȡ��Сֵ����Ϊ������󸡵���
		float varmax=-3.4e38f; //ȡ���ֵ����Ϊ������󸡵���
		int Num=0;
		for(i=0;i<NumSam;i++)
		{
			if(CurveData[i]==-9999.0)continue;
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
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCurveView::OnFreshBtn()
{
	int rColor=0,gColor=0,bColor=0;
	CRect clientrect;
	CString cs;
	GetClientRect(&clientrect);	//��ÿͻ����ľ��δ�С
	clientrect.top +=20;
	CClientDC dc(this);	//����һ��DC����(�����豸������)
	CPen RedPen,GreenPen,BluePen;	//������֧�±�
	GreenPen.CreatePen(PS_SOLID,1,RGB(0xC0, 0xC0, 0xC0)); //����һ֧�±�(��ɫ)
	BluePen.CreatePen(PS_SOLID,1,RGB(0,0,255)); //����һ֧�±�(��ɫ)
	if(!bReadCurve)
	{
		dc.TextOut(150,150,"���˳�");
		return;
	}

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
	RedPen.CreatePen(PS_SOLID,LineWidth,RGB(rColor,gColor,bColor)); //����һ֧�±�(��ɫ),���ڻ�����
	
	CPen* pOriginalPen;  //����һ��ָ���±ʵ�ָ��
	pOriginalPen=dc.SelectObject(&BluePen); //ѡ����ɫ��
	MyRect.left=clientrect.left+MyLeft;
	MyRect.top=clientrect.top+2;
	MyRect.right=clientrect.right-3;
	MyRect.bottom=clientrect.bottom-MyBottom;
	dc.Rectangle(&MyRect); //��һ����,����������ɫ����
	dc.SelectStockObject(NULL_BRUSH); //���ÿջ�ˢ
	dc.SelectObject(&pOriginalPen);
	pOriginalPen=dc.SelectObject(&RedPen); //ѡ���ɫ��
	MyRect.left++;
	CPoint current;
	//�������
	cs.Format("%s",CurveName);
	dc.SetTextAlign(TA_CENTER|TA_BOTTOM);
	dc.TextOut(MyRect.left+(MyRect.right-MyRect.left)/2,TopInsert-1,cs);  //������

	dc.SetTextAlign(TA_LEFT|TA_BOTTOM);
	cs.Format("%g",CurveMin);
	dc.TextOut(MyRect.left+3,TopInsert-1,cs);  //��̶�
	
	dc.SetTextAlign(TA_RIGHT | TA_BOTTOM);
	cs.Format("%g",CurveMax);
	dc.TextOut(MyRect.right-3,TopInsert-1,cs);  //�ҿ̶�

	dc.MoveTo(MyLeft,TopInsert);
	dc.LineTo(MyRect.right,TopInsert);

	//������
	dc.SelectObject(&GreenPen); //ѡ����ɫ��
	if(LineLog==0)
	{
		//���ԣ���Ϊ10�ȷ�
		for(int i=1;i<10;i++)
		{
			int x=MyRect.left+i*(MyRect.right-MyRect.left)/10;
			dc.MoveTo(x,TopInsert);
			dc.LineTo(x,MyRect.bottom);
		}
	}
	else
	{
		//ֻ�е���̶�С���ҿ̶ȣ�������������������
		for(int i=1;;i++)
		{
			double tp=CurveMin*pow(10.0,(double)i);
			if(tp > CurveMax) break;
			int ktp=(int)log10(tp);
			if(tp<1.0) ktp--;
			tp=pow(10.0,(double)ktp);
			int x=(int)(MyRect.left+(log10(tp)-log10(CurveMin))*(MyRect.right-MyRect.left)/(log10(CurveMax)-log10(CurveMin)));
			dc.MoveTo(x,TopInsert);
			dc.LineTo(x,MyRect.bottom);
		}
	}
	MyRect.top+=TopInsert;
	MyRect.bottom-=1;
	MyRect.left-=1;
	MyRect.right-=1;
	
	dc.SelectObject(&RedPen); //ѡ���ɫ��
	int n1=(int)((Sdep-Stdep)/Rlev);
	int n2=(int)((Edep-Stdep)/Rlev)+1;

	for(int i=n1;i<n2;i++)
	{
		float tp=CurveData[i];  //��ʱ��������֤buf��������
		if(LineLog)
		{
			if(tp<=1.1e-38) tp=1.1e-38f;
			current.x=MyRect.left+(int)((log10(tp)-log10(CurveMin))*(MyRect.right-MyRect.left)/(log10(CurveMax)-log10(CurveMin)));
		}
		else
		{
			current.x=MyRect.left+(int)((tp-CurveMin)*(MyRect.right-MyRect.left)/(CurveMax-CurveMin));
		}
		current.y=MyRect.top+(i-n1)*(MyRect.bottom-MyRect.top)/(n2-n1-1);
		if(current.x<MyRect.left)current.x=MyRect.left;
		if(current.x>MyRect.right)current.x=MyRect.right;
		if(i==n1)
		{
			dc.MoveTo(current);
		}
		else
		{
			dc.LineTo(current);
		}
	}
	//��ˮƽ��
	dc.SelectObject(&GreenPen); //ѡ����ɫ��
	int ktp=(int)log10(Edep-Sdep);
	dc.SetBkMode(TRANSPARENT);
	if(ktp > -1) //1������
	{
		double step=pow(10.0,(double)ktp);
		double sdep1=Sdep+step;
		sdep1=(int)(sdep1/step)*step;
		for(int i=0;;i++)
		{
			double dep=sdep1+i*step;
			if(dep>=Edep) break;
			int y=(int)(MyRect.top+(dep-Sdep)*(MyRect.bottom-MyRect.top)/(Edep-Sdep));
			dc.MoveTo(MyRect.left,y);
			dc.LineTo(MyRect.right,y);
			//д��ֹ���
			cs.Format("%g",dep);
			dc.SetTextAlign(TA_CENTER | TA_BASELINE);
			dc.TextOut(MyRect.right/2,y+6,cs); 
		}
	}
	dc.SelectObject(pOriginalPen);
}

void CCurveView::CleanUp()
{
	delete CurveData;
	CurveData=0;
}

void CCurveView::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CleanUp();
	CDialog::OnClose();
}

void CCurveView::OnPaint() 
{
	CDialog::OnPaint();
	// TODO: Add your message handler code here
	OnFreshBtn();
	// Do not call CDragDialog::OnPaint() for painting messages
}

void CCurveView::OnSize(UINT nType, int cx, int cy) 
{
	CDragDialog::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	OnPaint();  //�����ػ��ӳ���
}

void CCurveView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_Drawing)
	{
		CClientDC dc(this);
		dc.SetROP2(R2_NOT);
		dc.SelectStockObject(NULL_BRUSH);
		SetCursor(m_Hcursor);  //���ù����״
		dc.Rectangle(m_rectselect);
		m_rectselect.right=point.x;
		m_rectselect.bottom=point.y;
		dc.Rectangle(m_rectselect);
	}
	else
	{
		float dep=Sdep+(point.y-MyRect.top)*(Edep-Sdep)/(MyRect.bottom-MyRect.top);
		if(point.x>MyRect.left && dep>=Stdep && dep<=Endep)
		{		
			int n1=(int)((dep-Stdep+0.5*Rlev)*(NumSam-1)/(Endep-Stdep));
			dep=Stdep+n1*Rlev;
			CString cs;
			cs.Format("%.3f %.3f",dep,CurveData[n1]);
			m_Value.SetWindowText (cs);
		}
	}
	CDragDialog::OnMouseMove(nFlags, point);
}

void CCurveView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_Hcursor=AfxGetApp()->LoadStandardCursor(IDC_CROSS);  //������Ϊʮ����
		
	m_rectselect.left=point.x;
	m_rectselect.top=point.y;
	m_rectselect.right=point.x;
	m_rectselect.bottom=point.y;
	SetCapture();
	RECT rect;
	GetClientRect(&rect);
	rect.left+=MyLeft;
	rect.bottom-=MyBottom;
	ClientToScreen(&rect);
	ClipCursor(&rect);
	m_Drawing=TRUE;
	CDragDialog::OnLButtonDown(nFlags, point);
}

void CCurveView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_Drawing)
	{
		ReleaseCapture();
		ClipCursor(NULL);
		m_rectselect.right=point.x;
		m_rectselect.bottom=point.y;

		m_rectselect.NormalizeRect();
		CClientDC dc(this);
		dc.SetROP2(R2_NOT);
		dc.SelectStockObject(NULL_BRUSH);
		dc.Rectangle(m_rectselect);

		int y1,y2;
		y1=m_rectselect.top;
		y2=m_rectselect.bottom;
		float dep1=Sdep+(y1-MyRect.top)*(Edep-Sdep)/(MyRect.bottom-MyRect.top);
		float dep2=Sdep+(y2-MyRect.top)*(Edep-Sdep)/(MyRect.bottom-MyRect.top);
		if(dep1<Stdep) dep1=Stdep;
		if(dep2>Endep) dep2=Endep;
		if(dep2-dep1>Rlev)
		{
			Sdep=dep1;
			Edep=dep2;
			OnPaint();	//�ػ�
		}
		m_Drawing=FALSE;
	}
	CDragDialog::OnLButtonUp(nFlags, point);
}

void CCurveView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDragDialog::OnLButtonDblClk(nFlags, point);
	if((Sdep!=Stdep)||(Edep!=Endep))
	{
		Sdep=Stdep;Edep=Endep;
		OnPaint();	
	}	
}

void CCurveView::OnStyle() 
{
	// TODO: Add your control notification handler code here
	CAttrDlg dlg;
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
		OnPaint();	//�ػ�
	}
}

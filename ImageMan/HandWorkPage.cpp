// HandWorkPage.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "HandWorkPage.h"
#include "FIDio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHandWorkPage property page

IMPLEMENT_DYNCREATE(CHandWorkPage, CPropertyPage)

CHandWorkPage::CHandWorkPage() : CPropertyPage(CHandWorkPage::IDD)
{
	//{{AFX_DATA_INIT(CHandWorkPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CHandWorkPage::~CHandWorkPage()
{
}

void CHandWorkPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHandWorkPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_FILENAME, m_FileName);
	DDX_GridControl(pDX, IDC_GRID, m_Grid);
	DDX_Check(pDX, IDC_CHECK1, m_Check1);
	DDX_Check(pDX, IDC_CHECK6, m_Check6);
	DDX_Check(pDX, IDC_CHECK7, m_Check7);
	DDX_Check(pDX, IDC_CHECK8, m_Check8);
	DDX_Check(pDX, IDC_CHECK9, m_Check9);
	DDX_Check(pDX, IDC_CHECK10, m_Check10);
	DDX_Check(pDX, IDC_CHECK11, m_Check11);
}


BEGIN_MESSAGE_MAP(CHandWorkPage, CPropertyPage)
	//{{AFX_MSG_MAP(CHandWorkPage)
	ON_BN_CLICKED(IDC_ARRANG, OnArrang)
	ON_BN_CLICKED(IDC_INSERTROW, OnInsertrow)
	ON_BN_CLICKED(IDC_DELROW, OnDelrow)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHandWorkPage message handlers
void CHandWorkPage::OnBrowse() 
{
	// TODO: Add your control notification handler code here
	CString Path;
	if(DetectFileExist(m_FileName))
	{
		Path=m_FileName;
		Path=Path.Left(Path.ReverseFind('\\'));
	}
	else
		Path=GetCurrentUserPath();
	static char BASED_CODE szFilter[] = 
	"交互解释结论(*.List)|*.List|所有文件(*.*)|*.*||";
	CFileDialog  Browse(TRUE, "交互解释结论(*.List)", "HandWork.List", 
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	Browse.m_ofn.lpstrTitle = "打开文件";
	Browse.m_ofn.lpstrInitialDir=Path;
	if (Browse.DoModal()==IDOK)
	{
		m_FileName =Browse.GetPathName();
		int xyz=m_FileName.Find (".List");
		if((xyz>0)&&DetectFileExist(m_FileName))
		{
			UpdateData(FALSE);
			m_Grid.DeleteAllItems();
			m_Grid.Load(m_FileName);
			m_Grid.SetFixedRowCount(1);
			m_Grid.AutoSize ();
			m_Grid.SetColumnWidth (2,140);
		}
	}
}

void CHandWorkPage::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertyPage::OnOK();
	m_PlotContent="";
	if(m_Check1) m_PlotContent+="层理";
	if(m_Check6) m_PlotContent+="充填缝";
	if(m_Check7) m_PlotContent+="张开缝";
	if(m_Check8) m_PlotContent+="压裂缝";
	if(m_Check9) m_PlotContent+="层间缝";
	if(m_Check10) m_PlotContent+="绘截面图";
	if(m_Check11) m_PlotContent+="半充填缝";
	if(!m_FileName.IsEmpty ())
		m_Grid.Save (m_FileName);
}

BOOL CHandWorkPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();	
	// TODO: Add extra initialization here
	m_Grid.SetEditable(TRUE);
	m_Grid.EnableDragAndDrop(TRUE);
	m_Grid.SetBkColor(0xFFFFFF);
	m_Grid.SetGridColor (RGB(0xC0, 0xC0, 0xC0));
	int xyz=m_FileName.Find (".List");
	if((xyz>0)&&DetectFileExist(m_FileName))
	{
		m_Grid.Load(m_FileName);
		m_Grid.SetFixedRowCount(1);
		m_Grid.SetFixedColumnCount (1);
		m_Grid.AutoSize ();
		m_Grid.SetColumnWidth (0,50);
		m_Grid.SetColumnWidth (1,60);
		m_Grid.SetColumnWidth (2,90);
		m_Grid.SetColumnWidth (3,90);
	}
	m_Check1=m_Check6=m_Check7=m_Check8=m_Check9=m_Check10=FALSE;
	if(m_PlotContent.Find("层理")>-1)
		m_Check1=TRUE;
	if(m_PlotContent.Find("充填缝")>-1)
		m_Check6=TRUE;
	if(m_PlotContent.Find("张开缝")>-1)
		m_Check7=TRUE;
	if(m_PlotContent.Find("压裂缝")>-1)
		m_Check8=TRUE;
	if(m_PlotContent.Find("层间缝")>-1)
		m_Check9=TRUE;
	if(m_PlotContent.Find("绘截面图")>-1)
		m_Check10=TRUE;
	if(m_PlotContent.Find("半充填缝")>-1)
		m_Check11=TRUE;
	UpdateData (FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHandWorkPage::OnArrang() 
{
	// TODO: Add your control notification handler code here
		//井段从小到大排序
	m_Grid.Invalidate ();
	CString m_DepArray[3000][3],str;
	double data,Value;int MinPos,i,j,k;
	int m_DepNum=m_Grid.GetRowCount ();
	for(i=1;i<m_DepNum;i++)
	{
		str=m_Grid.GetItemText (i,1);
		str.TrimLeft ();
		str.TrimRight ();
		m_DepArray[i-1][0]=str;
		str=m_Grid.GetItemText (i,2);
		str.TrimLeft ();
		str.TrimRight ();
		m_DepArray[i-1][1]=str;
		str=m_Grid.GetItemText (i,3);
		str.TrimLeft ();
		str.TrimRight ();
		m_DepArray[i-1][2]=str;
	}
	m_DepNum--;
	CString str1;
	for(i=0;i<m_DepNum;i++)
	{
		MinPos=i;
		data=atof(m_DepArray[i][0]);
		str=m_DepArray[i][1];
		str1=m_DepArray[i][2];
		for(j=i+1;j<m_DepNum;j++)
		{
			Value=atof(m_DepArray[j][0]);
		    if(Value<data)
			{
				MinPos=j;
				data=Value;
				str=m_DepArray[j][1];
				str1=m_DepArray[j][2];
			}
		}
		if(MinPos>i);
		{
			for(k=MinPos;k>i;k--)
			{
				m_DepArray[k][0]=m_DepArray[k-1][0];
				m_DepArray[k][1]=m_DepArray[k-1][1];
				m_DepArray[k][2]=m_DepArray[k-1][2];
			}
			m_DepArray[i][0].Format ("%.3f",data);
			m_DepArray[i][1]=str;
			m_DepArray[i][2]=str1;
		}
	}
	m_Grid.SetRowCount (m_DepNum+1);
	for(i=0;i<=m_DepNum;i++)
	{
		str.Format ("%d",i+1);
		m_Grid.SetItemText (i+1,0,str);
		m_Grid.SetItemText (i+1,1,m_DepArray[i][0]);
		m_Grid.SetItemText (i+1,2,m_DepArray[i][1]);
		m_Grid.SetItemText (i+1,3,m_DepArray[i][2]);
	}
	m_Grid.Invalidate ();
}

void CHandWorkPage::OnInsertrow() 
{
	// TODO: Add your control notification handler code here
	int nRow = m_Grid.GetFocusCell().row;
	m_Grid.InsertRow(_T(" "),nRow);
	m_Grid.Invalidate();
}

void CHandWorkPage::OnDelrow() 
{
	// TODO: Add your control notification handler code here
	int nRow=m_Grid.GetFocusCell().row;
	if(nRow>0)
	{
		m_Grid.DeleteRow(nRow);	
	    m_Grid.Invalidate();
	}
}
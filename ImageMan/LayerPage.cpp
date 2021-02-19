// LayerPage.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "LayerPage.h"
#include "FIDio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLayerPage property page

IMPLEMENT_DYNCREATE(CLayerPage, CPropertyPage)

CLayerPage::CLayerPage() : CPropertyPage(CLayerPage::IDD)
{
	//{{AFX_DATA_INIT(CLayerPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CLayerPage::~CLayerPage()
{
}

void CLayerPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLayerPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO1, m_Box);
	DDX_Text(pDX, IDC_FILENAME, m_FileName);
	DDX_GridControl(pDX, IDC_GRID, m_Grid);
	DDX_Control(pDX, IDC_COMBO2, m_Option);
}


BEGIN_MESSAGE_MAP(CLayerPage, CPropertyPage)
	//{{AFX_MSG_MAP(CLayerPage)
	ON_BN_CLICKED(IDC_ARRANG, OnArrang)
	ON_BN_CLICKED(IDC_INSERTROW, OnInsertrow)
	ON_BN_CLICKED(IDC_DELROW, OnDelrow)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_CBN_KILLFOCUS(IDC_COMBO1, OnKillfocusCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnSelchangeCombo2)
	ON_CBN_KILLFOCUS(IDC_COMBO2, OnKillfocusCombo2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayerPage message handlers
void CLayerPage::OnBrowse() 
{
	// TODO: Add your control notification handler code here
	CString newName;
	static char BASED_CODE szFilter[] = 
	"层位标注文件(*.List)|*.List|所有文件(*.*)|*.*||";
	CFileDialog  Browse(TRUE, "层位标注文件(*.List)", "*.List", 
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	Browse.m_ofn.lpstrTitle = "打开文件";

	Browse.m_ofn.lpstrInitialDir=GetCurrentUserPath();

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

void CLayerPage::OnSelchangeCombo1() 
{
	// TODO: Add your control notification handler code here
	m_Box.ShowWindow (SW_HIDE);
	::SetParent(m_Box.GetSafeHwnd(), this->GetSafeHwnd());	
	if (m_Box.GetCurSel( ) == -1) return;
	CString szEdit;
	m_Box.GetLBText(m_Box.GetCurSel( ), szEdit);
	int nCol=m_Grid.GetFocusCell().col;
	int nRow=m_Grid.GetFocusCell ().row ;
	if(nCol>0)
	{
		if(m_Grid.GetItemText (0,nCol)=="标注")
		{		
			m_Grid.SetItemText (nRow,nCol,szEdit);	
			m_Grid.Invalidate();				
		}
	}
}

void CLayerPage::OnKillfocusCombo1() 
{
	// TODO: Add your control notification handler code here
	CString szEdit;
	m_Box.GetLBText(m_Box.GetCurSel( ), szEdit);
	int nCol=m_Grid.GetFocusCell().col;
	int nRow=m_Grid.GetFocusCell ().row ;
	if(nCol>0)
	{
		if(m_Grid.GetItemText (0,nCol)=="标注")
		{		
			szEdit.TrimLeft ();
			szEdit.TrimRight ();
			m_Grid.SetItemText (nRow,nCol,szEdit);	
			m_Grid.Invalidate();
	/*		COMBOBOXEXITEM item;
			item.mask = CBEIF_TEXT;
			item.iItem = 0;
			item.pszText = (LPSTR) (LPCTSTR) szEdit;
			m_Box.InsertItem (&item);*/
		}
	}
	m_Box.ShowWindow (SW_HIDE);
	::SetParent(m_Box.GetSafeHwnd(), this->GetSafeHwnd());	
}

void CLayerPage::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertyPage::OnOK();
	OnArrang();
	if(!m_FileName.IsEmpty ())
		m_Grid.Save (m_FileName);
	m_Box.SaveHistory ();	
}

BOOL CLayerPage::OnInitDialog() 
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
	LoadLayer();
	UpdateData (FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLayerPage::OnArrang() 
{
	// TODO: Add your control notification handler code here
		//井段从小到大排序
	m_Grid.Invalidate ();
	CString m_DepArray[100][3],str;
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

void CLayerPage::OnInsertrow() 
{
	// TODO: Add your control notification handler code here
	int nRow = m_Grid.GetFocusCell().row;
	m_Grid.InsertRow(_T(" "),nRow);
	m_Grid.Invalidate();
}

void CLayerPage::OnDelrow() 
{
	// TODO: Add your control notification handler code here
	int nRow=m_Grid.GetFocusCell().row;
	if(nRow>0)
	{
		m_Grid.DeleteRow(nRow);	
	    m_Grid.Invalidate();
	}
}

void CLayerPage::OnSelchangeCombo2() 
{
	// TODO: Add your control notification handler code here
	m_Option.ShowWindow (SW_HIDE);
	::SetParent(m_Option.GetSafeHwnd(), this->GetSafeHwnd());	
	if (m_Option.GetCurSel( ) == -1) return;
	CString szEdit;
	m_Option.GetLBText(m_Option.GetCurSel( ), szEdit);
	int nCol=m_Grid.GetFocusCell().col;
	int nRow=m_Grid.GetFocusCell ().row ;
	if(nCol>0)
	{
		if(m_Grid.GetItemText (0,nCol)=="选项")
		{		
			m_Grid.SetItemText (nRow,nCol,szEdit);	
			m_Grid.Invalidate();				
		}
	}
}

void CLayerPage::OnKillfocusCombo2() 
{
	// TODO: Add your control notification handler code here
	CString szEdit;
	m_Option.GetLBText(m_Option.GetCurSel( ), szEdit);
	int nCol=m_Grid.GetFocusCell().col;
	int nRow=m_Grid.GetFocusCell ().row ;
	if(nCol>0)
	{
		if(m_Grid.GetItemText (0,nCol)=="选项")
		{		
			szEdit.TrimLeft ();
			szEdit.TrimRight ();
			m_Grid.SetItemText (nRow,nCol,szEdit);	
			m_Grid.Invalidate();
		}
	}
	m_Option.ShowWindow (SW_HIDE);
	::SetParent(m_Option.GetSafeHwnd(), this->GetSafeHwnd());	
}

void CLayerPage::LoadLayer()
{
	CString FileName=GetWorkPath("Config")+"Layer.ini";
	FILE *fp;
	fp=fopen(FileName,"rt");
	if(fp!=NULL)
	{
		char ss[256];
		m_Box.ResetContent ();
		while(!feof(fp))
		{
			fscanf(fp,"%s",ss);
			m_Box.InsertItem (ss);
		}
		fclose(fp);
	}
	else
	{
		m_Box.LoadHistory("Addresses", "LayerComboEx");
		m_Box.SetMaxHistoryItems(30);
	}	
}

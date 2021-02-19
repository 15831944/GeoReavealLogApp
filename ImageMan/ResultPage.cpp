// ResultPage.cpp : implementation file
//
#include "stdafx.h"
#include "ImageMan.h"
#include "FIDDraw.h"
#include "ResultPage.h"
#include "ImageManDoc.h"
#include "FIDIO.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResultPage property page

IMPLEMENT_DYNAMIC(CResultPage, CPropertyPage)

CResultPage::CResultPage() : CPropertyPage(CResultPage::IDD)
{
	//{{AFX_DATA_INIT(CResultPage)
	m_ResultLR = -1;
	m_FileName = _T("");
	
	//}}AFX_DATA_INIT
	m_GroupCaption="位置";
	m_LeftCaption="左边";
	m_RightCaption="右边";
}

void CResultPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResultPage)
	DDX_Text(pDX, IDC_WELLNAME, m_FileName);
	DDX_GridControl(pDX, IDC_RESULTGRID, m_Grid);	
	//}}AFX_DATA_MA
	DDX_Control(pDX, IDC_COMBO1, m_Box);
}


BEGIN_MESSAGE_MAP(CResultPage, CPropertyPage)
	//{{AFX_MSG_MAP(CResultPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BROWSE_WELL, OnBrowseWell)
	ON_BN_CLICKED(IDC_DEL_ROW, OnDelRow)
	ON_BN_CLICKED(IDC_INSERT_ROW, OnInsertRow)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_CBN_KILLFOCUS(IDC_COMBO1, OnKillfocusCombo1)
	ON_BN_CLICKED(IDC_COMPOSITOR, OnCompositor)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResultPage message handlers

BOOL CResultPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	m_Grid.SetEditable(TRUE);
	m_Grid.EnableDragAndDrop(TRUE);
	m_Grid.SetBkColor(0xFFFFFF);
	m_Grid.SetGridColor (RGB(0xC0, 0xC0, 0xC0));
	int xyz=m_FileName.Find (".List");
	if((xyz>0)&&DetectFileExist(m_FileName))
	{
		m_Grid.Load(m_FileName);
		m_Grid.SetFixedRowCount(1);
	}
	xyz=m_FileName.Find (".ccd");
	if((xyz>0)&&DetectFileExist(m_FileName))
	{
		m_Grid.Load(m_FileName);
		m_Grid.SetFixedRowCount(1);
	}
	return TRUE; 
}

void CResultPage::OnOK() 
{
	if(m_FileName.Find (".ccd")==-1)
		m_Grid.Save (m_FileName);
	CPropertyPage::OnOK();
}

void CResultPage::OnBrowseWell() 
{
	CString newName;
	static char BASED_CODE szFilter[] = 
	"解释表文件(*.List)|*.List|Tab表文件(*.ccd)|*.ccd|所有文件(*.*)|*.*||";
	CFileDialog  Browse(TRUE, "解释表文件(*.List)", "*.List", 
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
		}
		xyz=m_FileName.Find (".ccd");
		if((xyz>0)&&DetectFileExist(m_FileName))
		{
			UpdateData(FALSE);
			m_Grid.DeleteAllItems();
			m_Grid.Load(m_FileName);
		}
	}
}

void CResultPage::OnDelRow()
{
	int nRow=m_Grid.GetFocusCell().row;
	if(nRow>0)
	{
		m_Grid.DeleteRow(nRow);	
	    m_Grid.Invalidate();
	}
}

void CResultPage::OnInsertRow()
{
	int nRow = m_Grid.GetFocusCell().row;
	m_Grid.InsertRow(_T("NewRow"),nRow);
	m_Grid.Invalidate();
}

void CResultPage::OnSelchangeCombo1() 
{
	// TODO: Add your control notification handler code here
	m_Box.ShowWindow (SW_HIDE);
	::SetParent(m_Box.GetSafeHwnd(), this->GetSafeHwnd());
	if (m_Box.GetCurSel( ) == -1) return;
	CString szEdit;
	m_Box.GetLBText(m_Box.GetCurSel( ), szEdit);
	int nCol=m_Grid.GetFocusCell().col;
	int nRow=m_Grid.GetFocusCell ().row ;
	if((nCol>0)&&(nRow>1))
	{
		if(m_Grid.GetItemText (0,nCol)=="解释结论")
		{		
			m_Grid.SetItemText (nRow,nCol,szEdit);	
			m_Grid.Invalidate();				
		}
	}
}

void CResultPage::OnKillfocusCombo1() 
{
	// TODO: Add your control notification handler code here
	m_Box.ShowWindow (SW_HIDE);
	::SetParent(m_Box.GetSafeHwnd(), this->GetSafeHwnd());
}

void CResultPage::OnCompositor() 
{
	//井段从小到大排序
	m_Grid.Invalidate ();
	CString str;
	double DepArray[300][3];
	int i,j,k,Col,MinPos,NoPos=-1,NoMin=1000,DepPos=-1;
	//查找序号位置 NoPos
	Col=m_Grid.GetColumnCount ();
	for(i=0;i<Col;i++)
	{
		str=m_Grid.GetItemText (0,i);
		str.TrimLeft ();
		str.TrimRight ();
		if(str=="序号")
		{
			NoPos=i;
			break;
		}
	}
	//查找深度位置 DepPos
	int m_DepNum=m_Grid.GetRowCount ();
	if(m_DepNum<4)
		return;
	for(i=0;i<Col;i++)
	{
		str=m_Grid.GetItemText (3,i);
		if(str.Find("--")>0)
		{
			DepPos=i;
			break;
		}
	}
	if(DepPos<0) return;
	//查找最小序号 NoMin
	for(i=2;i<m_DepNum;i++)
	{
		str=m_Grid.GetItemText (i,NoPos);
		str.TrimLeft ();
		str.TrimRight ();
		k=atoi(str);
		if(k<NoMin) NoMin=k;
	}
	if(NoMin<1) NoMin=1;

	for(i=2;i<m_DepNum;i++)
	{
		str=m_Grid.GetItemText (i,DepPos);
		str.TrimRight ();
		DepArray[i][0]=atof(str.Left(str.Find ("--")+1));
		DepArray[i][1]=(double)i;
	}
	double data,num;
	//按起始深度从下到大排序
	//DepArray[i][0] 各层起始深度
	//DepArray[i][1] 原始行号
	for(i=2;i<m_DepNum;i++)
	{
		MinPos=i;
		data=DepArray[i][0];
		num=DepArray[i][1];
		for(j=i+1;j<m_DepNum;j++)
		    if(DepArray[j][0]<data)
			{
				MinPos=j;
				data=DepArray[j][0];
				num=DepArray[j][1];
			}
		if(MinPos>i);
		{
			for(k=MinPos;k>i;k--)
			{
				DepArray[k][0]=DepArray[k-1][0];
				DepArray[k][1]=DepArray[k-1][1];
			}
			DepArray[i][0]=data;
			DepArray[i][1]=num;
		}
	}
	//对表格重新赋值
	k=m_DepNum;
	for(i=2;i<m_DepNum;i++)
	{	
		m_Grid.InsertRow("  ");
		for(j=0;j<Col;j++)
		{
			if(j==NoPos)
				str.Format ("%d",NoMin);
			else
				str=m_Grid.GetItemText ((int)(DepArray[i][1]),j);
			m_Grid.SetItemText(k,j,str);
		}
		k++;NoMin++;
	}
	//在表格中删除原有层段
	for(i=2;i<m_DepNum;i++)
		m_Grid.DeleteRow (2);
	m_Grid.Invalidate ();
}
// ReCurNamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ReCurNamDlg.h"
#include "ModiCurAttDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReCurNamDlg dialog


CReCurNamDlg::CReCurNamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReCurNamDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReCurNamDlg)
	m_nMaxLen = 0;
	m_nMaxLog = 0;
	m_sOldStr = _T("");
	m_sNewStr = _T("");
	m_NumLog = 0;
	//}}AFX_DATA_INIT
}


void CReCurNamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReCurNamDlg)
	DDX_Control(pDX, IDC_CURVE_NAME_LIST, m_CurveName);
	DDX_Text(pDX, IDC_MAXLEN_EDIT, m_nMaxLen);
	DDX_Text(pDX, IDC_MAXLOG_EDIT, m_nMaxLog);
	DDX_Text(pDX, IDC_OLDSTR_EDIT, m_sOldStr);
	DDX_Text(pDX, IDC_NEWSTR_EDIT, m_sNewStr);
	DDX_Text(pDX, IDC_NUMLOG_EDIT, m_NumLog);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReCurNamDlg, CDialog)
	//{{AFX_MSG_MAP(CReCurNamDlg)
	ON_NOTIFY(NM_RCLICK, IDC_CURVE_NAME_LIST, OnRclickCurveNameList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_CURVE_NAME_LIST, OnEndlabeleditCurveNameList)
	ON_BN_CLICKED(IDC_DELETE_BTN, OnDeleteBtn)
	ON_NOTIFY(NM_DBLCLK, IDC_CURVE_NAME_LIST, OnDblclkCurveNameList)
	ON_BN_CLICKED(IDC_MODIFYSTR_BTN, OnModifystrBtn)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_GETLEFT_BTN, OnGetleftBtn)
	ON_BN_CLICKED(IDC_GETRIGHT_BTN, OnGetrightBtn)
	ON_BN_CLICKED(IDC_CLEARERR_BTN, OnClearerBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReCurNamDlg message handlers

BOOL CReCurNamDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(m_SourceName.GetSize()==0)
	{
		OnCancel(); //没有曲线，直接关闭窗口
	}
	//设置CListctrl为报表形式
	DWORD dwStyle=GetWindowLong(m_CurveName.m_hWnd,GWL_STYLE);
	SetWindowLong(m_CurveName.m_hWnd,GWL_STYLE,dwStyle|LVS_REPORT);
	//设置CListctrl可以整行选择和网格条纹
	DWORD styles=m_CurveName.GetExtendedStyle();
	m_CurveName.SetExtendedStyle(styles|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_CurveName.InsertColumn(0,"新曲线名",LVCFMT_LEFT,80,0);
	m_CurveName.InsertColumn(1,"单位",LVCFMT_LEFT,60,0);
	m_CurveName.InsertColumn(2,"原曲线名",LVCFMT_LEFT,90,0);
	m_CurveName.InsertColumn(3,"维",LVCFMT_LEFT,30,0);
	m_CurveName.InsertColumn(4,"状态",LVCFMT_LEFT,45,0);
	CString s;
	for(int i=0;i<m_SourceName.GetSize();i++)
	{
		m_CurveName.InsertItem(m_CurveName.GetItemCount(),m_ObjectName.GetAt(i));
		m_CurveName.SetItemText(m_CurveName.GetItemCount()-1,1,m_Units.GetAt(i));
		m_CurveName.SetItemText(m_CurveName.GetItemCount()-1,2,m_SourceName.GetAt(i));
		if(i<m_NumDim.GetSize())
			s.Format ("%u",m_NumDim.GetAt(i)+1);
		else
			s="Unknow";
		m_CurveName.SetItemText(m_CurveName.GetItemCount()-1,3,s);
	}
	bSucceed=false;
	//对曲线名进行检查
	OnCheckBtn();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CReCurNamDlg::OnCheckBtn() 
{
	CString sError;
	UpdateData(false);
	bSucceed=true;
	m_NumLog=m_CurveName.GetItemCount();
	for(int i=0;i<m_NumLog;i++)
	{
		sError.Empty();
		CString sCurve1=m_CurveName.GetItemText(i,0);
		if(sCurve1.GetLength()>m_nMaxLen)
		{
			sError+="L";
			bSucceed=false;
		}
		if(!iswalpha(sCurve1.GetAt(0)))
		{
			sError+="F";
			bSucceed=false;
		}
		for(int j=0;j<sCurve1.GetLength();j++)
		{
			if(!iswalnum(sCurve1.GetAt(j)) && sCurve1.GetAt(j)!='_')
			{
				sError+="B";
				bSucceed=false;
				break;
			}
		}
		for(j=0;j<i;j++)
		{
			CString sCurve2=m_CurveName.GetItemText(j,0);
			if(sCurve1.CompareNoCase(sCurve2)==0)
			{
				sError+="E";
				bSucceed=false;
				break;
			}
		}
		m_CurveName.SetItemText(i,4,sError);
	}
	UpdateData(false);
}

void CReCurNamDlg::OnDblclkCurveNameList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	POSITION pos=m_CurveName.GetFirstSelectedItemPosition(); //位置
	if(pos!=NULL)
	{
		int nItem=m_CurveName.GetNextSelectedItem(pos) ;  //从位置中取得项索引
		EditCurveAttrib(nItem);
	}	
	*pResult = 0;
}
void CReCurNamDlg::EditCurveAttrib(int nItem)
{
	CString s;
	CModiCurAttDlg ModiCurAttDlg;
	ModiCurAttDlg.m_NewCurve=m_CurveName.GetItemText(nItem,0);
	ModiCurAttDlg.m_OldUnits=m_CurveName.GetItemText(nItem,1);
	ModiCurAttDlg.m_NewUnits=ModiCurAttDlg.m_OldUnits;
	ModiCurAttDlg.m_OldCurve=m_CurveName.GetItemText(nItem,2);
	if(ModiCurAttDlg.DoModal()==IDOK)
	{
		m_CurveName.SetItemText(nItem,0,(ModiCurAttDlg.m_NewCurve).Left(m_nMaxLen));
		CString Units=ModiCurAttDlg.m_NewUnits;
		int n=Units.Find(" -");
		if(n>0)
		{
			CString s=Units.Left(n);
			Units=s;
		}
		Units.TrimLeft();
		Units.TrimRight();
		m_CurveName.SetItemText(nItem,1,Units);
		OnCheckBtn();
	}
}

void CReCurNamDlg::OnRclickCurveNameList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	POSITION pos=m_CurveName.GetFirstSelectedItemPosition(); //位置
	if(pos!=NULL)
	{
		int nItem=m_CurveName.GetNextSelectedItem(pos) ;  //从位置中取得项索引
		m_CurveName.EditLabel(nItem);  //使该项可编辑
	}
	////////////////
	*pResult = 0;
}

void CReCurNamDlg::OnEndlabeleditCurveNameList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	CString sNewName=pDispInfo->item.pszText; 
	sNewName.Remove(' ');
	if(sNewName.IsEmpty())
	{
		return; //没有改名
	}
	POSITION pos=m_CurveName.GetFirstSelectedItemPosition();
	if(pos)
	{
		int nItem=m_CurveName.GetNextSelectedItem(pos) ;
		sNewName.MakeUpper(); //变成大写
		sNewName.TrimLeft();
		sNewName.TrimRight();
		m_CurveName.SetItemText(nItem,0,sNewName.Left(m_nMaxLen));
	}
	OnCheckBtn();
	////////////////////
	*pResult = 0;
}

void CReCurNamDlg::OnDeleteBtn() 
{
	CString s;
	int nItemSelected=m_CurveName.GetSelectedCount();//所选表项数 
	int nItemCount=m_CurveName.GetItemCount();//表项总数 
	if(nItemSelected<1) return; 

	for(int i=nItemCount-1;i>=0;i--)
	{ 
		int iState=m_CurveName.GetItemState(i,LVIS_SELECTED); 
		if(iState!=0) m_CurveName.DeleteItem(i); 
	}
	OnCheckBtn();
}
void CReCurNamDlg::OnOK() 
{
	// TODO: Add extra validation here
	OnCheckBtn();
	if(!bSucceed) 
	{
		AfxMessageBox("曲线名异常或曲线数太多!");
		return;
	}
	//取曲线名
	m_SourceName.RemoveAll();
	m_ObjectName.RemoveAll();
	m_Units.RemoveAll();
	for(int i=0;i<m_CurveName.GetItemCount();i++)
	{
		m_ObjectName.Add(m_CurveName.GetItemText(i,0));
		m_Units.Add(m_CurveName.GetItemText(i,1));
		m_SourceName.Add(m_CurveName.GetItemText(i,2));
	}
	CDialog::OnOK();
}

void CReCurNamDlg::OnModifystrBtn() 
{
	UpdateData();
	m_sNewStr.TrimRight();
	if(m_sOldStr.IsEmpty()) return;
	m_sNewStr.MakeUpper();
	m_sOldStr.MakeUpper();

	for(int i=0;i<m_CurveName.GetItemCount();i++)
	{
		CString sNewName=m_CurveName.GetItemText(i,0);
		sNewName.Replace(m_sOldStr,m_sNewStr);
		m_CurveName.SetItemText(i,0,sNewName);
	}
	OnCheckBtn();
}

HBRUSH CReCurNamDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	int nCtrlID=pWnd->GetDlgCtrlID();
	switch(nCtrlID)
	{
	case IDC_NUMLOG_EDIT:
		if(m_NumLog>m_nMaxLog)
		{
			pDC->SetTextColor( RGB( 0, 0, 128)) ;
			pDC->SetBkColor(RGB(255,0,0));
		}
		else
		{
			pDC->SetTextColor( RGB( 0, 0, 128 )) ; 
			pDC->SetBkColor( RGB( 192, 192, 192)) ;
		}
		break;
	case IDC_MAXLOG_EDIT:
		pDC->SetTextColor( RGB( 0, 0, 128)) ; 
		break;
	default:
		break;
	}
	return hbr;
}

void CReCurNamDlg::OnGetleftBtn() 
{
	UpdateData();
	for(int i=0;i<m_CurveName.GetItemCount();i++)
	{
		CString CurveName=m_CurveName.GetItemText(i,0);
		if(CurveName.GetLength()>m_nMaxLen)
		{
			m_CurveName.SetItemText(i,0,CurveName.Left(m_nMaxLen));
		}
	}
	OnCheckBtn();
}

void CReCurNamDlg::OnGetrightBtn() 
{
	UpdateData();
	for(int i=0;i<m_CurveName.GetItemCount();i++)
	{
		CString CurveName=m_CurveName.GetItemText(i,0);
		if(CurveName.GetLength()>m_nMaxLen)
		{
			CString NewCurve=CurveName.Right(m_nMaxLen);
			if(!iswalpha(NewCurve.GetAt(0)))
			{
				NewCurve=CurveName=CurveName.Left(1)+CurveName.Right(m_nMaxLen-1);
			}
			m_CurveName.SetItemText(i,0,NewCurve);
		}
	}
	OnCheckBtn();
}

void CReCurNamDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	m_SourceName.RemoveAll();
	m_ObjectName.RemoveAll();
	m_Units.RemoveAll();
	CDialog::OnCancel();
}

void CReCurNamDlg::OnClearerBtn() 
{
	UpdateData(true);
	m_NumLog=m_CurveName.GetItemCount();
	for(int i=m_NumLog-1;i>=0;i--)
	{
		CString sError=m_CurveName.GetItemText(i,4);
		if(!sError.IsEmpty())
		{
			m_CurveName.DeleteItem(i);
		}
	}
	UpdateData(false);
	OnCheckBtn();
}

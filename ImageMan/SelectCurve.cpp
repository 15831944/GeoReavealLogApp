// SelectCurve.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "SelectCurve.h"
#include "FIDio.h"
#include <io.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectCurve dialog


CSelectCurve::CSelectCurve(CString OpenPath,CWnd* pParent /*=NULL*/)
	: CDialog(CSelectCurve::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectCurve)
	m_bAlikePro = FALSE;
	//}}AFX_DATA_INIT
	m_OpenPath=OpenPath;
}


void CSelectCurve::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectCurve)
	DDX_Control(pDX, IDC_LISTBOX, m_ProCurveList);
	DDX_Check(pDX, IDC_CHECK1, m_bAlikePro);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectCurve, CDialog)
	//{{AFX_MSG_MAP(CSelectCurve)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectCurve message handlers

BOOL CSelectCurve::OnInitDialog() 
{
	int i;
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	char path[512];
	strcpy(path,m_OpenPath);
	CString strPath=m_OpenPath;
	int yyy=strPath.Find ("#");
	if(yyy<0)
	{
		MessageBox("打开井数据失败  ","错误提示",MB_OK);
		return TRUE;
	}
	else
	{
		CFIDIO::GetPath(path,strPath.GetBuffer(512));
		strPath.ReleaseBuffer();
		strPath=strPath.Left (strPath.ReverseFind ('\\'));
		strPath=strPath+"\\"+strPath.Mid (yyy+1);
	}
	OpenWell(strPath);
	int Num=m_PreCurveNameArray.GetSize ();
	for(i=0;i<Num;i++)
	{
		CFIDIO* pFIDIO = new CFIDIO(strPath+"."+m_PreCurveNameArray.GetAt (i)); 		
		if (pFIDIO->Open())
			m_ProCurveList.AddString(m_PreCurveNameArray.GetAt (i));
		delete pFIDIO;	
	}
	for (i=0; i<m_SelCurveNameArray.GetSize(); i++)
	{
		int k = m_ProCurveList.FindString(-1,m_SelCurveNameArray[i]);
		if (k != LB_ERR) m_ProCurveList.SetCheck(k,1);
		else m_ProCurveList.AddString(m_SelCurveNameArray[i]);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectCurve::OnOK() 
{
	// TODO: Add extra validation here	
	UpdateData(TRUE);
	int num = m_ProCurveList.GetCount();
	CString s;
	m_SelCurveNameArray.RemoveAll();
	for (int i=0; i<num; i++)
	{
		if (m_ProCurveList.GetCheck(i)) 
		{
			m_ProCurveList.GetText(i,s);
			m_SelCurveNameArray.Add(s);
		}
	}
	CDialog::OnOK();
}

void CSelectCurve::OpenWell(CString StrPath)
{
	CString WellName;
	CFIDIO::GetFullWellName(StrPath,WellName.GetBuffer(512));
	WellName.ReleaseBuffer();
	
	char szFind[512];
	CFIDIO::GetFileName(WellName,szFind,'I');
	strcat(szFind,".*");

	CString szDir;
	CFIDIO::GetPath(WellName,szDir.GetBuffer(512));
	szDir.ReleaseBuffer();

	char szFIDName[40];
	struct _finddata_t c_file;    
	long hFile;
	m_PreCurveNameArray.RemoveAll();
	if ((hFile = _findfirst(szFind, &c_file )) != -1L)
	{ 
		CFIDIO::GetFIDName(c_file.name,szFIDName);
		m_PreCurveNameArray.Add(szFIDName);
		while(_findnext(hFile,&c_file) == 0 ) 
		{
			CFIDIO::GetFIDName(c_file.name,szFIDName);
			m_PreCurveNameArray.Add(szFIDName);
		}	
 		_findclose( hFile );
	}
}

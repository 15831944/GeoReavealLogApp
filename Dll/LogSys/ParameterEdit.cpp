// ParameterEdit.cpp : implementation file
//

#include "stdafx.h"
#include "LogSysres.h"
#include "ParameterEdit.h"
#include "Editdlg.h"
#include "io.h"
#include "FIDio.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL FileExist(const char* s);
void dtos(double value, char* ss)
{
//	int i,j,k;
	sprintf(ss,"%g",value);
/*	k = strlen(ss);
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
	ss[j] = 0;*/
}
/////////////////////////////////////////////////////////////////////////////
// CTabParaObj
IMPLEMENT_SERIAL(CTabParaObj, CObject, 0)
CTabParaObj::CTabParaObj()
{
}
void CTabParaObj::Serialize( CArchive& ar )
{
	if( ar.IsStoring() )
	{
		ar << szTabName;
	}
	else
	{
		ar >> szTabName;
	}
	szParaNameArry.Serialize( ar );
	szParaDspArry.Serialize( ar );
}

/////////////////////////////////////////////////////////////////////////////
// CParameterEdit dialog


CParameterEdit::CParameterEdit(CWnd* pParent, 
							 int NNI, int NNO, int NNC,
	INCURVE* m_InCurve, OUTCURVE* m_OutCurve, INPNAME* m_InpName, float* m_InpValue)
	: CDialog(CParameterEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CParameterEdit)
	//}}AFX_DATA_INIT
	NoI =NNI;    //输入曲线条数
	NoO =NNO;    //输出曲线条数
	NoC =NNC;	 //参数个数
	pInCurve = new INCURVE[NoI]; 
    pOutCurve = new OUTCURVE[NoO]; 
	pNameC = new INPNAME[NoC]; 
	pValueC = new float[NoC]; 
	memcpy(pInCurve,m_InCurve,sizeof(INCURVE)*NoI);
	memcpy(pOutCurve,m_OutCurve,sizeof(OUTCURVE)*NoO);
	memcpy(pNameC,m_InpName,sizeof(INPNAME)*NoC);
	memcpy(pValueC,m_InpValue,sizeof(float)*NoC);
	bTplModified = FALSE;
	bParaModified = FALSE;
	m_MethodName = "Well logging Analysis";
	pParaValue = NULL;
}
CParameterEdit::~CParameterEdit()
{
	delete pInCurve;
	delete pOutCurve;
	delete [] pNameC;
	delete pValueC;
	CTabParaObj* pTabPara;
	POSITION pos = m_TabList.GetHeadPosition();
	while (pos != NULL) 
	{
		pTabPara = (CTabParaObj*)m_TabList.GetNext(pos);
		if (pTabPara != NULL) delete  pTabPara;
	}
	CStringArray* pParaValue1;
	pos = m_ValueList.GetHeadPosition();
	while (pos != NULL) 
	{
		pParaValue1 = (CStringArray*)m_ValueList.GetNext(pos);
		if (pParaValue1 != NULL) delete  pParaValue1;
	}
}
void CParameterEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParameterEdit)
	DDX_Control(pDX, IDC_ZOOMOUT, m_ZoomOut);
	DDX_Control(pDX, IDC_ZOOMIN, m_ZoomIn);
	DDX_Control(pDX, IDC_DEPSLIDER, m_DepSlider);
	DDX_Control(pDX, IDC_COMBO, m_Combo);
	DDX_Control(pDX, IDC_EDIT, m_Edit);
	DDX_Control(pDX, IDC_RULER, m_Ruler);
	DDX_Control(pDX, IDC_OUTCTRL, m_OutCtrl);
	DDX_Control(pDX, IDC_INCTRL, m_InCtrl);
	DDX_Control(pDX, IDC_DEFAULTCONCTRL, m_DefConCtrl);
	DDX_Control(pDX, IDC_CONTAB, m_ConTab);
	DDX_Control(pDX, IDC_CONCTRL, m_ConCtrl);
	DDX_Text(pDX, IDC_NOC, NoC);
	DDV_MinMaxInt(pDX, NoC, 0, 9999);
	DDX_Text(pDX, IDC_NOI, NoI);
	DDV_MinMaxInt(pDX, NoI, 0, 9999);
	DDX_Text(pDX, IDC_NOO, NoO);
	DDV_MinMaxInt(pDX, NoO, 0, 9999);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParameterEdit, CDialog)
	//{{AFX_MSG_MAP(CParameterEdit)
	ON_BN_CLICKED(ID_EDIT, OnEdit)
	ON_EN_CHANGE(IDC_EDIT, OnChangeEdit)
	ON_BN_CLICKED(IDC_ADDTAB, OnAddtab)
	ON_BN_CLICKED(IDC_CHTAB, OnChtab)
	ON_BN_CLICKED(IDC_DELTAB, OnDeltab)
	ON_BN_CLICKED(IDC_SAVETAB, OnSavetab)
	ON_NOTIFY(TCN_SELCHANGE, IDC_CONTAB, OnSelchangeContab)
	ON_BN_CLICKED(IDC_SAVEPARA, OnSavepara)
	ON_BN_CLICKED(IDC_TEXTHELP, OnHelp)
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO, OnSelchangeCombo)
	ON_BN_CLICKED(IDC_SAVEDEFAULTPARA, OnSavedefaultpara)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_ZOOMIN, OnDepZoomin)
	ON_BN_CLICKED(IDC_ZOOMOUT, OnDepZoomout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParameterEdit message handlers

BOOL CParameterEdit::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS);
	return CDialog::PreCreateWindow(cs);
}
HCURSOR CParameterEdit::OnQueryDragIcon() 
{
	return (HCURSOR) m_hIcon;	
}

BOOL CParameterEdit::PreTranslateMessage(MSG* pMsg)
{
	{
		if (m_ToolTip.m_hWnd != NULL )m_ToolTip.RelayEvent(pMsg);
		return CDialog::PreTranslateMessage(pMsg);
	}
}
BOOL CParameterEdit::OnInitDialog() 
{	
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	CDialog::OnInitDialog();
	m_Menu.LoadMenu(IDR_PARAMETERMEDITMENU);
	SetMenu(&m_Menu);
	m_ZoomIn.EnableWindow(FALSE);
	m_DepSlider.ShowWindow(SW_HIDE);
	SetWindowText(m_InpFileName.Mid(m_InpFileName.ReverseFind ('\\')+1));
	//初始化列表
	m_Combo.ShowWindow(SW_HIDE);
	m_Edit.ShowWindow(SW_HIDE);
	InitCombo();
	m_DefConCtrl.InitCtrl(NoC, pNameC, pValueC);
	ListView_SetExtendedListViewStyle(m_DefConCtrl, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ConCtrl.InitCtrl(&m_Edit);
	ListView_SetExtendedListViewStyle(m_ConCtrl, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_InCtrl.InitCtrl(NoI, pInCurve, &m_Combo);
	ListView_SetExtendedListViewStyle(m_InCtrl, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	
	m_OutCtrl.InitCtrl(NoO, pOutCurve, &m_Edit);
	ListView_SetExtendedListViewStyle(m_OutCtrl, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	//增加ToolTip
	{
		m_ToolTip.Create(this);
		EnableToolTips(TRUE);
		m_ToolTip.EnableToolTips(TRUE);
		m_ToolTip.Activate(TRUE);
		m_ToolTip.AddTool(GetDlgItem(IDC_ZOOMIN), IDS_MSG_ZOONOUT);
		m_ToolTip.AddTool(GetDlgItem(IDC_ZOOMOUT), IDS_MSG_ZOONIN);
		m_ToolTip.AddTool(GetDlgItem(IDC_ADDTAB), IDS_MSG_ADD);
		m_ToolTip.AddTool(GetDlgItem(IDC_DELTAB), IDS_MSG_DEL);
		m_ToolTip.AddTool(GetDlgItem(IDC_CHTAB), IDS_MSG_NAME);
		m_ToolTip.AddTool(GetDlgItem(IDC_SAVETAB), IDS_MSG_SAVETPL);
		m_ToolTip.AddTool(GetDlgItem(IDC_SAVEPARA), IDS_MSG_SAVEPARA);
		m_ToolTip.AddTool(GetDlgItem(IDC_SAVEDEFAULTPARA), IDS_MSG_SAVEDP);
		m_ToolTip.AddTool(GetDlgItem(ID_EDIT), IDS_MSG_EDIT);
		m_ToolTip.AddTool(GetDlgItem(IDC_RULER), IDS_MSG_RBUT);
		m_ToolTip.AddTool(GetDlgItem(IDOK), IDS_MSG_OK);
		m_ToolTip.AddTool(GetDlgItem(IDCANCEL), IDS_MSG_CANCEL);
		m_ToolTip.AddTool(GetDlgItem(IDC_DEFAULTCONCTRL), IDS_PARADRAG);
		m_ToolTip.AddTool(GetDlgItem(IDC_CONCTRL), IDS_RDEPTH);
		m_ToolTip.AddTool(GetDlgItem(IDC_TEXTHELP), IDS_TEXTHELP);
	}
	//读模板数据	
	BOOL bTemplate = TRUE;
	CString m_FileName = AfxGetApp()->m_pszExeName;
	if (m_MethodName =="") m_MethodName=m_FileName;
	
	CString szTName = GetWorkPath("ProParaCrd")+m_MethodName+".PTP";
	szTemplateName=szTName;

	CFile file;
	CArchive ar(&file, CArchive::load | CArchive::bNoFlushOnDelete);
	CFileException fe;
	if (!file.Open(szTName,CFile::modeRead | CFile::shareExclusive))
	{
		CString msg;
		AfxFormatString1(msg,IDS_NO_PARTEMPLATE,szTName);
		AfxMessageBox(msg, MB_ICONEXCLAMATION|MB_OK);
		bTemplate = FALSE;
	}
	else  
	{
		CString szParTemplateID;
		ar >> szParTemplateID;
		if (szParTemplateID.CompareNoCase("LogExpressTop 处理参数模板"))
		{
			CString msg;
		    AfxFormatString1(msg,IDS_ERROR_PARTEMPLATE,szTName);
		    AfxMessageBox(msg, MB_ICONEXCLAMATION|MB_OK);
			file.Close();
			bTemplate = FALSE;
		}
	}
	szParaNameArry.RemoveAll();
	m_ConTab.DeleteAllItems( );
	m_TabList.RemoveAll( );
	TC_ITEM TabCtrlItem;
	TabCtrlItem.mask = TCIF_TEXT;
	CTabParaObj* pTabPara;
	LV_FINDINFO FindInfo;
	FindInfo.flags = LVFI_STRING;
	if (bTemplate) 
	{
		//读模板	
		m_TabList.Serialize(ar);
		ar.Close();
		file.Close();
		int i, idx;
		CString szName;
		POSITION pos = m_TabList.GetTailPosition();
		while (pos != NULL) 
		{
			pTabPara = (CTabParaObj*)m_TabList.GetPrev(pos);
			TabCtrlItem.pszText = pTabPara->szTabName.GetBuffer(128);
			pTabPara->szTabName.ReleaseBuffer();
			m_ConTab.InsertItem(0, &TabCtrlItem );
			for (i=0; i<pTabPara->szParaNameArry.GetSize(); i++) {
				szName = pTabPara->szParaNameArry.GetAt(i); 
				FindInfo.psz = szName.GetBuffer(128);
				szName.ReleaseBuffer();
				idx = m_DefConCtrl.FindItem( &FindInfo);
				if (idx != -1) {
					szParaNameArry.Add(szName);				
				}
				else 
				{
					CString msg;
		            AfxFormatString1(msg,IDS_ERROR_PARTEMPLATE,szTName);
		            AfxMessageBox(msg, MB_ICONEXCLAMATION|MB_OK);
					//错误的模板参数
					pTabPara->szParaNameArry.RemoveAt(i);
					pTabPara->szParaDspArry.RemoveAt(i);
				}
			}
		}
	}
	else 
	{	CString szHT;
		szHT.LoadString(IDS_NORMALPARA);
		//设置空模板	
		TabCtrlItem.pszText = szHT.GetBuffer(128);
		szHT.ReleaseBuffer();
		pTabPara = new CTabParaObj;
		pTabPara->szTabName = szHT;
		pTabPara->szParaNameArry.RemoveAll();
		pTabPara->szParaDspArry.RemoveAll();
		m_TabList.RemoveAll( );
	    m_TabList.AddHead((CObject*)pTabPara);
		m_ConTab.DeleteAllItems( );
		m_ConTab.InsertItem( 0, &TabCtrlItem );
	}
	m_Ruler.SetMinMax(m_SDep, m_EDep, m_Rlev, &m_DepSlider);
	ReadInpData(); 
	m_ConTab.SetCurSel(0);
	m_ConCtrl.FillCon(pTabPara);
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	return TRUE; 
}
BOOL  CParameterEdit::ReadInpData()
{
	BOOL bFirst = TRUE;
	LV_FINDINFO FindInfo;
	FindInfo.flags = LVFI_STRING;
	CString szName, szValue;
	CStringArray* pDefParaValue = new CStringArray;
	pDefParaValue->RemoveAll();
	for (int i=0; i<szParaNameArry.GetSize(); i++) 
	{
		szName = szParaNameArry.GetAt(i); 
		FindInfo.psz = szName.GetBuffer(128);
		szName.ReleaseBuffer();
		int idx = m_DefConCtrl.FindItem( &FindInfo);
		if (idx != -1) 
		{
			szValue = m_DefConCtrl.GetItemText(idx, 1);
			pDefParaValue->Add(szValue);				
		}
	}
	pParaValue = pDefParaValue;
	CStringArray* pParaValue1;
	POSITION pos = m_ValueList.GetHeadPosition();
	while (pos != NULL) 
	{
		pParaValue1 = (CStringArray*)m_ValueList.GetNext(pos);
		if (pParaValue1 != NULL) delete  pParaValue1;
	}
	m_Ruler.m_ObList.RemoveAll();
	CRulerItem* pObj;
	pos = m_Ruler.m_ObList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CRulerItem*)m_Ruler.m_ObList.GetNext(pos);
		if (pObj != NULL ) delete pObj;
	}
	m_ValueList.RemoveAll( );
	BOOL bDef = FALSE;
	CStdioFile m_File;
	if (!m_File.Open( m_InpFileName, CFile::modeRead | CFile::typeText ))
	{
		m_File.Close ();
		CString szDefInpName;
		szDefInpName=GetWorkPath("ProParaCrd")+m_MethodName+".DEF";
		if (!m_File.Open( szDefInpName, CFile::modeRead | CFile::typeText ))
		{
            CString msg;
			AfxFormatString1(msg,IDS_DEFAULT_PARFILE_ERROR,szDefInpName);
            AfxMessageBox(msg,MB_ICONEXCLAMATION|MB_OK);  
			//缺省参数文件打开失败！ 
			//设缺省参数值
			m_ValueList.AddHead((CObject*)pParaValue);
			m_Ruler.AddTBRulerItem();
			return FALSE;
		}
		else 
			bParaModified = TRUE;
		bDef = TRUE;
	}
	CString szCurve, szCurve1;
	double sdep, edep, sdep1;
	int nAtt, idx;
	sdep = 0;
	edep = 99999;
	int is,ie;
	CString szStr=" ";
	BOOL bNewDep = FALSE;
	while (m_File.ReadString(szStr) && (szStr.CompareNoCase("END")))
	{
		szStr.MakeUpper();
		while (szStr.Find(' ') != -1)
		{
			is = szStr.Find(' ');
			szStr = szStr.Left(is) + szStr.Right(szStr.GetLength()-is-1);
		}
		if (szStr == "") continue;											//空行
		if ((szStr.GetAt(0) == '#') || (szStr.GetAt(0) == '/')) continue;	//注释行
		if (szStr.FindOneOf("<>") != -1) 									//重定向行
		{
			CString szStr1 = szStr;
			while (szStr1.FindOneOf("<>") != -1)
			{
				is = szStr1.FindOneOf("<>"); 
				ie = szStr1.Find(',');
				if (ie == -1) ie= szStr1.GetLength();
				szCurve = szStr1.Left(is);
				char nChar = szStr1.GetAt(is);
				szCurve1= szStr1.Mid(is+1, ie-is-1);
				szStr1 = szStr1.Right(szStr1.GetLength()-ie-1);
				if (szCurve1 == "") continue;
				if (!szCurve.CompareNoCase(szCurve1)) continue;
				if (nChar == '<')											//输入定向
					m_InCtrl.AddInCurve(szCurve, szCurve1);
				else														//输出定向
					m_OutCtrl.AddOutCurve(szCurve, szCurve1);
			}
		}
		if ((szStr.GetAt(0) >= '0') && (szStr.GetAt(0) <= '9') && (!bDef))	//深度段
		{
			ie = szStr.Find(',');
			if (ie == -1) continue;											//错误深度行
			szCurve = szStr.Left(ie);
			szCurve1 = szStr.Right(szStr.GetLength()-ie-1);
			sdep1 = atof(szCurve);
			if (sdep1<=sdep) 
			{
				char szDepth[20];
				CString szS;
				dtos(sdep1, szDepth);
				strcpy(szS.GetBuffer(128), szDepth);
				szS.ReleaseBuffer();
				CString msg;
				AfxFormatString1(msg,IDS_DEPTH_ERROR,szS);
				AfxMessageBox(msg, MB_ICONEXCLAMATION|MB_OK);
				//深度值小于上一段深度！\n该深度段被忽略 
				continue;
			}
			sdep = sdep1;
			edep = atof(szCurve1);
			if (sdep < m_SDep ) sdep = m_SDep;
			if (edep > m_EDep ) edep = m_EDep;
			if ((sdep >=edep) || (sdep >= m_EDep) || (edep <= m_SDep)) continue;
			nAtt = AttNor;
			if (m_Ruler.m_ObList.GetCount() == 0) nAtt =AttTop;
			m_Ruler.AddRulerItem(sdep, nAtt);
			bNewDep = TRUE;
			continue;
		}
		if ((!bNewDep) && (!bDef)) continue;
		{																    //读常数值
			CStringArray* pParaValue1 = new CStringArray;
			for (int k=0; k<pParaValue->GetSize(); k++)
				pParaValue1->Add(pParaValue->GetAt(k));
			pParaValue = pParaValue1;
			LV_FINDINFO FindInfo;
			FindInfo.flags = LVFI_STRING;
			CString szStr1 = szStr;
			while (szStr1.FindOneOf("=") != -1)
			{
				is = szStr1.FindOneOf("="); 
				ie = szStr1.Find(',');
				if (ie == -1) ie= szStr1.GetLength();
				szCurve = szStr1.Left(is);
				szCurve1= szStr1.Mid(is+1, ie-is-1);
				szStr1 = szStr1.Right(szStr1.GetLength()-ie-1);
				FindInfo.psz = szCurve.GetBuffer(128);
				szCurve.ReleaseBuffer();
				idx = m_DefConCtrl.FindItem( &FindInfo);
				if (idx != -1) {
					BOOL bInTpl = FALSE;
					int j;
					for (j=0; j<szParaNameArry.GetSize(); j++)
					{
						if (szParaNameArry.GetAt(j) == szCurve) 
						{
							bInTpl = TRUE;
							break;
						}
					}
					if (!bInTpl)
					{
						CString szDefVal = m_DefConCtrl.GetItemText(idx, 1);
						CTabParaObj* pTabPara1 = (CTabParaObj*)m_TabList.GetTail();
						pTabPara1->szParaNameArry.Add(szCurve);
						pTabPara1->szParaDspArry.Add("");
						szParaNameArry.Add(szCurve);
						CStringArray* pParaValue1;
						POSITION pos = m_ValueList.GetHeadPosition();
						while (pos != NULL) 
						{
							pParaValue1 = (CStringArray*)m_ValueList.GetNext(pos);
							if (pParaValue1 != NULL) pParaValue1->Add(szCurve1);
						}
						pParaValue->Add(szCurve1);				
					}
					else
					{
						CString szTmp = pParaValue->GetAt(j);
						pParaValue->SetAt(j,szCurve1);
					}
				}
				else 
				{
					CString msg;
					AfxFormatString1(msg,IDS_WRONG_PARNAME,szCurve);
					AfxMessageBox(msg, MB_ICONEXCLAMATION|MB_OK);
					//错误的参数名称
					continue;
				}
			}
			if ((szStr.GetAt(szStr.GetLength()-1) != ',') && (!bDef))
			{
				m_ValueList.AddTail((CObject*)pParaValue);
				bNewDep = FALSE;
			}
		}
	}
	if (m_Ruler.m_ObList.GetCount() == 0) 
	{
		m_Ruler.AddRulerItem(m_SDep, AttTop);
		CStringArray* pParaValue2 = new CStringArray;
		for (int k=0; k<pParaValue->GetSize(); k++)
			pParaValue2->Add(pParaValue->GetAt(k));
		pParaValue = pParaValue2;
		m_ValueList.AddTail((CObject*)pParaValue);
	}
	if ((bNewDep) && (!bDef))
	{																	    //读常数值
		CStringArray* pParaValue1 = new CStringArray;
		for (int k=0; k<pParaValue->GetSize(); k++)
			pParaValue1->Add(pParaValue->GetAt(k));
		pParaValue = pParaValue1;
		m_ValueList.AddTail((CObject*)pParaValue);
	}
	if (bFirst)	delete pDefParaValue;
	if (edep > m_EDep) edep = m_EDep;
	nAtt = AttBtm;
	m_Ruler.AddRulerItem(edep, nAtt);
	m_File.Close();
	pParaValue = (CStringArray*)m_ValueList.GetHead();
	return TRUE;
}
BOOL CParameterEdit::Is2DCurve(CString szCN)
{
	BOOL b2D = TRUE;
	CFIDIO*	FIDFile = new CFIDIO(szCN);
	if (!FIDFile->Open() || (int)FIDFile->GetStartDepth()==-99999 || (int)FIDFile->GetEndDepth()==-99999 ||
		 (int)FIDFile->GetDepthLevel()== -99999|| FIDFile->GetDimenSion() != 2)
	{
		b2D = FALSE;		
	}
	FIDFile->Close();
	delete	FIDFile;
	return b2D;
}

void CParameterEdit::InitCombo() 
{
	char szFind[512];
	CFIDIO::GetFullWellName(m_InpFileName,szFind);
	if (szFind == "") return;
	CFIDIO::GetFileName(szFind,szFind,'I');
	strcat(szFind,".*");
	m_Combo.ResetContent();

	char szFIDName[40];
	CString szDir;
	CFIDIO::GetPath(szFind,szDir.GetBuffer(512));
	szDir.ReleaseBuffer();

	struct _finddata_t c_file;
	long hFile;
	if ((hFile = _findfirst(szFind, &c_file )) != -1L)
	{	
	    if (Is2DCurve(szDir+c_file.name))
		{
			CFIDIO::GetFIDName(c_file.name,szFIDName);
			if (szFIDName[0] != 0) 	m_Combo.AddString(szFIDName); 
		}

		while(_findnext(hFile,&c_file) == 0 ) 
		{
		    if (Is2DCurve(szDir+c_file.name))
			{
				CFIDIO::GetFIDName(c_file.name,szFIDName);
				if (szFIDName[0] != 0) m_Combo.AddString(szFIDName); 
			}
		}	
		_findclose( hFile );
	}
}

void CParameterEdit::OnSavetab() 
{
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	bTplModified = FALSE;
	TC_ITEM TabCtrlItem;
	TabCtrlItem.mask = TCIF_TEXT;
	CFile file;
	if (!file.Open(szTemplateName,CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive))
	{
		CString msg;
		AfxFormatString1(msg,IDS_CANNT_CREATE_FILE,szTemplateName);
		AfxMessageBox(msg, MB_ICONEXCLAMATION|MB_OK);
		//参数模板产生失败", "文件错误
		return;
	}
	CArchive ar(&file, CArchive::store | CArchive::bNoFlushOnDelete);
	TRY
	{
		CString szParTemplateID = "LogExpressTop 处理参数模板";
		ar << szParTemplateID;
		m_TabList.Serialize(ar);
		ar.Close();
		file.Close();	
	}
	CATCH_ALL(e)
	{
		file.Abort();
		CString msg;
		AfxFormatString1(msg,IDS_CANNT_WRITE_FILE,szTemplateName);
		AfxMessageBox(msg, MB_ICONEXCLAMATION|MB_OK);
		//保存参数模板文件失败
		return;
	}
	END_CATCH_ALL
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}
void CParameterEdit::SavePara(BOOL bDefPara) 
{
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	CString szInpName;
	if (bDefPara)
	{
		szInpName=GetWorkPath("ProParaCrd")+m_MethodName+".DEF";	
	}
	else szInpName = m_InpFileName;

	CStdioFile m_File;
	if (!m_File.Open( szInpName, CFile::modeCreate | CFile::modeWrite | CFile::typeText ))
	{
		#ifdef _DEBUG
			afxDump << szInpName << "Create para file failed" << "\n";
		#endif
		CString msg;
		AfxFormatString1(msg,IDS_CANNT_CREATE_FILE,szTemplateName);
		AfxMessageBox(msg, MB_ICONEXCLAMATION|MB_OK);
		//参数文件产生失败!\n参数文件没有保存。
		return;
	}
	CString szStr = "";
	CString szName, szValue;
	int idx, i;
	m_File.WriteString("#  测井资料计算机解释平台\n");
	m_File.WriteString("#  版权所有 四川石油管理局测井公司 2000--2010\n");
	m_File.WriteString("#════════════════════════════\n");
	m_File.WriteString("#  应用软件名称："+m_MethodName+"\n");
	CTime date = CTime::GetCurrentTime();
	CString DateStr;
	DateStr.Format ("#  处理日期：%d-%d-%d",date.GetYear (),date.GetMonth (),date.GetDay ());
	CTime time = CTime::GetCurrentTime();
	CString strTime = time.Format(_T("  %I-%M-%S\n"));
	DateStr+=strTime;
	m_File.WriteString(DateStr);

	char StrFormat[256];
	//输入曲线
	m_File.WriteString("#\n");
	m_File.WriteString("#  输入曲线\n");
	szStr = "#";
	for (i=0; i<m_InCtrl.GetItemCount(); i++)
	{
		szName = m_InCtrl.GetItemText(i, 0);
		if (szStr != "#")
		{
			szStr +="    ";
			if (szStr.GetLength()>40) 
			{
				m_File.WriteString(szStr+"\n");
				szStr = "#";
			}
		}		
		sprintf(StrFormat,"%-8s ",szName);
		szStr+=StrFormat;
	}
	if (szStr != "#") 
		m_File.WriteString(szStr+"\n");
	m_File.WriteString("#  输入曲线重定向\n");
	szStr = "";
	for (i=0; i<m_InCtrl.GetItemCount(); i++)
	{
		szName = m_InCtrl.GetItemText(i, 0);
		szValue = m_InCtrl.GetItemText(i, 1);
		if(szName.CompareNoCase(szValue) &&(szValue != ""))
		{
			if (szStr != "")
			{
				szStr +=",";
				if (szStr.GetLength()>40) 
				{
					m_File.WriteString(szStr+"\n");
					szStr = "";
				}
			}
			szStr+=szName+"<"+szValue;
		}
	}
	if (szStr != "") 
	{
		m_File.WriteString(szStr+"\n");
	}
	
	//输出曲线
	m_File.WriteString("#\n");
	m_File.WriteString("#  输出曲线\n");	
	szStr = "#";
	for (i=0; i<m_OutCtrl.GetItemCount(); i++)
	{
		szName = m_OutCtrl.GetItemText(i, 0);
		if (szStr != "#")
		{
			szStr +="    ";
			if (szStr.GetLength()>40) 
			{
				m_File.WriteString(szStr+"\n");
				szStr = "#";
			}
		}
		sprintf(StrFormat,"%-8s ",szName);
		szStr+=StrFormat;
	}
	if (szStr != "#") 
		m_File.WriteString(szStr+"\n");
	m_File.WriteString("#  输出曲线重定向\n");
	szStr = "";
	for (i=0; i<m_OutCtrl.GetItemCount(); i++)
	{
		szName = m_OutCtrl.GetItemText(i, 0);
		szValue = m_OutCtrl.GetItemText(i, 1);
		if(szName.CompareNoCase(szValue) && (szValue != ""))
		{
			if (szStr != "")
			{
				szStr +=",";
				if (szStr.GetLength()>40) 
				{
					m_File.WriteString(szStr+"\n");
					szStr = "";
				}
			}
			szStr+=szName+">"+szValue;
		}
	}
	if (szStr != "") 
	{
		m_File.WriteString(szStr+"\n");
	}
	//参数
	m_File.WriteString("#\n");
	m_File.WriteString("#════════════处理参数════════════\n");

	CStringArray* pParaV1 = NULL;
	CStringArray* pParaV2 = NULL;
	for (idx=0; idx<m_ValueList.GetCount(); idx++)
	{
		CRulerItem* pR1 = (CRulerItem*)m_Ruler.m_ObList.GetAt(m_Ruler.m_ObList.FindIndex(idx));
		CRulerItem* pR2 = (CRulerItem*)m_Ruler.m_ObList.GetAt(m_Ruler.m_ObList.FindIndex(idx+1));
		if (!bDefPara) 
		{
			char szDepth[20];
			dtos(pR1->Depth, szDepth);
			strcpy(szStr.GetBuffer(128), szDepth);
			szStr.ReleaseBuffer();
			dtos(pR2->Depth, szDepth);
			szStr = szStr+","+szDepth;
		}
		else szStr = "0, 9999";
		m_File.WriteString(szStr+"\n");
		pParaV1 = pParaV2;
		pParaV2 = (CStringArray*)m_ValueList.GetAt(m_ValueList.FindIndex(idx));
		szStr = "";
		for (i=0; i<szParaNameArry.GetSize(); i++)
		{
			szName = szParaNameArry.GetAt(i);
			szValue = pParaV2->GetAt(i);
			if ((pParaV1 == NULL) || (szValue.CompareNoCase(pParaV1->GetAt(i))))
			{
				if (szStr != "")
				{
					szStr +=",";
					if (szStr.GetLength()>40) 
					{
						m_File.WriteString(szStr+"\n");
						szStr = "";
					}
				}
				szStr+=szName+"="+szValue;
			}
		}
		if (szStr != "") 
		{
			m_File.WriteString(szStr+"\n");
		}
		if (bDefPara) idx = m_OutCtrl.GetItemCount();

	}
	m_File.WriteString("END\n");
	m_File.WriteString("\n");
	m_File.Close();
	if (!bDefPara) bParaModified = FALSE;
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}
void CParameterEdit::OnSavepara() 
{
	SavePara(FALSE);
}
void CParameterEdit::OnSavedefaultpara() 
{
	SavePara(TRUE);
}
void CParameterEdit::OnSelchangeContab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	int item = m_ConTab.GetCurSel( );
	if (item == -1) return;
	CTabParaObj* pTabPara = (CTabParaObj*)m_TabList.GetAt(m_TabList.FindIndex(item));
	m_ConCtrl.FillCon(pTabPara);
}

void CParameterEdit::OnEdit() 
{
	if (bParaModified) 
	{
		CString msg;
		AfxFormatString1(msg,IDS_SAVE_PARFILE_CHAN,m_InpFileName);
		int nRet = AfxMessageBox(msg, MB_YESNOCANCEL|MB_ICONQUESTION);
		//保存当前参数修改？
		if (  nRet == IDCANCEL) return;
		if (  nRet == IDYES) OnSavepara();
	}
	CEditdlg dlg;
	dlg.m_ParaExt = m_ParaExt;
	dlg.m_MethodName = m_MethodName;
	dlg.szFileName = m_InpFileName;
	dlg.DoModal();

	ReadInpData(); 
	int item = m_ConTab.GetCurSel( );
	CTabParaObj* pTabPara = (CTabParaObj*)m_TabList.GetAt(m_TabList.FindIndex(item));
	m_ConCtrl.FillCon(pTabPara);
	m_Ruler.m_SelZoon =0;
	m_Ruler.Invalidate(NULL);
	UpdateData(TRUE);
	
}
void CParameterEdit::OnCancel() 
{
	int nRet;
	if (bTplModified)
	{   
		CString msg;
		AfxFormatString1(msg,IDS_SAVE_TEMCHA,szTemplateName);
		nRet = AfxMessageBox(msg, MB_YESNOCANCEL|MB_ICONQUESTION);
		//保存当前模板修改？
		if ( nRet== IDCANCEL) return;
		if ( nRet == IDYES) 
		{
			OnSavetab();
			OnSavepara();
		}
	}
	if (bParaModified) 
	{
		CString msg;
		AfxFormatString1(msg,IDS_SAVE_TEMCHA,szTemplateName);
		nRet = AfxMessageBox(msg, MB_YESNOCANCEL|MB_ICONQUESTION);
		//保存当前参数修改？
		if ( nRet== IDCANCEL) return;
		if ( nRet == IDYES) 
		{
			OnSavepara();
		}
	}
	CDialog::OnCancel();
}

void CParameterEdit::OnOK() 
{
	if (bTplModified)
	{
		CString msg;
		AfxFormatString1(msg,IDS_SAVE_TEMCHA,szTemplateName);
		int nRtn = AfxMessageBox(msg, MB_YESNOCANCEL|MB_ICONQUESTION);
		//保存当前模板修改？
		if (nRtn == IDCANCEL) return;
		if (nRtn == IDYES) OnSavetab();
	}	
	OnSavepara();
	CDialog::OnOK();
}
void CParameterEdit::OnChangeEdit() 
{
	CEditCtrl* pCtrl = (CEditCtrl*)m_Edit.GetParent();
	CString szEdit;
	m_Edit.GetWindowText(szEdit);
	if (pCtrl != NULL) {
		int row = pCtrl->GetNextItem( -1, LVNI_SELECTED );
		if (row != -1) 
		{
			CString szV = pCtrl->GetItemText( row, pCtrl->col);
			pCtrl->SetItemText( row, pCtrl->col, szEdit );
			if (pCtrl == &m_ConCtrl) {
				if ((pCtrl->col == 2) && (m_ConCtrl.pTabPara != NULL)) 
					m_ConCtrl.pTabPara->szParaDspArry.SetAt(row, szEdit);
				else
				if ((pCtrl->col == 1) && (pParaValue != NULL) && (row<szParaNameArry.GetSize()))
				{
					CString szName = m_ConCtrl.pTabPara->szParaNameArry.GetAt(row);
					for (int j=0; j<szParaNameArry.GetSize(); j++)
					{
						if (szParaNameArry.GetAt(j) == szName) 
						{
							CStringArray* pParaValue1;
							POSITION pos = m_ValueList.Find(pParaValue);
							while (pos != NULL) 
							{
								pParaValue1 = (CStringArray*)m_ValueList.GetNext(pos);
								if (pParaValue1 != NULL) 
								{
									CString szT = pParaValue1->GetAt(j);
									if (!szT.CompareNoCase(szV)) pParaValue1->SetAt(j, szEdit);
									else break;
								}
							}
							pParaValue->SetAt(j, szEdit);
							break;
						}
					}
				}
			}
		}
	}
}

void CParameterEdit::OnSelchangeCombo() 
{
	if (m_Combo.GetCurSel( ) == -1) return;
	CString szEdit;
	m_Combo.GetLBText(m_Combo.GetCurSel( ), szEdit);
	if (m_InCtrl.m_hWnd != NULL) {
		int row = m_InCtrl.GetNextItem( -1, LVNI_SELECTED );
		if (row != -1) 
		{
			bParaModified = TRUE;
			m_InCtrl.SetItemText( row, 1, szEdit );
		}
	}
}

void CParameterEdit::OnAddtab() 
{
	CString szHT;
	CTabNameDlg m_NameDlg;
	szHT.LoadString(IDS_OTHERPARA);
	m_NameDlg.m_TabName=szHT;
	if (m_NameDlg.DoModal() != IDOK) return;
	TC_ITEM TabCtrlItem;
	TabCtrlItem.mask = TCIF_TEXT;
	CString szText;
	for (int i=0; i<m_ConTab.GetItemCount(); i++) {
		TabCtrlItem.pszText = szText.GetBuffer(128);
		TabCtrlItem.cchTextMax=128;
		m_ConTab.GetItem( i, &TabCtrlItem );
		szText.ReleaseBuffer();
		if (!szText.CompareNoCase(m_NameDlg.m_TabName)) 
		{
			CString msg;
			AfxFormatString1(msg,IDS_PARAMETER_EXIST,szText);
			AfxMessageBox(msg, MB_OK|MB_ICONSTOP);
			return;
		}
	}
	TabCtrlItem.pszText = m_NameDlg.m_TabName.GetBuffer(128);
	m_NameDlg.m_TabName.ReleaseBuffer();
	m_ConTab.SetCurSel(m_ConTab.InsertItem( 0, &TabCtrlItem ));
	CTabParaObj* pTabPara = new CTabParaObj;
	pTabPara->szTabName = m_NameDlg.m_TabName;
	pTabPara->szParaNameArry.RemoveAll();
	pTabPara->szParaDspArry.RemoveAll();
	m_TabList.AddHead((CObject*)pTabPara);
	m_ConTab.SetCurSel(0);
	m_ConCtrl.FillCon(pTabPara);
	bTplModified = TRUE;
}

void CParameterEdit::OnChtab() 
{
	int item =  m_ConTab.GetCurSel( );
	TC_ITEM TabCtrlItem;
	TabCtrlItem.mask = TCIF_TEXT;
	CString szText;
	TabCtrlItem.pszText = szText.GetBuffer(128);
	TabCtrlItem.cchTextMax=128;
	m_ConTab.GetItem( item, &TabCtrlItem );
	szText.ReleaseBuffer();
	CTabNameDlg m_NameDlg;
	m_NameDlg.m_TabName=szText;
	if (m_NameDlg.DoModal() != IDOK) return;
	for (int i=0; i<m_ConTab.GetItemCount(); i++) {
		TabCtrlItem.pszText = szText.GetBuffer(128);
		TabCtrlItem.cchTextMax=128;
		m_ConTab.GetItem( i, &TabCtrlItem );
		szText.ReleaseBuffer();
		if ((!szText.CompareNoCase(m_NameDlg.m_TabName)) && (i != item))
		{
			CString msg;
			AfxFormatString1(msg,IDS_PARAMETER_EXIST,szText);
			AfxMessageBox(msg, MB_OK|MB_ICONSTOP);
			return;
		}
	}
	szText = m_NameDlg.m_TabName;
	TabCtrlItem.pszText = szText.GetBuffer(128);
	TabCtrlItem.cchTextMax=128;
	m_ConTab.SetItem(item, &TabCtrlItem );
	POSITION POS= m_TabList.FindIndex(item);
	CTabParaObj*pTabPara =(CTabParaObj*)m_TabList.GetAt(POS);
	pTabPara->szTabName = szText;
	m_TabList.SetAt(POS, (CObject*)pTabPara);
	bTplModified = TRUE;
}

void CParameterEdit::OnDeltab() 
{
	if (m_ConTab.GetItemCount() <= 1) return;
	TC_ITEM TabCtrlItem;
	TabCtrlItem.mask = TCIF_TEXT;
	CString szText;
	TabCtrlItem.pszText = szText.GetBuffer(128);
	TabCtrlItem.cchTextMax=128;
	m_ConTab.GetItem( m_ConTab.GetCurSel( ), &TabCtrlItem );
	szText.ReleaseBuffer();
	CString msg;
	AfxFormatString1(msg,IDS_DEL_PARGROUP,szText);
	if (AfxMessageBox(msg, MB_YESNOCANCEL|MB_ICONQUESTION) != IDYES) return;
	int item = m_ConTab.GetCurSel( );
	if (item == -1) return;
	for (int i=0; i<m_ConCtrl.GetItemCount(); i++)
		m_ConCtrl.DelItem(i);
	m_ConTab.DeleteItem(item);
	m_TabList.RemoveAt(m_TabList.FindIndex(item));
	if (item >= m_ConTab.GetItemCount()) item -= 1;
	m_ConTab.SetCurSel(item);
	CTabParaObj* pTabPara = (CTabParaObj*)m_TabList.GetAt(m_TabList.FindIndex(item));
	m_ConCtrl.FillCon(pTabPara);
	bTplModified = TRUE;
}
void CParameterEdit::OnHelp() 
{
	CString TextHelpName;
	TextHelpName=GetWorkPath("ProParaCrd")+m_MethodName+".TXT";

	CEditdlg dlg;
	dlg.m_MethodName = m_MethodName;
	dlg.szFileName = TextHelpName;
	dlg.DoModal();
}
void CParameterEdit::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CWnd* pWnd = GetFocus();
	if (pWnd == NULL) return;
	if (pWnd == &m_DepSlider)
	{
		m_Ruler.Invalidate();
	}
}
void CParameterEdit::OnDepZoomin() 
{
	if (m_Ruler.m_ZoomNum>1) 
	{
		m_Ruler.m_ZoomNum --;
		m_Ruler.RecalcRuler();
	}
	if (m_Ruler.m_ZoomNum == 1)
	{
		m_ZoomIn.EnableWindow(FALSE);
		m_DepSlider.ShowWindow(SW_HIDE);
	}
}
void CParameterEdit::OnDepZoomout() 
{
	m_Ruler.m_ZoomNum ++;
	m_ZoomIn.EnableWindow(TRUE);
	m_DepSlider.ShowWindow(SW_SHOW);
	m_Ruler.RecalcRuler();
}


/////////////////////////////////////////////////////////////////////////////
// CDefConCtrl

CDefConCtrl::CDefConCtrl()
{
	m_bDragging = FALSE;
	m_pimageListDrag = NULL;
}

CDefConCtrl::~CDefConCtrl()
{
}


BEGIN_MESSAGE_MAP(CDefConCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CDefConCtrl)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(LVN_BEGINRDRAG, OnBegindrag)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefConCtrl message handlers
void CDefConCtrl::InitCtrl(int No, INPNAME* pNameC, float* pValueC)
{
	CString szHT;
	CRect rect;
	GetClientRect(&rect);
	int wd = rect.Width()-GetSystemMetrics(SM_CXVSCROLL);
	LV_COLUMN		lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;
	szHT.LoadString(IDS_PARANAME);
	lvcolumn.pszText = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	lvcolumn.iSubItem = 0;
	lvcolumn.cx = GetStringWidth(lvcolumn.pszText)+17;
	InsertColumn(0, &lvcolumn);
	szHT.LoadString(IDS_PARAVALUE);
	lvcolumn.pszText = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	lvcolumn.iSubItem = 1;
	lvcolumn.cx = wd-GetColumnWidth(0);
	InsertColumn(1, &lvcolumn);
	SetTextColor(0x08000);
	LV_ITEM	lvitem;
	int iActualItem;
	char tt[20];
	lvitem.mask = LVIF_TEXT;
	for (int i=0; i<No; i++) {
		lvitem.iSubItem =0;
		lvitem.iItem =0;
		lvitem.pszText=pNameC[i];
		iActualItem = InsertItem(&lvitem);
		dtos((double)pValueC[i], tt);
		lvitem.pszText=tt;
		lvitem.iItem =iActualItem;
		lvitem.iSubItem = 1;
		SetItem(&lvitem);
	}
}
void CDefConCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CListCtrl::OnLButtonUp(nFlags, point);
	if (m_bDragging)  // end of the drag operation
	{
		m_bDragging = FALSE;
		m_pimageListDrag->DragLeave(NULL);
		m_pimageListDrag->EndDrag();
		::ReleaseCapture();
		CParameterEdit* pEditor =(CParameterEdit*) GetParent();
		CRect rect;
		pEditor->m_ConCtrl.GetClientRect(&rect);
		pEditor->m_ConCtrl.ClientToScreen(&rect);
		ClientToScreen(&point);
		if (rect.PtInRect(point)) {
			pEditor->m_ConCtrl.ScreenToClient(&point);
			pEditor->m_ConCtrl.AddCon(szName, szValue, point);
		}
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	}
}

void CDefConCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bDragging)
	{
		CPoint ptScr = point;
		ClientToScreen(&ptScr);
		m_pimageListDrag->DragMove(ptScr- m_sizeDelta);  // move the image
		CParameterEdit* pEditor =(CParameterEdit*) GetParent();
		CRect rect;
		pEditor->m_ConCtrl.GetClientRect(&rect);
		pEditor->m_ConCtrl.ClientToScreen(&rect);
		ClientToScreen(&point);
		if (rect.PtInRect(point))
			::SetCursor(AfxGetApp()->LoadCursor(IDC_DRAG));
		else 
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));


	}
	CListCtrl::OnMouseMove(nFlags, point);
	CParameterEdit* pEditor = (CParameterEdit*)GetParent();
	CString szHT;
	szHT.LoadString(IDS_PARADRAG);
	if (HitTest(point) != -1)
	{
		CString szHT;
		szHT.LoadString(IDS_PARADRAG);
		if (szHT.GetAt(0) =='Y') 
			pEditor->m_ToolTip.UpdateTipText("You can drag a parameter to left edit list", this);
		else 
			pEditor->m_ToolTip.UpdateTipText("将参数拖拉到左边的参数列表中", this);
	}
	else
		pEditor->m_ToolTip.UpdateTipText("", this);
}

void CDefConCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint			ptItem, ptAction, ptImage;
	NM_LISTVIEW		*pnmListView = (NM_LISTVIEW *)pNMHDR;
	ASSERT(!m_bDragging);
	m_bDragging = TRUE;
	m_iItemDrag = pnmListView->iItem;
	szName = GetItemText( m_iItemDrag, 0);
	szValue = GetItemText( m_iItemDrag, 1);
	ptAction = pnmListView->ptAction;
	GetItemPosition(m_iItemDrag, &ptItem);                 // ptItem is relative to (0,0) and not the view origin
	GetOrigin(&m_ptOrigin);
	m_pimageListDrag = CreateDragImage(m_iItemDrag, &ptImage);
	m_sizeDelta = ptAction - ptImage;                      // difference between cursor pos and image pos
	m_ptHotSpot = ptAction - ptItem + m_ptOrigin;          // calculate hotspot for the cursor
	m_pimageListDrag->DragShowNolock(TRUE);				   // lock updates and show drag image
	m_pimageListDrag->SetDragCursorImage(0, m_ptHotSpot);  // define the hot spot for the new cursor image
	m_pimageListDrag->BeginDrag(0, CPoint(0, 0));
	ptAction -= m_sizeDelta;
	ClientToScreen(&ptAction);
	m_pimageListDrag->DragEnter(NULL, ptAction);
	m_pimageListDrag->DragMove(ptAction);				   // move image to overlap original icon
	SetCapture();	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
// CEditCtrl

CEditCtrl::CEditCtrl()
{
	col = 1;
	ColNum = 2;
}

CEditCtrl::~CEditCtrl()
{
}


BEGIN_MESSAGE_MAP(CEditCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CEditCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_VSCROLL()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditCtrl message handlers
void CEditCtrl::InitCtrl(CWnd* pEdit)
{
	DeleteAllItems( );
//	this->SetBkColor (RGB(212,208,200));
//	this->SetTextBkColor (RGB(212,208,200));
	m_Edit = pEdit;
}
void CEditCtrl::PositionEdit() 
{
	int row;
	row = GetNextItem( -1, LVNI_SELECTED );
	if ( row == -1) {
		m_Edit->ShowWindow(SW_HIDE);
		return;	
	}
	::SetParent(m_Edit->GetSafeHwnd(), this->GetSafeHwnd());
	CRect rect;
	GetItemRect( row, &rect, LVIR_BOUNDS );
	rect.left=0;
	for (int i=0; i<col; i++) {
		rect.left +=GetColumnWidth(i);
	}
	rect.right =rect.left+GetColumnWidth(col);
	rect.left -= GetScrollPos(SB_HORZ);
	rect.right -= GetScrollPos(SB_HORZ);
	CRect rect1;
	GetClientRect(&rect1);
	if (rect.right > rect1.right) rect.right=rect1.right;
	CString szText = GetItemText( row, col);
	m_Edit->SetWindowText(szText);
	m_Edit->MoveWindow(rect.left, rect.top-1, rect.Width(), rect.Height()+2, FALSE);
	m_Edit->ShowWindow(SW_SHOW);
	m_Edit->SetFocus();
	CParameterEdit* pEditor = (CParameterEdit*)GetParent();
	if (m_Edit->IsKindOf(RUNTIME_CLASS(CEdit))) 
	{
		((CEdit*)m_Edit)->SetSel(0, -1, TRUE);
		pEditor->m_Combo.ShowWindow(SW_HIDE);
	}
	else
	{
		((CComboBox*)m_Edit)->SetCurSel(((CComboBox*)m_Edit)->FindStringExact(-1, szText));
		pEditor->m_Edit.ShowWindow(SW_HIDE);
	}
}
int CEditCtrl::HitTest( CPoint point, UINT* pFlags )
{
	int row = -1;
	CRect rect;
	for (int i=0; i<GetItemCount(); i++) 
	{
		GetItemRect( i, &rect, LVIR_BOUNDS );
		if (rect.PtInRect(point))
		{
			row = i;
			break;
		}
	}
	return row;
}
void CEditCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int xx = point.x; 
	CListCtrl::OnLButtonDown(nFlags, point);
	int row = HitTest(point);
	if ( row == -1) return;
	col =1;
	int nLeft = GetColumnWidth(0)-GetScrollPos(SB_HORZ);
	for (int i=1; i <ColNum; i++) {
		if (xx> nLeft) col = i;
		else break;
		nLeft += GetColumnWidth(i);
	}
	SetItemState( row, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
}

void CEditCtrl::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	PositionEdit();
	*pResult = 0;
}
void CEditCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
	PositionEdit();
}
void CEditCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
	PositionEdit();
}

void CEditCtrl::OnPaint() 
{
	CListCtrl::OnPaint();
	if (m_Edit->GetParent() != this) return;
	if (!m_Edit->IsWindowVisible()) return;
	int row;
	row = GetNextItem( -1, LVNI_SELECTED );
	if ( row == -1) return;
	if ( row <GetTopIndex()) {
		m_Edit->ShowWindow(SW_HIDE);
		return;
	}
    CRect rect1;
	m_Edit->GetWindowRect(&rect1);
	CRect rect2;
	GetItemRect( row, &rect2, LVIR_BOUNDS );
	ClientToScreen(&rect2);
	if (abs(rect2.top-rect1.top) >=2)
     PositionEdit();
}
BOOL CEditCtrl::AnyKey()
{
	CParameterEdit* pEditor = (CParameterEdit*)GetParent();
	if (col == 1) pEditor->bParaModified = TRUE;
	if (col == 2) pEditor->bTplModified = TRUE;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CInCtrl

CInCtrl::CInCtrl()
{
}

CInCtrl::~CInCtrl()
{
}


BEGIN_MESSAGE_MAP(CInCtrl, CEditCtrl)
	//{{AFX_MSG_MAP(CInCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInCtrl message handlers
void CInCtrl::InitCtrl(int No, INCURVE* pInCurve, CWnd* pEdit)
{
	CEditCtrl::InitCtrl(pEdit);
	CRect rect;
	GetClientRect(&rect);
	int wd = rect.Width()-GetSystemMetrics(SM_CXVSCROLL);
	LV_COLUMN		lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;
	CString szHT;
	szHT.LoadString(IDS_INCURVE);
	lvcolumn.pszText = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	lvcolumn.iSubItem = 0;
	lvcolumn.cx = GetStringWidth(lvcolumn.pszText)+17;
	InsertColumn(0, &lvcolumn);
	szHT.LoadString(IDS_RELOC);
	lvcolumn.pszText = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	lvcolumn.iSubItem = 1;
	lvcolumn.cx = wd-GetColumnWidth(0);
	InsertColumn(1, &lvcolumn);
	LV_ITEM	lvitem;
	int iActualItem;
	lvitem.mask = LVIF_TEXT;
	for (int i=0; i<No; i++) {
		lvitem.iSubItem =0;
		lvitem.iItem =0;
		lvitem.pszText=pInCurve[i].name;
		iActualItem = InsertItem(&lvitem);
		if (((CComboBox*)m_Edit)->FindStringExact(-1, pInCurve[i].name) == -1) continue;
		lvitem.iItem =iActualItem;
		lvitem.iSubItem = 1;
		SetItem(&lvitem);
	}
}
BOOL CInCtrl::AddInCurve(CString szCurve, CString szCurve1)
{
	CString str;
	int ItemNu=GetItemCount( );
	int i,idx=-1;
	for(i=0; i<ItemNu;i++)
	{
		str=GetItemText(i,0);
		if((!str.CompareNoCase(szCurve1))||(!str.CompareNoCase(szCurve)))
		{
			idx=i;break;
		}
	}
	if (idx == -1) 
	{	
		for(i=0; i<ItemNu;i++)
		{
			str=GetItemText(i,0);
			if((!str.CompareNoCase(szCurve1))||(!str.CompareNoCase(szCurve)))
			{
				idx=i;break;
			}
		}
		if (idx == -1)	return FALSE;
	}
	SetItemText(idx, 0, szCurve);
	SetItemText(idx, 1, szCurve1);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CConCtrl

CConCtrl::CConCtrl()
{
	m_bDragging = FALSE;
	m_pimageListDrag = NULL;
	m_iItemDragTo =-1;
	pTabPara = NULL;
}

CConCtrl::~CConCtrl()
{
}


BEGIN_MESSAGE_MAP(CConCtrl, CEditCtrl)
	//{{AFX_MSG_MAP(CConCtrl)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
	ON_NOTIFY_REFLECT(LVN_BEGINRDRAG, OnBegindrag)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConCtrl message handlers
void CConCtrl::InitCtrl(CWnd* pEdit)
{
	CEditCtrl::InitCtrl(pEdit);
	CRect rect;
	GetClientRect(&rect);
	int wd = rect.Width()-GetSystemMetrics(SM_CXVSCROLL);
	LV_COLUMN		lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;
	CString szHT;
	szHT.LoadString(IDS_PARANAME);
	lvcolumn.pszText = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	lvcolumn.iSubItem = 0;
	lvcolumn.cx = 60;
	InsertColumn(0, &lvcolumn);
	szHT.LoadString(IDS_PARAVALUE);
	lvcolumn.pszText = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	lvcolumn.iSubItem = 1;
	lvcolumn.cx = 70;
	InsertColumn(1, &lvcolumn);
	szHT.LoadString(IDS_PARADSP);
	lvcolumn.pszText = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	lvcolumn.iSubItem = 2;
	lvcolumn.cx = 600;
	InsertColumn(2, &lvcolumn);
	ColNum = 3;
}
BOOL CConCtrl::AnyKey()
{
	CEditCtrl::AnyKey();
	if (col == 1) return FALSE;
	return TRUE;
}
void CConCtrl::FillCon(CTabParaObj* pTabParaVar)
{
	pTabPara = pTabParaVar;
	DeleteAllItems();
	int iActualItem;
	CString szName, szValue, szDsp;
	CParameterEdit* pEditor = (CParameterEdit*) GetParent();
	for (int i=0; i<pTabPara->szParaNameArry.GetSize(); i++)
	{
		szName = pTabPara->szParaNameArry.GetAt(i);
		for (int j=0; j<pEditor->szParaNameArry.GetSize(); j++)
		{
			if (pEditor->szParaNameArry.GetAt(j) == szName) 
			{
				szValue = pEditor->pParaValue->GetAt(j);
				break;
			}
		}
		szDsp = pTabPara->szParaDspArry.GetAt(i);
		iActualItem = InsertItem(GetItemCount(), 
			szName);
		SetItemText(iActualItem, 1, szValue);
		SetItemText(iActualItem, 2, szDsp);
	}
	if (pTabPara->szParaNameArry.GetSize()>0) 
		SetItemState( 0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
	else 
		m_Edit->ShowWindow(SW_HIDE);
}

void CConCtrl::AddCon(CString szName, CString szValue, CPoint point)
{
	CParameterEdit* pEditor = (CParameterEdit*)GetParent();
	CString szDsp = "";
	BOOL bExist = FALSE;
	int idx;
	CTabParaObj* pTabPara1;
	POSITION pos = ((CParameterEdit*)GetParent())->m_TabList.GetHeadPosition();
	while ((pos != NULL) && (!bExist))
	{
		pTabPara1 = (CTabParaObj*)(((CParameterEdit*)GetParent())->m_TabList).GetNext(pos);
		if (pTabPara1 != NULL)
		{
			CString szName1;
			for (int i=0; i<pTabPara1->szParaNameArry.GetSize(); i++)
			{
				szName1 = pTabPara1->szParaNameArry.GetAt(i);
				if (!szName1.CompareNoCase(szName)) {
					for (int j=0; j<pEditor->szParaNameArry.GetSize(); j++)
					{
						if (pEditor->szParaNameArry.GetAt(j) == szName) 
						{
							szValue = pEditor->pParaValue->GetAt(j);
							break;
						}
					}
					bExist = TRUE;
					idx = i;
					break;
				}
			}
		}
	}
	if (bExist) {
		if ( pTabPara == pTabPara1 ) 
		{
			CString msg;
		    AfxFormatString1(msg,IDS_CONSTANT_EXIST,szName);
		    AfxMessageBox(msg,MB_OK|MB_ICONQUESTION);
			//常数列表已经有该常数
			return;
		}
		if (MessageBox(szName+"\n"+pTabPara1->szTabName+
			"参数组已经有该参数\n是否将其移到当前参数组中", "参数已存在",
			MB_YESNOCANCEL|MB_ICONQUESTION) != IDYES) return;
		szDsp = pTabPara1->szParaDspArry.GetAt(idx);
		pTabPara1->szParaNameArry.RemoveAt(idx);
		pTabPara1->szParaDspArry.RemoveAt(idx);	
	}
	int row = HitTest( point );
	LV_ITEM	lvitem;
	int iActualItem;
	lvitem.mask = LVIF_TEXT;
	lvitem.iSubItem =0;
	if (row == -1) lvitem.iItem =GetItemCount();
	else lvitem.iItem =row;
	lvitem.pszText=szName.GetBuffer(128);
	szName.ReleaseBuffer();
	iActualItem = InsertItem(&lvitem);
	lvitem.pszText=szValue.GetBuffer(128);
	szValue.ReleaseBuffer();
	lvitem.iItem =iActualItem;
	lvitem.iSubItem = 1;
	SetItem(&lvitem);
	lvitem.pszText=szDsp.GetBuffer(128);
	szDsp.ReleaseBuffer();
	lvitem.iSubItem = 2;
	SetItem(&lvitem);
	pTabPara->szParaNameArry.Add(szName);
	pTabPara->szParaDspArry.Add(szDsp);
	if (!bExist) 
	{
		pEditor->szParaNameArry.Add(szName);
		CStringArray* pParaValue1;
		pos = pEditor->m_ValueList.GetHeadPosition();
		while (pos != NULL) 
		{
			pParaValue1 = (CStringArray*)pEditor->m_ValueList.GetNext(pos);
			if (pParaValue1 != NULL) pParaValue1->Add(szValue);
		}
	}
	SetFocus();
	SetItemState( iActualItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
	pEditor->bTplModified = TRUE;
}
void CConCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint			ptItem, ptAction, ptImage;
	NM_LISTVIEW		*pnmListView = (NM_LISTVIEW *)pNMHDR;
	ASSERT(!m_bDragging);
	m_bDragging = TRUE;
	m_iItemDrag = pnmListView->iItem;
	ptAction = pnmListView->ptAction;
	GetItemPosition(m_iItemDrag, &ptItem);                 // ptItem is relative to (0,0) and not the view origin
	GetOrigin(&m_ptOrigin);
	m_pimageListDrag = CreateDragImage(m_iItemDrag, &ptImage);
	m_sizeDelta = ptAction - ptImage;                      // difference between cursor pos and image pos
	m_ptHotSpot = ptAction - ptItem + m_ptOrigin;          // calculate hotspot for the cursor
	m_pimageListDrag->DragShowNolock(TRUE);                // lock updates and show drag image
	m_pimageListDrag->SetDragCursorImage(0, m_ptHotSpot);  // define the hot spot for the new cursor image
	m_pimageListDrag->BeginDrag(0, CPoint(0, 0));
	ptAction -= m_sizeDelta;
	ClientToScreen(&ptAction);
	m_pimageListDrag->DragEnter(NULL, ptAction);
	m_pimageListDrag->DragMove(ptAction);                  // move image to overlap original icon
	SetCapture();	
	*pResult = 0;
}

void CConCtrl::MoveItem(int m_iItemDrag, int m_iItemDragTo)
{
	CString name, value, dsp;
	name = GetItemText(m_iItemDrag, 0);
	value = GetItemText(m_iItemDrag, 1);
	dsp = GetItemText(m_iItemDrag, 2);
	DeleteItem(m_iItemDrag);
	pTabPara->szParaNameArry.RemoveAt(m_iItemDrag);
	pTabPara->szParaDspArry.RemoveAt(m_iItemDrag);
	if (m_iItemDragTo>m_iItemDrag) 
	{
		if (m_iItemDragTo == 9999) m_iItemDragTo=GetItemCount();
	}
	int item;
	item = InsertItem(m_iItemDragTo, name);
	SetItemText(item, 1, value);
	SetItemText(item, 2, dsp);
	pTabPara->szParaNameArry.InsertAt(item, name);
	pTabPara->szParaDspArry.InsertAt(item, dsp);
	SetFocus();
	SetItemState( item, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
	((CParameterEdit*)GetParent())->bTplModified = TRUE;
}
void CConCtrl::DelItem(int m_iItemDrag)
{
	CParameterEdit* pEditor = (CParameterEdit*)GetParent();
	pEditor->bTplModified = TRUE;
	pEditor->bParaModified = TRUE;
	DeleteItem(m_iItemDrag);
	CString szName = pTabPara->szParaNameArry.GetAt(m_iItemDrag);
	for (int j=0; j<pEditor->szParaNameArry.GetSize(); j++)
	{
		if (pEditor->szParaNameArry.GetAt(j) == szName) 
		{
			pEditor->szParaNameArry.RemoveAt(j);
			break;
		}
	}
	CStringArray* pParaValue1;
	POSITION pos = pEditor->m_ValueList.GetHeadPosition();
	while (pos != NULL) 
	{
		pParaValue1 = (CStringArray*)pEditor->m_ValueList.GetNext(pos);
		if (pParaValue1 != NULL) pParaValue1->RemoveAt(j);
	}
	pTabPara->szParaNameArry.RemoveAt(m_iItemDrag);
	pTabPara->szParaDspArry.RemoveAt(m_iItemDrag);
	if (m_iItemDrag >(GetItemCount()-1)) 
		m_iItemDrag = GetItemCount()-1;
	SetFocus();
	SetItemState( m_iItemDrag, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
}
void CConCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CListCtrl::OnLButtonUp(nFlags, point);
	if (m_bDragging)                                       // end of the drag operation
	{
		m_bDragging = FALSE;
		m_pimageListDrag->DragLeave(NULL);
		m_pimageListDrag->EndDrag();
		::ReleaseCapture();
		CRect rect;
		GetClientRect(&rect);
		if (!rect.PtInRect(point))
		{
			CString msg;
			AfxFormatString1(msg,IDS_DEL_PARCONSTANT,GetItemText(m_iItemDrag,0));
			if (AfxMessageBox(msg,MB_YESNO|MB_ICONQUESTION) == IDYES)
			//从参数数列表中删除该参数 
			DelItem(m_iItemDrag);
		}
		else 
		{
			if ((m_iItemDragTo != -1) && (m_iItemDragTo != m_iItemDrag)) 
				MoveItem(m_iItemDrag, m_iItemDragTo);
		}
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	}
}

void CConCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	CListCtrl::OnMouseMove(nFlags, point);
	if (m_bDragging)
	{
		CPoint ptScr = point;
		ClientToScreen(&ptScr);
		m_pimageListDrag->DragMove(ptScr- m_sizeDelta);  // move the image
		CRect rect;
		GetClientRect(&rect);
		if (!rect.PtInRect(point))
			::SetCursor(AfxGetApp()->LoadCursor(IDC_DELICON));
		else 
		{
			CRect rect1;
			GetItemRect(GetItemCount()-1,&rect1,LVIR_BOUNDS);
			rect.left = GetColumnWidth(0);
			rect.bottom = rect1.bottom;
			if (rect.PtInRect(point)) m_iItemDragTo =-1;
			else {
				point.x =0;
				m_iItemDragTo = HitTest( point );
				if ((m_iItemDragTo == -1) && (m_iItemDrag !=(GetItemCount()-1))) 
					m_iItemDragTo = 9999;
			}
			if (m_iItemDragTo == -1)
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			else if (m_iItemDragTo<m_iItemDrag)
				::SetCursor(AfxGetApp()->LoadCursor(IDC_MOVEUPICON));
			else if (m_iItemDragTo>m_iItemDrag)
				::SetCursor(AfxGetApp()->LoadCursor(IDC_MOVEDOWNICON));
			else
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		}
	}
	CParameterEdit* pEditor = (CParameterEdit*)GetParent();
	if (CListCtrl::HitTest(point) != -1)
	{	
		CString szHT;
		szHT.LoadString(IDS_DRAGDEL);
		if (szHT.GetAt(0)=='Y')
			pEditor->m_ToolTip.UpdateTipText("You can drag a parameter out of the list to delete it", this);
		else 
			pEditor->m_ToolTip.UpdateTipText("将参数拖拉到列表框外删除该参数", this);
	}
	else
		pEditor->m_ToolTip.UpdateTipText("", this);

}

/////////////////////////////////////////////////////////////////////////////
// COutCtrl

COutCtrl::COutCtrl()
{
}

COutCtrl::~COutCtrl()
{
}


BEGIN_MESSAGE_MAP(COutCtrl, CEditCtrl)
	//{{AFX_MSG_MAP(COutCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutCtrl message handlers
void COutCtrl::InitCtrl(int No, OUTCURVE* pOutCurve, CWnd* pEdit)
{
	CRect rect;
	GetClientRect(&rect);
	int wd = rect.Width()-GetSystemMetrics(SM_CXVSCROLL);
	LV_COLUMN		lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;
	CString szHT;
	szHT.LoadString(IDS_OUTCURVE);
	lvcolumn.pszText = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	lvcolumn.iSubItem = 0;
	lvcolumn.cx = GetStringWidth(lvcolumn.pszText)+17;
	InsertColumn(0, &lvcolumn);
	szHT.LoadString(IDS_RELOC);
	lvcolumn.pszText = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	lvcolumn.iSubItem = 1;
	lvcolumn.cx = wd-GetColumnWidth(0);
	InsertColumn(1, &lvcolumn);
	LV_ITEM	lvitem;
	int iActualItem;
	lvitem.mask = LVIF_TEXT;
	for (int i=0; i<No; i++) 
	{
		lvitem.iSubItem =0;
		lvitem.iItem =0;
		lvitem.pszText=pOutCurve[i].name;
		iActualItem = InsertItem(&lvitem);
		lvitem.iItem =iActualItem;
		lvitem.iSubItem = 1;
		SetItem(&lvitem);
	}
	m_Edit= pEdit;	
}
BOOL COutCtrl::AddOutCurve(CString szCurve, CString szCurve1)
{
	CString str;
	int ItemNu=GetItemCount( );
	int i,idx=-1;
	for(i=0; i<ItemNu;i++)
	{
		str=GetItemText(i,0);
		if((!str.CompareNoCase(szCurve1))||(!str.CompareNoCase(szCurve)))
		{
			idx=i;break;
		}
	}
	if (idx == -1) 
	{	
		for(i=0; i<ItemNu;i++)
		{
			str=GetItemText(i,0);
			if((!str.CompareNoCase(szCurve1))||(!str.CompareNoCase(szCurve)))
			{
				idx=i;break;
			}
		}
		if (idx == -1)	return FALSE;
	}
	SetItemText(idx, 0, szCurve);
	SetItemText(idx, 1, szCurve1);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CRuler
#define	RulerH 10

CRuler::CRuler()
{
	m_SelZoon = 0;
	m_ZoomNum = 1;
	m_pSelItem = NULL;
	LOGFONT m_LogFont=
	{
		-10,					    //lfHeight
		0,							//lfWidth
		0,							//lfEscapement
		0,							//lfOrientation
		FW_LIGHT,					//lfWeight
		0,							//lfItalic
		0,							//lfUnderLine;
		0,							//lfStrikeOut;
		ANSI_CHARSET,				//lfCharSet
		OUT_TT_ONLY_PRECIS,			//lfOutPrecision
		CLIP_DEFAULT_PRECIS,		//lfClipPrecision
		DEFAULT_QUALITY,			//lfQuality
		VARIABLE_PITCH | FF_SWISS,	//lfPitchAndFamily
		"Times New Roman"			//lfFaceName
	};
	m_DepthFont=m_LogFont;
}

CRuler::~CRuler()
{
	CRulerItem* pObj;
	POSITION pos = m_ObList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CRulerItem*)m_ObList.GetNext(pos);
		if (pObj != NULL ) delete pObj;
	} 
}


BEGIN_MESSAGE_MAP(CRuler, CStatic)
	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(CRuler)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_DEPTH_CHANGE, OnDepthChange)
	ON_COMMAND(ID_DEPTH_DELETE, OnDepthDelete)
	ON_COMMAND(ID_DEPTH_SPLIT, OnDepthSplit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRuler message handlers

void CRuler::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rect, rect1; 
	GetClientRect(&rect);
	CBrush m_Brush(RGB(212,208,200));
	dc.FillRect( rect, &m_Brush);
	dc.Rectangle(rect.left+4, CenterH-RulerH, 
				 rect.right-4, CenterH+RulerH);
	CRulerItem* pObj = NULL;
	CRulerItem* pPrevObj = NULL;
	BOOL bTop = FALSE;
	BOOL bDrawFocus = FALSE;
	int item = -1;
	int SliderPos= DepthToPix((double)(pDepSlider->GetPos()/1000.0)) - m_nMin;
	POSITION pos = m_ObList.GetHeadPosition();
	while (pos != NULL)
	{
		pPrevObj = pObj;
		pObj = (CRulerItem*)m_ObList.GetNext(pos);
		if (pObj != NULL) 
		{
			item += 1;
			bTop = !bTop;
			if (pObj->m_nPos.x-SliderPos < rect.left+4 ) continue;
			if (pObj->m_nPos.x-SliderPos > rect.right-4 ) break;
			pObj->Draw(dc, bTop, SliderPos);
			if ((pObj->GetProperties() == AttTop) && ((pObj->GetHorzPosPix()-SliderPos)>rect.left+5))
			{
				rect1 = CRect(rect.left+5,CenterH-RulerH+2, 
							  pObj->GetHorzPosPix()- SliderPos-2,CenterH+RulerH-2);
				if (rect1.right>rect.right-5)	rect1.right=rect.right-5;
				dc.FillRect( rect1, &m_Brush);
			}
			else
			if ((pObj->GetProperties() == AttBtm) && ((pObj->GetHorzPosPix()-SliderPos)<rect.right-5))
			{
				rect1 = CRect(pObj->GetHorzPosPix()-SliderPos+1,CenterH-RulerH+2, 
							  rect.right-5,CenterH+RulerH-2);
				if (rect1.left<rect.left+5) rect1.left=rect.left+5;
				dc.FillRect( rect1, &m_Brush);
			}
			if ((m_SelZoon == item-1 ) && (pPrevObj != NULL) && (!bDrawFocus))
			{
				rect1 = CRect(pPrevObj->GetHorzPosPix()-SliderPos+2,CenterH-RulerH+2, 
							  pObj->GetHorzPosPix()-SliderPos-2,CenterH+RulerH-2);
				if (rect1.left<rect.left+5) rect1.left=rect.left+5;
				if (rect1.right>rect.right-5)	rect1.right=rect.right-5;
				dc.FillSolidRect( rect1, RGB(255, 0, 0));
				bDrawFocus = TRUE;
			}
			if ((m_SelZoon == item) && (!bDrawFocus))
			{
				CRulerItem* pNextObj = NULL;
				pNextObj = (CRulerItem*)m_ObList.GetNext(pos);
				if (pos == NULL) pos = m_ObList.GetTailPosition();
				else m_ObList.GetPrev(pos);
				if (pNextObj != NULL)
				{
					rect1 = CRect(pObj->GetHorzPosPix()-SliderPos+3,CenterH-RulerH+2, 
							  pNextObj->GetHorzPosPix()-SliderPos-3,CenterH+RulerH-2);
					if (rect1.left<rect.left+5) rect1.left=rect.left+5;
					if (rect1.right>rect.right-5)	rect1.right=rect.right-5;
					dc.FillSolidRect( rect1, RGB(0, 128, 128));
					bDrawFocus = TRUE;
				}
			}

		}
	} 
	// Do not call CStatic::OnPaint() for painting messages
}
CRulerItem* CRuler::HitTestPix(CPoint point)
{
	CRulerItem* pObj;
	POSITION pos = m_ObList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CRulerItem*)m_ObList.GetNext(pos);
		if (pObj->HitTestPix(point)) return pObj;
	} 
	return NULL;
}
void CRuler::SetMinMax(double SDep, double EDep, double Rlev, CSliderCtrl* m_pDepSlider)
{
	CRect rect;										
	GetClientRect(&rect);
	CenterH = (rect.bottom+rect.top)/2; 
	pDepSlider = m_pDepSlider;
	m_EDep = EDep;
	m_SDep = SDep;
	m_Rlev = Rlev;
	RecalcRuler();
}
void CRuler::RecalcRuler() 
{
	CRect rect;										
	GetClientRect(&rect);
	m_nMin = 4;
	m_nMax = m_ZoomNum*(rect.right-8) + m_nMin;
	AA =  (m_nMax-m_nMin)/(m_EDep-m_SDep);
	BB =  m_nMin - m_SDep*AA;
	AA1 =  1.0/AA;
	BB1 =  m_SDep - m_nMin*AA1;
	CRulerItem* pObj;
	POSITION pos = m_ObList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CRulerItem*)m_ObList.GetNext(pos);
		if (pObj != NULL )
		{
			pObj->m_nPos.x = DepthToPix(pObj->Depth);
		}
	}
	int nMin =(int)(m_SDep*1000);
	int nMax =(int)(m_EDep*1000);
	int nPos = nMin;
	if (m_ZoomNum >1) nPos = pDepSlider->GetPos();
	pDepSlider->SetRange(nMin, nMin+(nMax-nMin)/m_ZoomNum*(m_ZoomNum-1), TRUE);
	pDepSlider->SetLineSize(1);			
	pDepSlider->SetPageSize((nMax-nMin) / ((m_ZoomNum+1)*10));	
	pDepSlider->SetPos(nPos);	
	Invalidate();
}

double CRuler::PixToDepth(int nPos) 
{
	double dep = BB1 + nPos*AA1;
	int ss = (int)(dep*1000+0.5);
	int rr = (int) (m_Rlev*1000+0.5);
	ss = ss/rr;
	ss = ss*rr;
	dep = ((double) ss)/1000.0;
	return dep;
}
int CRuler::DepthToPix(double dep) 
{
	int x = (int)(BB + dep*AA+0.5);
	if (x < m_nMin) x = m_nMin;
	if (x > m_nMax) x = m_nMax;
	return x;
}											 
void CRuler::PtDpToLp(LPPOINT point)
{
	double PosDep = (double)(pDepSlider->GetPos()/1000.0);
	point->x += DepthToPix(PosDep) - m_nMin;
}
void CRuler::AddTBRulerItem() 
{
	CPoint point = CPoint(m_nMin, CenterH);
	CRulerItem* m_pSelItem = new CRulerItem(IDB_RULER, point, m_SDep, this);
	m_pSelItem->SetProperties(AttTop);
	m_ObList.AddTail((CObject*)m_pSelItem);
	point = CPoint(m_nMax, CenterH);
	CRulerItem* m_pSelItem1 = new CRulerItem(IDB_RULER, point, m_EDep, this);
	m_pSelItem1->SetProperties(AttBtm);
	m_ObList.AddTail((CObject*)m_pSelItem1);
}
void CRuler::AddRulerItem(double Dep, int Att) 
{
	int x = DepthToPix(Dep);
	CPoint point = CPoint(x, CenterH);
	CRulerItem* m_pSelItem = new CRulerItem(IDB_RULER, point, Dep, this);
	m_pSelItem->SetProperties(Att);
	m_ObList.AddTail((CObject*)m_pSelItem);
}
void CRuler::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CStatic::OnLButtonDown(nFlags, point);
	CPoint pt = point;
	PtDpToLp(&point);
	int Zoon = -1;
	m_pSelItem = NULL;
	CRulerItem* pObj = NULL;
	POSITION pos = m_ObList.GetHeadPosition();
	while (pos != NULL)
	{
		m_pPrevItem = pObj;
		pObj = (CRulerItem*)m_ObList.GetNext(pos);
		if (point.x>=pObj->GetHorzPosPix())
			Zoon +=1;
		if (pObj->HitTestPix(point)) 
		{
			m_pSelItem = pObj;
			if (pos != NULL) 
				m_pNextItem = (CRulerItem*)m_ObList.GetNext(pos);
			else
				m_pNextItem = NULL;
			break;
		}
	}
	if (m_pSelItem == NULL)	
	{
		CRect rect;
		GetClientRect(&rect);
		rect = CRect(rect.left+4, CenterH-RulerH, 
					 rect.right-4, CenterH+RulerH);
		if (!rect.PtInRect(pt)) return; 
		if ((Zoon>=0) && (Zoon<m_ObList.GetCount()-1))
		{
			m_SelZoon = Zoon;
			CParameterEdit* pEditor =(CParameterEdit*) GetParent();
			pEditor->pParaValue = (CStringArray*)(pEditor->m_ValueList).GetAt(pEditor->m_ValueList.FindIndex(m_SelZoon));
			int item = pEditor->m_ConTab.GetCurSel( );
			if (item != -1) 
			{
				CTabParaObj* pTabPara = (CTabParaObj*)(pEditor->m_TabList).GetAt(pEditor->m_TabList.FindIndex(item));
				pEditor->m_ConCtrl.FillCon(pTabPara);
			}
			Invalidate(NULL);
		}
		return;
	}
	SetCapture();
	m_pSelItem->Invalidate();
	::SetCursor(AfxGetApp()->LoadCursor(IDC_MOVE));
	((CParameterEdit*)GetParent())->bParaModified = TRUE;
}

void CRuler::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (::GetCapture() != m_hWnd) return;
	ReleaseCapture();
	m_pSelItem = NULL;
	CStatic::OnLButtonUp(nFlags, point);
}

void CRuler::OnMouseMove(UINT nFlags, CPoint point) 
{
	CStatic::OnMouseMove(nFlags, point);
	CPoint pt;
	PtDpToLp(&point);
	CParameterEdit* pEditor = (CParameterEdit*)GetParent();
	if (HitTestPix(point) != NULL) 
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_MOVE));
		if (nFlags != MK_LBUTTON) 
		{
			CString szHT;
			szHT.LoadString(IDS_RDEPTH);
			if (szHT.GetAt(0)=='P')
				pEditor->m_ToolTip.UpdateTipText("Press right button to modify depth", this);
			else 
				pEditor->m_ToolTip.UpdateTipText("使用鼠标右键修改处理井段", this);
		}
	}
	else 
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		CRect rect;
		GetClientRect(&rect);
		rect = CRect(rect.left+4, CenterH-RulerH, 
					 rect.right-4, CenterH+RulerH);
		if ((rect.PtInRect(point)) && (nFlags != MK_LBUTTON))
		{
			CString szHT;
			szHT.LoadString(IDS_LDEPTH);
			if (szHT.GetAt(0)=='P')
				pEditor->m_ToolTip.UpdateTipText("Press left button select depth zoon, right buton modify depth", this);
			else
				pEditor->m_ToolTip.UpdateTipText("使用鼠标左键选择井段,右键删除或分段", this);

		}
		else
			pEditor->m_ToolTip.UpdateTipText("", this);
	}
	if (::GetCapture() != m_hWnd) return;
	if (m_pSelItem == NULL) return;
	double dep = PixToDepth(point.x);
	if (m_pNextItem != NULL) {
		if (dep>= m_pNextItem->Depth) return;
	}
	if (m_pPrevItem != NULL) {
		if (dep<= m_pPrevItem->Depth) return;
	}
	int SliderPos= DepthToPix((double)(pDepSlider->GetPos()/1000.0)) - m_nMin;
	CRect rc = m_pSelItem->GetHitRectPix();
	rc.OffsetRect(-SliderPos, 0);
	CClientDC dc(this); // device context for painting
	CRect rc1 = m_pSelItem->GetDepRect(dc);
	rc1.OffsetRect(-SliderPos, 0);
	m_pSelItem->TrackHorzPosPix(point.x);
	InvalidateRect(rc1);
	rc1 = m_pSelItem->GetHitRectPix();
	rc1.OffsetRect(-SliderPos, 0);
	if (rc.left>rc1.left) rc.left = rc1.left;
	if (rc.right<rc1.right) rc.right = rc1.right;
	InvalidateRect(rc);
}

void CRuler::ContextMenu(CPoint point, int nMenu)
{
	// CG: This function was added by the Pop-up Menu component

	CMenu menu;
	VERIFY(menu.LoadMenu(nMenu));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	menu.EnableMenuItem( ID_DEPTH_SPLIT, bSplitDep );
	menu.EnableMenuItem( ID_DEPTH_CHANGE, bChangeDep );
	menu.EnableMenuItem( ID_DEPTH_DELETE, bDelDep );

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		this);
}
void CRuler::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CPoint pt = point;
	CStatic::OnRButtonDown(nFlags, point);
	PtDpToLp(&point);
	bSplitDep = MF_GRAYED;
	bDelDep = MF_GRAYED;
	bChangeDep = MF_GRAYED;
	m_pPrevItem = NULL;
	m_pNextItem = NULL;
	m_pRItem = NULL;
	int Zoon = -1;
	CRulerItem* pObj = NULL;
	POSITION pos = m_ObList.GetHeadPosition();
	while (pos != NULL)
	{
		m_pPrevItem = pObj;
		pObj = (CRulerItem*)m_ObList.GetNext(pos);
		if (pObj->HitTestPix(point)) 
		{
			bChangeDep = MF_ENABLED;
			m_pRItem = pObj;
			if (pos != NULL) 
				m_pNextItem = (CRulerItem*)m_ObList.GetNext(pos);
			else
				m_pNextItem = NULL;
			break;
		}
		m_pNextItem = pObj;
		if (point.x < pObj->GetHorzPosPix()) break;
		if (point.x>=pObj->GetHorzPosPix())
			Zoon +=1;
		if (pObj->GetProperties() == AttBtm) return;
	}
	if ( m_pRItem== NULL)
	{
		CRect rect;
		GetClientRect(&rect);
		rect = CRect(rect.left+4, CenterH-RulerH, 
					 rect.right-4, CenterH+RulerH);
		if (!rect.PtInRect(pt)) return;
		if ((Zoon<0) || (Zoon>=m_ObList.GetCount())) return;
		bSplitDep = MF_ENABLED;
		if (m_ObList.GetCount()>2) bDelDep = MF_ENABLED;
		m_SelZoon = Zoon;
		CParameterEdit* pEditor =(CParameterEdit*) GetParent();
		pEditor->pParaValue = (CStringArray*)(pEditor->m_ValueList).GetAt(pEditor->m_ValueList.FindIndex(m_SelZoon));
		int item = pEditor->m_ConTab.GetCurSel( );
		if (item != -1) 
		{
			CTabParaObj* pTabPara = (CTabParaObj*)(pEditor->m_TabList).GetAt(pEditor->m_TabList.FindIndex(item));
			pEditor->m_ConCtrl.FillCon(pTabPara);
		}
		Invalidate(NULL);
	}
	ClientToScreen(&pt);
	ContextMenu(pt, CG_IDR_POPUP_RULER);
}
void CRuler::OnDepthSplit() 
{
	CPoint point = CPoint((m_pPrevItem->m_nPos.x + m_pNextItem->m_nPos.x)/2,CenterH); 
	double dep = (m_pPrevItem->Depth+m_pNextItem->Depth)/2;
	CRect rect;
	GetClientRect(&rect);
	rect = CRect(rect.left+4, CenterH-RulerH, 
			 rect.right-4, CenterH+RulerH);
	int SliderPos= DepthToPix((double)(pDepSlider->GetPos()/1000.0)) - m_nMin;
	CPoint pt = point;
	pt.x -= SliderPos;
	if (!rect.PtInRect(pt))
	{
		if (pt.x<rect.left) pt.x = rect.left; 
		if (pt.x>rect.right) pt.x = rect.right;
		point.x = pt.x + SliderPos;
	}
	CRulerItem* pObj = new CRulerItem(IDB_RULER, point, dep, this);
	m_ObList.InsertAfter(m_ObList.Find((CObject*)m_pPrevItem), (CObject*)pObj);
	CParameterEdit* pEditor =(CParameterEdit*) GetParent();
	CStringArray* pParaValue1 = new CStringArray;
	for (int k=0; k<pEditor->pParaValue->GetSize(); k++)
		pParaValue1->Add(pEditor->pParaValue->GetAt(k));
	pEditor->m_ValueList.InsertAfter(pEditor->m_ValueList.Find((CObject*)pEditor->pParaValue), pParaValue1);
	pEditor->pParaValue = pParaValue1;
	Invalidate(NULL);
}

void CRuler::OnDepthChange() 
{
	CDepthDlg dlg;
	dlg.m_Dep = m_pRItem->Depth;
	if (m_pPrevItem != NULL)
		dlg.m_SDep = m_pPrevItem->Depth;
	else
		dlg.m_SDep = m_SDep;
	if (m_pNextItem != NULL)
		dlg.m_EDep = m_pNextItem->Depth;
	else
		dlg.m_EDep = m_EDep;
	if (dlg.DoModal() == IDOK)
	{
		m_pRItem->Depth = dlg.m_Dep;
		int x = DepthToPix(m_pRItem->Depth);
		int SliderPos= DepthToPix((double)(pDepSlider->GetPos()/1000.0)) - m_nMin;
		CRect rc = m_pRItem->GetHitRectPix();
		rc.OffsetRect(-SliderPos, 0);
		CClientDC dc(this); // device context for painting
		CRect rc1 = m_pRItem->GetDepRect(dc);
		rc1.OffsetRect(-SliderPos, 0);
		m_pRItem->m_nPos.x = x;
		InvalidateRect(rc1);
		rc1 = m_pRItem->GetHitRectPix();
		rc1.OffsetRect(-SliderPos, 0);
		if (rc.left>rc1.left) rc.left = rc1.left;
		if (rc.right<rc1.right) rc.right = rc1.right;
		InvalidateRect(rc);
		rc1 = m_pRItem->GetDepRect(dc);
		rc1.OffsetRect(-SliderPos, 0);
		InvalidateRect(rc1);
	}
}
void CRuler::OnDepthDelete() 
{
	CRulerItem* pObj;
	if (m_SelZoon !=0 ) pObj = m_pPrevItem;
	else pObj = m_pNextItem;
	char szDepth[20];
	sprintf(szDepth,"%8.3f-%8.3f",m_pPrevItem->Depth, m_pNextItem->Depth);
	CString szMsg;
	strcpy(szMsg.GetBuffer(128), szDepth);
	szMsg.ReleaseBuffer();
	if (AfxMessageBox(IDS_CANCEL_DEPTH, MB_ICONQUESTION|MB_YESNOCANCEL) != IDYES) return;
	//取消该深度段吗？
	CParameterEdit* pEditor =(CParameterEdit*) GetParent();
	m_ObList.RemoveAt(m_ObList.Find((CObject*)pObj));
	pEditor->m_ValueList.RemoveAt(pEditor->m_ValueList.Find((CObject*)pEditor->pParaValue));
	if (m_SelZoon >0 ) m_SelZoon--;
	pEditor->pParaValue = (CStringArray*)(pEditor->m_ValueList).GetAt(pEditor->m_ValueList.FindIndex(m_SelZoon));
	int item = pEditor->m_ConTab.GetCurSel( );
	if (item != -1) 
	{
		CTabParaObj* pTabPara = (CTabParaObj*)(pEditor->m_TabList).GetAt(pEditor->m_TabList.FindIndex(item));
		pEditor->m_ConCtrl.FillCon(pTabPara);
	}
	Invalidate(NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CRulerItem
CRulerItem::CRulerItem(UINT nBitmapID, CPoint pt, double dep, CRuler* pRuler)
{
	m_nPos = pt;
	Depth = dep;
	m_pRuler = pRuler;
	m_hbm = NULL;
	m_hbmMask = NULL;
	nAtt = AttNor;
	if (nBitmapID != 0)
	{
		m_hbmMask = ::LoadBitmap(
			AfxFindResourceHandle(MAKEINTRESOURCE(nBitmapID+1), RT_BITMAP), 
			MAKEINTRESOURCE(nBitmapID+1));
		ASSERT(m_hbmMask != NULL);
		VERIFY(LoadMaskedBitmap(MAKEINTRESOURCE(nBitmapID)));
		BITMAP bm;
		::GetObject(m_hbm, sizeof(BITMAP), &bm);
		m_size = CSize(bm.bmWidth, bm.bmHeight);
	}
}

CRulerItem::~CRulerItem()
{
	if (m_hbm != NULL)
		::DeleteObject(m_hbm);
	if (m_hbmMask != NULL)
		::DeleteObject(m_hbmMask);
}
BOOL CRulerItem::LoadMaskedBitmap(LPCTSTR lpszResourceName)
{
	ASSERT(lpszResourceName != NULL);

	if (m_hbm != NULL)
		::DeleteObject(m_hbm);

	HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName, RT_BITMAP);
	HRSRC hRsrc = ::FindResource(hInst, lpszResourceName, RT_BITMAP);
	if (hRsrc == NULL)
		return FALSE;

	m_hbm = AfxLoadSysColorBitmap(hInst, hRsrc);
	return (m_hbm != NULL);
}

void CRulerItem::SetHorzPosPix(int nXPos)
{ 
	m_nPos.x = nXPos;
	Depth = (double)m_pRuler->PixToDepth(m_nPos.x);
}

void CRulerItem::TrackHorzPosPix(int nXPos)
{
	int nMin = m_pRuler->m_nMin;
	int nMax = m_pRuler->m_nMax;
	if (nXPos < nMin)
		nXPos = nMin;
	if (nXPos > nMax)
		nXPos = nMax;
	SetHorzPosPix(nXPos);
}

void CRulerItem::DrawFocusLine()
{
	if (m_nPos.x != 0)
	{
/*		m_rcTrack.left = m_rcTrack.right = GetHorzPosPix();
		ASSERT(m_pDC != NULL);
		int nLeft =0;
		m_pRuler->XRulerToClient(m_rcTrack.left);
		m_pDC->MoveTo(nLeft, m_rcTrack.top);
		m_pDC->LineTo(nLeft, m_rcTrack.bottom);
*/
	}
}


void CRulerItem::Invalidate()
{
	CRect rc = GetHitRectPix();
	m_pRuler->InvalidateRect(rc);
}

CRect CRulerItem::GetHitRectPix()
{
	int nx = m_nPos.x- m_size.cx/2-1;
	if (nx<0) nx =0;
	return CRect(CPoint(nx, m_nPos.y- m_size.cy/2), m_size);
}

void CRulerItem::Draw(CDC& dc, BOOL bTop, int SliderPos)
{
	bDepTop = bTop;
	CDC dcBitmap;
	dcBitmap.CreateCompatibleDC(&dc);

	HGDIOBJ hbm = ::SelectObject(dcBitmap.m_hDC, m_hbmMask);
	int nx = m_nPos.x - SliderPos- m_size.cx/2-1;
	if (nx<0) nx =0;
	// do mask part
	dc.BitBlt(nx, m_nPos.y- m_size.cy/2, m_size.cx, m_size.cy, &dcBitmap, 0, 0, SRCAND);
	
	// do image part
	::SelectObject(dcBitmap.m_hDC, m_hbm);

	dc.BitBlt(nx, m_nPos.y- m_size.cy/2, m_size.cx, m_size.cy, &dcBitmap, 0, 0, SRCINVERT);

	::SelectObject(dcBitmap.m_hDC, hbm);
    CFont m_Font;
	m_Font.CreateFontIndirect(&m_pRuler->m_DepthFont);
   	CFont* pOldFont = dc.SelectObject(&m_Font);
	int iy, y;
	if (bTop) 
	{
		iy = TA_BOTTOM;
		y = m_nPos.y - m_size.cy/2;
	}
	else
	{
		iy = TA_TOP;
		y = m_nPos.y + m_size.cy/2;
	}
	dc.SetTextAlign(TA_LEFT | iy);
	char szDepth[20];
	dtos(Depth, szDepth);
	dc.SetBkMode(TRANSPARENT);
	CSize size = dc.GetTextExtent( szDepth );
	int x = m_nPos.x - SliderPos - size.cx /2;
	CRect rect;										
	m_pRuler->GetClientRect(&rect);
	rect.left += 4;
	rect.right-= 4;
	if (x<rect.left) x = rect.left;
	if ((x+size.cx)>rect.right) x = rect.right-size.cx;
    dc.TextOut(x, y,szDepth, strlen(szDepth));
	dc.SelectObject(pOldFont);
}
CRect CRulerItem::GetDepRect(CDC& dc)
{
	CRect rect;
	char szDepth[20];
	dtos(Depth, szDepth);
	CSize size = dc.GetTextExtent( szDepth );
	int x = m_nPos.x - size.cx /2;
	if (x<m_pRuler->m_nMin) x = m_pRuler->m_nMin;
	if ((x+size.cx)>m_pRuler->m_nMax) x = m_pRuler->m_nMax-size.cx;
	if (bDepTop) 
		rect = CRect(x-3, m_nPos.y - m_size.cy/2-size.cy, x + size.cx+3, m_nPos.y - m_size.cy/2);
	else
		rect = CRect(x-3, m_nPos.y + m_size.cy/2, x + size.cx+3, m_nPos.y + m_size.cy/2 + size.cy);
	return rect;
}
/////////////////////////////////////////////////////////////////////////////
// CTabNameDlg dialog


CTabNameDlg::CTabNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTabNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTabNameDlg)
	m_TabName = _T("");
	//}}AFX_DATA_INIT
}


void CTabNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabNameDlg)
	DDX_Text(pDX, IDC_TABNAME, m_TabName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabNameDlg, CDialog)
	//{{AFX_MSG_MAP(CTabNameDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabNameDlg message handlers




/////////////////////////////////////////////////////////////////////////////
// CDCEdit

CDCEdit::CDCEdit()
{
}

CDCEdit::~CDCEdit()
{
}


BEGIN_MESSAGE_MAP(CDCEdit, CEdit)
	//{{AFX_MSG_MAP(CDCEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDCEdit message handlers

void CDCEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CEditCtrl* pCtrl = (CEditCtrl*)GetParent();
	if (pCtrl == NULL) return;
	if (pCtrl->AnyKey()) 
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	else 
		if ((nChar >='0' && nChar <='9')|| (nChar=='.')||(nChar=='-')||(nChar==8))
			CEdit::OnChar(nChar, nRepCnt, nFlags);
	
}

/////////////////////////////////////////////////////////////////////////////
// CDepthDlg dialog


CDepthDlg::CDepthDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDepthDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDepthDlg)
	m_Dep = 0.0;
	m_EDep = 0.0;
	m_SDep = 0.0;
	//}}AFX_DATA_INIT
}


void CDepthDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDepthDlg)
	DDX_Text(pDX, IDC_DEP, m_Dep);
	DDX_Text(pDX, IDC_EDEP, m_EDep);
	DDX_Text(pDX, IDC_SDEP, m_SDep);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDepthDlg, CDialog)
	//{{AFX_MSG_MAP(CDepthDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDepthDlg message handlers

void CDepthDlg::OnOK() 
{
	UpdateData(TRUE);
	if ((m_Dep<=m_SDep) || (m_Dep>=m_EDep))
	{
		AfxMessageBox(IDS_WRONG_DEPTH, MB_ICONSTOP|MB_OK);
		return;
	}
	CDialog::OnOK();
}


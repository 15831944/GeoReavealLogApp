// DlisCurDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DlisCurDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlisCurDlg dialog


CDlisCurDlg::CDlisCurDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDlisCurDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlisCurDlg)
	m_nMaxLog = 0;
	m_NumLog = 0;
	m_sCurveStatus = _T("");
	//}}AFX_DATA_INIT
}


void CDlisCurDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlisCurDlg)
	DDX_Control(pDX, IDC_DLISCURVES_LIST, m_ctlDlisCurves);
	DDX_Text(pDX, IDC_MAXLOGNUM_EDIT, m_nMaxLog);
	DDX_Text(pDX, IDC_NUMLOG_EDIT, m_NumLog);
	DDX_Text(pDX, IDC_CURVES_STATUS_EDIT, m_sCurveStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlisCurDlg, CDialog)
	//{{AFX_MSG_MAP(CDlisCurDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_DLISCURVES_LIST, OnDblclkDliscurvesList)
	ON_NOTIFY(NM_RCLICK, IDC_DLISCURVES_LIST, OnRclickDliscurvesList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_DLISCURVES_LIST, OnEndlabeleditDliscurvesList)
	ON_COMMAND(ID_CURVENAME_ADDFRAMENO, OnCurvenameAddframeno)
	ON_COMMAND(ID_CURVENAME_ADDCOPYNUM, OnCurvenameAddcopynum)
	ON_COMMAND(ID_CURVENAME_ADDORIGIN, OnCurvenameAddorigin)
	ON_COMMAND(ID_CURVENAME_DELBAD, OnCurvenameDelbad)
	ON_COMMAND(ID_CURVENAME_DELSAME, OnCurvenameDelsame)
	ON_COMMAND(ID_CURVENAME_DELTOOLEN, OnCurvenameDeltoolen)
	ON_COMMAND(ID_CURVENAME_DELSELECT, OnCurvenameDelselect)
	ON_COMMAND(ID_CURVENAME_CLEARERROR, OnCurvenameClearerror)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlisCurDlg message handlers

BOOL CDlisCurDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	CString s;
	//����CListctrlΪ������ʽ
	DWORD dwStyle=GetWindowLong(m_ctlDlisCurves.m_hWnd,GWL_STYLE);
	SetWindowLong(m_ctlDlisCurves.m_hWnd,GWL_STYLE,dwStyle|LVS_REPORT);
	//����CListctrl��������ѡ�����������
	DWORD styles=m_ctlDlisCurves.GetExtendedStyle();
	m_ctlDlisCurves.SetExtendedStyle(styles|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

//	s.Format ("������(��%d�ֽ�)",m_nMaxLen);
	m_ctlDlisCurves.InsertColumn(0,"������",LVCFMT_LEFT,105,0);
	m_ctlDlisCurves.InsertColumn(1,"S",LVCFMT_CENTER,25,0);
	m_ctlDlisCurves.InsertColumn(2,"����������",LVCFMT_LEFT,80,0);
	m_ctlDlisCurves.InsertColumn(3,"O",LVCFMT_CENTER,25,0);
	m_ctlDlisCurves.InsertColumn(4,"C",LVCFMT_CENTER,18,0);
	m_ctlDlisCurves.InsertColumn(5,"��ֹ����",LVCFMT_CENTER,90,0);
	m_ctlDlisCurves.InsertColumn(6,"�������",LVCFMT_CENTER,60,0);
	m_ctlDlisCurves.InsertColumn(7,"֡��",LVCFMT_CENTER,40,0);
	m_ctlDlisCurves.InsertColumn(8,"ά����",LVCFMT_CENTER,75,0);
	for(int i=0;i<m_ObjectName.GetSize();i++)
	{
		m_ctlDlisCurves.InsertItem(m_ctlDlisCurves.GetItemCount(),m_ObjectName.GetAt(i));
		m_ctlDlisCurves.SetItemText(m_ctlDlisCurves.GetItemCount()-1,1,"");
		m_ctlDlisCurves.SetItemText(m_ctlDlisCurves.GetItemCount()-1,2,m_SourceName.GetAt(i));
		m_ctlDlisCurves.SetItemText(m_ctlDlisCurves.GetItemCount()-1,3,m_Origin.GetAt(i));
		m_ctlDlisCurves.SetItemText(m_ctlDlisCurves.GetItemCount()-1,4,m_CopyNum.GetAt(i));
		m_ctlDlisCurves.SetItemText(m_ctlDlisCurves.GetItemCount()-1,5,m_IndexInfo.GetAt(i));
		m_ctlDlisCurves.SetItemText(m_ctlDlisCurves.GetItemCount()-1,6,m_StepInfo.GetAt(i));
		m_ctlDlisCurves.SetItemText(m_ctlDlisCurves.GetItemCount()-1,7,m_FrameNum.GetAt(i));
		m_ctlDlisCurves.SetItemText(m_ctlDlisCurves.GetItemCount()-1,8,m_Dimension.GetAt(i));
	}
	bSucceed=false;
	//�����������м��
	OnCheckBtn();
	if(bSucceed && nTransMode!=3)
	{
		OnOK();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlisCurDlg::OnCheckBtn()
{
	UpdateData(false);
	bool bCurveName=true;
	CString sStatus;
	m_NumLog=m_ctlDlisCurves.GetItemCount();
	for(int i=0;i<m_NumLog;i++)
	{
		sStatus.Empty();
		CString sCurve1=m_ctlDlisCurves.GetItemText(i,0); //ȡ��������
		//��鳤��
		if(sCurve1.GetLength()>m_nMaxLen)
		{
			sStatus+="L";
		}
		//����Ƿ�ͬ��
		for(int j=0;j<i;j++)
		{
			CString sCurve2=m_ctlDlisCurves.GetItemText(j,0);
			if(sCurve1.CompareNoCase(sCurve2)==0)
			{
				if(m_sModuleName=="DLISTOLIS")
				{
					CString sFrame1=m_ctlDlisCurves.GetItemText(i,7);
					CString sFrame2=m_ctlDlisCurves.GetItemText(j,7);
					if(sFrame1==sFrame2)
					{	//ͬ֡����������ͬ
						sStatus+="S";
						break;
					}
				}
				else
				{	//��LISĿ���ļ�����������ͬ��ͬ
					sStatus+="S";
					break;
				}
			}
		}
		//����Ƿ��������ַ����»��߳���
		for(j=0;j<sCurve1.GetLength();j++)
		{
			if(!iswalnum(sCurve1.GetAt(j)) && sCurve1.GetAt(j)!='_' 
				&& sCurve1.GetAt(j)!='-' && sCurve1.GetAt(j)!='+'
				&& sCurve1.GetAt(j)!='[' && sCurve1.GetAt(j)!=']')
			{
				sStatus+="X";
				break;
			}
		}
		m_ctlDlisCurves.SetItemText(i,1,sStatus);
		if(!sStatus.IsEmpty())
		{
			bCurveName=false;
		}
	}
	bSucceed=true;
	m_sCurveStatus.Empty();
	if(bCurveName && m_NumLog<=m_nMaxLog)
	{
		m_sCurveStatus="��������";
	}
	else if(bCurveName && m_NumLog>m_nMaxLog)
	{
		m_sCurveStatus="����̫��";
		bSucceed=false;
	}
	else if(!bCurveName && m_NumLog<m_nMaxLog)
	{
		m_sCurveStatus="�������쳣";
		bSucceed=false;
	}
	else
	{
		m_sCurveStatus="�������쳣,����̫��";
		bSucceed=false;
	}
	UpdateData(false);
}

void CDlisCurDlg::OnDblclkDliscurvesList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	POSITION pos=m_ctlDlisCurves.GetFirstSelectedItemPosition(); //λ��
	if(pos!=NULL)
	{
		int nItem=m_ctlDlisCurves.GetNextSelectedItem(pos) ;  //��λ����ȡ��������
		m_ctlDlisCurves.EditLabel(nItem);  //ʹ����ɱ༭
	}	
	*pResult = 0;
}

void CDlisCurDlg::OnRclickDliscurvesList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CMenu menu;
	menu.LoadMenu(IDR_CURVENAME_POPUP);
	CMenu* pM=menu.GetSubMenu(0);
	CPoint pt;
	GetCursorPos(&pt);
	pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
	///////////////////////////////////////////////////////////
	*pResult = 0;
}

void CDlisCurDlg::OnEndlabeleditDliscurvesList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	CString sNewName=pDispInfo->item.pszText; 
	sNewName.Remove(' ');
	if(sNewName.IsEmpty())
		return; //û�и���
	POSITION pos=m_ctlDlisCurves.GetFirstSelectedItemPosition();
	if(pos)
	{
		int nItem=m_ctlDlisCurves.GetNextSelectedItem(pos) ;
		sNewName.MakeUpper(); //��ɴ�д
		sNewName.TrimLeft();
		sNewName.TrimRight();
		m_ctlDlisCurves.SetItemText(nItem,0,sNewName.Left(m_nMaxLen));
	}
	OnCheckBtn();
	////////////////////	
	*pResult = 0;
}

void CDlisCurDlg::OnOK() 
{
	// TODO: Add extra validation here
	OnCheckBtn();
	if(!bSucceed) 
	{
		AfxMessageBox("�������쳣��������̫��!");
		return;
	}
	//ȡ������
	FreeArrayMem(); //�ͷ�����
	GetArrayMem(); //ȡ���޸ĺ���Ϣ
	///////////////////////////////
	CDialog::OnOK();
}

void CDlisCurDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	FreeArrayMem();//�ͷ�����
	CDialog::OnCancel();
}

void CDlisCurDlg::FreeArrayMem()
{
	m_ObjectName.RemoveAll(); //���������
	m_SourceName.RemoveAll(); //����������
	m_Origin.RemoveAll(); //ͨ����Դ
	m_CopyNum.RemoveAll(); //ͨ���п�����
	m_IndexInfo.RemoveAll(); //��ֹ����
	m_StepInfo.RemoveAll(); //�������
	m_FrameNum.RemoveAll(); //֡������֡����Ŵ��棩
	m_Dimension.RemoveAll(); //ά����
}

void CDlisCurDlg::GetArrayMem()
{
	for(int i=0;i<m_ctlDlisCurves.GetItemCount();i++)
	{
		m_ObjectName.Add(m_ctlDlisCurves.GetItemText(i,0));
		m_SourceName.Add(m_ctlDlisCurves.GetItemText(i,2));
		m_Origin.Add(m_ctlDlisCurves.GetItemText(i,3));
		m_CopyNum.Add(m_ctlDlisCurves.GetItemText(i,4));
		m_IndexInfo.Add(m_ctlDlisCurves.GetItemText(i,5));
		m_StepInfo.Add(m_ctlDlisCurves.GetItemText(i,6));
		m_FrameNum.Add(m_ctlDlisCurves.GetItemText(i,7));
		m_Dimension.Add(m_ctlDlisCurves.GetItemText(i,8));
	}	
}

void CDlisCurDlg::OnCurvenameAddframeno() 
{
	CString s;
	OnCheckBtn();
	for(int i=1;i<m_ctlDlisCurves.GetItemCount();i++)
	{
		s=m_ctlDlisCurves.GetItemText(i,1);
		if(s.Find("S")>=0)
		{
			s.Format("%s%s",m_ctlDlisCurves.GetItemText(i,0),m_ctlDlisCurves.GetItemText(i,7));
			m_ctlDlisCurves.SetItemText(i,0,s);
		}
	}
	OnCheckBtn();
}

void CDlisCurDlg::OnCurvenameAddcopynum() 
{
	CString s;
	OnCheckBtn();
	for(int i=1;i<m_ctlDlisCurves.GetItemCount();i++)
	{
		s=m_ctlDlisCurves.GetItemText(i,1);
		if(s.Find("S")>=0)
		{
			s.Format("%s%s",m_ctlDlisCurves.GetItemText(i,0),m_ctlDlisCurves.GetItemText(i,4));
			m_ctlDlisCurves.SetItemText(i,0,s);
		}
	}
	OnCheckBtn();
}

void CDlisCurDlg::OnCurvenameAddorigin() 
{
	CString s;
	OnCheckBtn();
	for(int i=1;i<m_ctlDlisCurves.GetItemCount();i++)
	{
		s=m_ctlDlisCurves.GetItemText(i,1);
		if(s.Find("S")>=0)
		{
			s.Format("%s%s",m_ctlDlisCurves.GetItemText(i,0),m_ctlDlisCurves.GetItemText(i,3));
			m_ctlDlisCurves.SetItemText(i,0,s);
		}
	}
	OnCheckBtn();
}

void CDlisCurDlg::OnCurvenameDelbad() 
{
	CString s;
	OnCheckBtn();
	int nItemCount=m_ctlDlisCurves.GetItemCount();//��������
	if(nItemCount<1) return; 
	for(int i=nItemCount-1;i>=0;i--)
	{ 
		s=m_ctlDlisCurves.GetItemText(i,1);
		if(s.Find("X")>=0)
		{
			m_ctlDlisCurves.DeleteItem(i);
		}
	}
	OnCheckBtn();
}

void CDlisCurDlg::OnCurvenameDelsame() 
{
	CString s;
	OnCheckBtn();
	int nItemCount=m_ctlDlisCurves.GetItemCount();//��������
	if(nItemCount<1) return; 
	for(int i=nItemCount-1;i>=0;i--)
	{ 
		s=m_ctlDlisCurves.GetItemText(i,1);
		if(s.Find("S")>=0)
		{
			m_ctlDlisCurves.DeleteItem(i);
		}
	}
	OnCheckBtn();
}

void CDlisCurDlg::OnCurvenameDeltoolen() 
{
	CString s;
	OnCheckBtn();
	int nItemCount=m_ctlDlisCurves.GetItemCount();//��������
	if(nItemCount<1) return; 
	for(int i=nItemCount-1;i>=0;i--)
	{ 
		s=m_ctlDlisCurves.GetItemText(i,1);
		if(s.Find("L")>=0)
		{
			m_ctlDlisCurves.DeleteItem(i);
		}
	}
	OnCheckBtn();
}

void CDlisCurDlg::OnCurvenameDelselect() 
{
	CString s;
	int nItemSelected=m_ctlDlisCurves.GetSelectedCount();//��ѡ������ 
	int nItemCount=m_ctlDlisCurves.GetItemCount();//�������� 
	if(nItemSelected<1) return; 

	for(int i=nItemCount-1;i>=0;i--)
	{ 
		int iState=m_ctlDlisCurves.GetItemState(i,LVIS_SELECTED); 
		if(iState!=0) m_ctlDlisCurves.DeleteItem(i); 
	}
	OnCheckBtn();	
}

void CDlisCurDlg::OnCurvenameClearerror() 
{
	CString s;
	OnCheckBtn();
	int nItemCount=m_ctlDlisCurves.GetItemCount();//��������
	if(nItemCount<1) return; 
	for(int i=nItemCount-1;i>=0;i--)
	{ 
		s=m_ctlDlisCurves.GetItemText(i,1);
		if(!s.IsEmpty())
			m_ctlDlisCurves.DeleteItem(i);
	}
	OnCheckBtn();	
}

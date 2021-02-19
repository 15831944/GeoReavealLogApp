// SelectCurve.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "ProMultiCurveDep.h"
#include "FIDio.h"
#include <io.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProMultiCurveDep dialog


CProMultiCurveDep::CProMultiCurveDep(CString OpenPath,CImageManView *pView,CWnd* pParent /*=NULL*/)
	: CDialog(CProMultiCurveDep::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProMultiCurveDep)
	m_StrID = _T("");
	//}}AFX_DATA_INIT
	m_OpenPath=OpenPath;
	m_SectNum=0;
	m_AllCheck=FALSE;
	m_pView=pView;
}


void CProMultiCurveDep::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProMultiCurveDep)
	DDX_Control(pDX, IDC_PROGRESS2, m_CurveProgress);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
	DDX_Control(pDX, IDC_LIST1, m_DepList);
	DDX_Text(pDX, IDC_EDIT1, m_OptionStr);
	DDX_Control(pDX, IDC_LISTBOX, m_ProCurveList);
	DDX_Text(pDX, IDC_EDIT2, m_OpenPath);
	DDX_Text(pDX, IDC_EDIT3, m_StrID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProMultiCurveDep, CDialog)
	//{{AFX_MSG_MAP(CProMultiCurveDep)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProMultiCurveDep message handlers

BOOL CProMultiCurveDep::OnInitDialog() 
{
	int i;
	CDialog::OnInitDialog();
	CEdit *pEdit=(CEdit *)this->GetDlgItem (IDC_EDIT2);
	pEdit->SetSel (0,m_OpenPath.GetLength ());
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

	CFIDIO::GetPath(path,strPath.GetBuffer(512));
	strPath.ReleaseBuffer();
	strPath=strPath.Left (strPath.ReverseFind ('\\')+1);
	char szFIDName[40];
	struct _finddata_t c_file;
	long hFile;
	if ((hFile = _findfirst(strPath+"I*.*", &c_file )) != -1L)
	{
		CFIDIO* pFIDIO = new CFIDIO(strPath + c_file.name);
		if (pFIDIO->Open())
		{
			CFIDIO::GetFIDName(c_file.name,szFIDName);
			m_ProCurveList.AddString(szFIDName);
		}
		delete pFIDIO;
		while(_findnext(hFile,&c_file) == 0 ) 
		{
			CFIDIO* pFIDIO = new CFIDIO(strPath + c_file.name);
			if (pFIDIO->Open())
			{
				CFIDIO::GetFIDName(c_file.name,szFIDName);
				m_ProCurveList.AddString(szFIDName);
			}
			delete pFIDIO;
		}	        
		_findclose( hFile );
	}

	for (i=0; i<m_SelCurveNameArray.GetSize(); i++)
	{
		int k = m_ProCurveList.FindString(-1,m_SelCurveNameArray[i]);
		if (k != LB_ERR) m_ProCurveList.SetCheck(k,1);
		else m_ProCurveList.AddString(m_SelCurveNameArray[i]);
	}
	m_DepList.InsertColumn (0,"序号",LVCFMT_LEFT,40);
	m_DepList.InsertColumn (1,"处理井段",LVCFMT_LEFT,135);
	m_DepList.InsertColumn (2,"伸长/压缩量",LVCFMT_LEFT,80);
	CString str;
	for(i=0;i<m_SectNum;i++)
	{
		str.Format ("%d",i+1);
		m_DepList.InsertItem (i,str);
		m_DepList.SetItem (i,1,LVIF_TEXT,m_DepArray[i],-1,0,0,0);
		m_DepList.SetItem (i,2,LVIF_TEXT,m_ReviseArray[i],-1,0,0,0);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProMultiCurveDep::OnOK() 
{
	// TODO: Add extra validation here	
	UpdateData(TRUE);
	int num = m_ProCurveList.GetCount();
	CString s;
	m_SelCurveNameArray.RemoveAll();
	for (int i=0; i<num; i++)
	{
		if(m_ProCurveList.GetCheck(i)) 
		{
			m_ProCurveList.GetText(i,s);
			m_SelCurveNameArray.Add(s);
		}
	}
	int SelCurveNum=m_SelCurveNameArray.GetSize();
	ProMultiCurve(SelCurveNum);
	CDialog::OnOK();
}

void CProMultiCurveDep::OnButton2() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_AllCheck=!m_AllCheck;
	int num = m_ProCurveList.GetCount();
	int i;
	if(m_StrID.IsEmpty ())
	{
		for (i=0; i<num; i++)
			m_ProCurveList.SetCheck(i,m_AllCheck);
	}
	else
	{
		CString str;
		for (i=0; i<num; i++)
		{
			m_ProCurveList.GetText (i,str);
			if (str.Find (m_StrID) >-1)
				m_ProCurveList.SetCheck(i,m_AllCheck);
		}
	}
}

void CProMultiCurveDep::ProMultiCurve(int SelCurveNum)
{
	char path[512];
	strcpy(path,m_OpenPath);
	CString strPath=m_OpenPath;
	int yyy=strPath.Find ("#");
	if(yyy<0)
	{
		MessageBox("打开井数据失败  ","错误提示",MB_OK);
		return;
	}
	else
	{
		CFIDIO::GetPath(path,strPath.GetBuffer(512));
		strPath.ReleaseBuffer();
		strPath=strPath.Left (strPath.ReverseFind ('\\'));
		strPath=strPath+"\\"+strPath.Mid (yyy+1)+".FID";
	}
	//得到包括路径的井名FullWellName
	CString FullWellName;
	CFIDIO::GetFullWellName(strPath,FullWellName.GetBuffer(512));
	FullWellName.ReleaseBuffer();

	CString CurveName,CopyName;
	int i;float DaltaMove;
	m_Progress.SetRange32(0,m_SectNum);
	for(i=0;i<SelCurveNum;i++)
	{
		DaltaMove=m_pView->depArray[0][1]-m_pView->depArray[0][0];
		CurveName=m_SelCurveNameArray[i];
		CFIDIO* pFIDtemp = NULL;
		pFIDtemp = new CFIDIO( FullWellName + "."+CurveName);
		if(!pFIDtemp->Open())
		{
			MessageBox(CurveName+" 处理失败  ","错误提示...",MB_OK);
			delete pFIDtemp;
			continue;
		}			
		int dim=pFIDtemp->GetDimenSion ();
		CopyName.Format (".#$#%d",m_pView->m_InitOprationNum+1);
		CopyName=m_pView->m_UnDoFilePath+CopyName;	
		m_Progress.SetPos(0);
		m_CurveProgress.SetPos(0);
		GetDlgItem (IDC_CURVENAME)->SetWindowText(CurveName+"数据备份");
		if(dim>2)
		{
			ArrayCopy(FullWellName+"."+CurveName,CopyName,&m_CurveProgress);
			m_CurveProgress.SetPos(0);
		}
		else
			pFIDtemp->Copy(CopyName);
		
		float m_Sdep=pFIDtemp->GetStartDepth ();
		float m_Edep=pFIDtemp->GetEndDepth ();
		float m_Rlev=pFIDtemp->GetDepthLevel ();
		if(m_SectNum>-1)
		{
			FIDCONTENT* Content = pFIDtemp->GetContent(dim);
			short* ibuf = NULL;
			float* fbuf = NULL;
			char* cbuf = NULL;
			int nps = Content->nps;
			if (Content != NULL)
			{
				if (Content->type == REPR_FLOAT) 
					fbuf = new float[nps];
				else if (Content->type == REPR_CHAR) 
					cbuf = new char[nps];
				else 
					ibuf = new short[nps];
			}
			int m;
			float m_ExSdep,m_ExEdep,m_Extend,Prodep=0;
			for(m=0;m<m_SectNum;m++)
			{
				m_Extend=(m_pView->depArray[m+1][1]-m_pView->depArray[m][1])-(m_pView->depArray[m+1][0]-m_pView->depArray[m][0]);
				m_Extend=((int)(m_Extend/m_Rlev))*m_Rlev;
				Prodep+=m_Extend;
			}
			if(fabs(Prodep)>m_Rlev)
			{
				Prodep=m_Edep+Prodep;
				pFIDtemp->SetDepth(m_Sdep,Prodep);
			}
			delete pFIDtemp;
			CFIDIO* pFIDW = NULL;
			pFIDW = new CFIDIO(FullWellName + "."+CurveName);
			if(!pFIDW->Open())
			{
				MessageBox(CurveName+" 处理失败  ","错误提示",MB_OK);
				delete pFIDW;
				continue;
			}			
			CFIDIO* pFIDR = NULL;
			pFIDR = new CFIDIO(CopyName);
			if(!pFIDR->Open()) 
			{
				MessageBox(CurveName+" 处理失败  ","错误提示",MB_OK);
				delete pFIDW;
				delete pFIDR;
				continue;
			}
			pFIDW->SeekToDepth(m_pView->depArray[0][0]);
			int PointNum,ii;
			GetDlgItem (IDC_CURVENAME)->SetWindowText(CurveName+"深度处理");		
			for(m=0;m<m_SectNum;m++)
			{
				m_Progress.SetPos(m);
				m_ExSdep=((int)(m_pView->depArray[m][0]/m_Rlev))*m_Rlev;
				m_ExEdep=((int)(m_pView->depArray[m+1][0]/m_Rlev))*m_Rlev;
				m_Extend=(m_pView->depArray[m+1][1]-m_pView->depArray[m][1])-(m_pView->depArray[m+1][0]-m_pView->depArray[m][0]);
				m_Extend=((int)(m_Extend/m_Rlev))*m_Rlev;
				float ExDalta=m_pView->depArray[m+1][1]-m_pView->depArray[m][1];
				ExDalta=1.f-m_Extend/ExDalta;
				float x,depx;
				ii=0;
				PointNum=(int)((fabs(m_ExEdep-m_ExSdep+m_Extend))/m_Rlev);
				m_CurveProgress.SetRange32(0,PointNum);
				while(ii<PointNum)
				{
					m_CurveProgress.SetPos(ii);
					x=ExDalta*(float)ii;
					depx=m_ExSdep+((int)(x))*m_Rlev;
					pFIDR->SeekToDepth ((float)depx);
					if(Content->type == REPR_FLOAT)
					{
						pFIDR->Read(dim,nps,fbuf);
						pFIDW->Write(dim,nps,fbuf);
					}
					else if(Content->type == REPR_CHAR)
					{
						pFIDR->Read(dim,nps,cbuf);
						pFIDW->Write(dim,nps,cbuf);
					}
					else 
					{
						pFIDR->Read(dim,nps,ibuf);
						pFIDW->Write(dim,nps,ibuf);					
					}		
					pFIDW->Next();ii++;	
				}	
			}
			
			//处理后续深度
			if(fabs(m_pView->depArray[m_SectNum][0]-m_pView->depArray[m_SectNum][1])>m_Rlev)
			{
				if(m_pView->depArray[m_SectNum][0]<m_Edep)
				{
					pFIDR->SeekToDepth(m_pView->depArray[m_SectNum][0]);
					while(m_ExEdep<m_Edep)
					{
						if(Content->type == REPR_FLOAT)
						{
							pFIDR->Read(dim,nps,fbuf);
							pFIDW->Write(dim,nps,fbuf);
						}
						else if(Content->type == REPR_CHAR)
						{
							pFIDR->Read(dim,nps,cbuf);
							pFIDW->Write(dim,nps,cbuf);
						}
						else 
						{
							pFIDR->Read(dim,nps,ibuf);
							pFIDW->Write(dim,nps,ibuf);					
						}	
						pFIDR->Next();pFIDW->Next();
						m_ExEdep+=m_Rlev;
					}
				}
			}
			delete pFIDR;delete pFIDW;
			if (cbuf != NULL)	delete cbuf;
			if (ibuf != NULL)	delete ibuf;
			if (fbuf != NULL)	delete fbuf;
		}
		else
			delete pFIDtemp;
		if(fabs(DaltaMove)>m_Rlev)
		{
			CFIDIO* pFIDM = NULL;
			pFIDM= new CFIDIO(FullWellName + "."+CurveName);
			if(!pFIDM->Open())
				MessageBox(CurveName+" 整体移动失败  ","错误提示...",MB_OK);
			else
				pFIDM->MoveDepth (DaltaMove);
			delete pFIDM;	
		}
		m_pView->m_InitOprationNum++;
		m_pView->m_UnDoName.Add(FullWellName+"."+CurveName);
		m_pView->m_UnDoNameCopy.Add(CopyName);
	}
}

// FlowChartDoc.cpp : implementation of the CFlowChartDoc class
//

#include "stdafx.h"
#include "FlowChart.h"
#include "FlowChartDoc.h"
#include "FlowChartView.h"
#include "LinkWellData.h"
#include "PlayBack.h"
#include "BackupData.h"
#include "PadCorrect.h"
#include "ImageCreate.h"
#include "DataUnite.h"
#include "DataPoisePro.h"
#include "ImageToneup.h"
#include "ImageRMFScale.h"
#include "ImageAzCorrect.h"
#include "ImageGeoCorrect.h"
#include "ManPro.h"
#include "PORCompute.h"
#include "DIPCompute.h"
#include "AACCorrect.h"
#include "FlowNode.h"
#include "ProItem.h"
#include "CentOffCorrect.h"
#include "MinThickCp.h"
#include "CrackCompute.h"
#include "ComputeRADS.h"
#include "FIDio.h"
#include "DiffrenceCp.h"
#include "EdgeDetect.h"
#include "DataUnite.h"
#include "ImageAnisotropy.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlowChartDoc

IMPLEMENT_DYNCREATE(CFlowChartDoc, CDocument)

BEGIN_MESSAGE_MAP(CFlowChartDoc, CDocument)
	//{{AFX_MSG_MAP(CFlowChartDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
//	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlowChartDoc construction/destruction

CFlowChartDoc::CFlowChartDoc()
{
	// TODO: add one-time construction code here
	strcpy(m_VersionNo,"1.0   ");
	strcpy(m_Series,"STARV          ");
	m_szOption="";
	m_DocFileName="";
	m_bAunAll=FALSE;
}

CFlowChartDoc::~CFlowChartDoc()
{
	DeleteObjList();
}

BOOL CFlowChartDoc::OnNewDocument()
{
	CProItem dlg;
	dlg.DoModal ();
	if(dlg.m_ProItemName !="")
	{
		if (!CDocument::OnNewDocument())
			return FALSE;
		DeleteContents();
		m_szOption=dlg.m_ProItemName;
		SetTitle(m_szOption);
		CBackupData *pObj3=new CBackupData("��ʼ�����ݻ�����",FALSE);
		m_ParentList.AddTail(pObj3);
		if(m_szOption=="��������")
		{
			CComputeRADS *pObj11=new CComputeRADS("�����̶�",FALSE);
			m_ParentList.AddTail(pObj11);
			CCentOffCorrect *pObj12=new CCentOffCorrect("ƫ��У��",FALSE);
			m_ParentList.AddTail(pObj12);
			CImageCreat *pObj13=new CImageCreat("ͼ��ϳ�",FALSE);
			pObj13->m_WinLongth =0.6069f;
			m_ParentList.AddTail(pObj13);
			CImageAzCorrect *pObj14=new CImageAzCorrect("ͼ��λУ��",FALSE);
			pObj14->m_szImageType ="ACIMG";
			m_ParentList.AddTail(pObj14);
			
			CMinThickCp *pObj16=new CMinThickCp("������״����",FALSE);
			m_ChildList0.AddTail(pObj16);
			CManPro *pObj18=new CManPro("ͼ�񽻻�����",FALSE);
			m_ChildList1.AddTail(pObj18);	
			return TRUE;
		}
		if(m_szOption=="٤���ܶ�")
		{
			CImageCreat *pObj21=new CImageCreat("��̬ͼ��ϳ�",FALSE);
			pObj21->m_WinLongth =0.6069f;
			m_ParentList.AddTail(pObj21);	
			CImageGeoCorrect *pObj22=new CImageGeoCorrect("  ͼ�񼸺�У��  ",FALSE);
			m_ParentList.AddTail(pObj22);	
			return TRUE;
		}
		if(m_szOption=="��Ǵ���")
		{
			CDIPCompute *pObj31=new CDIPCompute("�ز���Ǽ���",FALSE);
			m_ChildList0.AddTail(pObj31);
			CDIPCompute *pObj32=new CDIPCompute("���ģʽ�Զ�ʶ��",FALSE);
			m_ChildList0.AddTail(pObj32);
			CManPro *pObj33=new CManPro("��ǽ�������",FALSE);
			m_ChildList1.AddTail(pObj33);
			return TRUE;
		}
		if(m_szOption!="��λ������")
		{
			CPadCorrect *pObj5=new CPadCorrect("���Ԥ����",FALSE);
			m_ParentList.AddTail(pObj5);
//			CDataUnite *pObj6=new CDataUnite("���ݺϲ�",FALSE);
//			m_ParentList.AddTail(pObj6);
		}
		CDataPoisePro *pObj7=new CDataPoisePro("����Ԥ����",FALSE);
		m_ParentList.AddTail(pObj7);
		CImageCreat *pObj8=new CImageCreat("��̬ͼ��ϳ�",FALSE);
		pObj8->m_WinLongth =0.6069f;
		m_ParentList.AddTail(pObj8);
		
		CImageGeoCorrect *pObjA0=new CImageGeoCorrect("��̬ͼ�񼸺�У��",FALSE);
		pObjA0->m_szImageType ="��̬ͼ��";
		m_ChildList0.AddTail(pObjA0);
		CImageToneup *pObjA1=new CImageToneup("��̬��ǿ����",FALSE);
		m_ChildList0.AddTail(pObjA1);
		CImageGeoCorrect *pObjA2=new CImageGeoCorrect("��̬ͼ�񼸺�У��",FALSE);
		pObjA2->m_szImageType ="��̬ͼ��";
		m_ChildList0.AddTail(pObjA2);
		CDIPCompute *pObjA3=new CDIPCompute("�ز���Ǽ���",FALSE);
		m_ChildList0.AddTail(pObjA3);
		CDIPCompute *pObjA4=new CDIPCompute("ȫ����ͼ���޸�",FALSE);
		m_ChildList0.AddTail(pObjA4);
		CDiffrenceCp *pObjA5=new CDiffrenceCp("����ͼ����ά�ؽ�",FALSE);
		pObjA5->m_szDifName ="��̬ͼ��";
		m_ChildList0.AddTail(pObjA5);

		CManPro *pObjB0=new CManPro("ͼ�񽻻�����",FALSE);
		m_ChildList1.AddTail(pObjB0);
		CImageRMFScale *pObjB1=new CImageRMFScale("�����ʿ̶�",FALSE);
		m_ChildList1.AddTail(pObjB1);
		CImageGeoCorrect *pObjB2=new CImageGeoCorrect("�̶�ͼ�񼸺�У��",FALSE);
		pObjB2->m_szImageType ="�̶�ͼ��";
		m_ChildList1.AddTail(pObjB2);
		CImageAnisotropy *pObjB3=new CImageAnisotropy("�������Է���",FALSE);
		m_ChildList1.AddTail(pObjB3);
		CPORCompute *pObjB4=new CPORCompute("�����϶���׷���",FALSE);
		m_ChildList1.AddTail(pObjB4);
		return TRUE;
	}
	return FALSE;
}

BOOL CFlowChartDoc::FindFlowNode(CString ItemName)
{
	CFlowNode *pObj=NULL;
	POSITION pos = m_ParentList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)m_ParentList.GetNext(pos);
		if (pObj != NULL) 
		{			
			if(pObj->m_szNodeName==ItemName)
				return TRUE;
		}
	}
	pos = m_ChildList0.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)m_ChildList0.GetNext(pos);
		if (pObj != NULL) 
		{			
			if(pObj->m_szNodeName==ItemName)
					return TRUE;
		}
	}
	pos = m_ChildList1.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)m_ChildList1.GetNext(pos);
		if (pObj != NULL) 
		{			
			if(pObj->m_szNodeName==ItemName)
				return TRUE;
		}
	}
	pos = m_ChildList2.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)m_ChildList2.GetNext(pos);
		if (pObj != NULL) 
		{			
			if(pObj->m_szNodeName==ItemName)
				return TRUE;
		}
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CFlowChartDoc serialization

void CFlowChartDoc::Serialize(CArchive& ar)
{
	char m_AppName[] = "FLOWCHART ";
	char tmp[100];
	strcpy(tmp,"Copyright (C) 2006-2010 SICHUAN LOG Company All rights reserved.");
	char Opt[40];
	strcpy(m_VersionNo,"1.0   ");
	if(ar.IsStoring())
	{
		// TODO: add storing code here
		ar.Write(m_AppName,10);
		ar.Write(m_VersionNo,6);
		ar.Write(m_Series,15);
		strcpy(Opt,m_szOption);
		ar.Write(Opt,40);
		ar.Write(tmp,60);
	}
	else
	{
		// TODO: add loading code here
		ar.Read(m_AppName,10);
		m_AppName[10] = '\0';
		ar.Read(m_VersionNo,6);
		m_VersionNo[6]= '\0';
		ar.Read(m_Series,15);
		ar.Read(Opt,40);
		m_szOption=Opt;
		ar.Read(tmp,60);
		if(m_szOption.Find ("Copyright (C)")>-1)
		{
			m_szOption=m_DocFileName.Mid(m_DocFileName.ReverseFind('\\')+1);
			m_szOption=m_szOption.Left(m_szOption.ReverseFind('.'));
		}
	}
	m_ParentList.Serialize(ar);
	m_ChildList0.Serialize(ar);
	m_ChildList1.Serialize(ar);
	m_ChildList2.Serialize(ar);
}

void CFlowChartDoc::InitPara()
{
	CFlowNode *pObj=NULL;
	POSITION pos = m_ParentList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)m_ParentList.GetNext(pos);
		if (pObj != NULL) 
			pObj->InitPara(this);
	}
	pos = m_ChildList0.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)m_ChildList0.GetNext(pos);
		if (pObj != NULL) 
			pObj->InitPara(this);
	}
	pos = m_ChildList1.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)m_ChildList1.GetNext(pos);
		if (pObj != NULL) 
			pObj->InitPara(this);
	}
	pos = m_ChildList2.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)m_ChildList2.GetNext(pos);
		if (pObj != NULL) 
			pObj->InitPara(this);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CFlowChartDoc diagnostics

#ifdef _DEBUG
void CFlowChartDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFlowChartDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFlowChartDoc commands

void CFlowChartDoc::DeleteContents() 
{
	// TODO: Add your specialized code here and/or call the base class
	DeleteObjList();
	CDocument::DeleteContents();
}

void CFlowChartDoc::DeleteObjList()
{
	CFlowNode *pObj=NULL;
	POSITION pos = m_ParentList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)m_ParentList.GetNext(pos);
		if(pObj!=NULL)
			delete pObj;
	}
	m_ParentList.RemoveAll();
	
	pos = m_ChildList0.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)m_ChildList0.GetNext(pos);
		if(pObj!=NULL)
			delete pObj;
	}
	m_ChildList0.RemoveAll();
	
	pos = m_ChildList1.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)m_ChildList1.GetNext(pos);
		if(pObj!=NULL)
			delete pObj;
	}
	m_ChildList1.RemoveAll();

	pos = m_ChildList2.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFlowNode*)m_ChildList2.GetNext(pos);
		if(pObj!=NULL)
			delete pObj;
	}
	m_ChildList2.RemoveAll();

	pos = GetFirstViewPosition();
	CFlowChartView* pView = (CFlowChartView*)GetNextView(pos);
	if(pView!=NULL)
	{
		pView->m_lnkTreeCtrl.CleanAllBuf();
		UpdateAllViews(pView);
	}
}

BOOL CFlowChartDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{	
	// TODO: Add your specialized creation code here
	m_DocFileName=lpszPathName;
 	if (!CDocument::OnOpenDocument(lpszPathName))
 		return FALSE;
	return TRUE;
}

BOOL CFlowChartDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	CString FileName=lpszPathName;
	CString str=FileName;
	str.MakeUpper();
	if(str.Find("FLOWCHART")>0)
	{
		CString Path=GetCurrentUserPath();
		FileName=FileName.Mid(FileName.ReverseFind('\\')+1);
		FileName=Path+FileName;
		static char BASED_CODE szFilter[] = 
		"������������(*.flow)|*.flow|�����ļ�(*.*)|*.*||";
		CFileDialog  Browse(FALSE, "������������(*.flow)", FileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
		Browse.m_ofn.lpstrTitle = "�����ļ�";
		Browse.m_ofn.lpstrInitialDir=Path;
		if(Browse.DoModal()==IDOK)
			FileName = Browse.GetPathName();
		else 
			return FALSE;
	}
	return CDocument::OnSaveDocument(FileName);
}

void CFlowChartDoc::OnFileSaveAs()
{
	CString FileName=m_DocFileName;
	CString Path=FileName.Left(FileName.ReverseFind('\\'));
	CString str=FileName;
	str.MakeUpper();
	if(str.Find("FLOWCHART")>0)
	{
		CString Path=GetCurrentUserPath();
		FileName=FileName.Mid(FileName.ReverseFind('\\')+1);
		FileName=Path+FileName;
	}
	static char BASED_CODE szFilter[] = 
	"������������(*.flow)|*.flow|�����ļ�(*.*)|*.*||";
	CFileDialog  Browse(FALSE, "������������(*.flow)", FileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	Browse.m_ofn.lpstrTitle = "�����ļ�";
	Browse.m_ofn.lpstrInitialDir=Path;
	if(Browse.DoModal()==IDOK)
		FileName = Browse.GetPathName();
	else 
		return;
	CDocument::OnSaveDocument(FileName);
}

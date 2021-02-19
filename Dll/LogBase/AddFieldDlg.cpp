/****************************************************************/
/*																*/
/*  AddFieldDlg.cpp												*/
/*																*/
/*  Implementation of the CAddFieldDlg class.					*/
/*	This file is part of the DBF Explorer application.			*/
/*																*/
/*																*/
/*  Last updated: 25 Feb 2004									*/
/*																*/
/****************************************************************/


#include "stdafx.h"
#include "AddFieldDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CAddFieldDlg::CAddFieldDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddFieldDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddFieldDlg)
	m_nDecimals = 0;
	m_nLength = 10;
	m_strName = _T("");
	//}}AFX_DATA_INIT
	m_nType = 0;
}


void CAddFieldDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddFieldDlg)
	DDX_Control(pDX, IDC_TYPE, m_cmbType);
	DDX_Text(pDX, IDC_DECIMALS, m_nDecimals);
	DDX_Text(pDX, IDC_LENGTH, m_nLength);
	DDX_Text(pDX, IDC_NAME, m_strName);
	//}}AFX_DATA_MAP

	pDX->PrepareEditCtrl(IDC_LENGTH);
	if (pDX->m_bSaveAndValidate)
	{
		switch(m_nType)
		{
			// CHAR
			case 0:
				// validate length
				DDV_MinMaxInt(pDX, m_nLength, 1, 255);
				break;
			// NUMERIC or FLOAT
			case 2:
			case 4:
				// validate length
				DDV_MinMaxInt(pDX, m_nLength, 1, 19);
				break;
		}
	}
}


BEGIN_MESSAGE_MAP(CAddFieldDlg, CDialog)
	//{{AFX_MSG_MAP(CAddFieldDlg)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_CBN_SELCHANGE(IDC_TYPE, OnSelchangeType)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(IDC_LENGTH, OnUpdateLength)
	ON_UPDATE_COMMAND_UI(IDC_LENGTH_STATIC, OnUpdateLength)
	ON_UPDATE_COMMAND_UI(IDC_DECIMALS, OnUpdateDecimals)
	ON_UPDATE_COMMAND_UI(IDC_DECIMALS_STATIC, OnUpdateDecimals)
	ON_UPDATE_COMMAND_UI(IDOK, OnUpdateOK)
END_MESSAGE_MAP()


/********************************************************************/
/*																	*/
/* Function name : OnInitDialog										*/		
/* Description   : Initialize dialog								*/
/*																	*/
/********************************************************************/
BOOL CAddFieldDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// limit input fields
	SendDlgItemMessage(IDC_NAME, EM_LIMITTEXT , (WPARAM)11 ,0L);
	SendDlgItemMessage(IDC_LENGTH, EM_LIMITTEXT , (WPARAM)3 ,0L);
	SendDlgItemMessage(IDC_DECIMALS, EM_LIMITTEXT , (WPARAM)2 ,0L);

	if (!m_strTitle.IsEmpty())
		SetWindowText(m_strTitle);

	CenterWindow();

	// select field type
	m_cmbType.SetCurSel(m_nType);
	
	// do not allow change to or from memo field
/*	if (!m_strName.IsEmpty())
	{
		if (m_nType == 5)
		{
			m_cmbType.EnableWindow(FALSE);
		}
		else
			m_cmbType.DeleteString(5);
	}
	*/
	OnSelchangeType();
	return TRUE;
}


/********************************************************************/
/*																	*/
/* Function name : OnUpdateLength									*/		
/* Description   : Update 'Length' field							*/
/*																	*/
/********************************************************************/
void CAddFieldDlg::OnUpdateLength(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_nType == 0 || m_nType == 2 || m_nType == 4);
}


/********************************************************************/
/*																	*/
/* Function name : OnUpdateDecimals									*/		
/* Description   : Update 'Decimals' field							*/
/*																	*/
/********************************************************************/
void CAddFieldDlg::OnUpdateDecimals(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_nType == 2 || m_nType == 4);	
}


/********************************************************************/
/*																	*/
/* Function name : OnUpdateOK										*/		
/* Description   : Update 'OK' button								*/
/*																	*/
/********************************************************************/
void CAddFieldDlg::OnUpdateOK(CCmdUI* pCmdUI) 
{
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_NAME);
	pCmdUI->Enable(pEdit->GetWindowTextLength());	
}


/********************************************************************/
/*																	*/
/* Function name : OnChangeName										*/		
/* Description   : Name has been changed							*/
/*																	*/
/********************************************************************/
void CAddFieldDlg::OnChangeName() 
{
	UpdateDialogControls(this, FALSE);
}


/********************************************************************/
/*																	*/
/* Function name : OnSelchangeType									*/		
/* Description   : Type has changed									*/
/*																	*/
/********************************************************************/
void CAddFieldDlg::OnSelchangeType() 
{
	m_nType = m_cmbType.GetCurSel();
	switch(m_nType)
	{
		case 0:
			SetDlgItemInt(IDC_DECIMALS, 0);
			break;
		case 1:
			SetDlgItemInt(IDC_LENGTH, 8);
			SetDlgItemInt(IDC_DECIMALS, 0);
			break;
		case 3:
			SetDlgItemInt(IDC_LENGTH, 1);
			SetDlgItemInt(IDC_DECIMALS, 0);
			break;
		case 4:
			SetDlgItemInt(IDC_LENGTH, 10);
			SetDlgItemInt(IDC_DECIMALS, 0);
			break;
	}
	UpdateDialogControls(this, FALSE);	
}

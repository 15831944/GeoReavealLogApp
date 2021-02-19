// ExportFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ExportFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExportFileDialog

IMPLEMENT_DYNAMIC(CExportFileDialog, CFileDialog)

CExportFileDialog::CExportFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}


BEGIN_MESSAGE_MAP(CExportFileDialog, CFileDialog)
	//{{AFX_MSG_MAP(CExportFileDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CExportFileDialog::OnTypeChange()
{
	HWND hComboExt = ::GetDlgItem(::GetParent(m_hWnd), 0x0470);
	if (hComboExt != NULL)
	{
		::SendMessage(hComboExt, CB_GETLBTEXT, (WPARAM)(m_ofn.nFilterIndex - 1), (LPARAM)(LPCTSTR)m_strType.GetBuffer(128));
		m_strType.ReleaseBuffer();
	}
}

BOOL CExportFileDialog::OnInitDialog() 
{
	CFileDialog::OnInitDialog();
	
	OnTypeChange();
	return TRUE;
}

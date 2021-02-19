#pragma once

#include "GeoStorage.h"
class CBusyMessage;
// CWellBrowse 对话框
class CWellBrowse : public CDialog
{
	DECLARE_DYNAMIC(CWellBrowse)

public:
	CWellBrowse(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWellBrowse();

	CString	m_szSever;

	DWORD		m_RootID;
	CString		m_RootName;
	DWORD		m_ProjectID;
	CString		m_ProjectName;
	CString		m_WellName;

	CImageList	WellimageTree;
	CTreeCtrl	m_TreeCtrl;
	CListCtrl	m_WellCtrl;

	CGeoStorage	m_Storage;
// 对话框数据
	enum { IDD = IDD_WELL_BROWSE};
	CBusyMessage *pBusy;
	void PumpMessages();
	void AddtoWell(DWORD UpperID);

protected:
	void ChangTreeContent();
	void AddtoTree(CTreeCtrl& pTree,HTREEITEM hNode, DWORD UpperID);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTvnSelchangedWelltree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickWellListctrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBrowseAll();
	afx_msg void OnNMDblclkWellListctrl(NMHDR *pNMHDR, LRESULT *pResult);
};

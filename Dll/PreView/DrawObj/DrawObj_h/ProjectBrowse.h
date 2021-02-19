#pragma once

#include "GeoStorage.h"
class CBusyMessage;
// CProjectBrowse 对话框
class CProjectBrowse : public CDialog
{
	DECLARE_DYNAMIC(CProjectBrowse)

public:
	CProjectBrowse(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProjectBrowse();

	CString	m_szSever;

	DWORD		m_RootID;
	CString		m_RootName;
	DWORD		m_ProjectID;
	CString		m_ProjectName;
	CString		m_WellName;

	CImageList	WellimageTree;
	CTreeCtrl	m_TreeCtrl;
	CListCtrl	m_WellCtrl;
	CListCtrl	m_ProjectCtrl;

	CGeoStorage	m_Storage;
// 对话框数据
	enum { IDD = IDD_PROJECT_BROWSE};
	CBusyMessage *pBusy;
	void PumpMessages();
	void AddtoWell(DWORD UpperID);
	void AddtoProject(DWORD UpperID);
protected:
	void ChangTreeContent();
	void AddtoTree(CTreeCtrl& pTree,HTREEITEM hNode, DWORD UpperID);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTvnSelchangedWelltree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickWellListctrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickProjectListctrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBrowseAll();
	afx_msg void OnNMDblclkProjectListctrl(NMHDR *pNMHDR, LRESULT *pResult);
};

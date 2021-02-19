// SysImageList.cpp: implementation of the CSysImageList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SysImageList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSysImageList::CSysImageList(BOOL bLargeIcons) : 
							m_bLargeIcons(bLargeIcons), 
							m_nFolderImage(-1), 
							m_nHtmlImage(-1), 
							m_hImageList(NULL)
{

}

CSysImageList::~CSysImageList()
{

}

BOOL CSysImageList::Initialize()
{
	if (m_hImageList)
		return TRUE;

	// set up system image list
	char szWindows[MAX_PATH];
	GetWindowsDirectory(szWindows, MAX_PATH);

	SHFILEINFO sfi;

	UINT nFlags = SHGFI_SYSICONINDEX | (m_bLargeIcons ? SHGFI_ICON : SHGFI_SMALLICON);
	HIMAGELIST hIL = (HIMAGELIST)SHGetFileInfo(szWindows, 0, &sfi, sizeof(sfi), nFlags);

	if (hIL)
	{
		m_hImageList = hIL;

		// intialize the stock icons
		m_nFolderImage = sfi.iIcon;
		m_nHtmlImage = GetFileImageIndex("test.html");
	}

	// else
	return (m_hImageList != NULL);
}

int CSysImageList::GetFileImageIndex(LPCTSTR szFilePath)
{
	if (!m_hImageList && !Initialize())
		return -1;

	// check if its a folder first
	DWORD dwFileAttrib = GetFileAttributes(szFilePath);

	if (dwFileAttrib != 0xffffffff && (dwFileAttrib & FILE_ATTRIBUTE_DIRECTORY))
		return m_nFolderImage;

	// then test for web protocol
	if (IsWebAddress(szFilePath))
		return m_nHtmlImage;

	// else
	SHFILEINFO sfi;

	// use the entire path if <= MAX_PATH in length else just the extension
	char szExt[_MAX_EXT];

	if (lstrlen(szFilePath) > MAX_PATH)
	{
		_splitpath(szFilePath, NULL, NULL, NULL, szExt);
		szFilePath = szExt;
	}

	UINT nFlags = SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES | (m_bLargeIcons ? SHGFI_ICON : SHGFI_SMALLICON);
	HIMAGELIST hIL = (HIMAGELIST)SHGetFileInfo(szFilePath, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), nFlags);

	ASSERT (!hIL || hIL == m_hImageList);

	m_hImageList = hIL;

	if (!hIL)
		return -1;

	return sfi.iIcon;
}

int CSysImageList::GetFolderImageIndex() 
{ 
	if (!m_hImageList && !Initialize())
		return -1;

	return m_nFolderImage;
}

HICON CSysImageList::ExtractFileIcon(LPCTSTR szFilePath)
{
	if (!m_hImageList && !Initialize())
		return NULL;

	int nIndex = GetFileImageIndex(szFilePath);

	if (nIndex != -1)
		return GetImageList()->ExtractIcon(nIndex);

	return NULL;
}

HICON CSysImageList::ExtractFolderIcon()
{
	if (!m_hImageList && !Initialize())
		return NULL;

	return GetImageList()->ExtractIcon(m_nFolderImage);
}

const CImageList* CSysImageList::GetImageList() const
{ 
	CImageList* pIL = CImageList::FromHandle(m_hImageList); 

	if (pIL)
		return pIL;

	// else
	static CImageList il;
	return &il;
}

CImageList* CSysImageList::GetImageList() 
{ 
	if (!m_hImageList)
		Initialize();

	CImageList* pIL = CImageList::FromHandle(m_hImageList); 

	if (pIL)
		return pIL;

	// else backup plan
	static CImageList il;
	return &il;
}

BOOL CSysImageList::IsWebAddress(LPCTSTR szFilePath)
{
	CString sPath(szFilePath);

	sPath.MakeUpper();
	sPath.TrimLeft();
	sPath.TrimRight();

	if (sPath.Find("HTTP://") == 0)
		return TRUE;

	else if (sPath.Find("FTP://") == 0)
		return TRUE;

	else if (sPath.Find("WWW.") == 0)
		return TRUE;

//	else if (sPath.Find("???://") == 0)
//		return TRUE;

	// else
	return FALSE;
}

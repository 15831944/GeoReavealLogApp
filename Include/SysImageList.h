// SysImageList.h: interface for the CSysImageList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSIMAGELIST_H__6280CAC5_7A95_48C8_8BBA_3EB7284234F7__INCLUDED_)
#define AFX_SYSIMAGELIST_H__6280CAC5_7A95_48C8_8BBA_3EB7284234F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSysImageList  
{
public:
	CSysImageList(BOOL bLargeIcons = FALSE);
	virtual ~CSysImageList();

	const CImageList* GetImageList() const; // temporary. should not be stored
	CImageList* GetImageList(); // temporary. should not be stored

	BOOL Initialize();
	int GetFileImageIndex(LPCTSTR szFilePath); // will call Initialize if nec.
	int GetFolderImageIndex(); // will call Initialize if nec.

	// caller is responsible for clean up
	HICON ExtractFileIcon(LPCTSTR szFilePath);
	HICON ExtractFolderIcon();

protected:
	BOOL m_bLargeIcons;
	int m_nFolderImage, m_nHtmlImage;
	HIMAGELIST m_hImageList;

protected:
	BOOL IsWebAddress(LPCTSTR szFilePath);
};

#endif // !defined(AFX_SYSIMAGELIST_H__6280CAC5_7A95_48C8_8BBA_3EB7284234F7__INCLUDED_)

// DBase.h: interface for the CDBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBASE_H__EED90929_43D8_4D03_8AB8_53B46E5BC364__INCLUDED_)
#define AFX_DBASE_H__EED90929_43D8_4D03_8AB8_53B46E5BC364__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDBase  
{
public:
	CDBase();
	virtual ~CDBase();
// Attributes
public:
	CDbaseFile m_dBaseFile;
// Operations
public:
	BOOL NewDBase();
	BOOL OpenDBase(LPCTSTR lpszPathName);
	void OnCloseDBase();
	BOOL OnSaveDBase(LPCTSTR lpszPathName);

// Implementation
public:
	BOOL SetFilter(LPCTSTR lpszFieldName, LPCTSTR lpszCriteria);
public:
	void OnFileStructure();
	void OnFilePackdatabase();
	void OnFileExport();
protected:

protected:
	void ExportToHTML(LPCTSTR lpszFileName);
	void ExportToText(LPCTSTR lpszFileName);
	BOOL CopyBackupData(LPCTSTR lpszBackupFile);
	char m_szTempFileName[MAX_PATH];
};

#endif // !defined(AFX_DBASE_H__EED90929_43D8_4D03_8AB8_53B46E5BC364__INCLUDED_)

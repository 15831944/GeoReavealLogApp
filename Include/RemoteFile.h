// RemoteFile.h: interface for the CRemoteFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REMOTEFILE_H__04648C98_565F_41B2_A1F7_E10E00800691__INCLUDED_)
#define AFX_REMOTEFILE_H__04648C98_565F_41B2_A1F7_E10E00800691__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "remotefiledialog.h"
#include <afxinet.h>
enum RMERR
{
	RMERR_SUCCESS,
	RMERR_UNKNOWN,
	RMERR_SERVERDETAILS,
	RMERR_REMOTEPATH,
	RMERR_LOCALPATH,
	RMERR_OPENINGREMOTEFILETOREAD,
	RMERR_OPENINGREMOTEFILETOWRITE,
	RMERR_OPENINGLOCALFILETOREAD,
	RMERR_OPENINGLOCALFILETOWRITE,
	RMERR_CONNECTING,
	RMERR_USERCANCELLED,
	RMERR_NOTCONNECTED,
	RMERR_READINGREMOTEFILE,
	RMERR_READINGLOCALFILE,
	RMERR_WRITINGREMOTEFILE,
	RMERR_WRITINGLOCALFILE,
//	RMERR_,
};

enum // options
{
	RMO_ALLOWDIALOG			= 0x0001,
	RMO_USETEMPFILE			= 0x0002, // only valid for download
	RMO_KEEPEXTENSION		= 0x0004, // only valid if RMO_USETEMPFILE is specified
	RMO_LOWERCASEPATHS		= 0x0008,
	RMO_DELETEFAILURES		= 0x0010, 
	RMO_CREATEDOWNLOADDIR	= 0x0020, // if not already there
	RMO_ANONYMOUSLOGIN		= 0x0040, 
	RMO_CONFIRMOVERWRITE	= 0x0080, 
	RMO_NONAVIGATE			= 0x0100, // restricts browsing to the initial folder
	RMO_NOANONYMOUSLOGIN	= 0x0200, // prevents anonymous logins and hides the checkbox on the server dialog
/*	RMO_ = 0x0001,
	RMO_ = 0x0001,
*/
};

// note: if you don't need the browse capabilities then you can #define NO_DIALOGS 
// in which case you must pass in correct parameters to avoid failure

// note: file filter takes standard form: eg. "Zip Files (*.zip)|*.zip||"

class CProgressDlg;
struct FILERESULT;

class AFX_EXT_CLASS CRemoteFile : protected CInternetSession
{
public:
	CRemoteFile(LPCTSTR szAgent = NULL, LPCTSTR szServer = NULL, LPCTSTR szUsername = NULL, LPCTSTR szPassword = NULL, CWnd* pParent = NULL);
	virtual ~CRemoteFile();

	// sRemoteFile is always sans server eg 'abstractspoon\test.htm' 

	// both sRemotePath and sLocalPath may reference folders in both GetFile() and PutFile()

	// if sRemotePath is a folder then it must have a trailing forward slash '/'
	// if sLocalPath is a folder that exists then it need not have a trailing backslash '\'
	// however if it might not exist and you want it created then append a trailing backslash

	RMERR GetFile(CString& sRemotePath, CString& sLocalPath, 
				DWORD dwOptions = RMO_ALLOWDIALOG | RMO_CREATEDOWNLOADDIR | RMO_CONFIRMOVERWRITE, 
				LPCTSTR szFilter = NULL);
	RMERR PutFile(CString& sLocalPath, CString& sRemotePath, 
				DWORD dwOptions = RMO_ALLOWDIALOG | RMO_LOWERCASEPATHS | RMO_DELETEFAILURES | 
				RMO_CONFIRMOVERWRITE);

	CString GetServer() { return m_sServer; }
	CString GetUsername() { return m_sUsername; }
	CString GetPassword() { return m_sPassword; }
	CString GetLastError() { return m_sLastError; }

	static void SplitPath(LPCTSTR szFullRemotePath, CString& sServer, CString& sFile);

protected:
	CString m_sServer, m_sUsername, m_sPassword;
	CString m_sRemotePath, m_sLocalPath;
	CFtpConnection* m_pConnection;
	DWORD m_dwInternetErr;
	CWnd* m_pParent;
	CString m_sLastError;
	
protected:
	BOOL EstablishConnection(RMERR& nRes, DWORD dwOptions);
	BOOL DoServerDlg(DWORD dwOptions, BOOL& bAnonLogin); // returns TRUE for IDOK else FALSE (cancel)
	void CloseConnection();

	RMERR DownloadFile(const FILERESULT* pRemoteFile, LPCTSTR szToLocalPath, DWORD dwOptions, CProgressDlg* pDlg = NULL);
	RMERR UploadFile(LPCTSTR szFromLocalPath, const FILERESULT* pRemoteFile, DWORD dwOptions, CProgressDlg* pDlg = NULL);

	void ValidateOptions(DWORD& dwOptions, BOOL bDownload);
	RMERR SaveErrorMsg(RMERR nErr, LPCTSTR szRemotePath = NULL, LPCTSTR szLocalPath = NULL); // returns nErr to allow chaining
	DWORD GetRemoteFileSize(LPCTSTR szRemotePath);

	RMERR GetRemotePaths(CFRArray& aRemoteFiles, const CStringArray& aLocalFiles, DWORD dwOptions, LPCTSTR szRemoteDir = NULL); // for upload
	RMERR GetRemotePaths(CFRArray& aRemoteFiles, DWORD dwOptions, LPCTSTR szFilter, LPCTSTR szRemoteDir = NULL); // for download

	RMERR GetLocalPaths(CStringArray& aLocalFiles, BOOL& bTemp, const CFRArray& aRemoteFiles, DWORD dwOptions, LPCTSTR szLocalDir = NULL); // for download
	RMERR GetLocalPaths(CStringArray& aLocalFiles, DWORD dwOptions, LPCTSTR szLocalDir = NULL); // for upload

	CString GetTempPath(const CString& sRemotePath, BOOL bKeepExt);
	BOOL RemoteFileExists(LPCTSTR szRemotePath);

	static BOOL ValidateLocalFolder(CString& sFolder, BOOL bAllowCreation);
};

#endif // !defined(AFX_REMOTEFILE_H__04648C98_565F_41B2_A1F7_E10E00800691__INCLUDED_)

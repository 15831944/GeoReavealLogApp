// RemoteFile.cpp: implementation of the CRemoteFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RemoteFile.h"

#include "serverdlg.h"
#include "RemoteFiledialog.h"
#include "progressdlg.h"
#include "folderdialog.h"
#include "filemisc.h"

#pragma comment(lib, "wininet.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const UINT BUFSIZE = 5 * 1024;
const LPCTSTR ALLFILES = "All Files (*.*)\0*.*\0";

CRemoteFile::CRemoteFile(LPCTSTR szAgent, LPCTSTR szServer, LPCTSTR szUsername, LPCTSTR szPassword, CWnd* pParent)
	: CInternetSession(szAgent), 
	  m_sServer(szServer), 
	  m_sUsername(szUsername), 
	  m_sPassword(szPassword), 
	  m_pConnection(NULL),
	  m_pParent(pParent)
{
	m_sServer.TrimLeft();
	m_sServer.TrimRight();
	m_sUsername.TrimLeft();
	m_sUsername.TrimRight();
	m_sPassword.TrimLeft();
	m_sPassword.TrimRight();
}

CRemoteFile::~CRemoteFile()
{
	CloseConnection();
	Close();
}

RMERR CRemoteFile::SaveErrorMsg(RMERR nErr, LPCTSTR szLocalPath, LPCTSTR szRemotePath)
{
	m_sLastError.Empty();

	switch (nErr)
	{
	case RMERR_SUCCESS:
		break;

	case RMERR_REMOTEPATH:
		m_sLastError = "The remote file could not be found or was not correctly specified";
		break;

	case RMERR_LOCALPATH:
		m_sLastError = "The local file could not be found was not correctly specified";
		break;

	case RMERR_OPENINGREMOTEFILETOREAD:
		m_sLastError.Format("The remote file (%s) could not be opened for reading", szRemotePath);
		break;

	case RMERR_OPENINGREMOTEFILETOWRITE:
		m_sLastError.Format("The remote file (%s) could not be opened for writing", szRemotePath);
		break;

	case RMERR_OPENINGLOCALFILETOREAD:
		m_sLastError.Format("The local file (%s) could not be opened for reading", szLocalPath);
		break;

	case RMERR_OPENINGLOCALFILETOWRITE:
		m_sLastError.Format("The local file (%s) could not be opened for writing", szLocalPath);
		break;

	case RMERR_CONNECTING:
		m_sLastError.Format("The remote server (%s) did not accept the connection", m_sServer);
		break;

	case RMERR_USERCANCELLED:
		m_sLastError.Format("用户中断连结");
		break;

	case RMERR_NOTCONNECTED:
		m_sLastError.Format("There is no current connection to the server (%s)", m_sServer);
		break;

	case RMERR_READINGREMOTEFILE:
		m_sLastError.Format("The remote file (%s) could not be read", szRemotePath);
		break;

	case RMERR_WRITINGREMOTEFILE:
		m_sLastError.Format("The remote file (%s) could not be written to", szRemotePath);
		break;

	case RMERR_READINGLOCALFILE:
		m_sLastError.Format("The local file (%s) could not be read", szLocalPath);
		break;

	case RMERR_WRITINGLOCALFILE:
		m_sLastError.Format("The local file (%s) could not be written to", szLocalPath);
		break;

	default:
	case RMERR_UNKNOWN:
		if (m_dwInternetErr)
			m_sLastError.Format("An internet error occurred (%d)", m_dwInternetErr);
		else
			m_sLastError = "An unknown error occurred";
	}

	return nErr;
}

// download
RMERR CRemoteFile::GetFile(CString& sRemotePath, CString& sLocalPath, DWORD dwOptions, LPCTSTR szFilter)
{
	ValidateOptions(dwOptions, TRUE);

	RMERR nRes = RMERR_SUCCESS;
	BOOL bAllowDialog = (dwOptions & RMO_ALLOWDIALOG);

	if (!EstablishConnection(nRes, dwOptions))
		return SaveErrorMsg(nRes);

	// remote path(s)
	CFRArray aRemoteFiles;

	if (nRes == RMERR_SUCCESS)
	{
		sRemotePath.TrimLeft();
		sRemotePath.TrimRight();

		if (sRemotePath.IsEmpty())
			nRes = GetRemotePaths(aRemoteFiles, dwOptions, szFilter);
		else
		{
			sRemotePath.Replace('\\', '/');

			// if the remote path ends in '/' then its a folder 
			// so we must still get the file
			if (sRemotePath.GetAt(sRemotePath.GetLength() - 1) == '/')
				nRes = GetRemotePaths(aRemoteFiles, dwOptions, szFilter, sRemotePath);

			else // its a file
				aRemoteFiles.Add(FILERESULT(sRemotePath));
		}
	}

	// local file(s)
	CStringArray aLocalFiles;
	BOOL bTemp = FALSE;

	if (nRes == RMERR_SUCCESS)
	{
		sLocalPath.TrimLeft();
		sLocalPath.TrimRight();

		// if we are downloading more than one file
		// or the local path has not been specified correctly then get it
		BOOL bLocalPathIsDir = FolderExists(sLocalPath);
		BOOL bMulti = (aRemoteFiles.GetSize() > 1);

		if (bMulti || bLocalPathIsDir || !CreateFolderFromFilePath(sLocalPath))
		{
			CString sInitialDir;

			// try to create/get the initial dir
			if (!bLocalPathIsDir)
			{
				if (!sLocalPath.IsEmpty() && CreateFolderFromFilePath(sLocalPath))
				{
					char szDrive[_MAX_DRIVE], szPath[MAX_PATH];
					_splitpath(sLocalPath, szDrive, szPath, NULL, NULL);

					sInitialDir.Format("%s%s", szDrive, szPath);
				}
			}
			else
				sInitialDir = sLocalPath;

			nRes = GetLocalPaths(aLocalFiles, bTemp, aRemoteFiles, dwOptions, sInitialDir);
		}
		else // single file 
			aLocalFiles.Add(sLocalPath);
	}

	if (nRes == RMERR_SUCCESS && aRemoteFiles.GetSize())
	{
		CProgressDlg progDlg;

		if (progDlg.Create("数据传输", NULL, m_pParent))
		{
			if (m_pParent)
				m_pParent->EnableWindow(FALSE);

			BOOL bConfirmOverwrite = (dwOptions & RMO_CONFIRMOVERWRITE);
			int nFileCount = aRemoteFiles.GetSize();
			
			for (int nFile = 0; nFile < nFileCount; nFile++)
			{
				if (nFileCount > 1)
				{
					CString sDlgTitle;
					sDlgTitle.Format("数据传输(%d-%d)", nFile + 1, nFileCount);
					progDlg.SetCaption(sDlgTitle);
				}

				RMERR nFileRes = DownloadFile(&aRemoteFiles[nFile], aLocalFiles[nFile], bConfirmOverwrite, &progDlg);

				// handle single files
				if (nFileCount == 1)
				{
					nRes = nFileRes;
					sRemotePath = aRemoteFiles[0].sFilePath;
					sLocalPath = aLocalFiles[0];
				}
			}

			if (m_pParent)
				m_pParent->EnableWindow(TRUE);
		}
	}

	// cleanup
	if (nRes != RMERR_SUCCESS && bTemp)
	{
		int nFile = aLocalFiles.GetSize();

		while (nFile--)
			::DeleteFile(aLocalFiles[nFile]);
	}
	// if multiple files downloaded then set sLocalPath to be the parent folder
	else if (aLocalFiles.GetSize() > 1)
	{
		char szDrive[_MAX_DRIVE], szPath[MAX_PATH];
		_splitpath(aLocalFiles[0], szDrive, szPath, NULL, NULL);

		sLocalPath.Format("%s%s", szDrive, szPath);
	}

	CloseConnection();

	return SaveErrorMsg(nRes, sLocalPath, sRemotePath);
}

// upload
RMERR CRemoteFile::PutFile(CString& sLocalPath, CString& sRemotePath, DWORD dwOptions)
{
	ValidateOptions(dwOptions, FALSE);

	RMERR nRes = RMERR_SUCCESS;
	BOOL bAllowDialog = (dwOptions & RMO_ALLOWDIALOG);

	// if local file not specified then get browse for it
	CStringArray aLocalFiles;

	if (GetFileAttributes(sLocalPath) == 0xffffffff) // _file_ must exist
		nRes = GetLocalPaths(aLocalFiles, dwOptions);

	else if (FolderExists(sLocalPath)) // localpath is a folder
		nRes = GetLocalPaths(aLocalFiles, dwOptions, sLocalPath);

	else // existing file
	{
		if (dwOptions & RMO_LOWERCASEPATHS)
			sLocalPath.MakeLower();

		aLocalFiles.Add(sLocalPath);
	}

	if (nRes != RMERR_SUCCESS || !EstablishConnection(nRes, dwOptions))
		return SaveErrorMsg(nRes);

	// if remote path not specified or is a folder then allow browsing unless bAllowDialog == FALSE
	CFRArray aRemoteFiles;

	if (nRes == RMERR_SUCCESS)
	{
		sRemotePath.TrimLeft();
		sRemotePath.TrimRight();
		
		if (sRemotePath.IsEmpty())
			nRes = GetRemotePaths(aRemoteFiles, aLocalFiles, dwOptions);
		else
		{
			sRemotePath.Replace('\\', '/');

			// if the remote path ends in '/' then its a folder 
			// so we must still get the file
			if (sRemotePath.GetAt(sRemotePath.GetLength() - 1) == '/')
				nRes = GetRemotePaths(aRemoteFiles, aLocalFiles, dwOptions, sRemotePath);

			else // its a file
				aRemoteFiles.Add(FILERESULT(sRemotePath));
		}
	}

	if (nRes == RMERR_SUCCESS && aRemoteFiles.GetSize())
	{
		CProgressDlg progDlg;

		if (progDlg.Create("数据传输", NULL, m_pParent))
		{
			if (m_pParent)
				m_pParent->EnableWindow(FALSE);

			int nFileCount = aRemoteFiles.GetSize();

			for (int nFile = 0; nFile < nFileCount; nFile++)
			{
				if (nFileCount > 1)
				{
					CString sDlgTitle;
					sDlgTitle.Format("Upload Progress (%d of %d)", nFile + 1, nFileCount);
					progDlg.SetCaption(sDlgTitle);
				}

				RMERR nFileRes = UploadFile(aLocalFiles[nFile], &aRemoteFiles[nFile], dwOptions, &progDlg);

				// handle single files
				if (nFileCount == 1)
				{
					nRes = nFileRes;
					sRemotePath = aRemoteFiles[0].sFilePath;
					sLocalPath = aLocalFiles[0];
				}
			}

			if (m_pParent)
				m_pParent->EnableWindow(TRUE);
		}
	}

	CloseConnection();

	return SaveErrorMsg(nRes, sLocalPath, sRemotePath);
}

// for upload
RMERR CRemoteFile::GetRemotePaths(CFRArray& aRemoteFiles, const CStringArray& aLocalFiles, DWORD dwOptions, LPCTSTR szRemoteDir)
{
	aRemoteFiles.RemoveAll();

	BOOL bAllowDialog = (dwOptions & RMO_ALLOWDIALOG);

	if (!bAllowDialog)
		return RMERR_REMOTEPATH;

	// if multiple files are being uploaded then display folder dialog
	else if (aLocalFiles.GetSize() > 1)
	{
		CRemoteFileDialog dialog(m_pConnection, m_sServer);
		CString sFolder(szRemoteDir);
		
	//	if (sFolder.IsEmpty())
	//		sFolder = AfxGetApp()->GetProfileString("RemoteSettings\\LastUploadFolder", m_sServer);
		
		if (dialog.DoModal(RFD_UPLOAD | RFD_FOLDERSELECT, sFolder) == IDOK && dialog.GetPathCount())
		{
			sFolder = dialog.GetFirstPath();
			ASSERT (!sFolder.IsEmpty());

			for (int nFile = 0; nFile < aLocalFiles.GetSize(); nFile++)
			{
				char szFilename[_MAX_FNAME], szExt[_MAX_EXT];
				_splitpath(aLocalFiles[nFile], NULL, NULL, szFilename, szExt);

				char szRemotePath[MAX_PATH];
				_makepath(szRemotePath, NULL, sFolder, szFilename, szExt);

				aRemoteFiles.Add(FILERESULT(szRemotePath));
			}

			AfxGetApp()->WriteProfileString("RemoteSettings\\LastUploadFolder", m_sServer, sFolder);
		}
		else
			return RMERR_USERCANCELLED;
	}
	else if (aLocalFiles.GetSize() == 1)
	{
		CString sFolder = AfxGetApp()->GetProfileString("RemoteSettings\\LastUploadFolder", m_sServer);
		CRemoteFileDialog dialog(m_pConnection, m_sServer, NULL, sFolder);

		// base remote name on local name
		char szFilename[_MAX_FNAME], szExt[_MAX_EXT];
		_splitpath(aLocalFiles[0], NULL, NULL, szFilename, szExt);
		lstrcat(szFilename, szExt);
		
		if (dialog.DoModal(RFD_UPLOAD, szFilename) == IDOK)
			dialog.GetPaths(aRemoteFiles);
		else
			return RMERR_USERCANCELLED;
	}
	else
		return RMERR_LOCALPATH;
	
	return RMERR_SUCCESS;
}

// for download
RMERR CRemoteFile::GetRemotePaths(CFRArray& aRemoteFiles, DWORD dwOptions, LPCTSTR szFilter, LPCTSTR szRemoteDir)
{
	aRemoteFiles.RemoveAll();

	BOOL bAllowDialog = (dwOptions & RMO_ALLOWDIALOG);

	if (!bAllowDialog)
		return RMERR_REMOTEPATH;
	else
	{
		// get last folder browsed for this server
		CString sFolder(szRemoteDir);
		
	//	if (sFolder.IsEmpty())
	//		sFolder = AfxGetApp()->GetProfileString("RemoteSettings\\LastDownloadFolder", m_sServer);

		// browse for file(s)
		CWaitCursor cursor;
		CRemoteFileDialog dialog(m_pConnection, m_sServer, szFilter, sFolder);
		DWORD dwOptions = RFD_NONAVIGATE | RFD_DOWNLOAD | RFD_FILEMUSTEXIST | RFD_MULTISELECT;
		
		if (dialog.DoModal(dwOptions) == IDOK && dialog.GetPathCount())
		{
			dialog.GetPaths(aRemoteFiles);

			AfxGetApp()->WriteProfileString("RemoteSettings\\LastDownloadFolder", m_sServer, sFolder);
		}
		else
			return RMERR_USERCANCELLED;
	}
	
	return RMERR_SUCCESS;
}

RMERR CRemoteFile::GetLocalPaths(CStringArray& aLocalFiles, BOOL& bTemp, const CFRArray& aRemoteFiles, DWORD dwOptions, LPCTSTR szLocalDir)
{
	aLocalFiles.RemoveAll();

	BOOL bKeepExt = (dwOptions & RMO_KEEPEXTENSION);
	BOOL bAllowDialog = (dwOptions & RMO_ALLOWDIALOG);
	bTemp = (dwOptions & RMO_USETEMPFILE);

	if (bTemp)
	{
		for (int nFile = 0; nFile < aRemoteFiles.GetSize(); nFile++)
		{
			CString sLocalPath = GetTempPath(aRemoteFiles[nFile].sFileName, bKeepExt);

			if (!sLocalPath.IsEmpty())
				aLocalFiles.Add(sLocalPath);
			else
			{
				aLocalFiles.RemoveAll();
				return RMERR_LOCALPATH;
			}
		}
	}
	else if (bAllowDialog)
	{
		CString sFolder(szLocalDir);
		BOOL bCreatePath = (dwOptions & RMO_CREATEDOWNLOADDIR);
		
		if (!ValidateLocalFolder(sFolder, bCreatePath))
			sFolder = AfxGetApp()->GetProfileString("LocalSettings\\LastDownloadFolder", m_sServer);
		
		// if multiple remote files are being downloaded then display folder dialog
		if (aRemoteFiles.GetSize() > 1)
		{
			CFolderDialog dialog("选择文件存放位置", 
									sFolder, m_pParent);

			if (dialog.DoModal() == IDOK)
			{
				CString sFolder = dialog.GetFolderPath();
				char szLocalPath[MAX_PATH];

				for (int nFile = 0; nFile < aRemoteFiles.GetSize(); nFile++)
				{
					_makepath(szLocalPath, NULL, sFolder, aRemoteFiles[nFile].sFileName, NULL);
					aLocalFiles.Add(szLocalPath);
				}

				// save folder location for next time
				AfxGetApp()->WriteProfileString("LocalSettings\\LastDownloadFolder", m_sServer, sFolder);
			}
			else
				return RMERR_USERCANCELLED;
		}
		else if (aRemoteFiles.GetSize() == 1)
		{
			CFileDialog dialog(FALSE);

			dialog.m_ofn.lpstrFilter = ALLFILES;
			dialog.m_ofn.Flags &= ~OFN_ALLOWMULTISELECT;

			lstrcpy(dialog.m_ofn.lpstrFile, aRemoteFiles[0].sFileName);
			dialog.m_ofn.nMaxFile = MAX_PATH;

			if (!sFolder.IsEmpty())
				dialog.m_ofn.lpstrInitialDir = sFolder;

			if (dialog.DoModal() == IDOK)
				aLocalFiles.Add(dialog.GetPathName());
			else
				return RMERR_USERCANCELLED;
		}
	}
	else
		return RMERR_LOCALPATH;

	return RMERR_SUCCESS;
}

// for upload
RMERR CRemoteFile::GetLocalPaths(CStringArray& aLocalFiles, DWORD dwOptions, LPCTSTR szLocalDir)
{
	aLocalFiles.RemoveAll();

	BOOL bAllowDialog = (dwOptions & RMO_ALLOWDIALOG);

	if (!bAllowDialog)
		return RMERR_LOCALPATH;
	else
	{
		// get last folder browsed for this server
		CString sFolder(szLocalDir);
		
		if (!ValidateLocalFolder(sFolder, FALSE))
			sFolder = AfxGetApp()->GetProfileString("LocalSettings\\LastUploadFolder", m_sServer);

		// browse for file(s)
		CFileDialog dialog(TRUE);

		const UINT BUFSIZE = 1024 * 5;
		static char FILEBUF[BUFSIZE] = { 0 };
				
		dialog.m_ofn.lpstrFilter = ALLFILES;
		dialog.m_ofn.lpstrTitle = "Select Files to Upload";
		dialog.m_ofn.lpstrFile = FILEBUF;
		dialog.m_ofn.nMaxFile = BUFSIZE;
		dialog.m_ofn.Flags |= OFN_ALLOWMULTISELECT;

		if (!sFolder.IsEmpty())
			dialog.m_ofn.lpstrInitialDir = sFolder;
		
		if (dialog.DoModal() == IDOK)
		{
			POSITION pos = dialog.GetStartPosition();

			while (pos)
			{
				CString sLocalPath = dialog.GetNextPathName(pos);

				if (dwOptions & RMO_LOWERCASEPATHS)
					sLocalPath.MakeLower();

				aLocalFiles.Add(sLocalPath);
			}

			AfxGetApp()->WriteProfileString("LocalSettings\\LastUploadFolder", m_sServer, sFolder);
		}
		else
			return RMERR_USERCANCELLED;
	}

	return RMERR_SUCCESS;
}

BOOL CRemoteFile::ValidateLocalFolder(CString& sFolder, BOOL bAllowCreation)
{
	DWORD dwAttrib = GetFileAttributes(sFolder);
	
	if (dwAttrib == 0xffffffff) // no such path
	{
		sFolder.TrimLeft();
		sFolder.TrimRight();

		if (bAllowCreation && sFolder.GetLength())
		{
			// if the folder ends in a backslash then create as is
			sFolder.Replace('/', '\\');

			if (sFolder.GetAt(sFolder.GetLength() - 1) == '\\')
				CreateFolder(sFolder);

			else // split first
			{
				char szDrive[_MAX_DRIVE], szFolder[MAX_PATH];
				_splitpath(sFolder, szDrive, szFolder, NULL, NULL);

				CString sTemp;
				sTemp.Format("%s%s", szDrive, szFolder);

				if (CreateFolder(sTemp))
					sFolder = sTemp;
			}

			return (GetFileAttributes(sFolder) != 0xffffffff);
		}
		else
			return FALSE;
	}
	else
		return (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

CString CRemoteFile::GetTempPath(const CString& sRemotePath, BOOL bKeepExt)
{
	CString sTempPath;
	char szTempDir[MAX_PATH], szTempFile[MAX_PATH];
	
	if (::GetTempPath(MAX_PATH, szTempDir) && ::GetTempFileName(szTempDir, "rmf", 0, szTempFile))
	{
		sTempPath = szTempFile;
		
		if (bKeepExt)
		{
			char szExt[_MAX_EXT];
			_splitpath(sRemotePath, NULL, NULL, NULL, szExt);
			sTempPath.Replace(".tmp", szExt);
			
			// delete the file which windows created
			::DeleteFile(szTempFile);
		}
	}
	
	return sTempPath;
}

BOOL CRemoteFile::EstablishConnection(RMERR& nRes, DWORD dwOptions)
{
	CWaitCursor cursor;
	CloseConnection();

	BOOL bAllowDialog = (dwOptions & RMO_ALLOWDIALOG);
	BOOL bAnonLogin = (dwOptions & RMO_ANONYMOUSLOGIN);

	// try to establish a connection until we succeed or the user cancels
	// or bAllowDialog == FALSE and we fail
	while (TRUE)
	{
		if (!m_sServer.IsEmpty() && (bAnonLogin || (!m_sUsername.IsEmpty() && !m_sPassword.IsEmpty())))
		{
			try
			{
				if (bAnonLogin)
					m_pConnection = GetFtpConnection(m_sServer);
				else
					m_pConnection = GetFtpConnection(m_sServer, m_sUsername, m_sPassword);

				if (m_pConnection)
				{
					nRes = RMERR_SUCCESS;
					return TRUE;
				}
			}
			catch(CInternetException* e)
			{
				nRes = RMERR_CONNECTING;
				m_dwInternetErr = e->m_dwError;
				e->Delete();
			}
			catch (...)
			{
				CloseConnection();
				nRes = RMERR_CONNECTING;
			}

			// must mean failure if we got here
			if (bAllowDialog)
			{
				CString sMessage;
				sMessage.Format("连结 %s 失败.\n\n是否重新设置远程服各器?        ", m_sServer);
				
				if (AfxMessageBox(sMessage, MB_YESNO) == IDNO)
				{
					nRes = RMERR_USERCANCELLED;
					return FALSE;
				}

				if (!DoServerDlg(dwOptions, bAnonLogin))
				{
					nRes = RMERR_USERCANCELLED;
					return FALSE;
				}
			}
			else
				return FALSE; // can't try again
		}
		else if (bAllowDialog)
		{
			if (!DoServerDlg(dwOptions, bAnonLogin))
			{
				nRes = RMERR_USERCANCELLED;
				return FALSE;
			}
		}
		else
		{
			nRes = RMERR_SERVERDETAILS;
			return FALSE;
		}
	}

	return FALSE;
}

BOOL CRemoteFile::DoServerDlg(DWORD dwOptions, BOOL& bAnonLogin)
{
	AL_TYPE nAnonLogin = (dwOptions & RMO_NOANONYMOUSLOGIN) ? ANONLOGIN_HIDE : 
						(bAnonLogin ? ANONLOGIN_YES : ANONLOGIN_AUTO);

	CServerDlg dialog(m_sServer, m_sUsername, m_sPassword, ANONLOGIN_HIDE);//nAnonLogin);
				
	if (dialog.DoModal() == IDOK)
	{
		m_sServer = dialog.GetServer();
		m_sUsername = dialog.GetUsername();
		m_sPassword = dialog.GetPassword();
		bAnonLogin = dialog.GetAnonynousLogin();
		return TRUE;
	}

	// else
	return FALSE;
}

void CRemoteFile::CloseConnection()
{
	if (m_pConnection)
		m_pConnection->Close();

	delete m_pConnection;
	m_pConnection = NULL;
}

DWORD CRemoteFile::GetRemoteFileSize(LPCTSTR szRemotePath)
{
	try
	{
		CFtpFileFind ff(m_pConnection);
		
		if (ff.FindFile(szRemotePath))
		{
			ff.FindNextFile();
			return ff.GetLength();
		}
	}
	catch(CInternetException* e)
	{
		m_dwInternetErr = e->m_dwError;
		e->Delete();
	}
	catch (...)
	{
	}

	return -1;
}

BOOL CRemoteFile::RemoteFileExists(LPCTSTR szRemotePath)
{
	try
	{
		CFtpFileFind ff(m_pConnection);
		
		if (ff.FindFile(szRemotePath))
			return TRUE;
	}
	catch(CInternetException* e)
	{
		e->Delete();
	}
	catch (...)
	{
	}

	return FALSE;
}

RMERR CRemoteFile::DownloadFile(const FILERESULT* pRemoteFile, LPCTSTR szToLocalPath, DWORD dwOptions, CProgressDlg* pDlg)
{
	if (!m_pConnection)
		return RMERR_NOTCONNECTED;

	// check for existing local file
	BOOL bLocalExists = (::GetFileAttributes(szToLocalPath) != 0xffffffff);

	if (bLocalExists && (dwOptions & RMO_CONFIRMOVERWRITE))
	{
		CString sMessage;
		sMessage.Format("本地已存在 %s\n\n是否覆盖现有文件?            ", szToLocalPath);

		HWND hWnd = m_pParent ? m_pParent->GetSafeHwnd() : NULL;

		if (MessageBox(hWnd, sMessage, "操作确认", MB_YESNO) == IDNO)
			return RMERR_USERCANCELLED;
	}

	// get remote file size if not specified
	DWORD dwFileSize = pRemoteFile->dwSize;

	if (!dwFileSize)
	{
		dwFileSize = GetRemoteFileSize(pRemoteFile->sFilePath);

		if (-1 == dwFileSize)
			return RMERR_REMOTEPATH; // probably doen't exist
	}

	// initialize progress dialog
	CProgressDlg progDlg;

	BOOL InitLink=FALSE;
	if(GetUsername()=="AnyOne")
		InitLink=TRUE;
	if (!pDlg && progDlg.Create("数据传输", NULL, m_pParent))
	{
		if (m_pParent)
			m_pParent->EnableWindow(FALSE);
		pDlg = &progDlg;
	}
	
	CString sDescription;
	if(!InitLink)
		sDescription.Format("下载 %s", pRemoteFile->sFilePath);
	else
		sDescription="\n连结服务器...";
	
	pDlg->SetDescription(sDescription);
	pDlg->Continue();

	// open remote file for input
	CWaitCursor cursor;
	CInternetFile* pFile = NULL;

	try
	{
		pFile = m_pConnection->OpenFile(pRemoteFile->sFilePath);
	}
	catch(CInternetException* e)
	{
		m_dwInternetErr = e->m_dwError;
		e->Delete();
	}
	catch(...)
	{
	}

	if (!pFile)
		return RMERR_OPENINGREMOTEFILETOREAD;

	// open local file for output
	RMERR nRes = RMERR_SUCCESS;
	CFile file;

	// make sure the parent folder is created first
	VERIFY (CreateFolderFromFilePath(szToLocalPath));

	if (file.Open(szToLocalPath, CFile::modeCreate | CFile::modeWrite))
	{
		// write out as we read in
		BYTE ucBuffer[BUFSIZE];
		DWORD dwBytesRead = 0, dwBytesWritten = 0;

		while (pDlg->Continue())
		{
			UINT nCount = 0;

			try
			{
				nCount = pFile->Read((LPVOID)ucBuffer, BUFSIZE);
			}
			catch(CInternetException* e)
			{
				nRes = RMERR_READINGREMOTEFILE;
				m_dwInternetErr = e->m_dwError;
				e->Delete();
			}
			catch (...)
			{
				nRes = RMERR_READINGREMOTEFILE;
				break;
			}

			if (nCount)
			{
				dwBytesRead += nCount;
				
				CString sProgress;
				
				if(!InitLink)
				{
					if (!dwFileSize)
						sProgress.Format("已传输 %0.1fKB ", dwBytesRead);
					else
					{
						sProgress.Format("已传输 %0.1fKb (%0.1f)", dwBytesRead / 1024.0, (dwFileSize - dwBytesRead) / 1024.0);
						pDlg->SetProgress(MulDiv(dwBytesRead, 100, dwFileSize));
					}
				}
				else
				{
					sProgress="初始数据库查询系统,稍候...";	
					pDlg->SetProgress(MulDiv(dwBytesRead, 100, dwFileSize));
				}

				pDlg->SetProgress(sProgress);

				try
				{
					file.Write(ucBuffer, nCount);

					dwBytesWritten += nCount;
				}
				catch (...)
				{
					nRes = RMERR_WRITINGLOCALFILE;
					break;
				}
			}
			
			if (nCount < BUFSIZE)
				break;
		}

		// if the download failed and the local file did not exist or 
		// we wrote to the file (so it will be corrupt) and we should delete failures
		// then cleanup
		if (nRes != RMERR_SUCCESS && (!bLocalExists || (dwBytesWritten && (dwOptions & RMO_DELETEFAILURES))))
			::DeleteFile(szToLocalPath);
	}
	else
		nRes = RMERR_OPENINGLOCALFILETOWRITE;

	// cleanup
	if (m_pParent && pDlg == &progDlg)
		m_pParent->EnableWindow(TRUE);

	if (!pDlg->Continue())
		nRes = RMERR_USERCANCELLED;

	if (pDlg == &progDlg)
		progDlg.DestroyWindow();

	file.Close();
	pFile->Close();
	delete pFile;


	return nRes;
}

RMERR CRemoteFile::UploadFile(LPCTSTR szFromLocalPath, const FILERESULT* pRemoteFile, DWORD dwOptions, CProgressDlg* pDlg)
{
	if (!m_pConnection)
		return RMERR_NOTCONNECTED;

	// open local file for input
	CFile file;

	if (!file.Open(szFromLocalPath, CFile::modeRead))
		return RMERR_OPENINGLOCALFILETOREAD;

	CString sRemotePath(pRemoteFile->sFilePath);

	// check for existing remote file
	if (dwOptions & RMO_LOWERCASEPATHS)
		sRemotePath.MakeLower();

	BOOL bRemoteExists = RemoteFileExists(sRemotePath);

	if (bRemoteExists && (dwOptions & RMO_CONFIRMOVERWRITE))
	{
		CString sMessage;
		sMessage.Format("远程已存在 %s\n\n是否覆盖现有文件?            ", sRemotePath);

		HWND hWnd = m_pParent ? m_pParent->GetSafeHwnd() : NULL;

		if (MessageBox(hWnd, sMessage, "操作确认", MB_YESNO) == IDNO)
			return RMERR_USERCANCELLED;
	}

	// create dialog if required
	CProgressDlg progDlg;

	if (!pDlg && progDlg.Create("文件传输", NULL, m_pParent))
	{
		if (m_pParent)
			m_pParent->EnableWindow(FALSE);

		pDlg = &progDlg;
	}

	CString sDescription;

	sDescription.Format("上传 %s", pRemoteFile->sFileName);
	pDlg->SetDescription(sDescription);

	// open remote file for ouput
	CWaitCursor cursor;
	CInternetFile* pFile = NULL;
	pDlg->Continue();

	try
	{
		pFile = m_pConnection->OpenFile(pRemoteFile->sFilePath, GENERIC_WRITE);
	}
	catch(CInternetException* e)
	{
		m_dwInternetErr = e->m_dwError;
		e->Delete();
	}
	catch(...)
	{
	}

	if (!pFile)
		return RMERR_OPENINGREMOTEFILETOWRITE;

	RMERR nRes = RMERR_SUCCESS;

	// write out as we read in
	BYTE ucBuffer[BUFSIZE];
	DWORD dwBytesRead = 0, dwBytesWritten = 0;
	DWORD dwFileSize = file.GetLength();

	while (pDlg->Continue())
	{
		UINT nCount = 0;
		
		try
		{
			nCount = file.Read((LPVOID)ucBuffer, BUFSIZE);
		}
		catch (...)
		{
			nRes = RMERR_READINGLOCALFILE;
			break;
		}

		if (nCount)
		{
			dwBytesRead += nCount;
			
			CString sProgress;
			sProgress.Format("已传输 %0.1fKb (%0.1f)", dwBytesRead / 1024.0, (dwFileSize - dwBytesRead) / 1024.0);
			
			pDlg->SetProgress(MulDiv(dwBytesRead, 100, dwFileSize));
			pDlg->SetProgress(sProgress);

			try
			{
				pFile->Write(ucBuffer, nCount);
			
				dwBytesWritten += nCount;
			}
			catch (CInternetException* e)
			{
				nRes = RMERR_WRITINGREMOTEFILE;
				m_dwInternetErr = e->m_dwError;
				e->Delete();
				break;
			}
			catch (...)
			{
				nRes = RMERR_WRITINGREMOTEFILE;
				break;
			}
		}
		
		if (nCount < BUFSIZE)
			break;
	}

	// if the upload failed and the remote file did not exist or 
	// (we wrote to the file (so it will be corrupt) and we should delete failures)
	// then cleanup
	if (nRes != RMERR_SUCCESS && (!bRemoteExists || (dwBytesWritten && (dwOptions & RMO_DELETEFAILURES))))
		m_pConnection->Remove(pRemoteFile->sFilePath);

	// cleanup
	if (m_pParent && pDlg == &progDlg)
		m_pParent->EnableWindow(TRUE);

	if (!pDlg->Continue())
		nRes = RMERR_USERCANCELLED;

	if (pDlg == &progDlg)
		progDlg.DestroyWindow();

	file.Close();
	pFile->Close();
	delete pFile;

	return nRes;
}

void CRemoteFile::SplitPath(LPCTSTR szFullRemotePath, CString& sServer, CString& sFile)
{
	DWORD dwDummy = 0;
	INTERNET_PORT nDummy = 0;

	AfxParseURL(szFullRemotePath, dwDummy, sServer, sFile, nDummy);
}

void CRemoteFile::ValidateOptions(DWORD& dwOptions, BOOL bDownload)
{
	// validate options
	if (bDownload)
	{
		if (!(dwOptions & RMO_USETEMPFILE))
			dwOptions &= ~RMO_KEEPEXTENSION;

		if (dwOptions & RMO_NOANONYMOUSLOGIN)
			dwOptions &= ~RMO_ANONYMOUSLOGIN;
	}
	else // upload
		dwOptions &= ~(RMO_USETEMPFILE | RMO_KEEPEXTENSION | RMO_CREATEDOWNLOADDIR);
}

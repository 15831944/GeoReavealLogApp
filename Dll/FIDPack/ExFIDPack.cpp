#include <io.h>
#include <direct.h>
#include "ExFIDPack.h"
#include ".\Shared\SelfExtractor.h"

void CallBack(int nMsgType, void *ExtractData, void *userData)
{
	CSEFileInfo* pData = NULL;
	int nProgress;
	switch(nMsgType)
	{
	case SFX_FILE_START:
		pData = static_cast<CSEFileInfo*>(ExtractData);
		break;
	case SFX_FILE_END:
		break;
	case SFX_ZIP_INFO:
		memcpy(&nProgress, ExtractData, sizeof(int));
		break;
	}
}

CExFIDPack::CExFIDPack(CString sFIDPackFile)
{
	m_FIDPackFile =sFIDPackFile;
}

CExFIDPack::~CExFIDPack ()
{
}

BOOL CExFIDPack::IsFIDPack ()
{
	CSelfExtractor	m_Extractor;
	if(m_Extractor.ReadTOC(m_FIDPackFile)==SUCCESS)
		return TRUE;
	else
		return FALSE;
}

BOOL CExFIDPack::ExtractPack (CString ExPath)
{
	CSelfExtractor	m_Extractor;
	if(m_Extractor.ReadTOC(m_FIDPackFile)==SUCCESS)	
	{		
		if(ExPath.Right (1)!='\\')
			ExPath+="\\";
		CWaitCursor wait;
		CString m_WellName=m_FIDPackFile.Mid(m_FIDPackFile.ReverseFind ('\\')+1) ;
		int xxx=m_WellName.Find (".");
		if(xxx>-1) m_WellName=m_WellName.Left (xxx);
		if(m_WellName.Find ("#")==-1) 
			m_WellName="#"+m_WellName;
		ExPath=ExPath+m_WellName;
		_mkdir(ExPath);
		int ret = m_Extractor.ExtractAll(ExPath, CallBack, static_cast<void*>(this));
		if(ret==SUCCESS)
			return TRUE;
	}
	return FALSE;
}
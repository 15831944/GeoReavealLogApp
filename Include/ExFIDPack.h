#if !defined _EXFIDPACK_H_
#define _EXFIDPACK_H_
void CallBack(int nMsgType, void *ExtractData, void *userData);

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif
/////////////////////////////////////////////////////////////////////
//
//    ��FID�Խ�ѹ���ݰ���
//    ��FID���ݼ�¼��ʽ��ѹ���û�ָ����Ŀ¼��
//
//    sFIDPackFile   
//    ��Ҫ��ѹFID���ݰ�����·����
//
//    IsFIDPack()
//    �ж��Ƿ�ΪFID�Խ�ѹ���ݰ�
//
//    ExtractPack(CString ExPath)
//    ��ѹFID���ݰ����������ߺ�����ĵ��� ExPath Ŀ¼��
//    
//    ExPath
//    ��ָ���Ľ�ѹĿ¼
//    ����Ϊ sFIDPackFile �����ļ���
//    
/////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CExFIDPack
{
public:
	CExFIDPack(CString sFIDPackFile);
	~CExFIDPack();
	CString m_FIDPackFile;
	BOOL IsFIDPack();
	BOOL ExtractPack(CString ExPath); 
};
#endif
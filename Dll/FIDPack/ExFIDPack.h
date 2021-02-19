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
//    将FID自解压数据包，
//    以FID数据记录格式解压到用户指定的目录下
//
//    sFIDPackFile   
//    需要解压FID数据包完整路径名
//
//    IsFIDPack()
//    判断是否为FID自解压数据包
//
//    ExtractPack(CString ExPath)
//    解压FID数据包中所有曲线和相关文档到 ExPath 目录下
//    
//    ExPath
//    户指定的解压目录
//    井名为 sFIDPackFile 的主文件名
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
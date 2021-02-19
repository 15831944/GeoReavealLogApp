// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#ifndef PI
#define PI 3.1415926257
#endif

#undef WINVER
#define WINVER 0x0500
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxodlgs.h>       // MFC OLE dialog classes

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#include <afxtempl.h>

#endif // _AFX_NO_AFXCMN_SUPPORT

#include "QGdiPlus.h"



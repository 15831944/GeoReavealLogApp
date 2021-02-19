// LOGSYS.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static AFX_EXTENSION_MODULE LOGSYSDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("LOGSYS.DLL Initializing!\n");
		// Extension DLL one-time initialization
		AfxInitExtensionModule(LOGSYSDLL, hInstance);
		// Insert this DLL into the resource chain
		new CDynLinkLibrary(LOGSYSDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("LOGSYS.DLL Terminating!\n");
	}
	return 1;   // ok
}

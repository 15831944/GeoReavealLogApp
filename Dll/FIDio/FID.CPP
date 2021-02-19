// FID.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>
#include <io.h>
#include "FIDio.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
BOOL Register();
void GetNetwareCode(char *NetwareCode);
void NetwareCode2LoginCode(char *HardwareCode,char *LoginCode);

static AFX_EXTENSION_MODULE FIDDLL = { NULL, NULL };
extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{			
//	if(!Register())
//		return 0;
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("FIDio.DLL Initializing!\n");
		// Extension DLL one-time initialization
		AfxInitExtensionModule(FIDDLL, hInstance);
		// Insert this DLL into the resource chain
		new CDynLinkLibrary(FIDDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("FIDio.DLL Terminating!\n");
	}

	return 1;   // ok
}
// PreView.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include <afxdllx.h>
#include <io.h>
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE PreViewDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// ���ʹ�� lpReserved���뽫���Ƴ�
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("PreView.DLL ���ڳ�ʼ����\n");
		
		// ��չ DLL һ���Գ�ʼ��
		if (!AfxInitExtensionModule(PreViewDLL, hInstance))
			return 0;

		// ���� DLL ���뵽��Դ����
		// ע��: �������չ DLL ��
		//  MFC ���� DLL(�� ActiveX �ؼ�)��ʽ���ӵ�
		//  �������� MFC Ӧ�ó������ӵ�������Ҫ
		//  �����д� DllMain ���Ƴ������������һ��
		//  �Ӵ���չ DLL �����ĵ����ĺ����С�ʹ�ô���չ DLL ��
		//  ���� DLL Ȼ��Ӧ��ʽ
		//  ���øú����Գ�ʼ������չ DLL������
		//  CDynLinkLibrary ���󲻻ḽ�ӵ�
		//  ���� DLL ����Դ���������������ص�
		//  ���⡣

		new CDynLinkLibrary(PreViewDLL);

		char szPath[MAX_PATH];
		GetTempPath(MAX_PATH,szPath);
		CString szStr;
		szStr.Format("%s\\xLogtemp\\",szPath);
		_mkdir(szStr);
		//��ջ�����
		struct _finddata_t c_file;
		intptr_t hFile;
		if((hFile = _findfirst(szStr+"*.*", &c_file ))!=-1L)
		{
			_findnext( hFile, &c_file );
			while( _findnext( hFile, &c_file ) == 0 )
				remove(szStr+c_file.name);    
		}
		_findclose( hFile );
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("PreView.DLL ������ֹ��\n");

		// �ڵ�����������֮ǰ��ֹ�ÿ�
		AfxTermExtensionModule(PreViewDLL);
	}
	return 1;   // ȷ��
}

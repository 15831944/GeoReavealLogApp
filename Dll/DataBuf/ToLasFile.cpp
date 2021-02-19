// ToLasFile.cpp: implementation of the ToLasFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToLasFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToLasFile::CToLasFile()
{

}

CToLasFile::~CToLasFile()
{

}
bool CToLasFile::ToLasFile(CString SourceFile,CString ObjectPath,int m_nCurvesRadio,CString sInputFormat,int m_nConvEncode,float m_FixRlev,FILE *fo)
{
	fp=fo;
	bool bResult=false;
	//�����ļ�����ȷ��ת������
	if(sInputFormat.CompareNoCase("FID")==0)
	{
		bResult=FidToLas.FidToLas(SourceFile,ObjectPath,m_nCurvesRadio,m_nConvEncode,m_FixRlev,fp);
	}
	return bResult;
}
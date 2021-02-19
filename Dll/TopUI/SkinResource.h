// SkinResource.h: interface for the CSkinResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINRESOURCE_H__6546AFFB_72B3_42A4_B312_499C6685D706__INCLUDED_)
#define AFX_SKINRESOURCE_H__6546AFFB_72B3_42A4_B312_499C6685D706__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SkinButton.h"

class CSkinResource  
{
	CSkinButtonResource m_btnres;
public:
	CSkinResource();
	BOOL	LoadSkin( const char *skinfile );
	
	virtual ~CSkinResource();

};

#endif // !defined(AFX_SKINRESOURCE_H__6546AFFB_72B3_42A4_B312_499C6685D706__INCLUDED_)

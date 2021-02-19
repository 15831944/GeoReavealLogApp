// Grapher.h: interface for the CGrapher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHER_H__7C1A5C26_0372_4E7B_BFA1_7BF50127E64E__INCLUDED_)
#define AFX_GRAPHER_H__7C1A5C26_0372_4E7B_BFA1_7BF50127E64E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseException.h"

// PI value
const float PI = 3.14159f;

class CGrapherException : public CBaseException
{
public:
	CGrapherException() throw() : CBaseException(_T("GrapherException")) {}
	CGrapherException(std::string message) throw() : CBaseException(message) {}
	CGrapherException(std::string message, std::string location) throw() : CBaseException(message, location) {}
};

#endif // !defined(AFX_GRAPHER_H__7C1A5C26_0372_4E7B_BFA1_7BF50127E64E__INCLUDED_)

#pragma once

// Add GDI+ support to MFC or WTL application.
//
// Include this file in StdAfx.h
//
// MFC: Add a QGdiPlus variable to your application class to start and stop GDI+.
// ATL: Create a QGdiPlus local variable in _tWinMain.
// Constructor starts, destructor stops.

// (C) 2003, Sjaak Priester, Amsterdam
// 2005/02/12: Bug fix, thanks to Joris van der Pol.



// Ensure that GdiPlus header files work properly with MFC DEBUG_NEW and STL header files.
// Q317799: PRB: Microsoft Foundation Classes DEBUG_NEW Does Not Work with GDI+

#define iterator _iterator

#ifdef _DEBUG

#include <map>

namespace QGdiPlusDebug {

	struct ObjData
	{
		ObjData(size_t s, LPCTSTR f = _T(""), int ln = 0): size(s), file(f), line(ln)	{}
		size_t size;
		CString file;
		int line;
	};

	__declspec(selectany) std::map<void *, ObjData> Objects;
};

using namespace QGdiPlusDebug;

namespace Gdiplus
{
	namespace DllExports
	{
		#include <GdiplusMem.h>
	};

	#ifndef _GDIPLUSBASE_H
	#define _GDIPLUSBASE_H
	class GdiplusBase
	{
		public:
			void (operator delete)(void* in_pVoid)
			{
				Objects.erase(in_pVoid);
				DllExports::GdipFree(in_pVoid);
			}

			void* (operator new)(size_t in_size)
			{
				void * p = DllExports::GdipAlloc(in_size);
				Objects.insert(std::pair<void *, ObjData>(p, ObjData(in_size)));
				return p;
			}

			void (operator delete[])(void* in_pVoid)
			{
				Objects.erase(in_pVoid);
				DllExports::GdipFree(in_pVoid);
			}

			void* (operator new[])(size_t in_size)
			{
				void * p = DllExports::GdipAlloc(in_size);
				Objects.insert(std::pair<void *, ObjData>(p, ObjData(in_size)));
				return p;
			}

			void * (operator new)(size_t nSize, LPCSTR lpszFileName, int nLine)
			{
				void * p = DllExports::GdipAlloc(nSize);
				Objects.insert(std::pair<void *, ObjData>(p, ObjData(nSize, lpszFileName, nLine)));
				return p;
			}

			void operator delete(void* p, LPCSTR lpszFileName, int nLine)
			{
				Objects.erase(p);
				DllExports::GdipFree(p);
			}

		};
	#endif // #ifndef _GDIPLUSBASE_H
}
#endif // #ifdef _DEBUG

#include <gdiplus.h>
#ifdef _MFC_VER
#include <afx.h>
#endif
#undef iterator

using namespace Gdiplus;
#pragma comment (lib, "Gdiplus.lib")

class QGdiPlus
{
public:
	QGdiPlus(): m_Token(0)	{ Gdiplus::GdiplusStartupInput input; Gdiplus::GdiplusStartup(& m_Token, & input, NULL); }
	~QGdiPlus()
	{
		Gdiplus::GdiplusShutdown(m_Token);
#ifdef _DEBUG
#ifdef _MFC_VER
		if (Objects.size() > 0)
		{
			TRACE1(_T("GdiPlus Memory Leaks %d objects!\nDumping GDI+ objects ->\n"), Objects.size());
			for (std::map<void *, ObjData>::const_iterator it = Objects.begin(); it != Objects.end(); ++it)
			{
				TRACE2(_T("%s(%d) : GDI+ object "), it->second.file , it->second.line);
				TRACE2(_T("at 0x%08x, %d bytes long.\n"), it->first , it->second.size);
			}
		}
#else
#ifdef _ATL_VER
		if (Objects.size() > 0)
		{
			ATLTRACE2(_T("GdiPlus Memory Leaks %d objects!\nDumping GDI+ objects ->\n"), Objects.size());
			for (std::map<void *, ObjData>::const_iterator it = Objects.begin(); it != Objects.end(); ++it)
			{
				ATLTRACE2(_T("%s(%d) : GDI+ object "), it->second.file , it->second.line);
				ATLTRACE2(_T("at 0x%08x, %d bytes long.\n"), it->first , it->second.size);
			}
		}
#endif
#endif
#endif
	}
private:
	// The token we get from GDI+
	ULONG_PTR m_Token;
};

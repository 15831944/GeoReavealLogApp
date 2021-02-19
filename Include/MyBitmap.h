// MyBitmap.h: interface for the CMyBitmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYBITMAP_H__73EF928E_90A7_461A_B879_7E3007684679__INCLUDED_)
#define AFX_MYBITMAP_H__73EF928E_90A7_461A_B879_7E3007684679__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EnBitmap.h"

class AFX_EXT_CLASS CMyBitmap : public CEnBitmap  
{
public:
	BOOL Draw( CDC *pDC, LPRECT r);
	//draw sub bmp to special point
	BOOL Draw( CDC *pDC, int x, int y, LPRECT sr );
	BOOL Draw( CDC *pDC, int x, int y, LPRECT sr, COLORREF colTrans, BOOL bTrans );

	BOOL StretchDraw(CDC *pDC, LPRECT tr, LPRECT sr );
	BOOL StretchDraw(CDC *pDC, LPRECT r);

	int	 Width()
	{
		return GetWidth();
	}
		
	int	 GetWidth()
	{
		BITMAP bm;
		memset( &bm, 0, sizeof(bm) );
		GetBitmap(&bm);
		return bm.bmWidth;
	}

	int	 Height()
	{
		return GetHeight();
	}
	
	int	 GetHeight()
	{
		BITMAP bm;
		memset( &bm, 0, sizeof(bm) );
		GetBitmap(&bm);
		return bm.bmHeight;
	}
	
	CMyBitmap();
	virtual ~CMyBitmap();


	BOOL Attach( HBITMAP hbmp )
	{
		return CBitmap::Attach( hbmp );
	}

	BOOL LoadBitmap(LPCTSTR szFilename) 
	{ 
		DeleteObject();
		return LoadImage(szFilename);
	}

	BOOL DrawTransparent(CDC * pDC, int x, int y, COLORREF crColour);	
	HRGN CreateRgnFromFile( COLORREF color );
};

#endif // !defined(AFX_MYBITMAP_H__73EF928E_90A7_461A_B879_7E3007684679__INCLUDED_)

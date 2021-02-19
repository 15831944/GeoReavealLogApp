//  dibapi.cpp
//
//  Source file for Device-Independent Bitmap (DIB) API.  Provides
//  the following functions:
//
//  PaintDIB()          - Painting routine for a DIB
//  CreateDIBPalette()  - Creates a palette from a DIB
//  FindDIBBits()       - Returns a pointer to the DIB bits
//  DIBWidth()          - Gets the width of the DIB
//  DIBHeight()         - Gets the height of the DIB
//  PaletteSize()       - Gets the size required to store the DIB's palette
//  DIBNumColors()      - Calculates the number of colors
//                        in the DIB's color table
//  CopyHandle()        - Makes a copy of the given global memory block
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1997 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "dibapi.h"
#include <io.h>
#include <errno.h>
#include <mmsystem.h> 
/*************************************************************************
 *
 * PaintDIB()
 *
 * Parameters:
 *
 * HDC hDC          - DC to do output to
 *
 * LPRECT lpDCRect  - rectangle on DC to do output to
 *
 * HDIB hDIB        - handle to global memory with a DIB spec
 *                    in it followed by the DIB bits
 *
 * LPRECT lpDIBRect - rectangle of DIB to output into lpDCRect
 *
 * CPalette* pPal   - pointer to CPalette containing DIB's palette
 *
 * Return Value:
 *
 * BOOL             - TRUE if DIB was drawn, FALSE otherwise
 *
 * Description:
 *   Painting routine for a DIB.  Calls StretchDIBits() or
 *   SetDIBitsToDevice() to paint the DIB.  The DIB is
 *   output to the specified DC, at the coordinates given
 *   in lpDCRect.  The area of the DIB to be output is
 *   given by lpDIBRect.
 *
 ************************************************************************/

BOOL WINAPI PaintDIB(HDC     hDC,
					LPRECT  lpDCRect,
					HDIB    hDIB,
					LPRECT  lpDIBRect,
					CPalette* pPal)
{
	LPSTR    lpDIBHdr;            // Pointer to BITMAPINFOHEADER
	LPSTR    lpDIBBits;           // Pointer to DIB bits
	BOOL     bSuccess=FALSE;      // Success/fail flag
	HPALETTE hPal=NULL;           // Our DIB's palette
	HPALETTE hOldPal=NULL;        // Previous palette

	/* Check for valid DIB handle */
	if (hDIB == NULL)
		return FALSE;

	/* Lock down the DIB, and get a pointer to the beginning of the bit
	 *  buffer
	 */
	lpDIBHdr  = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);
	lpDIBBits = ::FindDIBBits(lpDIBHdr);

	// Get the DIB's palette, then select it into DC
	if (pPal != NULL)
	{
		hPal = (HPALETTE) pPal->m_hObject;

		// Select as background since we have
		// already realized in forground if needed
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);
	}

	/* Make sure to use the stretching mode best for color pictures */
	::SetStretchBltMode(hDC, COLORONCOLOR);

	/* Determine whether to call StretchDIBits() or SetDIBitsToDevice() */
	if ((RECTWIDTH(lpDCRect)  == RECTWIDTH(lpDIBRect)) &&
	   (RECTHEIGHT(lpDCRect) == RECTHEIGHT(lpDIBRect)))
		bSuccess = ::SetDIBitsToDevice(hDC,                    // hDC
								   lpDCRect->left,             // DestX
								   lpDCRect->top,              // DestY
								   RECTWIDTH(lpDCRect),        // nDestWidth
								   RECTHEIGHT(lpDCRect),       // nDestHeight
								   lpDIBRect->left,            // SrcX
								   (int)DIBHeight(lpDIBHdr) -
									  lpDIBRect->top -
									  RECTHEIGHT(lpDIBRect),   // SrcY
								   0,                          // nStartScan
								   (WORD)DIBHeight(lpDIBHdr),  // nNumScans
								   lpDIBBits,                  // lpBits
								   (LPBITMAPINFO)lpDIBHdr,     // lpBitsInfo
								   DIB_RGB_COLORS);            // wUsage
   else
	  bSuccess = ::StretchDIBits(hDC,                          // hDC
							   lpDCRect->left,                 // DestX
							   lpDCRect->top,                  // DestY
							   RECTWIDTH(lpDCRect),            // nDestWidth
							   RECTHEIGHT(lpDCRect),           // nDestHeight
							   lpDIBRect->left,                // SrcX
							   lpDIBRect->top,                 // SrcY
							   RECTWIDTH(lpDIBRect),           // wSrcWidth
							   RECTHEIGHT(lpDIBRect),          // wSrcHeight
							   lpDIBBits,                      // lpBits
							   (LPBITMAPINFO)lpDIBHdr,         // lpBitsInfo
							   DIB_RGB_COLORS,                 // wUsage
							   SRCCOPY);                       // dwROP

   ::GlobalUnlock((HGLOBAL) hDIB);

	/* Reselect old palette */
	if (hOldPal != NULL)
	{
		::SelectPalette(hDC, hOldPal, TRUE);
	}

   return bSuccess;
}

/*************************************************************************
 *
 * CreateDIBPalette()
 *
 * Parameter:
 *
 * HDIB hDIB        - specifies the DIB
 *
 * Return Value:
 *
 * HPALETTE         - specifies the palette
 *
 * Description:
 *
 * This function creates a palette from a DIB by allocating memory for the
 * logical palette, reading and storing the colors from the DIB's color table
 * into the logical palette, creating a palette from this logical palette,
 * and then returning the palette's handle. This allows the DIB to be
 * displayed using the best possible colors (important for DIBs with 256 or
 * more colors).
 *
 ************************************************************************/


BOOL WINAPI CreateDIBPalette(HDIB hDIB, CPalette* pPal)
{
	LPLOGPALETTE lpPal;      // pointer to a logical palette
	HANDLE hLogPal;          // handle to a logical palette
	HPALETTE hPal = NULL;    // handle to a palette
	int i;                   // loop index
	WORD wNumColors;         // number of colors in color table
	LPSTR lpbi;              // pointer to packed-DIB
	LPBITMAPINFO lpbmi;      // pointer to BITMAPINFO structure (Win3.0)
	LPBITMAPCOREINFO lpbmc;  // pointer to BITMAPCOREINFO structure (old)
	BOOL bWinStyleDIB;       // flag which signifies whether this is a Win3.0 DIB
	BOOL bResult = FALSE;

	/* if handle to DIB is invalid, return FALSE */

	if (hDIB == NULL)
	  return FALSE;

   lpbi = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);

   /* get pointer to BITMAPINFO (Win 3.0) */
   lpbmi = (LPBITMAPINFO)lpbi;

   /* get pointer to BITMAPCOREINFO (old 1.x) */
   lpbmc = (LPBITMAPCOREINFO)lpbi;

   /* get the number of colors in the DIB */
   wNumColors = ::DIBNumColors(lpbi);

   if (wNumColors != 0)
   {
		/* allocate memory block for logical palette */
		hLogPal = ::GlobalAlloc(GHND, sizeof(LOGPALETTE)
									+ sizeof(PALETTEENTRY)
									* wNumColors);

		/* if not enough memory, clean up and return NULL */
		if (hLogPal == 0)
		{
			::GlobalUnlock((HGLOBAL) hDIB);
			return FALSE;
		}

		lpPal = (LPLOGPALETTE) ::GlobalLock((HGLOBAL) hLogPal);

		/* set version and number of palette entries */
		lpPal->palVersion = PALVERSION;
		lpPal->palNumEntries = (WORD)wNumColors;

		/* is this a Win 3.0 DIB? */
		bWinStyleDIB = IS_WIN30_DIB(lpbi);
		for (i = 0; i < (int)wNumColors; i++)
		{
			if (bWinStyleDIB)
			{
				int j=lpbmi->bmiColors[i].rgbRed;
				lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;
				j=lpbmi->bmiColors[i].rgbGreen;
				lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
				j=lpbmi->bmiColors[i].rgbBlue;
				lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;
				lpPal->palPalEntry[i].peFlags = 0;
			}
			else
			{
				lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;
				lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;
				lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;
				lpPal->palPalEntry[i].peFlags = 0;
			}
		}

		/* create the palette and get handle to it */
		bResult = pPal->CreatePalette(lpPal);
		::GlobalUnlock((HGLOBAL) hLogPal);
		::GlobalFree((HGLOBAL) hLogPal);
	}

	::GlobalUnlock((HGLOBAL) hDIB);

	return bResult;
}

/*************************************************************************
 *
 * FindDIBBits()
 *
 * Parameter:
 *
 * LPSTR lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * LPSTR            - pointer to the DIB bits
 *
 * Description:
 *
 * This function calculates the address of the DIB's bits and returns a
 * pointer to the DIB bits.
 *
 ************************************************************************/


LPSTR WINAPI FindDIBBits(LPSTR lpbi)
{
	return (lpbi + *(LPDWORD)lpbi + ::PaletteSize(lpbi));
}


/*************************************************************************
 *
 * DIBWidth()
 *
 * Parameter:
 *
 * LPSTR lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * DWORD            - width of the DIB
 *
 * Description:
 *
 * This function gets the width of the DIB from the BITMAPINFOHEADER
 * width field if it is a Windows 3.0-style DIB or from the BITMAPCOREHEADER
 * width field if it is an other-style DIB.
 *
 ************************************************************************/


DWORD WINAPI DIBWidth(LPSTR lpDIB)
{
	LPBITMAPINFOHEADER lpbmi;  // pointer to a Win 3.0-style DIB
	LPBITMAPCOREHEADER lpbmc;  // pointer to an other-style DIB

	/* point to the header (whether Win 3.0 and old) */

	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	/* return the DIB width if it is a Win 3.0 DIB */
	if (IS_WIN30_DIB(lpDIB))
		return lpbmi->biWidth;
	else  /* it is an other-style DIB, so return its width */
		return (DWORD)lpbmc->bcWidth;
}


/*************************************************************************
 *
 * DIBHeight()
 *
 * Parameter:
 *
 * LPSTR lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * DWORD            - height of the DIB
 *
 * Description:
 *
 * This function gets the height of the DIB from the BITMAPINFOHEADER
 * height field if it is a Windows 3.0-style DIB or from the BITMAPCOREHEADER
 * height field if it is an other-style DIB.
 *
 ************************************************************************/


DWORD WINAPI DIBHeight(LPSTR lpDIB)
{
	LPBITMAPINFOHEADER lpbmi;  // pointer to a Win 3.0-style DIB
	LPBITMAPCOREHEADER lpbmc;  // pointer to an other-style DIB

	/* point to the header (whether old or Win 3.0 */

	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	/* return the DIB height if it is a Win 3.0 DIB */
	if (IS_WIN30_DIB(lpDIB))
		return lpbmi->biHeight;
	else  /* it is an other-style DIB, so return its height */
		return (DWORD)lpbmc->bcHeight;
}


/*************************************************************************
 *
 * PaletteSize()
 *
 * Parameter:
 *
 * LPSTR lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * WORD             - size of the color palette of the DIB
 *
 * Description:
 *
 * This function gets the size required to store the DIB's palette by
 * multiplying the number of colors by the size of an RGBQUAD (for a
 * Windows 3.0-style DIB) or by the size of an RGBTRIPLE (for an other-
 * style DIB).
 *
 ************************************************************************/


WORD WINAPI PaletteSize(LPSTR lpbi)
{
   /* calculate the size required by the palette */
   if (IS_WIN30_DIB (lpbi))
	  return (WORD)(::DIBNumColors(lpbi) * sizeof(RGBQUAD));
   else
	  return (WORD)(::DIBNumColors(lpbi) * sizeof(RGBTRIPLE));
}


/*************************************************************************
 *
 * DIBNumColors()
 *
 * Parameter:
 *
 * LPSTR lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * WORD             - number of colors in the color table
 *
 * Description:
 *
 * This function calculates the number of colors in the DIB's color table
 * by finding the bits per pixel for the DIB (whether Win3.0 or other-style
 * DIB). If bits per pixel is 1: colors=2, if 4: colors=16, if 8: colors=256,
 * if 24, no colors in color table.
 *
 ************************************************************************/


WORD WINAPI DIBNumColors(LPSTR lpbi)
{
	WORD wBitCount;  // DIB bit count

	/*  If this is a Windows-style DIB, the number of colors in the
	 *  color table can be less than the number of bits per pixel
	 *  allows for (i.e. lpbi->biClrUsed can be set to some value).
	 *  If this is the case, return the appropriate value.
	 */

	if (IS_WIN30_DIB(lpbi))
	{
		DWORD dwClrUsed;

		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;
		if (dwClrUsed != 0)
			return (WORD)dwClrUsed;
	}

	/*  Calculate the number of colors in the color table based on
	 *  the number of bits per pixel for the DIB.
	 */
	if (IS_WIN30_DIB(lpbi))
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	else
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;

	/* return number of colors based on bits per pixel */
	switch (wBitCount)
	{
		case 1:
			return 2;

		case 4:
			return 16;

		case 8:
			return 256;

		default:
			return 0;
	}
}


//////////////////////////////////////////////////////////////////////////
//// Clipboard support

//---------------------------------------------------------------------
//
// Function:   CopyHandle (from SDK DibView sample clipbrd.c)
//
// Purpose:    Makes a copy of the given global memory block.  Returns
//             a handle to the new memory block (NULL on error).
//
//             Routine stolen verbatim out of ShowDIB.
//
// Parms:      h == Handle to global memory to duplicate.
//
// Returns:    Handle to new global memory block.
//
//---------------------------------------------------------------------

HGLOBAL WINAPI CopyHandle (HGLOBAL h)
{
	if (h == NULL)
		return NULL;

	DWORD dwLen = ::GlobalSize((HGLOBAL) h);
	HGLOBAL hCopy = ::GlobalAlloc(GHND | GMEM_DDESHARE, dwLen);

	if (hCopy != NULL)
	{
		void* lpCopy = ::GlobalLock((HGLOBAL) hCopy);
		void* lp     = ::GlobalLock((HGLOBAL) h);
		memcpy(lpCopy, lp, dwLen);
		::GlobalUnlock(hCopy);
		::GlobalUnlock(h);
	}

	return hCopy;
}
/**************************************************************************** 
* 
*     FUNCTION: BytesPerLine 
* 
*     PURPOSE:  Calculates the number of bytes in one scan line. 
* 
*     PARAMS:   LPBYTE lpDIB - pointer to the BITMAPINFOHEADER 
that begins the CF_DIB block 
* 
*     RETURNS:  DWORD - number of bytes in one scan line (DWORD aligned) 
* 
\****************************************************************************/ 
DWORD BytesPerLine(LPSTR lpDIB) 
{ 
    return WIDTHBYTES(((LPBITMAPINFOHEADER)lpDIB)->biWidth * ((LPBITMAPINFOHEADER)lpDIB)->biPlanes * ((LPBITMAPINFOHEADER)lpDIB)->biBitCount); 
} 

DWORD BytesPerLine(HDIB hDIB) 
{ 
	LPSTR lpDIB = (LPSTR)GlobalLock(hDIB);
	DWORD dw = BytesPerLine(lpDIB);
	GlobalUnlock(hDIB);
	return dw;
} 


/**************************************************************************** 
* 
*     FUNCTION: DIBToDIBSection 
* 
*     PURPOSE:  Create DIBSECTION from DIB
* 
*     PARAMS:   LPBYTE lpDIB - pointer to DIB data buffer
* 
*     RETURNS:  HBITMAP - handle of DIBSECTION, or NULL for failure 
* 
\****************************************************************************/ 
HBITMAP DIBToDIBSection(LPSTR lpDIB) 
{
    LPBYTE       lpSourceBits; 
    HDC			 hDC = NULL, hSourceDC; 
    HBITMAP      hSourceBitmap, hOldSourceBitmap; 
    DWORD        dwSourceBitsSize; 

	LPBITMAPINFO lpSrcDIB = (LPBITMAPINFO)lpDIB;
	if (! lpSrcDIB)
		return NULL;

    // Gonna use DIBSections and BitBlt() to do the conversion, so make 'em 
	hDC = GetDC( NULL ); 
    hSourceBitmap = CreateDIBSection( hDC, lpSrcDIB, DIB_RGB_COLORS, (VOID **)&lpSourceBits, NULL, 0 ); 
    hSourceDC = CreateCompatibleDC( hDC ); 
 
    // Flip the bits on the source DIBSection to match the source DIB 
    dwSourceBitsSize = lpSrcDIB->bmiHeader.biHeight * BytesPerLine((LPSTR)&(lpSrcDIB->bmiHeader)); 
    memcpy( lpSourceBits, FindDIBBits((LPSTR)lpSrcDIB), dwSourceBitsSize ); 
 
    // Select DIBSections into DCs 
    hOldSourceBitmap = (HBITMAP)SelectObject( hSourceDC, hSourceBitmap ); 
 
    // Set the color tables for the DIBSections 
    if( lpSrcDIB->bmiHeader.biBitCount <= 8 ) 
        SetDIBColorTable( hSourceDC, 0, 1 << lpSrcDIB->bmiHeader.biBitCount, lpSrcDIB->bmiColors ); 

    // Clean up and delete the DCs 
    SelectObject( hSourceDC, hOldSourceBitmap ); 
    DeleteDC( hSourceDC ); 
    ReleaseDC( NULL, hDC ); 
 
    // Flush the GDI batch, so we can play with the bits 
    GdiFlush(); 
 
    return hSourceBitmap;
} 

// Create DIBSECTION from DIB
HBITMAP DIBToDIBSection(HDIB hDIB) 
{
    HBITMAP      hSourceBitmap; 

	// Get DIB pointer
	if (! hDIB)
		return NULL;
	LPSTR lpSrcDIB = (LPSTR)GlobalLock(hDIB);
	if (! lpSrcDIB)
		return NULL;

	hSourceBitmap = DIBToDIBSection(lpSrcDIB);

    // final cleanup 
	GlobalUnlock(hDIB);
 
    return hSourceBitmap;
} 

/**************************************************************************** 
* 
*     FUNCTION: DIBSectionToDIB 
* 
*     PURPOSE:  Create DIB from DIBSECTION
* 
*     PARAMS:   HBITMAP - handle of DIBSECTION, or NULL for failure 
* 
*     RETURNS:  HBITMAP hBitmap - DIB handle
* 
\****************************************************************************/ 
HDIB DIBSectionToDIB(HBITMAP hBitmap)
{
    HDC			 hDC = NULL, hSourceDC; 
	HBITMAP		 hOldSourceBitmap;
	HDIB		 hNewDIB;
    LPBITMAPINFO lpbmi = NULL; 
	DWORD		 dwSize;
	DIBSECTION   ds;
    DWORD		 dwColorNum; 

	// get DIB info
	GetObject(hBitmap, sizeof(DIBSECTION), &ds);
    dwColorNum = ds.dsBmih.biClrUsed; 
    if (dwColorNum == 0 && ds.dsBmih.biBitCount <= 8) 
		dwColorNum = 1 << ds.dsBmih.biBitCount;

    // Allocate and fill out a BITMAPINFO struct for the new DIB 
	dwSize = sizeof(BITMAPINFOHEADER) + (dwColorNum * sizeof(RGBQUAD)) + ds.dsBmih.biSizeImage;
	hNewDIB =(HDIB)GlobalAlloc(GHND, dwSize);
	if (! hNewDIB)
		return NULL;
	lpbmi = (LPBITMAPINFO)GlobalLock(hNewDIB);
	if (! lpbmi)
		return NULL;
    lpbmi->bmiHeader.biSize = sizeof( BITMAPINFOHEADER ); 
    lpbmi->bmiHeader.biWidth = ds.dsBm.bmWidth; 
    lpbmi->bmiHeader.biHeight = ds.dsBm.bmHeight; 
    lpbmi->bmiHeader.biPlanes = 1; 
    lpbmi->bmiHeader.biBitCount = ds.dsBmih.biBitCount; 
    lpbmi->bmiHeader.biCompression = ds.dsBmih.biCompression; 
    lpbmi->bmiHeader.biSizeImage = ds.dsBmih.biSizeImage; 
    lpbmi->bmiHeader.biXPelsPerMeter = ds.dsBmih.biXPelsPerMeter; 
    lpbmi->bmiHeader.biYPelsPerMeter = ds.dsBmih.biYPelsPerMeter; 
    lpbmi->bmiHeader.biClrUsed = ds.dsBmih.biClrUsed; 
    lpbmi->bmiHeader.biClrImportant = ds.dsBmih.biClrImportant; 

	// get DC for operation
	hDC = GetDC( NULL ); 
 
	// get DIB bits
    if (! GetDIBits(hDC,
                    hBitmap,
                    0L,
                    (DWORD)ds.dsBm.bmHeight,
                    (LPBYTE)lpbmi + (WORD)lpbmi->bmiHeader.biSize + (dwColorNum*sizeof(RGBQUAD)),
                    (LPBITMAPINFO)lpbmi, 
					(DWORD)DIB_RGB_COLORS))
	{
         GlobalUnlock(hNewDIB);
         ReleaseDC(NULL,hDC);
         return NULL;
    }

	// memory DC
    hSourceDC = CreateCompatibleDC( hDC ); 
    // Select DIBSections into DCs 
    hOldSourceBitmap = (HBITMAP)SelectObject( hSourceDC, hBitmap ); 
 
    // Fill in the color table from DIBSection
    if( lpbmi->bmiHeader.biBitCount <= 8 ) 
        GetDIBColorTable( hSourceDC, 0, 1 << lpbmi->bmiHeader.biBitCount, lpbmi->bmiColors ); 
 
    // Clean up and delete the DCs 
    SelectObject( hSourceDC, hOldSourceBitmap ); 
    DeleteDC( hSourceDC ); 
    ReleaseDC( NULL, hDC ); 
	GlobalUnlock(hNewDIB);
 
    return hNewDIB; 
}
/*************************************************************************
*
* BitmapToDIB()
*
* Parameter:
*
* HBITMAP - handle to a Bitmap that the DIB is to be 
*            created from
* HPALETTE - palette to use in creation of DIB
*
* Return Value:
*
* HDIB - handle of the DIB created from the Bitmap, NULL
*        on error
*
* Description:
*
* Returns an HDIB created from an HBITMAP.
* This function creates a DIB from a bitmap using the 
* specified palette.
************************************************************************/

HDIB WINAPI BitmapToDIB(HBITMAP hBitmap, HPALETTE hPal)
{
	ASSERT(hBitmap);

	BITMAP bm;               // bitmap structure
	BITMAPINFOHEADER bi;     // bitmap header
	LPBITMAPINFOHEADER lpbi; // pointer to BITMAPINFOHEADER
	DWORD dwLen;             // size of memory block
	HDIB hDib, h;            // handle to DIB, temp handle
	HDC hDC;                 // handle to DC
	WORD biBits;             // bits per pixel
	UINT wLineLen;
	DWORD dwSize;
	DWORD wColSize;

	// check if bitmap handle is valid
	if(!hBitmap)
		return NULL;

	// fill in BITMAP structure, return NULL if it didn't work
	if(!::GetObject(hBitmap, sizeof(bm), &bm))
		return NULL;

	// if no palette is specified, use default palette
	if(hPal == NULL)
		hPal = (HPALETTE)::GetStockObject(DEFAULT_PALETTE);
	// calculate bits per pixel
	biBits = (WORD) (bm.bmPlanes * bm.bmBitsPixel);

	wLineLen = ( bm.bmWidth * biBits + 31 ) / 32 * 4;
	wColSize = sizeof(RGBQUAD) * (( biBits <= 8 ) ? 1 << biBits : 0 );
	dwSize = sizeof( BITMAPINFOHEADER ) + wColSize +(DWORD)(UINT)wLineLen * (DWORD)(UINT)bm.bmHeight;

	// make sure bits per pixel is valid
	if (biBits <= 1)
		biBits = 1;
	else if (biBits <= 4)
		biBits = 4;
	else if (biBits <= 8)
		biBits = 8;
	else // if greater than 8-bit, force to 24-bit
		biBits = 24;

	// initialize BITMAPINFOHEADER
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bm.bmWidth;
	bi.biHeight = bm.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = biBits;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = dwSize - sizeof(BITMAPINFOHEADER) - wColSize;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = ( biBits <= 8 ) ? 1 << biBits : 0;	
	bi.biClrImportant = 0;

	// calculate size of memory block required to store BITMAPINFO
	dwLen = bi.biSize + ::PaletteSize((LPSTR) &bi);

	// get a DC
	hDC = ::GetDC(NULL);

	// select and realize our palette
	hPal = ::SelectPalette(hDC, hPal, FALSE);
	::RealizePalette(hDC);

	// alloc memory block to store our bitmap
	hDib = (HDIB)::GlobalAlloc(GHND, dwLen);

	// if we couldn't get memory block
	if (!hDib)
	{
		// clean up and return NULL
		::SelectPalette(hDC, hPal, TRUE);
		::RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
		return NULL;
	}

	// lock memory and get pointer to it
	lpbi = (LPBITMAPINFOHEADER)::GlobalLock((HGLOBAL)hDib);
	if (!lpbi)
	{
		// clean up and return NULL
		::SelectPalette(hDC, hPal, TRUE);
		::RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
		return NULL;
	}

	// use our bitmap info. to fill BITMAPINFOHEADER
	*lpbi = bi;

	// call GetDIBits with a NULL lpBits param, so it will 
	// calculate the biSizeImage field for us
	::GetDIBits(hDC, hBitmap, 0, (WORD)bi.biHeight, NULL, (LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

	// get the info. returned by GetDIBits and unlock 
	// memory block
	bi = *lpbi;
	bi.biClrUsed = ( biBits <= 8 ) ? 1 << biBits : 0;
	::GlobalUnlock(hDib);

	// if the driver did not fill in the biSizeImage field, 
	// make one up
	if (bi.biSizeImage == 0)
		bi.biSizeImage = WIDTHBYTES((DWORD)bm.bmWidth * biBits) * bm.bmHeight;


	// realloc the buffer big enough to hold all the bits
	dwLen = bi.biSize + ::PaletteSize((LPSTR) &bi) + 
	bi.biSizeImage;
	h = (HDIB)::GlobalReAlloc(hDib, dwLen, 0);
	if ( h )
		hDib = h;
	else
	{
		// clean up and return NULL
		::GlobalFree(hDib);
		hDib = NULL;
		::SelectPalette(hDC, hPal, TRUE);
		::RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
		return NULL;
	}

	// lock memory block and get pointer to it
	lpbi = (LPBITMAPINFOHEADER)::GlobalLock((HGLOBAL)hDib);
	if(!lpbi)
	{
		// clean up and return NULL
		::GlobalFree(hDib);
		hDib = NULL;
		::SelectPalette(hDC, hPal, TRUE);
		::RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
		return NULL;
	}

	// call GetDIBits with a NON-NULL lpBits param, and 
	// actualy get the bits this time
	if (::GetDIBits(hDC, hBitmap, 0, (WORD)bi.biHeight, (LPSTR)lpbi + (WORD)lpbi->biSize + 
				::PaletteSize((LPSTR) lpbi), (LPBITMAPINFO)lpbi, DIB_RGB_COLORS) == 0)
	{
		// clean up and return NULL
		::GlobalUnlock(hDib);
		hDib = NULL;
		::SelectPalette(hDC, hPal, TRUE);
		::RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
		return NULL;
	}
	bi = *lpbi;
	// clean up
	::GlobalUnlock(hDib);
	::SelectPalette(hDC, hPal, TRUE);
	::RealizePalette(hDC);
	::ReleaseDC(NULL, hDC);

	// return handle to the DIB
	return hDib;
}

/*************************************************************************
*
* DIBToBitmap()
*
* Parameters:
* HDIB - handle to a DIB that the Bitmap (DDB) is to be 
*        created from
*
* HPALETTE - palette to use in creation of bitmap (DDB)
*
* Return Value:
*
* HBITMAP - handle of the Bitmap created from the DIB,
*           NULL on error
* Description:
*
* Returns an HBITMAP created from an HDIB.
* This function creates a bitmap from a DIB using the 
* specified palette. If no palette is specified, one is 
* created, used for the conversion, and then deleted.
*
* The bitmap returned from this funciton is always a 
* bitmap compatible with the screen (e.g. same 
* bits/pixel and color planes) rather than a bitmap 
* with the same attributes as the DIB.
* This behavior is by design, and occurs because this 
* function calls CreateDIBitmap to do its work, and 
* CreateDIBitmap always creates a bitmap compatible with 
* the hDC parameter passed in (because it in turn calls 
* CreateCompatibleBitmap).
*
* So for instance, if your DIB is a monochrome DIB and 
* you call this function, you will not get back a 
* monochrome HBITMAP -- you will get an HBITMAP 
* compatible with the screen DC, but with only 2 colors 
* used in the bitmap.
*
* if your application requires a monochrome HBITMAP 
* returned for a monochrome DIB, use the function SetDIBits().
************************************************************************/

HBITMAP WINAPI DIBToBitmap(HDIB hDib, HPALETTE hPal)
{
   ASSERT(hDib);
   
   LPVOID lpDIBHdr = NULL;  // pointer to DIB header
   LPVOID lpDIBBits = NULL; // pointer to DIB bits
   HBITMAP hBitmap = NULL;  // handle to DDB
   HDC hDC = NULL;          // handle to DC
   HPALETTE hOldPal = NULL; // handle to a palette
   BOOL bPalCreated = FALSE;
   
   // if invalid handle, return NULL
   if (!hDib)
   {
      return NULL;
   }
   
   // lock memory block and get a pointer to it
   lpDIBHdr = ::GlobalLock((HGLOBAL)hDib);
   if (!lpDIBHdr)
   {
      return NULL;
   }
   
   // get a pointer to the DIB bits
   lpDIBBits = ::FindDIBBits((LPSTR) lpDIBHdr);
   if (!lpDIBBits)
      return NULL;
   
   // get a DC
   hDC = ::GetDC(NULL);
   if (!hDC)
   {
      // clean up and return NULL
      ::GlobalUnlock(hDib);
      
      return NULL;
   }
   
   // select and realize palette
   if (!hPal)
   {
      CPalette    DibPal; 
      CreateDIBPalette(hDib,&DibPal);
      hPal = (HPALETTE) DibPal.Detach();
      bPalCreated = TRUE;
   }
   hOldPal = ::SelectPalette(hDC, hPal, FALSE);
   ::RealizePalette(hDC);
   
   // create bitmap from DIB info. and bits
   hBitmap = ::CreateDIBitmap(hDC, (LPBITMAPINFOHEADER)lpDIBHdr, 
      CBM_INIT, lpDIBBits, (LPBITMAPINFO)lpDIBHdr, 
      DIB_RGB_COLORS);
   if (!hBitmap)
   {
      if (hOldPal)
         ::SelectPalette(hDC, hOldPal, FALSE);
      ::DeleteObject(hPal);
      return NULL;
   }
   
   // restore previous palette
   if (hOldPal)
      ::SelectPalette(hDC, hOldPal, FALSE);
   
   // if we created the palette then we clean it up
   if (bPalCreated && hPal)
      ::DeleteObject(hPal);
   
   // clean up
   ::ReleaseDC(NULL, hDC);
   ::GlobalUnlock(hDib);
   
   // return handle to the bitmap
   return hBitmap;
}
/************************************************************************* 
* 
* Delay() 
* 
* Parameters: 
* 
* DWORD dwDelayTime   - ms to delay
* 
* Return Value: none
* 
* Description: 
* 
* This function Delay specified ms before perform next instruction 
* 
************************************************************************/ 
void Delay(DWORD dwDelayTime)
{
/*    DWORD dwTimeBegin, dwTimeEnd;
    dwTimeBegin = timeGetTime();
    do
	{
        dwTimeEnd = timeGetTime();
	} while (dwTimeEnd - dwTimeBegin < dwDelayTime);*/
//	TRACE("%d,%d,%d\n",dwTimeBegin,dwTimeEnd,dwTimeEnd-dwTimeBegin);	
}
/************************************************************************* 
* 
* Fade() 
* 
* Parameters: 
* 
* int nDeltaR   -  change value of Red
* int nDeltaG   -  change value of Green
* int nDeltaB   -  change value of Blue
* BYTE rm		 -  maxinum value of Red
* BYTE gm		 -  maxinum value of Green
* BYTE bm		 -  maxinum value of Blue
* BYTE *r		 -  pointer of value of Red after change
* BYTE *g		 -  pointer of value of Green after change
* BYTE *b		 -  pointer of value of Blue after change
* 
* Return Value: 
*
* BOOL          -  TRUE if success, FALSE is failue
* 
* Description: 
* 
* This function change the value of R, G, b
* 
************************************************************************/
BOOL Fade(int nDeltaR, int nDeltaG, int nDeltaB, 
		  BYTE rm, BYTE gm, BYTE bm,
		  BYTE *r, BYTE *g, BYTE *b)
{
	int R = *r + nDeltaR;
	int G = *g + nDeltaG;
	int B = *b + nDeltaB;
	
	if (nDeltaR < 0)
		*r = (BYTE)BOUND(R, rm, 255);
	else
		*r = (BYTE)BOUND(R, 0, rm);
	if (nDeltaG < 0)
		*g = (BYTE)BOUND(G, gm, 255);
	else
		*g = (BYTE)BOUND(G, 0, gm);
	if (nDeltaB < 0)
		*b = (BYTE)BOUND(B, bm, 255);
	else
		*b = (BYTE)BOUND(B, 0, bm);
	
	if (*r==rm && *g==gm && *b==bm)
		return TRUE;
	else
		return FALSE;
}


//
//  Source file for Device-Independent Bitmap (DIB) API.  Provides
//  the following functions:
//
//  SaveDIB()           - Saves the specified dib in a file
//  ReadDIBFile()       - Loads a DIB from a file
//
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1997 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include <math.h>
#include <io.h>
#include <direct.h>
#include "dibapi.h"

/*
 * Dib Header Marker - used in writing DIBs to files
 */
#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')

#ifdef _MAC
#define SWAPWORD(x) MAKEWORD(HIBYTE(x), LOBYTE(x))
#define SWAPLONG(x) MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))
void ByteSwapHeader(BITMAPFILEHEADER* bmiHeader);
void ByteSwapInfo(LPSTR lpHeader, BOOL fWin30Header);
#endif

/*************************************************************************
 *
 * SaveDIB()
 *
 * Saves the specified DIB into the specified CFile.  The CFile
 * is opened and closed by the caller.
 *
 * Parameters:
 *
 * HDIB hDib - Handle to the dib to save
 *
 * CFile& file - open CFile used to save DIB
 *
 * Return value: TRUE if successful, else FALSE or CFileException
 *
 *************************************************************************/


BOOL WINAPI SaveDIB(HDIB hDib, CFile& file)
{
	BITMAPFILEHEADER bmfHdr; // Header for Bitmap file
	LPBITMAPINFOHEADER lpBI;   // Pointer to DIB info structure
	DWORD dwDIBSize;

	if (hDib == NULL)
		return FALSE;

	/*
	 * Get a pointer to the DIB memory, the first of which contains
	 * a BITMAPINFO structure
	 */
	lpBI = (LPBITMAPINFOHEADER) ::GlobalLock((HGLOBAL) hDib);
	if (lpBI == NULL)
		return FALSE;

	if (!IS_WIN30_DIB(lpBI))
	{
		::GlobalUnlock((HGLOBAL) hDib);
		return FALSE;       // It's an other-style DIB (save not supported)
	}

	/*
	 * Fill in the fields of the file header
	 */

	/* Fill in file type (first 2 bytes must be "BM" for a bitmap) */
	bmfHdr.bfType = DIB_HEADER_MARKER;  // "BM"

	// Calculating the size of the DIB is a bit tricky (if we want to
	// do it right).  The easiest way to do this is to call GlobalSize()
	// on our global handle, but since the size of our global memory may have
	// been padded a few bytes, we may end up writing out a few too
	// many bytes to the file (which may cause problems with some apps).
	//
	// So, instead let's calculate the size manually (if we can)
	//
	// First, find size of header plus size of color table.  Since the
	// first DWORD in both BITMAPINFOHEADER and BITMAPCOREHEADER conains
	// the size of the structure, let's use this.

	dwDIBSize = *(LPDWORD)lpBI + ::PaletteSize((LPSTR)lpBI);  // Partial Calculation

	// Now calculate the size of the image

	if ((lpBI->biCompression == BI_RLE8) || (lpBI->biCompression == BI_RLE4))
	{
		// It's an RLE bitmap, we can't calculate size, so trust the
		// biSizeImage field

		dwDIBSize += lpBI->biSizeImage;
	}
	else
	{
		DWORD dwBmBitsSize;  // Size of Bitmap Bits only

		// It's not RLE, so size is Width (DWORD aligned) * Height

		dwBmBitsSize = WIDTHBYTES((lpBI->biWidth)*((DWORD)lpBI->biBitCount)) * lpBI->biHeight;

		dwDIBSize += dwBmBitsSize;

		// Now, since we have calculated the correct size, why don't we
		// fill in the biSizeImage field (this will fix any .BMP files which
		// have this field incorrect).

		lpBI->biSizeImage = dwBmBitsSize;
	}


	// Calculate the file size by adding the DIB size to sizeof(BITMAPFILEHEADER)

	bmfHdr.bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;

	/*
	 * Now, calculate the offset the actual bitmap bits will be in
	 * the file -- It's the Bitmap file header plus the DIB header,
	 * plus the size of the color table.
	 */
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + lpBI->biSize
											  + PaletteSize((LPSTR)lpBI);
#ifdef _MAC
	ByteSwapHeader(&bmfHdr);

	// First swap the size field
	*((LPDWORD)lpBI) = SWAPLONG(*((LPDWORD)lpBI));

	// Now swap the rest of the structure (we don't save < Win30 files)
	ByteSwapInfo((LPSTR)lpBI, TRUE);
#endif

	TRY
	{
		// Write the file header
		file.Write((LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER));
		//
		// Write the DIB header and the bits
		//
		file.Write(lpBI, dwDIBSize);
	}
	CATCH (CFileException, e)
	{
#ifdef _MAC
		// Swap everything back
		*((LPDWORD)lpBI) = SWAPLONG(*((LPDWORD)lpBI));
		ByteSwapInfo((LPSTR)lpBI, TRUE);
#endif
		::GlobalUnlock((HGLOBAL) hDib);
		THROW_LAST();
	}
	END_CATCH

#ifdef _MAC
	// Swap everything back
	*((LPDWORD)lpBI) = SWAPLONG(*((LPDWORD)lpBI));
	ByteSwapInfo((LPSTR)lpBI, TRUE);
#endif

	::GlobalUnlock((HGLOBAL) hDib);
	return TRUE;
}


/*************************************************************************

  Function:  ReadDIBFile (CFile&)

   Purpose:  Reads in the specified DIB file into a global chunk of
			 memory.

   Returns:  A handle to a dib (hDIB) if successful.
			 NULL if an error occurs.

  Comments:  BITMAPFILEHEADER is stripped off of the DIB.  Everything
			 from the end of the BITMAPFILEHEADER structure on is
			 returned in the global memory handle.

*************************************************************************/


HDIB WINAPI ReadDIBFile(CFile& file)
{
	BITMAPFILEHEADER bmfHeader;
	DWORD dwBitsSize;
	HDIB hDIB;
	LPSTR pDIB;

	/*
	 * get length of DIB in bytes for use when reading
	 */

	dwBitsSize = file.GetLength();

	/*
	 * Go read the DIB file header and check if it's valid.
	 */
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
		return NULL;

#ifdef _MAC
	ByteSwapHeader(&bmfHeader);
#endif
	if (bmfHeader.bfType != DIB_HEADER_MARKER)
		return NULL;

	/*
	 * Allocate memory for DIB
	 */
	hDIB = (HDIB) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize);
	if (hDIB == 0)
	{
		return NULL;
	}
	pDIB = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);

	/*
	 * Go read the bits.
	 */
	if (file.Read(pDIB, dwBitsSize - sizeof(BITMAPFILEHEADER)) !=
		dwBitsSize - sizeof(BITMAPFILEHEADER) )
	{
		::GlobalUnlock((HGLOBAL) hDIB);
		::GlobalFree((HGLOBAL) hDIB);
		return NULL;
	}
#ifdef _MAC
	// First swap the size field
	*((LPDWORD)pDIB) = SWAPLONG(*((LPDWORD)pDIB));

	// Now swap the rest of the structure
	ByteSwapInfo(pDIB, IS_WIN30_DIB(pDIB));
#endif
	::GlobalUnlock((HGLOBAL) hDIB);
	return hDIB;
}


#ifdef _MAC
void ByteSwapHeader(BITMAPFILEHEADER* bmfHeader)
{
	bmfHeader->bfType = SWAPWORD(bmfHeader->bfType);
	bmfHeader->bfSize = SWAPLONG(bmfHeader->bfSize);
	bmfHeader->bfOffBits = SWAPLONG(bmfHeader->bfOffBits);
}


void ByteSwapInfo(LPSTR lpHeader, BOOL fWin30Header)
{
	// Note this doesn't swap the bcSize/biSize field.  It assumes that the
	// size field was swapped during read or while setting the fWin30Header
	// flag.

	if (fWin30Header)
	{
		LPBITMAPINFOHEADER lpBMIH = &(LPBITMAPINFO(lpHeader)->bmiHeader);

		//lpBMIH->biSize = SWAPLONG(lpBMIH->biSize);
		lpBMIH->biWidth = SWAPLONG(lpBMIH->biWidth);
		lpBMIH->biHeight = SWAPLONG(lpBMIH->biHeight);
		lpBMIH->biPlanes = SWAPWORD(lpBMIH->biPlanes);
		lpBMIH->biBitCount = SWAPWORD(lpBMIH->biBitCount);
		lpBMIH->biCompression = SWAPLONG(lpBMIH->biCompression);
		lpBMIH->biSizeImage = SWAPLONG(lpBMIH->biSizeImage);
		lpBMIH->biXPelsPerMeter = SWAPLONG(lpBMIH->biXPelsPerMeter);
		lpBMIH->biYPelsPerMeter = SWAPLONG(lpBMIH->biYPelsPerMeter);
		lpBMIH->biClrUsed = SWAPLONG(lpBMIH->biClrUsed);
		lpBMIH->biClrImportant = SWAPLONG(lpBMIH->biClrImportant);
	}
	else
	{
		LPBITMAPCOREHEADER lpBMCH = &(LPBITMAPCOREINFO(lpHeader)->bmciHeader);

		lpBMCH->bcWidth = SWAPWORD(lpBMCH->bcWidth);
		lpBMCH->bcHeight = SWAPWORD(lpBMCH->bcHeight);
		lpBMCH->bcPlanes = SWAPWORD(lpBMCH->bcPlanes);
		lpBMCH->bcBitCount = SWAPWORD(lpBMCH->bcBitCount);
	}
}

#endif
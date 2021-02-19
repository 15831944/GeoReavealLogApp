// DlgUnits.cpp: implementation of the CDlgUnits class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DlgUnits.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDlgUnits::CDlgUnits(HWND hDlg) : m_hDlg(NULL)
{
	if (!hDlg)
	{
		DWORD dwDLBU = ::GetDialogBaseUnits();
		
		m_dlgBaseUnits.cx = LOWORD(dwDLBU);
		m_dlgBaseUnits.cy = HIWORD(dwDLBU);
	}
	else
	{
		m_hDlg = hDlg;
		m_dlgBaseUnits.cx = m_dlgBaseUnits.cy = 0;
	}
}

CDlgUnits::~CDlgUnits()
{
}

int CDlgUnits::ToPixelsX(int x)
{
	if (m_hDlg)
	{
		CRect rect(0, 0, x, 0);
		MapDialogRect(m_hDlg, rect);
	
		return rect.right;
	}
	else
		return MulDiv(x, m_dlgBaseUnits.cx, 4);
}

int CDlgUnits::ToPixelsY(int y)
{
	if (m_hDlg)
	{
		CRect rect(0, 0, 0, y);
		MapDialogRect(m_hDlg, rect);
	
		return rect.bottom;
	}
	else
		return MulDiv(y, m_dlgBaseUnits.cy, 8);
}

void CDlgUnits::ToPixels(long& x, long& y)
{
	if (m_hDlg)
	{
		CRect rect(0, 0, x, y);
		MapDialogRect(m_hDlg, rect);

		x = rect.right;
		y = rect.bottom;
	}
	else
	{
		x = MulDiv(x, m_dlgBaseUnits.cx, 4);
		y = MulDiv(y, m_dlgBaseUnits.cy, 8);
	}
}

void CDlgUnits::ToPixels(int& x, int& y)
{
	ToPixels((long&)x, (long&)y);
}

void CDlgUnits::ToPixels(short& x, short& y)
{
	ToPixels((long&)x, (long&)y);
}

void CDlgUnits::ToPixels(POINT& point)
{
	ToPixels(point.x, point.y);
}

void CDlgUnits::ToPixels(RECT& rect)
{
	ToPixels(rect.left, rect.top);
	ToPixels(rect.right, rect.bottom);
}

void CDlgUnits::ToPixels(SIZE& size)
{
	ToPixels(size.cx, size.cy);
}

void CDlgUnits::FromPixels(long& x, long& y)
{
	if (m_hDlg)
	{
		// this is a bit messy ebcause there is no MapDialogRect() in reverse
		CRect rect(0, 0, 1000, 1000);
		MapDialogRect(m_hDlg, rect);

		x = MulDiv(x, 1000, rect.right);
		y = MulDiv(y, 1000, rect.right);
	}
	else
	{
		x = MulDiv(x, 4, m_dlgBaseUnits.cx);
		y = MulDiv(y, 8, m_dlgBaseUnits.cy);
	}
}

void CDlgUnits::FromPixels(int& x, int& y)
{
	FromPixels((long&)x, (long&)y);
}

void CDlgUnits::FromPixels(short& x, short& y)
{
	FromPixels((long&)x, (long&)y);
}

void CDlgUnits::FromPixels(POINT& point)
{
	FromPixels(point.x, point.y);
}

void CDlgUnits::FromPixels(RECT& rect)
{
	FromPixels(rect.left, rect.top);
	FromPixels(rect.right, rect.bottom);
}

void CDlgUnits::FromPixels(SIZE& size)
{
	FromPixels(size.cx, size.cy);
}


// DlgUnits.h: interface for the CDlgUnits class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLGUNITS_H__18921FC5_A659_4AA7_BDF8_EF630D0D13E6__INCLUDED_)
#define AFX_DLGUNITS_H__18921FC5_A659_4AA7_BDF8_EF630D0D13E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDlgUnits  
{
public:
	CDlgUnits(HWND hDlg = NULL);
	virtual ~CDlgUnits();

	int ToPixelsX(int x);
	int ToPixelsY(int y);

	void ToPixels(long& x, long& y);
	void ToPixels(int& x, int& y);
	void ToPixels(short& x, short& y);
	void ToPixels(POINT& point);
	void ToPixels(RECT& rect);
	void ToPixels(SIZE& size);
	void FromPixels(long& x, long& y);
	void FromPixels(int& x, int& y);
	void FromPixels(short& x, short& y);
	void FromPixels(POINT& point);
	void FromPixels(RECT& rect);
	void FromPixels(SIZE& size);

protected:
	SIZE m_dlgBaseUnits;
	HWND m_hDlg;

};

#endif // !defined(AFX_DLGUNITS_H__18921FC5_A659_4AA7_BDF8_EF630D0D13E6__INCLUDED_)

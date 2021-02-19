// Surfer.h: interface for the CSurfer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SURFER_H__C59B55FC_9468_41FB_8676_B162BF05685F__INCLUDED_)
#define AFX_SURFER_H__C59B55FC_9468_41FB_8676_B162BF05685F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3DGrapher.h"
#include "Image.h"

class AFX_EXT_CLASS CSurfer : public C3DGrapher  
{
public:
	CSurfer();
	virtual ~CSurfer();
	COLORREF *Colors;
	void Init(HWND hWnd) throw();
	void Destroy() throw();
	void Render() throw();
	void SetWall(bool wall) throw() { m_bWall = wall; }
	bool IsWall() const throw() { return m_bWall; }
	void SetLegend(bool legend) throw() { m_bShowLegend = legend; }
	bool IsLegend() const throw() { return m_bShowLegend; }
	void SetBoundary(bool bound) throw() { m_bBoundary = bound; }
	bool IsBoundary() const throw() { return m_bBoundary; }
	void SetMinValue(double min) throw() { m_dThicknessMin = min; }
	
	int		m_nLegend;
	CString m_SchemeFile;
	int		m_nNumOfLevels;
	int		m_ZMin,m_ZMax;
	CString GetWorkPath(CString flag);
	BOOL GetLogTopRegString(CString szSubKey,LPCTSTR lpszEntry,CString& strValue);
private:
	void BuildBoundaryList() const throw();
	void BuildAxisList() const throw();
	void BuildDataList() const throw();
	void BuildWallList() const throw();
	void ShowLegend() const throw();
	void SetDataColor(double thickness) const throw();
	COLORREF	m_crBoundary;
	bool		m_bBoundary;
	bool		m_bWall;
	double		m_dThicknessMin;
	CImage*		m_pImage;
	bool		m_bSupportImage;
	bool		m_bShowImage;
	bool		m_bShowLegend;
};

#endif // !defined(AFX_SURFER_H__C59B55FC_9468_41FB_8676_B162BF05685F__INCLUDED_)

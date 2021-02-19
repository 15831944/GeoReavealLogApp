// TrackBasePage.h : header file
//

#ifndef __TRACKBASEPAGE_H__
#define __TRACKBASEPAGE_H__
/////////////////////////////////////////////////////////////////////////////
// CTrackBasePage dialog

class CTrackBasePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTrackBasePage)

// Construction
public:
	CTrackBasePage(CTrackObj* pTrackObj=NULL);
	~CTrackBasePage();

// Dialog Data
	//{{AFX_DATA(CTrackBasePage)
	enum { IDD = IDD_TRACK_BASE };
	int		m_DepthTrackPos;
	int		m_DepthValuePos;
	BOOL	m_bShowDepth;
	BOOL	m_bShowTitle;
	BOOL    m_bTrackResize;
	double	m_PlotEdep;
	double	m_PlotSdep;
	//}}AFX_DATA

CTrackObj* pObj;
BOOL bSetRange;
int m_DepthTrackPosition,m_DepthValuePosition;
BOOL bDrawDepth,bDrawTitle,bTrackResize;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTrackBasePage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTrackBasePage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif // __TRACKBASEPAGE_H__

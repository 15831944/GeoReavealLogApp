
#ifndef __GRID_H__
#define __GRID_H__   

#define LINEGRID 0
#define LOGGRID 1 
#define DIPGRID 2 

class AFX_EXT_CLASS CGridObj : public CObject
{                      
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CGridObj)
#else
	DECLARE_SERIAL(CGridObj);
#endif
public:
	CGridObj();
	CGridObj(const CRect& rect, double sdep, double edep, int nGridType = LINEGRID);
	~CGridObj();       

//Attribute:
public:
	CRect m_Rect;
	double m_PlotSdep,m_PlotEdep;
    BOOL m_bDrawDepLine,m_bDrawVerLine,m_bHoriOpaque,m_bVertOpaque,m_bDrawRuler;
	int m_VerLine,m_VerLine1,m_VerLine2,m_GridType;     
	int m_DepLine1,m_DepLine2,m_DepLine3;
	LOGPEN m_DepPen1,m_DepPen2,m_DepPen3,m_VerPen1,m_VerPen2;
	double m_LeftVal,m_RightVal;   
	int* m_ZerX;
	double* m_AngX;
	double m_DepC;
	BOOL bDrawDepth;	
	WORD m_DepthTrackPosition;
// Operation
public:
	void SetRect(const CRect& rect);
	void SetDepth(double sdep, double edep);
	void SetDepC(double depc);
	CRect GetRect();
	void DrawHoriLine(CDC* pDC);
	void DrawVertLine(CDC* pDC);
	void Serialize(CArchive& ar);
	void CreateVerLine();
//	void OnDepthTypeChange(int flag);

protected:
	double YCoorToDepth(long y);
	long DepthToYCoor(double depth);
};

#endif // __GRID_H__
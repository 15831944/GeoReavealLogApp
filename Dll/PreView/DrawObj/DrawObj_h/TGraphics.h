// TGraphics.h: interface for the CTGraphics class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TGRAPHICS_H)
#define TGRAPHICS_H

class CTGraphics  
{
public:
	CTGraphics();
	virtual ~CTGraphics();

	int m_X,m_Y;
	// CTGraphics class interface
public:
	void DrawPie(HDC hDC, int x1, int y1, int x2, int y2, float startAngle, float endAngle, COLORREF color);
	void DrawPolyline(HDC hDC, POINT* points, int numberPoints, COLORREF color);
	void DrawRoundRect(HDC hDC, int x1, int y1, int x2, int y2, int width, int height, COLORREF color);
	void DrawArc(HDC hDC, int x1, int y1, int x2, int y2, float startAngle, float endAngle, COLORREF color);
	void DrawEllipse(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color);
	void DrawLine(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color);
	void MoveTo(int x, int y);
    void LineTo(HDC hDC, int x2, int y2, COLORREF color,int nWidth=1);
};

#endif // !defined(TGRAPHICS_H)

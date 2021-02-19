// TGraphics.cpp: implementation of the CTGraphics class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TGraphics.h"
#include "math.h"


CTGraphics::CTGraphics()
{
	m_X=0;m_Y=0;
}

CTGraphics::~CTGraphics()
{
}

void CTGraphics::DrawLine(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color)
{
	// Calculate line params
	int dx = (x2 - x1);
	int dy = (y2 - y1);
	int temp;
	double k;
	// Set start pixel
	::SetPixel(hDC, x1, y1, color);
	double distance;
	int red, green, blue;
	COLORREF bgColor;
	// X-dominant line
	if(abs(dx)>abs(dy))
	{
		// Ex-change line end points
		if (dx < 0)
		{
			temp = x1;
			x1 = x2;
			x2 = temp;

			temp = y1;
			y1 = y2;
			y2 = temp;
		}
		k = (double)dy / (double)dx;

		// Set middle pixels
		int xs;
		double yt = (double)y1 + k;
		for (xs=x1+1; xs<x2; xs++)
		{
			distance =yt -(int)yt;
			if(distance<0)
			{
				distance=fabs(distance);
				bgColor= ::GetPixel(hDC, xs, (int)yt-1);
				red = (int)(distance*GetRValue(color)+(1.0-distance)*GetRValue(bgColor));
				green = (int)(distance*GetGValue(color)+(1.0-distance)*GetGValue(bgColor));
				blue = (int)(distance*GetBValue(color)+(1.0-distance)*GetBValue(bgColor));
				::SetPixel(hDC, xs, (int)yt-1, RGB(red,green,blue));
				bgColor = ::GetPixel(hDC, xs, (int)yt);
				red = (int)((1.0-distance)*GetRValue(color)+distance*GetRValue(bgColor));
				green = (int)((1.0-distance)*GetGValue(color)+distance*GetGValue(bgColor));
				blue = (int)((1.0-distance)*GetBValue(color)+distance*GetBValue(bgColor));
				::SetPixel(hDC, xs, (int)yt, RGB(red,green,blue));
			}
			else
			{
				bgColor= ::GetPixel(hDC, xs, (int)yt);
				red = (int)(distance*GetRValue(bgColor)+(1.0-distance)*GetRValue(color));
				green = (int)(distance*GetGValue(bgColor)+(1.0-distance)*GetGValue(color));
				blue = (int)(distance*GetBValue(bgColor)+(1.0-distance)*GetBValue(color));
				::SetPixel(hDC, xs, (int)yt, RGB(red,green,blue));
				bgColor = ::GetPixel(hDC, xs, (int)yt+1);
				red = (int)((1.0-distance)*GetRValue(bgColor)+distance*GetRValue(color));
				green = (int)((1.0-distance)*GetGValue(bgColor)+distance*GetGValue(color));
				blue = (int)((1.0-distance)*GetBValue(bgColor)+distance*GetBValue(color));
				::SetPixel(hDC, xs, (int)yt+1, RGB(red,green,blue));
			}
			yt +=k;
		}
	}
	// Y-dominant line
	else
	{
		// Ex-change line end points
		if (dy < 0)
		{
			temp = x1;
			x1 = x2;
			x2 = temp;

			temp = y1;
			y1 = y2;
			y2 = temp;
		}
		k = (double)dx / (double)dy;

		// Set middle pixels
		int ys;
		double xt = (float)x1 + k;
		for (ys=y1+1; ys<y2; ys++)
		{
			distance = xt - (int)xt;
			if(distance<0)
			{
				distance=fabs(distance);
				bgColor = ::GetPixel(hDC, (int)xt-1, ys);
				red = (int)(distance*GetRValue(bgColor)) + (int)((1.0f-distance)*GetRValue(color));
				green = (int)(distance*GetGValue(bgColor)) + (int)((1.0f-distance)*GetGValue(color));
				blue = (int)(distance*GetBValue(bgColor)) + (int)((1.0f-distance)*GetBValue(color));
				::SetPixel(hDC, (int)xt-1, ys, RGB(red,green,blue));
	
				bgColor = ::GetPixel(hDC, (int)xt, ys);
				red = (int)((1.0f-distance)*GetRValue(bgColor)) + (int)(distance*GetRValue(color));
				green = (int)((1.0f-distance)*GetGValue(bgColor)) + (int)(distance*GetGValue(color));
				blue = (int)((1.0f-distance)*GetBValue(bgColor)) + (int)(distance*GetBValue(color));
				::SetPixel(hDC, (int)xt, ys, RGB(red,green,blue));
			}
			else
			{
				bgColor = ::GetPixel(hDC, (int)xt, ys);
				red = (int)(distance*GetRValue(bgColor)) + (int)((1.0f-distance)*GetRValue(color));
				green = (int)(distance*GetGValue(bgColor)) + (int)((1.0f-distance)*GetGValue(color));
				blue = (int)(distance*GetBValue(bgColor)) + (int)((1.0f-distance)*GetBValue(color));
				::SetPixel(hDC, (int)xt, ys, RGB(red,green,blue));

				bgColor = ::GetPixel(hDC, (int)xt+1, ys);
				red = (int)((1.0f-distance)*GetRValue(bgColor)) + (int)(distance*GetRValue(color));
				green = (int)((1.0f-distance)*GetGValue(bgColor)) + (int)(distance*GetGValue(color));
				blue = (int)((1.0f-distance)*GetBValue(bgColor)) + (int)(distance*GetBValue(color));
				::SetPixel(hDC, (int)xt+1, ys, RGB(red,green,blue));
			}
			xt += k;
		}
	}
	// Set end pixel
	::SetPixel(hDC, x2, y2, color);
}

void CTGraphics::MoveTo(int x, int y)
{
	m_X=x;
	m_Y=y;
}

void CTGraphics::LineTo(HDC hDC,int x2, int y2, COLORREF color,int nWidth)
{
	int x1;int y1;
	x1=m_X;
	y1=m_Y;
	int i=0;
	while(i<nWidth)
	{
		DrawLine(hDC,x1+i,y1,x2+i,y2,color);
		i++;
	}
}

void CTGraphics::DrawEllipse(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color)
{
	int centerX = (x1 + x2) / 2;
	int centerY = (y1 + y2) / 2;
	int dx = (x2 - x1);
	int dy = (y2 - y1);
	int radiusX = (int)(fabs(dx/2.0f));
	int radiusY = (int)(fabs(dy/2.0f));
	float rel = (float)sqrt(radiusX*radiusX*1.0+ radiusY*radiusY) / (float)radiusY;
	COLORREF bgColor;
	int temp;
	float distance;
	UCHAR red, green, blue;

	if (x2 < x1)
	{
		temp = x1;
		x1 = x2;
		x2 = temp;

		temp = y1;
		y1 = y2;
		y2 = temp;
	}

	int xs;
	float yt;
	for (xs=-radiusX; xs<=radiusX; xs++)
	{
		yt = (float)(radiusY * sqrt(1-(float)(xs*xs)/(float)(radiusX*radiusX)));
		distance = (float)(yt - (int)yt);

		bgColor = ::GetPixel(hDC, centerX+xs, centerY+(int)yt);
		red = (UCHAR)(distance*GetRValue(bgColor)) + (UCHAR)((1.0f-distance)*GetRValue(color));
		green = (UCHAR)(distance*GetGValue(bgColor)) + (UCHAR)((1.0f-distance)*GetGValue(color));
		blue = (UCHAR)(distance*GetBValue(bgColor)) + (UCHAR)((1.0f-distance)*GetBValue(color));
		::SetPixel(hDC, centerX+xs, centerY+(int)yt, RGB(red,green,blue));

		bgColor = ::GetPixel(hDC, centerX+xs, centerY+(int)yt+1);
		red = (UCHAR)((1.0f-distance)*GetRValue(bgColor)) + (UCHAR)(distance*GetRValue(color));
		green = (UCHAR)((1.0f-distance)*GetGValue(bgColor)) + (UCHAR)(distance*GetGValue(color));
		blue = (UCHAR)((1.0f-distance)*GetBValue(bgColor)) + (UCHAR)(distance*GetBValue(color));
		::SetPixel(hDC, centerX+xs, centerY+(int)yt+1, RGB(red,green,blue));

		bgColor = ::GetPixel(hDC, centerX-xs, centerY-(int)yt);
		red = (UCHAR)(distance*GetRValue(bgColor)) + (UCHAR)((1.0f-distance)*GetRValue(color));
		green = (UCHAR)(distance*GetGValue(bgColor)) + (UCHAR)((1.0f-distance)*GetGValue(color));
		blue = (UCHAR)(distance*GetBValue(bgColor)) + (UCHAR)((1.0f-distance)*GetBValue(color));
		::SetPixel(hDC, centerX-xs, centerY-(int)yt, RGB(red,green,blue));

		bgColor = ::GetPixel(hDC, centerX-xs, centerY-(int)yt-1);
		red = (UCHAR)((1.0f-distance)*GetRValue(bgColor)) + (UCHAR)(distance*GetRValue(color));
		green = (UCHAR)((1.0f-distance)*GetGValue(bgColor)) + (UCHAR)(distance*GetGValue(color));
		blue = (UCHAR)((1.0f-distance)*GetBValue(bgColor)) + (UCHAR)(distance*GetBValue(color));
		::SetPixel(hDC, centerX-xs, centerY-(int)yt-1, RGB(red,green,blue));
	}

	int ys;
	float xt;
	for (ys=-(int)((float)radiusY/rel); ys<=(int)((float)radiusY/rel); ys++)
	{
		xt = (float)(radiusX * sqrt(1-(float)(ys*ys)/(float)(radiusY*radiusY)));
		distance = (float)(xt - (int)xt);

		bgColor = ::GetPixel(hDC, centerX+(int)xt, centerY+ys);
		red = (UCHAR)(distance*GetRValue(bgColor)) + (UCHAR)((1.0f-distance)*GetRValue(color));
		green = (UCHAR)(distance*GetGValue(bgColor)) + (UCHAR)((1.0f-distance)*GetGValue(color));
		blue = (UCHAR)(distance*GetBValue(bgColor)) + (UCHAR)((1.0f-distance)*GetBValue(color));
		::SetPixel(hDC, centerX+(int)xt, centerY+ys, RGB(red,green,blue));

		bgColor = ::GetPixel(hDC, centerX+(int)xt+1, centerY+ys);
		red = (UCHAR)((1.0f-distance)*GetRValue(bgColor)) + (UCHAR)(distance*GetRValue(color));
		green = (UCHAR)((1.0f-distance)*GetGValue(bgColor)) + (UCHAR)(distance*GetGValue(color));
		blue = (UCHAR)((1.0f-distance)*GetBValue(bgColor)) + (UCHAR)(distance*GetBValue(color));
		::SetPixel(hDC, centerX+(int)xt+1, centerY+ys, RGB(red,green,blue));

		bgColor = ::GetPixel(hDC, centerX-(int)xt, centerY-ys);
		red = (UCHAR)(distance*GetRValue(bgColor)) + (UCHAR)((1.0f-distance)*GetRValue(color));
		green = (UCHAR)(distance*GetGValue(bgColor)) + (UCHAR)((1.0f-distance)*GetGValue(color));
		blue = (UCHAR)(distance*GetBValue(bgColor)) + (UCHAR)((1.0f-distance)*GetBValue(color));
		::SetPixel(hDC, centerX-(int)xt, centerY-ys, RGB(red,green,blue));

		bgColor = ::GetPixel(hDC, centerX-(int)xt-1, centerY-ys);
		red = (UCHAR)((1.0f-distance)*GetRValue(bgColor)) + (UCHAR)(distance*GetRValue(color));
		green = (UCHAR)((1.0f-distance)*GetGValue(bgColor)) + (UCHAR)(distance*GetGValue(color));
		blue = (UCHAR)((1.0f-distance)*GetBValue(bgColor)) + (UCHAR)(distance*GetBValue(color));
		::SetPixel(hDC, centerX-(int)xt-1, centerY-ys, RGB(red,green,blue));
	}
}

void CTGraphics::DrawArc(HDC hDC, int x1, int y1, int x2, int y2, float startAngle, float endAngle, COLORREF color)
{
	int centerX = (x1 + x2) / 2;
	int centerY = (y1 + y2) / 2;
	int dx = (x2 - x1);
	int dy = (y2 - y1);
	int radiusX = abs(dx)/2;
	int radiusY = abs(dy)/2;
	COLORREF bgColor;
	float distance;
	UCHAR red, green, blue;

	float angle = 0.0f;
	while ((angle+90.0f) < startAngle)
		angle += 90.0f;
	while (angle < endAngle)
	{
		if (startAngle >= angle)
		{
			float eAngle;
			if (endAngle <= angle+90.0f)
				eAngle = endAngle;
			else
				eAngle = angle+90.0f;

			float sx = (float)(radiusX*cos((startAngle/180.0f)*3.1415f));
			if (sx > 0.0f)
				sx += 0.5f;
			else
				sx -= 0.5f;
			int startX = (int)sx;
			int endX = (int)(radiusX*cos((eAngle/180.0f)*3.1415f));
			int deltaX;
			if (endX - startX != 0)
				deltaX = (int)((endX-startX) / abs(endX-startX));
			else
				deltaX = 0;

			float sy = (float)(radiusY*sin((startAngle/180.0f)*3.1415f));
			if (sy > 0.0f)
				sy += 0.5f;
			else
				sy -= 0.5f;
			int startY = (int)sy;
			int endY = (int)(radiusY*sin((eAngle/180.0f)*3.1415f));
			int deltaY;
			if (endY - startY != 0)
				deltaY = (int)((endY-startY) /abs(endY-startY));
			else
				deltaY = 0;

			if (deltaX != 0)
			{
				int oldY = startY;
				int xs = startX;
				float yt;
				while (xs != endX)
				{
					yt = (float)(radiusY * sqrt(1.0f-(float)(xs*xs)/(float)(radiusX*radiusX)));
					distance = (float)(yt - (int)yt);

					if (abs(oldY - (int)yt) < 2)
					{
						if (deltaX < 0)
						{
							if (deltaY > 0)
								bgColor = ::GetPixel(hDC, centerX+xs, centerY+(int)yt);
							else
								bgColor = ::GetPixel(hDC, centerX+xs, centerY+(int)yt);
						}
						else
						{
							if (deltaY < 0)
								bgColor = ::GetPixel(hDC, centerX+xs, centerY-(int)yt);
							else
								bgColor = ::GetPixel(hDC, centerX+xs, centerY-(int)yt);
						}
						red = (UCHAR)(distance*GetRValue(bgColor)) + (UCHAR)((1.0f-distance)*GetRValue(color));
						green = (UCHAR)(distance*GetGValue(bgColor)) + (UCHAR)((1.0f-distance)*GetGValue(color));
						blue = (UCHAR)(distance*GetBValue(bgColor)) + (UCHAR)((1.0f-distance)*GetBValue(color));
						if (deltaX < 0)
						{
							if (deltaY > 0)
								::SetPixel(hDC, centerX+xs, centerY+(int)yt, RGB(red,green,blue));
							else
								::SetPixel(hDC, centerX+xs, centerY+(int)yt, RGB(red,green,blue));
						}
						else
						{
							if (deltaY < 0)
								::SetPixel(hDC, centerX+xs, centerY-(int)yt, RGB(red,green,blue));
							else
								::SetPixel(hDC, centerX+xs, centerY-(int)yt, RGB(red,green,blue));
						}

						if (deltaX < 0)
						{
							if (deltaY > 0)
								bgColor = ::GetPixel(hDC, centerX+xs, centerY+(int)yt+1);
							else
								bgColor = ::GetPixel(hDC, centerX+xs, centerY+(int)yt+1);
						}
						else
						{
							if (deltaY < 0)
								bgColor = ::GetPixel(hDC, centerX+xs, centerY-(int)yt-1);
							else
								bgColor = ::GetPixel(hDC, centerX+xs, centerY-(int)yt-1);
						}
						red = (UCHAR)((1.0f-distance)*GetRValue(bgColor)) + (UCHAR)(distance*GetRValue(color));
						green = (UCHAR)((1.0f-distance)*GetGValue(bgColor)) + (UCHAR)(distance*GetGValue(color));
						blue = (UCHAR)((1.0f-distance)*GetBValue(bgColor)) + (UCHAR)(distance*GetBValue(color));
						if (deltaX < 0)
						{
							if (deltaY > 0)
								::SetPixel(hDC, centerX+xs, centerY+(int)yt+1, RGB(red,green,blue));
							else
								::SetPixel(hDC, centerX+xs, centerY+(int)yt+1, RGB(red,green,blue));
						}
						else
						{
							if (deltaY < 0)
								::SetPixel(hDC, centerX+xs, centerY-(int)yt-1, RGB(red,green,blue));
							else
								::SetPixel(hDC, centerX+xs, centerY-(int)yt-1, RGB(red,green,blue));
						}
					}
					
					oldY = (int)yt;
					xs += deltaX;
				}
			}

			if (deltaY != 0)
			{
				int oldX = startX;
				int ys = startY;
				float xt;
				while (ys != endY)
				{
					xt = (float)(radiusX * sqrt(1.0f-(float)(ys*ys)/(float)(radiusY*radiusY)));
					distance = (float)(xt - (int)xt);

					if (abs(oldX - (int)xt) < 2)
					{
						if (deltaX < 0)
						{
							if (deltaY > 0)
								bgColor = ::GetPixel(hDC, centerX+(int)xt, centerY+ys);
							else
								bgColor = ::GetPixel(hDC, centerX-(int)xt, centerY+ys);
						}
						else
						{
							if (deltaY < 0)
								bgColor = ::GetPixel(hDC, centerX-(int)xt, centerY+ys);
							else
								bgColor = ::GetPixel(hDC, centerX+(int)xt, centerY+ys);
						}
						red = (UCHAR)(distance*GetRValue(bgColor)) + (UCHAR)((1.0f-distance)*GetRValue(color));
						green = (UCHAR)(distance*GetGValue(bgColor)) + (UCHAR)((1.0f-distance)*GetGValue(color));
						blue = (UCHAR)(distance*GetBValue(bgColor)) + (UCHAR)((1.0f-distance)*GetBValue(color));
						if (deltaX < 0)
						{
							if (deltaY > 0)
								::SetPixel(hDC, centerX+(int)xt, centerY+ys, RGB(red,green,blue));
							else
								::SetPixel(hDC, centerX-(int)xt, centerY+ys, RGB(red,green,blue));
						}
						else
						{
							if (deltaY < 0)
								::SetPixel(hDC, centerX-(int)xt, centerY+ys, RGB(red,green,blue));
							else
								::SetPixel(hDC, centerX+(int)xt, centerY+ys, RGB(red,green,blue));
						}

						if (deltaX < 0)
						{
							if (deltaY > 0)
								bgColor = ::GetPixel(hDC, centerX+(int)xt+1, centerY+ys);
							else
								bgColor = ::GetPixel(hDC, centerX-(int)xt-1, centerY+ys);
						}
						else
						{
							if (deltaY < 0)
								bgColor = ::GetPixel(hDC, centerX-(int)xt-1, centerY+ys);
							else
								bgColor = ::GetPixel(hDC, centerX+(int)xt+1, centerY+ys);
						}
						red = (UCHAR)((1.0f-distance)*GetRValue(bgColor)) + (UCHAR)(distance*GetRValue(color));
						green = (UCHAR)((1.0f-distance)*GetGValue(bgColor)) + (UCHAR)(distance*GetGValue(color));
						blue = (UCHAR)((1.0f-distance)*GetBValue(bgColor)) + (UCHAR)(distance*GetBValue(color));
						if (deltaX < 0)
						{
							if (deltaY > 0)
								::SetPixel(hDC, centerX+(int)xt+1, centerY+ys, RGB(red,green,blue));
							else
								::SetPixel(hDC, centerX-(int)xt-1, centerY+ys, RGB(red,green,blue));
						}
						else
						{
							if (deltaY < 0)
								::SetPixel(hDC, centerX-(int)xt-1, centerY+ys, RGB(red,green,blue));
							else
								::SetPixel(hDC, centerX+(int)xt+1, centerY+ys, RGB(red,green,blue));
						}
					}

					oldX = (int)xt;
					ys += deltaY;
				}
			}
		}

		angle += 90.0f;
		startAngle = angle;
	}
}

void CTGraphics::DrawRoundRect(HDC hDC, int x1, int y1, int x2, int y2, int width, int height, COLORREF color)
{
	int dx = (x2 - x1);
	int dy = (y2 - y1);
	int offsetX = width / 2;
	width = offsetX * 2;
	int offsetY = height / 2;
	height = offsetY * 2;

	if ((width > abs(dx)) || (height > abs(dy)))
	{
		DrawEllipse(hDC, x1, y1, x2, y2, color);
	}
	else
	{
		DrawLine(hDC, x1+offsetX, y1, x2-offsetX, y1, color);
		DrawArc(hDC, x2-width, y1, x2, y1+height, 270.0f, 360.0f, color);
		DrawLine(hDC, x2, y1+offsetY, x2, y2-offsetY, color);
		DrawArc(hDC, x2-width, y2-height, x2, y2, 0.0f, 90.0f, color);
		DrawLine(hDC, x2-offsetX, y2, x1+offsetX, y2, color);
		DrawArc(hDC, x1, y2-height, x1+width, y2, 90.0f, 180.0f, color);
		DrawLine(hDC, x1, y2-offsetY, x1, y1+offsetY, color);
		DrawArc(hDC, x1, y1, x1+width, y1+height, 180.0f, 270.0f, color);
	}
}

void CTGraphics::DrawPolyline(HDC hDC, POINT* points, int numberPoints, COLORREF color)
{
	for (int i=0; i<numberPoints-1; i++)
	{
		DrawLine(hDC, points[i].x, points[i].y, points[i+1].x, points[i+1].y, color);
	}
}

void CTGraphics::DrawPie(HDC hDC, int x1, int y1, int x2, int y2, float startAngle, float endAngle, COLORREF color)
{
	int centerX = (x1 + x2) / 2;
	int centerY = (y1 + y2) / 2;
	int dx = (x2 - x1);
	int dy = (y2 - y1);
	int radiusX = (abs(dx) / 2);
	int radiusY = (abs(dy) / 2);

	int startX = (int)(radiusX*cos((startAngle/180.0f)*3.1415f));
	int startY = (int)(radiusY*sin((startAngle/180.0f)*3.1415f));
	int endX = (int)(radiusX*cos((endAngle/180.0f)*3.1415f));
	int endY = (int)(radiusY*sin((endAngle/180.0f)*3.1415f));

	DrawArc(hDC, x1, y1, x2, y2, startAngle, endAngle, color);
	DrawLine(hDC, centerX, centerY, centerX+startX, centerY+startY, color);
	DrawLine(hDC, centerX, centerY, centerX+endX, centerY+endY, color);
}

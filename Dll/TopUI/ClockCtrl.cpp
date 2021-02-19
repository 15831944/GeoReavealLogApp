/*
Module : CLOCKCTRL.CPP
Purpose: Implementation for an MFC GUI control which implements
         an analog clock
Created: PJN / 01-02-2000
History: PJN / 16-02-2000 1. Removed noticable flicker which sometimes occurred 
                             when the seconds hand moved
         PJN / 27-07-2001 1. Fixed a drawing glitch which left the old hour hand 
                             being displayed every 12 minutes
                          2. Made most of the functions virtual to allow easier
                          customisation.
         PJN / 01-08-2001 1. Second hand can now be updated with sub second precision
                          with the "SetSubSecondPrecision" method
         PJN / 23-03-2001 1. Updated Copyright details.
                          2. Fixed a bug where PreSubclassWindow was not calling 
                          RecalcLayout before a redraw was occurring.

Copyright (c) 2000 - 2002 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/

/////////////////////////////////  Includes  //////////////////////////////////
#include "stdafx.h"
#include <math.h>
#include "ClockCtrl.h"

///////////////////////////////// Macros //////////////////////////////////////

const double PI = 3.14159;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



///////////////////////////////// Implementation //////////////////////////////

BEGIN_MESSAGE_MAP(CClockCtrl, CStatic)
  //{{AFX_MSG_MAP(CClockCtrl)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CClockCtrl::CClockCtrl()
{
  //Initialize the member variables to their default values
  m_ColorHourMinutesHand = GetSysColor(COLOR_BTNFACE);
  m_ColorPoints = GetSysColor(COLOR_BTNFACE);
  m_nXRadius = -1;
  m_nYRadius = -1;
  m_nPointWidth = -1;
  m_nHour = 0;
  m_nHourPosAsMinutes = 0;
  m_nMinute = 0;
  m_nMillisecond = 0;
  m_nSecond = 0;
  m_nTimerID = 0;
  m_bEnableRealtime = TRUE;
  m_bShowHours = TRUE;
  m_bShowMinutes = TRUE;
  m_bShowSeconds = TRUE;
  m_bShowPoints = TRUE;
  m_bShowMinorPoints = FALSE;
  m_ColorBackground = GetSysColor(COLOR_HIGHLIGHT);//GetSysColor(COLOR_BTNFACE);
  m_b3dPoints = TRUE;
  m_bSubSecondPrecision = TRUE;
}

void CClockCtrl::PreSubclassWindow() 
{
  //Let the parent class do its thing
  CStatic::PreSubclassWindow();

  //Setup the timer if we are displaying realtime values
  if (m_bEnableRealtime)
  {
    RecalcLayout();
    SetRealTime(TRUE);
  }
}

void CClockCtrl::OnPaint() 
{
  //Force a recalc if not initialized
  if (m_nXRadius == -1)
    RecalcLayout();

  // device context for painting
  CPaintDC dc(this);

  //Setup the brush we are going to use
  CBrush PointBrush(m_ColorPoints);
  CBrush* pOldBrush = dc.SelectObject(&PointBrush);

  //Draw the points going around the clock
  if (m_bShowPoints)
  {
    for (int nMinute=0; nMinute<60; nMinute++)
    {
      //Work out the face length which is different 
      //depending on where we are on the ellipse edge
      CPoint p = ComputePoint(nMinute, 0.9);
      if ((nMinute % 5) == 0)
        DrawFacePoint(dc, p, TRUE);
      else if (m_bShowMinorPoints)
        DrawFacePoint(dc, p, FALSE);
    }
  }
  //Restore the DC
  dc.SelectObject(pOldBrush);

  //Then draw all the hands
  DrawHands(dc, m_nHour, m_nMinute, m_nSecond, m_nMillisecond, TRUE);
}

void CClockCtrl::DrawHands(CDC& dc, int nHour, int nMinute, int nSecond, int nMillisecond, BOOL bDrawAll)
{
  //Validate our parameters
  ASSERT(nHour >= 0 && nHour <= 12);
  ASSERT(nMinute >= 0 && nMinute <= 59);
  ASSERT(nSecond >= 0 && nSecond <= 59);
  ASSERT(nMillisecond >= 0 && nMillisecond <= 999);

  //Setup the brush we are going to use
  CBrush PointBrush(m_ColorHourMinutesHand);
  CBrush* pOldBrush = dc.SelectObject(&PointBrush);

  int nHourPosAsMinutes = (m_nHour*5) + (m_nMinute/12);
  if (bDrawAll)
  {
    //Draw the hour hand
    if (m_bShowHours)
      DrawHand(dc, nHourPosAsMinutes, HOUR, TRUE);

    //Draw the minute hand
    if (m_bShowMinutes)
      DrawHand(dc, nMinute, MINUTE, TRUE);

    //Draw the seconds hand
    if (m_bShowSeconds)
    {
      if (m_bSubSecondPrecision)
        DrawSecondHand(dc, nSecond, nMillisecond);
      else
        DrawHand(dc, nSecond, SECOND, TRUE);
    }
  }
  else
  {
    //Should we do a complete redraw
    BOOL bCompleteRedraw = ((m_nHour != nHour) || (m_nMinute != nMinute) || (m_nHourPosAsMinutes != nHourPosAsMinutes));
    if (bCompleteRedraw)
    {
      //Erase the seconds hand
      if (m_bShowSeconds)
      {
        if (m_bSubSecondPrecision)
          DrawSecondHand(dc, m_nSecond, m_nMillisecond);
        else
          DrawHand(dc, m_nSecond, SECOND, FALSE);
      }

      //Erase the hour hand
      if (m_bShowHours)
        DrawHand(dc, m_nHourPosAsMinutes, HOUR, FALSE);

      //Erase the minute hand
      if (m_bShowMinutes)
        DrawHand(dc, m_nMinute, MINUTE, FALSE);

      //Draw the hour hand
      if (m_bShowHours)
        DrawHand(dc, nHourPosAsMinutes, HOUR, TRUE);

      //Draw the minute hand
      if (m_bShowMinutes)
        DrawHand(dc, nMinute, MINUTE, TRUE);

      //Draw the seconds hand
      if (m_bShowSeconds)
      {
        if (m_bSubSecondPrecision)
          DrawSecondHand(dc, nSecond, nMillisecond);
        else
          DrawHand(dc, nSecond, SECOND, TRUE);
      }
    }
    else
    {
      //Erase the seconds hand
      if (m_bShowSeconds)
      {
        if (m_bSubSecondPrecision)
          DrawSecondHand(dc, m_nSecond, m_nMillisecond);
        else
          DrawHand(dc, m_nSecond, SECOND, FALSE);
      }

      //Draw the seconds hand
      if (m_bShowSeconds)
      {
        if (m_bSubSecondPrecision)
          DrawSecondHand(dc, nSecond, nMillisecond);
        else
          DrawHand(dc, nSecond, SECOND, TRUE);
      }
    }
  }

  //Store away the new values
  m_nHour = nHour;
  m_nHourPosAsMinutes = nHourPosAsMinutes;
  m_nMinute = nMinute;
  m_nSecond = nSecond;
  m_nMillisecond = nMillisecond;

  //Restore the DC
  dc.SelectObject(pOldBrush);
}

void CClockCtrl::SetTime(int nHour, int nMinute, int nSecond, int nMillisecond)
{
  //Validate our parameters
  ASSERT(m_hWnd);
  ASSERT(nHour >= 0 && nHour <= 12);
  ASSERT(nMinute >= 0 && nMinute <= 59);
  ASSERT(nSecond >= 0 && nSecond <= 59);
  ASSERT(nMillisecond >= 0 && nMillisecond <= 999);

  //Do the updated drawing of the hands
  CClientDC dc(this);                           
  DrawHands(dc, nHour, nMinute, nSecond, nMillisecond, FALSE);
}

double CClockCtrl::MinuteToRadian(double minute)
{
  return (minute-15)*PI/30;
}

CPoint CClockCtrl::ComputeSecondsPoint(double Minute, double ratio)
{
  CPoint point;

  double angle = MinuteToRadian(Minute);
  point.x = m_MiddlePoint.x + (int) (m_nXRadius*ratio*cos(angle) + 0.5);
  point.y = m_MiddlePoint.y + (int) (m_nYRadius*ratio*sin(angle) + 0.5);

  return point;
}

CPoint CClockCtrl::ComputePoint(int nMinute, double ratio)
{
  CPoint point;

  double angle = MinuteToRadian(nMinute);
  point.x = m_MiddlePoint.x + (int) (m_nXRadius*ratio*cos(angle) + 0.5);
  point.y = m_MiddlePoint.y + (int) (m_nYRadius*ratio*sin(angle) + 0.5);

  return point;
}

void CClockCtrl::DrawFacePoint(CDC& dc, const CPoint& point, BOOL bMajor)
{
  //Work out the size of the point rectangle
 /* CRect rPoint(point, point);
  if (bMajor)
  {
    int nPointRadius = m_nPointWidth / 2 + 1;
    rPoint.InflateRect(nPointRadius, nPointRadius);

    //Do the actual point drawing
    dc.Rectangle(&rPoint);
    if (m_b3dPoints)
      dc.Draw3dRect(&rPoint, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW));
  }
  else
  {
    if (m_bShowMinorPoints)
    {
      rPoint.InflateRect(1, 1);

      //Do the actual point drawing
      dc.Draw3dRect(&rPoint, GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW));
    }
  }*/
}

void CClockCtrl::DrawSecondHand(CDC& dc, int nSecond, int nMilliSecond)
{
  CPoint handPoints[2];
  GetSecondsHandPoints(nSecond + nMilliSecond/1000.0, handPoints);

  //Draw the hand
  int nOldRop = dc.SetROP2(R2_NOTXORPEN);
  dc.MoveTo(handPoints[0]);
  dc.LineTo(handPoints[1]);
  dc.SetROP2(nOldRop);
}

void CClockCtrl::DrawHand(CDC& dc, int nMinute, HandType type, BOOL bDraw)
{
  //Calculate the point positions of the hand
  CPoint handPoints[4];
  GetHandPoints(nMinute, type, handPoints);

  if (type == SECOND)
  {
    //Draw the hand
    int nOldRop = dc.SetROP2(R2_NOTXORPEN);
    dc.MoveTo(handPoints[0]);
    dc.LineTo(handPoints[1]);
    dc.SetROP2(nOldRop);
  }
  else
  {
    //Pick the correct colors to be used
    COLORREF colorBrush;
    COLORREF colorPen;
    if (bDraw)
    {
      colorBrush = m_ColorHourMinutesHand;
      colorPen = RGB(0, 255, 0);
    }
    else
    {
      colorBrush = m_ColorBackground;
      colorPen = m_ColorBackground;
    }

    //Setup the DC
    CBrush brushHand(colorBrush);
    CPen penHand(PS_SOLID, 1, colorPen);
    CBrush* pOldBrush = dc.SelectObject(&brushHand);
    CPen* pOldPen = dc.SelectObject(&penHand);

    //Draw the hand
    dc.Polygon(handPoints, 4);

    //Restore the DC
    dc.SelectObject(pOldPen);
    dc.SelectObject(pOldBrush);
  }
}

void CClockCtrl::GetSecondsHandPoints(double Value, CPoint* pPoints)
{
  ASSERT(pPoints);

  //Second hand goes out to 80% of the radius
  double ratio = 0.8;

  pPoints[0] = m_MiddlePoint;
  pPoints[1] = ComputeSecondsPoint(Value, ratio);
}


void CClockCtrl::GetHandPoints(int nValue, HandType type, CPoint* pPoints)
{
  ASSERT(pPoints);

  double ratio = 0;
  switch (type)
  {
    case HOUR:
    {
      //hour hand goes out to 50% of the radius
      ratio = 0.5;
      break;
    }
    case MINUTE:
    {
      //Minute hand goes out to 70% of the radius
      ratio = 0.7;
      break;
    }
    case SECOND:
    {
      //Second hand goes out to 80% of the radius
      ratio = 0.8;
      break;
    }
    default:
    {
      ASSERT(FALSE);
      break;
    }
  }

  if (type == SECOND)
  {
    pPoints[0] = m_MiddlePoint;
    pPoints[1] = ComputePoint(nValue, ratio);
  }
  else
  {
    //Compute the face points, First point is the back side,
    //second point is the right, third point is the top,
    //and the fourth is left.
    pPoints[0] = ComputePoint(nValue+30, 0.1);
    pPoints[1] = ComputePoint(nValue+15, 0.05);
    pPoints[2] = ComputePoint(nValue,    ratio);
    pPoints[3] = ComputePoint(nValue-15, 0.05);
  }
}

void CClockCtrl::OnSize(UINT nType, int cx, int cy) 
{
  //Let the parent class do its thing
  CStatic::OnSize(nType, cx, cy);

  //Force a recalc of the internal control metrics
  RecalcLayout();
}

void CClockCtrl::RecalcLayout()
{
  //Get the window rect
  CRect rClient;
  GetClientRect(&rClient);

  //Calculate the middle point
  m_MiddlePoint.x = rClient.Width() / 2;
  m_MiddlePoint.y = rClient.Height() / 2;

  //Calculate the radius
  m_nXRadius = m_MiddlePoint.x;
  m_nYRadius = m_MiddlePoint.y;

  //Calculate the width of the marks going around the clock
  m_nPointWidth = min(m_nXRadius, m_nYRadius) / 20;
  if (m_nPointWidth < 2)
    m_nPointWidth = 2;

  Invalidate(TRUE);
}

void CClockCtrl::OnTimer(UINT nIDEvent) 
{
  //Let the parent class do its thing
  CStatic::OnTimer(nIDEvent);

  if (m_nTimerID == nIDEvent)
  {
    //Get the local time and force a redraw
    SYSTEMTIME st;
    GetLocalTime(&st);
    SetTime(st.wHour % 12, st.wMinute, st.wSecond, st.wMilliseconds);
  }
}

void CClockCtrl::SetRealTime(BOOL bRealTime)
{
  m_bEnableRealtime = bRealTime;

  //Force an explicit refresh of the current state before we go realtime
  if (m_bEnableRealtime && m_hWnd)
  {
    CClientDC dc(this);                           

    //Redraw the background
    CRect rClient;
    GetClientRect(&rClient);
    dc.FillSolidRect(rClient, m_ColorBackground);

    //Redraw the hand
    DrawHands(dc, m_nHour, m_nMinute, m_nSecond, m_nMillisecond, TRUE);
  }

  if (bRealTime)
  {
    if (m_nTimerID == 0)
    {
      SYSTEMTIME st;
      GetLocalTime(&st);
      SetTime(st.wHour % 12, st.wMinute, st.wSecond, st.wMilliseconds);

      if (m_bSubSecondPrecision)
        m_nTimerID = SetTimer(1, 50, NULL); //Use a higher accuracy timer if we are doing sub second drawing
      else
        m_nTimerID = SetTimer(1, 100, NULL);
    }
  }
  else
  {
    if (m_nTimerID)
    {
      KillTimer(m_nTimerID);
      m_nTimerID = 0;
    }
  }
}

BOOL CClockCtrl::GetRealTime() const
{
  return (m_nTimerID != 0);
}

void CClockCtrl::OnDestroy() 
{
  //Turn off timers if we are using them
  SetRealTime(FALSE);

  //Let the parent class do its thing
  CStatic::OnDestroy();
}

void CClockCtrl::SetShowHours(BOOL bShowHours)
{
  if (bShowHours != m_bShowHours)
  {
    m_bShowHours = bShowHours;
    if (m_hWnd)
      RedrawWindow();
  }
}

void CClockCtrl::SetShowMinutes(BOOL bShowMinutes)
{
  if (bShowMinutes != m_bShowMinutes)
  {
    m_bShowMinutes = bShowMinutes;
    if (m_hWnd)
      RedrawWindow();
  }
}

void CClockCtrl::SetShowSeconds(BOOL bShowSeconds)
{
  if (bShowSeconds != m_bShowSeconds)
  {
    m_bShowSeconds = bShowSeconds;
    if (m_hWnd)
      RedrawWindow();
  }
}

void CClockCtrl::SetShowPoints(BOOL bShowPoints)
{
  if (bShowPoints != m_bShowPoints)
  {
    m_bShowPoints = bShowPoints;
    if (m_hWnd)
      RedrawWindow();
  }
}

void CClockCtrl::SetShowMinorPoints(BOOL bShowMinorPoints)
{
  if (bShowMinorPoints != m_bShowMinorPoints)
  {
    m_bShowMinorPoints = bShowMinorPoints;
    if (m_hWnd)
      RedrawWindow();
  }
}

void CClockCtrl::SetHourMinutesHandColor(COLORREF color)
{
  if (color != m_ColorHourMinutesHand)
  {
    m_ColorHourMinutesHand = color;
    if (m_hWnd)
      RedrawWindow();
  }
}

void CClockCtrl::SetBackgroundColor(COLORREF color)
{
  if (color != m_ColorBackground)
  {
    m_ColorBackground = color;
    if (m_hWnd)
      RedrawWindow();
  }
}

void CClockCtrl::Set3dPoints(BOOL b3dPoints)
{
  if (b3dPoints != m_b3dPoints)
  {
    m_b3dPoints = b3dPoints;
    if (m_hWnd)
      RedrawWindow();
  }
}

void CClockCtrl::SetSubSecondPrecision(BOOL bSubSecondPrecision)
{
  if (bSubSecondPrecision != m_bSubSecondPrecision)
  {
    m_bSubSecondPrecision = bSubSecondPrecision;
    if (m_hWnd)
      RedrawWindow();
  }
}

BOOL CClockCtrl::OnEraseBkgnd(CDC* pDC) 
{
  //CRect rClient;
 // GetClientRect(&rClient);
 // pDC->FillSolidRect(rClient, m_ColorBackground);
  return TRUE;
}

void CClockCtrl::Refrush()
{
	CClientDC dc(this); 
	DrawHands(dc, 0, 0, 0, 0, TRUE);
}

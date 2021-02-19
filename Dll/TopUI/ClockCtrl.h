/*
Module : CLOCKCTRL.H
Purpose: Defines the interface for an MFC GUI control which implements
         an analog clock
Created: PJN / 01-02-2000

Copyright (c) 2000 - 2002 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/

////////////////////////////////// Macros ///////////////////////////

#ifndef _CLOCKCTRL_H__
#define _CLOCKCTRL_H__
////////////////////////////////// Classes //////////////////////////

class AFX_EXT_CLASS CClockCtrl : public CStatic
{
public:
	void Refrush();
//Constructors / Destructors
  CClockCtrl();

//Accessors
  int      GetHour() const { return m_nHour; };
  int      GetMinute() const { return m_nMinute; };
  int      GetSecond() const { return m_nSecond; };
  BOOL     GetRealTime() const;
  BOOL     GetShowHours() const { return m_bShowHours; };
  BOOL     GetShowMinutes() const { return m_bShowMinutes; };
  BOOL     GetShowSeconds() const { return m_bShowSeconds; };
  BOOL     GetShowPoints() const { return m_bShowPoints; };
  BOOL     GetShowMinorPoints() const { return m_bShowMinorPoints; };
  COLORREF GetHourMinutesHandColor() const { return m_ColorHourMinutesHand; };
  COLORREF GetBackgroundColor() const { return m_ColorBackground; };
  BOOL     Get3dPoints() const { return m_b3dPoints; };
  BOOL     GetSubSecondPrecision() const { return m_bSubSecondPrecision; };

//Mutators
  void SetTime(int nHour, int nMinute, int nSecond, int nMillisecond=0);
  void SetRealTime(BOOL bRealTime);
  void SetShowHours(BOOL bShowHours);
  void SetShowMinutes(BOOL bShowMinutes);
  void SetShowSeconds(BOOL bShowSeconds);
  void SetShowPoints(BOOL bShowPoints);
  void SetShowMinorPoints(BOOL bShowMinorPoints);
  void SetHourMinutesHandColor(COLORREF color);
  void SetBackgroundColor(COLORREF color);
  void Set3dPoints(BOOL b3dPoints);
  void SetSubSecondPrecision(BOOL bSubSecondPrecision);

protected:
  enum HandType
  {
    HOUR,
    MINUTE,
    SECOND
  }; 

  //{{AFX_VIRTUAL(CClockCtrl)
  virtual void PreSubclassWindow();
  //}}AFX_VIRTUAL

  //{{AFX_MSG(CClockCtrl)
  afx_msg void OnPaint();
  afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

  void   RecalcLayout();
  double MinuteToRadian(double minute);
  virtual CPoint ComputePoint(int nMinute, double ratio);
  virtual CPoint ComputeSecondsPoint(double Minute, double ratio);
  virtual void DrawFacePoint(CDC& dc, const CPoint& point, BOOL bMajor);
  virtual void DrawHands(CDC& dc, int nHour, int nMinute, int nSecond, int nMillisecond, BOOL bDrawAll);
  virtual void DrawHand(CDC& dc, int nMinute, HandType type, BOOL bDraw);
  virtual void GetHandPoints(int nValue, HandType type, CPoint* pPoints);
  virtual void GetSecondsHandPoints(double Value, CPoint* pPoints);
  virtual void DrawSecondHand(CDC& dc, int nSecond, int nMilliSecond);

//Member variables
  COLORREF m_ColorHourMinutesHand;
  COLORREF m_ColorPoints;
  COLORREF m_ColorBackground;
  CPoint   m_MiddlePoint;
  int      m_nXRadius;
  int      m_nYRadius;
  int      m_nPointWidth;
  int      m_nHourPosAsMinutes;
  int      m_nHour;
  int      m_nMinute;
  int      m_nSecond;
  int      m_nMillisecond;
  UINT     m_nTimerID;
  BOOL     m_bEnableRealtime;
  BOOL     m_bShowHours;
  BOOL     m_bShowMinutes;
  BOOL     m_bShowSeconds;
  BOOL     m_bShowPoints;
  BOOL     m_bShowMinorPoints;
  BOOL     m_b3dPoints;
  BOOL     m_bSubSecondPrecision;

  DECLARE_MESSAGE_MAP()
};

#endif //_CLOCKCTRL_H__

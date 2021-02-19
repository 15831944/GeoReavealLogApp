#pragma once
// CCurveView 视图

class CCurveView : public CScrollView
{
	DECLARE_DYNCREATE(CCurveView)

public:
	CSize m_nScrollSize;
	CString		m_szServer;	

	BOOL InitData();
	void DrawCurve(CDC *pDC);
	CString m_WellPath;			//井数据所在路径
	DWORD m_CurveID;
	CString m_CurveName;		//曲线名
	bool bReadCurve;			//是否已经读了曲线数据
	float Stdep,Endep,Rlev;		//数据的起止深度及采样间隔
	float CurveMin,CurveMax;	//绘图左右刻度
	int NumSam;					//曲线采样点数
	float *CurveData;			//曲线数据
	float Sdep,Edep;			//绘图起止深度
	CRect MyRect;				//绘图区域
	int MyBottom;				//客户区底部用于显示状态信息的高度
	int MyLeft;					//客户区左边用于显示按钮的宽度
	int TopInsert;				//顶部图例高度
	int LineWidth,LineColor;	//线宽及线色
	int LineLog;				//线性/对数开关，0为线性，1为对数

public:
	CCurveView();				// 动态创建所使用的受保护的构造函数
protected:	
	virtual ~CCurveView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual void OnInitialUpdate();     // 构造后的第一次
protected:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制此视图

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
};



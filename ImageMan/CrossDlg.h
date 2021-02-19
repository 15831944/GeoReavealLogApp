#pragma once

#define SCALE_LIN				0
#define SCALE_LOG				1

// CCrossDlg 对话框

class CCrossDlg : public CDialog
{
	DECLARE_DYNAMIC(CCrossDlg)

public:
	CCrossDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCrossDlg();

// 对话框数据
	enum { IDD = IDD_CROSS_DIALOG };
public:

	int		m_CrossData[3][100][100];	//折算到网格的交会点数 第1维为3种类型
	int     m_CrossX[100];				//X轴直方图数据
	int     m_CrossY[100];				//Y轴直方图数据
	
	int 	MarginX;
	int 	MarginY;	
	WORD 	LengthX;		//主绘图区的宽度
	WORD 	LengthY;		//主绘图区的高度

	LOGFONT m_AxiasTextFont,m_ScaleTextFont;

public: 
	COLORREF m_bkColor,m_AxiasTextColor,m_ScaleTextColor;
	LOGPEN  m_LogPen1,m_LogPen2,m_LogPen3,m_LogPen4;
	

	CString m_szAliaseX;	//横坐标别名	
	float	m_StartX;		//横坐标初值	
	float	m_ScaleIncX;	//横坐标增量	
	WORD	m_ScaleNumX;	//横坐标刻度数	
	WORD	m_ScaleTypeX;	//横坐标刻度类型

	CString m_szAliaseY;	//纵坐标别名		
	float	m_StartY;		//纵坐标初值	
	float	m_ScaleIncY;	//纵坐标增量	
	WORD	m_ScaleNumY;	//纵坐标刻度数	
	WORD	m_ScaleTypeY;	//纵坐标刻度类型

	CPoint 	m_nPos;
	CPoint 	m_nPosLT;
	CPoint 	m_nPosRB;	

	//用于直方图颜色显示
	float m_ImageMin,m_ImageMax;
public:
	void dtos(double value, char* ss);
	void DrawAxias(CDC* pDC);
	void DrawCrossData(CDC* pDC);
	void OnSendToClip();
	void XFrequency();
	void YFrequency();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual BOOL OnInitDialog();
};

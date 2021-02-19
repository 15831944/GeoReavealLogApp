#if !defined(AFX_CURVEVIEW_H__7B8222F2_15BD_41F1_B271_706357EB7428__INCLUDED_)
#define AFX_CURVEVIEW_H__7B8222F2_15BD_41F1_B271_706357EB7428__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CurveView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCurveView dialog
#include "FIDres.h"
#include "DragDialog.h"
#include "WndShadow.h"
class CCurveView : public CDragDialog
{
// Construction
public:
	CCurveView(char* CurverName,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCurveView)
	enum { IDD = IDD_LOGVIEW };
	CStatic	m_Value;
	//}}AFX_DATA
public:
	void CleanUp();
	void OnFreshBtn();
	CString m_WellPath; //井数据所在路径
	CString m_IndexFile;  //索引文件名
	CString m_DataFile;  //数据文件名
	CString m_CurveName;  //曲线名
	CString m_WellName;  //井名
	bool bReadCurve;  //是否已经读了曲线数据
	int Dimension;  //维数
	float Stdep,Endep,Rlev;  //数据的起止深度及采样间隔
	char DeptUnits[41]; //深度单位
	char CurveName[41],CurveUnits[41]; //曲线名和曲线单位
	float CurveMin,CurveMax;  //绘图左右刻度
	int NumSam; //曲线采样点数
	float *CurveData;  //曲线数据
	float Sdep,Edep; //绘图起止深度
	CRect MyRect;   //绘图区域
	int MyBottom;  //客户区底部用于显示状态信息的高度
	int MyLeft;    //客户区左边用于显示按钮的宽度
	int TopInsert;  //顶部图例高度
	int LineWidth,LineColor;  //线宽及线色
	int LineLog;  //线性/对数开关，0为线性，1为对数
	bool m_Drawing;  //是否开始拖动
	HCURSOR m_Hcursor;  //光标
	CRect m_rectselect;  //鼠村拖动区区域
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCurveView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWndShadow m_Shadow;
	// Generated message map functions
	//{{AFX_MSG(CCurveView)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnStyle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CURVEVIEW_H__7B8222F2_15BD_41F1_B271_706357EB7428__INCLUDED_)

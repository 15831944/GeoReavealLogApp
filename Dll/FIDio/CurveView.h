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
	CString m_WellPath; //����������·��
	CString m_IndexFile;  //�����ļ���
	CString m_DataFile;  //�����ļ���
	CString m_CurveName;  //������
	CString m_WellName;  //����
	bool bReadCurve;  //�Ƿ��Ѿ�������������
	int Dimension;  //ά��
	float Stdep,Endep,Rlev;  //���ݵ���ֹ��ȼ��������
	char DeptUnits[41]; //��ȵ�λ
	char CurveName[41],CurveUnits[41]; //�����������ߵ�λ
	float CurveMin,CurveMax;  //��ͼ���ҿ̶�
	int NumSam; //���߲�������
	float *CurveData;  //��������
	float Sdep,Edep; //��ͼ��ֹ���
	CRect MyRect;   //��ͼ����
	int MyBottom;  //�ͻ����ײ�������ʾ״̬��Ϣ�ĸ߶�
	int MyLeft;    //�ͻ������������ʾ��ť�Ŀ��
	int TopInsert;  //����ͼ���߶�
	int LineWidth,LineColor;  //�߿���ɫ
	int LineLog;  //����/�������أ�0Ϊ���ԣ�1Ϊ����
	bool m_Drawing;  //�Ƿ�ʼ�϶�
	HCURSOR m_Hcursor;  //���
	CRect m_rectselect;  //����϶�������
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

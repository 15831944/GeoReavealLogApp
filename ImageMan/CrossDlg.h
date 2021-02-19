#pragma once

#define SCALE_LIN				0
#define SCALE_LOG				1

// CCrossDlg �Ի���

class CCrossDlg : public CDialog
{
	DECLARE_DYNAMIC(CCrossDlg)

public:
	CCrossDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCrossDlg();

// �Ի�������
	enum { IDD = IDD_CROSS_DIALOG };
public:

	int		m_CrossData[3][100][100];	//���㵽����Ľ������ ��1άΪ3������
	int     m_CrossX[100];				//X��ֱ��ͼ����
	int     m_CrossY[100];				//Y��ֱ��ͼ����
	
	int 	MarginX;
	int 	MarginY;	
	WORD 	LengthX;		//����ͼ���Ŀ��
	WORD 	LengthY;		//����ͼ���ĸ߶�

	LOGFONT m_AxiasTextFont,m_ScaleTextFont;

public: 
	COLORREF m_bkColor,m_AxiasTextColor,m_ScaleTextColor;
	LOGPEN  m_LogPen1,m_LogPen2,m_LogPen3,m_LogPen4;
	

	CString m_szAliaseX;	//���������	
	float	m_StartX;		//�������ֵ	
	float	m_ScaleIncX;	//����������	
	WORD	m_ScaleNumX;	//������̶���	
	WORD	m_ScaleTypeX;	//������̶�����

	CString m_szAliaseY;	//���������		
	float	m_StartY;		//�������ֵ	
	float	m_ScaleIncY;	//����������	
	WORD	m_ScaleNumY;	//������̶���	
	WORD	m_ScaleTypeY;	//������̶�����

	CPoint 	m_nPos;
	CPoint 	m_nPosLT;
	CPoint 	m_nPosRB;	

	//����ֱ��ͼ��ɫ��ʾ
	float m_ImageMin,m_ImageMax;
public:
	void dtos(double value, char* ss);
	void DrawAxias(CDC* pDC);
	void DrawCrossData(CDC* pDC);
	void OnSendToClip();
	void XFrequency();
	void YFrequency();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual BOOL OnInitDialog();
};

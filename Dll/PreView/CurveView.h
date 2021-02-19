#pragma once
// CCurveView ��ͼ

class CCurveView : public CScrollView
{
	DECLARE_DYNCREATE(CCurveView)

public:
	CSize m_nScrollSize;
	CString		m_szServer;	

	BOOL InitData();
	void DrawCurve(CDC *pDC);
	CString m_WellPath;			//����������·��
	DWORD m_CurveID;
	CString m_CurveName;		//������
	bool bReadCurve;			//�Ƿ��Ѿ�������������
	float Stdep,Endep,Rlev;		//���ݵ���ֹ��ȼ��������
	float CurveMin,CurveMax;	//��ͼ���ҿ̶�
	int NumSam;					//���߲�������
	float *CurveData;			//��������
	float Sdep,Edep;			//��ͼ��ֹ���
	CRect MyRect;				//��ͼ����
	int MyBottom;				//�ͻ����ײ�������ʾ״̬��Ϣ�ĸ߶�
	int MyLeft;					//�ͻ������������ʾ��ť�Ŀ��
	int TopInsert;				//����ͼ���߶�
	int LineWidth,LineColor;	//�߿���ɫ
	int LineLog;				//����/�������أ�0Ϊ���ԣ�1Ϊ����

public:
	CCurveView();				// ��̬������ʹ�õ��ܱ����Ĺ��캯��
protected:	
	virtual ~CCurveView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual void OnInitialUpdate();     // �����ĵ�һ��
protected:
	virtual void OnDraw(CDC* pDC);      // ��д�Ի��ƴ���ͼ

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
};



/******************************************************************
*   PickupHoleDraw.h - declarations of Stratum Draw object class       *           
*******************************************************************/


#ifndef __PICKUPHOLEDRAW_H__
#define __PICKUPHOLEDRAW_H__

#include "FIDDraw.h"
#include "Grid.h"

class CContour : public CObject  //����������
{
public:
	CContour(BOOL bAdd);
	CContour();
	virtual ~CContour();
protected:
	DECLARE_SERIAL(CContour);
public:
	long ComputeArea();
	BOOL IsIntersect(CRect rt);//�жϴ˿��������Ƿ������rt�ཻ
	BOOL InRegion(CPoint pt);  //�ж�pt���Ƿ��ڴ˿���������

public:
	CArray<CPoint,CPoint&>m_pointArray; //��ſ�������������
	CRect m_rect;						//�����Ӵ˿����ľ���
	BOOL  m_bAdd;                       //m_bAdd��ʾ�Ƿ������ӿ���
	float m_Ver;                        //����汾��
	float m_Radius;						//�ܶ��뾶
	float m_Round;						//�ܶ�Բ��
	virtual void Serialize(CArchive& ar);
};

class CPickupHoleObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CPickupHoleObj)
#else
	DECLARE_SERIAL(CPickupHoleObj);
#endif
public:
	CPickupHoleObj();
	CPickupHoleObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CPickupHoleObj();
	
	//��ȡͼ����ؿ��Ʋ���
	BOOL	m_bDrawRadius;				   //��ʾ�׶�ֱ��
	CString m_PickupFIDName;               //����ȫ��
	CString m_PickupAliasName;             //���߱���
	int 	m_PickupBit;                   //��ͷֱ�� mm
	int 	m_PickupHorScale;              //�����ͼ����
	CRect	m_PickupRect;                  //�½�ʱ��ͼ���С 
	float	m_PickupStDep;                 //�½�ʱ����ʼ���
	float	m_PickupEnDep;                 //�½�ʱ�Ľ������
	float   m_PickupVerScale;              //�����ͼ����
	float	m_CoverageRate;                //�Զ�ʰȡ������
	//�洢��ʰȡ�Ŀ�����׶�
	CTypedPtrList<CObList,CContour*>m_ContourList;

	void CreatePoint();
	void DrawContour(CDC* pDC);
//Attributes
protected:   
	WORD m_PenWidth;
	COLORREF m_PenColor,m_FillColor;
	LOGFONT  m_ScaleFont;
	int	 m_bFill;
//Implementation
public:
	virtual void Serialize(CArchive& ar);
	virtual void DrawHead(CDC *pDC);	
	virtual void DrawClient(CDC *pDC);	
	virtual BOOL Properties(CView* pView);
};                

#endif //__PICKUPHOLEDRAW_H__
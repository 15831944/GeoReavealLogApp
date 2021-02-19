// FlowNode.h: interface for the CFlowNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLOWNODE_H__FB9C8652_6318_446B_9A7B_B70EEA1F51FE__INCLUDED_)
#define AFX_FLOWNODE_H__FB9C8652_6318_446B_9A7B_B70EEA1F51FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CFlowChartDoc;
class CFlowNode : public CObject  
{
	DECLARE_SERIAL(CFlowNode);
public:
	CFlowNode();
	 ~CFlowNode();
// Attributes
public:
	float m_Ver;
	CString m_szNodeName;                          //ģ����
	BOOL m_bResult;                                //ģ�����н��
	CString m_szMessage;                           //ģ�����м�¼
	CFlowChartDoc *m_pDoc;
//  Implementation
public:
	virtual BOOL Run();                            //��������������ģ��     
	virtual void InitPara(CFlowChartDoc* pDoc);    //��ʼ��ģ�����
	virtual void Serialize(CArchive& ar);          //��ȡ����ģ�����
};

#endif // !defined(AFX_FLOWNODE_H__FB9C8652_6318_446B_9A7B_B70EEA1F51FE__INCLUDED_)

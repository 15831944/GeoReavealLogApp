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
	CString m_szNodeName;                          //模块名
	BOOL m_bResult;                                //模块运行结果
	CString m_szMessage;                           //模块运行记录
	CFlowChartDoc *m_pDoc;
//  Implementation
public:
	virtual BOOL Run();                            //两击鼠标左键运行模块     
	virtual void InitPara(CFlowChartDoc* pDoc);    //初始化模块参数
	virtual void Serialize(CArchive& ar);          //读取保存模块参数
};

#endif // !defined(AFX_FLOWNODE_H__FB9C8652_6318_446B_9A7B_B70EEA1F51FE__INCLUDED_)

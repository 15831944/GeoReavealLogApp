#pragma once
//������Ҫ���ݿ�ʱ�������ݿ����
#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","adoEOF") rename("BOF","adoBOF")

class AFX_EXT_CLASS CGeoAdo  
{
private:
	_ConnectionPtr m_pConn;
	_RecordsetPtr  m_pRS;

public:
	CGeoAdo();
	virtual ~CGeoAdo();
	//���ݿ������ַ���
	//�磺10.132.204.16;sa;log;LogBase
	CString m_szSever;

	DWORD		m_RecordID;		// ��¼��
	CString		m_FillUser;		// ���
	CString		m_FillDate;		// �����
	CString		m_Describe;		// ��ע
	DWORD		m_UpperID;		// �����洢��¼��
public:							
	//��ʼ���ͽ������ݿ�����	 
	BOOL InitData();			
	BOOL InitData(CString szCon);
	void Disconnect();
public:
	_RecordsetPtr&	GetRecordSet(CString strSQL);				// ִ��SQL���  ���ؼ�¼��
	BOOL			Execute(CString strSQL);					// ִ��SQL���  �޼�¼������
	DWORD			GetMaxRecordID(CString TableName);			// ��ȡĳ�ű��е�����¼
};


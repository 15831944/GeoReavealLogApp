#pragma once
//������Ҫ���ݿ�ʱ�������ݿ����
#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","adoEOF") rename("BOF","adoBOF")

class CGeoAdo  
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
	//��ʼ���ͽ������ݿ�����
	BOOL InitData();
	BOOL InitData(CString szCon);
	void Disconnect();
public:
	_RecordsetPtr& GetRS(CString strSQL);	// ִ��  ���ؼ�¼����SQL���
	BOOL Execute(CString strSQL);			// ִ�в����ؼ�¼����SQL���
	BOOL GetRegString(CString szSubKey,LPCTSTR lpszEntry,CString& strValue);
};


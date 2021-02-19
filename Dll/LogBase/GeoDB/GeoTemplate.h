// Production.h: interface for the CDocument class.
//////////////////////////////////////////////////////////////////////

#pragma once
#include "GeoAdo.h"

/***************************ͼ��չʾģ����Ϣ��*************************************
CREATE TABLE Template
(
	RecordID		int PRIMARY KEY IDENTITY,		--ģ����
	TempName		varchar(128) NOT NULL,			--ģ������
	TempType		varchar(16),					--ģ������
	TempKind		varchar(64),					--ģ�����
	TempSize		int,							--ģ���ֽ���
	TempCreDate		datetime,						--ģ�崴������
	Describe 		varchar(512),					--ģ������
	FillUser		varchar(64),					--�ύ��
	FillDate		datetime,						--�ύ����
	DataZip			int,							--��ѹ����ʶ 0-ûѹ�� 16-Zlib��׼6��ѹ��
	DataByte		image							--���ݿռ�
)*/
//////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CGeoTemplate :public CGeoAdo
{	
public:
	CString		m_Name;		
	CString		m_Type;
	CString		m_Kind;
	DWORD		m_DataSize;		
	CString		m_CreDate;	
		
	CString		m_Describe; 	
	CString		m_FillUser;	
	CString		m_FillDate;		
	int			m_DataZip;
	CGeoTemplate();
	virtual ~CGeoTemplate();
	
public:	//��Ա�����ԵĲ�������
	

public:  //���ݿ��������
	//�ж�ָ�����������Ƿ����
	DWORD	HaveName(CString cDocName);			
	//����ָ�������߱�ŷ�����������
	CString GetName(DWORD iDocID);					
	//�������߱�Ŷ�ȡ�����ֶ�ֵ
	BOOL	GetData(DWORD iDocID);					
	//�����ĵ�ID�޸��ĵ�����DocByte�ֶ�ֵ
	void	SetDataByte(DWORD iDocID,DWORD nLen,char *pBuf);
	//�����ĵ�ID��ȡ�ĵ�����DocByte�ֶ�ֵ
	DWORD	GetDataByte(DWORD iDocID,DWORD nLen,char *pBuf);

	DWORD	sql_insert();
	BOOL	sql_update(DWORD iDocID);	
	BOOL	sql_delete(DWORD iDocID);	

};

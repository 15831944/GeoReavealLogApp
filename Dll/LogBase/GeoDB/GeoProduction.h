// Production.h: interface for the CDocument class.
//////////////////////////////////////////////////////////////////////
#pragma once
#include "GeoAdo.h"
/***************************�����ɹ��ĵ���Ϣ��************************************
CREATE TABLE Production
(
	ProductionID	int PRIMARY KEY IDENTITY,		--�ĵ����
	DocName			varchar(64) NOT NULL,			--�ĵ�����
	DocType			varchar(16),					--�ĵ�����
	DocTitle		varchar(64)						--�ĵ�����
	DocAuthor		varchar(32)						--�ĵ�����
	DocCreDate		datetime						--��������
	Describe 		varchar(512),					--�ĵ�����
	FileFormat		varchar(32)						--�ļ���ʽ
	FileName		varchar(256)					--�ļ���	
	StorageMode		varchar(32)						--���ݴ洢��ʽ
	FilePath		varchar(256)					--�ļ��洢·��
	DataSize		bigint							--�������С
	PieceSize		bigint							--���ݿ��С
	DataZip			tinyint							--����ѹ����ʶ 0-δѹ�� 16-��׼6��Zlibѹ��
	DataStID		bigint							--���ݿ���ʼ��¼��
	DataPieceNum	int								--���ݿ���
	FillUser		varchar(64),					--�ύ��
	FillDate		varchar(32),					--�ύ����
	UpperID			int NOT NULL,					--�����⾮��Ŀ���
)*/
//////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CGeoProduction :public CGeoAdo
{	
public:
	CString		m_DocName;							//�ĵ�����
	CString		m_DocType;							//�ĵ�����
	CString		m_DocTitle;							//�ĵ�����
	CString		m_DocAuthor;						//�ĵ�����
	CString		m_DocCreDate;						//��������

	CString		m_FileFormat;						//�ļ���ʽ
	CString		m_FileName;							//�ļ���	
	CString		m_StorageMode;						//���ݴ洢��ʽ
	CString		m_FilePath;							//�ļ��洢·��
	DWORD		m_DataSize;							//�������С
	DWORD		m_PieceSize;						//���ݿ��С
	int			m_DataZip;							//����ѹ����ʶ 0-δѹ�� 16-��׼6��Zlibѹ��
	DWORD		m_DataStID;							//���ݿ���ʼ��¼��
	int			m_DataPieceNum;						//���ݿ���

	CGeoProduction();
	virtual ~CGeoProduction();

public:	//��Ա�����ԵĲ�������
	

public:  //���ݿ��������
	//�ж�ָ�����������Ƿ����
	DWORD	HaveName(CString cDocName);			
	//����ָ�������߱�ŷ�����������
	CString GetName(DWORD iDocID);					
	//�������߱�Ŷ�ȡ�����ֶ�ֵ
	BOOL	GetData(DWORD iDocID);					
	//�����ĵ�ID�޸��ĵ�����DataByte�ֶ�ֵ
	DWORD	SetDataByte(DWORD iDocID,int Number,DWORD nLen,char *pBuf);
	//�����ĵ�ID��ȡ�ĵ�����DataByte�ֶ�ֵ
	DWORD	GetDataByte(DWORD iDocID,DWORD nLen,char *pBuf);
	//���ݲ⾮��ĿIDɾ������Ŀ�е����гɹ��ĵ�
	BOOL	DeleteProject(DWORD iProjectID);

	DWORD	sql_insert();
	BOOL	sql_update(DWORD iDocID);
	//���´洢�ռ���Ϣ���ڵ�ת�洢�ռ�ʱ����
	BOOL	sql_updateStorage(DWORD iStorageID);		
	BOOL	sql_delete(DWORD iDocID);	

};

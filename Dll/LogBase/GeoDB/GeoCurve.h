// GeoCurve.h: interface for the CCurve class.
//////////////////////////////////////////////////////////////////////
#pragma once

#include "GeoAdo.h"
/***************************�����⾮������Ϣ��************************************
CREATE TABLE Curve
(
	CurveID			int PRIMARY KEY IDENTITY,		--���߱��
	CurveName    	varchar(32) NOT NULL,			--��������
	CurveAlias		varchar(32),					--���߱���
	Dimension		tinyint,						--����ά��
	DepUnit    		varchar(8),						--��ȵ�λ
	DepSt			float,							--��ʼ���
	DepEn			float,							--�������
	DepRlev			float,							--��Ȳ���
	XName    		varchar(32),					--X������
	XUnit    		varchar(8),						--X�ᵥλ
	XMin			float,							--X����Сֵ
	XMax			float,							--X�����ֵ
	XRlev			float,							--X���������
	XNPS			int,							--X�������
	DataType		tinyint,						--��������
	DataUnit    	varchar(8),						--���ݵ�λ
	CreateDate		varchar(32),					--��д����
	DataSize		bigint							--�������С
	PieceSize		bigint							--���ݿ��С
	DataZip			tinyint,						--����ѹ����ʶ 0-δѹ�� 16-��׼6��Zlibѹ��
	DataStID		bigint							--���ݿ���ʼ��¼��
	DataPieceNum	int								--���ݿ���
	FillUser		varchar(32),					--�ύ��
	FillDate		datetime						--�ύ����
	UpperID			int NOT NULL					--�����⾮��Ŀ���
)*/
//////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CGeoCurve :public CGeoAdo
{	

public://��������
	DWORD		m_CurveID;				//���߱��
	CString		m_CurveName;			//��������
	CString		m_CurveAlias;			//���߱���
	int			m_Dimension;			//����ά��
	CString		m_DepUnit;    			//��ȵ�λ
	float		m_DepSt;				//��ʼ���
	float		m_DepEn;				//�������
	float		m_DepRlev;				//��Ȳ���
	CString		m_XName;    			//X������
	CString		m_XUnit;    			//X�ᵥλ
	float		m_XMin;					//X����Сֵ
	float		m_XMax;					//X�����ֵ
	float		m_XRlev;				//X���������
	int			m_XNPS;					//X�������
	int			m_DataType;				//��������
	CString		m_DataUnit;    			//���ݵ�λ
	CString		m_CreateDate;			//��д����
	DWORD		m_DataSize;				//�������С
	DWORD		m_PieceSize;			//���ݿ��С
	int			m_DataZip;				//����ѹ����ʶ 0-δѹ�� 16-��׼6��Zlibѹ��
	DWORD		m_DataStID;				//���ݿ���ʼ��¼��
	WORD		m_DataPieceNum;			//���ݿ���

	CGeoCurve();
	virtual ~CGeoCurve();

public:	//��Ա�����ԵĲ�������
	

public:  //���ݿ��������
	// �ж�ָ�����������Ƿ����
	DWORD	HaveName(CString cCurveName);			
	// ����ָ�������߱�ŷ�����������
	CString GetName(DWORD iCurveID);					
	// �������߱�Ŷ�ȡ�����ֶ�ֵ
	BOOL	GetData(DWORD iCurveID);					
	// ��������ID�޸���������DataByte�ֶ�ֵ
	DWORD	SetDataByte(DWORD iCurveID,int Number,DWORD nLen,char *pBuf);
	// ��������ID��ȡ��������DataByte�ֶ�ֵ
	DWORD	GetDataByte(DWORD iCurveID,DWORD nLen,char *pBuf);
	// ���ݲ⾮��ĿIDɾ������Ŀ�е���������
	BOOL	DeleteProject(DWORD iProjectID);

	DWORD	sql_insert();
	BOOL	sql_update(DWORD iCurveID);
	//���´洢�ռ���Ϣ���ڵ�ת�洢�ռ�ʱ����
	BOOL	sql_updateStorage(DWORD iStorageID);		
	BOOL	sql_delete(DWORD CurveID);	

};


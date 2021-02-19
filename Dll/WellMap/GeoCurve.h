// GeoCurve.h: interface for the CCurve class.
//////////////////////////////////////////////////////////////////////
#if !defined _GEOCURVE_H__
#define _GEOCURVE_H__

#include "GeoAdo.h"

#pragma once
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
	DataZip			tinyint,						--����ѹ����ʶ 0-δѹ�� 16-��׼6��Zlibѹ��
	UpperID			int NOT NULL,					--�洢�ռ���
	DataByte		image,							--���ݿռ�
)*/
//////////////////////////////////////////////////////////////////////
class CGeoCurve  
{	
private:
	CGeoAdo		m_ado;

public://��������
	DWORD		m_CurveID;		
	CString	m_CurveName;   
	CString	m_CurveAlias;	
	int		m_Dimension;	
	CString	m_DepUnit;    	
	float	m_DepSt;		
	float	m_DepEn;		
	float	m_DepRlev;		
	CString	m_XName;    	
	CString	m_XUnit;    	
	float	m_XMin;		
	float	m_XMax;		
	float	m_XRlev;		
	int		m_XNPS;		
	int		m_DataType;	
	CString	m_DataUnit;    	
	CString	m_CreateDate;
	int		m_UpperID;
	int		m_DataZip;
	DWORD	m_DataSize;
	CGeoCurve();
	virtual ~CGeoCurve();
	
	
	BOOL InitData(CString szCon);
	void Disconnect();
public:	//��Ա�����ԵĲ�������
	

public:  //���ݿ��������
	//�ж�ָ�����������Ƿ����
	DWORD	HaveName(CString cCurveName);			
	//����ָ�������߱�ŷ�����������
	CString GetName(DWORD iCurveID);					
	//�������߱�Ŷ�ȡ�����ֶ�ֵ
	BOOL	GetData(DWORD iCurveID);					
	//��������ID�޸���������DataByte�ֶ�ֵ
	void	SetDataByte(DWORD iCurveID,DWORD nLen,char *pBuf,char szBlob[MAX_PATH]);
	//��������ID��ȡ��������DataByte�ֶ�ֵ
	DWORD	GetDataByte(DWORD iCurveID,DWORD nLen,char *pBuf,char szBlob[MAX_PATH]);
	//���ݲ⾮��ĿIDɾ������Ŀ�е���������
	BOOL	DeleteProject(DWORD iProjectID);

	DWORD	sql_insert();
	BOOL	sql_update(DWORD iCurveID);
	//���´洢�ռ���Ϣ���ڵ�ת�洢�ռ�ʱ����
	BOOL	sql_updateStorage(int iStorageID);		
	BOOL	sql_delete(DWORD CurveID);	

};


//////////////////////////////////////////////////////////////////////
#endif 

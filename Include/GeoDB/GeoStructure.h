// GeoStructure.h: interface for the CGeoStructur class.
//////////////////////////////////////////////////////////////////////

#pragma once
#include "GeoAdo.h"

/***************************����������Ϣ��************************************
CREATE TABLE COM_Structure
(
	StructureID     int PRIMARY KEY IDENTITY,		--������
	StructureName	varchar(128) NOT NULL,			--��������
	Alias			varchar(128),					--�������
	Area  			float,							--�������
	Region			varchar(256),					--����
	Oilfield		varchar(64),			        --��������
	ViewX			float,							--�۲��X����
	ViewY			float,							--�۲��Y����
	Longitude		float,							--�۲�㾭��
	Latitude		float,							--�۲��γ��
	GeoDescribe 	varchar(2048),					--�����������
	FormDescribe 	varchar(2048),					--�����������
	DrillDescribe 	varchar(2048),					--�����꾮�����������
	OilDescribe 	varchar(2048),					--�������������������
	ImageType		varchar(16),					--����ͼ����
	ImageSize		int,							--����ͼ�ֽ���
	ImageCreDate	datetime,						--����ͼ��������
	FillUser		varchar(64),					--�ύ��
	FillDate		datetime,						--�ύ����
	UpperID			int NOT NULL,					--�洢�ռ���
	DataByte		image							--����ͼ
)
*/
//////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CGeoStructure :public CGeoAdo 
{	
public:
	CString		m_StructureName;					//��������
	CString		m_Alias;							//�������
	float		m_Area;  							//�������
	CString		m_Region;							//����
	CString		m_Oilfield;							//��������
	float		m_ViewX;							//�۲��X����
	float		m_ViewY;							//�۲��Y����
	float		m_Longitude;						//�۲�㾭��
	float		m_Latitude;							//�۲��γ��
	CString		m_GeoDescribe; 						//�����������
	CString		m_FormDescribe; 					//�����������
	CString		m_DrillDescribe; 					//�����꾮�����������
	CString		m_OilDescribe; 						//�������������������
	CString		m_ImageType;						//����ͼ����
	DWORD		m_ImageSize;						//����ͼ�ֽ���
	CString		m_ImageCreDate;						//����ͼ��������


public: 
	CGeoStructure();
	virtual ~CGeoStructure();


public:  //���ݿ��������
	//�ж�ָ�����������Ƿ����
	DWORD	HaveName(CString cStructureName);			
	//����ָ�������߱�ŷ�����������
	CString GetName(DWORD iStructureID);					
	//���ݹ�����Ϣ�Ŷ�ȡ�����ֶ�ֵ
	BOOL	GetData(DWORD iStructureID);
	//���������洢�ձ�Ŷ�ȡ������Ϣ�����ֶ�ֵ
	BOOL GetDataReferToUpperID(DWORD iUpperID);

	//�����ĵ�ID�޸��ĵ�����StructureByte�ֶ�ֵ
	void	SetStructureByte(DWORD iStructureID,DWORD nLen,char *pBuf);
	//�����ĵ�ID��ȡ�ĵ�����StructureByte�ֶ�ֵ
	DWORD	GetStructureByte(DWORD iStructureID,DWORD nLen,char *pBuf);
	//���ݲ⾮��ĿIDɾ������Ŀ�е����гɹ��ĵ�
	BOOL	DeleteProject(DWORD iProjectID);

	DWORD	sql_insert();
	BOOL	sql_update(DWORD iStructureID);	
	BOOL	sql_delete(DWORD iStructureID);
	//���ݽṹ��ϢID�޸���������(���ڴ洢�ռ��ת����ģ����)
	BOOL	sql_updateUpper(DWORD iUpperID);	
};


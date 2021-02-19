// GeoWell.h
/*********************************************************************/
#pragma once
#include <afxtempl.h>
#include <afxcoll.h>
#include "GeoAdo.h"
/*
CREATE TABLE Well
(
	WellID			bigint PRIMARY KEY IDENTITY,	--�����							
	WellName		varchar(64) NOT NULL,			--������									JM
	Alias			varchar(64),					--������									BM
	Structure		varchar(64),					--��������									GZ
	Oilfield		varchar(64),			        --��������									DM
	Owner			varchar(64),			        --ҵ����λ									DW				
    Region			varchar(128),			        --����									
    Location		varchar(128),					--����λ��
    Line_Location	varchar(128),					--����λ��
    ElevationGd		float,							--���溣��									HBGD
    Elevation0		float,							--���ĺ���
    DepOffset		float,							--���ĸ߶�									BXGD
    OilOffset		float,							--�Ͳ���
    PipeOffset		float,							--�ײ���
    MagneticV		float,							--��ƫ��
	WellDepth		float,							--�꾮����									JS
	WellBottom		float,							--�˹�����
	StartDate		datetime,						--��������									RQ
	EndDate			datetime,						--��������									WZRQ
	WellX			float,							--��λX����									X
	WellY			float,						    --��λY����									Y
	Longitude		float,						    --��λ����
	Latitude		float,						    --��λγ��
	WellType  		varchar(32),			        --��̽����(̽����Ԥ̽�������۾���������)	JB
	WellKind  		varchar(32),			        --���̷���(ֱ����б����ˮƽ������ʽ��)
	DrillAim		varchar(4096),					--��̽Ŀ��
	AimLayer		varchar(64),					--Ŀ���λ
	ReservoirType	varchar(32),					--��������(����������ú������ҳ����)
	Describe 		varchar(512),					--������									BZ
	FillUser		varchar(64),					--�ύ��
	FillDate		datetime,						--�ύ����
	UpperID			bigint NOT NULL					--�洢�ռ���
)
*/
class AFX_EXT_CLASS CGeoWell  :public CGeoAdo
{

public:
	CString		m_WellName;	         //������               
	CString		m_Alias;			 //������
	CString		m_Structure;		 //��������
	CString		m_Oilfield;			 //��������
	CString		m_Owner;			 //ҵ����λ
	CString		m_Region;			 //����
	CString		m_Location;			 //����λ��
	CString		m_Line_Location;	 //����λ��
	float		m_ElevationGd;		 //���溣��
	float		m_Elevation0;		 //��׼�溣��
	float		m_DepOffset;		 //���ĸ߶�
	float		m_OilOffset;		 //�Ͳ���
	float		m_PipeOffset;		 //�ײ���
	float		m_MagneticV;		 //��ƫ��
	float		m_WellDepth;		 //�꾮����
	float		m_WellBottom;		 //�˹�����
	CString		m_StartDate;		 //��������
	CString		m_EndDate;			 //��������
	float		m_WellX;			 //��λX����
	float		m_WellY;			 //��λY����
	double		m_Longitude;		 //��λ����
	double		m_Latitude;			 //��λγ��
	CString		m_WellType;			 //��̽����(̽����Ԥ̽�������۾���������)
	CString		m_WellKind;			 //���̷���(ֱ����б����ˮƽ������ʽ��)
	CString		m_DrillAim;			 //��̽Ŀ��
	CString		m_AimLayer;			 //Ŀ���λ
	CString		m_ReservoirType;	 //��������(���������㡢ú������ҳ����)

	// �����Ϣ
	CArray<float,float&> a_BitSize;
	CArray<float,float&> a_BitDep;
	CArray<float,float&> a_CasingSize;
	CArray<float,float&> a_CasingDep;
public:
	CGeoWell();
	virtual ~CGeoWell();

public:	
	CString GetDescribe();
	void	SetDescribe(CString cDescribe);
	DWORD	GetUpperID();
	void	SetUpperID(DWORD iUpperID);
	CString GetFillUser();
	void	SetFillUser(CString cUser);
	CString GetFillDate();
	void	SetFillDate(CString cDate);

public:		//���ݿ����	 
	//�����²����¼�ı��
	DWORD	sql_insert();					 
	
	BOOL	sql_update(DWORD iWellID);
	BOOL	sql_delete(DWORD iWellID);
	//���ݾ���Ϣ��Ŷ�ȡ�����ֶ�ֵ
	BOOL GetData(DWORD iWellID);
	//���ݾ���ϢID����������(���ڴ洢�ռ��ת����ģ����)
	BOOL	sql_updateUpper(DWORD iUpperID);
	//���������洢�ձ�Ŷ�ȡ����Ϣ�����ֶ�ֵ
	BOOL GetDataReferToUpperID(DWORD iUpperID);

	BOOL	GetDrillTool(DWORD iWellID);
	DWORD	InsertDrillTool(DWORD iWellID);
	BOOL	DeleteDrillTool(DWORD iWellID);
	BOOL	UpdateDrillTool(DWORD iWellID);
};


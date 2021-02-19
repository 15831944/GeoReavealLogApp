// BackupData.h: interface for the CBackupData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BACKUPDATA_H__84313969_D705_4BFD_ACFF_DF761C82C3F6__INCLUDED_)
#define AFX_BACKUPDATA_H__84313969_D705_4BFD_ACFF_DF761C82C3F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Flownode.h"
#include "GainRegress.h"
class CFlowChartDoc;
class CBackupData : public CFlowNode  
{
	DECLARE_SERIAL(CBackupData);
public:
	CBackupData();
	CBackupData(CString szNodename,BOOL bResult);
	~CBackupData();
// Attributes
public:
	CString m_szOption;                   //������Ŀ
	CString m_szBufPath;                  //���ݻ�����ȫ·����
	CString m_szWellName;                 //����·���ľ���
	float m_StProDep,m_EnProDep,m_Rlev;   //������
	//����������
	CString	m_szSeries;                   //��������
	int m_PadNum;                         //������
	int m_ButtonNum;                      //ÿ����������
	int	m_ButtonMid;                      //ÿ�������м���λ��
	int m_CalNum;                         //����������
	CString m_szPadNames[64];             //����������
	CString m_szCalNames[64];             //�뾶
	CString m_szAZ1Name;                  //1�ż��巽λ����
	CString FCName[8][2];                 //FMI����������
	
	CString	m_szDIPNames[64];             //��Ǵ���絼��������
	CString	m_szDazName;                  //��б��λ
	CString	m_szDevName;                  //��б��
	CString	m_szRBName;                   //��Է�λ

	//��������
	CString m_szAMPName;                  //��������
	CString m_szTTName;                   //ʱ������
	CString m_szRADName;                  //�뾶����
	float m_Bits;                         //��ͷֱ��

	//����ָ�
	CString m_szGrdVol;                   //���β�����ѹ�������� PD6G
	CString m_szGrdGan;                   //���ε缫�������� GG01
	CString m_szPadGan;                   //���������� PADG
	float	m_Rm;                         //�ཬ������
	BOOL	m_bGain;                      //�Ƿ�����ָ�
	CGainRegress m_GainPro;               //����ָ�����

	//STARII���ٶ�У������
	CString m_szGAZFName;                 //����̽ͷ���ٶ�(GAZFQH)
	CString m_szTimeName;                 //�⾮����ʱ����(ETMIEQH)

//  Implementation
public:
	virtual BOOL Run();
	virtual void InitPara(CFlowChartDoc* pDoc);
	virtual void Serialize(CArchive& ar);
	BOOL ACDataCopy(float NewRlev=0.00254f);

	BOOL AzimuthCorret(CString Name);

	BOOL EMISTARDataCopy(float NewRlev=0.00254f);	
	BOOL FMIDataCopy(float NewRlev=0.00254f);
	BOOL CMIDataCopy(float NewRlev=0.00254f);

	BOOL CreatRADS(CString Name,float newRlev);
	BOOL STARGainCopy(float NewRlev=0.00254f);        //��������ָ�����
	
	BOOL CreateZACCArray();     //�ϲ���׼��ȡ����ٶȡ�ʱ����������������

	BOOL DipDataCopy(float NewRlev=0.00254f);
	BOOL SubDataCopy(float NewRlev=0.00254f);

	BOOL BufCurveToWell(CString InName,CString OutName);
	BOOL DataVerFilterPro(CString Name,int nFilter);
	BOOL CurveReSample(CString InCurveName,CString OutCurveName,float NewRlev,BOOL bAz=TRUE);
};	

#endif // !defined(AFX_BACKUPDATA_H__84313969_D705_4BFD_ACFF_DF761C82C3F6__INCLUDED_)

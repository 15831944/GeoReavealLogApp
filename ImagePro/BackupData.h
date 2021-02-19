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
	CString m_szOption;                   //处理项目
	CString m_szBufPath;                  //数据缓冲区全路径名
	CString m_szWellName;                 //包括路径的井名
	float m_StProDep,m_EnProDep,m_Rlev;   //处理井段
	//输入曲线名
	CString	m_szSeries;                   //仪器类型
	int m_PadNum;                         //极板数
	int m_ButtonNum;                      //每个极板电扣数
	int	m_ButtonMid;                      //每个极板中间电扣位置
	int m_CalNum;                         //井径曲线数
	CString m_szPadNames[64];             //极板曲线名
	CString m_szCalNames[64];             //半径
	CString m_szAZ1Name;                  //1号极板方位曲线
	CString FCName[8][2];                 //FMI极板曲线名
	
	CString	m_szDIPNames[64];             //倾角处理电导率曲线名
	CString	m_szDazName;                  //井斜方位
	CString	m_szDevName;                  //井斜角
	CString	m_szRBName;                   //相对方位

	//声成像处理
	CString m_szAMPName;                  //幅度曲线
	CString m_szTTName;                   //时间曲线
	CString m_szRADName;                  //半径曲线
	float m_Bits;                         //钻头直径

	//增益恢复
	CString m_szGrdVol;                   //屏蔽测量电压阵列数据 PD6G
	CString m_szGrdGan;                   //屏蔽电极增益曲线 GG01
	CString m_szPadGan;                   //极增益曲线 PADG
	float	m_Rm;                         //泥浆电阻率
	BOOL	m_bGain;                      //是否增益恢复
	CGainRegress m_GainPro;               //增益恢复处理

	//STARII加速度校正曲线
	CString m_szGAZFName;                 //仪器探头加速度(GAZFQH)
	CString m_szTimeName;                 //测井采样时间间隔(ETMIEQH)

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
	BOOL STARGainCopy(float NewRlev=0.00254f);        //备份增益恢复曲线
	
	BOOL CreateZACCArray();     //合并标准深度、加速度、时间生成新阵列曲线

	BOOL DipDataCopy(float NewRlev=0.00254f);
	BOOL SubDataCopy(float NewRlev=0.00254f);

	BOOL BufCurveToWell(CString InName,CString OutName);
	BOOL DataVerFilterPro(CString Name,int nFilter);
	BOOL CurveReSample(CString InCurveName,CString OutCurveName,float NewRlev,BOOL bAz=TRUE);
};	

#endif // !defined(AFX_BACKUPDATA_H__84313969_D705_4BFD_ACFF_DF761C82C3F6__INCLUDED_)

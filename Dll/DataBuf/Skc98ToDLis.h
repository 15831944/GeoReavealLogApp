// Skc98ToDLis.h: interface for the CSkc98ToDLis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKC98TODLIS_H_INCLUDED_)
#define AFX_SKC98TODLIS_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "Skc98R.h"
#include "DLisWrite.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"

class CSkc98ToDLis  
{
public:
	CSkc98ToDLis();
	virtual ~CSkc98ToDLis();
	bool Skc98ToDLis(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sProducer,FILE *fo);
private:
	bool Transform(); //转换控制
	void SelectCurves(); //选择曲线
	void GetDlisFDataIndexFromSkc98(); //取得DLIS数据分帧信息
	void SetDlisChannelsFromSkc98(); //设置DLIS通道信息
	void Prt_FdataItemInfo(); //打印曲线分帧信息
	void SetStoUnitLab(); //设置存储单元标志
	void SetFileHeader(); //设置文件头(FILE-HEADER)参数
	void SetOrigin(); //设置源(ORIGIN)参数
	void SetDlisParameter(); //修改或补充DLIS文件参数
	void SetDlisFrameFromSkc98(); //设置DLIS帧
	void SetAxisFromSkc98(); //设置轴
	void Skc98DataToDlis(); //SKC98数据向DLIS转换

private:
	CWordArray CurveItem; //分组曲线索引，其结构为：组数->第１组项数及索引值->第２组项数及索引...
	bool IsConvCurve(int index);


private:
	CString m_SourceFile; //源文件名
	CString m_sObjectPath; //输出目录
	CString sInWellPath; //输入井路径
	CString sInWellName; //得到的井名
	CString sInField; //得到的油田名
	int m_nCurvesRadio; //过滤开关
	CString m_sProducer; //生产者名称(含代码)
	FILE *fp;

private:
	CSkc98R *Skc98R;
	CDlisWrite *DlisWrite;

private:
	CProgressDlg* pGuage; //进度条
	CString DlisFileName; //当前输出文件
	CWordArray CurveIndex; //源曲线索引号
	CStringArray SelCurveName; //选择曲线名表
	CStringArray NewCurveName; //新曲线名表
	CStringArray NewUnits; ////新单位名表
	int *ItemNum; //曲线属组
	int	NumItem; //组数
	WORD ProducerCode; //生产者代码
	char ProducerName[64]; //生产者名称

};

#endif // !defined(AFX_SKC98TODLIS_H_INCLUDED_)

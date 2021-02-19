// XtfToDLis.h: interface for the CXtfToDLis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTFTODLIS_H_INCLUDED_)
#define AFX_XTFTODLIS_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "XtfRead.h"
#include "DLisWrite.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"

class CXtfToDLis  
{
public:
	CXtfToDLis();
	virtual ~CXtfToDLis();
	bool XtfToDLis(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sProducer,FILE *fo);
private:
	bool Transform(); //转换控制
	void SelectCurves(); //选择曲线
	void GetDlisFDataIndexFromXtf(); //从Xtf的曲线索引中取得DLIS数据分帧信息
	void SetDlisChannelsFromXtf(); //从Xtf的曲线信息设置DLIS通道信息
	void Prt_FdataItemInfo(); //打印曲线分帧信息
	void SetStoUnitLab(); //设置存储单元标志
	void SetFileHeader(); //设置文件头(FILE-HEADER)参数
	void SetOrigin(); //设置源(ORIGIN)参数
	void SetDlisParameter(); //修改或补充DLIS文件参数
	BYTE XtfRepCodeToDlis(int RepCode); //设置表示码
	void SetDlisFrameFromXtf(); //设置DLIS帧
	void SetAxisFromXtf(); //设置轴
	void XtfDataToDlis(); //XTF数据向DLIS转换

private:
	CWordArray CurveItem; //分组曲线索引，其结构为：组数->第１组项数及索引值->第２组项数及索引...


private:
	CString m_SourceFile; //源文件名
	CString m_sObjectPath; //输出目录
	CString sInWellPath; //输入井路径
	CString sInWellName; //从FID文件名得到的井名
	CString sInField; //从FID文件名得到的油田名
	int m_nCurvesRadio; //过滤开关
	CString m_sProducer; //生产者名称(含代码)
	FILE *fp;

private:
	CXtfRead *XtfRead;
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

#endif // !defined(AFX_XTFTODLIS_H_INCLUDED_)

// LisToDlis.h: interface for the CLisToDlis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTODLIS_H_INCLUDED_)
#define AFX_LISTODLIS_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LisRead.h"
#include "DlisWrite.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"

class CLisToDlis  
{
public:
	CLisToDlis();
	virtual ~CLisToDlis();
	bool LisToDlis(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sProducer,FILE *fo);

private:
	bool Transform();//转换控制
	void SelectCurves();//选择曲线
	bool LogicalToDlis(int NumFile); //转换一个逻辑文件
	void SetStoUnitLab();//设置存储单元标志
	void GetDlisFDataIndexFromLis();//取得DLIS数据分帧信息
	void Prt_FdataItemInfo(); //打印曲线分帧信息
	void SetDlisChannelsFromLis(); //设置DLIS通道信息
	void SetDlisFrameFromLis();//设置DLIS帧
	void SetAxisFromLis();//设置轴
	void SetFileHeader(int NumFile);//设置文件头(FILE-HEADER)参数
	void SetOrigin(int NumFile);//设置源(ORIGIN)参数
	void SetDlisParameter();//修改或补充DLIS文件参数
	void LisDataToDlis();//逻辑文件数据体的转换
	BYTE FidRepCToDlis(BYTE FidRepC);//设置表示码

private:
	CString m_SourceFile; //源文件名
	CString m_sObjectPath; //输出目录
	int m_nCurvesRadio; //过滤开关
	FILE *fp;
	int *ItemNum; //曲线属组
	int	NumItem; //组数

private:
	CLisRead *LisRead;
	CDlisWrite *DlisWrite;
	CString DlisFileName; //DLIS输出文件名
	CString sProducerName; //公司名
	CString OriginOfdata; //数据来源

private:
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CStringArray NewUnits;
	CString m_sProducer; //生产者名称(含代码)

	CProgressDlg* pGuage;

};

#endif // !defined(AFX_LISTODLIS_H_INCLUDED_)

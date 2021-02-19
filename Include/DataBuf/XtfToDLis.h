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
	bool Transform(); //ת������
	void SelectCurves(); //ѡ������
	void GetDlisFDataIndexFromXtf(); //��Xtf������������ȡ��DLIS���ݷ�֡��Ϣ
	void SetDlisChannelsFromXtf(); //��Xtf��������Ϣ����DLISͨ����Ϣ
	void Prt_FdataItemInfo(); //��ӡ���߷�֡��Ϣ
	void SetStoUnitLab(); //���ô洢��Ԫ��־
	void SetFileHeader(); //�����ļ�ͷ(FILE-HEADER)����
	void SetOrigin(); //����Դ(ORIGIN)����
	void SetDlisParameter(); //�޸Ļ򲹳�DLIS�ļ�����
	BYTE XtfRepCodeToDlis(int RepCode); //���ñ�ʾ��
	void SetDlisFrameFromXtf(); //����DLIS֡
	void SetAxisFromXtf(); //������
	void XtfDataToDlis(); //XTF������DLISת��

private:
	CWordArray CurveItem; //����������������ṹΪ������->�ڣ�������������ֵ->�ڣ�������������...


private:
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	CString sInWellPath; //���뾮·��
	CString sInWellName; //��FID�ļ����õ��ľ���
	CString sInField; //��FID�ļ����õ���������
	int m_nCurvesRadio; //���˿���
	CString m_sProducer; //����������(������)
	FILE *fp;

private:
	CXtfRead *XtfRead;
	CDlisWrite *DlisWrite;

private:
	CProgressDlg* pGuage; //������
	CString DlisFileName; //��ǰ����ļ�
	CWordArray CurveIndex; //Դ����������
	CStringArray SelCurveName; //ѡ����������
	CStringArray NewCurveName; //����������
	CStringArray NewUnits; ////�µ�λ����
	int *ItemNum; //��������
	int	NumItem; //����
	WORD ProducerCode; //�����ߴ���
	char ProducerName[64]; //����������

};

#endif // !defined(AFX_XTFTODLIS_H_INCLUDED_)

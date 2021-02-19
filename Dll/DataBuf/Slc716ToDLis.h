// Slc716ToDLis.h: interface for the CSlc716ToDLis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLC716TODLIS_H_INCLUDED_)
#define AFX_SLC716TODLIS_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "Slc716R.h"
#include "DLisWrite.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"

class CSlc716ToDLis  
{
public:
	CSlc716ToDLis();
	virtual ~CSlc716ToDLis();
	bool Slc716ToDlis(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sProducer,FILE *fo);
private:
	bool Transform(); //ת������
	void SelectCurves(); //ѡ������
	void SetDlisChannelsFromSlc716(); //����DLISͨ����Ϣ
	void SetStoUnitLab(); //���ô洢��Ԫ��־
	void SetFileHeader(); //�����ļ�ͷ(FILE-HEADER)����
	void SetOrigin(); //����Դ(ORIGIN)����
	void SetDlisParameter(); //�޸Ļ򲹳�DLIS�ļ�����
	void SetDlisFrameFromSlc716(); //����DLIS֡
	void Slc716DataToDlis(); //SLC716������DLISת��

private:
	CWordArray CurveItem; //����������������ṹΪ������->�ڣ�������������ֵ->�ڣ�������������...


private:
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	CString m_sProducer; //����������(������)
	FILE *fp;

private:
	CSlc716R *Slc716R;
	CDlisWrite *DlisWrite;

private:
	CProgressDlg* pGuage; //������
	CString DlisFileName; //��ǰ����ļ�
	CStringArray SelCurveName; //ѡ����������
	CStringArray NewCurveName; //����������
	CStringArray NewUnits; ////�µ�λ����
	CWordArray CurveIndex; //Դ����������
	WORD ProducerCode; //�����ߴ���
	char ProducerName[64]; //����������

};

#endif // !defined(AFX_SLC716TODLIS_H_INCLUDED_)

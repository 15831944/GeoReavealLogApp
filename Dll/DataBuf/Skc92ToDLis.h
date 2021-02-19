// Skc92ToDLis.h: interface for the CSkc92ToDLis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKC92TODLIS_H_INCLUDED_)
#define AFX_SKC92TODLIS_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "Skc92R.h"
#include "DLisWrite.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"

class CSkc92ToDLis  
{
public:
	CSkc92ToDLis();
	virtual ~CSkc92ToDLis();
	bool Skc92ToDLis(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sProducer,FILE *fo);
private:
	bool Transform(); //ת������
	void SelectCurves(); //ѡ������
	void GetDlisFDataIndexFromSkc92(); //ȡ��DLIS���ݷ�֡��Ϣ
	void SetDlisChannelsFromSkc92(); //����DLISͨ����Ϣ
	void Prt_FdataItemInfo(); //��ӡ���߷�֡��Ϣ
	void SetStoUnitLab(); //���ô洢��Ԫ��־
	void SetFileHeader(); //�����ļ�ͷ(FILE-HEADER)����
	void SetOrigin(); //����Դ(ORIGIN)����
	void SetDlisParameter(); //�޸Ļ򲹳�DLIS�ļ�����
	void SetDlisFrameFromSkc92(); //����DLIS֡
	void SetAxisFromSkc92(); //������
	void Skc92DataToDlis(); //SKc92������DLISת��

private:
	CWordArray CurveItem; //����������������ṹΪ������->�ڣ�������������ֵ->�ڣ�������������...
	bool IsCCLCurve(int index);


private:
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	CString sInWellPath; //���뾮·��
	CString sInWellName; //�õ��ľ���
	CString sInField; //�õ���������
	int m_nCurvesRadio; //���˿���
	CString m_sProducer; //����������(������)
	FILE *fp;

private:
	CSkc92R *Skc92R;
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

#endif // !defined(AFX_SKC92TODLIS_H_INCLUDED_)

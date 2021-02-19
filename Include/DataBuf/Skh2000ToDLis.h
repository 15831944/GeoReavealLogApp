// Skh2000ToDLis.h: interface for the CSkh2000ToDLis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKH2000TODLIS_H_INCLUDED_)
#define AFX_SKH2000TODLIS_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "Skh2000R.h"
#include "DLisWrite.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"

class CSkh2000ToDLis  
{
public:
	CSkh2000ToDLis();
	virtual ~CSkh2000ToDLis();
	bool Skh2000ToDLis(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sProducer,FILE *fo);
private:
	bool Transform(); //ת������
	void SelectCurves(); //ѡ������
	void GetDlisFDataIndexFromSkh2000(); //ȡ��DLIS���ݷ�֡��Ϣ
	void SetDlisChannelsFromSkh2000(); //����DLISͨ����Ϣ
	void Prt_FdataItemInfo(); //��ӡ���߷�֡��Ϣ
	void SetStoUnitLab(); //���ô洢��Ԫ��־
	void SetFileHeader(); //�����ļ�ͷ(FILE-HEADER)����
	void SetOrigin(); //����Դ(ORIGIN)����
	void SetDlisParameter(); //�޸Ļ򲹳�DLIS�ļ�����
	void SetDlisFrameFromSkh2000(); //����DLIS֡
	void SetAxisFromSkh2000(); //������
	void Skh2000DataToDlis(); //SKH2000������DLISת��

private:
	CWordArray CurveItem; //����������������ṹΪ������->�ڣ�������������ֵ->�ڣ�������������...
	bool IsConvCurve(int index);


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
	CSkh2000R *Skh2000R;
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

#endif // !defined(AFX_SKH2000TODLIS_H_INCLUDED_)

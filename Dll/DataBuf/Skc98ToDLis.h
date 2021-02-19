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
	bool Transform(); //ת������
	void SelectCurves(); //ѡ������
	void GetDlisFDataIndexFromSkc98(); //ȡ��DLIS���ݷ�֡��Ϣ
	void SetDlisChannelsFromSkc98(); //����DLISͨ����Ϣ
	void Prt_FdataItemInfo(); //��ӡ���߷�֡��Ϣ
	void SetStoUnitLab(); //���ô洢��Ԫ��־
	void SetFileHeader(); //�����ļ�ͷ(FILE-HEADER)����
	void SetOrigin(); //����Դ(ORIGIN)����
	void SetDlisParameter(); //�޸Ļ򲹳�DLIS�ļ�����
	void SetDlisFrameFromSkc98(); //����DLIS֡
	void SetAxisFromSkc98(); //������
	void Skc98DataToDlis(); //SKC98������DLISת��

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
	CSkc98R *Skc98R;
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

#endif // !defined(AFX_SKC98TODLIS_H_INCLUDED_)

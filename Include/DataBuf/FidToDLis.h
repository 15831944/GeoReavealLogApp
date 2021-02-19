// FidToDLis.h: interface for the CFidToDLis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIDTODLIS_H_INCLUDED_)
#define AFX_FIDTODLIS_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "FidRead.h"
#include "DLisWrite.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"

class CFidToDLis  
{
public:
	CFidToDLis();
	virtual ~CFidToDLis();
	bool FidToDlis(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sProducer,FILE *fo);
private:
	bool Transform(); //ת������
	void SelectCurves(); //ѡ������
	void GetDlisFDataIndexFromFid(); //ȡ��DLIS���ݷ�֡��Ϣ
	void SetDlisChannelsFromFid(); //����DLISͨ����Ϣ
	void Prt_FdataItemInfo(); //��ӡ���߷�֡��Ϣ
	void SetStoUnitLab(); //���ô洢��Ԫ��־
	void SetFileHeader(); //�����ļ�ͷ(FILE-HEADER)����
	void SetOrigin(); //����Դ(ORIGIN)����
	void SetDlisParameter(); //�޸Ļ򲹳�DLIS�ļ�����
	BYTE FidRepCodeToDlis(int RepCode); //���ñ�ʾ��
	void SetDlisFrameFromFid(); //����DLIS֡
	void SetAxisFromFid(); //������
	void FidDataToDlis(); //FID������DLISת��

private:
	CWordArray CurveItem; //����������������ṹΪ������->�ڣ�������������ֵ->�ڣ�������������...


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
	CFidRead *FidRead;
	CDlisWrite *DlisWrite;

private:
	CProgressDlg* pGuage; //������
	CString DlisFileName; //��ǰ����ļ�
	CStringArray SelCurveName; //ѡ����������
	CStringArray NewCurveName; //����������
	CStringArray NewUnits; ////�µ�λ����
	int *ItemNum; //��������
	int	NumItem; //����
	WORD ProducerCode; //�����ߴ���
	char ProducerName[64]; //����������

};

#endif // !defined(AFX_FIDTODLIS_H_INCLUDED_)

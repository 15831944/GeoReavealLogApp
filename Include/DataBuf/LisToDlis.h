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
	bool Transform();//ת������
	void SelectCurves();//ѡ������
	bool LogicalToDlis(int NumFile); //ת��һ���߼��ļ�
	void SetStoUnitLab();//���ô洢��Ԫ��־
	void GetDlisFDataIndexFromLis();//ȡ��DLIS���ݷ�֡��Ϣ
	void Prt_FdataItemInfo(); //��ӡ���߷�֡��Ϣ
	void SetDlisChannelsFromLis(); //����DLISͨ����Ϣ
	void SetDlisFrameFromLis();//����DLIS֡
	void SetAxisFromLis();//������
	void SetFileHeader(int NumFile);//�����ļ�ͷ(FILE-HEADER)����
	void SetOrigin(int NumFile);//����Դ(ORIGIN)����
	void SetDlisParameter();//�޸Ļ򲹳�DLIS�ļ�����
	void LisDataToDlis();//�߼��ļ��������ת��
	BYTE FidRepCToDlis(BYTE FidRepC);//���ñ�ʾ��

private:
	CString m_SourceFile; //Դ�ļ���
	CString m_sObjectPath; //���Ŀ¼
	int m_nCurvesRadio; //���˿���
	FILE *fp;
	int *ItemNum; //��������
	int	NumItem; //����

private:
	CLisRead *LisRead;
	CDlisWrite *DlisWrite;
	CString DlisFileName; //DLIS����ļ���
	CString sProducerName; //��˾��
	CString OriginOfdata; //������Դ

private:
	CWordArray CurveIndex;
	CStringArray SelCurveName;
	CStringArray NewCurveName;
	CStringArray NewUnits;
	CString m_sProducer; //����������(������)

	CProgressDlg* pGuage;

};

#endif // !defined(AFX_LISTODLIS_H_INCLUDED_)

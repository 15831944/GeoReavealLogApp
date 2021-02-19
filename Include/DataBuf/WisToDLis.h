// WisToDLis.h: interface for the CWisToDLis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WISTODLIS_H_INCLUDED_)
#define AFX_WISTODLIS_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CodeSP.h"
#include "WisRead.h"
#include "DLisWrite.h"
#include "ProgDlg.h"
#include "ReCurNamDlg.h"

class CWisToDLis  
{
public:
	CWisToDLis();
	virtual ~CWisToDLis();
	bool WisToDLis(CString SourceFile,CString sObjectPath,int nCurvesRadio,CString sProducer,FILE *fo);
private:
	bool Transform(); //ת������
	void SelectCurves(); //ѡ������
	void GetDlisFDataIndexFromWis(); //ȡ��DLIS���ݷ�֡��Ϣ
	void SetDlisChannelsFromWis(); //����DLISͨ����Ϣ
	void Prt_FdataItemInfo(); //��ӡ���߷�֡��Ϣ
	void SetStoUnitLab(); //���ô洢��Ԫ��־
	void SetFileHeader(); //�����ļ�ͷ(FILE-HEADER)����
	void SetOrigin(); //����Դ(ORIGIN)����
	void SetDlisParameter(); //�޸Ļ򲹳�DLIS�ļ�����
	BYTE WisRepCodeToDlis(int RepCode); //���ñ�ʾ��
	void SetDlisFrameFromWis(); //����DLIS֡
	void SetAxisFromWis(); //������
	void WisDataToDlis(); //WIS������DLISת��

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
	CWisRead *WisRead;
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

#endif // !defined(AFX_WISTODLIS_H_INCLUDED_)

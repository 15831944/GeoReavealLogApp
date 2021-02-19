// DlisRead.h: interface for the CDlisRead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLISREAD_H_INCLUDED_)
#define AFX_DLISREAD_H_INCLUDED_

#include "Dlis_Head.h"
#include "DlisInput.h"
#include "CodeSP.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AFX_EXT_CLASS CDlisRead  
{
public:
	CDlisRead();
	virtual ~CDlisRead();
	bool Open(CString sDataFile,FILE *fo); //��DLIS�ļ�
	void Close(); //�ر�DLIS�ļ�
	int  GetDlisLogicalFileNumber(); //���ָ���߼��ļ���
	void ScanDlisFile(CString sDataFile,FILE *fo); //ɨ��DLIS�ļ�
	bool ScanDlisOneLogicalFile(int nf); //ɨ��һ��DLIS�߼��ļ�
	bool ReadLogicalFile(int nf);  //DLIS���ݶ�����֡Ϊ��λ���ļ�
	bool ReadCurve(int iFrame, int iCurve,int Num,float *buffer); //�������������ݵ�һ��
	bool ReadWaveArray(int iFrame, int iCurve,float Depth,float *buffer); //�����λ��������ݵ�һ��������
	void GetNewName(CString &sNewName,int noFrame,int Index,int CopyNum,int Origin,BOOL m_bAddFrameNo,BOOL m_bAddOrigin,BOOL m_bAddCopyNum);

private:
	void ReadStoUnitLab(); //��DLIS�洢����(�ļ���־)
	bool ReadPhyHeader(); //�������¼ͷ��Ϣ
	int  GetLogicalRecord(); //ȡ��һ���߼���¼��Ϣ
	CString RecordTypeToStr(BYTE RecType,BYTE Stru); //�����߼���¼���ʹ����ѯ��¼���ʹ�
	CString DlisCodeToStr(BYTE IRepCode); //����DLIS�������ʹ����ѯ���ʹ�
	BYTE DlisCodeOutType(BYTE IRepCode); //DLIS��ʾ�뵽�����ʾ��
	void Prt_StoUnitLab(); //��ӡ�ļ���־
	void Prt_LogFileIndex(); //�߼��ļ���������
	void InitVariable(); //��������ʼ��ֵ
	void RenewChannelData(); //����ͨ������
	void Prt_LogFileCurveInfo();//�߼��ļ�������Ϣ������ض��ļ�
	CFile fDlis; //DLIS�ļ�ָ��


public:
	CDWordArray LogFilePos; //�߼��ļ���ʼλ��
	DWORD FilePos; //�ļ���ǰλ��
	DWORD FileSize; //�ļ���С
	WORD NumChannel; //��������
	WORD NumAxis; //ʵ������
	WORD NumFrame; //֡������
	_StoUnitLab StoUnitLab;
	_PhyRecHead PhyRec;
	_FileHeader FileHeader;
	_LogRecHead LogRecHead;
	_LogRecAttrib LogRecAttrib;
	_Channel *Channel;
	_Axis *Axis;
	_Frame *Frame;
	_Origin Origin;
	int NumPrtFdata; //��ʾ֡������
	int NumPrtSample; //��ʾ������
	BOOL bPrtBaseInfo; //�Ƿ���ʾ������Ϣ
private:
	FILE *fp;
	CString m_sDataFile; //�����ļ���
	DWORD NumLogFile; //�߼��ļ���
	DWORD NumEncry; //���ܼ�¼��
	DWORD NumPhyRec; //�����¼��
	DWORD NumLogRec; //�߼���¼��
	DWORD nLogLen; //�߼���¼����(ȥ��β)
	DWORD LogRecBytes; //�߼���¼ȫ��(��β)

    char SetType[255+1]; //װ������
    char SetName[255+1]; //װ��������
	DWORD nLogPos; //�߼���¼λ��
	DWORD NpntLenC; //���Ķ������߼���¼�ֽ���
	DWORD NumEod; //Eod��¼��
	bool bRpr; //�Ƿ����һ�������¼
	DWORD BufLen; //����������
	BYTE *DlisBuf; //������

private:
	DWORD *IORIGIN;//����Դ
	BYTE  *ICOPYNUM;//���ݿ�����
	int   *IVALUE; //����ֵ
	float *VALUE; //����ֵ
	int   *LCVALUE; //�Ӵ�����
	char  *CVALUE; //�Ӵ���,�ٶ�һ���Ӵ�ƽ������Ϊ64�ֽ�

private:
	CDlisInput DlisIn; //DLIS�������ͽ���
	int  NumOrigin; //��ǰ�߼��ļ���ORIGIN��¼�� 
	BOOL bPrintEFLR; //��������Ƿ��ӡ��ֵ
	bool bTrans; //�Ƿ�Ϊ����ת��(trueΪת��)
	bool InitObject(DWORD IVORG1,BYTE IVCOPY1,char *ObjName); //��ʼ������
	void VariableReset(); //������λ
	bool DlisBufToEFLR(); //ֱ�Ӹ�ʽ��¼����
	bool ValueAnalyser(char *ObjName,char *AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd); //��ֵ����
	bool ValuesToFHLR(char *ObjName,char* AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd); //ȡ�ļ�ͷ�������ֵ
	bool ValuesToOLR(char *ObjName,char* AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd); //ȡԴ�������ֵ
	bool ValuesToCHANNL(char *ObjName,char* AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd); //ȡͨ���������ֵ
	bool ValuesToFRAME(char *ObjName,char* AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd); //ȡ֡�������ֵ
	bool ValuesToAXIS(char *ObjName,char* AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd); //ȡ֡�������ֵ

	void Prt_EflrPar(char *ObjName,char *AttribLabel,char *AttribUnits,DWORD ICount,BYTE IRepCd); //����Ҫ������ֵ��ʾ
	void Prt_RecordHeader(); //��ӡ��¼ͷ��Ϣ
	void Prt_FileHeader(); //��ӡ�ļ�ͷ��Ϣ
	void Prt_Channel(); //��ӡͨ��������Ϣ
	void Prt_Frame(); //��ӡ֡����(FRAME:FRAME)��Ϣ
	void Prt_Origin(); //��ӡԴ����(OLR:ORIGIN)��Ϣ
	void Prt_Eod(int IVORG, int IVCOPY, char *ObjName);
	void Prt_Axis(); //��ӡ�����

private:
	int	NumAttrib; //ģ��������
	int kAttrib; //��ǰ�������
	CStringArray TempAttribLabel; //ģ�����Ա�ʶ��
	CDWordArray TempAttribCount; //ģ�����Լ�����
	CByteArray TempAttribRepCd; //ģ�����Ա�ʾ��
	CStringArray TempAttribUnits; //ģ�����Ե�λ
	int NumSaveAttrib; //����ģ��������
	CString sTempFile; //����ģ����ʱ��ʱ�ļ�
	bool SaveAttrib(DWORD ICount,BYTE IRepCd); //����ģ������ֵ
	void LoadAttrib(); //����ģ������ֵ
	void SetZeroVALUE(); //����ȱʡֵ
	void GetIflrObject(int &IVORG, int &IVCOPY, int &NBYTE, char *ObjName); //ȡ�ü�Ӹ�ʽ�߼���¼������
	bool FdataDecode(int IVORG, int IVCOPY, int NBYTE, char *ObjName); //֡���ݽ���
	int GetFrameNo(int IVORG, int IVCOPY, char *ObjName); //ȡ�ø��������Ӧ��֡���
	void Prt_FData(DWORD i,int iChannel,int ICount,BYTE IRepCd); //��ӡ֡����
	CStringArray sMidFile; //�м������ļ�
	CFile *fMid,fTemp; //��ʱ�ļ�ָ��
	BYTE DlisCodeToFid(BYTE IRepCd); //DLIS��ʾ��ת��ΪFID��ʾ��
	bool ReadData(DWORD pos,BYTE RepCode,BYTE CodeLen,DWORD count,float *buf); //����ʱ�ļ���ȡ����
};
bool _declspec(dllexport) IsDLIS(CString sDataFile);

#endif // !defined(AFX_DLISREAD_H_INCLUDED_)

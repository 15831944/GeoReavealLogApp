// DlisWrite.h: interface for the CDlisWrite class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DLISWRITE_H__
#define __DLISWRITE_H__

#include "CodeSP.h"
#include "DlisOut.h"


class AFX_EXT_CLASS CDlisWrite  
{
public:
	CDlisWrite();
	virtual ~CDlisWrite();
	CFile fDlis;
	bool Open(CString sDataFile,FILE *fo); //������ļ�
	void Close(); //�ر�����ļ�
	void SetTrailLengthFlag(bool bFlag=false); //����β���ȱ�־
	void SetChannelNumber(int Num); //����ͨ���ռ�
	void SetFrameNumber(int Num); //����֡�ռ�
	void SetDlisAxisNumber(int Num); //������ռ�
	void WriteStoUnitLab(); //д�洢��Ԫ��־
	void WriteFileHeader(); //д�ļ�ͷ���̻�����
	void WriteLogBufToDlisBuf(BYTE RecFlag,BYTE RecType,int Buflen,BYTE *buf,bool bWFlag=false); //���߼���¼���������̻�����
	void WriteOrigin(); //дԴ���̻����� 
	void WriteParameter(); //д����STATIC:PARAMETER���̻�����
	void WriteAxis(); //д�ᵽ�̻����� 
	void WriteChannel(); //дͨ�����̻����� 
	void WriteFrame(); //д֡���̻����� 
	void StartWriteData(int noFrame); //��ʼ���ָ��֡����
	void StopWriteData(int noFrame); //�������ָ��֡����
	void WriteCurveData(int nItem,int nCurve,int NumR,float *buf); //�����������
	void WriteArrayData(int nItem,int iCurve,int noFra,int Count,float *vFloat); //������и�������
	void WriteArrayData(int nItem,int nCurve,int noFra,int Count,char *vChar); //��������ֽ�����
	void WriteArrayData(int nItem,int nCurve,int noFra,int Count,short *vShort); //������ж���������
	void Prt_Header(); //��ӡDLISͷ����

private:
	void VariableReset(); //����ʼ��
	void WriteDlisBufDToisk(); //�������������,�����û�����Ϊ0�ֽ�
	void FrameDataSize(); //֡���ݵĳ���
	int DlisCodeLen(BYTE RepCode); //DLIS���볤��
	void PutCompSET(const char *Type,const char *Name,int &IP,BYTE *buf); //��װ������鵽������
	void PutCompATTRIB(const char *Label,const DWORD Count,const BYTE RepCode,const char *Units,int &IP,BYTE *buf); //����������鵽������
	void PutCompOBJECT(const int Origin,const BYTE CopyNum,const char *Name,int &IP,BYTE *buf); //�Ͷ�������鵽������
	void SetIndexCurveInfo(); //��������������Ϣ
	void CheckLogBufLen(); //��鲢�޸��߼���¼����������
	void FloatToLogBuf(float vf,BYTE RepCode,int &NBYTE,BYTE *buf);

public:
	int MaxPhyLen; //��������¼����
	_PhyRecHead PhyRecHead; //�����¼ͷ��Ϣ
	bool bTrailLengthFlag; //�Ƿ��¼β����
	_LogRecAttrib LogRecAttrib; //�߼���¼����λ

public:
	WORD NumChannel; //��������
	WORD NumAxis; //ʵ������
	WORD NumFrame; //֡������
	_StoUnitLab StoUnitLab; //�洢��־
	_FileHeader FileHeader; //�ļ�ͷ
	_Origin Origin; //Դ
	_Channel *Channel;
	_Frame *Frame;
	_Axis *Axis; //��

private:
	CDlisOut DlisOut; //DLIS���ݱ���
	int DiskBufLen; //�ȴ���������ݳ���
	int LogBufLen; //�߼���¼����������
	BYTE *DiskBuf; //�ȴ����������
	BYTE *LogBuf; //�߼���¼���ݻ�����
private:
	DWORD *IORIGIN;//����Դ
	BYTE  *ICOPYNUM;//���ݿ�����
	int   *IVALUE; //����ֵ
	float *VALUE; //����ֵ
	char  *CVALUE; //�Ӵ���,�ٶ�һ���Ӵ�ƽ������Ϊ64�ֽ�


private:
	FILE *fp;
	CFile fTmp;
	CString sMidFile;
	CString m_sDataFile; //����ļ���
};

#endif 

// CodeSP.h: interface for .
//
//////////////////////////////////////////////////////////////////////

#ifndef __CODESP_H__
#define __CODESP_H__

//C++�����������Ͷ���
//                     ����ֵ  ���ͳ���   ����˵��
#define PC_REPR_INT     1     // 4        �з�������
#define PC_REPR_SHORT   2     // 2        �з��Ŷ�������
#define PC_REPR_LONG    3     // 4        �з��ų�������
#define PC_REPR_FLOAT   4     // 4        IEEE�����ȸ����� 
#define PC_REPR_DOUBLE  5     // 8        IEE˫���ȸ�����
#define PC_REPR_STRING  6     // ����     �ַ���
#define PC_REPR_CHAR    7     // 1        �з����ֽ���
#define PC_REPR_UCHAR   8     // 1        �޷����ֽ���
#define PC_REPR_USHORT  9     // 2        �޷��Ŷ�������
#define PC_REPR_UINT   10     // 4        �޷�������
#define PC_REPR_ULONG  11     // 4        �޷��ų�������

//WIS�ļ��������Ͷ���
//					����ֵ		���ͳ���  	����˵��
#define WIS_REPR_INT     1	//	4			�з�������
#define WIS_REPR_SHORT   2	//	2			�з��Ŷ�������
#define WIS_REPR_LONG    3	//	4			�з��ų�������
#define WIS_REPR_FLOAT   4	//	4			IEEE�����ȸ�����	
#define WIS_REPR_DOUBLE  5	//	8			IEE˫���ȸ�����
#define WIS_REPR_STRING  6	//  ����		�ַ���
#define WIS_REPR_CHAR    7	//	1			�з����ֽ���
#define WIS_REPR_UCHAR   8	//	1			�޷����ֽ���
#define WIS_REPR_USHORT  9	//	2			�޷��Ŷ�������
#define WIS_REPR_UINT   10	//	4			�޷�������
#define WIS_REPR_ULONG  11	//	4			�޷��ų�������

//�ļ����ı��͵���ճ��
BOOL _declspec(dllexport) TextFileToClipboard(char *ResultFile);
BOOL _declspec(dllexport) TextToClipboard(char *szBuf);

//ɾ������ǰ�����ǰ����ַ�
void _declspec(dllexport) StrTrimLeft(char *str);
void _declspec(dllexport) StrTrimRight(char *str);
void _declspec(dllexport) StrTrimLeftRight(char *str);

//UNIX��DOS����ת��
void _declspec(dllexport) SunI2TOPC(short &i2);
void _declspec(dllexport) SunI2TOPC(unsigned short &i2);
void _declspec(dllexport) SunI4TOPC(int &i4);
void _declspec(dllexport) SunI4TOPC(unsigned int &i4);
void _declspec(dllexport) SunR4TOPC(float &r4);
void _declspec(dllexport) SunR8TOPC(double &r8);
void _declspec(dllexport) MSunI2TOPC(int count,short *i2);
void _declspec(dllexport) MSunI2TOPC(int count,unsigned short *i2);
void _declspec(dllexport) MSunI4TOPC(int count,int *i4);
void _declspec(dllexport) MSunI4TOPC(int count,unsigned int *i4);
void _declspec(dllexport) MSunR4TOPC(int count,float *r4);
void _declspec(dllexport) MSunR8TOPC(int count,double *r8);

//4�ֽڳ��������븡���ת��
float _declspec(dllexport) Int4ToFloat(int i4);
void _declspec(dllexport) MInt4ToFloat(int count,int *i4,float *r4);
int _declspec(dllexport) FloatToInt4(float r4);
void _declspec(dllexport) MFloatToInt4(int count,float *r4,int *i4);

//BIT�ļ���������ת��
void _declspec(dllexport) GetBitR4(float &r4);

//�Ƿ�Ϊһ�Ϸ�·��
bool _declspec(dllexport) IsDirectory (const CString sDirName);

//�Ƿ�Ϊһ�Ϸ��ļ�
bool _declspec(dllexport) IsFileName(const CString sFileName);

//�Ƿ�Ϊһ�Ϸ�·�����ļ�
bool _declspec(dllexport) IsFilePathName(const CString sName);

//ȡ���ļ�����
int _declspec(dllexport) GetFileLength(CString SCurDataFile);

//��ȫ·���ļ�ȡ���Ӵ�
CString _declspec(dllexport) MyGetPathName(const CString szFilename);
CString _declspec(dllexport) MyGetFileName(const CString szFilename);
CString _declspec(dllexport) MyGetFileExt(const CString szFilename);
CString _declspec(dllexport) MyGetFileTitle(const CString szFilename);
CString _declspec(dllexport) MyGetPathNameFileTitle(const CString szFilename);

//���²���
void _declspec(dllexport) FloatResample(float *buf,int NumX,int NumY);
void _declspec(dllexport) FloatResample(float Start1,float Stop1,float Rlev1,float *buf1,float Start2,float Stop2,float Rlev2,float *buf2);

//ѡ���ļ�
void _declspec(dllexport) SelectOneFile(const char *szTitle,bool bOpen,const char *szFilter,char *szFileName,const char *ExtName,const char *OldFile);

//�ļ��ϲ�
bool _declspec(dllexport) FileUnite(const CString sObjectFile,const CString SourceFile); //��SourceFile�ϲ���sObjectFile

//�����ļ�
bool _declspec(dllexport) PutStringToFile(const CString sText,const CString sFile,BOOL bWarnIfExists=TRUE);

//ע����д
bool _declspec(dllexport) RegKeyValueQuery(const CString szKey,const CString lpszEntry,CString &strValue);
bool _declspec(dllexport) RegKeyValueWrite(const CString szKey, const CString lpszEntry,const CString strValue);
bool _declspec(dllexport) RegKeyValueQueryDword(const CString szKey,const CString lpszEntry,DWORD *dwFeatureSet);

//�Ӵ���ȡ����n������
CString _declspec(dllexport) MyGetSubStr(const CString szSource,int NumItem);

//Wis�ļ����ݱ�ʾ���Ӧ����
int _declspec(dllexport) WisCodeLen(DWORD RepCode);

//C++�������ݱ�ʾ���Ӧ����
int _declspec(dllexport) PcCodeLen(DWORD RepCode);

//�ַ����Ƿ�û�п���ʾ�ַ�
BOOL _declspec(dllexport) IsNullString(const char *str);
//�Ƿ�ȫΪ7λASCIIֵ�Ĵ�,�Ҳ�Ϊ�ո�
BOOL _declspec(dllexport) IsAsciiString(CString Source);

BOOL _declspec(dllexport) CheckLoginCode();
void GetHardwareCode(char *HardwareCode);
void HardwareCode2LoginCode(char *HardwareCode,char *LoginCode);
void _declspec(dllexport) ModiFileName(const char* Editer);
CString _declspec(dllexport) GetWorkPath(CString flag);
void _declspec(dllexport) LoadDataBufProPara();
BOOL _declspec(dllexport) DelDirAllFile(CString DirName);
#endif 

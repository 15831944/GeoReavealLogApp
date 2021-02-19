// XtfRead.cpp: implementation of the CXtfRead class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XtfRead.h"
#include <math.h>
#include <ctype.h>
#include <float.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXtfRead::CXtfRead()
{
}

CXtfRead::~CXtfRead()
{
}
bool _declspec(dllexport) IsXTF(CString sDataFile)
{
	CFile f;
	char Xtf_Label[4],str[5];
	int HeaderMap[8];
	if(f.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE) return false;
	f.Seek(0,CFile::begin);
	f.Read(HeaderMap,32);
	f.Seek(948,CFile::begin);
	BYTE System_Code;//ϵͳ���룬1=PC��2=PE��3=VAX��4=IBM��5=HP9000/SUN
	f.Read(&System_Code,1);
	if(System_Code==5)
	{
		MSunI4TOPC(8,HeaderMap);
	}
	f.Seek(760,CFile::begin);
	f.Read(Xtf_Label,4);
	f.Close();
	for(int i=0;i<8;i++)
	{
		if(HeaderMap[i]!=(i+1))
		{
			return false;
		}
	}
	sprintf(str,"%-4.4s",Xtf_Label);
	if(stricmp(str,".xtf")!=0)
	{
		return false;
	}
	return true;
}
bool CXtfRead::Open(CString sDataFile)
{
	if(fXtf.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE)
	{
		return false;
	}
	FileSize=fXtf.GetLength(); //�ļ�����
	fXtf.Seek(948,CFile::begin);
	BYTE System_Code;
	fXtf.Read(&System_Code,1);
	if(System_Code!=5 && System_Code!=1)
	{
		//ϵͳ���벻��hp9000/Sun��PC,�����޷�ʶ��
		fXtf.Close();
		return false;
	}
	return true;
}

void CXtfRead::Close()
{
	fXtf.Close();
}
void CXtfRead::GetHead()
{
	CString s;
	//��һ��¼
	fXtf.Seek(0,CFile::begin);
	fXtf.Read(XtfHead.header_location,32);
	fXtf.Read(XtfHead.iunused,182*4);
	fXtf.Read(XtfHead.extension,4);
	fXtf.Read(XtfHead.FileFullName,80);
	fXtf.Read(XtfHead.Depth_Unit,8);
	fXtf.Read(XtfHead.AIF_Revision_numbar,8);
	fXtf.Read(XtfHead.strunused,88);
	fXtf.Read(&XtfHead.System_Code,1);
	fXtf.Read(&XtfHead.QC,1);
	fXtf.Read(&XtfHead.Major_Revision_Number,1);
	fXtf.Read(&XtfHead.Minor_Revision_Number,1);
	fXtf.Read(XtfHead.hunused,44);
	fXtf.Read(&XtfHead.isnumcv,4);
	fXtf.Read(&XtfHead.ismaxcv,4);
	fXtf.Read(XtfHead.iunused2,20);
	fXtf.Read(XtfHead.UserData,64*16);
	fXtf.Read(&XtfHead.wttop,4);
	fXtf.Read(&XtfHead.wtbottom,4);
	fXtf.Read(&XtfHead.wtlevel,4);
	fXtf.Read(XtfHead.strunused2,1012);
	fXtf.Read(XtfHead.strunused3,1020);
	fXtf.Read(&XtfHead.FileRecord,4);

	//�ڶ���¼
	fXtf.Read(XtfHead.Datamap,4072);
	fXtf.Read(&XtfHead.bitmapfullflag,4);
	fXtf.Read(&XtfHead.firstrecord,4);
	fXtf.Read(&XtfHead.lastrecord,4);
	fXtf.Read(&XtfHead.prerecord,4);
	fXtf.Read(&XtfHead.thisrecord,4);
	fXtf.Read(&XtfHead.nextrecord,4);

	//������¼
	fXtf.Read(XtfHead.CurveName,4096);
	
	//���ļ�¼
	for(int i=0;i<256;i++)
	{
		fXtf.Read(&XtfHead.StartOffset[i],sizeof(int));
		fXtf.Read(&XtfHead.StartOffset[256+i],sizeof(int));
	}
	for(i=0;i<256;i++)
	{
		fXtf.Read(&XtfHead.LevelNumber[i],sizeof(int));
		fXtf.Read(&XtfHead.LevelNumber[256+i],sizeof(int));
	}

	//�����¼
	for(i=0;i<256;i++)
	{
		fXtf.Read(&XtfHead.ndimension[i],sizeof(short));
		fXtf.Read(&XtfHead.idimen1[i],sizeof(short));
		fXtf.Read(&XtfHead.ndimension[256+i],sizeof(short));
		fXtf.Read(&XtfHead.idimen1[256+i],sizeof(short));
	}
	for(i=0;i<256;i++)
	{
		fXtf.Read(&XtfHead.idimen2[i],sizeof(short));
		fXtf.Read(&XtfHead.idimen3[i],sizeof(short));
		fXtf.Read(&XtfHead.idimen2[256+i],sizeof(short));
		fXtf.Read(&XtfHead.idimen3[256+i],sizeof(short));
	}

	//������¼
	for(i=0;i<256;i++)
	{
		fXtf.Read(&XtfHead.Sdep[i],sizeof(float));
		fXtf.Read(&XtfHead.Sdep[i+256],sizeof(float));
	}
	for(i=0;i<256;i++)
	{
		fXtf.Read(&XtfHead.Edep[i],sizeof(float));
		fXtf.Read(&XtfHead.Edep[i+256],sizeof(float));
	}

	//���߼�¼
	for(i=0;i<256;i++)
	{
		fXtf.Read(&XtfHead.Level[i],sizeof(float));
		fXtf.Read(&XtfHead.Level[256+i],sizeof(float));
	}
	for(i=0;i<256;i++)
	{
		fXtf.Read(&XtfHead.CurveAttrib[i],sizeof(XtfHead.CurveAttrib[i]));
		fXtf.Read(&XtfHead.CurveAttrib[256+i],sizeof(XtfHead.CurveAttrib[256+i]));
	}

	//�ڰ˼�¼
	fXtf.Read(XtfHead.Wellsiteinfo,8); //��WSI����ǣ���ʾһ����Ч�ľ�����Ϣ��¼
	fXtf.Read(XtfHead.Wellname,80);     //����
	fXtf.Read(XtfHead.Fieldname,80);    //��������
	fXtf.Read(XtfHead.Companyname,80);  //��˾��
	fXtf.Read(XtfHead.Countryname,32);  //������
	fXtf.Read(XtfHead.Countyname,32);   //����
	fXtf.Read(XtfHead.Statename,16);    //����
	fXtf.Read(XtfHead.OCTname,8);       //�������Ʊ���
	fXtf.Read(XtfHead.Welllocation,40); //����λ��
	fXtf.Read(XtfHead.Section,4);       //������
	fXtf.Read(XtfHead.Township,4);      //������
	fXtf.Read(XtfHead.Range,4);         //��Χ
	fXtf.Read(XtfHead.Permanentdatum,2); //��������,��gl����sl��
	fXtf.Read(XtfHead.Drillingmeasurementdatum,2);  //�꾮��������(��kb������gl����)
	fXtf.Read(XtfHead.LogAnalysisCenter,4);  //�⾮�������Ĵ���
	fXtf.Read(XtfHead.Wellunits,8);          //����������λ
	fXtf.Read(XtfHead.strunused4,620);    //δ��

	fXtf.Read(&XtfHead.Latitude,4);   //����
	fXtf.Read(&XtfHead.Longitude,4);  //γ��
	fXtf.Read(&XtfHead.Kellybushing,4);  //���겹�ĺ����߶�
	fXtf.Read(&XtfHead.Derrickfloor,4);  //����ƽ̨�����߶�
	fXtf.Read(&XtfHead.Elevation,4);    //������������
	fXtf.Read(&XtfHead.MagneticDeclination,4);  //��ƫ��
	fXtf.Read(&XtfHead.ECC,4);   //ECC��
	fXtf.Read(&XtfHead.Groundtosealevelelevation,4); //���溣���߶�
	fXtf.Read(XtfHead.funused,120*sizeof(float));   //δ�õ�������
	fXtf.Read(XtfHead.dunused,64*sizeof(double));   //δ��˫������

	fXtf.Read(XtfHead.hunused2,6*sizeof(short));  //δ��
	fXtf.Read(&XtfHead.Metricflag,sizeof(short));  //���Ʊ��
	fXtf.Read(&XtfHead.Maximumcurveversion,sizeof(short)); //�����������(1-99)
	fXtf.Read(XtfHead.hunused3,504*sizeof(short));  //δ��
	fXtf.Read(XtfHead.unformattedbinaryinformation,1024); //δ��ʽ����������Ϣ

	//��ϵͳ��������Ƿ���в�ͬ����֮������ת��
	if(XtfHead.System_Code==0x05)
	{
		//����ΪSUN����վ����,Ӧת��
		SunXtfHead2PC();
	}
	XtfHead.ismaxcv=512; //��Щ�ļ��������������������
	//��XTF�ļ���ɾ�������Կ����������֣������Ӵ˹���
	ClearHeaderPack();
	for(int Index=0;Index<XtfHead.isnumcv;Index++)
	{
		OpenCurve(Index);
		memcpy(XtfHead.Units[Index],XtfCurve.Units,8);
		memcpy(XtfHead.IndexUnit,XtfCurve.DepthUnits,8);
	}
}
void CXtfRead::SunXtfHead2PC()
{
	//��һ��¼��Ҫת������Ϣ
	MSunI4TOPC(8,XtfHead.header_location);
	SunI4TOPC(XtfHead.isnumcv);
	SunI4TOPC(XtfHead.ismaxcv);
	for(int i=0;i<64;i++)
	{
		SunI4TOPC(XtfHead.UserData[i].StartRecord);
		SunI4TOPC(XtfHead.UserData[i].StopRecord);
	}
	SunR4TOPC(XtfHead.wttop);
	SunR4TOPC(XtfHead.wtbottom);
	SunR4TOPC(XtfHead.wtlevel);
	SunI4TOPC(XtfHead.FileRecord);

	//�ڶ���¼��Ҫת������Ϣ
	SunI4TOPC(XtfHead.bitmapfullflag);
	SunI4TOPC(XtfHead.firstrecord);
	SunI4TOPC(XtfHead.lastrecord);
	SunI4TOPC(XtfHead.prerecord);
	SunI4TOPC(XtfHead.thisrecord);
	SunI4TOPC(XtfHead.nextrecord);

	//������¼Ϊ������������ת��

	//���ļ�¼��Ҫת������Ϣ
	MSunI4TOPC(512,XtfHead.StartOffset);
	MSunI4TOPC(512,XtfHead.LevelNumber);

	//�����¼��Ҫת������Ϣ
	MSunI2TOPC(512,XtfHead.ndimension);
	MSunI2TOPC(512,XtfHead.idimen1);
	MSunI2TOPC(512,XtfHead.idimen2);
	MSunI2TOPC(512,XtfHead.idimen3);

	//������¼��Ҫת������Ϣ
	MSunR4TOPC(512,XtfHead.Sdep);
	MSunR4TOPC(512,XtfHead.Edep);

	//���߼�¼��Ҫת������Ϣ
	MSunR4TOPC(512,XtfHead.Level);

	//�ڰ˼�¼��Ҫת������Ϣ
	SunR4TOPC(XtfHead.Latitude);
	SunR4TOPC(XtfHead.Longitude);
	SunR4TOPC(XtfHead.Kellybushing);
	SunR4TOPC(XtfHead.Derrickfloor);
	SunR4TOPC(XtfHead.Elevation);
	SunR4TOPC(XtfHead.MagneticDeclination);
	SunR4TOPC(XtfHead.ECC);
	SunR4TOPC(XtfHead.Groundtosealevelelevation);
	SunI2TOPC(XtfHead.Metricflag);
	SunI2TOPC(XtfHead.Maximumcurveversion);
}

void CXtfRead::SunCurveHead2PC()
{
	SunR4TOPC(XtfCurve.deptop);  //���ߵĶ������
	SunR4TOPC(XtfCurve.depbot);  //���ߵĵײ����
	SunR4TOPC(XtfCurve.rlevcv);  //���ߵĲ������
	SunR4TOPC(XtfCurve.curvmin); //������Сֵ
	SunR4TOPC(XtfCurve.curvmax); //�������ֵ
	SunR4TOPC(XtfCurve.curvavg); //����ƽ��ֵ
	SunR4TOPC(XtfCurve.timeinc); //ʱ�����������������
	SunR4TOPC(XtfCurve.starttm); //������������ʼʱ��
	SunR4TOPC(XtfCurve.stadepth); //��̬�������
	SunR4TOPC(XtfCurve.stddev);  //��׼����
	SunR4TOPC(XtfCurve.cpwinbeg); //���ͼ������Window begin��
	SunR4TOPC(XtfCurve.cpwinstp); //���ͼ������Window step��
	SunR4TOPC(XtfCurve.cpwinlen); //���ͼ������Window length��
	SunR4TOPC(XtfCurve.cbilodeg); //CBIL��λ�Ķ���
	SunR4TOPC(XtfCurve.IntervalTR);  //���������������ľ���
	SunR4TOPC(XtfCurve.IntervalRR);  //���������������ľ���
	SunR4TOPC(XtfCurve.RawOffset);   //ԭʼ����ƫת
	SunR4TOPC(XtfCurve.TimeDelay);   //ʱ���ӳ�
	SunR4TOPC(XtfCurve.Angle);       //�ο��۵������б۵ļн�
	SunR4TOPC(XtfCurve.XPad);    //�����ϴ�������Xλ��
	SunR4TOPC(XtfCurve.YPad);    //�����ϴ�������Yλ��
	MSunR4TOPC(6,XtfCurve.LossMinor); //�μ���ʧ����ֵ

	SunR8TOPC(XtfCurve.dblinit);  //˫���ȳ�ʼֵ
	SunR8TOPC(XtfCurve.dbtopdep); //˫���ȶ������
	SunR8TOPC(XtfCurve.dbbotdep); //˫���ȵײ����
	SunR8TOPC(XtfCurve.dbrlevel); //˫���Ȳ������
	MSunR8TOPC(6,XtfCurve.smvalsdb); //Array pf 6 secondary Missing Datum Value
	SunR8TOPC(XtfCurve.idxtop2); //�μ���������ͷ��
	SunR8TOPC(XtfCurve.idxbot2); //�μ����������ײ�
	SunR8TOPC(XtfCurve.idxrlev2); //�μ������������

	SunI4TOPC(XtfCurve.EncodedCreationDate);  //�������ڱ���
	SunI4TOPC(XtfCurve.EncodedCreationTime);  //����ʱ�����
	SunI4TOPC(XtfCurve.EncodedLastAccessDate);  //����ȡ���ڱ���
	SunI4TOPC(XtfCurve.EncodedLastAccessTime);  //����ȡʱ�����
	SunI4TOPC(XtfCurve.NumberOfCurveHeaderrecords);  //����ͷ��¼��
	SunI4TOPC(XtfCurve.RecordNumberOfLastCurvedataRecord);  //�������һ�����ݼ�¼��
	SunI4TOPC(XtfCurve.DatumInitializationVariable);  //���ݳ�ʼ����������4�ֽ���
	SunI4TOPC(XtfCurve.NumberOfLevelsForTheCurve);  //���ߵĲ�����
	SunI4TOPC(XtfCurve.CurrentEncodedDateSignature);  //��ǰ�������ڱ��
	SunI4TOPC(XtfCurve.CurrentEncodedTimeSignature);  //��ǰ�������ڱ��
	MSunI4TOPC(6,XtfCurve.LossMinor2); //6���μ���ʧ����ֵ������
	MSunI4TOPC(2,XtfCurve.Arrays2);  //��ֵ���У�����ṹ�����߶������ĩ��¼
	MSunI4TOPC(2,XtfCurve.Arrays3);  //��ֵ���У�����ṹ����������������ĩ��¼
	
	SunI2TOPC(XtfCurve.SurvlibMajor);  //SURVLIB��Ҫ��
	SunI2TOPC(XtfCurve.SurvlibMinor); //SURVLIB��Ҫ��
	SunI2TOPC(XtfCurve.Ihcurv);  //����ʱ�����߾��
	SunI2TOPC(XtfCurve.Ictype);  //��������:1-����,2-����,3-����
	SunI2TOPC(XtfCurve.RepCode);  //��������:2-INT*2,4=REAL*4,��
	SunI2TOPC(XtfCurve.Ivtype);  //��ֱ��������
	SunI2TOPC(XtfCurve.Ihtype);  //ˮƽ��������
	SunI2TOPC(XtfCurve.Ndims);   //���ߵ�ά��
	MSunI2TOPC(7,XtfCurve.Idims);  //��iά�ĵ�Ԫ��
	SunI2TOPC(XtfCurve.I2init);  //INT*2�����ĳ�ʼֵ
	SunI2TOPC(XtfCurve.Intflfix);  //����/ʵ��/�̶��ͱ��
	SunI2TOPC(XtfCurve.Isign); //�з���/�޷��ű��
	SunI2TOPC(XtfCurve.Numbits); //ÿ���ݵ�Ԫ����λ��
	SunI2TOPC(XtfCurve.Lisrepcd); //��ȡ��ֵ����������ԭʼLIS������ʽ
	SunI2TOPC(XtfCurve.Lisnsamp);  //Nsamp���ͬʱLisrepcd�й�
	SunI2TOPC(XtfCurve.Lisrcsiz);  //LIS�����С���ֽڣ�
	SunI2TOPC(XtfCurve.Lisnsize);  //ÿ������LIS�����ܽڽ���
	SunI2TOPC(XtfCurve.Istartfv);  //0-�̶���������ʼʱ��,1-�ɱ��������ʼʱ��
	SunI2TOPC(XtfCurve.Igainfv);   //��Ϊ0����������������̶�
	SunI2TOPC(XtfCurve.Igainepx);  //���й̶���������л����ߵ�����ָ��
	SunI2TOPC(XtfCurve.Igainmth);  //������������л����ߵ������ʾ��-�̶���ɱ�
	SunI2TOPC(XtfCurve.Igainapp);  //����Ӧ�ÿ���
	SunI2TOPC(XtfCurve.Lntaphed);  //RAWTPHD�ĳ���
	SunI2TOPC(XtfCurve.Ntools);    //������
	SunI2TOPC(XtfCurve.Idxingrp);  //��������
	MSunI2TOPC(6,XtfCurve.Smvalsi2);  //6���μ���ʧ����ֵ������
	if(XtfCurve.RepCode==4 || XtfCurve.RepCode==2)
	{
		float *f4=(float*)&XtfCurve.DatumInitializationVariable;
		if(!_isnan(*f4))
		{
			XtfCurve.DatumInitializationVariable=int(*f4);		
		}
	}
}
void CXtfRead::ClearHeaderPack()
{
	CString s;
	int ki[512]={-1};
	int NumLog=0;
	for(int i=0;i<XtfHead.ismaxcv;i++)
	{
		s.Format("%-.8s",XtfHead.CurveName[i]);
		s.TrimLeft();
		s.TrimRight();
		if(!s.IsEmpty())
		{
			ki[NumLog]=i;
			NumLog++;
		}
		if(NumLog==XtfHead.isnumcv) break;
	}

	for(i=0;i<XtfHead.isnumcv;i++)
	{
		if(i == ki[i]) continue; //�˾䲻��ʡȥ���������������ԣ����ַ���������������
		sprintf(XtfHead.CurveName[i],"%-.8s",XtfHead.CurveName[ki[i]]); //Head3
		XtfHead.StartOffset[i]=XtfHead.StartOffset[ki[i]]; //Head4
		XtfHead.LevelNumber[i]=XtfHead.LevelNumber[ki[i]]; //Head4
		XtfHead.ndimension[i]=XtfHead.ndimension[ki[i]]; //Head5
		XtfHead.idimen1[i]=XtfHead.idimen1[ki[i]]; //Head5
		XtfHead.idimen2[i]=XtfHead.idimen2[ki[i]]; //Head5
		XtfHead.idimen3[i]=XtfHead.idimen3[ki[i]]; //Head5
		XtfHead.Sdep[i]=XtfHead.Sdep[ki[i]]; //Head6
		XtfHead.Edep[i]=XtfHead.Edep[ki[i]]; //Head6
		XtfHead.Level[i]=XtfHead.Level[ki[i]]; //Head7
		XtfHead.CurveAttrib[i]=XtfHead.CurveAttrib[ki[i]]; //Head7
	}
}
int CXtfRead::OpenCurve(CString sCurveName)
{
	index=GetCurveIndex(sCurveName);
	if(index<0) return false;
	return ReadCurveHead();
}

int CXtfRead::OpenCurve(int Index)
{
	if(Index<0) return false;
	index=Index;
	return ReadCurveHead();
}
int CXtfRead::ReadCurveHead()
{
	CString s;
	int pos=(XtfHead.StartOffset[index]-1)*4096;
	fXtf.Seek(pos,CFile::begin);
	fXtf.Read(&XtfCurve,sizeof(XtfCurve));
	if(XtfHead.System_Code==5)
	{
		SunCurveHead2PC();
	}
	return CheckCurveHeader();
}
int CXtfRead::CheckCurveHeader()
{
	CString s;
	if(!isalpha(XtfCurve.Name[0]))
	{	
		return -1;//���������ַ�������ĸ
	}
	s.Format("%-.8s",XtfCurve.Name);
	s.TrimRight();
	if(s.IsEmpty())
	{	
		return -1;//������Ϊ��
	}
	if(XtfCurve.NumberOfCurveHeaderrecords<=0)
	{
		return -2;  //����ͷ��¼��
	}
	if(XtfCurve.RecordNumberOfLastCurvedataRecord<0)
	{
		return -3;  //�������һ�����ݼ�¼��
	}
	if(XtfCurve.NumberOfLevelsForTheCurve<0)
	{
		return -4;  //���ߵĲ�����
	}
	if(XtfCurve.Ihcurv<0 || XtfCurve.Ihcurv>512)
	{
		return -5; //����������
	}
	if(XtfCurve.Ictype<1 || XtfCurve.Ictype>3)
	{
		return -6; //��������:1-����,2-����,3-����
	}
	if(XtfCurve.RepCode<0 || XtfCurve.RepCode>12)
	{
		return -7;  //��������:2-INT*2,4=REAL*4,��
	}
	if(XtfCurve.Ndims>7 || XtfCurve.Ndims<1)
	{
		return -8;   //���ߵ�ά��
	}
	if(XtfCurve.Numbits<1 || XtfCurve.Numbits>64)
	{
		return -9; //ÿ���ݵ�Ԫ����λ��
	}
	return 0;
}
int CXtfRead::GetCurveIndex(CString sCurveName)
{
	CString s;
	sCurveName.TrimLeft();
	sCurveName.TrimRight();
	index=-1;
	for(int i=0;i<XtfHead.ismaxcv;i++)
	{
		s.Format ("%-.8s",XtfHead.CurveName[i]);
		s.TrimLeft();
		s.TrimRight();
		if(s.CompareNoCase(sCurveName)==0)
		{
			index=i;
			break;
		}
	}
	return index;
}
DWORD CXtfRead::ReadCurve(int index, DWORD count, float* buffer)
{
	if(XtfCurve.Ictype>1) 
	{
		return 0; //�ǳ�������
	}
	DWORD pos=XtfHead.StartOffset[index]*4096;
	ReadData(pos,count,buffer);
	return count;
}
bool CXtfRead::ReadWave(int index,float depth,float *buffer)
{
	CString s;
	if(XtfCurve.Ictype != 2) 
	{
		return false; //�����ǲ�������
	}
	if(depth<XtfCurve.deptop)
	{
		return false; //��Ȳ���
	}
	DWORD np=DWORD((depth-XtfCurve.deptop)/XtfCurve.rlevcv+0.5);
	DWORD count=XtfCurve.Idims[0];
	int NumData=1;
	for(int i=0;i<XtfCurve.Ndims;i++)
	{
		NumData*=XtfCurve.Idims[i];
	}
	DWORD BlockSize=NumData*XtfCurve.Numbits/8;
	DWORD pos=XtfHead.StartOffset[index]*4096+np*BlockSize;
	ReadData(pos,count,buffer);
	return true;
}
bool CXtfRead::ReadWaveFromMatrix(int index,int subindex,float depth,float *buffer)
{
	CString s;
	if(XtfCurve.Ictype != 3) 
	{
		return false; //�����ǲ�������
	}
	if(depth<XtfCurve.deptop)
	{
		return false; //��Ȳ���
	}
	DWORD np=DWORD((depth-XtfCurve.deptop)/XtfCurve.rlevcv+0.5);
	DWORD count=XtfCurve.Idims[0];
	int NumData=1;
	for(int i=0;i<XtfCurve.Ndims;i++)
	{
		NumData*=XtfCurve.Idims[i];
	}
	DWORD BlockSize=NumData*XtfCurve.Numbits/8;
	DWORD SubBlockSize=count*XtfCurve.Numbits/8;
	DWORD pos=XtfHead.StartOffset[index]*4096+np*BlockSize+subindex*SubBlockSize;
	ReadData(pos,count,buffer);
	return true;
}
bool CXtfRead::ReadMatrix(int index,float depth,float *buffer)
{
	CString s;
	if(XtfCurve.Ictype <2) 
	{
		return false; //�����ǲ��λ���������
	}
	if(depth<XtfCurve.deptop)
	{
		return false; //��Ȳ���
	}
	DWORD np=DWORD((depth-XtfCurve.deptop)/XtfCurve.rlevcv+0.5);
	int count=1;
	for(int i=0;i<XtfCurve.Ndims;i++)
	{
		count*=XtfCurve.Idims[i];
	}
	DWORD BlockSize=count*XtfCurve.Numbits/8;
	DWORD pos=XtfHead.StartOffset[index]*4096+np*BlockSize;
	ReadData(pos,count,buffer);
	return true;
}
void CXtfRead::ReadData(DWORD pos,DWORD count,float *buffer)
{
	short *hBuf=NULL;
	int *iBuf=NULL;
	double *dfBuf=NULL;
	char *cBuf=NULL;
	unsigned char *ucBuf=NULL;
	unsigned short *uhBuf=NULL;
	unsigned int *uiBuf=NULL;
	DWORD i,NumBytes;

	fXtf.Seek(pos,CFile::begin);
	switch(XtfCurve.RepCode)
	{
	case XTF_REPR_BIT: //λֵ
		NumBytes=(count-1)/8+1;
		cBuf=new char[NumBytes];
		fXtf.Read(cBuf,NumBytes*sizeof(char));
		for(i=0;i<count;i++)
		{
			int k1=i/8;
			int k2=i%8;
			if( (cBuf[k1]<<k2) & 0x80 )
			{
				buffer[i]=1;
			}
			else
			{
				buffer[i]=0;
			}
		}
		delete []cBuf; cBuf=NULL;
		break;
	case XTF_REPR_INT2: //�з��Ŷ�������
		hBuf=new short[count];
		fXtf.Read(hBuf,count*sizeof(short));
		if(XtfHead.System_Code==5)
		{
			MSunI2TOPC(count,hBuf);
		}
		for(i=0;i<count;i++) buffer[i]=float(hBuf[i]);
		delete []hBuf; hBuf=NULL;
		break;
	case XTF_REPR_UCHAR: //�޷����ֽ���
		ucBuf=new unsigned char[count];
		fXtf.Read(ucBuf,count*sizeof(unsigned char));
		for(i=0;i<count;i++) 
		{
			buffer[i]=float(ucBuf[i]);
		}
		delete []ucBuf; ucBuf=NULL;
		break;
	case XTF_REPR_FLOAT: //IEEE�����ȸ�����	
		fXtf.Read(buffer,count*sizeof(float));
		if(XtfHead.System_Code==5)
		{
			MSunR4TOPC(count,buffer);
		}
		break;
	case XTF_REPR_CHAR: //�з����ֽ���
		cBuf=new char[count];
		fXtf.Read(cBuf,count*sizeof(char));
		for(i=0;i<count;i++) 
		{
			buffer[i]=float(cBuf[i]);
		}
		delete []cBuf; cBuf=NULL;
		break;
	case XTF_REPR_DOUBLE: //IEE˫���ȸ�����
		dfBuf=new double[count];
		fXtf.Read(dfBuf,count*sizeof(double));
		if(XtfHead.System_Code==5)
		{
			MSunR8TOPC(count,dfBuf);
		}
		for(i=0;i<count;i++) buffer[i]=float(dfBuf[i]);
		delete []dfBuf; dfBuf=NULL;
		break;
	case XTF_REPR_UINT2: //�޷��Ŷ�������
		uhBuf=new unsigned short[count];
		fXtf.Read(uhBuf,count*sizeof(unsigned short));
		if(XtfHead.System_Code==5)
		{
			MSunI2TOPC(count,uhBuf);
		}
		for(i=0;i<count;i++) buffer[i]=float(uhBuf[i]);
		delete []uhBuf; uhBuf=NULL;
		break;
	case XTF_REPR_INT4: //�з��ų�������
		iBuf=new int[count];
		fXtf.Read(iBuf,count*sizeof(int));
		if(XtfHead.System_Code==5)
		{
			MSunI4TOPC(count,iBuf);
		}
		for(i=0;i<count;i++) buffer[i]=float(iBuf[i]);
		delete []iBuf; iBuf=NULL;
		break;
	case XTF_REPR_UINT4: //�޷��ų�������
		uiBuf=new unsigned int[count];
		fXtf.Read(uiBuf,count*sizeof(unsigned int));
		if(XtfHead.System_Code==5)
		{
			MSunI4TOPC(count,uiBuf);
		}
		for(i=0;i<count;i++) buffer[i]=float(uiBuf[i]);
		delete []uiBuf; uiBuf=NULL;
		break;
	}	
}


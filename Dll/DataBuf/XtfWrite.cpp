// XtfWrite.cpp: implementation of the CXtfWrite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XtfWrite.h"
#include "XtfRead.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXtfWrite::CXtfWrite()
{

}

CXtfWrite::~CXtfWrite()
{

}
bool CXtfWrite::Open(CString sDataFile)
{
	sXtfFile=sDataFile;
	if(fXtf.Open(sDataFile,CFile::modeCreate|CFile::modeReadWrite)==FALSE)
	{
		return false;
	}
	return true;
}
void CXtfWrite::Close()
{
	fXtf.Close();
}
void CXtfWrite::WriteXtfHeader()
{
	XTF_HEAD TmpHead;
	TmpHead=XtfHead; //����ͷ��Ϣ
	PcXtfHead2Sun(); //����ת��
	WriteHeader(); //ͷ��Ϣд��
	XtfHead=TmpHead; //�ָ�ͷ��Ϣ
	fXtf.SetLength(XtfHead.FileRecord*4096);
}
void CXtfWrite::InitXtfHeader()
{
	for(int i=0;i<8;i++) XtfHead.header_location[i]=i+1;//�ļ�ͷ��¼��λ��
	for(i=0;i<182;i++)XtfHead.iunused[i]=0; //δ��
	strcpy(XtfHead.extension,".xtf");  //��չ����һ��Ϊ��.XTF��
	if(sXtfFile.GetLength()>80)
	{	//�ļ���̫������ȡ����·�����ļ���
		sprintf(XtfHead.FileFullName,"%-80.80s",MyGetFileName(sXtfFile)); //�ļ�ȫ������·������
	}
	else
	{
		sprintf(XtfHead.FileFullName,"%-80.80s",sXtfFile); //�ļ�ȫ������·������
	}
	strcpy(XtfHead.Depth_Unit,"meters  "); //��ȵ�λ
	for(i=0;i<8;i++)XtfHead.AIF_Revision_numbar[i]=32; //AIF��ð汾��
	for(i=0;i<88;i++)XtfHead.strunused[i]=0;//δ��
	XtfHead.System_Code=5;  //ϵͳ���룬1=PC��2=PE��3=VAX��4=IBM��5=��HP9000/SUN
	XtfHead.QC=0; //����ָʾĿ��ϵͳ�ڽ������и�ʽת��
	XtfHead.Major_Revision_Number=4; //SURVLIB ���汾��
	XtfHead.Minor_Revision_Number=14; //SURVLIB ���汾��
	for(i=0;i<22;i++)XtfHead.hunused[i]=0; //δ��
	XtfHead.isnumcv=0;   //��������
	XtfHead.ismaxcv=512; //���������,һ��Ϊ512
	for(i=0;i<5;i++) XtfHead.iunused2[i]=0;//δ��
	for(i=0;i<64;i++)//���ݼ�¼���ͼ�����ֹ��¼
	{
		for(int j=0;j<8;j++)XtfHead.UserData[i].Name[j]=0;
		XtfHead.UserData[i].StartRecord=0;
		XtfHead.UserData[i].StopRecord=0;
	}
	XtfHead.wttop=0.0f;     //ȱʡ��ʼ���
	XtfHead.wtbottom=0.0f;  //ȱʡ�������
	XtfHead.wtlevel=0.0;    //ȱʡ�������
	for(i=0;i<1012;i++) XtfHead.strunused2[i]=0;//δ��
	for(i=0;i<1020;i++) XtfHead.strunused3[i]=0;//δ��
	XtfHead.FileRecord=8; //�ļ���С,�Կ�(4096�ֽ�Ϊ1��)����ʾ
	//�ڶ���ͷ��¼
	XtfHead.Datamap[0]=0xFF;  //��һλͼ����ӳ�ļ�ͷ�ļ�¼
	for(i=1;i<4072;i++) XtfHead.Datamap[i]=0X00; //λͼ��ÿһλ��ʾ4096�ֽ�
	XtfHead.bitmapfullflag=0; //λͼ���ر��
	XtfHead.firstrecord=0x1;     //λͼ�е������¼
	XtfHead.lastrecord=0x7f40;      //λͼ�е����һ����¼
	XtfHead.prerecord=0;       //ǰһ��λͼ�ļ�¼��
	XtfHead.thisrecord=0x2;      //��ǰλͼ��¼��
	XtfHead.nextrecord=0;      //��һ��λͼ�ļ�¼��
	//������ͷ��¼
	for(i=0;i<512;i++)
	{
		sprintf(XtfHead.CurveName[i],"%-8.8s",""); //������
	}
	//���ļ�¼
	for(i=0;i<512;i++)
	{
		XtfHead.StartOffset[i]=0; //ÿ�����ߵ���ʼ��¼�ţ���1��ʼ����
		XtfHead.LevelNumber[i]=0; //ÿ�����ߵĲ�����
	}
	//�����¼
	for(i=0;i<512;i++)
	{
		XtfHead.ndimension[i]=0; //���ߵ�ά����ȡֵΪ1-3
		XtfHead.idimen1[i]=0;    //���ߵ�һά�ĵ�Ԫ��
		XtfHead.idimen2[i]=0;    //���ߵڶ�ά�ĵ�Ԫ��
		XtfHead.idimen3[i]=0;    //���ߵ���ά�ĵ�Ԫ��
	}
	//������¼
	for(i=0;i<512;i++)
	{
		XtfHead.Sdep[i]=0.0f; //���ߵ���ʼ���
		XtfHead.Edep[i]=0.0f;   //���ߵĽ������
	}
	//���߼�¼
	for(i=0;i<512;i++)
	{
		XtfHead.Level[i]=0.0f;    //���߲������
		XtfHead.CurveAttrib[i].CurType=0; //��������
		XtfHead.CurveAttrib[i].RepCode=0; //��������
		XtfHead.CurveAttrib[i].HorType=0; //ˮƽ����
		XtfHead.CurveAttrib[i].VerType=0; //��ֱ����
	}
	//�ڰ˸���¼
	//��һ�飺ASCII
	sprintf(XtfHead.Wellsiteinfo,"%-8.8s","WSI"); //��WSI����ǣ���ʾһ����Ч�ľ�����Ϣ��¼
	sprintf(XtfHead.Wellname,"%-80.80s",""); //����
	sprintf(XtfHead.Fieldname,"%-80.80s","SiChuan Petroleum Management Office"); //��������
	sprintf(XtfHead.Companyname,"%-80.80s","SiChuan Petroleum Logging Corporation,CHONG QING,400021"); //��˾��
	sprintf(XtfHead.Countryname,"%-32.32s","CHINA"); //������
	sprintf(XtfHead.Countyname,"%-32.32s",""); //����
	sprintf(XtfHead.Statename,"%-16.16s",""); //����
	sprintf(XtfHead.OCTname,"%-8.8s",""); //�������Ʊ���
	sprintf(XtfHead.Welllocation,"%-40.40s",""); //����λ��
	sprintf(XtfHead.Section,"%-4.4s",""); //������
	sprintf(XtfHead.Township,"%-4.4s",""); //������
	sprintf(XtfHead.Range,"%-4.4s",""); //��Χ
	sprintf(XtfHead.Permanentdatum,"%-2.2s","GL"); //��������,��gl����sl��
	sprintf(XtfHead.Drillingmeasurementdatum,"%-2.2s","KB"); //�꾮��������(��kb������gl����)
	sprintf(XtfHead.LogAnalysisCenter,"%-4.4s","SLC"); //�⾮�������Ĵ���
	sprintf(XtfHead.Wellunits,"%-8.8s","meters"); //����������λ
	for(i=0;i<620;i++)XtfHead.strunused4[i]=' '; //δ��
	//�ڶ��飺����
	XtfHead.Latitude=-9999;   //����
	XtfHead.Longitude=-9999;  //γ��
	XtfHead.Kellybushing=0;  //���겹�ĺ����߶�
	XtfHead.Derrickfloor=0;  //����ƽ̨�����߶�
	XtfHead.Elevation=0;    //������������
	XtfHead.MagneticDeclination=-9999;  //��ƫ��
	XtfHead.ECC=0.0f;   //ECC��
	XtfHead.Groundtosealevelelevation=0; //���溣���߶�
	for(i=0;i<120;i++)XtfHead.funused[i]=0; //δ��
	for(i=0;i<64;i++)XtfHead.dunused[i]=0; //δ��
	//�����飺����
	for(i=0;i<6;i++)XtfHead.hunused2[i]=0; //δ��
	XtfHead.Metricflag=0;  //���Ʊ��
	XtfHead.Maximumcurveversion=0; //�����������(1-99)
	for(i=0;i<504;i++)XtfHead.hunused3[i]=0; //δ��
	//���Ŀ飺������
	for(i=0;i<1024;i++)XtfHead.unformattedbinaryinformation[i]=0; //δ��ʽ����������Ϣ
}
void CXtfWrite::PcXtfHead2Sun()
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

void CXtfWrite::WriteHeader()
{
	//��һ��¼
	fXtf.SeekToBegin();
	fXtf.Write(XtfHead.header_location,32);
	fXtf.Write(XtfHead.iunused,182*4);
	fXtf.Write(XtfHead.extension,4);
	fXtf.Write(XtfHead.FileFullName,80);
	fXtf.Write(XtfHead.Depth_Unit,8);
	fXtf.Write(XtfHead.AIF_Revision_numbar,8);
	fXtf.Write(XtfHead.strunused,88);
	fXtf.Write(&XtfHead.System_Code,1);
	fXtf.Write(&XtfHead.QC,1);
	fXtf.Write(&XtfHead.Major_Revision_Number,1);
	fXtf.Write(&XtfHead.Minor_Revision_Number,1);
	fXtf.Write(XtfHead.hunused,44);
	fXtf.Write(&XtfHead.isnumcv,4);
	fXtf.Write(&XtfHead.ismaxcv,4);
	fXtf.Write(XtfHead.iunused2,20);
	fXtf.Write(XtfHead.UserData,64*16);
	fXtf.Write(&XtfHead.wttop,4);
	fXtf.Write(&XtfHead.wtbottom,4);
	fXtf.Write(&XtfHead.wtlevel,4);
	fXtf.Write(XtfHead.strunused2,1012);
	fXtf.Write(XtfHead.strunused3,1020);
	fXtf.Write(&XtfHead.FileRecord,4);

	//�ڶ���¼
	fXtf.Write(XtfHead.Datamap,4072);
	fXtf.Write(&XtfHead.bitmapfullflag,4);
	fXtf.Write(&XtfHead.firstrecord,4);
	fXtf.Write(&XtfHead.lastrecord,4);
	fXtf.Write(&XtfHead.prerecord,4);
	fXtf.Write(&XtfHead.thisrecord,4);
	fXtf.Write(&XtfHead.nextrecord,4);

	//������¼
	fXtf.Write(XtfHead.CurveName,4096);
	
	//���ļ�¼
	for(int i=0;i<256;i++)
	{
		fXtf.Write(&XtfHead.StartOffset[i],sizeof(int));
		fXtf.Write(&XtfHead.StartOffset[256+i],sizeof(int));
	}
	for(i=0;i<256;i++)
	{
		fXtf.Write(&XtfHead.LevelNumber[i],sizeof(int));
		fXtf.Write(&XtfHead.LevelNumber[256+i],sizeof(int));
	}

	//�����¼
	for(i=0;i<256;i++)
	{
		fXtf.Write(&XtfHead.ndimension[i],sizeof(short));
		fXtf.Write(&XtfHead.idimen1[i],sizeof(short));
		fXtf.Write(&XtfHead.ndimension[256+i],sizeof(short));
		fXtf.Write(&XtfHead.idimen1[256+i],sizeof(short));
	}
	for(i=0;i<256;i++)
	{
		fXtf.Write(&XtfHead.idimen2[i],sizeof(short));
		fXtf.Write(&XtfHead.idimen3[i],sizeof(short));
		fXtf.Write(&XtfHead.idimen2[256+i],sizeof(short));
		fXtf.Write(&XtfHead.idimen3[256+i],sizeof(short));
	}

	//������¼
	for(i=0;i<256;i++)
	{
		fXtf.Write(&XtfHead.Sdep[i],sizeof(float));
		fXtf.Write(&XtfHead.Sdep[i+256],sizeof(float));
	}
	for(i=0;i<256;i++)
	{
		fXtf.Write(&XtfHead.Edep[i],sizeof(float));
		fXtf.Write(&XtfHead.Edep[i+256],sizeof(float));
	}

	//���߼�¼
	for(i=0;i<256;i++)
	{
		fXtf.Write(&XtfHead.Level[i],sizeof(float));
		fXtf.Write(&XtfHead.Level[256+i],sizeof(float));
	}
	for(i=0;i<256;i++)
	{
		fXtf.Write(&XtfHead.CurveAttrib[i],sizeof(XtfHead.CurveAttrib[i]));
		fXtf.Write(&XtfHead.CurveAttrib[256+i],sizeof(XtfHead.CurveAttrib[256+i]));
	}

	//�ڰ˼�¼
	fXtf.Write(XtfHead.Wellsiteinfo,8); //��WSI����ǣ���ʾһ����Ч�ľ�����Ϣ��¼
	fXtf.Write(XtfHead.Wellname,80);     //����
	fXtf.Write(XtfHead.Fieldname,80);    //��������
	fXtf.Write(XtfHead.Companyname,80);  //��˾��
	fXtf.Write(XtfHead.Countryname,32);  //������
	fXtf.Write(XtfHead.Countyname,32);   //����
	fXtf.Write(XtfHead.Statename,16);    //����
	fXtf.Write(XtfHead.OCTname,8);       //�������Ʊ���
	fXtf.Write(XtfHead.Welllocation,40); //����λ��
	fXtf.Write(XtfHead.Section,4);       //������
	fXtf.Write(XtfHead.Township,4);      //������
	fXtf.Write(XtfHead.Range,4);         //��Χ
	fXtf.Write(XtfHead.Permanentdatum,2); //��������,��gl����sl��
	fXtf.Write(XtfHead.Drillingmeasurementdatum,2);  //�꾮��������(��kb������gl����)
	fXtf.Write(XtfHead.LogAnalysisCenter,4);  //�⾮�������Ĵ���
	fXtf.Write(XtfHead.Wellunits,8);          //����������λ
	fXtf.Write(XtfHead.strunused4,620);    //δ��

	fXtf.Write(&XtfHead.Latitude,4);   //����
	fXtf.Write(&XtfHead.Longitude,4);  //γ��
	fXtf.Write(&XtfHead.Kellybushing,4);  //���겹�ĺ����߶�
	fXtf.Write(&XtfHead.Derrickfloor,4);  //����ƽ̨�����߶�
	fXtf.Write(&XtfHead.Elevation,4);    //������������
	fXtf.Write(&XtfHead.MagneticDeclination,4);  //��ƫ��
	fXtf.Write(&XtfHead.ECC,4);   //ECC��
	fXtf.Write(&XtfHead.Groundtosealevelelevation,4); //���溣���߶�
	fXtf.Write(XtfHead.funused,120*sizeof(float));   //δ�õ�������
	fXtf.Write(XtfHead.dunused,64*sizeof(double));   //δ��˫������

	fXtf.Write(XtfHead.hunused2,6*sizeof(short));  //δ��
	fXtf.Write(&XtfHead.Metricflag,sizeof(short));  //���Ʊ��
	fXtf.Write(&XtfHead.Maximumcurveversion,sizeof(short)); //�����������(1-99)
	fXtf.Write(XtfHead.hunused3,504*sizeof(short));  //δ��
	fXtf.Write(XtfHead.unformattedbinaryinformation,1024); //δ��ʽ����������Ϣ
}

void CXtfWrite::WriteCurveHeader(int index)
{
	Index=index;
	CURVE_HEAD TmpCurveHead;
	TmpCurveHead=XtfCurve; //����ͷ��Ϣ
	DWORD pos=(XtfHead.StartOffset[index]-1)*4096;
	PcCurveHead2Sun(); //����ת��
	fXtf.Seek(pos,CFile::begin);
	fXtf.Write(&XtfCurve,sizeof(XtfCurve));
	XtfCurve=TmpCurveHead; //�ָ�ͷ��Ϣ
}
void CXtfWrite::PcCurveHead2Sun()
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
}
void CXtfWrite::WriteDataToXtf(DWORD pos,DWORD count,float *buffer)
{
	CString s;
	DWORD i;
	short *hBuf=0;
	unsigned char *ucBuf=0;
	char *cBuf=0;
	double *dfBuf=0;
	unsigned short *uhBuf=0;
	int *iBuf=0;
	unsigned int *uiBuf=0;
	fXtf.Seek(pos,CFile::begin);
	switch(XtfCurve.RepCode)
	{
	case XTF_REPR_INT2://�з��Ŷ�������
		hBuf=new short[count];
		for(i=0;i<count;i++)
		{
			hBuf[i]=short(buffer[i]);
		}
		MSunI2TOPC(count,hBuf);
		fXtf.Write(hBuf,count*sizeof(short));
		delete []hBuf; hBuf=0;
		break;
	case XTF_REPR_UCHAR://�޷����ֽ�
		ucBuf=new unsigned char[count];
		for(i=0;i<count;i++)
		{
			ucBuf[i]=unsigned char(buffer[i]);
		}
		fXtf.Write(ucBuf,count);
		delete []ucBuf; ucBuf=0;
		break;
	case XTF_REPR_FLOAT://IEEE�����ȸ�����
		MSunR4TOPC(count,buffer);
		fXtf.Write(buffer,count*sizeof(float));
		break;
	case XTF_REPR_CHAR://�з����ֽ���
		cBuf=new char[count];
		for(i=0;i<count;i++)
		{
			cBuf[i]=char(buffer[i]);
		}
		fXtf.Write(cBuf,count);
		delete []cBuf; cBuf=0;
		break;
	case XTF_REPR_DOUBLE://˫���ȸ���
		dfBuf=new double[count];
		for(i=0;i<count;i++)
		{
			dfBuf[i]=double(buffer[i]);
		}
		MSunR8TOPC(count,dfBuf);
		fXtf.Write(dfBuf,count*sizeof(double));
		delete []dfBuf; dfBuf=0;
		break;
	case XTF_REPR_UINT2://�޷��Ŷ�������
		uhBuf=new unsigned short[count];
		for(i=0;i<count;i++)
		{
			uhBuf[i]=unsigned short(buffer[i]);
		}
		MSunI2TOPC(count,uhBuf);
		fXtf.Write(uhBuf,count*sizeof(unsigned short));
		delete []uhBuf; uhBuf=0;
		break;
	case XTF_REPR_INT4://������
		iBuf=new int[count];
		for(i=0;i<count;i++)
		{
			iBuf[i]=int(buffer[i]);
		}
		MSunI4TOPC(count,iBuf);
		fXtf.Write(iBuf,count*sizeof(int));
		delete []iBuf; iBuf=0;
		break;
	case XTF_REPR_UINT4://�޷��ų�������
		uiBuf=new unsigned int[count];
		for(i=0;i<count;i++)
		{
			uiBuf[i]=unsigned int(buffer[i]);
		}
		MSunI4TOPC(count,uiBuf);
		fXtf.Write(uiBuf,count*sizeof(unsigned int));
		delete []uiBuf; uiBuf=0;
		break;
	default:
		break;
	}
}

void CXtfWrite::WriteGenData(int count,float *buf)
{
	DWORD pos=XtfHead.StartOffset[Index]*4096;
	WriteDataToXtf(pos,count,buf);
}
void CXtfWrite::WriteWave(float depth,float *buffer)
{
	CString s;
	if(XtfCurve.Ictype != 2) 
	{
		return; //�����ǲ�������
	}
	if(depth<XtfCurve.deptop || depth>XtfCurve.depbot)
	{
		return; //��Ȳ���
	}
	DWORD np=DWORD((depth-XtfCurve.deptop)/XtfCurve.rlevcv+0.5);
	DWORD count=XtfCurve.Idims[0];
	DWORD BlockSize=count*XtfRepCodeToLen(XtfCurve.RepCode)/8;
	DWORD pos=XtfHead.StartOffset[Index]*4096+np*BlockSize;
	WriteDataToXtf(pos,count,buffer);
}
void CXtfWrite::WriteWaveArray(float depth,float *buffer)
{
	CString s;
	if(XtfCurve.Ictype < 2) 
	{
		return; //�����ǲ��λ���������
	}
	if(depth<XtfCurve.deptop || depth>XtfCurve.depbot)
	{
		return; //��Ȳ���
	}
	DWORD np=DWORD((depth-XtfCurve.deptop)/XtfCurve.rlevcv+0.5);
	int count=1;
	for(int i=0;i<XtfCurve.Ndims;i++)
	{
		count*=XtfCurve.Idims[i];
	}
	DWORD BlockSize=count*XtfRepCodeToLen(XtfCurve.RepCode)/8;
	DWORD pos=XtfHead.StartOffset[Index]*4096+np*BlockSize;
	WriteDataToXtf(pos,count,buffer);
}

short CXtfWrite::XtfRepCodeToLen(short RepCode)
{
	short XtfCodeLen=8;
	switch(RepCode)
	{
	case XTF_REPR_BIT:
		XtfCodeLen=1;
		break;
	case XTF_REPR_UCHAR:
	case XTF_REPR_CHAR:
		XtfCodeLen=8;
		break;
	case XTF_REPR_INT2:
	case XTF_REPR_UINT2:
		XtfCodeLen=16;
		break;
	case XTF_REPR_FLOAT:
	case XTF_REPR_INT4:
	case XTF_REPR_UINT4:
		XtfCodeLen=32;
		break;
	case XTF_REPR_DOUBLE:
		XtfCodeLen=64;
		break;
	default:
		XtfCodeLen=8;
		break;
	}
	return XtfCodeLen;
}
short CXtfWrite::XtfRepCodeToIntFlFix(short XtfRepCode)
{
	short nIntFlFix=1;
	switch(XtfRepCode)
	{
	case XTF_REPR_INT2:
	case XTF_REPR_UCHAR:
	case XTF_REPR_INT4:
	case XTF_REPR_UINT4:
	case XTF_REPR_UINT2:
		nIntFlFix=1;
		break;
	case XTF_REPR_FLOAT:
		nIntFlFix=2;
		break;
	case XTF_REPR_DOUBLE:
		nIntFlFix=4;
		break;
	case XTF_REPR_CHAR:
		nIntFlFix=5;
		break;
	default:
		nIntFlFix=3;
	}
	return nIntFlFix;
}
short CXtfWrite::XtfRepCodeToSign(short XtfRepCode)
{
	if((XtfRepCode==XTF_REPR_BIT)
		|| (XtfRepCode==XTF_REPR_UCHAR)
		|| (XtfRepCode==XTF_REPR_UINT2)
		|| (XtfRepCode==XTF_REPR_UINT4))
	{
		return 2; //�޷��ű��
	}
	else
	{
		return 1; //�з���
	}
}
void CXtfWrite::PrintHead(FILE *fp)
{
	CString s;
	fprintf(fp,"\n------- XTF FILE HEADER ---------\n");
	fprintf(fp,"%+25s: %d\n","System Code",XtfHead.System_Code);
	fprintf(fp,"%+25s: %-.80s\n","Fully expanded filename",XtfHead.FileFullName);
	fprintf(fp,"%+25s: %-.8s\n","Depth Units",XtfHead.Depth_Unit);
	fprintf(fp,"%+25s: %d\n","Number of Curves",XtfHead.isnumcv);
	fprintf(fp,"%+25s: %d\n","Max    of Curves",XtfHead.ismaxcv);
	fprintf(fp,"%+25s: %G\n","Default Start Depth",XtfHead.wttop);
	fprintf(fp,"%+25s: %G\n","Default End Depth",XtfHead.wtbottom);
	fprintf(fp,"%+25s: %G\n","Default Level Spacing",XtfHead.wtlevel);
	fprintf(fp,"%+25s: %d (%d Bytes)\n","Number of Record",XtfHead.FileRecord,XtfHead.FileRecord*4096);

	fprintf(fp,"\n------- XTF SELF-DEFINED DATATYPE ---------\n");
	fprintf(fp," %15s %15s %15s\n","DataType Name","Start Record","End Record");
	for(int i=0;i<64;i++)
	{
		s.Format ("%-.8s",XtfHead.UserData[i].Name);
		s.TrimLeft();
		s.TrimRight();
		if(s.IsEmpty()) continue;
		fprintf(fp," %15s %15d %15d\n",(const char*)s,XtfHead.UserData[i].StartRecord,XtfHead.UserData[i].StopRecord);
	}
	//������Ϣ
	fprintf(fp,"\n----------------------  XTF Curve Information  -------------------\n");
	fprintf(fp,"%-8s %-4s %-8s %8s %8s %8s %5s %5s %4s %4s %4s %4s %4s %4s %4s\n",
		"Name","Type","Units","Start","Stop","Rlev","Num","Offs",
		"#Dim","#El1","#El2","#El3",
		"RepC","Hor","Ver");
	CString sCurveName;
	int NumCurveHeadErr=0; //����ͷ�д��������
	for(i=0;i<XtfHead.isnumcv;i++)
	{
		fprintf(fp,"%-8.8s",XtfHead.CurveName[i]);
		if(XtfHead.CurveAttrib[i].CurType==1)
		{
			fprintf(fp," %-4s","C");
		}
		else if(XtfHead.CurveAttrib[i].CurType==2)
		{
			fprintf(fp," %-4s","WF");
		}
		else
		{
			fprintf(fp," %-4s","MATR");
		}
		fprintf(fp," %-.8s",XtfHead.Units[i]);
		fprintf(fp," %8G",XtfHead.Sdep[i]);
		fprintf(fp," %8G",XtfHead.Edep[i]);
		fprintf(fp," %8G",XtfHead.Level[i]);
		fprintf(fp," %5u",XtfHead.LevelNumber[i]);
		fprintf(fp," %5u",XtfHead.StartOffset[i]);
		fprintf(fp," %4u",XtfHead.ndimension[i]);
		fprintf(fp," %4u",XtfHead.idimen1[i]);
		fprintf(fp," %4u",XtfHead.idimen2[i]);
		fprintf(fp," %4u",XtfHead.idimen3[i]);
		fprintf(fp," %4u",XtfHead.CurveAttrib[i].RepCode);
		fprintf(fp," %4u",XtfHead.CurveAttrib[i].HorType);
		fprintf(fp," %4u",XtfHead.CurveAttrib[i].VerType);
		fprintf(fp,"\n");

		////////////////////////////////////////////////////////
		//PrtXtfCurveHead(XtfHead.CurveName[i]);
	}
	if(NumCurveHeadErr>0)
	{
		fprintf(fp,"\nNumber of Curve Header Error: %d\n",NumCurveHeadErr);
	}
	fprintf(fp,"\n");
	fprintf(fp,"---- End Head information ---\n\n");

}

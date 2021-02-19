// BitR.cpp: implementation of the CBitR class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BitR.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBitR::CBitR()
{

}

CBitR::~CBitR()
{

}
bool _declspec(dllexport) IsBIT(CString sDataFile)
{
	CString s;
	s=MyGetFileExt(sDataFile);
	if(s.CompareNoCase("bit")!=0)
	{
		return false; //BIT�ļ�������չ��������BIT
	}
	CFile f;
	BIT_HEAD bit;
	if(f.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE) return false;
	f.Read(&bit,sizeof(bit));
	f.Close();
	SunI2TOPC(bit.NumLog);
	GetBitR4(bit.Stdep);
	GetBitR4(bit.Endep);
	if(fabs(bit.Stdep-bit.Endep)<0.01)
	{
		return false;
	}
	if(bit.NumLog<=0 || bit.NumLog>20)
	{
		return false; //BIT�ļ���������Ϊ1-20֮��
	}
	for(int i=0;i<bit.NumLog;i++)
	{
		if(!isalpha(bit.CurveName[i][0])) return false; //���������ַ���������ĸ
	}
	return true;
}
bool CBitR::Open(CString sDataFile)
{
	if(fBit.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE)
	{
		return false;
	}
	FileSize=fBit.GetLength(); //�ļ�����
	return true;
}
bool CBitR::ReadHead()
{
	fBit.SeekToBegin();
	fBit.Read(&BitHead,sizeof(BitHead));
	SunI2TOPC(BitHead.NumSample);
	SunI2TOPC(BitHead.nMonth);
	SunI2TOPC(BitHead.nDay);
	SunI2TOPC(BitHead.nYear);
	SunI2TOPC(BitHead.nHour);
	SunI2TOPC(BitHead.nMinute);
	SunI2TOPC(BitHead.nSecond);

	SunI2TOPC(BitHead.NumLog);
	SunI2TOPC(BitHead.UnitFlag);
	GetBitR4(BitHead.Stdep);
	GetBitR4(BitHead.Endep);
	GetBitR4(BitHead.Rlev);
	SunI2TOPC(BitHead.Idc);
	SunI2TOPC(BitHead.DataFlag);
	GetBitR4(BitHead.Spcpr);

	if(fabs(BitHead.Rlev)<0.0001)
	{
		//����BIT�ļ�����Щ�������Ϊ0�����Ա����ǿ��
		BitHead.Rlev=1;
	}
	//�����ļ���С��Spcpr��ֵ������ֹ���
	int WFSize=BitHead.NumWF*BitHead.NumSample;
	if(int(BitHead.DataFlag)>1)
	{
		bWaveform=true; //�Ƿ�Ϊ�������ݣ�TRUE��ʾ����
		NumBlock=(FileSize-sizeof(BitHead))/((4*BitHead.NumLog+WFSize)*int(BitHead.Spcpr));
		NumPoint=NumBlock;
		for(int i=0;i<BitHead.NumWF;i++)
		{
			char str[20];
			sprintf(str,"WF%d  ",i+1);
			memcpy(BitHead.CurveName[BitHead.NumLog+i],str,4);
		}
		BitHead.NumLog+=BitHead.NumWF; //���Ӳ���������
	}
	else
	{
		bWaveform=false; //�Ƿ�Ϊ�������ݣ�false��ʾ�޲���
		NumBlock=(FileSize-sizeof(BitHead))/(4*BitHead.NumLog*int(BitHead.Spcpr));
		NumPoint=NumBlock*int(BitHead.Spcpr);
	}
	//����ļ���û��ȷ�еĽ�����ȣ�������ļ��м�¼��Ϣ���м���
	if(fabs(BitHead.Endep)<0.01)
	{
		//�ļ��н������Ϊ0����ʾ�ϲ�
		BitHead.Endep=BitHead.Stdep-(NumPoint-1)*BitHead.Rlev;
	}
	else if(fabs(BitHead.Endep-99999.0)<0.01)
	{
		//�ļ��н������Ϊ99999����ʾ�²�
		BitHead.Endep=BitHead.Stdep+(NumPoint-1)*BitHead.Rlev;
	}
	//���ݲ�������ж��Ƿ�Ϊ���׵�λ������ת��Ϊ��
	if(fabs(BitHead.Rlev-1)<0.001)
	{
		BitHead.Stdep*=0.1f;
		BitHead.Endep*=0.1f;
		BitHead.Rlev*=0.1f;
	}
	//�Ƚ���ʼ�ͽ���������жϲ⾮����
	if(BitHead.Stdep>BitHead.Endep)
	{
		BitHead.Rlev=-BitHead.Rlev; //�������Ϊ����ʾ�ϲ�
	}

	//һ����Ĵ�С
	nBlockSize=4*BitHead.NumLog*int(BitHead.Spcpr);
	if(bWaveform)
	{
		//�ļ��д��ڲ�������
		nBlockSize=nBlockSize+WFSize-4*4; //����4������������ȥ�����Ǳ�����õĿռ�
	}
	if(bWaveform)
	{
		NumConvLog=BitHead.NumLog-4;//����������
		NumWaveLog=4; //����������
	}
	else
	{
		NumConvLog=BitHead.NumLog;//����������
		NumWaveLog=0; //����������
	}

	return true;
}

void CBitR::ClearHead()
{
	//��ͷ��Ϣ���й���
	for(int i=71;i>=0;i--)
	{
		if(BitHead.WellName[i]!=' ' && BitHead.WellName[i]!=0x0)break;
		BitHead.WellName[i]=0;
	}
	for(i=71;i>=0;i--)
	{
		if(BitHead.CompanyName[i]!=' ' && BitHead.CompanyName[i]!=0x0)break;
		BitHead.CompanyName[i]=0;
	}

	for(i=BitHead.NumLog;i<20;i++)
	{
		for(int j=0;j<4;j++)
		{
			BitHead.CurveName[i][j]=32;
		}
	}
}

bool CBitR::ReadChannel(int Index,float *buf)
{
	for(int i=0;i<NumBlock;i++)
	{
		int pos=sizeof(BitHead)+i*nBlockSize+4*Index*int(BitHead.Spcpr);
		fBit.Seek(pos,CFile::begin);
		fBit.Read(&buf[i*int(BitHead.Spcpr)],4*int(BitHead.Spcpr));
	}
	//�������
	for(i=0;i<NumPoint;i++)
	{
		GetBitR4(buf[i]);
	}
	//���Ϊ�ϲ⣬��������������
	if(BitHead.Stdep>BitHead.Endep)
	{
		for(int i=0;i<NumPoint/2;i++)
		{
			float fTemp=buf[i];
			buf[i]=buf[NumPoint-i-1];
			buf[NumPoint-i-1]=fTemp;
		}
	}
	return true;
}
bool CBitR::ReadWaveform(float Depth,char *buf)
{
	if(!bWaveform)
	{
		return false; //�ļ����޲�������
	}
	int nRec=int(fabs((Depth-BitHead.Stdep)/BitHead.Rlev)+0.5);
	if(nRec<0 || nRec>=NumBlock) return false;
	int pos=sizeof(BitHead)+nRec*nBlockSize+4*(BitHead.NumLog-4);
	fBit.Seek(pos,CFile::begin);
	fBit.Read(buf,BitHead.NumSample*BitHead.NumWF);
	return true;
}
bool CBitR::ReadWaveform(int nWave,float Depth,char *buf)
{
	CString s;
	if(!bWaveform)
	{
		AfxMessageBox("�ļ����޲�������");
		return false; //�ļ����޲�������
	}
	if(nWave<0 || nWave>=4)
	{
		s.Format ("�������ߵ�������=%d��������Χ(0-3)",nWave);
		AfxMessageBox(s);
		return false; //ֻ��4������
	}
	int nRec=int(fabs((Depth-BitHead.Stdep)/BitHead.Rlev)+0.5);
	if(nRec<0 || nRec>=NumBlock) return false;
	int pos=sizeof(BitHead)+nRec*nBlockSize+4*(BitHead.NumLog-4)+nWave*BitHead.NumSample;
	fBit.Seek(pos,CFile::begin);
	fBit.Read(buf,BitHead.NumSample);
	return true;
}

void CBitR::Close()
{
	fBit.Close();
}

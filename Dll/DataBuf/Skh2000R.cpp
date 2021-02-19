// Skh2000R.cpp: implementation of the CSkh2000R class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Skh2000R.h"
#include <math.h>
#include "CodeSP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkh2000R::CSkh2000R()
{

}

CSkh2000R::~CSkh2000R()
{

}
bool _declspec(dllexport) IsSKH2000(CString sDataFile)
{
	CString s;
	s=MyGetFileExt(sDataFile);
	if(s.CompareNoCase("DAT")!=0 && s.CompareNoCase("CBL")!=0)
	{
		return false; //SKH2000�ļ�������չ��������DAT��CBL
	}
	CFile f;
	SKH2000_HEAD Skh2000;
	if(f.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE) return false;
	f.Read(&Skh2000,sizeof(Skh2000));
	f.Close();
	if(fabs(Skh2000.Spcpr-128)>0.5)
	{
		return false;
	}
	if(Skh2000.NumLog<=0 || Skh2000.NumLog>20)
	{
		return false; //������Ϊ1-20
	}
	for(int i=0;i<Skh2000.NumLog;i++)
	{
		if(!isalpha(Skh2000.CurveName[i][0])) return false; //���������ַ���������ĸ
	}
	return true;
}

bool CSkh2000R::Open(CString sDataFile)
{
	CString s;
	nHeadSize=sizeof(Skh2000Head);//������С
	if(nHeadSize != 512)
	{
		s.Format ("SKH2000_HEAD �ṹ�Ĵ�С:%d�ֽ�\n\n���ݶ���ӦΪ512�ֽ�\n\n���޸Ľṹ����뻷������!",nHeadSize);
		AfxMessageBox(s);
		return false;
	}
	if(fSkh2000.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE)
	{
		return false;
	}
	FileSize=fSkh2000.GetLength(); //�ļ�����
	CString sExt=MyGetFileExt(sDataFile);
	if(sExt.CompareNoCase("CBL")==0)
	{
		bIsWAVE=TRUE; //����ļ���׺ΪCBL����Ϊ�ǲ�������
	}
	else
	{
		bIsWAVE=FALSE;
	}
	return true;
}
bool CSkh2000R::ReadHead()
{
	CString s;
	fSkh2000.SeekToBegin();
	fSkh2000.Read(&Skh2000Head,512);
	if(fabs(Skh2000Head.Endep)<0.001 || Skh2000Head.Endep<Skh2000Head.Stdep)
	{
		Skh2000Head.Rlev=-float(fabs(Skh2000Head.Rlev)); //�ϲ⣬��������Ϊ��
	}
	NumPoint=int(fabs((Skh2000Head.Endep-Skh2000Head.Stdep)/Skh2000Head.Rlev)+0.5)+1;
	if(bIsWAVE)
	{
		//��������:�����512�ֽڣ�������ÿ��¼2048�ֽڣ���β��¼
		nTrailSize=0; //CBL�Ĳ����ļ���β��¼
		nBlockSize=2048;
	}
	else
	{
		//��������:�����512�ֽڣ�������ÿ��¼���ȸ������߶�С������β��4096�ֽ�
		nTrailSize=4096; //�������ݴ����ļ�β��¼
		nBlockSize=int(Skh2000Head.Spcpr)*4*Skh2000Head.NumLog;
	}
	NumBlock=(FileSize-nHeadSize-nTrailSize)/nBlockSize;//���ݼ�¼�������������ͬ
	int NumData=NumBlock*128; //ʵ�ʴ��ڵ����ݵ���
	if(bIsWAVE)
	{
		NumData=NumBlock; //����ǲ������ݣ����ݵ��������ݼ�¼����ͬ
	}
	if(NumPoint>NumData)
	{
		NumPoint=NumData;
		Skh2000Head.Endep=Skh2000Head.Stdep+(NumPoint-1)*Skh2000Head.Rlev;
	}
	
	//���ڲ������ݣ��������������������¶��岨��������
	if(bIsWAVE)
	{
		sprintf(Skh2000Head.CurveName[0],"VDL3");
		sprintf(Skh2000Head.CurveName[1],"VDL5");
		Skh2000Head.NumLog=2;
	}
	return true;
}
void CSkh2000R::ClearHead()
{
	//��ͷ��Ϣ���й���
	for(int i=39;i>=0;i--)
	{
		if(Skh2000Head.WellName[i]!=' ' && Skh2000Head.WellName[i]!=0x0)break;
		Skh2000Head.WellName[i]=0;
	}
	for(i=39;i>=0;i--)
	{
		if(Skh2000Head.CompanyName[i]!=' ' && Skh2000Head.CompanyName[i]!=0x0)break;
		Skh2000Head.CompanyName[i]=0;
	}
	for(i=19;i>=0;i--)
	{
		if(Skh2000Head.TeamName[i]!=' ' && Skh2000Head.TeamName[i]!=0x0)break;
		Skh2000Head.TeamName[i]=0;
	}
	for(i=19;i>=0;i--)
	{
		if(Skh2000Head.WellSite[i]!=' ' && Skh2000Head.WellSite[i]!=0x0)break;
		Skh2000Head.WellSite[i]=0;
	}

	for(i=Skh2000Head.NumLog;i<20;i++)
	{
		for(int j=0;j<4;j++)
		{
			Skh2000Head.CurveName[i][j]=32;
		}
	}
}

bool CSkh2000R::ReadChannel(int Index,float *buf)
{
	if(bIsWAVE)
	{
		return false;
	}
	for(int i=0;i<NumBlock;i++)
	{
		int pos=nHeadSize+(i*Skh2000Head.NumLog+Index)*512;
		fSkh2000.Seek(pos,CFile::begin);
		fSkh2000.Read(&buf[i*128],512);
	}

	//���Ϊ�ϲ⣬��������������
	if(Skh2000Head.Stdep>Skh2000Head.Endep)
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
bool CSkh2000R::ReadWAVE(float Depth,short *buf)
{
	if(!bIsWAVE)
	{
		return false;
	}
	int nRec=int(fabs((Depth-Skh2000Head.Stdep)/Skh2000Head.Rlev)+0.5);
	if(nRec<0 || nRec>=NumPoint) return false;

	int pos=nHeadSize+nRec*nBlockSize;
	fSkh2000.Seek(pos,CFile::begin);
	 //VDL3��VDL5����������ÿ������ռһ���ֽڣ�Ҫ��������õ���ѹֵ,
	//ÿ����ֻ��ǰ��1000��������
	BYTE b[2048];
	fSkh2000.Read(b,2048);
	for(int i=0;i<2048;i++)
	{
		buf[i]=int((b[i]*19.6087)-2500);
	}
	return true;
}
bool CSkh2000R::ReadWAVE(int index,float Depth,short *buf)
{
	if(!bIsWAVE)
	{
		return false;
	}
	int nRec=int(fabs((Depth-Skh2000Head.Stdep)/Skh2000Head.Rlev)+0.5);
	if(nRec<0 || nRec>=NumPoint) return false;

	int pos=nHeadSize+nRec*nBlockSize+index*1024;
	fSkh2000.Seek(pos,CFile::begin);
	 //VDL3��VDL5����������ÿ������ռһ���ֽڣ�Ҫ��������õ���ѹֵ,
	//ÿ����ֻ��ǰ��1000��������
	BYTE b[1024];
	fSkh2000.Read(b,1024);
	for(int i=0;i<1024;i++)
	{
		buf[i]=int((b[i]*19.6087)-2500);
	}
	return true;
}

void CSkh2000R::Close()
{
	fSkh2000.Close();
}

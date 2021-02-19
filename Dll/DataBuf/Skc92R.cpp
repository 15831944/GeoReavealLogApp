// Skc92R.cpp: implementation of the CSkc92R class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Skc92R.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkc92R::CSkc92R()
{
	bVdl=false; //VDL�⾮�Ҵ���VDL����
	bCCLFour=false;
}

CSkc92R::~CSkc92R()
{

}
bool _declspec(dllexport) IsSKC92(CString sDataFile)
{
	CString s;
	CFile f;
	SKC92_HEAD skc92;
	if(f.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE) return false;
	f.Read(&skc92,sizeof(skc92));
	f.Close();
	if(skc92.NumLog<=0 || skc92.NumLog>20)
	{
		return false; //������Ϊ1-20
	}
	for(int i=0;i<skc92.NumLog;i++)
	{
		if(!isalpha(skc92.CurveName[i][0])) return false; //���������ַ���������ĸ
	}
	return true;
}
bool CSkc92R::Open(CString sDataFile)
{
	if(fSkc92.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE)
	{
		return false;
	}
	FileSize=fSkc92.GetLength(); //�ļ�����
	return true;
}
bool CSkc92R::ReadHead()
{
	CString s;
	fSkc92.SeekToBegin();
	fSkc92.Read(&Skc92Head,sizeof(Skc92Head));
	if(fabs(Skc92Head.Rlev)<0.001)
	{
		Skc92Head.Rlev=0.1f; //����ļ��в��������¼������
	}
	bCCLFour=IsCCLHasFourPoint(); //�ж�CCL�����Ƿ��м��ܲ���
	nBlockSize=0;
	for(int i=0;i<Skc92Head.NumLog;i++)
	{
		CurveTypes[i]=4;
		s.Format ("%-4.4s",Skc92Head.CurveName[i]);
		s.TrimLeft();
		s.TrimRight();
		s.MakeUpper();
		if(bCCLFour && s=="CCL")
		{
			CurveTypes[i]=16; //�׹ܽӹ����ߵĲ������ǳ����4��
		}
		if(Skc92Head.VdlFlag>0 && (s=="VDL"  || s=="WAVE" || s.Left(2)=="WF"))
		{
			CurveTypes[i]=1920; //���λ���ܶ�
			bVdl=true; //VDL�⾮�Ҵ���VDL����
		}
		nBlockSize+=CurveTypes[i]; //�ۼƼ�¼����
	}
	NumBlock=(FileSize-sizeof(Skc92Head))/nBlockSize;//���ݼ�¼�������������ͬ
	
	Skc92Head.Rlev=0.1f; 
	//����ļ���û��ȷ�еĽ�����ȣ�������ļ��м�¼��Ϣ���м���
	if(fabs(Skc92Head.Endep)<0.01 || Skc92Head.Stdep>Skc92Head.Endep)
	{
		//�ļ��н������Ϊ0����ʾ�ϲ�
		Skc92Head.Rlev=-0.1f;
	}
	NumPoint=int(fabs((Skc92Head.Endep-Skc92Head.Stdep)/Skc92Head.Rlev)+0.5)+1;
	if(NumPoint>NumBlock)
	{
		NumPoint=NumBlock;
	}
	//����������
	Skc92Head.Endep=Skc92Head.Stdep+(NumPoint-1)*Skc92Head.Rlev;
	return true;
}

void CSkc92R::ClearHead()
{
	//��ͷ��Ϣ���й���
	for(int i=79;i>=0;i--)
	{
		if(Skc92Head.WellName[i]!=' ' && Skc92Head.WellName[i]!=0x0)break;
		Skc92Head.WellName[i]=0;
	}
	for(i=79;i>=0;i--)
	{
		if(Skc92Head.CompanyName[i]!=' ' && Skc92Head.CompanyName[i]!=0x0)break;
		Skc92Head.CompanyName[i]=0;
	}
	for(i=Skc92Head.NumLog;i<20;i++)
	{
		for(int j=0;j<4;j++)
		{
			Skc92Head.CurveName[i][j]=32;
		}
	}
}

bool CSkc92R::ReadChannel(int Index,float *buf)
{
	int nOffSet=0; //����ƫ����
	int NumItem=CurveTypes[Index]/4;
	for(int i=0;i<Index;i++)
	{
		nOffSet+=CurveTypes[i];
	}
	for(i=0;i<NumPoint;i++)
	{
		int pos=sizeof(Skc92Head)+i*nBlockSize+nOffSet;
		fSkc92.Seek(pos,CFile::begin);
		fSkc92.Read(&buf[NumItem*i],CurveTypes[Index]);
	}
	//���Ϊ�ϲ⣬��������������
	int Num=NumPoint*NumItem;
	if(Skc92Head.Stdep>Skc92Head.Endep)
	{
		for(int i=0;i<Num/2;i++)
		{
			float fTemp=buf[i];
			buf[i]=buf[Num-i-1];
			buf[Num-i-1]=fTemp;
		}
	}
	return true;
}
bool CSkc92R::ReadVDL(int Index,float Depth,short *buf)
{
	int nItem=CurveTypes[Index]/2;//һ���㲨�β�����
	if(nItem != 960) 
	{
		return false; //SKC92���ܶȵ�����Ϊ960����
	}
	int nRec=int(fabs((Depth-Skc92Head.Stdep)/Skc92Head.Rlev)+0.5);
	if(nRec<0 || nRec>=NumPoint) 
	{
		return false;
	}
	int nOffSet=0; //����ƫ����
	for(int i=0;i<Index;i++)
	{
		nOffSet+=CurveTypes[i];
	}
	int pos=sizeof(Skc92Head)+nRec*nBlockSize+nOffSet;
	fSkc92.Seek(pos,CFile::begin);
	fSkc92.Read(buf,CurveTypes[Index]);
	for(i=0;i<nItem;i++)
	{
		buf[i]=((buf[i]>>4)&0X0FFF)-2048;
	}
	return true;
}
void CSkc92R::Close()
{
	fSkc92.Close();
}
bool CSkc92R::ExistVDL()
{
	CString s;
	for(int i=0;i<Skc92Head.NumLog;i++)
	{
		s.Format ("%-4.4s",Skc92Head.CurveName[i]);
		s.TrimLeft();
		s.TrimRight();
		s.MakeUpper();
		if(Skc92Head.VdlFlag>0 && (s=="VDL"  || s=="WAVE" || s.Left(2)=="WF"))
		{
			return true;
		}
	}
	return false;
}
bool CSkc92R::IsCCLHasFourPoint()
{
	CString s;
	bool bCCL=false;
	for(int i=0;i<Skc92Head.NumLog;i++)
	{
		s.Format ("%-4.4s",Skc92Head.CurveName[i]);
		s.TrimLeft();
		s.TrimRight();
		s.MakeUpper();
		if(s=="CCL")
		{
			bCCL=true;
			break;
		}
	}
	if(!bCCL)
	{
		return false; //��CCL����
	}
	int NumPoint=int(fabs((Skc92Head.Endep-Skc92Head.Stdep)/Skc92Head.Rlev)+0.5);
	int DataLength=FileSize-sizeof(Skc92Head);
	int BlockSize=Skc92Head.NumLog*4;
	if(ExistVDL())
	{
		BlockSize+=(1920-4);
	}
	int NumPoint1=DataLength/BlockSize; //������CCL���߼��ܲ���ʱ�Ĳ�����
	int NumPoint2=DataLength/(BlockSize+12); //����CCL���߼��ܲ���ʱ�Ĳ�����
	if(abs(NumPoint-NumPoint1)<abs(NumPoint-NumPoint2))
	{
		return false; //������CCL���߼��ܲ���ʱ�Ĳ���������ȼ�����ӽ�
	}
	return true;
}

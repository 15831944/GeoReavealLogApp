// Skc98R.cpp: implementation of the CSkc98R class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Skc98R.h"
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

CSkc98R::CSkc98R()
{

}

CSkc98R::~CSkc98R()
{

}
bool _declspec(dllexport) IsSKC98(CString sDataFile)
{
	CString s;
	s=MyGetFileExt(sDataFile);
	if(s.CompareNoCase("skc")!=0)
	{
		return false; //BIT�ļ�������չ��������BIT
	}
	CFile f;
	SKC98_HEAD skc98;
	if(f.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE) return false;
	f.Read(&skc98,sizeof(skc98));
	f.Close();
	if(skc98.SKCFlag != 3000)
	{
		return false;
	}
	if(skc98.NumLog<=0 || skc98.NumLog>30)
	{
		return false; //������Ϊ1-30
	}
	for(int i=0;i<skc98.NumLog;i++)
	{
		if(!isalpha(skc98.CurveName[i][0])) return false; //���������ַ���������ĸ
	}
	return true;
}

bool CSkc98R::Open(CString sDataFile)
{
	if(fSkc98.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE)
	{
		return false;
	}
	FileSize=fSkc98.GetLength(); //�ļ�����
	return true;
}
bool CSkc98R::ReadHead()
{
	CString s;
	fSkc98.SeekToBegin();
	fSkc98.Read(&Skc98Head,sizeof(Skc98Head));
	nBlockSize=0;
	for(int i=0;i<Skc98Head.NumLog;i++)
	{
		nBlockSize+=Skc98Head.CurveTypes[i];
	}
	NumPoint=int(fabs((Skc98Head.Endep-Skc98Head.Stdep)/Skc98Head.Rlev)+0.5)+1;
	NumBlock=(FileSize-sizeof(Skc98Head))/nBlockSize;//���ݼ�¼�������������ͬ
	if(Skc98Head.Stdep>Skc98Head.Endep)
	{
		Skc98Head.Rlev=-float(fabs(Skc98Head.Rlev));
	}
	if(NumPoint>NumBlock)
	{
		NumPoint=NumBlock;
		//����������
		Skc98Head.Endep=Skc98Head.Stdep+(NumPoint-1)*Skc98Head.Rlev;
	}
	for(i=0;i<Skc98Head.NumLog;i++)
	{
		yStep[i]=Skc98Head.Rlev;
		if(Skc98Head.CurveTypes[i]<=16 && Skc98Head.CurveTypes[i]>4)
		{
			int NumY=Skc98Head.CurveTypes[i]/4;
			yStep[i]=Skc98Head.Rlev/NumY;
		}
	}
	return true;
}

void CSkc98R::ClearHead()
{
	//��ͷ��Ϣ���й���
	for(int i=79;i>=0;i--)
	{
		if(Skc98Head.WellName[i]!=' ' && Skc98Head.WellName[i]!=0x0)break;
		Skc98Head.WellName[i]=0;
	}
	for(i=79;i>=0;i--)
	{
		if(Skc98Head.CompanyName[i]!=' ' && Skc98Head.CompanyName[i]!=0x0)break;
		Skc98Head.CompanyName[i]=0;
	}
	for(i=Skc98Head.NumLog;i<30;i++)
	{
		memset(Skc98Head.CurveName[i],' ',4);
	}
}

bool CSkc98R::ReadChannel(int Index,float *buf)
{
	int nOffSet=0; //����ƫ����
	for(int i=0;i<Index;i++)
	{
		nOffSet+=Skc98Head.CurveTypes[i];
	}

	//������һ����ȵ�Ĳ�����(���ڳ�������)
	int nItem=Skc98Head.CurveTypes[Index]/4; 

	for(i=0;i<NumPoint;i++)
	{
		int pos=sizeof(Skc98Head)+i*nBlockSize+nOffSet;
		fSkc98.Seek(pos,CFile::begin);
		fSkc98.Read(&buf[nItem*i],Skc98Head.CurveTypes[Index]);
	}

	//���Ϊ�ϲ⣬��������������
	if(Skc98Head.Stdep>Skc98Head.Endep)
	{
		int Number=NumPoint*nItem;
		for(int i=0;i<Number/2;i++)
		{
			float fTemp=buf[i];
			buf[i]=buf[Number-i-1];
			buf[Number-i-1]=fTemp;
		}
	}
	return true;
}
bool CSkc98R::ReadVDL(int Index,float Depth,short *buf)
{
	int nOffSet=0; //����ƫ����
	for(int i=0;i<Index;i++)
	{
		nOffSet+=Skc98Head.CurveTypes[i];
	}
	int nItem=Skc98Head.CurveTypes[Index]/2;//һ���㲨�β�����
	int nRec=int(fabs((Depth-Skc98Head.Stdep)/Skc98Head.Rlev)+0.5);
	if(nRec<0 || nRec>=NumPoint) return false;

	int pos=sizeof(Skc98Head)+nRec*nBlockSize+nOffSet;
	fSkc98.Seek(pos,CFile::begin);
	fSkc98.Read(buf,Skc98Head.CurveTypes[Index]);
	for(i=0;i<nItem;i++)
	{
		buf[i]=((buf[i]>>4)&0X0FFF)-2048;
	}
	return true;
}
void CSkc98R::Close()
{
	fSkc98.Close();
}
bool CSkc98R::IsConvCurve(int index)
{
	if(Skc98Head.CurveTypes[index]==4)
	{
		return true;
	}
	CString s;
	s.Format ("%-4.4s",Skc98Head.CurveName[index]);
	s.TrimRight();
	if(s=="VDL"  || s=="WAVE" || s.Left(2)=="WF")
	{
		return false;
	}
	return true;
}

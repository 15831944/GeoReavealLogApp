// Qh716W.cpp: implementation of the CQh716W class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Qh716W.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQh716W::CQh716W()
{

}

CQh716W::~CQh716W()
{

}

bool CQh716W::Open(CString sDataFile)
{
	if(fQh716.Open(sDataFile,CFile::modeCreate|CFile::modeWrite)==FALSE)
	{
		return false;
	}
	return true;
}

void CQh716W::Close()
{
	fQh716.Close();
}

void CQh716W::ResetHead() //ͷ��Ϣ���
{
	Head.Head=0; 
	Head.ECC=0; 
	memset(Head.CorpName,' ',80);
	memset(Head.WellName,' ',80);
	Head.NumLog=0;
	Head.IZ=0;
	for(int i=0;i<128;i++)
	{
		memset(Head.CurveName[i],' ',4);
	}
	Head.Stdep=0;
	Head.Endep=0;
	Head.Rlev=0.125;
	Head.IDC=0;
	Head.IZZ=0;
	Head.Feet=64;
	for(i=0;i<4;i++)
	{
		Head.DumFull[i]=0;
	}
	Head.End=0; 
}

bool CQh716W::WriteHead()
{ 
	CString s;
	QH716_HEAD qh=Head;
	//���ݾ��ε�������Ϣ�����ļ�����
	NumPoint=int((Head.Endep-Head.Stdep+0.5*Head.Rlev)/Head.Rlev)+1;
	NumBlock=(NumPoint-1)/64+1;
	BlockSize=4*Head.NumLog*int(Head.Feet);
	if(Head.NumLog<=20) 
	{
		Head.Head=276;
	}
	else
	{
		Head.Head=276+(qh.NumLog-20)*4;
	}
	
	FileSize=Head.Head+8+NumBlock*(BlockSize+8);
	//�����ļ�����
	fQh716.SetLength(FileSize);
	//ת���������ϢΪSUN��ʽ
	qh=Head;
	SunI4TOPC(qh.Head);
	SunR4TOPC(qh.ECC);
	SunI2TOPC(qh.NumLog);
	SunR4TOPC(qh.Stdep);
	SunR4TOPC(qh.Endep);
	SunR4TOPC(qh.Rlev);
	SunI2TOPC(qh.IDC);
	SunR4TOPC(qh.Feet);
	SunI2TOPC(qh.IDC);
	SunI4TOPC(qh.End);
	//д�ļ�ͷ��Ϣ
	fQh716.SeekToBegin();
	fQh716.Write(&qh.Head,sizeof(int)); //ͷ����
	fQh716.Write(&qh.ECC,4);
	fQh716.Write(qh.CorpName,80);
	fQh716.Write(qh.WellName,80);
	fQh716.Write(&qh.NumLog,2);
	fQh716.Write(&qh.IZ,2);
	for(int i=0;i<20;i++)
	{
	  fQh716.Write(qh.CurveName[i],4);
	}
	fQh716.Write(&qh.Stdep,4);
	fQh716.Write(&qh.Endep,4);
	fQh716.Write(&qh.Rlev,4);
	fQh716.Write(&qh.IDC,2); 
	fQh716.Write(&qh.IZZ,2);
	fQh716.Write(&qh.Feet,4);
	fQh716.Write(qh.DumFull,8);
	if(Head.NumLog>20)
	{
		for(int i=20;i<Head.NumLog;i++)
		{
			fQh716.Write(qh.CurveName[i],4);
		}
	}
	fQh716.Write(&qh.Head,sizeof(int)); //ͷ����
	//���ÿ����ĳ���
	int BlockLen=BlockSize;
	SunI4TOPC(BlockLen);
	for(i=0;i<NumBlock;i++)
	{
		int RecHeadPos=Head.Head+8+i*(BlockSize+8);
		int RecTrailPos=RecHeadPos+BlockSize+4;
		fQh716.Seek(RecHeadPos,CFile::begin);
		fQh716.Write(&BlockLen,4); //��¼ͷ����
		fQh716.Seek(RecTrailPos,CFile::begin);
		fQh716.Write(&BlockLen,4); //��¼β����
	}
	return true;
}

void CQh716W::WriteChannel(int Index,float *buf)
{
	//ת�����ݴ洢��ʽ
	for(int i=0;i<NumBlock*int(Head.Feet);i++)
	{
		SunR4TOPC(buf[i]);
	}
	//��ǰ����ƫ����
	int RecSiz=4+Index*int(Head.Feet)*4;
	//���һ����������
	for(i=0;i<NumBlock;i++)
	{
		int pos=Head.Head+8+i*(BlockSize+8)+RecSiz;
		fQh716.Seek(pos,CFile::begin);
		fQh716.Write(&buf[i*int(Head.Feet)],4*int(Head.Feet));
	}
}
void CQh716W::PrintHead(FILE *fp)
{
	//���ת�������Ϣ
	fprintf(fp,"Well Name: %-.80s\n",Head.WellName);
	fprintf(fp,"Company name: %-.80s\n",Head.CorpName);
	fprintf(fp,"Start Depth: %G\n",Head.Stdep);
	fprintf(fp,"Stop Depth: %G\n",Head.Endep);
	fprintf(fp,"Sample Interval: %G\n",Head.Rlev);
	fprintf(fp,"Curve Number: %d\n",Head.NumLog);
	fprintf(fp,"Curve Name:\n");
	for(int i=0;i<Head.NumLog;i++)
	{
		if((i>0) && (i%10==0))
		{
			fprintf(fp,"\n");
		}
		fprintf(fp," %-4.4s",Head.CurveName[i]);
	}
	fprintf(fp,"\n---- End Head information ---\n\n");
}
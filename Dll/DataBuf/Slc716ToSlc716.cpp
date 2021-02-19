// Slc716ToSlc716.cpp: implementation of the CSlc716ToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "Slc716ToSlc716.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSlc716ToSlc716::CSlc716ToSlc716()
{

}

CSlc716ToSlc716::~CSlc716ToSlc716()
{

}
bool CSlc716ToSlc716::Slc716ToSlc716(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
{
	CString s;
	m_SourceFile=SourceFile; //Դ�ļ���   
	m_sObjectPath=sObjectPath;  //���Ŀ¼
	m_nCurvesRadio=nCurvesRadio;//���˿���
	fp=fo;

	pGuage= new CProgressDlg;
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetRange(0,100);
	pGuage->SetStep(1);
	pGuage->OffsetPos(0);
	pGuage->ShowWindow(SW_SHOW);

	CurveIndex.RemoveAll();
	Slc716R=new CSlc716R;
	Slc716W=new CSlc716W;
	bool bResult=Transform();

	//���ת�������Ϣ
	if(bResult)
	{
		Slc716W->PrintHead(fp);
	}

	////////////////////////////////////
	CurveIndex.RemoveAll();
	delete Slc716W;	Slc716W=NULL;
	delete Slc716R; Slc716R=NULL;
	delete pGuage;  //ɾ��������
	return bResult;
}

bool CSlc716ToSlc716::Transform()
{
	if(!IsSLC716(m_SourceFile))
	{
		fprintf(fp,"%s is not SLC716 Format\n\n",m_SourceFile);
		return false;
	}

	CString s;
	//�������Ŀ¼�������ļ���ȷ������ļ���
	sOutFileName.Format ("%s\\%s.716",m_sObjectPath,MyGetFileTitle(m_SourceFile));
	ModiFileName(sOutFileName); //�������������Ƿ��޸��ļ���

	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)sOutFileName);

	if(!Slc716R->Open(m_SourceFile)) 
	{
		fprintf(fp,"\n!!!!���ӵ������ļ���ʧ��\n\n");
		return FALSE; //������SLC716
	}
	Slc716R->ReadHead(); //��SLC716ͷ
	if(Slc716W->Open(sOutFileName))
	{
		Slc716W->Slc716Head=Slc716R->Slc716Head; //ͷ��Ϣ����
		Slc716HeadToSlc716Head(); //Slc716R->Slc716Head,Slc716W->Slc716Head,nIndex);
		Slc716W->NumPoint=Slc716R->NumPoint; //һ�����ߵĲ�������
		Slc716W->NumBlock=Slc716R->NumBlock; //���ݼ�¼��
		Slc716W->ClearHead(); //������ļ�ͷ
		Slc716W->WriteHead(); //���ͷ��Ϣ
		pGuage->SetRange(0,Slc716W->Slc716Head.NumLog);
		float *buf=new float[Slc716R->NumBlock*64]; //һ�����ߵĿռ�
		for(int i=0;i<Slc716W->Slc716Head.NumLog;i++)
		{
			pGuage->SetPos(i+1);  //���ý�������ǰֵ
			if (pGuage->CheckCancelButton())
			{
				sp_Break_Status=true;
				break;
			}
			Slc716R->ReadChannel(CurveIndex.GetAt(i),buf);
			Slc716W->WriteChannel(i,buf);
		}
		Slc716W->Close();
		delete []buf; buf=0;
	}
	Slc716R->Close(); //�ر������ļ�

	return !sp_Break_Status;
}
void CSlc716ToSlc716::Slc716HeadToSlc716Head()
{
	CString s;
	CurveIndex.RemoveAll();
	if(m_nCurvesRadio==0)
	{
		Slc716W->Slc716Head.NumLog=0; //�����������Ϊ0��Ȼ��ӹ��˱��в���
		for(int i=0;i<Slc716R->Slc716Head.NumLog;i++)
		{
			s.Format ("%-4.4s",Slc716R->Slc716Head.CurveName[i]);
			s.TrimLeft();
			s.TrimRight();
			for(int j=0;j<sp_FilterCurves.GetSize();j++)
			{
				if(s.CompareNoCase(sp_FilterCurves.GetAt(j))==0)
				{
					CurveIndex.Add(i);
					sprintf(Slc716W->Slc716Head.CurveName[Slc716W->Slc716Head.NumLog],"%-4.4s",Slc716R->Slc716Head.CurveName[i]);
					Slc716W->Slc716Head.NumLog++;
					break;
				}
			}
		}
	}
	else
	{
		for(int i=0;i<Slc716W->Slc716Head.NumLog;i++)
		{
			CurveIndex.Add(i);
		}
	}
}

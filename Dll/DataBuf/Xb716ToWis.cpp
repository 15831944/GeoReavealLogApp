// Xb716ToWis.cpp: implementation of the CXb716ToWis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "Xb716ToWis.h"
#include <direct.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXb716ToWis::CXb716ToWis()
{
	WisWrite=NULL;
}

CXb716ToWis::~CXb716ToWis()
{

}
bool CXb716ToWis::Xb716ToWis(CString SourceFile,CString ObjectPath,int nCurvesRadio,FILE *fo)
{
	m_SourceFile=SourceFile; //Դ�ļ���
	m_sObjectPath=ObjectPath; //���Ŀ¼
	m_nCurvesRadio=nCurvesRadio; //���˿���
	fp=fo;
	pGuage= new CProgressDlg;
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetRange(0,100);
	pGuage->SetStep(1);
	pGuage->OffsetPos(0);
	pGuage->ShowWindow(SW_SHOW);
	
	Xb716R=new CXb716R;
	WisWrite=new CWisWrite;
	CurveIndex.RemoveAll();
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	bool bResult=Transform();

	//���ת�������Ϣ
	if(bResult)
	{
		WisWrite->PrintHead(fp);
		fprintf(fp,"%d Curve:\n",NewCurveName.GetSize());
		for(int i=0;i<NewCurveName.GetSize();i++)
		{
			if(i>0 && i%5==0)
			{
				fprintf(fp,"\n");
			}
			fprintf(fp," %-10.15s",(const char*)(NewCurveName.GetAt(i)));
		}
		fprintf(fp,"\n");
		fprintf(fp,"---- End Head information ---\n\n");
	}

	delete WisWrite; WisWrite=NULL;
	delete Xb716R; Xb716R=NULL;
	CurveIndex.RemoveAll();
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	delete pGuage;  //ɾ��������
	return true;
}
bool CXb716ToWis::Transform()
{
	CString s;
	if(!IsXB716(m_SourceFile))
	{
		fprintf(fp,"%s is not XB716 Format\n\n",m_SourceFile);
		return false;
	}
	//�������Ŀ¼�������ļ���ȷ������ļ���
	CString Filetitle=MyGetFileTitle(m_SourceFile);
	if(sp_bDirectory)
	{
		WisFileName.Format ("%s\\#%s\\%s.wis",m_sObjectPath,Filetitle,Filetitle);
		CString WellPath=m_sObjectPath+"\\#"+Filetitle;
		_mkdir(WellPath);
	}
	else
	{
		WisFileName.Format ("%s\\%s.wis",m_sObjectPath,Filetitle);
	}
	ModiFileName(WisFileName); //�������������Ƿ��޸��ļ���
	
	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)WisFileName);

	//////////////
	if(!Xb716R->Open(m_SourceFile))
	{
		return false;
	}
	Xb716R->ReadHead(); //��XB716ͷ
	if(Xb716R->Head.NumLog<1)
	{
		Xb716R->Close();
		return false;
	}
	WisWrite->Open(WisFileName);
	GetWisObjectIndexFromXb716();
	//���������¼����������
	WisWrite->InitWisHead(CurveIndex.GetSize());
	CString sCurveName;
	WisWrite->m_ObjectEntry=NULL; //�������
	WisWrite->WisChannel=NULL; //ͨ����Ϣ
	WisWrite->m_ObjectEntry=new WIS_OBJECT_ENTRY;
	WisWrite->WisChannel=new WIS_CHANNEL;
	WisWrite->InitWisObjectEntry();
	WisWrite->InitWisChannel();
	///////////////////////////
	pGuage->SetRange(0,CurveIndex.GetSize());
	int nBufLen=Xb716R->NumBlock*64; //����������
	float *buf=new float[nBufLen]; //��716
	for(int index=0;index<CurveIndex.GetSize();index++)
	{
		pGuage->SetPos(index+1);  //���ý�������ǰֵ
		if (pGuage->CheckCancelButton())
		{
			sp_Break_Status=true;
			break;
		}
		int k=CurveIndex.GetAt(index);
		SetWisChannelFromXb716Head(k,index); //����ͨ������
		WisWrite->WriteChannel(); //дͨ����Ϣ��WIS�ļ�
		sCurveName.Format("%-.4s",Xb716R->Head.CurveName[k]);
		sCurveName.TrimRight();
		Xb716R->ReadCurve(index,buf);
		WisWrite->WriteGendata(Xb716R->NumPoint,buf);
	}
	delete []buf; buf=NULL;
	WisWrite->Close();
	Xb716R->Close();
	////////////////////////////////
	delete WisWrite->m_ObjectEntry; 
	WisWrite->m_ObjectEntry=NULL;
	delete WisWrite->WisChannel; 
	WisWrite->WisChannel=NULL;
	
	return !sp_Break_Status;
}
void CXb716ToWis::GetWisObjectIndexFromXb716()
{
	CString s,sCurveName;
	CurveIndex.RemoveAll();
	WisWrite->WisHead.ObjectNumber=0;
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	if(m_nCurvesRadio==3)
	{	//��������
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=15;
		ReCurNamDlg.m_nMaxLog=2048;
		for(int i=0;i<Xb716R->Head.NumLog;i++)
		{
			sCurveName.Format ("%-.4s",Xb716R->Head.CurveName[i]);
			sCurveName.TrimLeft();
			sCurveName.TrimRight();
			ReCurNamDlg.m_SourceName.Add(sCurveName);
			ReCurNamDlg.m_ObjectName.Add(sCurveName);
			ReCurNamDlg.m_Units.Add("");
			ReCurNamDlg.m_NumDim.Add(1);
		}
		int Result=ReCurNamDlg.DoModal();
		for(i=0;i<ReCurNamDlg.m_SourceName.GetSize();i++)
		{
			SelCurveName.Add(ReCurNamDlg.m_SourceName.GetAt(i));
			NewCurveName.Add(ReCurNamDlg.m_ObjectName.GetAt(i));
			NewUnits.Add(ReCurNamDlg.m_Units.GetAt(i));
		}
		ReCurNamDlg.m_SourceName.RemoveAll();
		ReCurNamDlg.m_ObjectName.RemoveAll();
		ReCurNamDlg.m_Units.RemoveAll();
		ReCurNamDlg.m_NumDim.RemoveAll();
		for(i=0;i<SelCurveName.GetSize();i++)
		{
			for(int j=0;j<Xb716R->Head.NumLog;j++)
			{
				sCurveName.Format ("%-.4s",Xb716R->Head.CurveName[j]);
				sCurveName.TrimLeft();
				sCurveName.TrimRight();
				if(sCurveName.CompareNoCase(SelCurveName.GetAt(i))==0)
				{
					CurveIndex.Add(j);
					break;
				}
			}
		}
	}
	else
	{	//�ǽ�������
		for(int i=0;i<Xb716R->Head.NumLog;i++)
		{
			bool bOk=false;
			sCurveName.Format ("%-.4s",Xb716R->Head.CurveName[i]);
			sCurveName.TrimLeft();
			sCurveName.TrimRight();
			if(m_nCurvesRadio==0)
			{	//����
				for(int j=0;j<sp_FilterCurves.GetSize();j++)
				{
					if(sCurveName.CompareNoCase(sp_FilterCurves.GetAt(j))==0)
					{
						bOk=true;
						break;
					}
				}
			}
			else
			{
				bOk=true;
			}
			if(!bOk) continue;
			CurveIndex.Add(i);
			SelCurveName.Add(sCurveName);
			NewCurveName.Add(sCurveName);
			NewUnits.Add("");
		}
	}
}

void CXb716ToWis::SetWisChannelFromXb716Head(int IndexXb716,int IndexWis)
{
	
	//��������(����������WIS�����������)
	sprintf(WisWrite->m_ObjectEntry->Name,"%-.15s",NewCurveName.GetAt(IndexWis));
	WisWrite->m_ObjectEntry->Attribute=1;
	WisWrite->m_ObjectEntry->SubAttribute=1;
	//ͨ������
	sprintf(WisWrite->WisChannel->Unit,"%-.7s",NewUnits.GetAt(IndexWis));
	sprintf(WisWrite->WisChannel->AliasName,"%-.4s",Xb716R->Head.CurveName[IndexXb716]);
	sprintf(WisWrite->WisChannel->AliasUnit,"%-.15s",WisWrite->WisChannel->Unit);
	
	WisWrite->WisChannel->RepCode=WIS_REPR_FLOAT;
	WisWrite->WisChannel->CodeLen=4;
	WisWrite->WisChannel->MinVal=0;
	WisWrite->WisChannel->MaxVal=100;
	WisWrite->WisChannel->Reserved=0;
	WisWrite->WisChannel->NumOfDimension=1;
	DWORD NumPoint=DWORD((Xb716R->Head.Endep-Xb716R->Head.Stdep)/Xb716R->Head.Rlev+0.5)+1;
	//����һά��Ϣ
	strcpy(WisWrite->WisChannel->DimInfo[0].Name,"DEP");
	strcpy(WisWrite->WisChannel->DimInfo[0].Unit,"m");
	strcpy(WisWrite->WisChannel->DimInfo[0].AliasName,"DEPTH");
	WisWrite->WisChannel->DimInfo[0].StartVal=Xb716R->Head.Stdep;
	WisWrite->WisChannel->DimInfo[0].Delta=Xb716R->Head.Rlev;
	WisWrite->WisChannel->DimInfo[0].Samples=NumPoint;
	WisWrite->WisChannel->DimInfo[0].MaxSamples=WisWrite->WisChannel->DimInfo[0].Samples;
	WisWrite->WisChannel->DimInfo[0].Size=4;
	WisWrite->WisChannel->DimInfo[0].RepCode=WIS_REPR_FLOAT;
	WisWrite->WisChannel->DimInfo[0].Reserved=0;
}

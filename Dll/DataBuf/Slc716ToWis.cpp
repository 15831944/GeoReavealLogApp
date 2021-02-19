// Slc716ToWis.cpp: implementation of the CSlc716ToWis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "Slc716ToWis.h"
#include <direct.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSlc716ToWis::CSlc716ToWis()
{
	WisWrite=NULL;
}

CSlc716ToWis::~CSlc716ToWis()
{

}
bool CSlc716ToWis::Slc716ToWis(CString SourceFile,CString ObjectPath,int nCurvesRadio,FILE *fo)
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
	
	Slc716R=new CSlc716R;
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
	delete Slc716R; Slc716R=NULL;
	CurveIndex.RemoveAll();
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	delete pGuage;  //ɾ��������
	return true;
}
bool CSlc716ToWis::Transform()
{
	CString s;
	if(!IsSLC716(m_SourceFile))
	{
		fprintf(fp,"%s is not SLC716 Format\n\n",m_SourceFile);
		return false;
	}
	//�������Ŀ¼�������ļ���ȷ������ļ���
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
	if(!Slc716R->Open(m_SourceFile))
	{
		return false;
	}
	Slc716R->ReadHead(); //��SLC716ͷ
	if(Slc716R->Slc716Head.NumLog<1)
	{
		Slc716R->Close();
		return false;
	}
	WisWrite->Open(WisFileName);
	GetWisObjectIndexFromSlc716();
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
	int nBufLen=Slc716R->NumBlock*64; //����������
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
		SetWisChannelFromSlc716Head(k,index); //����ͨ������
		WisWrite->WriteChannel(); //дͨ����Ϣ��WIS�ļ�
		sCurveName.Format("%-.4s",Slc716R->Slc716Head.CurveName[k]);
		sCurveName.TrimRight();
		Slc716R->ReadChannel(sCurveName,buf);
		WisWrite->WriteGendata(Slc716R->NumPoint,buf);
	}
	delete []buf; buf=NULL;
	WisWrite->Close();
	Slc716R->Close();
	////////////////////////////////
	delete WisWrite->m_ObjectEntry; 
	WisWrite->m_ObjectEntry=NULL;
	delete WisWrite->WisChannel; 
	WisWrite->WisChannel=NULL;
	
	return !sp_Break_Status;
}
void CSlc716ToWis::GetWisObjectIndexFromSlc716()
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
		for(int i=0;i<Slc716R->Slc716Head.NumLog;i++)
		{
			sCurveName.Format ("%-.4s",Slc716R->Slc716Head.CurveName[i]);
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
			for(int j=0;j<Slc716R->Slc716Head.NumLog;j++)
			{
				sCurveName.Format ("%-.4s",Slc716R->Slc716Head.CurveName[j]);
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
		for(int i=0;i<Slc716R->Slc716Head.NumLog;i++)
		{
			bool bOk=false;
			sCurveName.Format ("%-.4s",Slc716R->Slc716Head.CurveName[i]);
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

void CSlc716ToWis::SetWisChannelFromSlc716Head(int IndexSlc716,int IndexWis)
{
	//��������(����������WIS�����������)
	sprintf(WisWrite->m_ObjectEntry->Name,"%-.15s",NewCurveName.GetAt(IndexWis));
	WisWrite->m_ObjectEntry->Attribute=1;
	WisWrite->m_ObjectEntry->SubAttribute=1;
	//ͨ������
	sprintf(WisWrite->WisChannel->Unit,"%-.7s",NewUnits.GetAt(IndexWis));
	sprintf(WisWrite->WisChannel->AliasName,"%-.4s",Slc716R->Slc716Head.CurveName[IndexSlc716]);
	sprintf(WisWrite->WisChannel->AliasUnit,"%-.15s",WisWrite->WisChannel->Unit);
	
	WisWrite->WisChannel->RepCode=WIS_REPR_FLOAT;
	WisWrite->WisChannel->CodeLen=4;
	WisWrite->WisChannel->MinVal=0;
	WisWrite->WisChannel->MaxVal=100;
	WisWrite->WisChannel->Reserved=0;
	WisWrite->WisChannel->NumOfDimension=1;
	DWORD NumPoint=DWORD((Slc716R->Slc716Head.Endep-Slc716R->Slc716Head.Stdep)/Slc716R->Slc716Head.Rlev+0.5)+1;
	//����һά��Ϣ
	strcpy(WisWrite->WisChannel->DimInfo[0].Name,"DEP");
	strcpy(WisWrite->WisChannel->DimInfo[0].Unit,"m");
	strcpy(WisWrite->WisChannel->DimInfo[0].AliasName,"DEPTH");
	WisWrite->WisChannel->DimInfo[0].StartVal=Slc716R->Slc716Head.Stdep;
	WisWrite->WisChannel->DimInfo[0].Delta=Slc716R->Slc716Head.Rlev;
	WisWrite->WisChannel->DimInfo[0].Samples=NumPoint;
	WisWrite->WisChannel->DimInfo[0].MaxSamples=WisWrite->WisChannel->DimInfo[0].Samples;
	WisWrite->WisChannel->DimInfo[0].Size=4;
	WisWrite->WisChannel->DimInfo[0].RepCode=WIS_REPR_FLOAT;
	WisWrite->WisChannel->DimInfo[0].Reserved=0;
}

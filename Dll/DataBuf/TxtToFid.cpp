// TxtToFid.cpp: implementation of the CTxtToFid class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "TxtToFid.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTxtToFid::CTxtToFid()
{

}

CTxtToFid::~CTxtToFid()
{

}

bool CTxtToFid::TxtToFid(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
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

	FwdTxtR=new CFwdTxtR ;
	FidWrite=new CFidWrite ; //�����
	CurveIndex.RemoveAll();
	bool bResult=Transform();
	//���ת�������Ϣ
	if(bResult)
	{
		FidWrite->PrintHead(fp);
	}
	delete pGuage;  //ɾ��������

	delete FidWrite; FidWrite=NULL;
	delete FwdTxtR;	FwdTxtR=NULL;
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	CurveIndex.RemoveAll();
	return bResult;
}
bool CTxtToFid::Transform()
{
	CString s;
	if(!IsFwdTxt(m_SourceFile))
	{
		fprintf(fp,"%s is not TXT Format\n\n",m_SourceFile);
		return false;
	}
	//�������Ŀ¼�������ļ���ȷ�����������·��
	CString sFileTitle=MyGetFileTitle(m_SourceFile);
	CString sOutWellPath=m_sObjectPath+"\\#"+sFileTitle;
	ModiFileName(sOutWellPath); //�������������Ƿ��޸��ļ���
	sFileTitle=MyGetFileTitle(sOutWellPath);
	CString sWellName=sFileTitle.Mid(1); //ȥ����#��
	FidWrite->SetFidFileName(m_sObjectPath,sWellName);

	if(!FwdTxtR->Open(m_SourceFile)) 
	{
		fprintf(fp,"\n!!!!���ӵ������ļ���ʧ��\n\n");
	}
	else
	{
		FwdTxtR->ReadHead(); //��TXTͷ
		FwdTxtR->Prt_Header(fp);
		SelectCurves();
		if(CurveIndex.GetSize()==0) return false;
		SetFidHead();
		FidWrite->WriteFidInfo();
		int NumPoint=FwdTxtR->FwdTxtHd.NumPoint;
		int NumLog=FwdTxtR->FwdTxtHd.NumLog;
		int Count=NumPoint*NumLog;
		float *buf=new float[Count*2];
		for(int i=0;i<Count;i++)
			buf[i]=-999.25;
		pGuage->m_nTextMode=1; //�ı�ģʽ
		pGuage->SetRange(0,FidWrite->FidHead.NumLog);
		pGuage->m_sTextInfo="���ڶ�ȡ�ı��ļ�";
		pGuage->SetPos(1);  //���ý�������ǰֵ
		FwdTxtR->ReadCurve(buf);//��ȫ������
		FwdTxtR->Close(); //�ر������ļ�
		pGuage->m_sTextInfo="�����ݽ���������滯";
		pGuage->SetPos(1);  //���ý�������ǰֵ
		DataNormalization(NumPoint,NumLog,buf);
		for(i=0;i<FidWrite->FidHead.NumLog;i++)
		{
			pGuage->m_sTextInfo=NewCurveName.GetAt(i);
			pGuage->SetPos(i+1);  //���ý�������ǰֵ
			if(pGuage->CheckCancelButton())
			{
				sp_Break_Status=true;
				break;
			}
			int index=CurveIndex.GetAt(i);
			FidWrite->WriteGenData(i,NumPoint,&buf[index*NumPoint]);
		}
		delete []buf; buf=NULL;
	}
	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)FidWrite->m_sFidFile);
	return !sp_Break_Status;
}
void CTxtToFid::DataNormalization(int NumPoint,int NumLog,float *buf)
{
	//���Ϊ�ϲ⣬��������������
	if(FwdTxtR->FwdTxtHd.Rlev>0) return;
	for(int i=0;i<NumLog;i++)
	{
		int n1=i*NumPoint;
		int n2=i*NumPoint+NumPoint/2;
		for(int j=n1;j<n2;j++)
		{
			int n=(i+1)*NumPoint-j+n1-1;
			float fTemp=buf[j];
			buf[j]=buf[n];
			buf[n]=fTemp;
		}
	}
}

void CTxtToFid::SelectCurves()
{
	CString s;
	bool *CurveID=new bool[FwdTxtR->FwdTxtHd.NumLog];
	for(int i=0;i<FwdTxtR->FwdTxtHd.NumLog;i++)
		CurveID[i]=false;
	CString sCurveName;
	switch(m_nCurvesRadio)
	{
	case 0://���߹���		
		for(i=0;i<FwdTxtR->FwdTxtHd.NumLog;i++)
		{
			bool bOk=false;
			sCurveName.Format ("%s",FwdTxtR->FwdTxtHd.CurveName[i]);
			sCurveName.TrimRight();
			for(int j=0;j<sp_FilterCurves.GetSize();j++)
			{
				if(sCurveName.CompareNoCase(sp_FilterCurves.GetAt(j))==0)
				{
					bOk=true;
					break;
				}
			}
			if(bOk)
			{
				CurveID[i]=true;
			}
		}
		break;
	case 1: //ȫ������
	case 2: //��������
	case 3: //����ѡ��
		for(i=0;i<FwdTxtR->FwdTxtHd.NumLog;i++)
		{
			CurveID[i]=true;
		}
		break;
	}
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	//���ݿ������þ����Ƿ���н���ѡ��
	if(m_nCurvesRadio !=3 )
	{	//û�н�������
		for(i=0;i<FwdTxtR->FwdTxtHd.NumLog;i++)
		{
			if(!CurveID[i]) continue;
			sCurveName.Format("%s",FwdTxtR->FwdTxtHd.CurveName[i]);
			SelCurveName.Add(sCurveName);
			sCurveName.MakeUpper();
			sCurveName.TrimRight();
			NewCurveName.Add(sCurveName);
			NewUnits.Add("");
		}
	}
	else
	{	//��������
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=15; //������߳���
		ReCurNamDlg.m_nMaxLog=99999; //���������
		for(int i=0;i<FwdTxtR->FwdTxtHd.NumLog;i++)
		{
			if(!CurveID[i]) continue;
			sCurveName.Format ("%s",FwdTxtR->FwdTxtHd.CurveName[i]);
			ReCurNamDlg.m_SourceName.Add(sCurveName);
			sCurveName.TrimLeft();
			sCurveName.TrimRight();
			sCurveName.Remove(' ');
			ReCurNamDlg.m_ObjectName.Add(sCurveName);//Ԥ����������
			ReCurNamDlg.m_Units.Add("");
			ReCurNamDlg.m_NumDim.Add(1);
		}
		int Result=ReCurNamDlg.DoModal();
		for(i=0;i<ReCurNamDlg.m_SourceName.GetSize();i++)
		{
			//ԭ����
			SelCurveName.Add(ReCurNamDlg.m_SourceName.GetAt(i));
			//������
			s=ReCurNamDlg.m_ObjectName.GetAt(i);
			s.MakeUpper();
			NewCurveName.Add(s);
			//��λ
			NewUnits.Add(ReCurNamDlg.m_Units.GetAt(i));
		}
		ReCurNamDlg.m_SourceName.RemoveAll();
		ReCurNamDlg.m_ObjectName.RemoveAll();
		ReCurNamDlg.m_NumDim.RemoveAll();
		ReCurNamDlg.m_Units.RemoveAll();
	}
	///////////////////////////////////////////////////
	delete []CurveID; CurveID=NULL;
	CurveIndex.RemoveAll();
	for(i=0;i<SelCurveName.GetSize();i++)
	{
		for(int j=0;j<FwdTxtR->FwdTxtHd.NumLog;j++)
		{
			s.Format ("%s",FwdTxtR->FwdTxtHd.CurveName[j]);
			if(s.CompareNoCase(SelCurveName.GetAt(i))==0)
			{
				CurveIndex.Add(j);
				break;
			}
		}
	}
}

void CTxtToFid::SetFidHead()
{
	sprintf(FidWrite->FidHead.Oil_Field,"%-80.80s","");
	sprintf(FidWrite->FidHead.Region,"%-80.80s","");
	sprintf(FidWrite->FidHead.Company,"%-80.80s","�Ĵ�ʯ�͹���ֲ⾮��˾");
	sprintf(FidWrite->FidHead.Well,"%-80.80s","");
	sprintf(FidWrite->FidHead.X_Coordinate,"%-40.40s","");
	sprintf(FidWrite->FidHead.Y_Coordinate,"%-40.40s","");
	sprintf(FidWrite->FidHead.Exformation,"%-80.80s","");

	FidWrite->FidHead.NumLog=CurveIndex.GetSize();
	//����������(FidWrite->FidHead.NumLog)���ٿռ�
	FidWrite->InitFidChannel(); 
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		int k=CurveIndex.GetAt(i);
		strcpy(FidWrite->FidChannel[i].CurveName,NewCurveName.GetAt(i));
		FidWrite->FidChannel[i].NumOfDimension=2;
		
		//���õ�һά����
		strcpy(FidWrite->FidChannel[i].DimInfo[0].Name,"DEP");
		strcpy(FidWrite->FidChannel[i].DimInfo[0].Unit,"m");
		FidWrite->FidChannel[i].DimInfo[0].RepCode=4;
		FidWrite->FidChannel[i].DimInfo[0].CodeLen=4;
		FidWrite->FidChannel[i].DimInfo[0].Nps=1;
		FidWrite->FidChannel[i].DimInfo[0].Npw=0;
		if(FwdTxtR->FwdTxtHd.Rlev>0)
		{
			FidWrite->FidChannel[i].DimInfo[0].Start=FwdTxtR->FwdTxtHd.Stdep;
			FidWrite->FidChannel[i].DimInfo[0].Stop=FwdTxtR->FwdTxtHd.Endep;
			FidWrite->FidChannel[i].DimInfo[0].Rlev=FwdTxtR->FwdTxtHd.Rlev;
		}
		else
		{
			FidWrite->FidChannel[i].DimInfo[0].Start=FwdTxtR->FwdTxtHd.Endep;
			FidWrite->FidChannel[i].DimInfo[0].Stop=FwdTxtR->FwdTxtHd.Stdep;
			FidWrite->FidChannel[i].DimInfo[0].Rlev=float(fabs(FwdTxtR->FwdTxtHd.Rlev));
		}

		//���õڶ�ά����
		strcpy(FidWrite->FidChannel[i].DimInfo[1].Name,FidWrite->FidChannel[i].CurveName);
		if(NewUnits.GetAt(i).IsEmpty())
			strcpy(FidWrite->FidChannel[i].DimInfo[1].Unit,"none");
		else
			strcpy(FidWrite->FidChannel[i].DimInfo[1].Unit,NewUnits.GetAt(i));
		FidWrite->FidChannel[i].DimInfo[1].RepCode=4;
		FidWrite->FidChannel[i].DimInfo[1].CodeLen=4;
		FidWrite->FidChannel[i].DimInfo[1].Nps=1;
		FidWrite->FidChannel[i].DimInfo[1].Npw=1;
		FidWrite->FidChannel[i].DimInfo[1].Start=0;
		FidWrite->FidChannel[i].DimInfo[1].Stop=100;
		FidWrite->FidChannel[i].DimInfo[1].Rlev=0;
	}
}

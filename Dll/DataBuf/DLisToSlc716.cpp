// DLisToSlc716.cpp: implementation of the CDLisToSlc716 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "_comext.h"
#include "DLisToSlc716.h"
#include "DlisCurDlg.h"
#include <stdlib.h>
#include "ModiConvDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDLisToSlc716::CDLisToSlc716()
{

}

CDLisToSlc716::~CDLisToSlc716()
{

}
bool CDLisToSlc716::DLisToSlc716(CString SourceFile,CString sObjectPath,int nCurvesRadio,int nConvEncode,float FixRlev,FILE *fo)
{
	CString s;
	m_SourceFile=SourceFile; //Դ�ļ���   
	m_sObjectPath=sObjectPath;  //���Ŀ¼
	m_nCurvesRadio=nCurvesRadio;//���˿���
	m_nConvEncode=nConvEncode;
	m_FixRlev=FixRlev;
	fp=fo;
	pGuage= new CProgressDlg;
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetRange(0,100);
	pGuage->SetStep(1);
	pGuage->OffsetPos(0);
	pGuage->ShowWindow(SW_SHOW);
	
	DlisRead=new CDlisRead ;
	Slc716W=new CSlc716W ; //�����
	bool bResult=Transform();
	delete pGuage;  //ɾ��������
	delete Slc716W; Slc716W=NULL;
	delete DlisRead; DlisRead=NULL;
	
	NewCurveName.RemoveAll();

	return bResult;
}
bool CDLisToSlc716::Transform()
{
	CString s;
	if(!IsDLIS(m_SourceFile))
	{
		fprintf(fp,"%s is not DLIS Format\n\n",m_SourceFile);
		return false;
	}
	CString szWellName=MyGetFileTitle(m_SourceFile);
	if(!DlisRead->Open(m_SourceFile,fp)) 
	{
		fprintf(fp,"\n!!!!���ӵ������ļ���ʧ��\n\n");
	}
	else
	{
		//���ý�����ΪһС�Σ�Ϊ���Ӿ�
		pGuage->SetRange(0,100);
		pGuage->SetPos(6);
		//�������DLIS�ļ���ȡ���߼��ļ��������߼��ļ���λ��
		int NumLogFile=DlisRead->GetDlisLogicalFileNumber();
		for(int nf=0;nf<NumLogFile;nf++)
		{
			fprintf(fp,"\nDataBuff: %s\n",(const char*)m_SourceFile);
			fprintf(fp,"�� %d �߼��ļ�����ʼλ��: %u Bytes\n",nf+1,DlisRead->LogFilePos.GetAt(nf));
			pGuage->SetRange(0,NumLogFile);
			pGuage->SetPos(nf+1); //ֻ��Ϊ���ý�������Ϊ0
			DlisRead->ScanDlisOneLogicalFile(nf);
			//DlisRead->Prt_LogFileIndex();
			/////////////////////////////////////////////////////////
			CString WellName;
			if(nf==0)
			{
				WellName=szWellName;
			}
			else
			{
				WellName.Format("%s_%d",szWellName,nf+1);
			}
			bool bResult=LogicalToSlc716(WellName,nf);
			if(sp_Break_Status) break; //�ж�
			//���ת�������Ϣ
			if(bResult)
			{
				Slc716W->PrintHead(fp);
			}
		}
		DlisRead->Close(); //�ر������ļ�
	}
	return !sp_Break_Status;
}
bool CDLisToSlc716::DlisCurvesFilter()
{
	CString s;
	CStringArray sCurve;
	bool bFilter=false;
	int i;
	//����ѡ�񿪹ؽ��й���
	switch(m_nCurvesRadio)
	{
	case 0: //��������	
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				int k=DlisRead->Frame[i].ChannelID[j];
				CString sCurveName;
				bool bResult=false;
				sCurveName.Format ("%s",DlisRead->Channel[k].Name);
				for(int k1=0;k1<sp_FilterCurves.GetSize();k1++)
				{
					if(sCurveName.CompareNoCase(sp_FilterCurves.GetAt(k1))==0)
					{
						bResult=true;
						break;
					}
				}
				DlisRead->Frame[i].bDecode[j]=bResult;			
			}
		}
		break;
	case 2: //��������
	case 1: //ȫ������
	case 3: //����ѡ��
	default:
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				int k=DlisRead->Frame[i].ChannelID[j];
				if(DlisRead->Channel[k].Sample==1)
				{
					DlisRead->Frame[i].bDecode[j]=true;
				}
				else
				{
					DlisRead->Frame[i].bDecode[j]=false;
				}
			}
		}
		break;
	}
	//�����֡���ݣ�����Ӧ֡��������Ϊ������
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		if(DlisRead->Frame[i].noFrame>0) continue;
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			DlisRead->Frame[i].bDecode[j]=false;
		}
	}
	//����֡��DIRECTION�����ж��Ƿ����������ߣ�������ɢ���ݣ���ʱû�ж�����д���
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		//�����֡���߲������������������(����֡������������Ϊ������)
		if(DlisRead->Frame[i].bDiscrete)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				DlisRead->Frame[i].bDecode[j]=false;
			}
		}
	}
	//������������Ϊ������
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		DlisRead->Frame[i].bDecode[0]=false;
	}
	//ֻѡ�񳣹�����
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			int k=DlisRead->Frame[i].ChannelID[j];
			if(DlisRead->Channel[k].Sample!=1)
			{
				DlisRead->Frame[i].bDecode[j]=false;
			}
		}
	}

	//�����Ƿ���н�������
	switch(m_nCurvesRadio)
	{
	case 3: //��������
		bFilter=true;
		break;
	default: //���������������ж��Ƿ�Ҫ���и����Ȳ���
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				if(DlisRead->Frame[i].bDecode[j])
				{
					int k=DlisRead->Frame[i].ChannelID[j];
					//�жϳ����Ƿ����Ҫ��
					if(strlen(DlisRead->Channel[k].Name)>4)
					{
						bFilter=true;
						break;
					}
					//�ж��������Ƿ��Ѿ�����
					for(int k1=0;k1<sCurve.GetSize();k1++)
					{
						if(sCurve.GetAt(k1).Compare(DlisRead->Channel[k].Name)==0)
						{
							bFilter=true;
							break;				
						}
					}
					//ǰ��������ʱ����
					sCurve.Add(DlisRead->Channel[k].Name);
				}
			}
			if(bFilter) break;
		}
		break;
	}
	sCurve.RemoveAll();
	return bFilter;
}
void CDLisToSlc716::SetStartStopRlev()
{
	int nDep;
	CModiConvDlg ModiConvDlg;
	switch(m_nConvEncode)
	{
	case 1: //������ȼ����ʵ�����ֹ��ȼ��������
		nDep=int(Slc716W->Slc716Head.Stdep/m_FixRlev+0.95);
		Slc716W->Slc716Head.Stdep=nDep*m_FixRlev;
		nDep=int(Slc716W->Slc716Head.Endep/m_FixRlev);
		Slc716W->Slc716Head.Endep=nDep*m_FixRlev;
		Slc716W->Slc716Head.Rlev=m_FixRlev;
		break;
	case 2: //�˹�ָ����ֹ��ȼ��������
		ModiConvDlg.m_OldStart=Slc716W->Slc716Head.Stdep;
		ModiConvDlg.m_OldStop=Slc716W->Slc716Head.Endep;
		ModiConvDlg.m_OldRlev=Slc716W->Slc716Head.Rlev;
		if(ModiConvDlg.DoModal()==IDOK)
		{
			Slc716W->Slc716Head.Stdep=ModiConvDlg.m_NewStart;
			Slc716W->Slc716Head.Endep=ModiConvDlg.m_NewStop;
			Slc716W->Slc716Head.Rlev=ModiConvDlg.m_NewRlev;
		}
		break;
	default://��������ָ��������С�����������󾮶�
		break;
	}
}

bool CDLisToSlc716::LogicalToSlc716(CString WellName,int nf)
{
	CString s;
	//�������Ŀ¼�������ļ���ȷ������ļ���
	sOutFileName.Format ("%s\\%s.716",m_sObjectPath,WellName);
	ModiFileName(sOutFileName); //�������������Ƿ��޸��ļ���

	SetSlc716HeadFromDlisHead();
	if(NewCurveName.GetSize()==0) 
	{
		fprintf(fp,"\n!!!!û��Ҫ���������\n");
		return false;
	}
	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)sOutFileName);
	SetStartStopRlev(); //��������ָ����ȼ��������
	if(!Slc716W->Open(sOutFileName)) return false;
	Slc716W->NumPoint=int((Slc716W->Slc716Head.Endep-Slc716W->Slc716Head.Stdep)/Slc716W->Slc716Head.Rlev+0.5)+1;
	Slc716W->NumBlock=(Slc716W->NumPoint-1)/64+1;
	Slc716W->ClearHead(); //������ļ�ͷ
	Slc716W->WriteHead(); //���ͷ��Ϣ
	DlisRead->ReadLogicalFile(nf); //DLIS���ݶ�����֡Ϊ��λ���ļ�
	//���ý�������ΧΪ����������
	pGuage->SetRange(0,NewCurveName.GetSize());
	pGuage->SetPos(1);
	int NumLog=-1; //����������
	for(int i=0;i<DlisRead->NumFrame;i++)
	{
		if(sp_Break_Status) break;
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			pGuage->SetPos(NumLog+2);
			if (pGuage->CheckCancelButton())
			{
				sp_Break_Status=true;
				break;
			}
			if(!DlisRead->Frame[i].bDecode[j]) continue;
			NumLog++;
			int index=DlisRead->Frame[i].ChannelID[j];
			int Num1=DlisRead->Frame[i].noFrame;
			float *buf1=new float[Num1];
			float *buf2=new float[Slc716W->NumBlock*64];
			for(int k=0;k<Slc716W->NumBlock*64;k++)
			{
				buf2[k]=-999.25;
			}
			DlisRead->ReadCurve(i,j,Num1,buf1);
			float Start1=__min(DlisRead->Frame[i].Index_Start,DlisRead->Frame[i].Index_Stop);
			float Stop1=__max(DlisRead->Frame[i].Index_Start,DlisRead->Frame[i].Index_Stop);
			float Rlev1=float(fabs(DlisRead->Frame[i].Index_Step));
			FloatResample(Start1,Stop1,Rlev1,buf1,Slc716W->Slc716Head.Stdep,Slc716W->Slc716Head.Endep,Slc716W->Slc716Head.Rlev,buf2);
			Slc716W->WriteChannel(NumLog,buf2);
			delete []buf1;	buf1=NULL;
			delete []buf2;	buf2=NULL;
		}
	}
	Slc716W->Close();
	return true;
}
void CDLisToSlc716::SelectChannelFromWin()
{
	CString s;
	CDlisCurDlg DlisCurDlg;
	NewCurveName.RemoveAll();
	int NumLog=0;
	for(int i=0;i<DlisRead->NumFrame;i++)
	{
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			if(DlisRead->Frame[i].bDecode[j])
			{
				NumLog++;
				int k=DlisRead->Frame[i].ChannelID[j];
				CString sNewName=DlisRead->Channel[k].Name; //������
				DlisRead->GetNewName(sNewName,i,j,DlisRead->Channel[k].CopyNum,
					DlisRead->Channel[k].Origin,sp_bAddFrameNo,
					sp_bAddOrigin,sp_bAddCopyNum);
				DlisCurDlg.m_ObjectName.Add(sNewName); //���������;
				DlisCurDlg.m_SourceName.Add(DlisRead->Channel[k].Name); ////����������
				s.Format ("%u",DlisRead->Channel[k].Origin);
				DlisCurDlg.m_Origin.Add(s); //ͨ����Դ
				s.Format ("%u",DlisRead->Channel[k].CopyNum);
				DlisCurDlg.m_CopyNum.Add(s); //ͨ���п�����
				s.Format ("%.1f-%.1f",DlisRead->Frame[i].Index_Start,DlisRead->Frame[i].Index_Stop);
				DlisCurDlg.m_IndexInfo.Add(s); //��ֹ����
				s.Format ("%.5f",DlisRead->Frame[i].Index_Step);
				DlisCurDlg.m_StepInfo.Add(s); //�������
				s.Format ("%d",i);
				DlisCurDlg.m_FrameNum.Add(s); //֡��
				CString sValue="[";
				for(int k1=0;k1<DlisRead->Channel[k].NumDim;k1++)
				{
					if(k1>0)
					{
						sValue+=",";
					}
					s.Format ("%d",DlisRead->Channel[k].Dimension[k1]);
					sValue+=s;
				}
				sValue+="]";
				DlisCurDlg.m_Dimension.Add(sValue); //ά����
			}
		}
	}
	if(NumLog==0) return ; //û������
	DlisCurDlg.m_nMaxLen=4;
	DlisCurDlg.m_nMaxLog=128;
	DlisCurDlg.m_NumLog=NumLog;
	DlisCurDlg.nTransMode=m_nCurvesRadio;
	//////////////////////////////////////////////////////////////////
	DlisCurDlg.DoModal();
	//////////////////////////////////////////////////////////////////
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			DlisRead->Frame[i].bDecode[j]=false;
		}
	}
	for(i=0;i<DlisCurDlg.m_ObjectName.GetSize();i++)
	{
		CString sFrame=DlisCurDlg.m_FrameNum.GetAt(i);
	
		int iFrame=atol((const char*)sFrame);
		DWORD Origin=atol(DlisCurDlg.m_Origin.GetAt(i));
		BYTE CopyNum=BYTE(atol(DlisCurDlg.m_CopyNum.GetAt(i)));
		CString sCurveName=	DlisCurDlg.m_SourceName.GetAt(i);

		for(int j=0;j<DlisRead->Frame[iFrame].NumCurve;j++)
		{
			int k=DlisRead->Frame[iFrame].ChannelID[j];
			if(DlisRead->Channel[k].Origin != Origin) continue;
			if(DlisRead->Channel[k].CopyNum != CopyNum) continue;
			if(sCurveName.Compare(DlisRead->Channel[k].Name)==0)
			{
				DlisRead->Frame[iFrame].bDecode[j]=true;
				NewCurveName.Add(DlisCurDlg.m_ObjectName.GetAt(i));
				break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////
	DlisCurDlg.m_ObjectName.RemoveAll(); //���������
	DlisCurDlg.m_SourceName.RemoveAll(); //����������
	DlisCurDlg.m_IndexInfo.RemoveAll(); //��ֹ����
	DlisCurDlg.m_StepInfo.RemoveAll(); //�������
	DlisCurDlg.m_FrameNum.RemoveAll(); //֡������֡����Ŵ��棩
	DlisCurDlg.m_Dimension.RemoveAll(); //ά����
	DlisCurDlg.m_Origin.RemoveAll(); //ͨ����Դ
	DlisCurDlg.m_CopyNum.RemoveAll(); //ͨ���п�����
}

void CDLisToSlc716::SetSlc716HeadFromDlisHead()
{
	CString s;
	char str[128];
	Slc716W->ResetHead(); //ͷ��Ϣ���
	s.Format ("%-.80s",DlisRead->Origin.WellName);
	s.TrimLeft();
	s.TrimRight();
	sprintf(str,"%-80.80s",s);
	memcpy(Slc716W->Slc716Head.WellName,str,80);
	s.Format ("%-.80s",DlisRead->Origin.Company);
	s.TrimLeft();
	s.TrimRight();
	sprintf(str,"%-80.80s",s);
	memcpy(Slc716W->Slc716Head.CompanyName,str,80);
	Slc716W->Slc716Head.NumLog=0;
	CString sCurveName;
	//////////////////////////////////////////////////////////////
	bool bResult=DlisCurvesFilter();//��������ѡ�񿪹�(m_nCurvesRadio)�����߽��й���
	if(bResult)
	{
		//Ҫ���н�������
		SelectChannelFromWin();
	}
	else
	{//���ؽ��н�������
		NewCurveName.RemoveAll();
		for(int i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				if(DlisRead->Frame[i].bDecode[j])
				{
					int k=DlisRead->Frame[i].ChannelID[j];
					NewCurveName.Add(DlisRead->Channel[k].Name);
				}
			}
		}
	}
	////////////////////////////////////////////////////////////////////
	if(NewCurveName.GetSize()==0) return ;
	float MinIndex=-999.25;
	float MaxIndex=-999.25;
	float Step=-999.25;
	bool bFirst=true;
	for(int i=0;i<DlisRead->NumFrame;i++)
	{
		if(DlisRead->Frame[i].noFrame<2) continue;
		if(bFirst)
		{
			MinIndex=__min(DlisRead->Frame[i].Index_Start,DlisRead->Frame[i].Index_Stop);
			MaxIndex=__max(DlisRead->Frame[i].Index_Start,DlisRead->Frame[i].Index_Stop);
			Step=float(fabs(DlisRead->Frame[i].Index_Step));
			bFirst=false;
		}
		else
		{
			MinIndex=__min(MinIndex,__min(DlisRead->Frame[i].Index_Start,DlisRead->Frame[i].Index_Stop));
			MaxIndex=__max( MinIndex,__max(DlisRead->Frame[i].Index_Start,DlisRead->Frame[i].Index_Stop));
			Step=__min(Step,float(fabs(DlisRead->Frame[i].Index_Step)));
		}
	}
	Slc716W->Slc716Head.Stdep=MinIndex;
	Slc716W->Slc716Head.Endep=MaxIndex;
	Slc716W->Slc716Head.Rlev=Step;
	//������
	Slc716W->Slc716Head.NumLog=NewCurveName.GetSize();
	int NumLog=-1; //ʵ�����������
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			if(DlisRead->Frame[i].bDecode[j])
			{
				NumLog++;
				int index=DlisRead->Frame[i].ChannelID[j]; //������DLISͨ���е�������
				char str[5];
				sprintf(str,"%-4.4s",(const char*)(NewCurveName.GetAt(NumLog)));
				memcpy(Slc716W->Slc716Head.CurveName[NumLog],str,4);
			}
		}
	}
}

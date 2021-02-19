// DLisToWis.cpp: implementation of the CDLisToWis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "_Comext.h"
#include "DLisToWis.h"
#include "DlisCurDlg.h"
#include "CurveSplit.h"
#include <direct.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDLisToWis::CDLisToWis()
{

}

CDLisToWis::~CDLisToWis()
{

}
bool CDLisToWis::DLisToWis(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
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
	
	DlisRead=new CDlisRead ;
	WisWrite=new CWisWrite ; //�����
	bool bResult=Transform();
	delete pGuage;  //ɾ��������
	delete WisWrite; WisWrite=NULL;
	delete DlisRead; DlisRead=NULL;
	
	NewCurveName.RemoveAll();

	return bResult;
}
bool CDLisToWis::Transform()
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
		CString WellPath;
		int NumLogFile=DlisRead->GetDlisLogicalFileNumber();
		if(NumLogFile>0)
		{
			if(sp_bDirectory)
			{
				WellPath=m_sObjectPath+"\\#"+szWellName;
				_mkdir((const char*)WellPath);
			}
		}
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
			bool bResult=LogicalToWis(WellName,WellPath,nf);
			if(sp_Break_Status) break; //�ж�
			//���ת�������Ϣ
			if(bResult)
				WisWrite->PrintHead(fp);
			//�����Ϣ
		}
		DlisRead->Close(); //�ر������ļ�
	}
	return !sp_Break_Status;
}
bool CDLisToWis::LogicalToWis(CString WellName,CString WellPath,int nf)
{
	CString s;
	//�������Ŀ¼�������ļ���ȷ������ļ���
	if(sp_bDirectory)
		sOutFileName.Format ("%s\\%s.wis",WellPath,WellName);
	else
		sOutFileName.Format ("%s\\%s.wis",m_sObjectPath,WellName);
	ModiFileName(sOutFileName); //�������������Ƿ��޸��ļ���

	SetWisHeadFromDlisHead();
	if(NewCurveName.GetSize()==0)
	{
		fprintf(fp,"\n!!!!û��Ҫ���������\n");
		return false;
	}
	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)sOutFileName);
	if(!WisWrite->Open(sOutFileName)) return false;
	WisWrite->InitWisHead(NewCurveName.GetSize());
	WisWrite->m_ObjectEntry=NULL; //�������
	WisWrite->WisChannel=NULL; //ͨ����Ϣ
	WisWrite->m_ObjectEntry=new WIS_OBJECT_ENTRY;
	WisWrite->WisChannel=new WIS_CHANNEL;
	WisWrite->InitWisObjectEntry();
	WisWrite->InitWisChannel();
	DlisRead->ReadLogicalFile(nf); //DLIS���ݶ�����֡Ϊ��λ���ļ�
	//���ý�������ΧΪ����������
	pGuage->SetRange(0,NewCurveName.GetSize());
	pGuage->SetPos(1);
	int NumLog=-1; //����������
	fprintf(fp,"====���������Ϣ:\n");
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
			SetWisChannelFromFidHead(NumLog,i,j); //����ͨ������
			fprintf(fp,"%4d %-8.15s %9.3f %9.3f %6d %3d\n",
				NumLog+1,WisWrite->m_ObjectEntry->Name,
				WisWrite->WisChannel->DimInfo[0].StartVal,
				WisWrite->WisChannel->DimInfo[0].Delta,
				WisWrite->WisChannel->DimInfo[0].Samples,
				WisWrite->WisChannel->NumOfDimension);
			WisWrite->WriteChannel(); //дͨ����Ϣ��WIS�ļ�
			int NumPoint=DlisRead->Frame[i].noFrame;
			if(WisWrite->WisChannel->NumOfDimension==1)
			{
				float *buf=new float[NumPoint];
				DlisRead->ReadCurve(i,j,NumPoint,buf);
				WisWrite->WriteGendata(NumPoint,buf);
				delete []buf; buf=NULL;
			}
			else
			{	
				//���λ���������
				int Count=DlisRead->Channel[index].Sample;
				float *buf=new float[Count];
				for(int k=0;k<Count;k++)buf[k]=0; //��ֵ
				for(k=0;k<NumPoint;k++)
				{
					float depth=WisWrite->WisChannel->DimInfo[0].StartVal+k*WisWrite->WisChannel->DimInfo[0].Delta;
					DlisRead->ReadWaveArray(i,j,depth,buf);
					WisWrite->WriteWaveArray(depth,buf);
				}
				delete []buf; buf=NULL;
			}
		}
	}
	WisWrite->Close();
	delete WisWrite->m_ObjectEntry; 
	WisWrite->m_ObjectEntry=NULL;
	delete WisWrite->WisChannel; 
	WisWrite->WisChannel=NULL;

	return true;
}
void CDLisToWis::SetWisChannelFromFidHead(int IndexWis,int nFra,int nCur)
{
	CString s;
	//��������(����������WIS�����������)
	int IndexDlis=DlisRead->Frame[nFra].ChannelID[nCur];
	sprintf(WisWrite->m_ObjectEntry->Name,"%-.15s",NewCurveName.GetAt(IndexWis));
	WisWrite->m_ObjectEntry->Attribute=1;
	int kDim=1; //WIS�ļ�ͨ��ά��
	if(DlisRead->Channel[IndexDlis].Sample>1)
	{
		kDim=DlisRead->Channel[IndexDlis].NumDim+1;
	}
	if(kDim>1)
	{
		WisWrite->m_ObjectEntry->SubAttribute=2;
	}
	else
	{
		WisWrite->m_ObjectEntry->SubAttribute=1;
	}
	//ͨ������
	sprintf(WisWrite->WisChannel->Unit,"%-.8s",DlisRead->Channel[IndexDlis].Units);
	sprintf(WisWrite->WisChannel->AliasName,"%-.15s",WisWrite->m_ObjectEntry->Name);
	sprintf(WisWrite->WisChannel->AliasUnit,"%-.15s",WisWrite->WisChannel->Unit);
	
	WisWrite->WisChannel->RepCode=DlisRepCToWis(DlisRead->Channel[IndexDlis].RepCode);
	WisWrite->WisChannel->CodeLen=WisCodeLen(WisWrite->WisChannel->RepCode);
	WisWrite->WisChannel->MinVal=0;
	WisWrite->WisChannel->MaxVal=100;
	WisWrite->WisChannel->Reserved=0;
	WisWrite->WisChannel->NumOfDimension=kDim;
	float Start=DlisRead->Frame[nFra].Index_Start;
	float Stop=DlisRead->Frame[nFra].Index_Stop;
	float Step=float(fabs(DlisRead->Frame[nFra].Index_Step));
	if(Start>Stop)
	{	
		Stop=DlisRead->Frame[nFra].Index_Start;
		Start=DlisRead->Frame[nFra].Index_Stop;
	}
	DWORD NumPoint=DWORD(fabs((Stop-Start)/Step)+0.5)+1;
	//����һά��Ϣ
	s.Format ("%s",DlisRead->Frame[nFra].Index_Type);
	if(s.CompareNoCase("TIME")==0)
	{
		strcpy(WisWrite->WisChannel->DimInfo[0].Name,"TIME");
	}
	else
	{
		strcpy(WisWrite->WisChannel->DimInfo[0].Name,"DEPTH");
	}
	sprintf(WisWrite->WisChannel->DimInfo[0].Unit,"%-.7s",DlisRead->Frame[nFra].IndexUnits);
	strcpy(WisWrite->WisChannel->DimInfo[0].AliasName,WisWrite->WisChannel->DimInfo[0].Name);
	WisWrite->WisChannel->DimInfo[0].StartVal=Start;
	WisWrite->WisChannel->DimInfo[0].Delta=Step;
	WisWrite->WisChannel->DimInfo[0].Samples=NumPoint;
	WisWrite->WisChannel->DimInfo[0].MaxSamples=WisWrite->WisChannel->DimInfo[0].Samples;
	WisWrite->WisChannel->DimInfo[0].Size=DlisRead->Channel[IndexDlis].Sample*WisWrite->WisChannel->CodeLen; //////////////////////
	WisWrite->WisChannel->DimInfo[0].RepCode=WIS_REPR_FLOAT;
	WisWrite->WisChannel->DimInfo[0].Reserved=0;
	//����ά������Ϣ
	for(int k=1;k<WisWrite->WisChannel->NumOfDimension;k++)
	{
		int kx=WisWrite->WisChannel->NumOfDimension-k-1;
		//�������У���TIME��SENSOR_OFFSET��SENSOR��ORDINAL
		s.Format("%s",DlisRead->Channel[IndexDlis].AxisID[kx]);
		if(s=="MICRO_TIME" || s=="TIME")
		{
			strcpy(WisWrite->WisChannel->DimInfo[k].Name,"TIME");
		}
		else if(s=="SENSOR_OFFSET" || s=="SENSOR")
		{
			strcpy(WisWrite->WisChannel->DimInfo[k].Name,"SENSOR");
		}
		else
		{
			strcpy(WisWrite->WisChannel->DimInfo[k].Name,"ORDINAL");
		}
		sprintf(WisWrite->WisChannel->DimInfo[k].Unit,"%-.7s",DlisRead->Channel[IndexDlis].AxisUnits[kx]);
		sprintf(WisWrite->WisChannel->DimInfo[k].AliasName,"%-.15s",DlisRead->Channel[IndexDlis].AxisID[kx]);

		WisWrite->WisChannel->DimInfo[k].StartVal=DlisRead->Channel[IndexDlis].AxisStart[kx];
		WisWrite->WisChannel->DimInfo[k].Delta=DlisRead->Channel[IndexDlis].AxisStep[kx];
		if(WisWrite->WisChannel->DimInfo[k].Delta==0)
		{
			WisWrite->WisChannel->DimInfo[k].Delta=1; //��֤���������Ϊ�����Ա���WIS�ļ�����
		}
		WisWrite->WisChannel->DimInfo[k].Samples=1;
		WisWrite->WisChannel->DimInfo[k].Samples=DlisRead->Channel[IndexDlis].Dimension[kx];
		WisWrite->WisChannel->DimInfo[k].MaxSamples=WisWrite->WisChannel->DimInfo[k].Samples;
		int TotalSample=1;
		for(int j=0;j<=kx;j++)
		{
			TotalSample*=DlisRead->Channel[IndexDlis].Dimension[j];
		}
		WisWrite->WisChannel->DimInfo[k].Size=TotalSample*WisWrite->WisChannel->CodeLen;
		WisWrite->WisChannel->DimInfo[k].RepCode=WIS_REPR_FLOAT;
		WisWrite->WisChannel->DimInfo[k].Reserved=0;
	}

}
BYTE CDLisToWis::DlisRepCToWis(BYTE RepCode)
{
	BYTE WisRepC=WIS_REPR_FLOAT;
	switch(RepCode)
	{
	case DLIS_REPC_FSHORT:
	case DLIS_REPC_FSINGL:
	case DLIS_REPC_FSING1:
	case DLIS_REPC_FSING2:
	case DLIS_REPC_ISINGL:
	case DLIS_REPC_VSINGL:
	case DLIS_REPC_CSINGL:
		WisRepC=WIS_REPR_FLOAT;
		break;
	case DLIS_REPC_FDOUBL:
	case DLIS_REPC_FDOUB1:
	case DLIS_REPC_FDOUB2:
	case DLIS_REPC_CDOUBL:
		WisRepC=WIS_REPR_DOUBLE;
		break;
	case DLIS_REPC_SSHORT:
		WisRepC=WIS_REPR_CHAR;
		break;
	case DLIS_REPC_SNORM:
		WisRepC=WIS_REPR_SHORT;
		break;
	case DLIS_REPC_SLONG:
		WisRepC=WIS_REPR_LONG;
		break;
	case DLIS_REPC_USHORT:
		WisRepC=WIS_REPR_UCHAR;
		break;
	case DLIS_REPC_UNORM:
		WisRepC=WIS_REPR_USHORT;
		break;
	case DLIS_REPC_ULONG:
	case DLIS_REPC_UVARI:
		WisRepC=WIS_REPR_ULONG;
		break;
	case DLIS_REPC_IDENT:
	case DLIS_REPC_ASCII:
	case DLIS_REPC_DTIME:
	case DLIS_REPC_ORIGIN:
	case DLIS_REPC_OBNAME:
	case DLIS_REPC_OBJREF:
	case DLIS_REPC_ATTREF:
	case DLIS_REPC_ATATUS:
	case DLIS_REPC_UNITS:
		break;
	};
	return WisRepC;
}
void CDLisToWis::SetWisHeadFromDlisHead()
{
	CString s;
	CString sCurveName;
	//////////////////////////////////////////////////////////////
	bool bResult=DlisCurvesFilter();//��������ѡ�񿪹�(m_nCurvesRadio)�����߽��й���
	if(bResult)
		//Ҫ���н�������
		SelectChannelFromWin();
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
}
bool CDLisToWis::DlisCurvesFilter()
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
	case 1: //ȫ������
	case 3: //����ѡ��
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				DlisRead->Frame[i].bDecode[j]=true;
			}
		}
		break;
	default:
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
					if(strlen(DlisRead->Channel[k].Name)>FID_MAX_CURVENAME_LEN)
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
	//������������Ϊ������
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		DlisRead->Frame[i].bDecode[0]=false;
	}
	return bFilter;
}
BYTE CDLisToWis::PcCodeToFid(BYTE PcCode)
{
	BYTE FidCode=FID_REPR_FLOAT;
	switch(PcCode)
	{
	case  PC_REPR_FLOAT: //IEEE�����ȸ����� 
	case  PC_REPR_DOUBLE: //IEE˫���ȸ�����
	case  PC_REPR_UINT: //�޷�������
	case  PC_REPR_ULONG: //�޷��ų�������
		FidCode=FID_REPR_FLOAT;
		break;
	case  PC_REPR_SHORT://�з��Ŷ�������
	case  PC_REPR_UCHAR: //�޷����ֽ���
		FidCode=FID_REPR_SHORT;
		break;
	case  PC_REPR_INT: //�з�������
	case  PC_REPR_LONG: //�з��ų�������
	case  PC_REPR_USHORT: //�޷��Ŷ�������
		FidCode=FID_REPR_LONG;
		break;
	case  PC_REPR_CHAR: //�з����ֽ���
		FidCode=FID_REPR_CHAR;
		break;
	case  PC_REPR_STRING: //�ַ���
		FidCode=FID_REPR_CHAR;
		break;
	default:
		FidCode=FID_REPR_FLOAT;
		break;
	}
	return FidCode;
}

void CDLisToWis::SelectChannelFromWin()
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
				//DlisCurDlg.m_ObjectName.Add(DlisRead->Channel[k].Name); //���������;

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
	DlisCurDlg.m_nMaxLen=15;
	DlisCurDlg.m_nMaxLog=99999;
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

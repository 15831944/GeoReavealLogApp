// DLisToFid.cpp: implementation of the CDLisToFid class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "_Comext.h"
#include "DLisToFid.h"
#include "DlisCurDlg.h"
#include "CurveSplit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDLisToFid::CDLisToFid()
{

}

CDLisToFid::~CDLisToFid()
{

}
bool CDLisToFid::DLisToFid(CString SourceFile,CString sObjectPath,int nCurvesRadio,BOOL bCurveSplit,BOOL bDelSplitCur,FILE *fo)
{
	CString s;
	m_SourceFile=SourceFile; //Դ�ļ���   
	m_sObjectPath=sObjectPath;  //���Ŀ¼
	m_nCurvesRadio=nCurvesRadio;//���˿���
	m_bCurveSplit=bCurveSplit; //�������߲�ֿ���
	m_bDelSplitCur=bDelSplitCur; //ɾ�����������
	fp=fo;
	pGuage= new CProgressDlg;
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetRange(0,100);
	pGuage->SetStep(1);
	pGuage->OffsetPos(0);
	pGuage->ShowWindow(SW_SHOW);
	DlisRead=new CDlisRead ;
	FidWrite=new CFidWrite ; //�����
	bool bResult=Transform();
	delete pGuage;  //ɾ��������
	delete FidWrite;
	FidWrite=NULL;
	delete DlisRead; 
	DlisRead=NULL;
	
	NewCurveName.RemoveAll();

	return bResult;
}
bool CDLisToFid::Transform()
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
				WellName=szWellName;
			else
				WellName.Format("%s_%d",szWellName,nf+1);
			bool bResult=LogicalToFid(WellName,nf);
			
			if(sp_Break_Status) break; //�ж�
			//���ת�������Ϣ
			if(bResult)
				FidWrite->PrintHead(fp);
			//�����Ϣ
			fprintf(fp,"ToObject: %s\n",(const char*)FidWrite->m_sFidFile);
		}
		DlisRead->Close(); //�ر������ļ�
	}
	return !sp_Break_Status;
}
bool CDLisToFid::DlisCurvesFilter()
{
	CString s;
	bool bFilter=false;
	int i,j,k;
	//����ѡ�񿪹ؽ��й���
	switch(m_nCurvesRadio)
	{
	case 0: //��������
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				k=DlisRead->Frame[i].ChannelID[j];
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
	case 1: //ȫ������
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				k=DlisRead->Frame[i].ChannelID[j];
				DlisRead->Frame[i].bDecode[j]=true;
			}
		}
		break;
	case 2: //��������
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				k=DlisRead->Frame[i].ChannelID[j];
				if(DlisRead->Channel[k].Sample==1)
					DlisRead->Frame[i].bDecode[j]=true;
				else
					DlisRead->Frame[i].bDecode[j]=false;
			}
		}
		break;
	case 3: //����ѡ��
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(j=0;j<DlisRead->Frame[i].NumCurve;j++)
				DlisRead->Frame[i].bDecode[j]=true;
		}
		break;
	default:
		break;
	}
	//�����֡���ݣ�����Ӧ֡��������Ϊ������
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		if(DlisRead->Frame[i].noFrame>0) continue;
		for(j=0;j<DlisRead->Frame[i].NumCurve;j++)
			DlisRead->Frame[i].bDecode[j]=false;
	}
	//����֡��DIRECTION�����ж��Ƿ����������ߣ�������ɢ���ݣ���ʱû�ж�����д���
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		//�����֡���߲������������������(����֡������������Ϊ������)
		if(DlisRead->Frame[i].bDiscrete)
		{
			for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
				DlisRead->Frame[i].bDecode[j]=false;
		}
	}
	//�����Ƿ���н�������
	CStringArray sCurve;
	switch(m_nCurvesRadio)
	{
	case 3: //��������
		bFilter=true;
		break;
	default: //����������(������ͬ��)�ж��Ƿ�Ҫ���и����Ȳ���
		for(i=0;i<DlisRead->NumFrame;i++)
		{
			for(j=0;j<DlisRead->Frame[i].NumCurve;j++)
			{
				if(DlisRead->Frame[i].bDecode[j])
				{
					k=DlisRead->Frame[i].ChannelID[j];
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
		DlisRead->Frame[i].bDecode[0]=false;
	return bFilter;
}
BYTE CDLisToFid::PcCodeToFid(BYTE PcCode)
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


bool CDLisToFid::LogicalToFid(CString WellName,int nf)
{
	CString s;
	/////////////////////////////////////////////////////////////////
	//�������Ŀ¼�������ļ���ȷ�����������·��
	CString sFileTitle=WellName;
	CString sOutWellPath=m_sObjectPath+"\\#"+WellName;
	ModiFileName(sOutWellPath); //�������������Ƿ��޸��ļ���
	sFileTitle=MyGetFileTitle(sOutWellPath);

	CString sWellName=sFileTitle.Mid(1); //ȥ����#��
	FidWrite->SetFidFileName(m_sObjectPath,sWellName);
	SetFidHeadFromDlisHead();
	if(NewCurveName.GetSize()==0) return false;
	//���ý�������ΧΪ����������
	pGuage->SetRange(0,NewCurveName.GetSize());
	pGuage->SetPos(1);
	DlisRead->ReadLogicalFile(nf); //DLIS���ݶ�����֡Ϊ��λ���ļ�
	FidWrite->WriteFidInfo();
	
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
			int NumPoint=DlisRead->Frame[i].noFrame;
			/////////////////////////////////////////////////////////////////////
			if(FidWrite->FidChannel[NumLog].NumOfDimension==2)
			{	
				//��������
				float *buffer=new float[NumPoint];
				DlisRead->ReadCurve(i,j,NumPoint,buffer);
				FidWrite->WriteGenData(NumLog,NumPoint,buffer);
				delete []buffer; buffer=NULL;
			}
			else if(FidWrite->FidChannel[NumLog].NumOfDimension==3)
			{
				//��������
				if(FidWrite->OpenWave(NumLog))
				{
					int BufSize=DlisRead->Channel[index].Sample;
					float *buffer=new float[BufSize];
					for(int k=0;k<BufSize;k++)
					{
						buffer[k]=0;
					}
					for(k=0;k<NumPoint;k++)
					{
						float DepFram=DlisRead->Frame[i].Index_Start+k*DlisRead->Frame[i].Index_Step;
						DlisRead->ReadWaveArray(i,j,DepFram,buffer);
						FidWrite->WriteWave(DepFram,buffer);
					}
					delete []buffer; buffer=NULL;
					FidWrite->CloseWave();
				}
			}
			else
			{
				//��������
				if(FidWrite->OpenArray(NumLog))
				{
					int Count=DlisRead->Channel[index].Sample;
					float *buffer=new float[Count];
					for(int k=0;k<Count;k++)buffer[k]=0; //��ֵ
					for(k=0;k<NumPoint;k++)
					{
						float DepFram=DlisRead->Frame[i].Index_Start+k*DlisRead->Frame[i].Index_Step;
						DlisRead->ReadWaveArray(i,j,DepFram,buffer);
						FidWrite->WriteArray(DepFram,buffer);
					}
					delete []buffer; buffer=NULL;
					FidWrite->CloseArray();
				}
				//���������߽��н�ά����
				if(m_bCurveSplit)
				{
					CCurveSplit CurveSplit;
					bool bResult=CurveSplit.CurveSplit(FidWrite->m_sFidFile,FidWrite->FidChannel[NumLog].CurveName);
					if(m_bDelSplitCur)
					{	//ɾ�����������
						CString sPath=MyGetPathName(FidWrite->m_sFidFile);
						CString sWell=MyGetFileTitle(FidWrite->m_sFidFile);
						CString sCurveFile;
						sCurveFile.Format ("%s\\I%s.%s",sPath,sWell,FidWrite->FidChannel[NumLog].CurveName);
						remove(sCurveFile);
						sCurveFile.Format ("%s\\D%s.%s",sPath,sWell,FidWrite->FidChannel[NumLog].CurveName);
						remove(sCurveFile);
					}
				}
			}
		}
	}
	//��Ŀ��Ŀ¼������������ȡ������FID�ļ���
	FidWrite->FefurbishFidInfo();
	return true;
}

void CDLisToFid::SelectChannelFromWin()
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
						sValue+=",";
					s.Format ("%d",DlisRead->Channel[k].Dimension[k1]);
					sValue+=s;
				}
				sValue+="]";
				DlisCurDlg.m_Dimension.Add(sValue); //ά����
			}
		}
	}
	if(NumLog==0) return ; //û������
	DlisCurDlg.m_nMaxLen=FID_MAX_CURVENAME_LEN;
	DlisCurDlg.m_nMaxLog=99999;
	DlisCurDlg.m_NumLog=NumLog;
	DlisCurDlg.nTransMode=m_nCurvesRadio;
	//////////////////////////////////////////////////////////////////
	DlisCurDlg.DoModal();
	//////////////////////////////////////////////////////////////////
	for(i=0;i<DlisRead->NumFrame;i++)
	{
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
			DlisRead->Frame[i].bDecode[j]=false;
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

void CDLisToFid::SetFidHeadFromDlisHead()
{
	CString s;
	sprintf(FidWrite->FidHead.Oil_Field,"%-80.80s",DlisRead->Origin.FieldName);
	sprintf(FidWrite->FidHead.Region,"%-80.80s","");
	sprintf(FidWrite->FidHead.Company,"%-80.80s",DlisRead->Origin.Company);
	sprintf(FidWrite->FidHead.Well,"%-80.80s",DlisRead->Origin.WellName);
	sprintf(FidWrite->FidHead.X_Coordinate,"%-40.40s","");
	sprintf(FidWrite->FidHead.Y_Coordinate,"%-40.40s","");
	sprintf(FidWrite->FidHead.Exformation,"%-80.80s",DlisRead->Origin.CreationTime);
	FidWrite->FidHead.NumLog=0;
	CString sCurveName;
	//////////////////////////////////////////////////////////////
	bool bResult=DlisCurvesFilter();//��������ѡ�񿪹�(m_nCurvesRadio)�����߽��й���
	if(bResult)		
		SelectChannelFromWin();//Ҫ���н�������
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
	FidWrite->FidHead.NumLog=NewCurveName.GetSize();
	//����������(FidWrite->FidHead.NumLog)���ٿռ�
	FidWrite->InitFidChannel();
	int NumLog=-1; //ʵ�����������
	for(int i=0;i<DlisRead->NumFrame;i++)
	{
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			if(DlisRead->Frame[i].bDecode[j])
			{
				NumLog++;
				sprintf(FidWrite->FidChannel[NumLog].CurveName,"%s",(const char*)(NewCurveName.GetAt(NumLog)));
				int index=DlisRead->Frame[i].ChannelID[j]; //������DLISͨ���е�������
				if(DlisRead->Channel[index].Sample==1)
				{
					FidWrite->FidChannel[NumLog].NumOfDimension=2;
				}
				else
				{
					FidWrite->FidChannel[NumLog].NumOfDimension=DlisRead->Channel[index].NumDim+2;
				}
				int kD=FidWrite->FidChannel[NumLog].NumOfDimension-1; //���һά�±�
				//���õ�һά����
				if((strcmp(DlisRead->Frame[i].IndexUnits,"m")==0) && (kD<3))
					strcpy(FidWrite->FidChannel[NumLog].DimInfo[0].Name,"DEP");//��ά���������任
				else
					strcpy(FidWrite->FidChannel[NumLog].DimInfo[0].Name,DlisRead->Frame[i].Index_Type);
				strcpy(FidWrite->FidChannel[NumLog].DimInfo[0].Unit,DlisRead->Frame[i].IndexUnits);
				FidWrite->FidChannel[NumLog].DimInfo[0].RepCode=FID_REPR_FLOAT;
				FidWrite->FidChannel[NumLog].DimInfo[0].CodeLen=WisCodeLen(FID_REPR_FLOAT);
				FidWrite->FidChannel[NumLog].DimInfo[0].Nps=1;
				FidWrite->FidChannel[NumLog].DimInfo[0].Npw=0;
				FidWrite->FidChannel[NumLog].DimInfo[0].Rlev=float(fabs(DlisRead->Frame[i].Index_Step));
				if(DlisRead->Frame[i].Index_Start < DlisRead->Frame[i].Index_Stop)
				{
					FidWrite->FidChannel[NumLog].DimInfo[0].Start=DlisRead->Frame[i].Index_Start;
					FidWrite->FidChannel[NumLog].DimInfo[0].Stop= DlisRead->Frame[i].Index_Stop;
				}
				else
				{
					FidWrite->FidChannel[NumLog].DimInfo[0].Start=DlisRead->Frame[i].Index_Stop;
					FidWrite->FidChannel[NumLog].DimInfo[0].Stop=DlisRead->Frame[i].Index_Start;
				}
				//�����������һά(����ֵ)����
				sprintf(FidWrite->FidChannel[NumLog].DimInfo[kD].Name,"%s",FidWrite->FidChannel[NumLog].CurveName);
				if(j==0)
					s.Format("%s",DlisRead->Frame[i].IndexUnits); //��������
				else
					s.Format("%s",DlisRead->Channel[index].Units); //����������
				s.TrimLeft();
				s.TrimRight();
				s.Remove(' ');
				if(s.IsEmpty())
					strcpy(FidWrite->FidChannel[NumLog].DimInfo[kD].Unit,"none");
				else
					strcpy(FidWrite->FidChannel[NumLog].DimInfo[kD].Unit,(const char*)s);
				FidWrite->FidChannel[NumLog].DimInfo[kD].RepCode=PcCodeToFid(DlisRead->Channel[index].PcCode);
				if(kD==1)
				{	//�������ߣ�FID�б�ʾ��һ�������Ϊ4�ֽڸ���
					FidWrite->FidChannel[NumLog].DimInfo[kD].RepCode=FID_REPR_FLOAT;
				}
				else
				{
					FidWrite->FidChannel[NumLog].DimInfo[kD].RepCode=PcCodeToFid(DlisRead->Channel[index].PcCode);
					//FidWrite->FidChannel[NumLog].DimInfo[kD].RepCode=FID_REPR_SHORT;
				}
				FidWrite->FidChannel[NumLog].DimInfo[kD].CodeLen=WisCodeLen(FidWrite->FidChannel[NumLog].DimInfo[kD].RepCode);
				FidWrite->FidChannel[NumLog].DimInfo[kD].Nps=DlisRead->Channel[index].Dimension[0];
				FidWrite->FidChannel[NumLog].DimInfo[kD].Npw=FidWrite->FidChannel[NumLog].DimInfo[kD].Nps;
				FidWrite->FidChannel[NumLog].DimInfo[kD].Start=0;
				FidWrite->FidChannel[NumLog].DimInfo[kD].Stop=100;
				FidWrite->FidChannel[NumLog].DimInfo[kD].Rlev=0;
				//���������м��ά��Ϣ�����ڳ���kDΪ1,������γ��򲻻����У�
				for(int k=1;k<kD;k++)
				{
					//����FID�ļ�ֻ�����˲������ߣ����Զ��������߽�������������
					//int kx=DlisRead->Channel[index].NumDim-k; //���Ϊ��
					int kx=k-1; //
					if(kD==2)
						strcpy(FidWrite->FidChannel[NumLog].DimInfo[k].Name,"T");//���ڲ������ߣ�
					else
					{
						strcpy(FidWrite->FidChannel[NumLog].DimInfo[k].Name,DlisRead->Channel[index].AxisID[kx]); //����
						if(strlen(FidWrite->FidChannel[NumLog].DimInfo[k].Name)==0)
							sprintf(FidWrite->FidChannel[NumLog].DimInfo[k].Name,"ORDINAL");
					}
					strcpy(FidWrite->FidChannel[NumLog].DimInfo[k].Unit,DlisRead->Channel[index].AxisUnits[kx]); //��ĵ�λ
					if(strlen(FidWrite->FidChannel[NumLog].DimInfo[k].Unit)==0)
						strcpy(FidWrite->FidChannel[NumLog].DimInfo[k].Unit,"none");
					FidWrite->FidChannel[NumLog].DimInfo[k].RepCode=FID_REPR_FLOAT;
					FidWrite->FidChannel[NumLog].DimInfo[k].CodeLen=WisCodeLen(FID_REPR_FLOAT);
					FidWrite->FidChannel[NumLog].DimInfo[k].Nps=DlisRead->Channel[index].Dimension[kx];
					FidWrite->FidChannel[NumLog].DimInfo[k].Npw=0;
					FidWrite->FidChannel[NumLog].DimInfo[k].Rlev=DlisRead->Channel[index].AxisStep[kx];
					FidWrite->FidChannel[NumLog].DimInfo[k].Start=DlisRead->Channel[index].AxisStart[kx];
					FidWrite->FidChannel[NumLog].DimInfo[k].Stop=FidWrite->FidChannel[NumLog].DimInfo[k].Start+(FidWrite->FidChannel[NumLog].DimInfo[k].Nps-1)*FidWrite->FidChannel[NumLog].DimInfo[k].Rlev;
				}
			}
		}
	}
}

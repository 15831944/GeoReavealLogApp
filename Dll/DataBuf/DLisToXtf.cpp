// DLisToXtf.cpp: implementation of the CDLisToXtf class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include <stdlib.h>
#include "_Comext.h"
#include "DLisToXtf.h"
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

CDLisToXtf::CDLisToXtf()
{

}

CDLisToXtf::~CDLisToXtf()
{

}
bool CDLisToXtf::DlisToXtf(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
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
	XtfWrite=new CXtfWrite ; //�����
	bool bResult=Transform();
	delete pGuage;  //ɾ��������
	delete XtfWrite; XtfWrite=NULL;
	delete DlisRead; DlisRead=NULL;
	
	NewCurveName.RemoveAll();

	return bResult;
}
bool CDLisToXtf::Transform()
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
			bool bResult=LogicalToXtf(WellName,nf);
			if(sp_Break_Status) break; //�ж�
			//���ת�������Ϣ
			if(bResult)
				XtfWrite->PrintHead(fp);
			//�����Ϣ
		}
		DlisRead->Close(); //�ر������ļ�
	}
	return !sp_Break_Status;
}
bool CDLisToXtf::LogicalToXtf(CString WellName,int nf)
{
	CString s;
	//�������Ŀ¼�������ļ���ȷ������ļ���
	sOutFileName.Format ("%s\\%s.xtf",m_sObjectPath,WellName);
	ModiFileName(sOutFileName); //�������������Ƿ��޸��ļ���
	SetXtfHeadFromDlisHead();
	if(NewCurveName.GetSize()==0)
	{
		fprintf(fp,"\n!!!!û��Ҫ���������\n");
		return false;
	}
	//�����Ϣ
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)sOutFileName);
	if(!XtfWrite->Open(sOutFileName)) return false;
	XtfWrite->InitXtfHeader();
	SetXtfHeaderFromDlis();
	XtfWrite->WriteXtfHeader();
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
			int nChannel=DlisRead->Frame[i].ChannelID[j];
			SetXtfCurveHead(NumLog,nChannel,i,j); //��������ͷ����
			XtfWrite->WriteCurveHeader(NumLog);
			int NumPoint=DlisRead->Frame[i].noFrame;
			if(XtfWrite->XtfHead.CurveAttrib[NumLog].CurType==1)
			{
				float *buf=new float[NumPoint];
				DlisRead->ReadCurve(i,j,NumPoint,buf);
				XtfWrite->WriteGenData(NumPoint,buf);
				delete []buf; buf=NULL;
			}
			else
			{	
				//���λ���������
				int Count=DlisRead->Channel[nChannel].Sample;
				float *buf=new float[Count];
				for(int k=0;k<Count;k++)buf[k]=0; //��ֵ
				for(k=0;k<NumPoint;k++)
				{
					float depth=XtfWrite->XtfHead.Sdep[NumLog]+k*XtfWrite->XtfHead.Level[NumLog];
					DlisRead->ReadWaveArray(i,j,depth,buf);
					XtfWrite->WriteWaveArray(depth,buf);
				}
				delete []buf; buf=NULL;
			}
		}
	}
	XtfWrite->Close();
	
	return true;
}
void CDLisToXtf::SetXtfHeadFromDlisHead()
{
	CString s;
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
}
bool CDLisToXtf::DlisCurvesFilter()
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
					if(strlen(DlisRead->Channel[k].Name)>8)
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
void CDLisToXtf::SelectChannelFromWin()
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
	DlisCurDlg.m_nMaxLen=8;
	DlisCurDlg.m_nMaxLog=512;
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

BYTE CDLisToXtf::DlisRepCToXtf(BYTE RepCode)
{
	BYTE XtfRepC=XTF_REPR_FLOAT;
	switch(RepCode)
	{
	case DLIS_REPC_FSHORT:
	case DLIS_REPC_FSINGL:
	case DLIS_REPC_FSING1:
	case DLIS_REPC_FSING2:
	case DLIS_REPC_ISINGL:
	case DLIS_REPC_VSINGL:
	case DLIS_REPC_CSINGL:
		XtfRepC=XTF_REPR_FLOAT;
		break;
	case DLIS_REPC_FDOUBL:
	case DLIS_REPC_FDOUB1:
	case DLIS_REPC_FDOUB2:
	case DLIS_REPC_CDOUBL:
		XtfRepC=XTF_REPR_DOUBLE;
		break;
	case DLIS_REPC_SSHORT:
		XtfRepC=XTF_REPR_CHAR;
		break;
	case DLIS_REPC_SNORM:
		XtfRepC=XTF_REPR_INT2;
		break;
	case DLIS_REPC_SLONG:
		XtfRepC=XTF_REPR_INT4;
		break;
	case DLIS_REPC_USHORT:
		XtfRepC=XTF_REPR_UCHAR;
		break;
	case DLIS_REPC_UNORM:
		XtfRepC=XTF_REPR_UINT2;
		break;
	case DLIS_REPC_ULONG:
	case DLIS_REPC_UVARI:
		XtfRepC=XTF_REPR_UINT4;
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
	return XtfRepC;
}
void CDLisToXtf::FindFrameAndCurveIndex(const int index,int &nFrame,int &nCurve)
{
	int Num=-1;
	nFrame=-1;
	nCurve=-1;
	for(int i=0;i<DlisRead->NumFrame;i++)
	{
		for(int j=0;j<DlisRead->Frame[i].NumCurve;j++)
		{
			if(DlisRead->Frame[i].bDecode[j])
			{
				Num++;
				if(index==Num)
				{
					nFrame=i;
					nCurve=j;
					return;
				}
			}
		}
	}
	return;
}
void CDLisToXtf::SetXtfHeaderFromDlis()
{
	CString s;
	char str[256];
	sprintf(str,"%-80.80s",DlisRead->Origin.WellName); //����
	memcpy(XtfWrite->XtfHead.Wellname,str,80);
	sprintf(str,"%-80.80s",DlisRead->Origin.FieldName); //������
	memcpy(XtfWrite->XtfHead.Fieldname,str,80); 
	sprintf(str,"%-80.80s",DlisRead->Origin.Company); //��˾��
	memcpy(XtfWrite->XtfHead.Companyname,str,80);
	memset(XtfWrite->XtfHead.Welllocation,' ',40);
	XtfWrite->XtfHead.Metricflag=0; //���Ƶ�λ
	XtfWrite->XtfHead.FileRecord=8;  //�ļ���¼��(��Сֵ)
	XtfWrite->XtfHead.isnumcv=NewCurveName.GetSize();  //��������
	if(XtfWrite->XtfHead.isnumcv==0) return ;

	XtfWrite->XtfHead.wttop=0;  //���ߵĶ������
	XtfWrite->XtfHead.wtbottom=0;  //���ߵĵײ����
	XtfWrite->XtfHead.wtlevel=0;  //���ߵĲ������
	int NumDepCurve=0; //��Ȳ���������
	for(int i=0;i<NewCurveName.GetSize();i++)
	{
		int nFrame=-1; //��i����������֡��
		int nCurve=-1; //��i����������֡���������
		FindFrameAndCurveIndex(i,nFrame,nCurve);
		int nChannel=DlisRead->Frame[nFrame].ChannelID[nCurve];
		sprintf(str,"%-8.8s",(const char*)(NewCurveName.GetAt(i)));
		memcpy(XtfWrite->XtfHead.CurveName[i],str,8);
		sprintf(str,"%-8.8s",DlisRead->Channel[nChannel].Units);
		memcpy(XtfWrite->XtfHead.Units[i],str,8);

		XtfWrite->XtfHead.LevelNumber[i]=DlisRead->Frame[nFrame].noFrame;
		XtfWrite->XtfHead.Level[i]=float(fabs(DlisRead->Frame[nFrame].Index_Step));
		if(DlisRead->Frame[nFrame].Index_Start<DlisRead->Frame[nFrame].Index_Stop)
		{
			XtfWrite->XtfHead.Sdep[i]=DlisRead->Frame[nFrame].Index_Start;
			XtfWrite->XtfHead.Edep[i]=DlisRead->Frame[nFrame].Index_Stop;
		}
		else
		{
			XtfWrite->XtfHead.Sdep[i]=DlisRead->Frame[nFrame].Index_Stop;
			XtfWrite->XtfHead.Edep[i]=DlisRead->Frame[nFrame].Index_Start;	
		}
		//��Ȳ�������
		NumDepCurve++;
		if(NumDepCurve==1)
		{
			XtfWrite->XtfHead.wttop=XtfWrite->XtfHead.Sdep[i];
			XtfWrite->XtfHead.wtbottom=XtfWrite->XtfHead.Edep[i];
			XtfWrite->XtfHead.wtlevel=XtfWrite->XtfHead.Level[i];
		}
		else
		{
			if(XtfWrite->XtfHead.wttop>XtfWrite->XtfHead.Sdep[i])
			{
				XtfWrite->XtfHead.wttop=XtfWrite->XtfHead.Sdep[i];
			}
			if(XtfWrite->XtfHead.wtbottom<XtfWrite->XtfHead.Edep[i])
			{
				XtfWrite->XtfHead.wtbottom=XtfWrite->XtfHead.Edep[i];
			}
			if(XtfWrite->XtfHead.wtlevel>XtfWrite->XtfHead.Level[i])
			{
				XtfWrite->XtfHead.wtlevel=XtfWrite->XtfHead.Level[i];
			}
		}
		XtfWrite->XtfHead.ndimension[i]=DlisRead->Channel[nChannel].NumDim;
		XtfWrite->XtfHead.idimen1[i]=DlisRead->Channel[nChannel].Dimension[0]; //���ߵ�һά�ĵ�Ԫ��
		XtfWrite->XtfHead.idimen2[i]=DlisRead->Channel[nChannel].Dimension[1]; //���ߵڶ�ά�ĵ�Ԫ��
		XtfWrite->XtfHead.idimen3[i]=DlisRead->Channel[nChannel].Dimension[2]; //���ߵ���ά�ĵ�Ԫ��
		if(DlisRead->Channel[nChannel].Sample==1)
		{
			XtfWrite->XtfHead.CurveAttrib[i].CurType=1;//��������(1-3)������
		}
		else if(DlisRead->Channel[nChannel].NumDim==1)
		{
			XtfWrite->XtfHead.CurveAttrib[i].CurType=2;//��������(1-3)������
		}
		else
		{
			XtfWrite->XtfHead.CurveAttrib[i].CurType=3;//��������(1-3)������
		}
		XtfWrite->XtfHead.CurveAttrib[i].RepCode=DlisRepCToXtf(DlisRead->Channel[nChannel].RepCode); //��������(1-12)
		XtfWrite->XtfHead.CurveAttrib[i].HorType=1;   //ˮƽ����
		XtfWrite->XtfHead.CurveAttrib[i].VerType=1;  //��ֱ����
	
		int NumItem=XtfWrite->XtfHead.idimen1[i];
		if(XtfWrite->XtfHead.ndimension[i]>1)
		{
			NumItem*=XtfWrite->XtfHead.idimen2[i];
		}
		if(XtfWrite->XtfHead.ndimension[i]>2)
		{
			NumItem*=XtfWrite->XtfHead.idimen3[i];
		}
		short CodeLen=XtfWrite->XtfRepCodeToLen(XtfWrite->XtfHead.CurveAttrib[i].RepCode);
		int NumBytes=NumItem*XtfWrite->XtfHead.LevelNumber[i]*CodeLen/8;
		int NumRec=(NumBytes-1)/4096+2; //�����ߵ����ݼ�¼����������ͷ
		XtfWrite->XtfHead.StartOffset[i]=XtfWrite->XtfHead.FileRecord+1; 
		XtfWrite->XtfHead.FileRecord+=NumRec;
	}
	XtfWrite->XtfHead.firstrecord=1;     //λͼ�е������¼
	XtfWrite->XtfHead.lastrecord=32576;      //λͼ�е����һ����¼
	XtfWrite->XtfHead.prerecord=0;       //ǰһ��λͼ�ļ�¼��
	XtfWrite->XtfHead.thisrecord=2;      //��ǰλͼ��¼��
	if(XtfWrite->XtfHead.FileRecord<XtfWrite->XtfHead.lastrecord)
	{	
		XtfWrite->XtfHead.bitmapfullflag=0;  //λͼδ��
		XtfWrite->XtfHead.nextrecord=0;      //��һ��λͼ�ļ�¼��
		DWORD Num1=XtfWrite->XtfHead.FileRecord/8;
		DWORD Num2=XtfWrite->XtfHead.FileRecord%8;
		for(DWORD i=0;i<Num1;i++)
		{
			XtfWrite->XtfHead.Datamap[i]=0XFF;  //λͼ
		}
		XtfWrite->XtfHead.Datamap[Num1]=0;
		for(i=0;i<Num2;i++)
		{
			XtfWrite->XtfHead.Datamap[Num1]=(XtfWrite->XtfHead.Datamap[Num1]>>1)|0x80;
		}
	}
	else
	{	
		XtfWrite->XtfHead.bitmapfullflag=1;  //λͼ����
		XtfWrite->XtfHead.nextrecord=32576;  //��һ��λͼ�ļ�¼��
		for(int i=0;i<4072;i++)
		{
			XtfWrite->XtfHead.Datamap[i]=0XFF;
		}
	}
}

void CDLisToXtf::SetXtfCurveHead(int Index,int nChannel,int nFrame,int nCurve)
{
	CString s;
	sprintf(XtfWrite->XtfCurve.Name,"%-8.8s",NewCurveName.GetAt(Index));//����������������ĸ��ͷ
	sprintf(XtfWrite->XtfCurve.Units,"%-8.8s",DlisRead->Channel[nChannel].Units);

	sprintf(XtfWrite->XtfCurve.Comment,"%-24.24s"," "); //�����߱�ע�ֶΣ�����AIF�ļ�����ı�ע��
	sprintf(XtfWrite->XtfCurve.ServiceCompany,"%-8.8s","SLC"); //��˾��
	sprintf(XtfWrite->XtfCurve.ToolType,"%-8.8s",""); //��������
	sprintf(XtfWrite->XtfCurve.CurveClassType,"%-8.8s"," ");  //�������ͣ��硰WAVEFORM������DIPLOG����
	if(XtfWrite->XtfHead.CurveAttrib[Index].CurType==2)
	{
		sprintf(XtfWrite->XtfCurve.CurveClassType,"%-8.8s","WAVEFORM");  //�������ͣ��硰WAVEFORM������DIPLOG����
	}
	else if(XtfWrite->XtfHead.CurveAttrib[Index].CurType==3)
	{
		sprintf(XtfWrite->XtfCurve.CurveClassType,"%-8.8s","ARRAY");  //�������ͣ��硰WAVEFORM������DIPLOG����
	}
	sprintf(XtfWrite->XtfCurve.DepthUnits,"%-8.8s",DlisRead->Frame[nFrame].IndexUnits); //��ȵ�λ
	if(XtfWrite->XtfHead.CurveAttrib[Index].CurType>1)
	{
		sprintf(XtfWrite->XtfCurve.TimeIncrementUnits,"%-8.8s",DlisRead->Channel[nChannel].AxisUnits[0]); //�������ߵ�ʱ��������λ
		sprintf(XtfWrite->XtfCurve.StartTimeUnits,"%-8.8s",XtfWrite->XtfCurve.TimeIncrementUnits);  //��ʼʱ�䵥λ,���������й̶��Ϳɱ����ʼʱ��
	}
	else
	{
		sprintf(XtfWrite->XtfCurve.TimeIncrementUnits,"%-8.8s"," ");
		sprintf(XtfWrite->XtfCurve.StartTimeUnits,"%-8.8s"," "); 
	}
	sprintf(XtfWrite->XtfCurve.RawTapeHeaderMnemonic,"%-8.8s"," ");  //ԭʼ����ͷ���Ƿ�
	sprintf(XtfWrite->XtfCurve.UnitsOfStationary,"%-8.8s",XtfWrite->XtfCurve.StartTimeUnits); //��̬��ȵ�λ
	sprintf(XtfWrite->XtfCurve.CorrelogramParamterUnits,"%-8.8s"," ");  //���ͼ������λ
	sprintf(XtfWrite->XtfCurve.CbilOrientationMnemonic,"%-8.8s"," ");  //CBIL��λ���Ƿ�
	sprintf(XtfWrite->XtfCurve.LoggingDirection,"%-8.8s","down"); //�⾮����,ȡֵΪ��up������down����
	sprintf(XtfWrite->XtfCurve.AifCurveName,"%-24.24s"," ");//AIF�������Ͱ汾
	sprintf(XtfWrite->XtfCurve.OCT,"%-8.8s"," "); //���������ߵ�OCT
	sprintf(XtfWrite->XtfCurve.ToolNames,"%-48.48s"," "); //8�����ַ���������
	sprintf(XtfWrite->XtfCurve.strunused,"%-8.8s"," ");  //δ��
	sprintf(XtfWrite->XtfCurve.strunused2,"%-8.8s"," "); //δ��
	sprintf(XtfWrite->XtfCurve.LisName,"%-4.4s",XtfWrite->XtfCurve.Name);  //LIS������
	sprintf(XtfWrite->XtfCurve.strunused3,"%-4.4s"," "); //δ��
	sprintf(XtfWrite->XtfCurve.CreationRoutineName,"%-16.16s"," "); //ʮ���ַ��Ĵ������̺Ͱ汾
	sprintf(XtfWrite->XtfCurve.ModifiedRoutineName,"%-16.16s",XtfWrite->XtfCurve.CreationRoutineName); //ʮ���ַ����޸����̺Ͱ汾
	sprintf(XtfWrite->XtfCurve.strunused4,"%-28.28s"," "); //δ��
	sprintf(XtfWrite->XtfCurve.LongName,"%-20.20s"," ");  //�ӳ��˵�������
	sprintf(XtfWrite->XtfCurve.strunused5,"%-712.712s"," ");  //δ��

	XtfWrite->XtfCurve.deptop=XtfWrite->XtfHead.Sdep[Index];
	XtfWrite->XtfCurve.rlevcv=XtfWrite->XtfHead.Level[Index];
	XtfWrite->XtfCurve.depbot=XtfWrite->XtfHead.Edep[Index];
	XtfWrite->XtfCurve.curvmin=0; //������Сֵ
	XtfWrite->XtfCurve.curvmax=0; //�������ֵ
	XtfWrite->XtfCurve.curvavg=0; //����ƽ��ֵ
	if(XtfWrite->XtfHead.CurveAttrib[Index].CurType>1)
	{
		XtfWrite->XtfCurve.timeinc=DlisRead->Channel[nChannel].AxisStep[0];; //ʱ�����������������
		XtfWrite->XtfCurve.starttm=DlisRead->Channel[nChannel].AxisStart[0]; //������������ʼʱ��
	}
	else
	{
		XtfWrite->XtfCurve.timeinc=0;
		XtfWrite->XtfCurve.starttm=0;
	}
	XtfWrite->XtfCurve.stadepth=0; //��̬�������
	XtfWrite->XtfCurve.stddev=0;  //��׼����
	XtfWrite->XtfCurve.cpwinbeg=0; //���ͼ������Window begin��
	XtfWrite->XtfCurve.cpwinstp=0; //���ͼ������Window step��
	XtfWrite->XtfCurve.cpwinlen=0; //���ͼ������Window length��
	XtfWrite->XtfCurve.cbilodeg=0; //CBIL��λ�Ķ���
	XtfWrite->XtfCurve.IntervalTR=0;  //���������������ľ���
	XtfWrite->XtfCurve.IntervalRR=0;  //���������������ľ���
	XtfWrite->XtfCurve.RawOffset=0;   //ԭʼ����ƫת
	XtfWrite->XtfCurve.TimeDelay=0;   //ʱ���ӳ�
	XtfWrite->XtfCurve.Angle=0;       //�ο��۵������б۵�ƽ��
	XtfWrite->XtfCurve.XPad=0;    //�����ϴ�������Xλ��
	XtfWrite->XtfCurve.YPad=0;    //�����ϴ�������Yλ��
	for(int i=0;i<6;i++)
	{
		XtfWrite->XtfCurve.LossMinor[i]=0; //�μ���ʧ����ֵ
	}
	for(i=0;i<101;i++)
	{
		XtfWrite->XtfCurve.funused[i]=0; //δ��
	}

	XtfWrite->XtfCurve.dblinit=-9999.0;  //˫���ȳ�ʼֵ
	XtfWrite->XtfCurve.dbtopdep=XtfWrite->XtfCurve.deptop; //˫���ȶ������
	XtfWrite->XtfCurve.dbbotdep=XtfWrite->XtfCurve.depbot; //˫���ȵײ����
	XtfWrite->XtfCurve.dbrlevel=XtfWrite->XtfCurve.rlevcv; //˫���Ȳ������
	for(i=0;i<6;i++)
	{
		XtfWrite->XtfCurve.smvalsdb[i]=0; //Array pf 6 secondary Missing Datum Value
	}
	XtfWrite->XtfCurve.idxtop2=0; //�μ���������ͷ��
	XtfWrite->XtfCurve.idxbot2=0; //�μ����������ײ�
	XtfWrite->XtfCurve.idxrlev2=0; //�μ������������
	for(i=0;i<51;i++)
	{
		XtfWrite->XtfCurve.dunused[i]=0; //δ��
	}

	CTime t=CTime::GetCurrentTime(); //ȡ��ǰϵͳ���ں�ʱ��
	int iyear=t.GetYear();
	int imonth=t.GetMonth();
	int iday=t.GetDay();
	int ihour=t.GetHour();
	int iminute=t.GetMinute();
	int isecond=t.GetSecond();

	XtfWrite->XtfCurve.EncodedCreationDate=(iyear<<16)|(imonth<<8)|iday;  //�������ڱ���
	XtfWrite->XtfCurve.EncodedCreationTime=(ihour<<24)|(iminute<<16)|isecond;  //����ʱ�����
	XtfWrite->XtfCurve.EncodedLastAccessDate=XtfWrite->XtfCurve.EncodedCreationDate;  //����ȡ���ڱ���
	XtfWrite->XtfCurve.EncodedLastAccessTime=XtfWrite->XtfCurve.EncodedCreationTime;  //����ȡʱ�����

	XtfWrite->XtfCurve.NumberOfCurveHeaderrecords=1;  //����ͷ��¼��
	//�������һ�����ݼ�¼���ں������

	XtfWrite->XtfCurve.DatumInitializationVariable=-9999;  //���ݳ�ʼ������
	XtfWrite->XtfCurve.NumberOfLevelsForTheCurve=XtfWrite->XtfHead.LevelNumber[Index];  //���ߵĲ�����
	XtfWrite->XtfCurve.CurrentEncodedDateSignature=0;  //��ǰ�������ڱ��
	XtfWrite->XtfCurve.CurrentEncodedTimeSignature=0;  //��ǰ�������ڱ��
	for(i=0;i<6;i++)
	{
		XtfWrite->XtfCurve.LossMinor2[i]=0; //6���μ���ʧ����ֵ������
	}
	for(i=0;i<2;i++)
	{
		XtfWrite->XtfCurve.Arrays2[i]=0;  //��ֵ���У�����ṹ�����߶������ĩ��¼
		XtfWrite->XtfCurve.Arrays3[i]=0;  //��ֵ���У�����ṹ����������������ĩ��¼
	}
	for(i=0;i<108;i++)
	{
		XtfWrite->XtfCurve.iunused[i]=0;  //δ��
	}
	XtfWrite->XtfCurve.SurvlibMajor=4;  //SURVLIB��Ҫ��
	XtfWrite->XtfCurve.SurvlibMinor=14; //SURVLIB��Ҫ��
	XtfWrite->XtfCurve.Ihcurv=Index+1;  //����ʱ�����߾��
	XtfWrite->XtfCurve.Ictype=XtfWrite->XtfHead.CurveAttrib[Index].CurType;  //��������:1-����,2-����,3-����
	XtfWrite->XtfCurve.RepCode=XtfWrite->XtfHead.CurveAttrib[Index].RepCode;
	XtfWrite->XtfCurve.Ivtype=1;  //��ֱ��������
	XtfWrite->XtfCurve.Ihtype=1;  //ˮƽ��������
	XtfWrite->XtfCurve.Ndims=XtfWrite->XtfHead.ndimension[Index];   //���ߵ�ά��
	//���õ�iά�ĵ�Ԫ��
	XtfWrite->XtfCurve.Idims[0]=XtfWrite->XtfHead.idimen1[Index];
	XtfWrite->XtfCurve.Idims[1]=XtfWrite->XtfHead.idimen2[Index];
	XtfWrite->XtfCurve.Idims[2]=XtfWrite->XtfHead.idimen3[Index];
	for(i=XtfWrite->XtfCurve.Ndims;i<7;i++)
	{
		XtfWrite->XtfCurve.Idims[i]=0;  //��iά�ĵ�Ԫ��
	}
	XtfWrite->XtfCurve.I2init=-32767;  //INT*2�����ĳ�ʼֵ
	XtfWrite->XtfCurve.Intflfix=XtfWrite->XtfRepCodeToIntFlFix(XtfWrite->XtfCurve.RepCode);
	XtfWrite->XtfCurve.Isign=XtfWrite->XtfRepCodeToSign(XtfWrite->XtfCurve.RepCode); //�޷��ű��
	XtfWrite->XtfCurve.Numbits=XtfWrite->XtfRepCodeToLen(XtfWrite->XtfCurve.RepCode);
	XtfWrite->XtfCurve.Lisrepcd=0; //��ȡ��ֵ����������ԭʼLIS������ʽ
	XtfWrite->XtfCurve.Lisnsamp=0;  //Nsamp���ͬʱLisrepcd�й�
	XtfWrite->XtfCurve.Lisrcsiz=0;  //LIS�����С���ֽڣ�
	XtfWrite->XtfCurve.Lisnsize=0;  //ÿ������LIS�����ܽڽ���
	XtfWrite->XtfCurve.Istartfv=0;  //0-�̶���������ʼʱ��,1-�ɱ��������ʼʱ��
	XtfWrite->XtfCurve.Igainfv=0;   //��Ϊ0����������������̶�
	XtfWrite->XtfCurve.Igainepx=0;  //���й̶���������л����ߵ�����ָ��
	XtfWrite->XtfCurve.Igainmth=0;  //������������л����ߵ������ʾ��-�̶���ɱ�
	XtfWrite->XtfCurve.Igainapp=0;  //����Ӧ�ñ�־
	XtfWrite->XtfCurve.Lntaphed=0;  //RAWTPHD�ĳ���
	XtfWrite->XtfCurve.Ntools=0;    //������
	XtfWrite->XtfCurve.Idxingrp=0;  //��������
	for(i=0;i<6;i++)
	{
		XtfWrite->XtfCurve.Smvalsi2[i]=0;  //6���μ���ʧ����ֵ������
	}
	for(i=0;i<219;i++)
	{
		XtfWrite->XtfCurve.hunused[i]=0;  //δ��
	}
	for(i=0;i<24;i++)
	{
		XtfWrite->XtfCurve.TransmitterID[i]=0;  //24�ֽڴ���24��������ID��
		XtfWrite->XtfCurve.ReceiverID[i]=0;  //24�ֽڴ���24��������ID��
	}
	for(i=0;i<276;i++)
	{
		XtfWrite->XtfCurve.RawTapeHeaderInfo[i]=0;  //ԭʼ��ͷ��Ϣ
	}
	for(i=0;i<6;i++)
	{
		XtfWrite->XtfCurve.LossMinor3[i]=0;  //6�ֽڴ������ʧ��ֵ������
		XtfWrite->XtfCurve.LossMinor4[i]=0;  //6�ֽڴ������ʧ��ֵ������
	}
	for(i=0;i<687;i++)
	{
		XtfWrite->XtfCurve.bunused[i]=0;   //δ��
	}
	XtfWrite->XtfCurve.SystemCode=5;  //ϵͳ����:1-PC,2-PE,3-VAX,4-IBM Mainframe,5-hp9000/Sun,etc.

	DWORD BlockSize=XtfWrite->XtfHead.LevelNumber[Index]*XtfWrite->XtfCurve.Idims[0]*XtfWrite->XtfCurve.Numbits/8;
	DWORD NumRecord=(BlockSize-1)/4096+2; //����һ������ͷ
	XtfWrite->XtfCurve.RecordNumberOfLastCurvedataRecord=XtfWrite->XtfHead.StartOffset[Index]+NumRecord-1;
}

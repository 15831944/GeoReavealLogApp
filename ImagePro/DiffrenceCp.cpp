// DiffrenceCp.cpp: implementation of the CDiffrenceCp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "flowchart.h"
#include "DiffrenceCp.h"
#include "FlowNode.h"
#include "Gradient.h"
#include "FIDio.h"
#include "Guage.h"
#include "DiffrenceDlg.h"
#include "ImageGeoCorrect.h"

#include "Insert3D.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CDiffrenceCp,CFlowNode, 0)
CDiffrenceCp::CDiffrenceCp()
{
	m_pDoc=NULL;
}

CDiffrenceCp::~CDiffrenceCp()
{

}

CDiffrenceCp::CDiffrenceCp(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
	m_DaltaDep=0.0508f;
	m_szImageType="��̬ͼ��";
}

void CDiffrenceCp::Serialize(CArchive& ar)
{
	CFlowNode::Serialize(ar);	
	if(ar.IsStoring())
	{
		ar<<m_szImageType;
		ar<<m_DaltaDep;
	}
	else
	{
		ar>>m_szImageType;
		ar>>m_DaltaDep;
	}
}

//����ͼ����ά�ؽ���4����λ��ֱ��Ƭ
BOOL CDiffrenceCp::CreateSectionImage(CString InName,CString OutName,CString SectionName)
{
	int DataNum=0;
	CFIDIO* PadI=new CFIDIO(m_szWellName+"."+InName);
	if(PadI->Open())
	{
		m_StProDep=PadI->GetStartDepth();
		m_EnProDep=PadI->GetEndDepth();
		m_Rlev=PadI->GetDepthLevel();

		FIDCONTENT *Content = PadI->GetContent(3);
		if(Content != NULL)
			DataNum=Content->nps;
	}
	delete PadI;
	if(DataNum<1) 
	{
		AfxMessageBox(InName+"�����ߴ���ʧ��!        ");
		return FALSE;
	}
	
	//�������ָʾ��
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetTitle(InName+" ����ͼ����ά�ؽ�...");
	pGuage->ShowWindow(SW_SHOW);
     
	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"PAD","none",REPR_SHORT,4,16,0,0.f,0.f,0.f},
							   {"DATAST","1/ohm",REPR_FLOAT,4,16,16,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;
	m_Content[1].nps=DataNum;                      
	m_Content[1].min=2.54f;                            
	m_Content[1].max=m_Bits*3.1416f;                   
	m_Content[1].rlev = m_Bits;                        

	m_Content[2].nps=DataNum;
	m_Content[2].npw=DataNum;                        
	m_Content[2].min=(float)m_PadNum;                  
	m_Content[2].max=(float)m_ButtonNum;               
	m_Content[2].rlev =0.f;                            

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;
	//��ʼ��4����ֱ��Ƭͼ��
	double Radius=DataNum/2/PI;
	int Num=(int)(Radius*2);
	
	m_Content[1].nps=Num; 
	m_Content[2].nps=Num;
	m_Content[2].npw=Num; 
	CString szStr;
	int i,j,k=0;

	CFIDIO *pFIDW=NULL;
	for(i=0;i<4;i++)
	{
		strcpy(m_Content[2].name,SectionName);
		szStr.Format("%d",i*45);
		strcat(m_Content[2].name,szStr);
		szStr=m_szWellName+"."+SectionName+szStr;
		pFIDW = new CFIDIO(szStr,&m_Index);
		if(!pFIDW->Open())
		{	
			AfxMessageBox(szStr+" ��ʼ����ֱ��Ƭͼ��ʧ��!        ");
			delete pFIDW;
			delete pGuage;
			return FALSE;
		}
		pFIDW->Close();
		delete pFIDW;pFIDW=NULL;
	}
	char s[256],fn[256];
	strcpy(s,m_szWellName);
	strcat(s,".");strcat(s,InName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(!fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		AfxMessageBox(InName+" ������ʧ��!        ");
		delete pGuage;
		return FALSE;
	}

	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	float *Value=new float[DataNum*RecordNum];			//ͼ��ԭʼ����
	//��ȡ������������
	fp.Seek (0L,CFile::begin);
	fp.Read (Value,DataNum*RecordNum*4);
	fp.Close ();	

	CFile *pFile=new CFile[4];
	for(i=0;i<4;i++)
	{
		szStr.Format("%d",i*45);
		szStr=m_szWellName+"."+SectionName+szStr;
		strcpy(s,szStr);
		CFIDIO::GetFileName(s,fn,'D');
		if(!pFile[i].Open(fn,CFile::modeReadWrite|CFile::typeBinary))
		{
			delete pFile;
			delete Value;
			delete pGuage;
			return TRUE;
		}
	}
	//�������ά��ֵ
	//XY����Ϊ����ƽ��XYֵ
	//Z����ȡndy*2����Ȳ�������ȵ�λ��ȡndy����������ȡndy������
	int ndy=5;
	double x,y,z;
	vector<Point3D> input;
	Insert3D* pInterpolater = new Insert3D(DataNum,6);
	float **pData=new float* [Num];//��ֱ��Ƭ����
	for(i=0;i<4;i++)
		pData[i]=new float[Num];
	
	for(j=ndy;j<RecordNum-ndy;j++)
	{
		input.clear();
		for(k=0;k<ndy*2;k++)
		{
			for(i=0;i<DataNum;i++)
			{
				if(Value[DataNum*(j+k-ndy)+i]!=-9999.f)
				{
					x=Radius*sin((i*2*PI)/DataNum);
					y=Radius*cos((i*2*PI)/DataNum);
					z=k-ndy;
					input.push_back(Point3D(x,y,z,Value[DataNum*(j+k-ndy)+i]));
				}
			}
		}
		z=0;
		for(k=0;k<4;k++)
		{
			for(i=0;i<Num;i++)
			{
				x=(Num/2-i)*sin(k*PI/4);
				y=(Num/2-i)*cos(k*PI/4);
				pData[k][i]=pInterpolater->GetInsertV(x,y,z,(Point3D*)&(*input.begin()),(Point3D*)&(*input.end()));
			}
			pFile[k].Write(pData[k],Num*4);
		}
		pGuage->SetDepth(m_StProDep+j*m_Rlev);
		if(pGuage->CheckCancelButton())
			break;
	}
	
	delete pInterpolater;
	for(i=0;i<4;i++)
		delete pData[i];
	delete pData;

	for(i=0;i<4;i++)
		pFile[i].Close ();
	delete pFile;
	delete Value;
	delete pGuage;
	return TRUE;
}

BOOL CDiffrenceCp::Run()
{
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	if(!m_pDoc->m_bAunAll)
	{
		CDiffrenceDlg dlg;
		dlg.m_szStr="��̬ͼ��,��̬�ָ�ͼ��,��̬ͼ��,��̬�ָ�ͼ��";
		dlg.m_szDifName=m_szImageType;
		dlg.m_DaltaDep=m_DaltaDep;
		if(dlg.DoModal()!=IDOK)
			return FALSE;
		m_szImageType=dlg.m_szDifName;
		m_DaltaDep=dlg.m_DaltaDep;
	}

	CString InName,OutName,SectionName;
	if(m_szImageType=="��̬ͼ��")
	{
		InName="STAMI";
		OutName="STA3D";
		SectionName="STACT";
	}
	if(m_szImageType=="��̬�ָ�ͼ��")
	{
		InName="STAPIC";
		OutName="STA3D";
		SectionName="STACT";
	}
	else if(m_szImageType=="��̬�ָ�ͼ��")
	{
		InName="DYNPIC";
		OutName="DYN3D";
		SectionName="DYNCT";
	}
	else
	{
		InName="DYNMI";
		OutName="DYN3D";
		SectionName="DYNCT";
	}
	m_bResult=CreateSectionImage(InName,OutName,SectionName);
	if(m_bResult)
		OnWellRefresh();
	return m_bResult;

	/*else
	{
		if(m_szSeries=="ARI_16")
			InName="ARIGPT";
		else if(m_szSeries=="ARI_16")
			InName="RADSGPT";
		else
			InName="DATAST";
	}*/
/************************************************************************/
/* ���������Ե���                                                     */
/************************************************************************/
	//ģ��߶ȿ�ȼ��������ص�����
	int iTempH,iTempW,iTempCX,iTempCY;
	//ģ��Ԫ��ϵ����Ԫ������
	float Temp[9];
	//��ʼ���Ի������
/*	iTempH=3;
	iTempW=3;
	iTempCX=1;
	iTempCY=1;
	
	//��ģ��Ϊƽ��ģ��
	Temp[0]=1.0;
	Temp[1]=1.0;
	Temp[2]=1.0;
	Temp[3]=-1.0;
	Temp[4]=-2.0;
	Temp[5]=1.0;
	Temp[6]=-1.0;
	Temp[7]=-1.0;
	Temp[8]=1.0;*/
/************************************************************************/
/* ˮƽ��ֱ�����Ե���                                                 */
/************************************************************************/
	//��ģ��Ϊƽ��ģ��
/*	Temp[0]=-1.0;
	Temp[1]=0.0;
	Temp[2]=0.0;
	Temp[3]=0.0;
	Temp[4]=1.0;
	Temp[5]=0.0;
	Temp[6]=0.0;
	Temp[7]=0.0;
	Temp[8]=0.0;
	
	//��ʼ���Ի������
	iTempH=3;
	iTempW=3;
	iTempCX=1;
	iTempCY=1;
	fCoef=0.5;*/
/************************************************************************/
/* ������˹��                                                         */
/************************************************************************/
	//��ģ��Ϊƽ��ģ��
/*	Temp[0]=0.0;
	Temp[1]=-1.0;
	Temp[2]=0.0;
	Temp[3]=-1.0;
	Temp[4]=5.0;
	Temp[5]=-1.0;
	Temp[6]=0.0;
	Temp[7]=-1.0;
	Temp[8]=0.0;
	
	//��ʼ���Ի������
	iTempH=3;
	iTempW=3;
	iTempCX=1;
	iTempCY=1;
	fCoef=1.0;*/
/************************************************************************/
/* Sobel��Ե������Ӷ�ͼ����б�Ե���                                  */
/************************************************************************/
	//����Sobelģ�����
	iTempW=3;
	iTempH=3;
	iTempCX=1;
	iTempCY=1;

	Temp[0]=-1.0;
	Temp[1]=-2.0;
	Temp[2]=-1.0;
	Temp[3]=0.0;
	Temp[4]=0.0;
	Temp[5]=0.0;
	Temp[6]=1.0;
	Temp[7]=2.0;
	Temp[8]=1.0;
	
	//m_bResult=Template(InName,OutName,iTempH,iTempW,iTempCX,iTempCY,Temp);
	//����Sobelģ�����
	Temp[0]=-1.0;
	Temp[1]=0.0;
	Temp[2]=1.0;
	Temp[3]=-2.0;
	Temp[4]=0.0;
	Temp[5]=2.0;
	Temp[6]=-1.0;
	Temp[7]=0.0;
	Temp[8]=1.0;
/************************************************************************/
/* Prewitt��Ե������Ӷ�ͼ����б�Ե���                                */
/************************************************************************/
	Temp[0]=-1.0;
	Temp[1]=0.0;
	Temp[2]=1.0;
	Temp[3]=-1.0;
	Temp[4]=0.0;
	Temp[5]=1.0;
	Temp[6]=-1.0;
	Temp[7]=0.0;
	Temp[8]=1.0;
//	m_bResult=Template(InName,OutName,iTempH,iTempW,iTempCX,iTempCY,Temp);
/************************************************************************/
/* ������˹��Ե���                                                     */
/************************************************************************/
	//ģ��߶ȿ�ȼ��������ص�����
/*	int iTempH,iTempW,iTempCX,iTempCY;
	//ģ��Ԫ��ϵ����Ԫ������
	FLOAT Temp[25];

	//��ʼ���Ի������
	iTempH=5;
	iTempW=5;
	iTempCX=3;
	iTempCY=3;
	
	//��ģ��Ϊƽ��ģ��
	Temp[0]=-2.0;
	Temp[1]=-4.0;
	Temp[2]=-4.0;
	Temp[3]=-4.0;
	Temp[4]=-2.0;
	Temp[5]=-4.0;
	Temp[6]=0.0;
	Temp[7]=8.0;
	Temp[8]=0.0;
	Temp[9]=-4.0;
	Temp[10]=-4.0;
	Temp[11]=8.0;
	Temp[12]=24.0;
	Temp[13]=8.0;
	Temp[14]=-4.0;
	Temp[15]=-4.0;
	Temp[16]=0.0;
	Temp[17]=8.0;
	Temp[18]=0.0;
	Temp[19]=-4.0;
	Temp[20]=-2.0;
	Temp[21]=-4.0;
	Temp[22]=-4.0;
	Temp[23]=-4.0;
	Temp[23]=-2.0;
*/
//	m_bResult=Template(InName,OutName,iTempH,iTempW,iTempCX,iTempCY,Temp);
/************************************************************************/
/* ��ֵ�˲�                                                             */
/************************************************************************/	
	//��ʼ���Ի������
/*	iTempH=5;
	iTempW=5;
	iTempCX=2;
	iTempCY=2;
	m_bResult=MedianFilter(InName,"MEDIAN",iTempH,iTempW,iTempCX,iTempCY);*/
/************************************************************************/
/* ͼ��ƽ��                                                             */
/************************************************************************/	
	//��ģ��Ϊƽ��ģ��
/*	Temp[0]=1.0;
	Temp[1]=2.0;
	Temp[2]=1.0;
	Temp[3]=2.0;
	Temp[4]=4.0;
	Temp[5]=2.0;
	Temp[6]=1.0;
	Temp[7]=2.0;
	Temp[8]=1.0;

	//��ʼ���Ի������
	iTempH=3;
	iTempW=3;
	iTempCX=1;
	iTempCY=1;
	m_bResult=Template("MEDIAN",OutName,iTempH,iTempW,iTempCX,iTempCY,Temp);*/
/************************************************************************/
/* ̽�ⷨ�߽����                                                       */
/************************************************************************/	
//	m_bResult=TraceEdge(InName,OutName);
/************************************************************************/
/* k-��ֵ���෨ͼ��ָ�                                                 */
/************************************************************************/	
	m_bResult=KAveIncise(InName,OutName,4);
//	m_bResult=DiffrencePro(InName,OutName);
	CImageGeoCorrect Edge("΢��ͼ�񼸺�У��",FALSE);
	Edge.m_szImageType ="΢��ͼ��";
	Edge.InitPara(m_pDoc);
	Edge.m_szInName="DIFF";
	Edge.m_szOutName="DIFIMG";
	Edge.Run();
	return m_bResult;
}

void CDiffrenceCp::InitPara(CFlowChartDoc* pDoc)
{
	POSITION pos = pDoc->m_ParentList.GetHeadPosition();
	while (pos != NULL)
	{
		CFlowNode *pObj = (CFlowNode*)pDoc->m_ParentList.GetNext(pos);
		if (pObj != NULL) 
		{		
			if(pObj->IsKindOf(RUNTIME_CLASS(CImageCreat)))
			{
				m_szBufPath=((CImageCreat*)pObj)->m_szBufPath;
				m_szWellName=((CImageCreat*)pObj)->m_szWellName;
				m_szSeries=((CImageCreat*)pObj)->m_szSeries;
				m_PadNum=((CImageCreat*)pObj)->m_PadNum; 
				m_ButtonNum=((CImageCreat*)pObj)->m_ButtonNum; 
				m_ButtonMid=((CImageCreat*)pObj)->m_ButtonMid;
				m_DataNum=((CImageCreat*)pObj)->m_DataNum;
				m_CalNum=((CImageCreat*)pObj)->m_CalNum;
				m_StProDep=((CImageCreat*)pObj)->m_StProDep;
				m_EnProDep=((CImageCreat*)pObj)->m_EnProDep;
				m_Rlev=((CImageCreat*)pObj)->m_Rlev;
				m_WinLongth=((CImageCreat*)pObj)->m_WinLongth;          
				m_Bits=((CImageCreat*)pObj)->m_Bits; 
			}
		}
	}
	m_pDoc=pDoc;
}

BOOL CDiffrenceCp::DiffrencePro(CString InName,CString OutName)
{
	int m=(int)(m_DaltaDep/m_Rlev);
	if(m<1) return FALSE;
	CFIDIO* pFIDI = new CFIDIO(m_szBufPath+"."+InName);
	if(!pFIDI->Open())
	{
		delete pFIDI;return FALSE;
	}
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	
	//�������ָʾ��
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetTitle(m_szDifName+"΢�ִ���...");
	pGuage->ShowWindow(SW_SHOW);
	float dep=m_StProDep;;
	BOOL bUserBreak=FALSE;

	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"PAD","none",REPR_SHORT,4,16,0,0.f,0.f,0.f},
							   {"DATAST","1/ohm",REPR_FLOAT,4,16,16,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;
	m_Content[1].nps=m_DataNum;                        
	m_Content[1].min=2.54f;                            
	m_Content[1].max=m_Bits*3.1416f;                   
	m_Content[1].rlev = m_Bits;                        

	m_Content[2].nps=m_DataNum;
	m_Content[2].npw=m_DataNum;                        
	m_Content[2].min=(float)m_PadNum;                  
	m_Content[2].max=(float)m_ButtonNum;               
	m_Content[2].rlev =0.f;                            
	strcpy(m_Content[2].name,OutName);

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;

	CFIDIO* pFIDW = NULL;
	int i,j,k;
	float ScaleMin=32767.f;
	float ScaleMax=-32767;
	pFIDW = new CFIDIO(m_szBufPath+"."+OutName,&m_Index);
	if(pFIDW->Open())
	{				
		i=0;
		float *Value=NULL;	
		float *Value0=NULL;
		Value=new float[m_DataNum+10];
		Value0=new float[m_DataNum*m];
		pFIDI->SeekToDepth(m_StProDep);
		pFIDW->SeekToDepth(m_StProDep);
		pFIDI->Read(3,m_DataNum*m,Value0);
		pFIDI->SeekToDepth(m_StProDep+m_Rlev*m);
		i=m;
		float fx,fy;
		while(i<RecordNum)
		{
			pFIDI->Read(3,m_DataNum,Value);
			pFIDI->Next();
			for(j=0;j<m_DataNum-m;j++)
			{
				fy=Value[j]-Value0[j];
				fx=Value0[j+m]-Value0[j];
				Value0[j]=(fx+fy)/2.f;
			}
			pFIDW->Write(3,m_DataNum,Value0);
			pFIDW->Next();
			i++;
			dep=m_StProDep+m_Rlev*i;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton())
			{
				bUserBreak=TRUE;
				break;
			}
			for(k=0;k<m-1;k++)
				for(j=0;j<m_DataNum;j++)
					Value0[k*m_DataNum+j]=Value0[(k+1)*m_DataNum+j];
			for(j=0;j<m_DataNum;j++)
				Value0[(m-1)*m_DataNum+j]=Value[j];
		}
		delete Value;delete Value0;
	}
	else bUserBreak=TRUE;
	delete pFIDI;delete pFIDW;

	pGuage->SetTitle("΢��ͼ���׼�̶�...");
	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".");strcat(s,OutName);
	CFIDIO::GetFileName(s,fn,'D');
	
	CFile fp;
	if(fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		int Block=m_DataNum*4;
		float *fVal=NULL;
		fVal=new float[m_DataNum+10];
		i=0;
		while(i<RecordNum)
		{
			fp.Seek (i*Block,CFile::begin);
			fp.Read (fVal,Block);
			for(j=0;j<m_DataNum;j++)
				fVal[j]=(fVal[j]-ScaleMin)/(ScaleMax-ScaleMin)*1024.f;
			fp.Seek (i*Block,CFile::begin);
			fp.Write (fVal,Block);				
			i++;
			dep=m_StProDep+m_Rlev*i;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton())
			{
				bUserBreak=TRUE;
				break;
			}
		}
		delete fVal;
	}
	else bUserBreak=TRUE;
	fp.Close ();
	delete pGuage;	
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

/************************************************************************/
/*Template(LPSTR lpDIB, int iTempH, int iTempW, int iTempCX, int iTempCY, FLOAT *fpTempArray, FLOAT fCoef)
LPSTR lpDIB               //ָ��ԭDIBͼ���ָ��
int iTempH                //ģ��߶�
int iTempW                //ģ����                                    
int iTempCX               //ģ����������X������                           
int iTempCY               //ģ����������Y������
FLOAT *fpTempArray        //ָ��ģ�������ָ��     
FLOAT fCoef               //ģ��ϵ��
����ֵ��BOOL              //�ɹ�����TRUE,���򷵻�FALSE
˵�����ú�����ָ����ģ������ͼ����о������                            */
/************************************************************************/
BOOL CDiffrenceCp::Template(CString InName,CString OutName,int iTempH, int iTempW, int iTempCX, int iTempCY, FLOAT *fpTempArray)
{
	CFIDIO* pFIDI = new CFIDIO(m_szBufPath+"."+InName);
	if(!pFIDI->Open())
	{
		delete pFIDI;return FALSE;
	}
	int lHeight=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	int lWidth=m_DataNum;
	//�������ָʾ��
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetTitle(m_szDifName+"΢�ִ���...");
	pGuage->ShowWindow(SW_SHOW);
	float dep=m_StProDep;;
	BOOL bUserBreak=FALSE;

	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"PAD","none",REPR_SHORT,4,16,0,0.f,0.f,0.f},
							   {"DATAST","1/ohm",REPR_FLOAT,4,16,16,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;
	m_Content[1].nps=m_DataNum;                        
	m_Content[1].min=2.54f;                            
	m_Content[1].max=m_Bits*3.1416f;                   
	m_Content[1].rlev = m_Bits;                        

	m_Content[2].nps=m_DataNum;
	m_Content[2].npw=m_DataNum;                        
	m_Content[2].min=(float)m_PadNum;                  
	m_Content[2].max=(float)m_ButtonNum;               
	m_Content[2].rlev =0.f;                            
	strcpy(m_Content[2].name,OutName);

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;

	CFIDIO* pFIDW = NULL;
	int Num,l,j,k;
	pFIDW = new CFIDIO(m_szBufPath+"."+OutName,&m_Index);
	if(pFIDW->Open())
	{				
		float *Value=NULL;	
		Value=new float[m_DataNum+10];
		pFIDW->SeekToDepth(m_StProDep+m_Rlev*iTempCY);

		float *Value0=NULL;
		Value0=new float[m_DataNum*iTempH];
		pFIDI->SeekToDepth(m_StProDep);
		pFIDI->Read(3,m_DataNum*iTempH,Value0);
		pFIDI->SeekToDepth(m_StProDep+m_Rlev*iTempH);
		Num=iTempH;
		float fm,fCoef;
		fCoef=0.f;
		for(j=0;j<iTempH*iTempW;j++)
			fCoef+=fpTempArray[j];
		while(Num<lHeight)
		{
			pFIDI->Read(3,m_DataNum,Value);
			pFIDI->Next();
			for(j=0;j<m_DataNum;j++)
				Value0[(iTempH-1)*m_DataNum+j]=Value[j];
			
			//��(��ȥ��ԵiTempCX�б���Խ��)
			for(j=iTempCX;j<lWidth-iTempW+iTempCX+1;j++)
			{
				fm=0;
				for(k=0;k<iTempH;k++)
				{
					for(l=0;l<iTempW;l++)
					{
						//ָ��DIB��i-iTempCY+k�е�j-iTempCX+l�����ص�ָ��
						//����ƽ��ֵ
						fm+=Value0[k*m_DataNum+j-iTempCX+l]*fpTempArray[k*iTempW+l];
					}
				}
				//����ϵ��
				fm/=fCoef;
				Value[j]=(float)fabs(fm);
			}

			pFIDW->Write(3,m_DataNum,Value);
			pFIDW->Next();
			Num++;
			dep=m_StProDep+m_Rlev*Num;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton())
			{
				bUserBreak=TRUE;
				break;
			}
			for(k=0;k<iTempH-1;k++)
				for(j=0;j<m_DataNum;j++)
					Value0[k*m_DataNum+j]=Value0[(k+1)*m_DataNum+j];
			
		}
		delete Value;delete Value0;
	}
	else bUserBreak=TRUE;
	delete pFIDI;delete pFIDW;
	delete pGuage;	
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

/************************************************************************/
/*vFindMedianValue(float lpbArray, int iArrayLen)
������float lpbArray                 //ָ��Ҫ��ȡ��ֵ��������ֵ       
int iArrayLen                        //���鳤��
����ֵ��float                        //��������Ԫ�ص���ֵ
˵�����ú�����ð�ݷ���һά����������򣬲���������Ԫ�ص���ֵ            */
/************************************************************************/
float CDiffrenceCp::FindMedianValue(float *lpbArray, int iArrayLen)
{
	//ѭ������
	int i,j;
	//�м����
	float bTemp;
	//��ð�ݷ��������������
	for (j=0;j<iArrayLen;j++)
	{
		for(i=0;i<iArrayLen-j-1;i++)
		{
			if (lpbArray[i]>lpbArray[i+1])
			{
				//����
				bTemp=lpbArray[i];
				lpbArray[i]=lpbArray[i+1];
				lpbArray[i+1]=bTemp;
			}

		}

	}		
	//������ֵ
	if ((iArrayLen&1)>0)
	{
		//������������Ԫ�أ������м�һ��Ԫ��
		bTemp=lpbArray[(iArrayLen+1)/2];
	} 
	else
	{
		//������ż����Ԫ�أ������м�����Ԫ�ص�ƽ��ֵ
		bTemp=(lpbArray[iArrayLen/2]+lpbArray[iArrayLen/2+1])/2;
	}
	//������ֵ
	return bTemp;
}

/************************************************************************/
/*MedianFilter(int iFilterH, int iFilterW, int iFilterCX, int iFilterCY)
����:
int iFilterH                     //�˲����߶�
int iFilterW                     //�˲������
int iFilterCX                    //�˲�������Ԫ��X����
int iFilterCY                    //�˲�������Ԫ��Y����
����ֵ��BOOL                     //�ɹ�����TRUE,���򷵻�FALSE
�ú�����ָ������ֵ�˲�������ͼ�������ֵ�˲�����                        */
/************************************************************************/
BOOL CDiffrenceCp::MedianFilter(CString InName,CString OutName,int iFilterH, int iFilterW, int iFilterCX, int iFilterCY)
{
	CFIDIO* pFIDI = new CFIDIO(m_szBufPath+"."+InName);
	if(!pFIDI->Open())
	{
		delete pFIDI;return FALSE;
	}
	int lHeight=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	int lWidth=m_DataNum;
	//�������ָʾ��
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetTitle(m_szDifName+"΢�ִ���...");
	pGuage->ShowWindow(SW_SHOW);
	float dep=m_StProDep;;
	BOOL bUserBreak=FALSE;

	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"PAD","none",REPR_SHORT,4,16,0,0.f,0.f,0.f},
							   {"DATAST","1/ohm",REPR_FLOAT,4,16,16,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;
	m_Content[1].nps=m_DataNum;                        
	m_Content[1].min=2.54f;                            
	m_Content[1].max=m_Bits*3.1416f;                   
	m_Content[1].rlev = m_Bits;                        

	m_Content[2].nps=m_DataNum;
	m_Content[2].npw=m_DataNum;                        
	m_Content[2].min=(float)m_PadNum;                  
	m_Content[2].max=(float)m_ButtonNum;               
	m_Content[2].rlev =0.f;                            
	strcpy(m_Content[2].name,OutName);

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;

	CFIDIO* pFIDW = NULL;
	int Num,l,j,k;
	pFIDW = new CFIDIO(m_szBufPath+"."+OutName,&m_Index);
	if(pFIDW->Open())
	{				
		float *Value=NULL;	
		Value=new float[m_DataNum+10];
		pFIDW->SeekToDepth(m_StProDep+m_Rlev*iFilterCY);

		float *Value0=NULL;
		Value0=new float[m_DataNum*iFilterH];
		pFIDI->SeekToDepth(m_StProDep);
		pFIDI->Read(3,m_DataNum*iFilterH,Value0);
		pFIDI->SeekToDepth(m_StProDep+m_Rlev*iFilterH);
		Num=iFilterH;

		float *lpbFilter=new float[iFilterH*iFilterW];
		while(Num<lHeight)
		{
			pFIDI->Read(3,m_DataNum,Value);
			pFIDI->Next();
			for(j=0;j<m_DataNum;j++)
				Value0[(iFilterH-1)*m_DataNum+j]=Value[j];
			
			for(j=iFilterCX;j<lWidth-iFilterW+iFilterCX+1;j++)
			{
				//��ʼ���˲�������Ԫ��
				for (k=0;k<iFilterH;k++)
				{
					for (l=0;l<iFilterW;l++)
					{
						//ָ���i-iTempCY+k�е�j-iTempCX+l�����ص�ָ��
						//���浽�˲���������
						lpbFilter[k*iFilterW+l]=Value0[k*m_DataNum+j-iFilterCX+l];
					}
				}
				//���˲�����ֵ����ԭ���м�����ֵ
				Value[j]=FindMedianValue(lpbFilter,iFilterH*iFilterW);
			}

			pFIDW->Write(3,m_DataNum,Value);
			pFIDW->Next();
			Num++;
			dep=m_StProDep+m_Rlev*Num;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton())
			{
				bUserBreak=TRUE;
				break;
			}
			for(k=0;k<iFilterH-1;k++)
				for(j=0;j<m_DataNum;j++)
					Value0[k*m_DataNum+j]=Value0[(k+1)*m_DataNum+j];
			
		}
		delete Value;delete Value0;delete lpbFilter;
	}
	else bUserBreak=TRUE;
	delete pFIDI;delete pFIDW;
	delete pGuage;	
	if(bUserBreak) return FALSE;
	else           return TRUE;	
}

/************************************************************************/
/*TraceEdge(CString InName,CString OutName)
�ú�������̽�ⷨ�߽����
�ɹ�����TRUE,���򷵻�FALSE                                              */
/************************************************************************/
BOOL CDiffrenceCp::TraceEdge(CString InName,CString OutName)
{
	CFIDIO* pFIDI = new CFIDIO(m_szBufPath+"."+InName);
	if(!pFIDI->Open())
	{
		delete pFIDI;return FALSE;
	}

	//�������ָʾ��
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetTitle(m_szDifName+"̽�ⷨ�߽����...");
	pGuage->ShowWindow(SW_SHOW);
	float dep=m_StProDep;;
	BOOL bUserBreak=FALSE;

	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"PAD","none",REPR_SHORT,4,16,0,0.f,0.f,0.f},
							   {"DATAST","1/ohm",REPR_FLOAT,4,16,16,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;
	m_Content[1].nps=m_DataNum;                        
	m_Content[1].min=2.54f;                            
	m_Content[1].max=m_Bits*3.1416f;                   
	m_Content[1].rlev = m_Bits;                        

	m_Content[2].nps=m_DataNum;
	m_Content[2].npw=m_DataNum;                        
	m_Content[2].min=(float)m_PadNum;                  
	m_Content[2].max=(float)m_ButtonNum;               
	m_Content[2].rlev =0.f;                            
	strcpy(m_Content[2].name,OutName);

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;

	CFIDIO* pFIDW = NULL;
	int pos,i,j,k;
	pFIDW = new CFIDIO(m_szBufPath+"."+OutName,&m_Index);
	if(pFIDW->Open())
	{	
		int iWinH=100;	
		float *Value=NULL;	
		Value=new float[m_DataNum*iWinH];
		float *Value0=NULL;
		Value0=new float[m_DataNum*iWinH];

		//����ֵ
		float data,data0,Pixel,MaxPixel;
		//�Ƿ��ҵ���ʼ�㼰�ص���ʼ��
		BOOL bFindStartPoint;
		//�Ƿ�ɨ�赽һ���߽��
		BOOL bFindPoint;
		//��ʼ�߽��͵�ǰ�߽��
		CPoint StartPoint,CurrentPoint;
		//8���������ʼɨ�跽��
		POINT Direction[8]={{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0}};
		int BeginDirect;
		
		CString str;
		int Num=0;
		dep=m_StProDep;
		int x,y;
	
		while(dep<m_EnProDep-iWinH*m_Rlev)
		{
			pFIDI->SeekToDepth(dep);
			pFIDI->Read(3,m_DataNum*iWinH,Value0);
			for(i=0;i<m_DataNum*iWinH;i++)
				Value[i]=Value0[i];


			//���ҵ������Ϸ��ı߽��
			bFindStartPoint=FALSE;
			//���ҵ�1���߽��
			//ע�⣬����ͼ�������߽磬�Է�Խ��
			for(j=1;j<iWinH-1&&!bFindStartPoint;j++)
			{
				//ÿ��
				for(i=1;i<m_DataNum-1&&!bFindStartPoint;i++)
				{
					//��ʼ��
					data0=data=768.f;
					//ָ��ԭλͼ��j�е�i�е�λ��
					pos=j*m_DataNum+i;
					Pixel=Value0[pos];
					if(Pixel<512) continue;
					//ÿ�����ص����1�к���1�е�3��Ԫ��
					for(y=0;y<3;y++)
					{
						//ÿ�����ص����һ������һ�е�3�����ص�
						for(x=0;x<3;x++)
						{
							//��������ֵ
							data=Value0[pos+m_DataNum*(y-1)+(x-1)];
							//��ȡ��õ����ص����ڵ�8�����ص�����ֵ�����ֵ
							if(data>data0)
								data0=data;
						}
					}
				
					//����8�����ص���������ֵ
					MaxPixel=data0;	
					//�����ǰ���ص��Ǳ߽��
					if(MaxPixel!=Pixel)
					{
						//���ҵ��˱߽���ٵ���ʼ��
						bFindStartPoint=TRUE;
						//��¼��ʼ����ͼ���е�λ��
						StartPoint.y=j;
						StartPoint.x=i;
						//ָ��Ŀ��ͼ������j�к͵�i�и����ص�ָ��
						//�ѱ߽��Ϳ�ɺ�ɫ
						Value[j*m_DataNum+i]=0.f;
					}
				}
			}

			if(bFindStartPoint)
			{
				//������ʼ���������·�������ʼɨ�������Ϸ���
				BeginDirect=0;
				//���ٱ߽�
				bFindStartPoint=FALSE;
				//�ӳ�ʼ�㿪ʼɨ��
				CurrentPoint.y=StartPoint.y;
				CurrentPoint.x=StartPoint.x;

				//������ٻ�û�лص�ԭ����λ��
				while(!bFindStartPoint)
				{
					//û���ҵ��µı߽��
					bFindPoint=FALSE;
					//��������
					k=0;
					while(!bFindPoint)
					{
						//��ʼ��
						data0=data=768.f;
						//��ɨ�跽��鿴һ������
						pos=m_DataNum*(CurrentPoint.y+Direction[BeginDirect].y)+
							(CurrentPoint.x+Direction[BeginDirect].x);
						Pixel=Value0[pos];
						//ÿ�����ص����1�к���1�е�3��Ԫ��
						for(y=0;y<3;y++)
						{
							//ÿ�����ص����һ������һ�е�3�����ص�
							for(x=0;x<3;x++)
							{
								//��������ֵ
								data=Value0[pos+m_DataNum*(y-1)+(x-1)];
								//��ȡ��õ����ص����ڵ�8�����ص�����ֵ�����ֵ
								if(data>data0)
									data0=data;
							}
						}
						//����8�����ص���������ֵ
						MaxPixel=data0;
						//�ҵ����µı߽��
						if(MaxPixel!=Pixel)
						{
							//���ҵ��˱߽���ٵ���ʼ��	
							bFindPoint=TRUE;
							//��¼��ʼ����ͼ���е�λ��
							CurrentPoint.y+=Direction[BeginDirect].y;
							CurrentPoint.x+=Direction[BeginDirect].x;

							//����µı߽�������ʼ�㣬��������
							if(CurrentPoint==StartPoint)
								bFindStartPoint=TRUE;
							if(CurrentPoint.y>iWinH-1||CurrentPoint.x>m_DataNum-1)
								bFindStartPoint=TRUE;
							//���߽��Ϳ�ɺ�ɫ
							pos=m_DataNum*CurrentPoint.y+CurrentPoint.x;
							Value[pos]=0.f;

							//ɨ��ķ�����ʱ����ת����
							BeginDirect--;
							if(BeginDirect<0)
								BeginDirect=7;
							BeginDirect--;
							if(BeginDirect<0)
								BeginDirect=7;
						}
						//ÿ�ҵ��µı߽��
						else
						{
							//ɨ�跽��˳ʱ��תһ��
							BeginDirect++;
							if(BeginDirect>7)
								BeginDirect=0;
						}
					}
				}
			}

			pFIDW->SeekToDepth(dep);
			pFIDW->Write(3,m_DataNum*iWinH,Value);
			Num+=iWinH;
			dep=m_StProDep+m_Rlev*Num;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton())
			{
				bUserBreak=TRUE;
				break;
			}
		}
		delete Value;delete Value0;
	}
	else bUserBreak=TRUE;
	delete pFIDI;delete pFIDW;
	delete pGuage;	
	if(bUserBreak) return FALSE;
	else           return TRUE;	
}

/************************************************************************/
/*KAveIncise(CString InName,CString OutName,int k)
int k;                  //�ָ��������    
�ɹ�����TRUE,���򷵻�FALSE
�ú�������k-��ֵ���෨ͼ��ָ�                                          */
/************************************************************************/
BOOL CDiffrenceCp::KAveIncise(CString InName,CString OutName,int k)
{
	CFIDIO* pFIDI = new CFIDIO(m_szBufPath+"."+InName);
	if(!pFIDI->Open())
	{
		delete pFIDI;return FALSE;
	}

	//�������ָʾ��
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetTitle(m_szDifName+" k-��ֵ���෨ͼ��ָ�...");
	pGuage->ShowWindow(SW_SHOW);
	float dep=m_StProDep;;
	BOOL bUserBreak=FALSE;

	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"PAD","none",REPR_SHORT,4,16,0,0.f,0.f,0.f},
							   {"DATAST","1/ohm",REPR_FLOAT,4,16,16,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = m_StProDep;
	m_Content[0].max = m_EnProDep;
	m_Content[0].rlev = m_Rlev;
	m_Content[1].nps=m_DataNum;                        
	m_Content[1].min=2.54f;                            
	m_Content[1].max=m_Bits*3.1416f;                   
	m_Content[1].rlev = m_Bits;                        

	m_Content[2].nps=m_DataNum;
	m_Content[2].npw=m_DataNum;                        
	m_Content[2].min=(float)m_PadNum;                  
	m_Content[2].max=(float)m_ButtonNum;               
	m_Content[2].rlev =0.f;                            
	strcpy(m_Content[2].name,OutName);

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;

	CFIDIO* pFIDW = NULL;
	pFIDW = new CFIDIO(m_szBufPath+"."+OutName,&m_Index);
	if(pFIDW->Open())
	{		
		int iWinH=20;
		float *Value=NULL;	
		Value=new float[m_DataNum+10];
		pFIDW->SeekToDepth(m_StProDep+m_Rlev*iWinH/2);

		float *Value0=NULL;
		Value0=new float[m_DataNum*iWinH];
		pFIDI->SeekToDepth(m_StProDep);
		pFIDI->Read(3,m_DataNum*iWinH,Value0);
		pFIDI->SeekToDepth(m_StProDep+m_Rlev*iWinH);
		
		//ÿ��������������ص�ƽ��ֵ
		float *kAverage=new float[k];
		//ÿ�������������Ŀ
		int *kAveNum=new int[k];
		//ѭ��������
		BOOL bContinue;
		float Pixel;
		int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
		int Num,i,j,l,m,n;
		Num=iWinH;
		
		while(Num<RecordNum)
		{
			pFIDI->Read(3,m_DataNum,Value);
			pFIDI->Next();
			for(j=0;j<m_DataNum;j++)
				Value0[(iWinH-1)*m_DataNum+j]=Value[j];

			//��ʼ��
			for(j=0;j<k;j++)
			{
				kAveNum[j]=1;
				kAverage[j]=j*1024.f/(k-1);
			}
			//��ʼ��Ϊ��
			bContinue=TRUE;
			//ѭ��,��ͼ����зָ�
			while(bContinue)
			{
				//ͼ��ÿ��
				for(j=0;j<iWinH;j++)
				{
					//ͼ��ÿ��
					for(i=0;i<m_DataNum;i++)
					{
						//ԭͼ��ĵ�j�е�i�е�����
						Pixel=Value0[j*m_DataNum+i];
						//�жϸ����������ĸ�����
						for(l=0;l<k;l++)
						{
							n=0;
							for(m=0;m<k;m++)
							{
								if(m==1)
									continue;
								if(pow(Pixel-kAverage[l],2.f)>pow(Pixel-kAverage[m],2.f))
									break;
								else
									n++;
							}
							//������ڵ�l������
							if(n==(k-1))
							{
								//������������Ŀ��1
								kAveNum[l]++;
								//�޸ĸ����ص�ֵĿ����ʹ�������������
								Value[i]=l*1024.f/(k-1);
								//�����ж���һ������
								break;
							}
						}

						//�����l����������1�����غ�������ƽ��ֵ���䣬������ָ����
						if(fabs(kAverage[l]-(kAverage[l]*(kAveNum[l]-1)+Pixel)/kAveNum[l])<0.1f)
							bContinue=FALSE;
						//��������µ�ƽ��ֵ����ȡ��ԭ����ƽ��ֵ
						else
							kAverage[l]=(kAverage[l]*(kAveNum[l]-1)+Pixel)/kAveNum[l];
					}
				}
			}

			pFIDW->Write(3,m_DataNum,Value);
			pFIDW->Next();
			Num++;
			dep=m_StProDep+m_Rlev*Num;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton())
			{
				bUserBreak=TRUE;
				break;
			}
			for(i=0;i<iWinH-1;i++)
				for(j=0;j<m_DataNum;j++)
					Value0[i*m_DataNum+j]=Value0[(i+1)*m_DataNum+j];
			
		}
		delete Value;delete Value0;
		delete kAveNum;delete kAverage;
	}
	else bUserBreak=TRUE;
	delete pFIDI;delete pFIDW;
	delete pGuage;	
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

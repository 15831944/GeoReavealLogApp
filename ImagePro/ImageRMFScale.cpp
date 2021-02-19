// DataRMFScale.cpp: implementation of the CDataRMFScale class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "flowchart.h"
#include "ImageRMFScale.h"
#include "FlowNode.h"
#include "FlowChartDoc.h"
#include "ImageCreate.h"
#include "ImageGeoCorrect.h"
#include "RMFScaleDlg.h"
#include "FitCircle.h"
#include "FitEllipse.h"
#include "FIDio.h"
#include "Guage.h"
#include "LogProcess.h"
#include <math.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CImageRMFScale,CFlowNode, 0)
CImageRMFScale::CImageRMFScale()
{
	m_pDoc=NULL;
}

CImageRMFScale::~CImageRMFScale()
{
}

CImageRMFScale::CImageRMFScale(CString szNodename,BOOL bResult)
{
	m_szNodeName=szNodename;
	m_bResult=bResult;
	m_bResistance=TRUE;
	m_szRMFWell="";
	m_szRMFName="RXO"; 
	m_LeftScal=2.f;
	m_RightScal=20000.f;
	m_fWin=2.54f;           //10Ӣ��
	m_fRlev=0.0254f;		//��������
	m_iImage=0;             //ѡ��̶�ͼ��0--��̬ͼ��   1--��̬ͼ��
}

void CImageRMFScale::Serialize(CArchive& ar)
{
	CFlowNode::Serialize(ar);	
	if (ar.IsStoring())
	{
		ar<<m_bResistance;
		ar<<m_szRMFWell;
		ar<<m_szRMFName;
		ar<<m_LeftScal;
		ar<<m_RightScal;
		ar<<m_fRlev;
		ar<<m_fWin;
		ar<<m_iImage;
	}
	else
	{
		ar>>m_bResistance;	
		ar>>m_szRMFWell;
		ar>>m_szRMFName;
		ar>>m_LeftScal;
		ar>>m_RightScal;
		ar>>m_fRlev;
		ar>>m_fWin;
		ar>>m_iImage;
	}
}

void CImageRMFScale::InitPara(CFlowChartDoc* pDoc)
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

BOOL CImageRMFScale::Run()
{
	m_bResult=TRUE;
	if(m_pDoc!=NULL)
		m_pDoc->SetModifiedFlag();
	if(!m_pDoc->m_bAunAll)
	{
		CRMFScaleDlg dlg;
		dlg.m_bResistance=m_bResistance;
		dlg.m_szRMFWell=m_szRMFWell;
		dlg.m_szRMFName=m_szRMFName;
		dlg.m_EnProDep=m_EnProDep;
		dlg.m_StProDep=m_StProDep;
		dlg.m_fWin=m_fWin;                   //��������
		dlg.m_fRlev=m_fRlev;
		dlg.m_LeftScal=m_LeftScal;
		dlg.m_RightScal=m_RightScal;
		dlg.m_iImage=m_iImage;
		if(dlg.DoModal()==IDOK)
		{
			m_bResistance=dlg.m_bResistance;
			m_szRMFWell=dlg.m_szRMFWell;
			m_szRMFName=dlg.m_szRMFName;
			m_fWin=dlg.m_fWin;
			m_fRlev=dlg.m_fRlev;
			m_LeftScal=dlg.m_LeftScal;
			m_RightScal=dlg.m_RightScal;
			m_iImage=dlg.m_iImage;
			//����ǳ�������ߵ��������У������ѷ�ˮ�������ʱҪ��
			if(CurveToBuf(m_szRMFWell,m_szRMFName,"RMFGPT",m_Rlev,FALSE))
			{
				if(m_iImage==0)
					m_bResult=DataCopy("DATAST","DATARS");
				else
					m_bResult=DataCopy("DATADY","DATARS");
				if(m_bResult)
					CreateCrackImg(m_szRMFWell+"."+m_szRMFName);
			}
		}
	}
	return m_bResult;
}

//BOOL CImageRMFScale::CreateCrackImg(CString FullRTName)
//{
//	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
//	BOOL bUserBreak=FALSE;
//	//��ʼ���̶Ⱥ�ĵ�������������
//	FIDCONTENT Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
//   		                     {"AZ","o",REPR_FLOAT,4,1,0,0.f,360.f,0.f},
//							 {"RBUT","ohm",REPR_FLOAT,4,144,144,2.f,20000.f,0.f}};
//	Content[0].min = m_StProDep;
//	Content[0].max = m_EnProDep;
//	Content[0].rlev = m_Rlev;
//	Content[1].nps=m_DataNum+1;
//	Content[1].npw=0;
//	Content[2].nps=m_DataNum+1;
//	Content[2].npw=m_DataNum+1;
//	Content[2].min=m_LeftScal;
//	Content[2].max=m_RightScal;
//	FIDINDEX Index;
//	Index.dimension = 3;
//	Index.Contentlist = Content;
//	char adi[] = "==END==";
//	Index.adi = adi;
//	
//	CFIDIO* pBut = new CFIDIO(m_szWellName+".RBUT",&Index);
//	if(!pBut->Open())
//	{
//		delete pBut;
//		return FALSE;
//	}
//
//	//�������ָʾ��
//	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
//	pGuage->Create(CWnd::GetActiveWindow());	
//	pGuage->ShowWindow(SW_SHOW);
//	pGuage->SetTitle(" �����ʿ̶�...");
//
//	char s[256],fn[256];
//	strcpy(s,m_szBufPath);
//	strcat(s,".DATARS");
//	CFIDIO::GetFileName(s,fn,'D');
//	CFile Infp;
//	if(Infp.Open(fn,CFile::modeReadWrite|CFile::typeBinary))
//	{
//		CFitCircle FitCircle(m_DataNum);//���Բ
//		float Radius,fratio,fX,fY;
//		int nFitNum;
//
//		int Block=m_DataNum*4;
//		float *Value=NULL;
//		Value=new float[m_DataNum+1];
//
//		CPoint *points=NULL;
//		points=new CPoint[m_DataNum];
//
//		int i,j,k;
//		float dep,R0;
//
//		strcpy(s,m_szBufPath);
//		strcat(s,".RMFGPT");
//		CFIDIO::GetFileName(s,fn,'D');
//		CFile Rtfp;
//		if(Rtfp.Open(fn,CFile::modeReadWrite|CFile::typeBinary)) 
//		{
//			int kk,k12;
//			int iWin=(int)(m_fWin/m_Rlev);
//			float *pX=NULL;
//			pX=new float[iWin];
//			float *pY=NULL;
//			pY=new float[iWin];
//			float *WinVal=NULL;
//			WinVal=new float[m_DataNum*iWin];
//
//			float mX,mY,mXX,mXY,a,b;
//
//			//Ϊ�˽���һ�������ڵ�ͼ��̶�
//			//�����һ�������ڵĻع�������
//			k=0;
//			dep=m_StProDep;
//			kk=0;
//			while(k<iWin-1)
//			{
//				Rtfp.Seek(k*4,CFile::begin);
//				Rtfp.Read(&R0,4);
//					
//				Infp.Seek (k*Block,CFile::begin);
//				Infp.Read (Value,Block);
//				
//				fratio=0.01f;
//				//�����Բ�ķ�����ƽ���絼�� Radius
//				for(i=0;i<m_DataNum;i++)
//				{
//					if(Value[i]>fratio) 
//						fratio=Value[i];
//				}
//				fratio=3000.f/fratio;
//				
//				nFitNum=0;
//				for(i=0;i<m_DataNum;i++)
//				{
//					points[i].x=points[i].y=-9999;
//					if(Value[i]>0.f)
//					{
//						points[i].x=(int)(sin(6.2831853*i/m_DataNum)*fratio*Value[i]);
//						points[i].y=(int)(cos(6.2831853*i/m_DataNum)*fratio*Value[i]);
//						FitCircle.m_points[nFitNum].x=points[i].x;
//						FitCircle.m_points[nFitNum].y=points[i].y;
//						nFitNum++;
//					}
//				}
//				if(nFitNum>3)
//				{
//					FitCircle.m_nNum=nFitNum;
//					FitCircle.DoCircleFit();
//					fX = (float)FitCircle.m_fCenterX;
//					fY = (float)FitCircle.m_fCenterY;
//					Radius = (float)(FitCircle.m_fRadius);
//
//					pX[kk]=Radius;
//					pY[kk]=(log10(R0)-log10(m_LeftScal))*256.f/(log10(m_RightScal)-log10(m_LeftScal));
//					for(i=0;i<m_DataNum;i++)
//					{
//						if((points[i].x!=-9999)&&(points[i].y!=-9999))
//							Value[i]=(float)(sqrt((points[i].x-fX)*(points[i].x-fX)
//										+(points[i].y-fY)*(points[i].y-fY)));
//						WinVal[m_DataNum*kk+i]=Value[i];
//					}
//					kk++;
//				}
//				k++;
//				dep=m_StProDep+m_Rlev*k;
//				pGuage->SetDepth(dep);
//				if(pGuage->CheckCancelButton())
//				{
//					bUserBreak=TRUE;
//					break;
//				}						
//			}
//			//�ڴ����һ�������ڵ�����ʱ
//			//���û��ж�
//			if(bUserBreak)
//			{
//				Rtfp.Close();	
//				Infp.Close ();
//				delete pX;delete pY;
//				delete Value;delete points;
//				delete pGuage;delete pBut;
//				return FALSE;
//			}
/////////////////////////////////////////////////////////////
////����ǰ������ڵ�����
//			//�ڻ������������Իع齨���̶ȹ�ϵ
//			//y=a+bx
//			mX=mY=mXX=mXY=0.f;
//			for(i=0;i<kk;i++)
//			{
//				mX+=pX[i];
//				mY+=pY[i];
//				mXX+=pX[i]*pX[i];
//				mXY+=pX[i]*pY[i];
//			}
//			b=(mY*mX-mXY*kk)/(mX*mX-mXX*kk);
//			a=(mY-mX*b)/kk;
//			//����̶Ⱥ��ͼ����������
//			//���ݷ�Χ(0-1024)
//			k12=(int)(iWin/2);
//			for(j=0;j<k12;j++)
//			{
//				for(i=0;i<m_DataNum;i++)
//					Value[i]=a+b*WinVal[m_DataNum*j+i];
//				Infp.Seek(j*Block,CFile::begin);
//				Infp.Write(Value,Block);
//			}
/////////////////////////////////////////////////////////////
//			//ǰ���Ѽ����һ�����ڵ�������
//			pBut->SeekToDepth(m_StProDep+m_Rlev*iWin/2);
//			while(k<RecordNum)
//			{
//				Rtfp.Seek(k*4,CFile::begin);
//				Rtfp.Read(&R0,4);
//					
//				Infp.Seek (k*Block,CFile::begin);
//				Infp.Read (Value,Block);
//				
//				fratio=0.01f;
//				//�����Բ�ķ�����ƽ���絼�� Radius
//				for(i=0;i<m_DataNum;i++)
//				{
//					if(Value[i]>fratio) 
//						fratio=Value[i];
//				}
//				fratio=3000.f/fratio;
//				
//				nFitNum=0;
//				for(i=0;i<m_DataNum;i++)
//				{
//					points[i].x=points[i].y=-9999;
//					if(Value[i]>0.f)
//					{
//						points[i].x=(int)(sin(6.2831853*i/m_DataNum)*fratio*Value[i]);
//						points[i].y=(int)(cos(6.2831853*i/m_DataNum)*fratio*Value[i]);
//						FitCircle.m_points[nFitNum].x=points[i].x;
//						FitCircle.m_points[nFitNum].y=points[i].y;
//						nFitNum++;
//					}
//				}
//
//				if(nFitNum>3)
//				{
//					FitCircle.m_nNum=nFitNum;
//					FitCircle.DoCircleFit();
//					fX = (float)FitCircle.m_fCenterX;
//					fY = (float)FitCircle.m_fCenterY;
//					Radius = (float)(FitCircle.m_fRadius);
//
//					pX[kk]=Radius;
//					pY[kk]=(log10(R0)-log10(m_LeftScal))*256.f/(log10(m_RightScal)-log10(m_LeftScal));
//					for(i=0;i<m_DataNum;i++)
//					{
//						if((points[i].x!=-9999)&&(points[i].y!=-9999))
//							Value[i]=(float)(sqrt((points[i].x-fX)*(points[i].x-fX)
//										+(points[i].y-fY)*(points[i].y-fY)));
//						WinVal[m_DataNum*kk+i]=Value[i];
//					}
//					kk++;
//					//�ڻ������������Իع齨���̶ȹ�ϵ
//					//y=a+bx
//				
//					mX=mY=mXX=mXY=0.f;
//					for(i=0;i<kk;i++)
//					{
//						mX+=pX[i];
//						mY+=pY[i];
//						mXX+=pX[i]*pX[i];
//						mXY+=pX[i]*pY[i];
//					}
//					b=(mY*mX-mXY*kk)/(mX*mX-mXX*kk);
//					a=(mY-mX*b)/kk;
//
//					//����߷ֱ���ǳ��������
//					k12=(int)(kk/2);
//					R0=a+b*pX[k12];
//					R0/=256.f;
//					R0=(log10(m_RightScal)-log10(m_LeftScal))*R0+log10(m_LeftScal);
//					R0=pow(10.f,R0);
//					Value[m_DataNum]=R0;
//
//					//����̶Ⱥ�ĵ�������������
//					for(i=0;i<m_DataNum;i++)
//					{
//						Value[i]=a+b*WinVal[m_DataNum*k12+i];
//						Value[i]/=256.f;
//						Value[i]=(log10(m_RightScal)-log10(m_LeftScal))*Value[i]+log10(m_LeftScal);
//						Value[i]=pow(10.f,Value[i]);
//					}
//					pBut->Write(3,m_DataNum+1,Value);
//					pBut->Next();
//
//					//����̶Ⱥ��ͼ����������
//					//���ݷ�Χ(0-1024)
//					for(i=0;i<m_DataNum;i++)
//						Value[i]=a+b*WinVal[m_DataNum*k12+i];
//					k12=(int)(iWin/2);
//					Infp.Seek ((k-k12)*Block,CFile::begin);
//					Infp.Write(Value,Block);
//					for(i=1;i<kk;i++)
//					{
//						pX[i-1]=pX[i];
//						pY[i-1]=pY[i];
//						for(j=0;j<m_DataNum;j++)
//							WinVal[(i-1)*m_DataNum+j]=WinVal[i*m_DataNum+j];
//					}
//					kk--;
//				}
//
//				k++;
//				dep=m_StProDep+m_Rlev*k;
//				pGuage->SetDepth(dep);
//				if(pGuage->CheckCancelButton())
//				{
//					bUserBreak=TRUE;
//					break;
//				}						
//			}
/////////////////////////////////////////////////////////////
//			//������������ڵ�����
//			//����̶Ⱥ��ͼ����������
//			//���ݷ�Χ(0-1024)
//			k12=(int)(iWin/2);
//			for(j=0;j<k12-1;j++)
//			{
//				for(i=0;i<m_DataNum;i++)
//					Value[i]=a+b*WinVal[m_DataNum*(k12+j)+i];
//				Infp.Seek((k-j-1)*Block,CFile::begin);
//				Infp.Write(Value,Block);
//			}
/////////////////////////////////////////////////////////////
//			Rtfp.Close();
//			delete pX;delete pY;delete WinVal;
//		}
//		Infp.Close ();
//		delete Value;delete points;
//	}
//	else bUserBreak=TRUE;
//	delete pGuage;delete pBut;
//	if(bUserBreak) return FALSE;
//	else           return TRUE;
//}
BOOL CImageRMFScale::CreateCrackImg(CString FullRTName)
{
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	BOOL bUserBreak=FALSE;
	//��ʼ���̶Ⱥ�ĵ�������������
	FIDCONTENT Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
   		                     {"AZ","o",REPR_FLOAT,4,1,0,0.f,360.f,0.f},
							 {"RBUT","ohm",REPR_FLOAT,4,144,144,2.f,20000.f,0.f}};
	Content[0].min = m_StProDep;
	Content[0].max = m_EnProDep;
	Content[0].rlev = m_Rlev;
	Content[1].nps=m_DataNum+1;
	Content[1].npw=0;
	Content[2].nps=m_DataNum+1;
	Content[2].npw=m_DataNum+1;
	Content[2].min=m_LeftScal;
	Content[2].max=m_RightScal;
	FIDINDEX Index;
	Index.dimension = 3;
	Index.Contentlist = Content;
	char adi[] = "==END==";
	Index.adi = adi;
	
	CFIDIO* pBut = new CFIDIO(m_szWellName+".RBUT",&Index);
	if(!pBut->Open())
	{
		delete pBut;
		return FALSE;
	}

	//�������ָʾ��
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());	
	pGuage->ShowWindow(SW_SHOW);
	pGuage->SetTitle(" �����ʿ̶�...");

	char s[256],fn[256];
	strcpy(s,m_szBufPath);
	strcat(s,".DATARS");
	CFIDIO::GetFileName(s,fn,'D');
	CFile Infp;
	if(Infp.Open(fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		CFitCircle FitCircle(m_DataNum);//���Բ
		float Radius,fratio,fX,fY;
		int nFitNum;

		int Block=m_DataNum*4;
		float *Value=NULL;
		Value=new float[m_DataNum+1];

		CPoint *points=NULL;
		points=new CPoint[m_DataNum];

		int i,j,k;
		float dep,R0;

		strcpy(s,m_szBufPath);
		strcat(s,".RMFGPT");
		CFIDIO::GetFileName(s,fn,'D');
		CFile Rtfp;
		if(Rtfp.Open(fn,CFile::modeReadWrite|CFile::typeBinary)) 
		{
			int kk,k12;
			int iWin=(int)(m_fWin/m_Rlev);
			float *pX=NULL;
			pX=new float[iWin];
			float *pY=NULL;
			pY=new float[iWin];
			float *WinVal=NULL;
			WinVal=new float[m_DataNum*iWin];

			float mX,mY,mXX,mXY,a,b;

			//Ϊ�˽���һ�������ڵ�ͼ��̶�
			//�����һ�������ڵĻع�������
			k=0;
			dep=m_StProDep;
			kk=0;
			while(k<iWin-1)
			{
				Rtfp.Seek(k*4,CFile::begin);
				Rtfp.Read(&R0,4);
					
				Infp.Seek (k*Block,CFile::begin);
				Infp.Read (Value,Block);
				
				fratio=0.01f;
				//�����Բ�ķ�����ƽ���絼�� Radius
				for(i=0;i<m_DataNum;i++)
				{
					if(Value[i]>fratio) 
						fratio=Value[i];
				}
				fratio=3000.f/fratio;
				
				nFitNum=0;
				for(i=0;i<m_DataNum;i++)
				{
					points[i].x=points[i].y=-9999;
					if(Value[i]>0.f)
					{
						points[i].x=(int)(sin(6.2831853*i/m_DataNum)*fratio*Value[i]);
						points[i].y=(int)(cos(6.2831853*i/m_DataNum)*fratio*Value[i]);
						FitCircle.m_points[nFitNum].x=points[i].x;
						FitCircle.m_points[nFitNum].y=points[i].y;
						nFitNum++;
					}
				}
				if(nFitNum>3)
				{
					FitCircle.m_nNum=nFitNum;
					FitCircle.DoCircleFit();
					fX = (float)FitCircle.m_fCenterX;
					fY = (float)FitCircle.m_fCenterY;
					Radius = (float)(FitCircle.m_fRadius);

					pX[kk]=Radius;
					pY[kk]=(log10(R0)-log10(m_LeftScal))*256.f/(log10(m_RightScal)-log10(m_LeftScal));
					for(i=0;i<m_DataNum;i++)
					{
						if((points[i].x!=-9999)&&(points[i].y!=-9999))
							Value[i]=(float)(sqrt((points[i].x-fX)*(points[i].x-fX)
										+(points[i].y-fY)*(points[i].y-fY)));
						WinVal[m_DataNum*kk+i]=Value[i];
					}
					kk++;
				}
				k++;
				dep=m_StProDep+m_Rlev*k;
				pGuage->SetDepth(dep);
				if(pGuage->CheckCancelButton())
				{
					bUserBreak=TRUE;
					break;
				}						
			}
			//�ڴ����һ�������ڵ�����ʱ
			//���û��ж�
			if(bUserBreak)
			{
				Rtfp.Close();	
				Infp.Close ();
				delete pX;delete pY;
				delete Value;delete points;
				delete pGuage;delete pBut;
				return FALSE;
			}
///////////////////////////////////////////////////////////
//����ǰ������ڵ�����
			//�ڻ������������Իع齨���̶ȹ�ϵ
			//y=a+bx
			mX=mY=mXX=mXY=0.f;
			for(i=0;i<kk;i++)
			{
				mX+=pX[i];
				mY+=pY[i];
				mXX+=pX[i]*pX[i];
				mXY+=pX[i]*pY[i];
			}
			b=(mY*mX-mXY*kk)/(mX*mX-mXX*kk);
			a=(mY-mX*b)/kk;
			//����̶Ⱥ��ͼ����������
			//���ݷ�Χ(0-1024)
			k12=(int)(iWin/2);
			for(j=0;j<k12;j++)
			{
				for(i=0;i<m_DataNum;i++)
					Value[i]=a+b*WinVal[m_DataNum*j+i];
				Infp.Seek(j*Block,CFile::begin);
				Infp.Write(Value,Block);
			}
///////////////////////////////////////////////////////////
			//ǰ���Ѽ����һ�����ڵ�������
			pBut->SeekToDepth(m_StProDep+m_Rlev*iWin/2);
			while(k<RecordNum)
			{
				Rtfp.Seek(k*4,CFile::begin);
				Rtfp.Read(&R0,4);
					
				Infp.Seek (k*Block,CFile::begin);
				Infp.Read (Value,Block);
				
				fratio=0.01f;
				//�����Բ�ķ�����ƽ���絼�� Radius
				for(i=0;i<m_DataNum;i++)
				{
					if(Value[i]>fratio) 
						fratio=Value[i];
				}
				fratio=3000.f/fratio;
				
				nFitNum=0;
				for(i=0;i<m_DataNum;i++)
				{
					points[i].x=points[i].y=-9999;
					if(Value[i]>0.f)
					{
						points[i].x=(int)(sin(6.2831853*i/m_DataNum)*fratio*Value[i]);
						points[i].y=(int)(cos(6.2831853*i/m_DataNum)*fratio*Value[i]);
						FitCircle.m_points[nFitNum].x=points[i].x;
						FitCircle.m_points[nFitNum].y=points[i].y;
						nFitNum++;
					}
				}

				if(nFitNum>3)
				{
					FitCircle.m_nNum=nFitNum;
					FitCircle.DoCircleFit();
					fX = (float)FitCircle.m_fCenterX;
					fY = (float)FitCircle.m_fCenterY;
					Radius = (float)(FitCircle.m_fRadius);

					pX[kk]=Radius;
					pY[kk]=(log10(R0)-log10(m_LeftScal))*256.f/(log10(m_RightScal)-log10(m_LeftScal));
					for(i=0;i<m_DataNum;i++)
					{
						if((points[i].x!=-9999)&&(points[i].y!=-9999))
							Value[i]=(float)(sqrt((points[i].x-fX)*(points[i].x-fX)
										+(points[i].y-fY)*(points[i].y-fY)));
						WinVal[m_DataNum*kk+i]=Value[i];
					}
					kk++;
					//�ڻ������������Իع齨���̶ȹ�ϵ
					//y=a+bx
				
					mX=mY=mXX=mXY=0.f;
					for(i=0;i<kk;i++)
					{
						mX+=pX[i];
						mY+=pY[i];
						mXX+=pX[i]*pX[i];
						mXY+=pX[i]*pY[i];
					}
					b=(mY*mX-mXY*kk)/(mX*mX-mXX*kk);
					a=(mY-mX*b)/kk;

					//����߷ֱ���ǳ��������
					k12=(int)(kk/2);
					R0=a+b*pX[k12];
					R0/=256.f;
					R0=(log10(m_RightScal)-log10(m_LeftScal))*R0+log10(m_LeftScal);
					R0=pow(10.f,R0);
					Value[m_DataNum]=R0;

					//����̶Ⱥ�ĵ�������������
					for(i=0;i<m_DataNum;i++)
					{
						Value[i]=a+b*WinVal[m_DataNum*k12+i];
						Value[i]/=256.f;
						Value[i]=(log10(m_RightScal)-log10(m_LeftScal))*Value[i]+log10(m_LeftScal);
						Value[i]=pow(10.f,Value[i]);
					}
					pBut->Write(3,m_DataNum+1,Value);
					pBut->Next();

					//����̶Ⱥ��ͼ����������
					//���ݷ�Χ(0-1024)
					for(i=0;i<m_DataNum;i++)
						Value[i]=a+b*WinVal[m_DataNum*k12+i];
					k12=(int)(iWin/2);
					Infp.Seek ((k-k12)*Block,CFile::begin);
					Infp.Write(Value,Block);
					for(i=1;i<kk;i++)
					{
						pX[i-1]=pX[i];
						pY[i-1]=pY[i];
						for(j=0;j<m_DataNum;j++)
							WinVal[(i-1)*m_DataNum+j]=WinVal[i*m_DataNum+j];
					}
					kk--;
				}

				k++;
				dep=m_StProDep+m_Rlev*k;
				pGuage->SetDepth(dep);
				if(pGuage->CheckCancelButton())
				{
					bUserBreak=TRUE;
					break;
				}						
			}
///////////////////////////////////////////////////////////
			//������������ڵ�����
			//����̶Ⱥ��ͼ����������
			//���ݷ�Χ(0-1024)
			k12=(int)(iWin/2);
			for(j=0;j<k12-1;j++)
			{
				for(i=0;i<m_DataNum;i++)
					Value[i]=a+b*WinVal[m_DataNum*(k12+j)+i];
				Infp.Seek((k-j-1)*Block,CFile::begin);
				Infp.Write(Value,Block);
			}
///////////////////////////////////////////////////////////
			Rtfp.Close();
			delete pX;delete pY;delete WinVal;
		}
		Infp.Close ();
		delete Value;delete points;
	}
	else bUserBreak=TRUE;
	delete pGuage;delete pBut;
	if(bUserBreak) return FALSE;
	else           return TRUE;
}
BOOL CImageRMFScale::CurveToBuf(CString m_szInWell,CString InName,CString OutName,float NewRlev,BOOL bAz)
{
	CFIDIO* pFIDR = NULL;
	pFIDR = new CFIDIO(m_szInWell+"."+InName);
	if(!pFIDR->Open())
	{
		delete pFIDR;return FALSE;
	}
	float Rlev=pFIDR->GetDepthLevel();
	int dim=pFIDR->GetDimenSion();
	BOOL bUserBreak=FALSE;
	int RecordNum=(int)((m_EnProDep-m_StProDep)/Rlev);
	int i;
	FIDCONTENT* Content;

	FIDCONTENT ContentW[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
								{"","none",REPR_FLOAT,4,1,1,0.f,100.f,0.f},
								{"","none",REPR_FLOAT,4,1,1,0.f,100.f,0.f}};
	ContentW[0].min = m_StProDep;
	ContentW[0].max = m_EnProDep;
	ContentW[0].rlev =NewRlev;
	for(i=1;i<dim;i++)
	{
		Content = pFIDR->GetContent(i+1);
		strcpy(ContentW[i].name,OutName);
		strcpy(ContentW[i].unit,Content->unit);
		ContentW[i].type=Content->type;
		ContentW[i].length=Content->length;
		ContentW[i].nps=Content->nps;
		ContentW[i].npw=Content->npw;
		ContentW[i].min=Content->min;
		ContentW[i].max=Content->max;
		ContentW[i].rlev=NewRlev;
	}
	int nps;
	short* ibuf = NULL;
	float* fbuf = NULL;
	char* cbuf = NULL;
	if (Content != NULL)
	{
		nps = Content->nps;
		if (Content->type == REPR_FLOAT) fbuf = new float[nps];
		else if (Content->type == REPR_CHAR) cbuf = new char[nps];
		else ibuf = new short[nps];
	}

	FIDINDEX IndexW;
	IndexW.dimension = dim;
	ContentW[0].rlev =NewRlev ;
	IndexW.Contentlist = ContentW;
	char adi[] = "==END==";
  	IndexW.adi = adi;

	CFIDIO* pFIDW = NULL;
	pFIDW= new CFIDIO(m_szBufPath+"."+OutName,&IndexW);
	pFIDW->Open();
	unsigned k0,k;
	k0=k=0;

	//�������ָʾ��
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->ShowWindow(SW_SHOW);	

	CString ss=InName+" ���߸��Ƶ�������...";
	pGuage->SetTitle(ss);
	float dep=m_StProDep;

	//�������߽��в�ֵ
	pFIDR->SeekToDepth(dep+Rlev);
	if((dim==2)&&(nps==1)&&(Content->type == REPR_FLOAT))
	{
		float fdata0,fdata;
		pFIDR->Read(2,1,&fdata0);
		pFIDR->Next();
		k0++;
		float df;
		while(dep<=m_EnProDep)
		{
			pFIDR->Read(2,1,&fdata);
			pFIDR->Next();
			k0++;
			if(bAz&&fabs(fdata-fdata0)>180.)
				df=0.f;
			else
				df=(fdata-fdata0)*NewRlev/Rlev;
			while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
			{
				fdata0+=df;
				pFIDW->Write(2,1,&fdata0);
				pFIDW->Next();
				k++;
			}
			fdata0=fdata;
			dep=m_StProDep+Rlev*k0;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) 
			{
				bUserBreak=TRUE;
				break;
			}
		}
	}
	else
	{
		while(dep<=m_EnProDep)
		{
			if (Content->type == REPR_FLOAT)
					pFIDR->Read(dim,nps,fbuf);
			else if (Content->type == REPR_CHAR)
					pFIDR->Read(dim,nps,cbuf);
			else 
					pFIDR->Read(dim,nps,ibuf);
			pFIDR->Next();
			k0++;
			while(m_StProDep+k*NewRlev<m_StProDep+k0*Rlev)
			{
				if (Content->type == REPR_FLOAT)
					pFIDW->Write(dim,nps,fbuf);
				else if (Content->type == REPR_CHAR)
					pFIDW->Write(dim,nps,cbuf);
				else 
					pFIDW->Write(dim,nps,ibuf);
				pFIDW->Next();
				k++;
			} 
			dep=m_StProDep+Rlev*k0;
			pGuage->SetDepth(dep);
			if (pGuage->CheckCancelButton()) 
			{
				bUserBreak=TRUE;
				break;
			}
		}
	}
	delete pFIDR;delete pFIDW;
	if (cbuf != NULL)	delete cbuf;
	if (ibuf != NULL)	delete ibuf;
	if (fbuf != NULL)	delete fbuf;
	delete pGuage;
	if(bUserBreak) return FALSE;
	else           return TRUE;
}
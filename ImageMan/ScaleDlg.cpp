// RMFScaleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PrePlot.h"
#include "ScaleDlg.h"
#include "Database.h"
#include "FIDio.h"
#include "FitCircle.h"
#include "Guage.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScaleDlg dialog


CScaleDlg::CScaleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScaleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScaleDlg)
	m_bResistance = TRUE;
	m_szRMFWell = _T("");
	m_szRMFName = _T("RXO");
	//}}AFX_DATA_INIT
}


void CScaleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScaleDlg)
	DDX_Check(pDX, IDC_CHECK1, m_bResistance);
	
	DDX_Text(pDX, IDC_RMFWELL, m_szRMFWell);
	DDX_CBString(pDX, IDC_COMBO_RMFNAME, m_szRMFName);
	DDX_Control(pDX, IDC_COMBO_RMFNAME, m_NameComCtrl);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT1, m_fWin);
	DDX_Text(pDX, IDC_EDIT5, m_fRlev);
	DDX_Text(pDX, IDC_EDIT3, m_LeftScal);
	DDX_Text(pDX, IDC_EDIT4, m_RightScal);
}


BEGIN_MESSAGE_MAP(CScaleDlg, CDialog)
	//{{AFX_MSG_MAP(CScaleDlg)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_CBN_CLOSEUP(IDC_COMBO_RMFNAME, OnCloseupComboRmfname)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScaleDlg message handlers

BOOL CScaleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	InitComCtrl();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CScaleDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	static char BASED_CODE szFilter[] = 
	"测井数据(*.FID)|*.FID|所有文件(*.*)|*.*||";
	CFileDialog  Browse(TRUE, "测井数据(*.FID)", "*.FID", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	Browse.m_ofn.lpstrTitle = "打开文件";
	Browse.m_ofn.lpstrInitialDir=GetCurrentUserPath();
	if(Browse.DoModal()==IDOK)
	{
		CString str = Browse.GetPathName();
		int x=str.ReverseFind('.');
		if(x>0)
		{
			m_szRMFWell=str.Left(x);
			GetDlgItem(IDC_RMFWELL)->SetWindowText(m_szRMFWell);
			InitComCtrl();
		}
	}
}

void CScaleDlg::InitComCtrl()
{
	m_NameComCtrl.ResetContent();
	CString Path=m_szRMFWell.Left (m_szRMFWell.ReverseFind ('\\'));
	CDbaseFile m_dBaseFile;
	if(m_dBaseFile.Open(Path+"\\Curve.dbf")!=DBASE_SUCCESS)
		return;
	char szBuff[255];
	CString Name,Type;
	for(int rc=m_dBaseFile.GetFirstRecord();rc==DBASE_SUCCESS;rc=m_dBaseFile.GetNextRecord())
  	{
		if (m_dBaseFile.IsRecordDeleted())
			continue;
		m_dBaseFile.GetField(1, szBuff);
		Name=szBuff;
		Name.TrimLeft();Name.TrimRight();
		m_dBaseFile.GetField("曲线类型", szBuff);
		Type=szBuff;
		Type.TrimLeft();Type.TrimRight();
		if(Type=="常规")
			m_NameComCtrl.AddString (Name);
	}
	m_dBaseFile.Close();
	m_NameComCtrl.SetWindowText(m_szRMFName);
}

void CScaleDlg::OnCloseupComboRmfname() 
{
	// TODO: Add your control notification handler code here
	CString str;
	int nIndex = m_NameComCtrl.GetCurSel();
	if (nIndex == CB_ERR)
	{
		m_NameComCtrl.GetWindowText(str);
		return;
	}
	else
		m_NameComCtrl.GetLBText(nIndex, str);
	str.MakeUpper();
	if(m_szRMFName != str)
		m_szRMFName = str;
	GetDlgItem(IDC_COMBO_RMFNAME)->SetWindowText(m_szRMFName);
}
void CScaleDlg::OnBnClickedOk()
{
	OnOK();
	BOOL flag;
	if(m_DataType>2)
		flag=CreateCrackImg();
	else
		flag=CreateCrackImg2();
	if(flag)
	{
		CString WellName=m_szWell.Left(m_szWell.ReverseFind('.'));
		char TargetCurveName[256];
		strcpy(TargetCurveName,WellName);
		strcat(TargetCurveName,".FID");
		RefurbishFID(TargetCurveName);
	}
}

BOOL CScaleDlg::CreateCrackImg()
{
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	BOOL bUserBreak=FALSE;
	//初始化刻度后的电阻率阵列数据
	FIDCONTENT Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
   		                     {"AZ","none",REPR_FLOAT,4,1,0,0.f,360.f,0.f},
							 {"RBUT","ohm",REPR_FLOAT,4,144,144,2.f,20000.f,0.f}};
	Content[0].min = m_StProDep;
	Content[0].max = m_EnProDep;
	Content[0].rlev = m_Rlev;
	Content[1].nps=m_DataNum;
	Content[1].npw=0;
	Content[2].nps=m_DataNum;
	Content[2].npw=m_DataNum;
	Content[2].min=m_LeftScal;
	Content[2].max=m_RightScal;
	FIDINDEX Index;
	Index.dimension = 3;
	Index.Contentlist = Content;
	char adi[] = "==END==";
	Index.adi = adi;

	CString szStr=m_szName+"RXO";
	strcpy(Content[2].name,szStr);
	//新建电阻率刻度图像
	szStr=m_szWell.Left(m_szWell.ReverseFind('.')+1)+szStr;
	CFIDIO* pBut = new CFIDIO(szStr,&Index);
	if(!pBut->Open())
	{
		AfxMessageBox("新建电阻率刻度图像失败      ");
		delete pBut;
		return FALSE;
	}
	//新建高分辩率电阻率曲线 RXOH
	Content[1].npw=1;
	strcpy(Content[1].name,"RXOH");
	Index.dimension = 2;

	szStr=m_szWell.Left(m_szWell.ReverseFind('.')+1)+"RXOH";
	CFIDIO* pRXOH = new CFIDIO(szStr,&Index);
	if(!pRXOH->Open())
	{
		AfxMessageBox("新建高分辩率电阻率曲线失败      ");
		delete pBut;delete pRXOH;
		return FALSE;
	}
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());	
	pGuage->ShowWindow(SW_SHOW);
	pGuage->SetTitle(" 电阻率刻度...");

	char s[256],fn[256];
	szStr=m_szWell.Left(m_szWell.ReverseFind('.')+1);
	strcpy(s,szStr);
	strcat(s,m_szName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile Infp;
	if(Infp.Open(fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		CFitCircle FitCircle(m_DataNum);//拟合圆
		float Radius,fratio,fX,fY;
		int nFitNum;

		int Block=m_DataNum*4;
		float *Value=new float[m_DataNum];
		CPoint *points=new CPoint[m_DataNum];

		int i,j,k;
		float dep,R0;
		szStr=m_szRMFWell.Left(m_szRMFWell.ReverseFind('.')+1)+m_szRMFName;
		CFIDIO* pRxo = new CFIDIO(szStr);
		if(pRxo->Open())
		{
			int kk,k12;
			int iWin=(int)(m_fWin/m_Rlev);
			float *pX=new float[iWin];
			float *pY=new float[iWin];
			float *WinVal=new float[m_DataNum*iWin];

			float mX,mY,mXX,mXY,a,b;

			//为了将第一个窗口内的图像刻度
			//计算第一个窗口内的回归样本点
			k=0;
			dep=m_StProDep;
			kk=0;
			while(k<iWin-1)
			{	
				pRxo->SeekToDepth(dep);
				pRxo->Read(2,1,&R0);

				Infp.Seek (k*Block,CFile::begin);
				Infp.Read (Value,Block);

				fratio=0.01f;
				//用拟合圆的方法求平均电导率 Radius
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
			//在处理第一个窗口内的数据时
			//被用户中断
			if(bUserBreak)
			{
				delete pRxo;
				Infp.Close ();
				delete pX;delete pY;
				delete Value;delete points;
				delete pGuage;delete pBut;
				return FALSE;
			}
///////////////////////////////////////////////////////////
//处理前半个窗口的数据
			//在滑动窗口内线性回归建立刻度关系
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
			//计算刻度后的图像阵列数据
			//数据范围(0-1024)
			k12=(int)(iWin/2);
			pBut->SeekToDepth(m_StProDep);
			for(j=0;j<k12;j++)
			{
				for(i=0;i<m_DataNum;i++)
				{
					if(WinVal[m_DataNum*j+i]<0.f)
						Value[i]=-9999.f;
					else
						Value[i]=a+b*WinVal[m_DataNum*j+i];
				}
				pBut->Write(3,m_DataNum,Value);
				pBut->Next();
			}
///////////////////////////////////////////////////////////
			//前面已计算第一个窗口的样本点
			dep=m_StProDep+m_Rlev*iWin/2;
			pBut->SeekToDepth(dep);
			pRXOH->SeekToDepth(dep);
			while(k<RecordNum)
			{
				pRxo->SeekToDepth(dep);
				pRxo->Read(2,1,&R0);

				Infp.Seek (k*Block,CFile::begin);
				Infp.Read (Value,Block);

				fratio=0.01f;
				//用拟合圆的方法求平均电导率 Radius
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
					//在滑动窗口内线性回归建立刻度关系
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

					//计算高分辨率浅侧向数据
					k12=(int)(kk/2);
					R0=a+b*pX[k12];
					R0/=256.f;
					R0=(log10(m_RightScal)-log10(m_LeftScal))*R0+log10(m_LeftScal);
					R0=pow(10.f,R0);
			        //保存高分辩率微电阻率曲线
					pRXOH->Write(2,1,&R0);
					pRXOH->Next();

					//计算刻度后的电阻率阵列数据
					for(i=0;i<m_DataNum;i++)
					{
						Value[i]=a+b*WinVal[m_DataNum*k12+i];
						Value[i]/=256.f;
						Value[i]=(log10(m_RightScal)-log10(m_LeftScal))*Value[i]+log10(m_LeftScal);
						Value[i]=pow(10.f,Value[i]);
					}
					//计算刻度后的图像阵列数据
					//数据范围(0-1024)
					for(i=0;i<m_DataNum;i++)
					{
						if(WinVal[m_DataNum*k12+i]<0.f)
							Value[i]=-9999.f;
						else
							Value[i]=a+b*WinVal[m_DataNum*k12+i];
					}
					pBut->Write(3,m_DataNum,Value);
					pBut->Next();
					k12=(int)(iWin/2);
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
			//处理最后半个窗口的数据
			//计算刻度后的图像阵列数据
			//数据范围(0-1024)
			k12=(int)(iWin/2);
			for(j=0;j<k12-1;j++)
			{
				for(i=0;i<m_DataNum;i++)
				{
					if(WinVal[m_DataNum*(k12+j)+i]<0.f)
						Value[i]=-9999.f;
					else
						Value[i]=a+b*WinVal[m_DataNum*(k12+j)+i];
				}
				pBut->Write(3,m_DataNum,Value);
				pBut->Next();
			}
///////////////////////////////////////////////////////////
			delete pX;delete pY;delete WinVal;
		}
		delete pRxo;Infp.Close ();
		delete Value;delete points;
	}
	else bUserBreak=TRUE;
	delete pGuage;delete pBut;delete pRXOH;
	if(bUserBreak) return FALSE;
	else           return TRUE;
}

BOOL CScaleDlg::CreateCrackImg2()
{
	int RecordNum=(int)((m_EnProDep-m_StProDep)/m_Rlev);
	BOOL bUserBreak=FALSE;
	//初始化刻度后的电阻率阵列数据
	FIDCONTENT Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
   		                     {"AZ","none",REPR_FLOAT,4,1,0,0.f,360.f,0.f},
							 {"RBUT","ohm",REPR_FLOAT,4,144,144,2.f,20000.f,0.f}};
	Content[0].min = m_StProDep;
	Content[0].max = m_EnProDep;
	Content[0].rlev = m_Rlev;
	Content[1].nps=m_DataNum;
	Content[1].npw=0;
	Content[2].nps=m_DataNum;
	Content[2].npw=m_DataNum;
	Content[2].min=m_LeftScal;
	Content[2].max=m_RightScal;
	FIDINDEX Index;
	Index.dimension = 3;
	Index.Contentlist = Content;
	char adi[] = "==END==";
	Index.adi = adi;

	CString szStr=m_szName+"RXO";
	strcpy(Content[2].name,szStr);

	szStr=m_szWell.Left(m_szWell.ReverseFind('.')+1)+szStr;
	CFIDIO* pBut = new CFIDIO(szStr,&Index);
	if(!pBut->Open())
	{
		AfxMessageBox("新建电阻率刻度图像失败      ");
		delete pBut;
		return FALSE;
	}
	//新建高分辩率电阻率曲线 RXOH
	Content[1].npw=1;
	strcpy(Content[1].name,"RXOH");
	Index.dimension = 2;

	szStr=m_szWell.Left(m_szWell.ReverseFind('.')+1)+"RXOH";
	CFIDIO* pRXOH = new CFIDIO(szStr,&Index);
	if(!pRXOH->Open())
	{
		AfxMessageBox("新建高分辩率电阻率曲线失败      ");
		delete pBut;delete pRXOH;
		return FALSE;
	}
	//定义进度指示器
	CGuage* pGuage = new CGuage(m_StProDep,m_EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());	
	pGuage->ShowWindow(SW_SHOW);
	pGuage->SetTitle(" 电阻率刻度...");

	char s[256],fn[256];
	szStr=m_szWell.Left(m_szWell.ReverseFind('.')+1);
	strcpy(s,szStr);
	strcat(s,m_szName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile Infp;
	if(Infp.Open(fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		CFitCircle FitCircle(m_DataNum);//拟合圆
		float Radius,fratio,fX,fY;
		int nFitNum;

		int Block=m_DataNum*2;
		float *Value=new float[m_DataNum];
		short *iwf=new short[m_DataNum];
		CPoint *points=new CPoint[m_DataNum];

		int i,j,k;
		float dep,R0;
		szStr=m_szRMFWell.Left(m_szRMFWell.ReverseFind('.')+1)+m_szRMFName;
		CFIDIO* pRxo = new CFIDIO(szStr);
		if(pRxo->Open())
		{
			int kk,k12;
			int iWin=(int)(m_fWin/m_Rlev);
			float *pX=new float[iWin];
			float *pY=new float[iWin];
			float *WinVal=new float[m_DataNum*iWin];

			float mX,mY,mXX,mXY,a,b;

			//为了将第一个窗口内的图像刻度
			//计算第一个窗口内的回归样本点
			k=0;
			dep=m_StProDep;
			kk=0;
			while(k<iWin-1)
			{	
				pRxo->SeekToDepth(dep);
				pRxo->Read(2,1,&R0);

				Infp.Seek (k*Block,CFile::begin);
				Infp.Read (iwf,Block);
				for(i=0;i<m_DataNum;i++)
					Value[i]=(float)iwf[i];
				fratio=0.01f;
				//用拟合圆的方法求平均电导率 Radius
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
			//在处理第一个窗口内的数据时
			//被用户中断
			if(bUserBreak)
			{
				delete pRxo;
				Infp.Close ();
				delete pX;delete pY;
				delete Value;delete points;delete iwf;
				delete pGuage;delete pBut;
				return FALSE;
			}
///////////////////////////////////////////////////////////
//处理前半个窗口的数据
			//在滑动窗口内线性回归建立刻度关系
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
			//计算刻度后的图像阵列数据
			//数据范围(0-1024)
			k12=(int)(iWin/2);
			pBut->SeekToDepth(m_StProDep);
			for(j=0;j<k12;j++)
			{
				for(i=0;i<m_DataNum;i++)
				{
					if(WinVal[m_DataNum*j+i]<0.f)
						Value[i]=-9999.f;
					else
						Value[i]=a+b*WinVal[m_DataNum*j+i];
				}
				pBut->Write(3,m_DataNum,Value);
				pBut->Next();
			}
///////////////////////////////////////////////////////////
			//前面已计算第一个窗口的样本点
			dep=m_StProDep+m_Rlev*iWin/2;
			pBut->SeekToDepth(dep);
			pRXOH->SeekToDepth(dep);
			while(k<RecordNum)
			{
				pRxo->SeekToDepth(dep);
				pRxo->Read(2,1,&R0);

				Infp.Seek (k*Block,CFile::begin);
				Infp.Read (iwf,Block);
				for(i=0;i<m_DataNum;i++)
					Value[i]=(float)iwf[i];

				fratio=0.01f;
				//用拟合圆的方法求平均电导率 Radius
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
					//在滑动窗口内线性回归建立刻度关系
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

					//计算高分辨率浅侧向数据
					k12=(int)(kk/2);
					R0=a+b*pX[k12];
					R0/=256.f;
					R0=(log10(m_RightScal)-log10(m_LeftScal))*R0+log10(m_LeftScal);
					R0=pow(10.f,R0);
					//保存高分辩率微电阻率曲线
					pRXOH->Write(2,1,&R0);
					pRXOH->Next();

					//计算刻度后的电阻率阵列数据
					for(i=0;i<m_DataNum;i++)
					{
						Value[i]=a+b*WinVal[m_DataNum*k12+i];
						Value[i]/=256.f;
						Value[i]=(log10(m_RightScal)-log10(m_LeftScal))*Value[i]+log10(m_LeftScal);
						Value[i]=pow(10.f,Value[i]);
					}
					//计算刻度后的图像阵列数据
					//数据范围(0-1024)
					for(i=0;i<m_DataNum;i++)
					{
						if(WinVal[m_DataNum*k12+i]<0.f)
							Value[i]=-9999.f;
						else
							Value[i]=a+b*WinVal[m_DataNum*k12+i];
					}
					pBut->Write(3,m_DataNum,Value);
					pBut->Next();
					k12=(int)(iWin/2);
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
			//处理最后半个窗口的数据
			//计算刻度后的图像阵列数据
			//数据范围(0-1024)
			k12=(int)(iWin/2);
			for(j=0;j<k12-1;j++)
			{
				for(i=0;i<m_DataNum;i++)
				{
					if(WinVal[m_DataNum*(k12+j)+i]<0.f)
						Value[i]=-9999.f;
					else
						Value[i]=a+b*WinVal[m_DataNum*(k12+j)+i];
				}
				pBut->Write(3,m_DataNum,Value);
				pBut->Next();
			}
///////////////////////////////////////////////////////////
			delete pX;delete pY;delete WinVal;
		}
		delete pRxo;Infp.Close ();
		delete Value;delete points;delete iwf;
	}
	else bUserBreak=TRUE;
	delete pGuage;delete pBut;delete pRXOH;
	if(bUserBreak) return FALSE;
	else           return TRUE;
}
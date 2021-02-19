// XtfToFid.cpp: implementation of the CXtfToFid class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_comext.h"
#include "XtfToFid.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXtfToFid::CXtfToFid()
{

}

CXtfToFid::~CXtfToFid()
{

}
bool CXtfToFid::XtfToFid(CString SourceFile,CString sObjectPath,int nCurvesRadio,FILE *fo)
{
	CString s;

	m_SourceFile=SourceFile; //源文件名   
	m_sObjectPath=sObjectPath;  //输出目录
	m_nCurvesRadio=nCurvesRadio;//过滤开关
	fp=fo;

	pGuage= new CProgressDlg;
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetRange(0,100);
	pGuage->SetStep(1);
	pGuage->OffsetPos(0);
	pGuage->ShowWindow(SW_SHOW);

	XtfRead=new CXtfRead ;
	FidWrite=new CFidWrite ; //输出类
	CurveIndex.RemoveAll();
	bool bResult=Transform();
	//输出转换结果信息
	if(bResult)
	{
		FidWrite->PrintHead(fp);
	}

	delete pGuage;  //删除进度条
	delete FidWrite; FidWrite=NULL;
	delete XtfRead;	XtfRead=NULL;

	CurveIndex.RemoveAll();
	return bResult;
}
bool CXtfToFid::Transform()
{
	CString s;
	if(!IsXTF(m_SourceFile))
	{
		fprintf(fp,"%s is not XTF Format\n\n",m_SourceFile);
		return false;
	}
	//根据输出目录及输入文件名确定井名和输出路径
	CString sFileTitle=MyGetFileTitle(m_SourceFile);
	CString sOutWellPath=m_sObjectPath+"\\#"+sFileTitle;
	ModiFileName(sOutWellPath); //根据条件决定是否修改文件名
	sFileTitle=MyGetFileTitle(sOutWellPath);
	CString sWellName=sFileTitle.Mid(1); //去掉“#”
	FidWrite->SetFidFileName(m_sObjectPath,sWellName);

	if(!XtfRead->Open(m_SourceFile)) 
	{
		fprintf(fp,"\n!!!!打开输入文件%s有错\n\n",m_SourceFile);
		return false;
	}

	XtfRead->GetHead();
	pGuage->SetRange(0,XtfRead->XtfHead.isnumcv);

	//读Wis对象信息，并选择其中所要的曲线
	SetFidHeadFromXtfHead();
	
	FidWrite->WriteFidInfo();
	for(int i=0;i<CurveIndex.GetSize();i++)
	{	//常规曲线
		pGuage->SetPos(i+1);  //设置进度条当前值
		if (pGuage->CheckCancelButton())
		{
			sp_Break_Status=true;
			break;
		}
		int index=CurveIndex.GetAt(i); //从选择曲线中得到索引号
		int nResult=XtfRead->OpenCurve(index); //读通道信息
		if(nResult<0)
		{
			continue; //曲头有错误，忽略该曲线
		}
		SetFidChannelFromXtfChannel(i);	//从XTF通道设置FID通道信息
		if(XtfRead->XtfCurve.Ictype==1)
		{
			DWORD NumPoint=XtfRead->XtfCurve.NumberOfLevelsForTheCurve;
			float *buffer=new float[NumPoint];
			XtfRead->ReadCurve(index,NumPoint,buffer);
			FidWrite->WriteGenData(i,NumPoint,buffer);
			delete []buffer; buffer=NULL;
		}
		else if(XtfRead->XtfCurve.Ictype==2)
		{	//波形曲线
			if(FidWrite->OpenWave(i))
			{
				float *buffer=new float[FidWrite->FidChannel[i].DimInfo[2].Npw];
				for(int k=0;k<FidWrite->FidChannel[i].DimInfo[2].Npw;k++)
				{
					buffer[k]=0;
				}
				int Num=int((FidWrite->FidChannel[i].DimInfo[0].Stop-FidWrite->FidChannel[i].DimInfo[0].Start)/FidWrite->FidChannel[i].DimInfo[0].Rlev+0.5)+1;
				for(int j=0;j<Num;j++)
				{
					float depth=XtfRead->XtfCurve.deptop+j*XtfRead->XtfCurve.rlevcv;
					XtfRead->ReadWave(index,depth,buffer);
					FidWrite->WriteWave(depth*IndexSacle,buffer);
				}
				delete []buffer; buffer=NULL;
				FidWrite->CloseWave();
			}
		}
		else if(XtfRead->XtfCurve.Ictype==3)
		{	//阵列曲线,由于阵列曲线含有多条波形曲线，FID文件无法表示，将其拆分
			//并在曲线名后加上序号作为新的曲线名
			CString SaveCurveName; //阵列曲线本名
			SaveCurveName.Format ("%-.15s",FidWrite->FidChannel[i].CurveName);
			SaveCurveName.TrimRight();
			for(int subindex=0;subindex<XtfRead->XtfCurve.Idims[1];subindex++)
			{
				//根据阵列曲线名定义波曲线名
				s.Format("%s[%d]",(const char*)SaveCurveName,subindex+1);
				sprintf(FidWrite->FidChannel[i].CurveName,"%-.15s",(const char*)s);
				strcpy(FidWrite->FidChannel[i].DimInfo[2].Name,FidWrite->FidChannel[i].CurveName);
				//输出其中一条波形数据
				if(FidWrite->OpenWave(i))
				{
					float *buffer=new float[FidWrite->FidChannel[i].DimInfo[2].Npw];
					for(int k=0;k<FidWrite->FidChannel[i].DimInfo[2].Npw;k++)
					{
						buffer[k]=0;
					}
					int Num=int((FidWrite->FidChannel[i].DimInfo[0].Stop-FidWrite->FidChannel[i].DimInfo[0].Start)/FidWrite->FidChannel[i].DimInfo[0].Rlev+0.5)+1;
					for(int j=0;j<Num;j++)
					{
						float depth=XtfRead->XtfCurve.deptop+j*XtfRead->XtfCurve.rlevcv;
						XtfRead->ReadWaveFromMatrix(index,subindex,depth,buffer);
						FidWrite->WriteWave(depth*IndexSacle,buffer);
					}
					delete []buffer;
					buffer=NULL;
					FidWrite->CloseWave();
				}
			}
		}
		else
		{
			//维数多于3，无法识别
		}
	}
	XtfRead->Close();
	//输出信息
	fprintf(fp,"DataBuff: %s\n",(const char*)m_SourceFile);
	fprintf(fp,"ToObject: %s\n",(const char*)FidWrite->m_sFidFile);
	//对输出FID文件中曲线名进行修正
	FidWrite->FefurbishFidInfo();
	//////////////////
	return !sp_Break_Status;
}
void CXtfToFid::SetFidHeadFromXtfHead()
{
	CString s,sCurveName;
	sprintf(FidWrite->FidHead.Oil_Field,"%-80.80s",XtfRead->XtfHead.Fieldname);
	sprintf(FidWrite->FidHead.Region,"%-32.32s",XtfRead->XtfHead.Countyname);
	sprintf(FidWrite->FidHead.Company,"%-80.80s",XtfRead->XtfHead.Companyname);
	sprintf(FidWrite->FidHead.Well,"%-80.80s",XtfRead->XtfHead.Wellname);
	sprintf(FidWrite->FidHead.X_Coordinate,"%-40.40s","");
	sprintf(FidWrite->FidHead.Y_Coordinate,"%-40.40s","");
	sprintf(FidWrite->FidHead.Exformation,"%-40.40s",XtfRead->XtfHead.Welllocation);
	FidWrite->FidHead.NumLog=0;
	CurveIndex.RemoveAll();
	SelCurveName.RemoveAll();
	NewCurveName.RemoveAll();
	NewUnits.RemoveAll();
	if(m_nCurvesRadio==0)
	{
		for(int i=0;i<XtfRead->XtfHead.isnumcv;i++)
		{
			sCurveName.Format("%-.8s",XtfRead->XtfHead.CurveName[i]);
			sCurveName.TrimRight();
			for(int j=0;j<sp_FilterCurves.GetSize();j++)
			{
				if(sCurveName.CompareNoCase(sp_FilterCurves.GetAt(j))==0)
				{
					CurveIndex.Add(i);
					SelCurveName.Add(sCurveName);
					NewCurveName.Add(sCurveName);
					s.Format ("%-.8s",XtfRead->XtfHead.Units[i]);
					s.TrimLeft();
					s.TrimRight();
					NewUnits.Add(s);
					break;
				}
			}
		}
	}
	else if(m_nCurvesRadio==1)
	{
		for(int i=0;i<XtfRead->XtfHead.isnumcv;i++)
		{
			CurveIndex.Add(i);
			sCurveName.Format("%-.8s",XtfRead->XtfHead.CurveName[i]);
			sCurveName.TrimRight();
			SelCurveName.Add(sCurveName);
			NewCurveName.Add(sCurveName);
			s.Format ("%-.8s",XtfRead->XtfHead.Units[i]);
			s.TrimLeft();
			s.TrimRight();
			NewUnits.Add(s);
		}
	}
	else if(m_nCurvesRadio==2)
	{
		for(int i=0;i<XtfRead->XtfHead.isnumcv;i++)
		{
			if(XtfRead->XtfHead.CurveAttrib[i].CurType>1)
			{
				continue; //不是常规曲线被忽略
			}
			CurveIndex.Add(i);
			sCurveName.Format("%-.8s",XtfRead->XtfHead.CurveName[i]);
			sCurveName.TrimRight();
			SelCurveName.Add(sCurveName);
			NewCurveName.Add(sCurveName);
			s.Format ("%-.8s",XtfRead->XtfHead.Units[i]);
			s.TrimLeft();
			s.TrimRight();
			NewUnits.Add(s);
		}
	}
	else if(m_nCurvesRadio==3)
	{
		CReCurNamDlg ReCurNamDlg;
		ReCurNamDlg.m_nMaxLen=32;
		ReCurNamDlg.m_nMaxLog=10000;
		for(int i=0;i<XtfRead->XtfHead.isnumcv;i++)
		{
			s.Format("%-.8s",XtfRead->XtfHead.CurveName[i]);
			s.TrimRight();
			ReCurNamDlg.m_SourceName.Add(s);
			ReCurNamDlg.m_ObjectName.Add(s);
			s.Format ("%-.8s",XtfRead->XtfHead.Units[i]);
			s.TrimLeft();
			s.TrimRight();
			ReCurNamDlg.m_Units.Add(s);
			int nDim=XtfRead->XtfHead.CurveAttrib[i].CurType;
			ReCurNamDlg.m_NumDim.Add(nDim);
		}
		int Result=ReCurNamDlg.DoModal();
		for(i=0;i<ReCurNamDlg.m_SourceName.GetSize();i++)
		{
			SelCurveName.Add(ReCurNamDlg.m_SourceName.GetAt(i));
			NewCurveName.Add(ReCurNamDlg.m_ObjectName.GetAt(i));
			NewUnits.Add(ReCurNamDlg.m_Units.GetAt(i));
			for(int j=0;j<XtfRead->XtfHead.isnumcv;j++)
			{
				sCurveName.Format("%-.8s",XtfRead->XtfHead.CurveName[j]);
				sCurveName.TrimRight();
				if(sCurveName.CompareNoCase(SelCurveName.GetAt(i))==0)
				{
					CurveIndex.Add(j);
					break;
				}
			}
		}
		ReCurNamDlg.m_SourceName.RemoveAll();
		ReCurNamDlg.m_ObjectName.RemoveAll();
		ReCurNamDlg.m_Units.RemoveAll();
		ReCurNamDlg.m_NumDim.RemoveAll();
	}

	FidWrite->FidHead.NumLog=CurveIndex.GetSize();
	//根据曲线数(FidWrite->FidHead.NumLog)开辟空间
	FidWrite->InitFidChannel();
	//得到每条曲线的索引信息
	for(int i=0;i<CurveIndex.GetSize();i++)
	{
		sprintf(FidWrite->FidChannel[i].CurveName,"%-.15s",NewCurveName.GetAt(i));
	}
}

void CXtfToFid::SetFidChannelFromXtfChannel(int index)
{
	CString s;
	//维数
	if(XtfRead->XtfCurve.Ictype<3)
	{
		FidWrite->FidChannel[index].NumOfDimension=XtfRead->XtfCurve.Ictype+1;
	}
	else
	{
		FidWrite->FidChannel[index].NumOfDimension=XtfRead->XtfCurve.Ictype;
	}

	//根据索引单位判断曲线的采样驱动类型（时间或深度）
	IndexSacle=1; //深度索引转换因子
	int IndexType=0; //深度或时间索引

	//第一维
	sprintf(FidWrite->FidChannel[index].DimInfo[0].Name,"%-.8s","Dep");
	sprintf(FidWrite->FidChannel[index].DimInfo[0].Unit,"%-.8s","m");
	s.Format("%-.8s",XtfRead->XtfCurve.DepthUnits);
	s.TrimRight();
	//隐含为“米”
	if(s.CompareNoCase("dm")==0)
	{	
		IndexSacle=0.1f;//分米
	}
	else if(s.CompareNoCase("feet")==0)
	{	
		IndexSacle=0.3048f;//英尺
	}

	if(s.CompareNoCase("seconds")==0 || s.CompareNoCase("millsecs")==0)
	{	//时间驱动
		IndexType=1;
		sprintf(FidWrite->FidChannel[index].DimInfo[0].Name,"%-.8s","Time");
		sprintf(FidWrite->FidChannel[index].DimInfo[0].Unit,"%-.8s",XtfRead->XtfCurve.DepthUnits);
	}
	
	FidWrite->FidChannel[index].DimInfo[0].RepCode=FID_REPR_FLOAT;
	FidWrite->FidChannel[index].DimInfo[0].CodeLen=4;
	FidWrite->FidChannel[index].DimInfo[0].Nps=1;
	FidWrite->FidChannel[index].DimInfo[0].Npw=0;
	FidWrite->FidChannel[index].DimInfo[0].Start=XtfRead->XtfCurve.deptop*IndexSacle;
	FidWrite->FidChannel[index].DimInfo[0].Stop=XtfRead->XtfCurve.depbot*IndexSacle;
	FidWrite->FidChannel[index].DimInfo[0].Rlev=XtfRead->XtfCurve.rlevcv*IndexSacle;

	if(XtfRead->XtfCurve.Ictype==1)
	{
		sprintf(FidWrite->FidChannel[index].DimInfo[1].Name,"%-.8s",XtfRead->XtfCurve.Name);
		sprintf(FidWrite->FidChannel[index].DimInfo[1].Unit,"%-.8s",NewUnits.GetAt(index));
		FidWrite->FidChannel[index].DimInfo[1].RepCode=FID_REPR_FLOAT;
		FidWrite->FidChannel[index].DimInfo[1].CodeLen=WisCodeLen(FidWrite->FidChannel[index].DimInfo[1].RepCode);
		FidWrite->FidChannel[index].DimInfo[1].Nps=1;
		FidWrite->FidChannel[index].DimInfo[1].Npw=1;
		FidWrite->FidChannel[index].DimInfo[1].Start=XtfRead->XtfCurve.curvmin;
		FidWrite->FidChannel[index].DimInfo[1].Stop=XtfRead->XtfCurve.curvmax;
		FidWrite->FidChannel[index].DimInfo[1].Rlev=0;
	}
	else if((XtfRead->XtfCurve.Ictype==2) || (XtfRead->XtfCurve.Ictype==3))
	{
		sprintf(FidWrite->FidChannel[index].DimInfo[1].Name,"%-.8s","T");
		sprintf(FidWrite->FidChannel[index].DimInfo[1].Unit,"%-.8s",XtfRead->XtfCurve.TimeIncrementUnits);
		FidWrite->FidChannel[index].DimInfo[1].RepCode=FID_REPR_FLOAT;
		FidWrite->FidChannel[index].DimInfo[1].CodeLen=4;
		FidWrite->FidChannel[index].DimInfo[1].Nps=XtfRead->XtfCurve.Idims[0];
		FidWrite->FidChannel[index].DimInfo[1].Npw=0;

		//时间采样间隔为0的“波形曲线，实际上是多个常规曲线的“并”
		FidWrite->FidChannel[index].DimInfo[1].Start=XtfRead->XtfCurve.starttm;
		FidWrite->FidChannel[index].DimInfo[1].Rlev=XtfRead->XtfCurve.timeinc;
		FidWrite->FidChannel[index].DimInfo[1].Stop=XtfRead->XtfCurve.starttm+(XtfRead->XtfCurve.Idims[0]-1)*FidWrite->FidChannel[index].DimInfo[1].Rlev;
		if(XtfRead->XtfCurve.timeinc==0)
		{
			FidWrite->FidChannel[index].DimInfo[1].Rlev=5; //强制采样间隔，如果目标软件能识别则不必
			FidWrite->FidChannel[index].DimInfo[1].Stop=XtfRead->XtfCurve.starttm+(XtfRead->XtfCurve.Idims[0]-1)*FidWrite->FidChannel[index].DimInfo[1].Rlev;
		}

		sprintf(FidWrite->FidChannel[index].DimInfo[2].Name,"%-.8s",XtfRead->XtfCurve.Name);
		sprintf(FidWrite->FidChannel[index].DimInfo[2].Unit,"%-.8s",NewUnits.GetAt(index));

		FidWrite->FidChannel[index].DimInfo[2].RepCode=FID_REPR_SHORT;
		FidWrite->FidChannel[index].DimInfo[2].CodeLen=2;
		if((XtfRead->XtfCurve.RepCode==XTF_REPR_BIT)||(XtfRead->XtfCurve.RepCode==XTF_REPR_CHAR))
		{
			FidWrite->FidChannel[index].DimInfo[2].RepCode=FID_REPR_CHAR;
			FidWrite->FidChannel[index].DimInfo[2].CodeLen=1;
		}
		if((XtfRead->XtfCurve.RepCode==XTF_REPR_INT2)||(XtfRead->XtfCurve.RepCode==XTF_REPR_UCHAR))
		{
			FidWrite->FidChannel[index].DimInfo[2].RepCode=FID_REPR_SHORT;
			FidWrite->FidChannel[index].DimInfo[2].CodeLen=2;
		}
		if((XtfRead->XtfCurve.RepCode==XTF_REPR_FLOAT)||(XtfRead->XtfCurve.RepCode==XTF_REPR_UINT4))
		{
			FidWrite->FidChannel[index].DimInfo[2].RepCode=FID_REPR_FLOAT;
			FidWrite->FidChannel[index].DimInfo[2].CodeLen=4;
		}
		if(XtfRead->XtfCurve.RepCode==XTF_REPR_DOUBLE)
		{
			FidWrite->FidChannel[index].DimInfo[2].RepCode=FID_REPR_DOUBLE;
			FidWrite->FidChannel[index].DimInfo[2].CodeLen=8;
		}
		if((XtfRead->XtfCurve.RepCode==XTF_REPR_UINT2)||(XtfRead->XtfCurve.RepCode==XTF_REPR_INT4))
		{
			FidWrite->FidChannel[index].DimInfo[2].RepCode=FID_REPR_LONG;
			FidWrite->FidChannel[index].DimInfo[2].CodeLen=4;
		}
		FidWrite->FidChannel[index].DimInfo[2].Nps=XtfRead->XtfCurve.Idims[0];
		FidWrite->FidChannel[index].DimInfo[2].Npw=XtfRead->XtfCurve.Idims[0];
		FidWrite->FidChannel[index].DimInfo[2].Start=XtfRead->XtfCurve.curvmin;
		FidWrite->FidChannel[index].DimInfo[2].Stop=XtfRead->XtfCurve.curvmax;
		FidWrite->FidChannel[index].DimInfo[2].Rlev=FidWrite->FidChannel[index].DimInfo[1].Rlev;
		
	}
	//深度单位修正
	if(strnicmp(FidWrite->FidChannel[index].DimInfo[0].Unit,"none",4)==0)
	{
		strcpy(FidWrite->FidChannel[index].DimInfo[0].Unit,"m");
	}
	//其它单位的修正
	for(int i=0;i<FidWrite->FidChannel[index].NumOfDimension;i++)
	{
		s.Format ("%-.8s",FidWrite->FidChannel[index].DimInfo[i].Unit);
		s.TrimRight();
		if(s.IsEmpty())
		{
			sprintf(FidWrite->FidChannel[index].DimInfo[i].Unit,"%-.8s","none");
		}
	}
}

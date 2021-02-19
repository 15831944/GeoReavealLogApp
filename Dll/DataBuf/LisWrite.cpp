// LisWrite.cpp: implementation of the CLisWrite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LisWrite.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLisWrite::CLisWrite()
{
	Dsbr=NULL;
}

CLisWrite::~CLisWrite()
{
	if(Dsbr)
	{
		delete []Dsbr; Dsbr=NULL;
	}
}
bool CLisWrite::Open(CString sDataFile)
{
	if(fLis.Open(sDataFile,CFile::modeCreate|CFile::modeReadWrite)==FALSE)
	{
		return false;
	}
	return true;
}
void CLisWrite::Close()
{
	fLis.Close();
}
void CLisWrite::WriteRTHT(int type)
{
	LisBuf[0]=type;
	LisBuf[1]=0;
	memcpy(&LisBuf[2],TapeHead.SerName,6);
	memcpy(&LisBuf[8],TapeHead.Nul1,6);
	memcpy(&LisBuf[14],TapeHead.Date,8);
	memcpy(&LisBuf[22],TapeHead.Nul2,2);
	memcpy(&LisBuf[24],TapeHead.OriginData,4);
	memcpy(&LisBuf[28],TapeHead.Nul3,2);
	memcpy(&LisBuf[30],TapeHead.Name,8);
	memcpy(&LisBuf[38],TapeHead.Nul4,2);
	memcpy(&LisBuf[40],TapeHead.ConnectNum,2);
	memcpy(&LisBuf[42],TapeHead.Nul5,2);
	memcpy(&LisBuf[44],TapeHead.PreConName,8);
	memcpy(&LisBuf[52],TapeHead.Nul6,2);
	memcpy(&LisBuf[54],TapeHead.Comments,74);
	NumRecLen=128;
	WriteBuf(NumRecLen,LisBuf);
}
void CLisWrite::WriteEOF()
{
	LisBuf[0]=0X89;
	LisBuf[1]=0;
	NumRecLen=2;
	WriteBuf(NumRecLen,LisBuf);
}
int CLisWrite::GetCodeLength(unsigned char RepCode)
{
	int n;
	switch(RepCode)
	{
    case 49: n=2; break; //16位浮点
    case 50: n=4; break; //32位低分辨率浮点
    case 56: n=1; break; //8位补码整数
    case 66: n=1; break; //8位无符号整数
    case 68: n=4; break; //32位浮点
    case 70: n=4; break; //32位固定点数
    case 73: n=4; break; //32位整数
    case 77: n=0; break; //“屏蔽尺”
    case 79: n=2; break; //16位整数
    case 128: n=1; break; //“毛胚”数据块
    case 234: n=1; break; //
    default: n=1; break;
	}
	return n;
}
void CLisWrite::WriteFHR(int type)
{
	LisBuf[0]=type;
	LisBuf[1]=0;
	memcpy(&LisBuf[2],FileHead.Name,10);  //Filename
	memcpy(&LisBuf[12],FileHead.Nul1,2);  //Undefined
	memcpy(&LisBuf[14],FileHead.SubSerName,6); //Sub-Service Name
	memcpy(&LisBuf[20],FileHead.Version,8);  //Version
	memcpy(&LisBuf[28],FileHead.Date,8); //Date
	LisBuf[36]=FileHead.Nul2;  //Undefined
	memcpy(&LisBuf[37],FileHead.MaxPRL,5); //Maximum Physical Record Length
	memcpy(&LisBuf[42],FileHead.Nul3,2); //Undefined
	memcpy(&LisBuf[44],FileHead.FileType,2); //File Type
	memcpy(&LisBuf[46],FileHead.Nul4,2); //Undefined
	memcpy(&LisBuf[48],FileHead.PreConName,10); //Predecessor Continuation Filename
	NumRecLen=60;
	WriteBuf(NumRecLen,LisBuf);
}
void CLisWrite::Int16ToLis79I(short vh,BYTE *str)
{
	str[0]=(vh>>8)&0xff;
	str[1]=vh&0xff;
}

void CLisWrite::Int32ToLis73I(int vi,BYTE *str)
{
	str[0]=(vi>>24)&0xff;
	str[1]=(vi>>16)&0xff;
	str[2]=(vi>>8)&0xff;
	str[3]=vi&0xff;
}
void CLisWrite::Float32ToLIS68F(float vf,BYTE *str)
{
	union F4CH {long vi; char ch[4];};
	union F4CH f4ch;
	long i,n;
	long f;
	if(fabs(vf)==0.0)
	{
		str[0]=0;
		str[1]=0;
		str[2]=0;
		str[2]=0;
		return;
	}
	if(vf>=0.0)
	{
		n=(int)(log(vf)/log(2.0))+1;
		f=(long)(vf*pow(2.0,(double)(23-n)));
		f4ch.vi=((n+128)<<23)|f;
	}
	else
	{
		vf=(float)fabs(vf);
		n=(int)(log(vf)/log(2.0))+1;
		f=(long)(vf*pow(2.0,(double)(23-n)));
		f=((~f)+1)&0X007FFFFF;
		f4ch.vi=0X80000000|((127-n)<<23)|f;
	}
	for(i=0;i<4;i++)str[i]=f4ch.ch[3-i];
}
void CLisWrite::Float32ToLIS73I(float vf,BYTE *str)
{
	int vi=int(vf);
	str[0]=(vi>>24)&0xff;
	str[1]=(vi>>16)&0xff;
	str[2]=(vi>>8)&0xff;
	str[3]=vi&0xff;

}
void CLisWrite::Float32ToLIS79I(float vf,BYTE *str)
{
	short vh=short(vf);
	str[0]=(vh>>8)&0xff;
	str[1]=vh&0xff;
}
void CLisWrite::Float32ToLIS65C(float vf,BYTE *str)
{
	str[0]=char(vf);
}

void CLisWrite::WriteEB()
{
	LisBuf[0]=64;  //逻辑记录类型
	LisBuf[1]=0;  //无意义
	//////////////////////////////
	int NumEB=0; //条目块总长度
	BYTE str[256];
	//条目块：条目类型号、条目本身长度、条目信息代码号、条目本身
	LisBuf[2]=1;	LisBuf[3]=1;	LisBuf[4]=66;	LisBuf[5]=EnVa.DaRecTyp; //1:数据记录的逻辑类型号
	LisBuf[6]=2;	LisBuf[7]=1;	LisBuf[8]=66;	LisBuf[9]=EnVa.DSBTyp; //2:数据说明块类型号
	LisBuf[10]=3;	LisBuf[11]=2;	LisBuf[12]=79;	LisBuf[13]=EnVa.DaFraSiz>>8; LisBuf[14]=EnVa.DaFraSiz&0xFF;
	LisBuf[15]=4;	LisBuf[16]=1;	LisBuf[17]=66;	LisBuf[18]=EnVa.UDFlag; //4:上测/下测标志,1为上测
	LisBuf[19]=5;	LisBuf[20]=1;	LisBuf[21]=66;	LisBuf[22]=EnVa.DepUnitFlag; //5:深度单位标志,1为英尺，255为米,0为时间
	LisBuf[23]=8;	LisBuf[24]=4;	LisBuf[25]=68;	Float32ToLIS68F(EnVa.FraSpace,str); memcpy(&LisBuf[26],str,4);
	LisBuf[30]=9;	LisBuf[31]=4;	LisBuf[32]=65;	memcpy(&LisBuf[33],EnVa.FraUnt,4);
	LisBuf[37]=11;	LisBuf[38]=1;	LisBuf[39]=66;	LisBuf[40]=EnVa.MaxFraRec;
	LisBuf[41]=12;	LisBuf[42]=4;	LisBuf[43]=68;	Float32ToLIS68F(EnVa.AbsVal,str); memcpy(&LisBuf[44],str,4);
	LisBuf[48]=13;	LisBuf[49]=1;	LisBuf[50]=66;	LisBuf[51]=EnVa.DepMod;
	LisBuf[52]=14;	LisBuf[53]=4;	LisBuf[54]=65;	memcpy(&LisBuf[55],EnVa.DepUnt,4);
	LisBuf[59]=15;	LisBuf[60]=1;	LisBuf[61]=66;	LisBuf[62]=EnVa.DepRpc;
	LisBuf[63]=16;	LisBuf[64]=1;	LisBuf[65]=66;	LisBuf[66]=EnVa.DSBSTyp;
	LisBuf[67]=0;	LisBuf[68]=0;	LisBuf[69]=65;
	NumEB=70; 
	//格式说明块
	for(int i=0;i<NumLog;i++)
	{
		memcpy(&LisBuf[NumEB+i*40],Dsbr[i].Mnemon,4);
		memcpy(&LisBuf[NumEB+i*40+4],Dsbr[i].ServID,6);
		memcpy(&LisBuf[NumEB+i*40+10],Dsbr[i].ServOrd,8);
		memcpy(&LisBuf[NumEB+i*40+18],Dsbr[i].Units,4);
		Int32ToLis73I(Dsbr[i].APICode,str); 
		memcpy(&LisBuf[NumEB+i*40+22],str,4);
		LisBuf[NumEB+i*40+26]=Dsbr[i].FileNumber>>8;
		LisBuf[NumEB+i*40+27]=Dsbr[i].FileNumber&0XFF;
		LisBuf[NumEB+i*40+28]=Dsbr[i].Size>>8;
		LisBuf[NumEB+i*40+29]=Dsbr[i].Size&0XFF;
		LisBuf[NumEB+i*40+30]=Dsbr[i].Nul>>8;
		LisBuf[NumEB+i*40+31]=Dsbr[i].Nul&0XFF;
		LisBuf[NumEB+i*40+32]=Dsbr[i].ProLvl;
		LisBuf[NumEB+i*40+33]=Dsbr[i].Samples;
		LisBuf[NumEB+i*40+34]=Dsbr[i].RepC;
		memcpy(&LisBuf[NumEB+i*40+35],Dsbr[i].ProInd,5);
	}
	NumRecLen=NumEB+40*NumLog;
	WriteBuf(NumRecLen,LisBuf);
	LogicalRecLen=6+EnVa.DaFraSiz*EnVa.MaxFraRec; //逻辑记录长度
}
void CLisWrite::WriteBuf(int NumBytes,unsigned char *buf)
{
	BYTE PhysicalHeader[4]; //物理记录头
	int BlockSize=MAX_PHYRECORD_LENGTH-4; //不包括物理头的物理记录最大长度
	int NumBlock=(NumBytes-1)/BlockSize+1; //块数
	for(int i=0;i<NumBlock;i++)
	{
		int NumStart=i*BlockSize;
		int NumStop=NumStart+BlockSize;
		if(NumStop>=NumBytes) NumStop=NumBytes; 
		int NumWb=NumStop-NumStart; //实际写的字节
		WORD PhyLen=NumWb+4; //物理记录长度
		WORD PhyAtt=0; //记录属性
		if(i>0)PhyAtt=2; //前邻连接
		if(NumStop<NumBytes)PhyAtt=PhyAtt|1; //后邻连接
		PhysicalHeader[0]=PhyLen>>8;
		PhysicalHeader[1]=PhyLen&0XFF;
		PhysicalHeader[2]=PhyAtt>>8;
		PhysicalHeader[3]=PhyAtt&0XFF;
		fLis.Write(PhysicalHeader,4); //物理记录头
		fLis.Write(&buf[NumStart],NumWb); //记录内容
	}
}
void CLisWrite::StartWriteData()
{
	CString s;
	if(IsFileName(sMidFile))
	{
		remove((const char*)sMidFile);
	}
	TCHAR szTempPath  [MAX_PATH];
	DWORD dwResult=:: GetTempPath (MAX_PATH, szTempPath);
	ASSERT (dwResult);
	sMidFile.Format("%s~LisData.tmp",szTempPath);
	if(fTmp.Open(sMidFile,CFile::modeCreate|CFile::modeWrite)==FALSE) 
	{
		s.Format("创建临时文件 %s 出错!",sMidFile);
		AfxMessageBox(s,MB_ICONSTOP);
		return;
	}
	int number=int(fabs((EnDep-StDep)/Rlev)+0.5)+1;
	int fs=number*EnVa.DaFraSiz;
	fTmp.SetLength(fs);
}
void CLisWrite::StopWriteData()
{
	fTmp.Close();
	int NumFra=int((EnDep-StDep)/Rlev+0.5)+1; //根据深度计算的数据块数
	if(fTmp.Open(sMidFile,CFile::modeRead|CFile::shareDenyNone)==FALSE) 
	{
		return;
	}
	int fs=fTmp.GetLength();
	int NumBlock=fs/EnVa.DaFraSiz; //磁盘实际存在的数据块数
	if(NumBlock<NumFra)
	{
		NumFra=NumBlock;
	}
	int NumW=0; //输出记录中的帧数
	NumRecLen=0;
	for(int i=0;i<NumFra;i++)
	{
		float FraDep=StDep+i*Rlev;
		if(NumW==0)
		{
			LisBuf[0]=0;
			LisBuf[1]=0;
			NumRecLen=2;
			Float32ToLIS68F(FraDep,&LisBuf[NumRecLen]);
			NumRecLen+=4;
		}
		fTmp.Read(&LisBuf[NumRecLen],EnVa.DaFraSiz);
		NumRecLen+=EnVa.DaFraSiz;
		NumW++;
		if((NumW==EnVa.MaxFraRec) || (i==(NumFra-1)))
		{
			WriteBuf(NumRecLen,LisBuf);
			NumW=0;
		}
	}
	fTmp.Close();
	remove(sMidFile);
}

void CLisWrite::WriteCurveData(int index,float sdep,float edep,float rlev,int NumR,float *buf)
{
	const double er=0.00001; //深度误差
	int number=int((EnDep-StDep)/Rlev+0.5)+1;
	float *vf=new float[Dsbr[index].Samples];
	int OffSet=0;
	for(int i=0;i<index;i++)
	{
		OffSet+=Dsbr[i].Size;
	}
	edep=sdep+(NumR-1)*rlev;
	for(i=0;i<number;i++)
	{
		NumRecLen=0;
		float FraDep=StDep+i*Rlev;
		for(int j=0;j<Dsbr[index].Samples;j++)
		{
			float dept=FraDep+j*rlev;
			vf[j]=-999.25;
			if(fabs(dept-sdep)<er)
			{
				vf[j]=buf[0];	
			}
			else if(fabs(dept-edep)<er)
			{
				vf[j]=buf[NumR-1];
			}
			else if(dept<sdep || dept>edep)
			{
				vf[j]=-999.25;
			}
			else
			{
				float xk=(dept-sdep)/rlev;
				int k1=int(xk);
				int k2=k1+1;
				if(buf[k1]==-9999.0 || buf[k1]==-999.25)
				{
					vf[j]=buf[k2];
				}
				else if(buf[k2]==-9999.0 || buf[k2]==-999.25)
				{
					vf[j]=buf[k1];
				}
				else
				{
					if(fabs(xk-k1)<er)
					{
						vf[j]=buf[k1];
					}
					else if(fabs(xk-k2)<er)
					{
						vf[j]=buf[k2];
					}
					else
					{
						vf[j]=buf[k1]+(xk-k1)*(buf[k2]-buf[k1]);
					}
				}
			}
			if(vf[j]==-9999.0) vf[j]=-999.25;
			switch(Dsbr[index].RepC)
			{
			case 68:
				Float32ToLIS68F(vf[j],&LisBuf[NumRecLen]);
				NumRecLen+=4;
				break;
			case 73:
				Float32ToLIS73I(vf[j],&LisBuf[NumRecLen]);
				NumRecLen+=4;
				break;
			case 79:
				Float32ToLIS79I(vf[j],&LisBuf[NumRecLen]);
				NumRecLen+=2;
				break;
			default:
				Float32ToLIS65C(vf[j],&LisBuf[NumRecLen]);
				NumRecLen+=1;
				break;
			}
		}
		int pos=i*EnVa.DaFraSiz+OffSet;
		fTmp.Seek(pos,CFile::begin);
		fTmp.Write(LisBuf,NumRecLen);
	}
	delete []vf; vf=NULL;
}
void CLisWrite::WriteWaveData(int index,float depth,int NumX,float *buf)
{
	NumRecLen=0;
	for(int i=0;i<NumX;i++)
	{
		switch(Dsbr[index].RepC)
		{
		case 68:
			Float32ToLIS68F(buf[i],&LisBuf[NumRecLen]);
			NumRecLen+=4;
			break;
		case 73:
			Float32ToLIS73I(buf[i],&LisBuf[NumRecLen]);
			NumRecLen+=4;
			break;
		case 79:
			Float32ToLIS79I(buf[i],&LisBuf[NumRecLen]);
			NumRecLen+=2;
			break;
		default:
			Float32ToLIS65C(buf[i],&LisBuf[NumRecLen]);
			NumRecLen+=1;
			break;
		}
	}
	int OffSet=0;
	for(i=0;i<index;i++)
	{
		OffSet+=Dsbr[i].Size;
	}
	int np=int((depth-StDep)/Rlev+0.5);
	int pos=np*EnVa.DaFraSiz+OffSet;
	fTmp.Seek(pos,CFile::begin);
	fTmp.Write(LisBuf,NumRecLen);
}

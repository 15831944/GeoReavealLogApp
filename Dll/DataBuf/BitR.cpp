// BitR.cpp: implementation of the CBitR class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BitR.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBitR::CBitR()
{

}

CBitR::~CBitR()
{

}
bool _declspec(dllexport) IsBIT(CString sDataFile)
{
	CString s;
	s=MyGetFileExt(sDataFile);
	if(s.CompareNoCase("bit")!=0)
	{
		return false; //BIT文件名的扩展名必须是BIT
	}
	CFile f;
	BIT_HEAD bit;
	if(f.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE) return false;
	f.Read(&bit,sizeof(bit));
	f.Close();
	SunI2TOPC(bit.NumLog);
	GetBitR4(bit.Stdep);
	GetBitR4(bit.Endep);
	if(fabs(bit.Stdep-bit.Endep)<0.01)
	{
		return false;
	}
	if(bit.NumLog<=0 || bit.NumLog>20)
	{
		return false; //BIT文件的曲线数为1-20之间
	}
	for(int i=0;i<bit.NumLog;i++)
	{
		if(!isalpha(bit.CurveName[i][0])) return false; //曲线名首字符必须是字母
	}
	return true;
}
bool CBitR::Open(CString sDataFile)
{
	if(fBit.Open(sDataFile,CFile::modeRead|CFile::shareDenyNone)==FALSE)
	{
		return false;
	}
	FileSize=fBit.GetLength(); //文件长度
	return true;
}
bool CBitR::ReadHead()
{
	fBit.SeekToBegin();
	fBit.Read(&BitHead,sizeof(BitHead));
	SunI2TOPC(BitHead.NumSample);
	SunI2TOPC(BitHead.nMonth);
	SunI2TOPC(BitHead.nDay);
	SunI2TOPC(BitHead.nYear);
	SunI2TOPC(BitHead.nHour);
	SunI2TOPC(BitHead.nMinute);
	SunI2TOPC(BitHead.nSecond);

	SunI2TOPC(BitHead.NumLog);
	SunI2TOPC(BitHead.UnitFlag);
	GetBitR4(BitHead.Stdep);
	GetBitR4(BitHead.Endep);
	GetBitR4(BitHead.Rlev);
	SunI2TOPC(BitHead.Idc);
	SunI2TOPC(BitHead.DataFlag);
	GetBitR4(BitHead.Spcpr);

	if(fabs(BitHead.Rlev)<0.0001)
	{
		//由于BIT文件中有些采样间隔为0，所以便采用强制
		BitHead.Rlev=1;
	}
	//根据文件大小和Spcpr的值计算终止深度
	int WFSize=BitHead.NumWF*BitHead.NumSample;
	if(int(BitHead.DataFlag)>1)
	{
		bWaveform=true; //是否为波形数据，TRUE表示波形
		NumBlock=(FileSize-sizeof(BitHead))/((4*BitHead.NumLog+WFSize)*int(BitHead.Spcpr));
		NumPoint=NumBlock;
		for(int i=0;i<BitHead.NumWF;i++)
		{
			char str[20];
			sprintf(str,"WF%d  ",i+1);
			memcpy(BitHead.CurveName[BitHead.NumLog+i],str,4);
		}
		BitHead.NumLog+=BitHead.NumWF; //增加波形曲线数
	}
	else
	{
		bWaveform=false; //是否为波形数据，false表示无波形
		NumBlock=(FileSize-sizeof(BitHead))/(4*BitHead.NumLog*int(BitHead.Spcpr));
		NumPoint=NumBlock*int(BitHead.Spcpr);
	}
	//如果文件中没有确切的结束深度，则根据文件中记录信息进行计算
	if(fabs(BitHead.Endep)<0.01)
	{
		//文件中结束深度为0，表示上测
		BitHead.Endep=BitHead.Stdep-(NumPoint-1)*BitHead.Rlev;
	}
	else if(fabs(BitHead.Endep-99999.0)<0.01)
	{
		//文件中结束深度为99999，表示下测
		BitHead.Endep=BitHead.Stdep+(NumPoint-1)*BitHead.Rlev;
	}
	//根据采样间隔判断是否为分米单位，是则转换为米
	if(fabs(BitHead.Rlev-1)<0.001)
	{
		BitHead.Stdep*=0.1f;
		BitHead.Endep*=0.1f;
		BitHead.Rlev*=0.1f;
	}
	//比较起始和结束深度来判断测井方向
	if(BitHead.Stdep>BitHead.Endep)
	{
		BitHead.Rlev=-BitHead.Rlev; //采样间隔为负表示上测
	}

	//一个块的大小
	nBlockSize=4*BitHead.NumLog*int(BitHead.Spcpr);
	if(bWaveform)
	{
		//文件中存在波形数据
		nBlockSize=nBlockSize+WFSize-4*4; //增加4条波形曲线再去掉它们本身点用的空间
	}
	if(bWaveform)
	{
		NumConvLog=BitHead.NumLog-4;//常规曲线数
		NumWaveLog=4; //波形曲线数
	}
	else
	{
		NumConvLog=BitHead.NumLog;//常规曲线数
		NumWaveLog=0; //波形曲线数
	}

	return true;
}

void CBitR::ClearHead()
{
	//对头信息进行过滤
	for(int i=71;i>=0;i--)
	{
		if(BitHead.WellName[i]!=' ' && BitHead.WellName[i]!=0x0)break;
		BitHead.WellName[i]=0;
	}
	for(i=71;i>=0;i--)
	{
		if(BitHead.CompanyName[i]!=' ' && BitHead.CompanyName[i]!=0x0)break;
		BitHead.CompanyName[i]=0;
	}

	for(i=BitHead.NumLog;i<20;i++)
	{
		for(int j=0;j<4;j++)
		{
			BitHead.CurveName[i][j]=32;
		}
	}
}

bool CBitR::ReadChannel(int Index,float *buf)
{
	for(int i=0;i<NumBlock;i++)
	{
		int pos=sizeof(BitHead)+i*nBlockSize+4*Index*int(BitHead.Spcpr);
		fBit.Seek(pos,CFile::begin);
		fBit.Read(&buf[i*int(BitHead.Spcpr)],4*int(BitHead.Spcpr));
	}
	//浮点解码
	for(i=0;i<NumPoint;i++)
	{
		GetBitR4(buf[i]);
	}
	//如果为上测，则将数据重新排序
	if(BitHead.Stdep>BitHead.Endep)
	{
		for(int i=0;i<NumPoint/2;i++)
		{
			float fTemp=buf[i];
			buf[i]=buf[NumPoint-i-1];
			buf[NumPoint-i-1]=fTemp;
		}
	}
	return true;
}
bool CBitR::ReadWaveform(float Depth,char *buf)
{
	if(!bWaveform)
	{
		return false; //文件中无波形数据
	}
	int nRec=int(fabs((Depth-BitHead.Stdep)/BitHead.Rlev)+0.5);
	if(nRec<0 || nRec>=NumBlock) return false;
	int pos=sizeof(BitHead)+nRec*nBlockSize+4*(BitHead.NumLog-4);
	fBit.Seek(pos,CFile::begin);
	fBit.Read(buf,BitHead.NumSample*BitHead.NumWF);
	return true;
}
bool CBitR::ReadWaveform(int nWave,float Depth,char *buf)
{
	CString s;
	if(!bWaveform)
	{
		AfxMessageBox("文件中无波形曲线");
		return false; //文件中无波形数据
	}
	if(nWave<0 || nWave>=4)
	{
		s.Format ("波形曲线的索引号=%d，超出范围(0-3)",nWave);
		AfxMessageBox(s);
		return false; //只有4条波形
	}
	int nRec=int(fabs((Depth-BitHead.Stdep)/BitHead.Rlev)+0.5);
	if(nRec<0 || nRec>=NumBlock) return false;
	int pos=sizeof(BitHead)+nRec*nBlockSize+4*(BitHead.NumLog-4)+nWave*BitHead.NumSample;
	fBit.Seek(pos,CFile::begin);
	fBit.Read(buf,BitHead.NumSample);
	return true;
}

void CBitR::Close()
{
	fBit.Close();
}

// InputReader.cpp: implementation of the InputReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InputReader.h"
#include <fstream>
#include <sstream>

#include "GeoAdo.h"
#include "GeoStorage.h"
using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CInputReader::Read(string RootName,DWORD RootID,bool b3d) throw()
{
	CWaitCursor wait;
	CGeoAdo	ado;
	if(!ado.InitData(m_szSever)) 
		return;

	CGeoStorage storage;
	if(!storage.InitData(m_szSever))
		return;
	storage.LoadStorageInfo(RootID);

	double x, y, z;
	double x_max=-327670000.;
	double x_min=327670000.;
	double y_max=-327670000.;
	double y_min=327670000.;
	int	   z_max=-327670000;
	int	   z_min=327670000;
	
	CString	Str;
	DWORD i,RecNum,UpperID;
	_RecordsetPtr pRS = ado.GetRS("SELECT * FROM Well");

	RecNum=storage.a_UpperID.GetSize();
	while(pRS->adoEOF==0)
	{
		UpperID=atol((LPCTSTR)(_bstr_t)pRS->GetCollect("UpperID"));
		for(i=0; i<RecNum; i++)
		{
			// 打开a_UpperID数组中等于UpperID的元素
			if(UpperID == atol(storage.a_UpperID.GetAt(i)))
			{
				Str= (LPCTSTR)(_bstr_t)pRS->GetCollect("WellY");Str.Trim();
				if(Str=="") break;
                x=atof(Str);
				if(x<1.)  break;
				if(x>x_max) x_max=x;
				if(x<x_min) x_min=x;

				Str= (LPCTSTR)(_bstr_t)pRS->GetCollect("WellX");Str.Trim();
				if(Str=="") break;
				y=atof(Str);
				if(y<1.)  break;
				if(y>y_max) y_max=y;
				if(y<y_min) y_min=y;

				Str= (LPCTSTR)(_bstr_t)pRS->GetCollect("ElevationGd");Str.Trim();
				if(Str=="") break;
				z=(int)atof(Str);
				if(z>z_max) z_max=z;
				if(z<z_min) z_min=z;
				break;
			}
		}
		pRS->MoveNext();
	}
	

	//处理图标点显示边界,图像缺省长宽为600,坐标刻度偏移10个像素点
	x_max+=(x_max-x_min)/60;x_min-=(x_max-x_min)/60;
	y_max+=(y_max-y_min)/60;y_min-=(y_max-y_min)/60;

	m_XMax=x_max;
	m_XMin=x_min;
	m_YMax=y_max;
	m_YMin=y_min;
	m_ZMax=z_max;
	m_ZMin=z_min;

	double nDiameter=(double)m_nDiameter;
	if(b3d) nDiameter=(double)m_nDiameter3D;
	CString zStr,WStr;
	int Num=0,Num0=0;
	
	m_vecPoints.clear();
	m_Points.clear();

	pRS = ado.GetRS("SELECT * FROM Well");
	while(pRS->adoEOF==0)
	{
		UpperID=atol((LPCTSTR)(_bstr_t)pRS->GetCollect("UpperID"));
		for(i=0; i<RecNum; i++)
		{
			// 打开a_UpperID数组中等于UpperID的元素
			if(UpperID == atol(storage.a_UpperID.GetAt(i)))
			{
				//井别名(中文名)
				WStr= (LPCTSTR)(_bstr_t)pRS->GetCollect("Alias");WStr.Trim();
				Str= (LPCTSTR)(_bstr_t)pRS->GetCollect("WellY");Str.Trim();
				if(Str=="") break;
                x=atof(Str);
				if(x<1.)  break;
				
				Str= (LPCTSTR)(_bstr_t)pRS->GetCollect("WellX");Str.Trim();
				if(Str=="") break;
				y=atof(Str);
				if(y<1.)  break;

				x=(x-x_min)/(x_max-x_min)*nDiameter-nDiameter/2;
				y=(y-y_min)/(y_max-y_min)*nDiameter-nDiameter/2;
				zStr= (LPCTSTR)(_bstr_t)pRS->GetCollect("ElevationGd");zStr.Trim();
				if(zStr!="")
				{
					z=atof(zStr);
					m_vecPoints.push_back(Point3D(x, y, z,WStr));
					Num++;
				}
				else
				{
					m_Points.push_back(Point3D(x, y,-999.25,WStr));
					Num0++;
				}
				break;
			}
		}
		pRS->MoveNext();
	}
	m_vecNum=Num;
	m_PointNum=Num0;
}

void CInputReader::Dump(ostream& os) const throw()
{
	for(DWORD i=0; i<m_vecPoints.size(); i++)
		os << m_vecPoints[i].x << "\t" << m_vecPoints[i].y << "\t" << m_vecPoints[i].z << endl;
}
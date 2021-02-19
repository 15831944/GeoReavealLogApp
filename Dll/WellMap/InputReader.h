// InputReader.h: interface for the InputReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUTREADER_H__5975EFD1_AEFB_4A37_AF24_80EB0E8894D3__INCLUDED_)
#define AFX_INPUTREADER_H__5975EFD1_AEFB_4A37_AF24_80EB0E8894D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include "Interpolater.h"
#include "GeoStorage.h"

using namespace std;
class CInputReader  
{
public:
	CString m_szSever;

	double	m_XMin,m_XMax;int m_XScale;	//X轴刻度
	double	m_YMin,m_YMax;int m_YScale;	//Y轴刻度
	int 	m_ZMin,m_ZMax,m_ZScaleNum;	//Z轴刻度
	int		m_zDataMin,m_zDataMax;		//记录数据表文件中Z轴最小最大值
	int		m_nDiameter;				//图形宽度
	int		m_nDiameter3D;				//3D图形宽度
	int		m_PointNum,m_vecNum;
public:
	CInputReader() throw(){};
	//CInputReader(string filename,bool b3d) throw() { Read(filename,b3d); }
	void Read(string RootName,DWORD RootID,bool b3d) throw();
	const vector<Point3D>& Get3DPoints() const throw() { return m_vecPoints; }
	const vector<Point3D>& GetPoints() const throw() { return m_Points; }
	void Dump(ostream& os) const throw();
private:
	vector<Point3D> m_vecPoints;
	vector<Point3D> m_Points;
};

#endif // !defined(AFX_INPUTREADER_H__5975EFD1_AEFB_4A37_AF24_80EB0E8894D3__INCLUDED_)

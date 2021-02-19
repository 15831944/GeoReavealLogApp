// InverseDist.h: interface for the InverseDist class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INVERSEDIST_H__BB8C4FFF_A66C_4669_8B04_7D3080F5066A__INCLUDED_)
#define AFX_INVERSEDIST_H__BB8C4FFF_A66C_4669_8B04_7D3080F5066A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
using namespace std;

template<typename T>
struct TPoint3D
{
	TPoint3D() : x(0), y(0), z(0),v(0){}

	TPoint3D(T xx, T yy, T zz, T vv) 
		: x(xx), y(yy), z(zz),v(vv) {}
	T x;
	T y;
	T z;
	T v;
};
typedef TPoint3D<double> Point3D;

struct WeightAndZ
{
	WeightAndZ(double w, double zz) : weight(w), z(zz) {}
	double weight;
	double z;
};

template<class ForwardIterator>
class CInsert3D
{
public:
	CInsert3D(double dRadius=400, DWORD dExponent=8) : m_dRadius(dRadius), m_dExponent(dExponent) {}
	double GetInsertV(double xpos, double ypos, double zpos,ForwardIterator first, ForwardIterator last) 
	{
		double distance = 0;
		double weight = 0;
		double totalWeight = 0;
		DWORD i;
		vector<WeightAndZ> vecWeight;
		ForwardIterator start = first;
		while(start != last) 
		{
			distance = ::sqrt((xpos-(*start).x)*(xpos-(*start).x)+(ypos-(*start).y)*(ypos-(*start).y)+(zpos-(*start).z)*(zpos-(*start).z));
			if(distance == 0) // exact match
				return (*start).v;

			if(distance > m_dRadius) 
			{
				++start;
				continue;
			}
			
			/*weight = 1 / ::pow(distance, m_dExponent);*/
			// 为了提高效率 权重反间距处理时不用 pow函数
			weight = 1/distance;
			for(i=1;i<(DWORD)m_dExponent;i++)
				weight/=distance;

			vecWeight.push_back(WeightAndZ(weight, (*start).v));
			totalWeight += weight;
			++start;
		}

		for(i=0; i<vecWeight.size(); i++)
			vecWeight[i].weight /= totalWeight;

		double nodeValue = 0;
		for(i=0; i<vecWeight.size(); i++) 
		{
			nodeValue += vecWeight[i].weight * vecWeight[i].z;
			++first;
		}
		return nodeValue;
	}
private:
	double m_dRadius;
	DWORD m_dExponent;
};
//
typedef CInsert3D<Point3D*> Insert3D;

#endif // !defined(AFX_INVERSEDIST_H__BB8C4FFF_A66C_4669_8B04_7D3080F5066A__INCLUDED_)

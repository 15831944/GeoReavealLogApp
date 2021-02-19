/////////////////////////////////////////////////////////////////
// File Name:		Clustering.h
// File Function:	1. Maximun clustering algorithm
//					2. LBG (k-Mean) clustering algorithm
//////////////////////////////////////////////////////////////////////

#pragma once

struct   VQ_VECTOR
{
	double*	Data;		//Input vector
	int		nDimension;	//Dimension of input vector
	int		nCluster;	//Class the vector belong to during clustering
						//Value may changed every epoch
	char*	pFileName;
};

struct   VQ_CENTER
{
	double*	Data;		//Clustering center vector
	int		nDimension;	//Dimension of center vector
	int		Num;		//Number of vectors belong to the clustering
};
/////////////////////////////////////////////////////////////////////
//
// K均值聚类分析
// 用于成像数据自动求取刻度范围、曲线自动读值、孔隙度谱分析
/***********************************************************************
* kmpp - K-Means++
* Traditional data clustering with a special initialization
* by Andy Allinger, 2017, released to the public domain
* This program may be used by any person for any purpose
*
* Origin:
*     Hugo Steinhaus, 1956
*
* Refer to:
*     "kmeans++: the advantages of careful seeding"
*     David Arthur and Sergei Vassilvitskii
*     Proceedings of the eighteenth annual ACM-SIAM symposium
*       on Discrete algorithms, 2007
*
*___Variable___Type______I/O_______Description__________________________
*   x[p][n]    float     In        Data points
*   p          int       In        Dimension of the data
*   n          int       In        Number of points
*   k          int       In        # clusters
*   c[p][k]    float     Out       Center points of clusters
*   z[n]       int       Out       What cluster a point is in
*   work[n]    float     Neither
*
* Return value: 0 no errors
*               1 empty cluster
*               2 max iterations exceeded
*               3 k not in valid range
***********************************************************************/
int _declspec(dllexport)_cdecl Kmeans (float **x, int p, int n, int k, float **c, int *z, float *work);

//Calculate the correlation of two vectors
/*********************************************************************
/*	Name:		GetCorrelation
/*	Function:	Calculate correlation of two vectors
/*	Parameter:	X -- Vector one
/*				Y -- Vector two
/*				nDimension -- Dimension of the vectors
/*	Return:		Correlation of two vectors
/*	
/*********************************************************************/
double _declspec(dllexport)_cdecl GetCorrelation( double* X, double*	Y, int nDimension );

//LBG clustering algorithm
/******************************************************************************
/*	Name:		LBGCluster
/*	Function:	Clustering input vectors using LBG algorithm
/*				Using Euclidean distance
/*	Parameter:	X -- Input vecters
/*				N -- Number of input vectors
/*				Y -- Clustering result
/*				M -- Number of clustering center
/*	Return:		0 -- Correct
/*				1 -- Error
/*
/******************************************************************************/
int _declspec(dllexport)_cdecl LBGCluster(VQ_VECTOR *X, int N, VQ_CENTER *Y, int M);

//LBG clustering algorithm using correlation distance
/******************************************************************************
/*	Name:		LBGClusterCor
/*	Function:	Clustering input vectors using LBG algorithm
/*				Using correlation distance
/*	Parameter:	X -- Input vecters
/*				N -- Number of input vectors
/*				Y -- Clustering result
/*				M -- Number of clustering center
/*	Return:		0 -- Correct
/*				1 -- Error
/*
/******************************************************************************/
int _declspec(dllexport)_cdecl LBGClusterCor(VQ_VECTOR *X, int N, VQ_CENTER *Y, int M);

//Max clustering algorithm
/******************************************************************************
/*	Name:		MaxCluster
/*	Function:	Clustering input vectors using Maximum algorithm
/*	Parameter:	X -- Input vecters
/*				N -- Number of input vectors
/*				Y -- Clustering result
/*				M -- Number of clustering center
/*	Return:		0 -- Correct
/*				1 -- Error
/*
/******************************************************************************/
int _declspec(dllexport)_cdecl MaxCluster(VQ_VECTOR *X, int N, VQ_CENTER *Y, int M);


//Dump clustering result to text file for debugging
/******************************************************************************
/*	Name:		DumpClusterData
/*	Function:	Dump clustering result to a text file for debugging
/*	Parameter:	FileName -- Dump text file name
/*				X -- Input vecters
/*				N -- Number of input vectors
/*				Y -- Clustering result
/*				M -- Number of clustering center
/*	Return:		0 -- Correct
/*				1 -- Error
/*
/******************************************************************************/
int _declspec(dllexport)_cdecl DumpClusterData(CString FileName, VQ_VECTOR *X, int N, VQ_CENTER *Y, int M);


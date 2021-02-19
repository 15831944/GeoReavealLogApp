///////////////////////////////////////////////////////////////////////
// File Name:		Clustering.cpp
// File Function:	1. Maximun clustering algorithm
//					2. LBG (k-Mean) clustering algorithm
// Date:			2001.10.3
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Clustering.h"
#include "stdio.h"
#include "math.h"

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
int Kmeans (float **x, int p, int n, int k, float **c, int *z, float *work)
{
/* Local variables */
	int h, i, j, l;
	float u, d, d2;
	int i1, l0, l1;
	float frm, tot, best;
	bool change;

/* constants */
	const int ITER=1000;
	const float BIG=1e33f;

/* Begin. */
	if(k < 1 || k > n) 
		return 3;      /* k out of bounds */
	for(i = 0; i < n; ++i) 
		z[i] = 0;  /* clear Z */
	frm = (float)(RAND_MAX);

/* Initial centers */
	for (i = 0; i < n; ++i) 
		work[i] = 1e33f;
	u = (float)(rand()) / frm;
	i1 = __min((int)(u * (float)(n)) + 1, n);      /* choose first center at random */
	for(j = 0; j < p; ++j) /* copy */
		c[j][0] = x[j][i1];     
	for(l = 1; l < k; ++l)  /* initialize other centers */
	{       
		tot = 0.f;
		for (i = 0; i < n; ++i)  /* measure from each point */
		{           
			best = work[i];
			d2 = 0.f;			/* to prior center */
			for (j = 0; j < p; ++j)  /* Squared Euclidean distance */
			{      
				d = x[j][i] - c[j][l-1];
				d2 += d * d;
				if (d2 >= best) break;     /* needless to add to d2 */
			} /* next dimension */
			if (d2 < best) best = d2;       /* shortest squared distance? */
			work[i] = best;
			tot += best;                         /* cumulative squared distance */
		} /* next data point */

/* Choose center with probability proportional to its squared distance
     from existing centers. */
		u = (float)(rand()) / frm;
		u *= tot;                     /* uniform at random over cumulative distance */
		tot = 0.f;
		for(i = 0; i < n; ++i) 
		{
			i1 = i;
			tot += work[i];
			if (tot > u) break;
		} /* next i */
		for(j = 0; j < p; ++j) 
			c[j][l] = x[j][i1];        /* assign center */
	} /* next center to initialize */

/* Main loop */
	for (h = 0; h < ITER; ++h) 
	{
		change = false;

/* Find nearest center for each point */
		for (i = 0; i < n; ++i) 
		{
			l0 = z[i] - 1;
			l1 = 0;
			best = BIG;
			for (l = 0; l < k; ++l) 
			{
				d2 = 0.f;
				for (j = 0; j < p; ++j) 
				{
					d = x[j][i] - c[j][l];
					d2 += d * d;
					if (d2 >= best) break;
				} /* next j */

				if (d2 < best) 
				{
					best = d2;
					l1 = l;
				}
			} /* next L */

			if (l0 != l1) 
			{
				z[i] = l1 + 1;         /* reassign point */
				change = true;
			}
		} /* next i */
		if (! change) return 0;          /* success */

/* Find cluster centers */
		for (l = 0; l < k; ++l) 
			work[l] = 0.f;   /* zero population */
		for (j = 0; j < p; ++j) 
		{                /* zero centers */
			for (l = 0; l < k; ++l) 
				c[j][l] = 0.f;
		}

		for (i = 0; i < n; ++i) 
		{
			l = z[i] - 1;
			work[l] += 1.f;             /* count */
			for (j = 0; j < p; ++j) 
				c[j][l] += x[j][i];    /* add */
		}

		for (l = 0; l < k; ++l) 
		{
			if (work[l] < .5f) return 1;  /* empty cluster check */
			u = 1.f / work[l];
			for (j = 0; j < p; ++j) /* multiplication is faster than division*/
				c[j][l] *= u; 
		}
	} /* next h */
	return 2;  /* too many iterations */
}

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
int LBGCluster(VQ_VECTOR *X, int N, VQ_CENTER *Y, int M)
{
	if(N<M)			 return -1;
	int		L=1000, m=1, nCenter, i, j, k;//L,迭代的次数
	int		nDimension = X[0].nDimension;
	double  D0, D;
	struct  VQ_CENTERINFO
	{
		double*	  Data;
		int		  nDimension;
		double*   SumData;
		int		  Num;
	};
	VQ_CENTERINFO	*Center = (VQ_CENTERINFO*)malloc(M*sizeof(VQ_CENTERINFO));
	if(Center == NULL)		return -1;
	double			*Distance = (double*)malloc(N*sizeof(double));
	if(Distance == NULL)	return -1;

	for( i=0; i<M; i++)
	{
		Center[i].nDimension = nDimension;
		Center[i].Data = (double*)malloc(sizeof(double)*nDimension);
		Center[i].SumData = (double*)malloc(sizeof(double)*nDimension);
		if( Center[i].Data == NULL || Center[i].SumData == NULL )
		{
			AfxMessageBox( "Memory used up!" );
			return -1;
		}
		for( j=0; j<nDimension; j++ )
		{
			Center[i].Data[j] = X[i*N/M].Data[j];
			Center[i].SumData[j] = 0;
		}
		Center[i].Num = 0;
	}

	D0=1;         D=1e+10;	
	while(m<L && fabs(D0-D)/D0>1e-5)
	{
		for(i=0; i<M; i++)
		{
			for( j=0;	j<nDimension; j++ )
				Center[i].SumData[j] = 0;
			Center[i].Num = 0;
		}
		D0 = D;			D = 0;		m++;
		for(i=0; i<N; i++)
		{
			Distance[i] = 1e+10;
			for(int j=0; j<M; j++)
			{
				double  Dist = 0;
				for( k=0; k<nDimension; k++ )
					Dist += (X[i].Data[k]-Center[j].Data[k])*(X[i].Data[k]-Center[j].Data[k]);
				if(  Dist < Distance[i])
				{			
					nCenter = j;
					Distance[i] = Dist;
				}
			}
			X[i].nCluster = nCenter;
			for( k=0; k<nDimension; k++ )
				Center[nCenter].SumData[k] += X[i].Data[k];
			Center[nCenter].Num++;
			D += Distance[i];
		}
		for(i=0; i<M; i++)
		{
			if(Center[i].Num != 0)
				for( k=0; k<nDimension; k++ )
					Center[i].Data[k] = Center[i].SumData[k]/Center[i].Num;
			else
			{	
				int MaxNum=0;
				for( k=1; k<M; k++)
					MaxNum = Center[i].Num > Center[MaxNum].Num ? i: MaxNum;
				int   Num = Center[MaxNum].Num/2;
				for( k=0; k<nDimension; k++ )
					Center[MaxNum].SumData[k] = 0;
				Center[MaxNum].Num = 0;
				for(k=0; k<N; k++)
				{	
					if(X[k].nCluster != MaxNum)		continue;
					if(Center[i].Num < Num)
					{   
						X[k].nCluster = i;
						for( m=0; m<nDimension; m++)
							Center[i].SumData[m] += X[k].Data[m];
						Center[i].Num++;
					}
					else
					{
						for( m=0; m<nDimension; m++ )
							Center[MaxNum].SumData[m] += X[k].Data[m];
						Center[MaxNum].Num++;
					}
				}
				for( m=0; m<nDimension; m++ )
					Center[i].Data[m] = Center[i].SumData[m] / Center[i].Num;
				if(MaxNum < i)
					for( m=0; m<nDimension; m++ )
						Center[MaxNum].Data[m] = Center[MaxNum].SumData[m] / Center[MaxNum].Num;
			}
		}
	}
	for(i=0; i<M; i++)
	{
		for( m=0; m<nDimension; m++ )
			Y[i].Data[m] = Center[i].Data[m];
		Y[i].Num = Center[i].Num;
	}
	for( i=0; i<M; i++ )
	{
		free( Center[i].Data );
		free( Center[i].SumData );
	}
	free(Center);
	free(Distance);
	return 0;
}

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
int LBGClusterCor(VQ_VECTOR *X, int N, VQ_CENTER *Y, int M)
{
	if(N<M)			 return -1;
	int		L=1000, m=1, nCenter, i, j, k;
	int		nDimension = X[0].nDimension;
	double  SumCor0, SumCor;
	struct  VQ_CENTERINFO
	{
		double*	  Data;
		int		  nDimension;
		double*   SumData;
		int		  Num;
	};
	VQ_CENTERINFO	*Center = (VQ_CENTERINFO*)malloc(M*sizeof(VQ_CENTERINFO));
	if(Center == NULL)		return -1;
	double			*Correlation = (double*)malloc(N*sizeof(double));
	if(Correlation == NULL)	return -1;

	for( i=0; i<M; i++)
	{
		Center[i].nDimension = nDimension;
		Center[i].Data = (double*)malloc(sizeof(double)*nDimension);
		Center[i].SumData = (double*)malloc(sizeof(double)*nDimension);
		if( Center[i].Data == NULL || Center[i].SumData == NULL )
		{
			AfxMessageBox( "Memory used up!" );
			return -1;
		}	
		for( j=0; j<nDimension; j++ )
		{
			Center[i].Data[j] = X[i*N/M].Data[j];
			Center[i].SumData[j] = 0;
		}
		Center[i].Num = 0;
	}

	SumCor0=0.001;         SumCor=0.1;	
	while(m<L && fabs(SumCor0-SumCor)/SumCor0>1e-20)
	{
		for(i=0; i<M; i++)
		{
			for( j=0;	j<nDimension; j++ )
				Center[i].SumData[j] = 0;
			Center[i].Num = 0;
		}
		SumCor0 = SumCor;			SumCor = 0;		m++;
		for(i=0; i<N; i++)
		{
			Correlation[i] = 0;
			for(int j=0; j<M; j++)
			{
				double  Cor = GetCorrelation( X[i].Data, Center[j].Data, nDimension);
				if(  Cor > Correlation[i])
				{			
					nCenter = j;
					Correlation[i] = Cor;
				}
			}
			X[i].nCluster = nCenter;
			for( k=0; k<nDimension; k++ )
				Center[nCenter].SumData[k] += X[i].Data[k];
			Center[nCenter].Num++;
			SumCor += Correlation[i];
		}
		for(i=0; i<M; i++)
		{
			if(Center[i].Num != 0)
				for( k=0; k<nDimension; k++ )
					Center[i].Data[k] = Center[i].SumData[k]/Center[i].Num;
			else
			{	
				int MaxNum=0;
				for( k=1; k<M; k++)
					MaxNum = Center[i].Num > Center[MaxNum].Num ? i: MaxNum;
				int   Num = Center[MaxNum].Num/2;
				for( k=0; k<nDimension; k++ )
					Center[MaxNum].SumData[k] = 0;
				Center[MaxNum].Num = 0;
				for(k=0; k<N; k++)
				{	
					if(X[k].nCluster != MaxNum)		continue;
					if(Center[i].Num < Num)
					{   
						X[k].nCluster = i;
						for( m=0; m<nDimension; m++)
							Center[i].SumData[m] += X[k].Data[m];
						Center[i].Num++;
					}
					else
					{
						for( m=0; m<nDimension; m++ )
							Center[MaxNum].SumData[m] += X[k].Data[m];
						Center[MaxNum].Num++;
					}
				}
				for( m=0; m<nDimension; m++ )
					Center[i].Data[m] = Center[i].SumData[m] / Center[i].Num;
				if(MaxNum < i)
					for( m=0; m<nDimension; m++ )
						Center[MaxNum].Data[m] = Center[MaxNum].SumData[m] / Center[MaxNum].Num;
			}
		}
	}
	for(i=0; i<M; i++)
	{
		for( m=0; m<nDimension; m++ )
			Y[i].Data[m] = Center[i].Data[m];
		Y[i].Num = Center[i].Num;
	}
	for( i=0; i<M; i++ )
	{
		free( Center[i].Data );
		free( Center[i].SumData );
	}
	free(Center);
	free(Correlation);
	return 0;
}

/*********************************************************************
/*	Name:		GetCorrelation
/*	Function:	Calculate correlation of two vectors
/*	Parameter:	X -- Vector one
/*				Y -- Vector two
/*				nDimension -- Dimension of the vectors
/*	Return:		Correlation of two vectors
/*	
/*********************************************************************/
double	GetCorrelation( double* X, double*	Y, int nDimension )
{
	double	Correlation=0, DX=0, DY=0;
	for(int i=0; i<nDimension; i++ )
	{
		Correlation += X[i]*Y[i];
		DX += X[i]*X[i];
		DY += Y[i]*Y[i];
	}
	return  Correlation/sqrt( DX*DY );
}

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
int MaxCluster(VQ_VECTOR *X, int N, VQ_CENTER *Y, int M)
{
	if(N<M)			 return -1;
	int		m=1;
	struct  VQ_CENTERINFO
	{
		double*	  Data;
		int		  nDimension;
		double*   SumData;
		int		  Num;
	};
	VQ_CENTERINFO	*Center = (VQ_CENTERINFO*)malloc(M*sizeof(VQ_CENTERINFO));
	if(Center == NULL)		return -1;
	double			*Distance = (double*)malloc(N*sizeof(double));
	if(Distance == NULL)	return -1;

	int		nDimension = X[0].nDimension;
	int		i, j, k;
	for(i=0; i<M; i++)
	{
		for( k=0; k<nDimension; k++ )
			Center[i].SumData[k] = 0;
		Center[i].Num = 0;
	}
	double*	SumData = (double*)malloc(sizeof(double)*nDimension);
	for( k=0; k<nDimension; k++ )
		SumData[k]= 0;
	for(i=0; i<N; i++)
		for( k=0; k<nDimension; k++ )
			SumData[k] += X[i].Data[k];	
	for( k=0; k<nDimension; k++ )
		SumData[k]	/= N;	
	
	int  l=0;
	double L=0, Dist;
	for(i=0; i<N; i++)
	{
		Dist = 0;
		for( k=0; k<nDimension; k++ )
			Dist += (X[i].Data[k]-SumData[k])*(X[i].Data[k]-SumData[k]);
		if(Dist > L)
		{
			L = Dist;
			l = i;
		}
	}
	for( k=0; k<nDimension; k++ )
		Center[0].Data[k] = X[l].Data[k];
	for(m=1; m<M; m++)
	{
		double  MaxMinDist = -1;
		int		MaxMin = -1;
		for( j=0; j<N; j++)
		{
			double MinDist = 1e+10;
			for(int k=0; k<m; k++)
			{
				Dist = 0;
				for( k=0; k<nDimension; k++ )
					Dist += (X[j].Data[k] - Center[k].Data[k])*(X[j].Data[k] - Center[k].Data[k]);
				MinDist = min(Dist , MinDist);
			}
			if(MinDist > MaxMinDist)
			{
				MaxMinDist = MinDist;
				MaxMin  = j;
			}
		}
		for( k=0; k<nDimension; k++ )
			Center[m].Data[k] = X[MaxMin].Data[k];
	}
	for(i=0; i<N; i++)
	{	int		Min = -1;
		double  MinDist = 1e+10;
		for(m=0; m<M; m++)
		{
			Dist = 0;
			for( k=0; k<nDimension; k++ )
				Dist += (X[i].Data[k] - Center[m].Data[k])*(X[i].Data[k] - Center[m].Data[k]);
			if(Dist < MinDist)
			{
				MinDist = Dist;
				Min = m;
			}
		}
		X[i].nCluster = Min;
		if(Min<0 || Min>=M)
			return -1;
		for( k=0; k<nDimension; k++ )
			Center[Min].SumData[k] += X[i].Data[k];
		Center[Min].Num++;
	}
	for(m=0; m<M; m++)
		for( k=0; k<nDimension; k++ )
			Center[m].Data[k] = Center[m].SumData[k] / Center[m].Num;
	for(i=0; i<M; i++)
	{
		for( k=0; k<nDimension; k++ )
			Y[i].Data[k] = Center[i].Data[k];
		Y[i].Num = Center[i].Num;
	}
	free( SumData );
	for( i=0; i<M; i++ )
	{
		free( Center[i].Data );
		free( Center[i].SumData );
	}
	free(Center);
	free(Distance);
	return 0;
}


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
int DumpClusterData(CString FileName, VQ_VECTOR *X, int N, VQ_CENTER *Y, int M)
{
	int		i, j, k;
	int		nDimension = X[0].nDimension;
	FILE *fp = fopen(FileName, "wt");
	for( i=0; i<M; i++)
	{
		fprintf(fp, "Center%02d: ", i);
		for(  k=0; k<nDimension; k++ )
			fprintf( fp, "%5.1f ", Y[i].Data[k] );
		fprintf( fp, "Num=%03d\n", Y[i].Num);
		for( j=0; j<N; j++)
		{	if(X[j].nCluster != i)		continue;
			double Distance = 0;
			for( k=0; k<nDimension; k++)
				Distance += (X[j].Data[k]- Y[i].Data[k])*(X[j].Data[k]-Y[i].Data[k]);
			Distance = sqrt( Distance );
			for( k=0; k<nDimension; k++ )
				fprintf(fp, "  %03d  ", (int)X[j].Data[k] );
			fprintf( fp, " D=%5.1f\n", Distance);
		}
	}
	fclose(fp);
	return 0;
}


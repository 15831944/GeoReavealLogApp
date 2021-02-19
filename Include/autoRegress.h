#pragma once

#define MAXENTROPY      0
#define LEASTSQUARES    1

int  _declspec(dllexport)_cdecl AutoRegression(double *,int,int,double *,int);

int  _declspec(dllexport)_cdecl ARMaxEntropy(double *,int,int,double **,
												double *,double *,double *,double *
											);

int  _declspec(dllexport)_cdecl ARLeastSquare(double *,int,int,double *);

int  _declspec(dllexport)_cdecl SolveLE(double **,double *,unsigned int);

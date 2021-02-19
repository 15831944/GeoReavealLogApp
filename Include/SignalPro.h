#pragma once
/////////////////////////////////////////////////////////////////////////////////////
//
// 多尺度图像对比增强处理
/*
整体的流程还是比较简单的，问题是图像处理中的一些细节，例如滤波时边界的处理，图像抽样时w或者h为奇数时w/2（h/2）的取值，
也就是离散整数的取整误差问题。还有些编程时内存分配的问题，临时缓存的分配等。但是不考虑得太复杂都使用最常用的方式，
以上仅将大概的问题列出，碰到时候需要注意。
    入口函数伪代码：
void GDIPlusImage::IPFuncMUSICA()
{
    O = 原始图像;
    for（分解级数）
      {计算每级的长宽保存到矩阵S;}
    ImageGrey24To8(O);//灰度化，仅对灰度图象进行处理
    X = MUSICA_Decomposition(O,S);//分解，得到X为所有的图像按S里的顺序排列，30
    X = MUSICA_Mapping(X,S);//对分解图像进行变换，32
    O = MUSICA_Reconstruction(X,S);//重构，得到增强后的图像，34
    ImageGrey8To24(O);//转化成24为图，显示、后继处理起来方便，仅此而已
}
    MUSICA_Mapping(X,S)函数相对简单，只需要对S中的残差距阵进行灰度级的查表替换，替换的表根据s型函数事先构造好，
	其过程类似进行反色处理，可以参考前面的文章。可以考虑将替换表的构造独立出来，通过参数传递，方便改进。
*/
//分解
BOOL _declspec(dllexport)_cdecl MUSICA_Decomposition(BYTE* pPixelsGrey,BYTE* pDecomposition,int * matrixInfo);
//变换
void _declspec(dllexport)_cdecl MUSICA_Mapping(BYTE* pDecomposition,int * matrixInfo);
//重构
BOOL _declspec(dllexport)_cdecl MUSICA_Reconstruction(BYTE* pPixelsGrey,BYTE* pDecomposition,int * matrixInfo);
BOOL MUSICA_ImageSubsample(BYTE* X,int X_w,int X_h,int X_stride,BYTE* X_1,int X_1_w,int X_1_h,int X_1_stride);
BOOL MUSICA_ImageInterpolator(BYTE* X_1,int X_1_w,int X_1_h,int X_1_stride,BYTE* X,int X_w,int X_h,int X_stride);
void ImageFilter8(double *fliter,int windowWidth,BYTE* pixels,int w,int h,int stride);
void MUSICA_ImageSub8(BYTE* pMinuend,BYTE* pSubtrahend,BYTE* pResult,int w,int h,int stride);
void MUSICA_ImageAdd8(BYTE* pOriginal,BYTE* pDetail,BYTE* pResult,int w,int h,int stride);

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// 测井数据卡尔曼滤波分片处理
//                 In                                                                               Out
void  _declspec(dllexport)_cdecl SegmentSignal(double LOG[], int NSAMPS, double F, int ORDER, int ORDER1, int RMODE, int NITER, double Q[], int &NQ, double FLTLOG[], double SEGLOG[], double R[], double &C, double &D, int &NACT, int &IER);

//                                   In                                                                  Out
void SingleMostLikelihoodReplacement(double G[], double S[], double C, int NSAMPS, double D, double Q[], bool &CONV, int &IER);

//             In                                               Out
void Threshold(double JMPSEQ[], double C, double F, int NSAMPS, double Q[], double &D);

//                 In                                    Out
void MovingAverage(double ARR1[], int NSAMPS, int ORDER, double ARR2[]);

//                In                                                          Out
void KalmanFilter(double LOG[], double Q[], double C, double R[], int NSAMPS, double GAIN[], double ZTLT[], double ETA[], double STATE[]);

//                                In                                In/Out
void FixedIntervalOptimalSmoother(double Q[], int NSAMPS, double C, double ARR1[], double ARR2[], double ARR3[]);

//                        In                                    Out
void AverageArraySegments(double ARR[], double Q[], int NSAMPS, double AVGOUT[]);

//                                    In                                               Intermediate    Out
void DeglitchAndEstimateNoiseVariance(double LOG[], double Q[], int NSAMPS, int RMODE, double WORK1[], double R[]);

//              In												  Out
void GetSegment(double LOG[], double Q[], int NSAMPS, int SEGSTR, int &SEGEND, int &SEGLEN, double &SEGAVG, bool &endofarray);

//            In          In/Out
void Deglitch(int NSAMPS, double Q[]);

//                 In                                  Out
void GetSpikeyZone(double Q[], int NSAMPS, int SEGSTR, int &SEGEND, bool &endofarray);

//                        In                      Out
void EstimateEventDensity(double Q[], int NSAMPS, double &D, int &NQ);

//                                  In                        Intermediate    Out
void EstimateVarianceOfJumpSequence(double LOG[], int NSAMPS, double WORK1[], double &C);

//                            In                                    Out
void CalculateMeanAndVariance(double ARRAY[], int NSAMPS, int MODE, double &MEAN, double &VAR);

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

#pragma once
/////////////////////////////////////////////////////////////////////////////////////
//
// ��߶�ͼ��Ա���ǿ����
/*
��������̻��ǱȽϼ򵥵ģ�������ͼ�����е�һЩϸ�ڣ������˲�ʱ�߽�Ĵ���ͼ�����ʱw����hΪ����ʱw/2��h/2����ȡֵ��
Ҳ������ɢ������ȡ��������⡣����Щ���ʱ�ڴ��������⣬��ʱ����ķ���ȡ����ǲ����ǵ�̫���Ӷ�ʹ����õķ�ʽ��
���Ͻ�����ŵ������г�������ʱ����Ҫע�⡣
    ��ں���α���룺
void GDIPlusImage::IPFuncMUSICA()
{
    O = ԭʼͼ��;
    for���ֽ⼶����
      {����ÿ���ĳ����浽����S;}
    ImageGrey24To8(O);//�ҶȻ������ԻҶ�ͼ����д���
    X = MUSICA_Decomposition(O,S);//�ֽ⣬�õ�XΪ���е�ͼ��S���˳�����У�30
    X = MUSICA_Mapping(X,S);//�Էֽ�ͼ����б任��32
    O = MUSICA_Reconstruction(X,S);//�ع����õ���ǿ���ͼ��34
    ImageGrey8To24(O);//ת����24Ϊͼ����ʾ����̴����������㣬���˶���
}
    MUSICA_Mapping(X,S)������Լ򵥣�ֻ��Ҫ��S�еĲв������лҶȼ��Ĳ���滻���滻�ı����s�ͺ������ȹ���ã�
	��������ƽ��з�ɫ�������Բο�ǰ������¡����Կ��ǽ��滻��Ĺ������������ͨ���������ݣ�����Ľ���
*/
//�ֽ�
BOOL _declspec(dllexport)_cdecl MUSICA_Decomposition(BYTE* pPixelsGrey,BYTE* pDecomposition,int * matrixInfo);
//�任
void _declspec(dllexport)_cdecl MUSICA_Mapping(BYTE* pDecomposition,int * matrixInfo);
//�ع�
BOOL _declspec(dllexport)_cdecl MUSICA_Reconstruction(BYTE* pPixelsGrey,BYTE* pDecomposition,int * matrixInfo);
BOOL MUSICA_ImageSubsample(BYTE* X,int X_w,int X_h,int X_stride,BYTE* X_1,int X_1_w,int X_1_h,int X_1_stride);
BOOL MUSICA_ImageInterpolator(BYTE* X_1,int X_1_w,int X_1_h,int X_1_stride,BYTE* X,int X_w,int X_h,int X_stride);
void ImageFilter8(double *fliter,int windowWidth,BYTE* pixels,int w,int h,int stride);
void MUSICA_ImageSub8(BYTE* pMinuend,BYTE* pSubtrahend,BYTE* pResult,int w,int h,int stride);
void MUSICA_ImageAdd8(BYTE* pOriginal,BYTE* pDetail,BYTE* pResult,int w,int h,int stride);

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// �⾮���ݿ������˲���Ƭ����
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
// K��ֵ�������
// ���ڳ��������Զ���ȡ�̶ȷ�Χ�������Զ���ֵ����϶���׷���
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

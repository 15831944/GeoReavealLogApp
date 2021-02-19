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

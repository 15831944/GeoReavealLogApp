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

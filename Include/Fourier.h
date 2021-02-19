#pragma once
//快速傅立叶变换FFT被评选为二十世纪科学和工程界最具影响力的十大算法之一
class AFX_EXT_CLASS CFourier
{
public:
	double pi;
	unsigned long int fundamental_frequency;
	float *vector;
	CFourier(void);
	~CFourier(void);
	// FFT 1D
	void ComplexFFT(float data[], unsigned long number_of_samples, unsigned int sample_rate, int sign);
};

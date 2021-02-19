#pragma once
//���ٸ���Ҷ�任FFT����ѡΪ��ʮ���Ϳ�ѧ�͹��̽����Ӱ������ʮ���㷨֮һ
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

# include "upsampler_20190225.h"

# include <complex>
# include "..\algorithms\matrixInversion\matrixInversion_20190320.h"

using namespace std;

double sinc(double X);


void Upsampler::initialize(void) {

	filter.resize(2 * upSamplingFactor*numberOfTaps - 1);
	delayLine.resize(filter.size());

	aux = ceil(filter.size() / 2);

	vector<double> F(2, 0.0);
	vector<double> M(2, upSamplingFactor);


	double freqmult = 0.5;
	F[1] = freqmult / (2.0 * upSamplingFactor);

	double f = 1.0 / upSamplingFactor;
	while (f <= 0.5)
	{
		F.push_back(f - freqmult / (2 * upSamplingFactor));
		F.push_back(f + freqmult / (2 * upSamplingFactor));

		M.push_back(0);
		M.push_back(0);

		f += 1.0 / upSamplingFactor;
	}
	if (F.back() > 0.5) F.back() = 0.5;

	int N = 2 * upSamplingFactor*numberOfTaps - 1;

	vector<double> W(F.size() / 2, 1);

	int L = (N - 1) / 2;

	vector<int> k;
	for (int i = 0; i <= L; i++) k.push_back(i);
	const size_t aux = k.size();

	vector<vector<double>> I1(k.size());
	vector<vector<double>> I2(k.size());
	vector<vector<double>> G(k.size());
	vector<vector<double>> Ginverted(k.size());

	for (int i = 0; i < k.size(); i++) {
		I1[i] = vector<double>(k.size());
		I2[i] = vector<double>(k.size());
		G[i] = vector<double>(k.size());
		Ginverted[i] = vector<double>(k.size());
	}



	for (int i = 0; i < aux; i++)
	{
		for (int j = 0; j < aux; j++)
		{
			I1[i][j] = i + j;
			I2[i][j] = i - j;
		}

	}

	k.erase(k.begin(), k.begin() + 1);

	double b0 = 0;
	vector<double> b(k.size(), 0);

	for (size_t i = 1; i <= F.size(); i+=2)
	{
		double m = (M[i] - M[i - 1]) / (F[i] - F[i - 1]);
		double b1 = M[i - 1] - m * F[i - 1];

		b0 = b0 + (b1*(F[i] - F[i-1]) + m / 2 * (F[i]*F[i] - F[i-1]*F[i-1])) * abs(W[i / 2] * W[i / 2]);
		for (size_t j = 0; j < b.size(); j++) b[j] = b[j] + (m / (4.0 * PI*PI)*(cos(2.0 * PI*k[j] * F[i]) - cos(2.0 * PI*k[j] * F[i - 1])) / (k[j] * k[j]))* abs(W[(i) / 2] * W[(i) / 2]);

		for (size_t j = 0; j < b.size(); j++) b[j] = b[j] + (F[i]*(m*F[i] + b1)*sinc(2.0 * k[j] *F[i]) - F[i-1]*(m*F[i-1] + b1)*sinc(2 * k[j] *F[i-1]))* abs(W[i / 2] * W[i / 2]);

		for (size_t j = 0; j < G.size(); j++)
			for (size_t n = 0; n < G.size(); n++) 
				G[j][n] = G[j][n] + (.5*F[i] * (sinc(2.0 * I1[j][n] *F[i]) + sinc(2.0 * I2[j][n] *F[i])) - .5*F[i - 1] * (sinc(2.0 * I1[j][n] *F[i - 1]) + sinc(2.0 * I2[j][n] *F[i - 1])))* abs(W[i / 2] * W[i / 2]);


	}

	b.insert(b.begin(), b0);

	Ginverted = matrixInversion(G);

	vector<double> a(Ginverted.size(), 0);
	for (size_t i = 0; i < a.size(); i++)
		for (size_t j = 0; j < Ginverted.size(); j++)
			a[i] += Ginverted[i][j] * b[j];

	for (size_t i = 0; i < a.size(); i++)
		filter[i] = a[a.size()-1 - i] / 2;
	filter[a.size()-1] = a[0];
	for (size_t i = 1; i < a.size(); i++)
		filter[a.size()+i-1] = a[i] / 2;

	double samplingPeriod = inputSignals[0]->getSamplingPeriod();

	outputSignals[0]->setSymbolPeriod(inputSignals[0]->getSymbolPeriod());
	outputSignals[0]->setSamplingPeriod(inputSignals[0]->getSamplingPeriod());
}


bool Upsampler::runBlock(void) {

	t_complex in;
	t_complex out;

	int space = outputSignals[0]->space();
	int ready = inputSignals[0]->ready();

	int process = min(ready, space);

	if (process == 0) return false;
	
	if (firstRun)
	{
		firstRun = false;
		for (int i = 0; i < filter.size(); i++)
		{
			inputSignals[0]->bufferGet(&in);
			rotate(delayLine.begin(), delayLine.begin()+delayLine.size() - 1, delayLine.end());
			delayLine[0] = in;
			out = 0;
			for (int j = 0; j <= i; j++)
			{
				out += filter[j] * delayLine[j];
			}
			outputSignals[0]->bufferPut(out);
		}
	}
	
	space = outputSignals[0]->space();
	ready = inputSignals[0]->ready();

	process = min(ready, space);

	if (process == 0) return false;

	for (int i = 0; i < process; i++)
	{
		inputSignals[0]->bufferGet(&in);
		rotate(delayLine.begin(), delayLine.begin() + 1, delayLine.end());
		delayLine[0] = in;
		out = 0;
		for (int j = 0; j < filter.size(); j++)
		{
			out += filter[j] * delayLine[j];
		}
		outputSignals[0]->bufferPut(out);
	}

	return true;
}


double sinc(double X) {
	if (X==0) return 1.0;
	else return sin(PI*X) / (PI*X);

}



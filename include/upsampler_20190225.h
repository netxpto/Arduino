# ifndef PROGRAM_INCLUDE_UPSAMPLER_H_
# define PROGRAM_INCLUDE_UPSAMPLER_H_


# include "netxpto_20180815.h"

# include <complex>
# include <random>
# include <vector>


class Upsampler : public Block {

	/* State Variables */


	/* Input Parameters */
	t_complex sum;

	int aux;

	int upSamplingFactor = 2;
	int numberOfTaps = 8;
	vector<t_complex> delayLine;
	vector<t_complex> filter;

	bool firstRun{ true };


public:

	/* Methods */
	Upsampler(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	void setUpsamplingFactor(int uFactor) { upSamplingFactor = uFactor; }
	int const getUpsamplingFactor(void) { return upSamplingFactor; };

	void setNumberOfTaps(int nTaps) { numberOfTaps = nTaps; }
	int const getNumberOfTaps(void) { return numberOfTaps; };

};

#endif 

# ifndef PROGRAM_INCLUDE_RESAMPLER_H_
# define PROGRAM_INCLUDE_RESAMPLER_H_


# include "../include/netxpto_20180815.h"
# include "../include/interspace0s_20190319.h"
# include "../include/upsampler_20190225.h"

# include <complex>
# include <random>
# include <vector>


class Resampler : public SuperBlock {

public:

	/* Methods */
	Resampler(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig);

	void initialize(void);
	bool runBlock(void);

	int upSamplingFactor = 4;

	bool firstTime{ true };

	void setUpsamplingFactor(int uFactor) { upSamplingFactor = uFactor, Interspace0s_.setUpsamplingFactor(uFactor); Upsampler_.setUpsamplingFactor(uFactor); }
	int const getUpsamplingFactor(void) { return Interspace0s_.getUpsamplingFactor(); };

	void setNumberOfTaps(int nTaps) { Upsampler_.setNumberOfTaps(nTaps); }
	int const getNumberOfTaps(void) { return Upsampler_.getNumberOfTaps(); };

private:

	string signalsFolderName{ "signals/SuperBlock_Resampler" };
	bool logValue{ true };
	string logFileName{ "SuperBlock_Resampler.txt" };

	// #####################################################################################################
	// ########################### Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	TimeContinuousAmplitudeContinuousComplex SignalIn{ "S0_In.sgn", inputSignals[0]->getBufferLength() };

	TimeContinuousAmplitudeContinuousComplex Interspaced{ "S1_Interspaced.sgn", SignalIn.getBufferLength() * upSamplingFactor };

	TimeContinuousAmplitudeContinuousComplex Upsampled{ "S2_Upsampled.sgn", SignalIn.getBufferLength() * upSamplingFactor };


	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	Interspace0s Interspace0s_{ { &SignalIn},{ &Interspaced } };

	Upsampler Upsampler_{ { &Interspaced},{ &Upsampled } };
};

#endif 

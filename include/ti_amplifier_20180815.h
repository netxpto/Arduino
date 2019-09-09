# ifndef TI_AMPLIFIER_H_
# define TI_AMPLIFIER_H_

# include "../include/netxpto_20180815.h"

# include "../include/white_noise_20180815.h"
# include "../include/add_20180815.h"
# include "../include/ideal_amplifier_20180815.h"
# include "../include/electrical_filter_20180815.h" 


class TiAmplifier : public SuperBlock {

public:

	//##############################################################################################################

	TiAmplifier(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals) : SuperBlock(inputSignals, outputSignals)
	{
		setSignalsFolderName("signals/TiAmplifier");
		setLogFileName("TiAmplifier.txt");
	};
	TiAmplifier(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals, string sFolderName) : SuperBlock(inputSignals, outputSignals)
	{
		setSignalsFolderName(sFolderName);
		setLogFileName("TiAmplifier.txt");
	};

	void initialize(void);
	bool runBlock(void);

	bool firstTime{ true };

	//##############################################################################################################

	void setInputReferredNoisePowerSpectralDensity(double newInputReferredNPDS) { WhiteNoise_.setNoiseSpectralDensity(newInputReferredNPDS); }
	double const getInputReferredNoisePowerSpectralDensity(void) { WhiteNoise_.getNoiseSpectralDensity(); }

	void setNoiseSamplingPeriod(t_real samplingPeriod) { WhiteNoise_.setSamplingPeriod(samplingPeriod); }
	t_real getNoiseSamplingPeriod(void) { return WhiteNoise_.getSamplingPeriod(); };

	void setNoiseSymbolPeriod(t_real symPeriod) { WhiteNoise_.setSymbolPeriod(symPeriod); }
	t_real getNoiseSymbolPeriod(void) { return WhiteNoise_.getSymbolPeriod(); };

	void setGain(double ga) { IdealAmplifier_.setGain(ga); };
	double const getGain(void) { IdealAmplifier_.getGain(); }

	void setFilterType(Filter filterType) { ElectricalFilter_.setFilterType(filterType); };
	double const getFilterType(void) { return ElectricalFilter_.getFilterType(); };

	void setCutoffFrequency(double cutoffFreq) { ElectricalFilter_.setCutoffFrequency(cutoffFreq); };
	double const getCutoffFrequency(void) { return ElectricalFilter_.getCutoffFrequency(); };

	void setImpulseResponseTimeLength(int impResponseTimeLength) { ElectricalFilter_.setImpulseResponseTimeLength(impResponseTimeLength); };
	int const getImpulseResponseTimeLength(void) { return ElectricalFilter_.getImpulseResponseTimeLength(); };

	void setImpulseResponse(vector<t_real> ir) { ElectricalFilter_.setImpulseResponse(ir); };
	vector<t_real> getImpulseResponse(void) { ElectricalFilter_.getImpulseResponse(); };

	void setImpulseResponseFilename(string fName) { ElectricalFilter_.setImpulseResponseFilename(fName); }
	string getImpulseResponseFilename(void) { ElectricalFilter_.getImpulseResponseFilename(); }

	void setImpulseResponseLength(int impResponseLength) { ElectricalFilter_.setImpulseResponseLength(impResponseLength); };
	int const getImpulseResponseLength(void) { return ElectricalFilter_.getImpulseResponseLength(); };

	void setSeeBeginningOfImpulseResponse(bool sBegginingOfImpulseResponse) { ElectricalFilter_.setSeeBeginningOfImpulseResponse(sBegginingOfImpulseResponse); };
	double const getSeeBeginningOfImpulseResponse(void) { return ElectricalFilter_.getSeeBeginningOfImpulseResponse(); };

	//###############################################################################################################

private:

	// #####################################################################################################
	// ################################## Input Parameters #################################################
	// #####################################################################################################

	/*string signalsFolderName{ "signals/SuperBlock_TiAmplifier" };
	bool logValue{ true };
	string logFileName{ "SuperBlock_TiAmplifier.txt" };
	*/

	// #####################################################################################################
	// ################## Internal Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	TimeContinuousAmplitudeContinuousReal ElectricalSignal_In{ "S0_ElectricalSignal_In.sgn" };
	TimeContinuousAmplitudeContinuousReal WhiteNoise_Out{ "S1_WhiteNoiseOut.sgn" }; // Equivalent noise at the amplifier input
	TimeContinuousAmplitudeContinuousReal Add_Out{ "S2_AddOut.sgn" }; // Signal + noise at the input
	TimeContinuousAmplitudeContinuousReal AmplifiedSignalAndNoise{ "S3_AmplifiedSignalAndNoise.sgn" }; // Signal + noise amplified
	TimeContinuousAmplitudeContinuousReal AmplifiedAndFilteredSignalAndNoise{ "S4_AmplifiedAndFilteredSignalAndNoise.sgn" }; // Filtered Noisy Signal

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	WhiteNoise WhiteNoise_{ {},{&WhiteNoise_Out} };
	Add Add_{ {&ElectricalSignal_In, &WhiteNoise_Out},{ &Add_Out } };
	IdealAmplifier IdealAmplifier_{ { &Add_Out },{ &AmplifiedSignalAndNoise } };
	ElectricalFilter ElectricalFilter_{ { &AmplifiedSignalAndNoise }, { &AmplifiedAndFilteredSignalAndNoise } };

};


#endif

# ifndef COHERENT_RECEIVER_OPTICAL_FRONT_END_H_
# define COHERENT_RECEIVER_OPTICAL_FRONT_END_H_

# include "../include/netxpto_20180815.h"

# include "../include/optical_hybrid_20180815.h"
# include "../include/balanced_photodetector_20180815.h"
# include "../include/ti_amplifier_20180815.h"

class CoherentReceiverOpticalFrontEnd : public SuperBlock {

public:

	//##############################################################################################################

	CoherentReceiverOpticalFrontEnd(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals) : SuperBlock(inputSignals, outputSignals)
	{
		setSignalsFolderName("signals/SuperBlock_MQAMReceiver/SuperBlock_CoherentReceiverOpticalFrontEnd");
		setLogFileName("SuperBlock_CoherentReceiverOpticalFrontEnd.txt");
	};
	CoherentReceiverOpticalFrontEnd(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals, string sFolderName) : SuperBlock(inputSignals, outputSignals)
	{
		setSignalsFolderName(sFolderName);
		setLogFileName("SuperBlock_CoherentReceiverOpticalFrontEnd.txt");
	};


	void initialize(void);
	bool runBlock(void);

	bool firstTime{ true };

	//##############################################################################################################

	// Photodiodes config
	void  setPhotodiodesResponsivity(t_real Responsivity) { BalancedPhotodetector_0_.setResponsivity(Responsivity); BalancedPhotodetector_1_.setResponsivity(Responsivity); };

	// Ti Amplifier config

	void setTiAmplifierNoiseSamplingPeriod(t_real samplingPeriod) { TiAmplifier_0_.setNoiseSamplingPeriod(samplingPeriod); TiAmplifier_1_.setNoiseSamplingPeriod(samplingPeriod); };
	t_real getTiAmplifierNoiseSamplingPeriod(void) { return TiAmplifier_0_.getNoiseSamplingPeriod(); };

	void setTiAmplifierNoiseSymbolPeriod(t_real symPeriod) { TiAmplifier_0_.setNoiseSymbolPeriod(symPeriod); TiAmplifier_1_.setNoiseSymbolPeriod(symPeriod); }
	t_real getTiAmplifierNoiseSymbolPeriod(void) { return TiAmplifier_0_.getNoiseSymbolPeriod(); };

	void setTiAmplifierGain(t_real gain) { TiAmplifier_0_.setGain(gain); TiAmplifier_1_.setGain(gain); };
	t_real getTiAmplifierGain(void) { TiAmplifier_0_.getGain(); };

	void setTiAmplifierInputReferredNoisePowerSpectralDensity(t_real NoiseSpectralDensity) { TiAmplifier_0_.setInputReferredNoisePowerSpectralDensity(NoiseSpectralDensity); TiAmplifier_1_.setInputReferredNoisePowerSpectralDensity(NoiseSpectralDensity); };
	t_real getTiAmplifierInputReferredNoisePowerSpectralDensity(void) { TiAmplifier_0_.getInputReferredNoisePowerSpectralDensity(); };

	void setTiAmplifierFilterType(Filter fType) { TiAmplifier_0_.setFilterType(fType); TiAmplifier_1_.setFilterType(fType); };

	void setTiAmplifierCutoffFrequency(double ctfFreq) { TiAmplifier_0_.setCutoffFrequency(ctfFreq); TiAmplifier_1_.setCutoffFrequency(ctfFreq); };

	void setTiAmplifierImpulseResponseTimeLength_symbolPeriods(int irl) { TiAmplifier_0_.setImpulseResponseTimeLength(irl);  TiAmplifier_1_.setImpulseResponseTimeLength(irl); };

	void setTiAmplifierImpulseResponse(vector<t_real> ir) { TiAmplifier_0_.setImpulseResponse(ir); TiAmplifier_1_.setImpulseResponse(ir); };

	void setTiAmplifierSeeBeginningOfImpulseResponse(bool sBeginningOfImpulseResponse) { TiAmplifier_0_.setSeeBeginningOfImpulseResponse(sBeginningOfImpulseResponse); TiAmplifier_1_.setSeeBeginningOfImpulseResponse(sBeginningOfImpulseResponse); };
	double const getTiAmplifierSeeBeginningOfImpulseResponse(void) { return TiAmplifier_0_.getSeeBeginningOfImpulseResponse(); };

	//##############################################################################################################

private:

	// #####################################################################################################
	// ################################## Input Parameters #################################################
	// #####################################################################################################

	/*string signalsFolderName{ "signals/SuperBlock_MQamReceiver/SuperBlock_CoherentReceiverOpticalFrontEnd" };
	bool logValue{ true };
	string logFileName{ "SuperBlock_CoherentReceiverOpticalFrontEnd.txt" };
	*/

	// #####################################################################################################
	// ########################### Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	OpticalSignal OpticalSignal_In{ "S0_OpticalSignal_In.sgn" };
	OpticalSignal LocalOscillator_In{ "S1_LocalOscillator_In.sgn" };

	OpticalSignal OpticalHybrid_Out_S_Plus_L{ "S2_OpticalHybrid_Out_S_Plus_L.sgn" }; // S+L
	OpticalSignal OpticalHybrid_Out_S_Minus_L{ "S3_OpticalHybrid_Out_S_Minus_L.sgn" }; // S-L
	OpticalSignal OpticalHybrid_Out_S_Plus_iL{ "S4_OpticalHybrid_Out_S_Plus_iL.sgn" }; // S+iL
	OpticalSignal OpticalHybrid_Out_S_Minus_iL{ "S5_OpticalHybrid_Out_S_Minus_iL.sgn" }; // S-iL

	TimeContinuousAmplitudeContinuousReal BalancedPhotodetector_0_Out{ "S6_BalancedPhotodetector_0_Out.sgn" }; // Photodiode's output
	TimeContinuousAmplitudeContinuousReal BalancedPhotodetector_1_Out{ "S7_BalancedPhotodetector_1_Out.sgn" }; // Photodiode's output

	TimeContinuousAmplitudeContinuousReal TiAmplifier_0_Out{ "S8_TiAmplifier_0_Out.sgn" }; // TI_Amplifier output
	TimeContinuousAmplitudeContinuousReal TiAmplifier_1_Out{ "S9_TiAmplifier_1_Out.sgn" }; // TI_Amplifier output

	
	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	OpticalHybrid OpticalHybrid_{ {&OpticalSignal_In, &LocalOscillator_In },{ &OpticalHybrid_Out_S_Plus_L, &OpticalHybrid_Out_S_Minus_L, &OpticalHybrid_Out_S_Plus_iL, &OpticalHybrid_Out_S_Minus_iL } };

	BalancedPhotodetector BalancedPhotodetector_0_{ { &OpticalHybrid_Out_S_Plus_L, &OpticalHybrid_Out_S_Minus_L }, { &BalancedPhotodetector_0_Out } };
	BalancedPhotodetector BalancedPhotodetector_1_{ { &OpticalHybrid_Out_S_Plus_iL, &OpticalHybrid_Out_S_Minus_iL }, { &BalancedPhotodetector_1_Out } };
	
	TiAmplifier TiAmplifier_0_{ { &BalancedPhotodetector_0_Out }, { &TiAmplifier_0_Out }, "signals/SuperBlock_MQamReceiver/SuperBlock_CoherentReceiverOpticalFrontEnd/TiAmplifier_0_" };
	TiAmplifier TiAmplifier_1_{ { &BalancedPhotodetector_1_Out }, { &TiAmplifier_1_Out }, "signals/SuperBlock_MQamReceiver/SuperBlock_CoherentReceiverOpticalFrontEnd/TiAmplifier_1_" };

};

#endif

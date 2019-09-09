# ifndef M_QAM_RECEIVER_H_
# define M_QAM_RECEIVER_H_

# include "../include/netxpto_20180815.h"

# include "../include/coherent_receiver_optical_front_end_20180815.h"
# include "../include/coherent_receiver_dsp_20180815.h"


class MQAMReceiver : public SuperBlock {

public:

	//##############################################################################################################

	MQAMReceiver(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals) : SuperBlock(inputSignals, outputSignals)
	{
		setSignalsFolderName("signals/SuperBlock_MQAMReceiver");
		setLogFileName("SuperBlock_MQAMReceiver.txt");
	};

	MQAMReceiver(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals, string sFolderName) : SuperBlock(inputSignals, outputSignals)
	{
		setSignalsFolderName(sFolderName);
		setLogFileName("SuperBlock_MQAMReceiver.txt");
	};


	void initialize(void);
	bool runBlock(void);

	bool firstTime{ true };

	//##############################################################################################################

	// Photodiodes config
	void  setPhotodiodesResponsivity(t_real Responsivity) { CoherentReceiverOpticalFrontEnd_.setPhotodiodesResponsivity(Responsivity); };

	// TI Amplifier config

	void setTiAmplifierNoiseSamplingPeriod(t_real samplingPeriod) { CoherentReceiverOpticalFrontEnd_.setTiAmplifierNoiseSamplingPeriod(samplingPeriod); }
	t_real getTiAmplifierNoiseSamplingPeriod(void) { return CoherentReceiverOpticalFrontEnd_.getTiAmplifierNoiseSamplingPeriod(); };

	void setTiAmplifierNoiseSymbolPeriod(t_real symPeriod) { CoherentReceiverOpticalFrontEnd_.setTiAmplifierNoiseSymbolPeriod(symPeriod); }
	t_real getTiAmplifierNoiseSymbolPeriod(void) { return CoherentReceiverOpticalFrontEnd_.getTiAmplifierNoiseSymbolPeriod(); };

	void setTiAmplifierInputNoisePowerSpectralDensity(t_real NoiseSpectralDensity) { CoherentReceiverOpticalFrontEnd_.setTiAmplifierInputReferredNoisePowerSpectralDensity(NoiseSpectralDensity); };
	t_real getTiAmplifierInputNoisePowerSpectralDensity(void) { CoherentReceiverOpticalFrontEnd_.getTiAmplifierInputReferredNoisePowerSpectralDensity(); };

	void setTiAmplifierGain(t_real gain) { CoherentReceiverOpticalFrontEnd_.setTiAmplifierGain(gain); };
	t_real getTiAmplifierGain(void) { CoherentReceiverOpticalFrontEnd_.getTiAmplifierGain(); };

	void setTiAmplifierFilterType(Filter fType) { CoherentReceiverOpticalFrontEnd_.setTiAmplifierFilterType(fType); };

	void setTiAmplifierCutoffFrequency(double ctfFreq) { CoherentReceiverOpticalFrontEnd_.setTiAmplifierCutoffFrequency(ctfFreq); }

	void setTiAmplifierImpulseResponseTimeLength_symbolPeriods(int irl) { CoherentReceiverOpticalFrontEnd_.setTiAmplifierImpulseResponseTimeLength_symbolPeriods(irl); }

	void setElectricalFilterImpulseResponse(vector<t_real> ir) {CoherentReceiverOpticalFrontEnd_.setTiAmplifierImpulseResponse(ir);}

	void setElectricalSeeBeginningOfImpulseResponse(bool sBeginningOfImpulseResponse) { CoherentReceiverOpticalFrontEnd_.setTiAmplifierSeeBeginningOfImpulseResponse(sBeginningOfImpulseResponse); };
	double const getElectricalSeeBeginningOfImpulseResponse(void) { return CoherentReceiverOpticalFrontEnd_.getTiAmplifierSeeBeginningOfImpulseResponse(); };

	// Electrical Filter
	void setElectricalFilterImpulseResponseTimeLength(int impResponseTimeLength) { CoherentReceiverDSP_.setElectricalFilterImpulseResponseTimeLength_symbolPeriods(impResponseTimeLength); };
	void setElectricalFilterType(pulse_shapper_filter_type fType) { CoherentReceiverDSP_.setElectricalFilterFilterType(fType); };
	void setElectricalFilterRollOffFactor(double rOffFactor) { CoherentReceiverDSP_.setElectricalFilterRollOffFactor(rOffFactor); };
	void setElectricalFilterPassiveFilterMode(bool pFilterMode) { CoherentReceiverDSP_.setElectricalFilterPassiveFilterMode(pFilterMode); }
	void setElectricalFilterRrcNormalizeEnergy(bool ne) { CoherentReceiverDSP_.setElectricalFilterRrcNormalizeEnergy(ne); };
	void setElectricalFilterImpulseResponseFilename(string fName) { CoherentReceiverDSP_.setElectricalFilterImpulseResponseFilename(fName); }
	void setElectricalFilterSeeBeginningOfImpulseResponse(bool sBeginningOfImpulseResponse) { CoherentReceiverDSP_.setElectricalFilterSeeBeginningOfImpulseResponse(sBeginningOfImpulseResponse); };
	double const getElectricalFilterFSeeBeginningOfImpulseResponse(void) { return CoherentReceiverDSP_.getElectricalFilterSeeBeginningOfImpulseResponse(); };
	

	// Sampler config	void setSamplesToSkip(int sToSkip) { CoherentReceiverDSP_.setSamplesToSkip(sToSkip); };
	//void setSamplerOpticalPower_dBm(double optPower_dBm) { B14.setOutputOpticalPower_dBm(optPower_dBm); B15.setOutputOpticalPower_dBm(optPower_dBm); };


	// Decoder config
	void setIqAmplitudes(vector<t_iqValues> iqAmplitudesValues) { CoherentReceiverDSP_.setIqAmplitudes(iqAmplitudesValues); };
	vector<t_iqValues> const getIqAmplitudes(void) { return CoherentReceiverDSP_.getIqAmplitudes(); };

	//##############################################################################################################

private:

	// #####################################################################################################
	// ################################## Input Parameters #################################################
	// #####################################################################################################

	/*string signalsFolderName{ "signals/SuperBlock_MQAMReceiver" };
	bool logValue{ true };
	string logFileName{ "SuperBlock_MQAMReceiver.txt" };*/

	//double samplingPeriod{ 0.0 };

	// #####################################################################################################
	// ########################### Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	OpticalSignal OpticalSignal_In{ "S0_OpticalSignal_In.sgn" };
	OpticalSignal LocalOscillator_In{ "S1_LocalOscillator_In.sgn" };

	TimeContinuousAmplitudeContinuousReal InPhase_Out{ "S2_InPhase_Out.sgn" }; // TI_Amplifier output
	TimeContinuousAmplitudeContinuousReal Quadrature_Out{ "S3_InPhase_Out.sgn" }; // TI_Amplifier output

	Binary BinaryData_Out{ "S4_Binary_Data_Out.sgn" }; // recovery Sequence


	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	CoherentReceiverOpticalFrontEnd CoherentReceiverOpticalFrontEnd_{ {&OpticalSignal_In, &LocalOscillator_In },{ &InPhase_Out, &Quadrature_Out }, "signals/SuperBlock_MQAMReceiver/SuperBlock_CoherentReceiverOpticalFrontEnd" };

	CoherentReceiverDSP CoherentReceiverDSP_{ { &InPhase_Out, &Quadrature_Out },{ &BinaryData_Out }, "signals/SuperBlock_MQAMReceiver/SuperBlock_CoherentReceiverDSP" };

};

#endif

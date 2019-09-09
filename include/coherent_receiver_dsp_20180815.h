# ifndef COHERENT_RECEIVER_DSP_H_
# define COHERENT_RECEIVER_DSP_H_

# include "../include/netxpto_20180815.h"

# include "../include/pulse_shaper_20180815.h"
# include "../include/sampler_20180815.h"
# include "../include/decoder_20180815.h"

class CoherentReceiverDSP : public SuperBlock {

public:

	//##############################################################################################################

	CoherentReceiverDSP(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals) : SuperBlock(inputSignals, outputSignals)
	{
		setSignalsFolderName("signals/SuperBlock_MQAMReceiver/SuperBlock_CoherentReceiverDSP");
		setLogFileName("SuperBlock_CoherentReceiverDSP.txt");
	};
	CoherentReceiverDSP(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals, string sFolderName) : SuperBlock(inputSignals, outputSignals)
	{
		setSignalsFolderName(sFolderName);
		setLogFileName("SuperBlock_CoherentReceiverDSP.txt");
	};

	void initialize(void);
	bool runBlock(void);

	bool firstTime{ true };

	//##############################################################################################################

	// Pulse shaper config
	void setElectricalFilterImpulseResponseTimeLength_symbolPeriods(int impResponseTimeLength) { ReceiverFilter_0_.setImpulseResponseTimeLength_symbolPeriods(impResponseTimeLength); ReceiverFilter_1_.setImpulseResponseTimeLength_symbolPeriods(impResponseTimeLength); };
	void setElectricalFilterFilterType(pulse_shapper_filter_type fType) { ReceiverFilter_0_.setFilterType(fType); ReceiverFilter_1_.setFilterType(fType); };
	void setElectricalFilterRollOffFactor(double rOffFactor) { ReceiverFilter_0_.setRollOffFactor(rOffFactor);  ReceiverFilter_1_.setRollOffFactor(rOffFactor); };
	void setElectricalFilterPassiveFilterMode(bool pFilterMode) { ReceiverFilter_0_.setPassiveFilterMode(pFilterMode); ReceiverFilter_1_.setPassiveFilterMode(pFilterMode); }
	void setElectricalFilterRrcNormalizeEnergy(bool ne) { ReceiverFilter_0_.setRrcNormalizeEnergy(ne); ReceiverFilter_1_.setRrcNormalizeEnergy(ne); };
	void setElectricalFilterImpulseResponseFilename(string fName) { ReceiverFilter_0_.setImpulseResponseFilename(fName); ReceiverFilter_1_.setImpulseResponseFilename(fName); }
	void setElectricalFilterSeeBeginningOfImpulseResponse(bool sBeginningOfImpulseResponse) { ReceiverFilter_0_.setSeeBeginningOfImpulseResponse(sBeginningOfImpulseResponse); ReceiverFilter_1_.setSeeBeginningOfImpulseResponse(sBeginningOfImpulseResponse); };
	double const getElectricalFilterSeeBeginningOfImpulseResponse(void) { return ReceiverFilter_0_.getSeeBeginningOfImpulseResponse(); };

	// Sampler config
	void setSamplesToSkip(int sToSkip) { Sampler_0_.setSamplesToSkip(sToSkip); Sampler_1_.setSamplesToSkip(sToSkip); };
	//void setSamplerOpticalPower_dBm(double optPower_dBm) { B14.setOutputOpticalPower_dBm(optPower_dBm); B15.setOutputOpticalPower_dBm(optPower_dBm); };

	// Decoder config
	void setIqAmplitudes(vector<t_iqValues> iqAmplitudesValues) { Decoder_.setIqAmplitudes(iqAmplitudesValues); };
	vector<t_iqValues> const getIqAmplitudes(void) { return Decoder_.getIqAmplitudes(); };

	//##############################################################################################################

private:

	// #####################################################################################################
	// ################################## Input Parameters #################################################
	// #####################################################################################################


	//double samplingPeriod{ 0.0 };

	// #####################################################################################################
	// ########################### Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	TimeContinuousAmplitudeContinuousReal InPhase_In{ "S0_InPhase_In.sgn" }; // TI_Amplifier output
	TimeContinuousAmplitudeContinuousReal Quadrature_In{ "S1_Quadrature_In.sgn" }; // TI_Amplifier output

	TimeContinuousAmplitudeContinuousReal ReceiverFilter_0_Out{ "S2_ReceiverFilter_0_Out.sgn" }; // Filtered
	TimeContinuousAmplitudeContinuousReal ReceiverFilter_1_Out{ "S3_ReceiverFilter_1_Out.sgn" }; // Filtered

	TimeDiscreteAmplitudeContinuousReal Sampler_0_Out{ "S4_Sampler_0_Out.sgn" }; // Sampled 
	TimeDiscreteAmplitudeContinuousReal Sampler_1_Out{ "S5_Sampler_1_Out.sgn" }; // Sampled 

	Binary BinaryData_Out{ "S18_Binary_Data_Out.sgn" }; // recovery Sequence


	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	PulseShaper ReceiverFilter_0_{ { &InPhase_In },{ &ReceiverFilter_0_Out } }; // Matched Filter
	PulseShaper ReceiverFilter_1_{ { &Quadrature_In },{ &ReceiverFilter_1_Out } }; // Matched Filter

	Sampler Sampler_0_{ { &ReceiverFilter_0_Out },{ &Sampler_0_Out } };
	Sampler Sampler_1_{ { &ReceiverFilter_1_Out },{ &Sampler_1_Out } };
	
	Decoder Decoder_{ { &Sampler_0_Out, &Sampler_1_Out },{ &BinaryData_Out } };

};

#endif

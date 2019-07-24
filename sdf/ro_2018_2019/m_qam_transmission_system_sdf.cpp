# include "../../include/netxpto_20180815.h"

# include "../../include/binary_source_20180815.h"
# include "../../include/laser_20180815.h"
# include "../../include/m_qam_transmitter_20180815.h"
# include "../../include/m_qam_receiver_20190815.h"
# include "../../include/bit_error_rate_20180815.h"
# include "../../include/sink_20180815.h"


// #####################################################################################################
// ################################### System Input Parameters #########################################
// #####################################################################################################

int constellationCardinality{ 4 };	
double symbolPeriod_s{ 2e-11 };	
int samplesPerSymbol{ 8 };
double txLocalOscillatorPower_dBm{ 0 };
pulse_shapper_filter_type txPulseShapperType{ pulse_shapper_filter_type::RootRaisedCosine };
double raisedCosineRollOffFactor{ 0.1 };
int txPulseShaperLength_symbolPeriods{ 20 };
double rxLocalOscillatorPower_dBm{ 6 };
double rxTiAmplifierInputNoisePowerSpectralDensity{ 0 };
int rxTiAmplifierImpulseResponseTimeLength_symbolPeriods{ 20 };
double rxTiAmplifierBandwidth{ 50.0e9 };
pulse_shapper_filter_type rxElectricalFilterImpulseResponseType{ pulse_shapper_filter_type::RootRaisedCosine };
int rxSamplerSamplesToSkip{ (txPulseShaperLength_symbolPeriods - 1)*samplesPerSymbol + (rxTiAmplifierImpulseResponseTimeLength_symbolPeriods *samplesPerSymbol/2)};


int main() {

	// #####################################################################################################
	// ########################### Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	Binary BinarySourceOut_0{ "S0_BinarySourceOut_0.sgn" };

	Binary BinarySourceOut_1{ "S1_BinarySourceOut_0.sgn" };

	OpticalSignal TxLocalOscillatorOut{ "S2_TxLocalOscillatorOut.sgn" };
	
	OpticalSignal MQAMTransmitterOut{ "S3_MQAMTransmitterOut.sgn" };
	
	OpticalSignal RxLocalOscillatorOut{ "S4_RxLocalOscillatorOut.sgn" };

	Binary MQAMReceiverOut{ "S5_MQAMReceiverOut.sgn" };

	Binary BitErrorRateOut{ "S6_BitErrorRateOut.sgn" };

		
	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################


	BinarySource BinarySource_{ {}, { &BinarySourceOut_0, &BinarySourceOut_1 } };
	BinarySource_.setBitPeriod(symbolPeriod_s / log2(constellationCardinality));
	BinarySource_.setNumberOfBits(100);

	Laser TxLocalOscillator_{ {},{ &TxLocalOscillatorOut } };
	TxLocalOscillator_.setSymbolPeriod(symbolPeriod_s);
	TxLocalOscillator_.setSamplingPeriod(symbolPeriod_s / samplesPerSymbol);
	TxLocalOscillator_.setOpticalPower_dBm(txLocalOscillatorPower_dBm);

	MQAMTransmitter MQAMTransmitter_{ {&BinarySourceOut_0, &TxLocalOscillatorOut}, {&MQAMTransmitterOut}, "signals/SuperBlock_MQAMTransmitter"  };
	MQAMTransmitter_.setLogValue(true);
	MQAMTransmitter_.setSaveInternalSignals(true);
	MQAMTransmitter_.setNumberOfSamplesPerSymbol(samplesPerSymbol);
	MQAMTransmitter_.setImpulseResponseTimeLength_symbolPeriods(txPulseShaperLength_symbolPeriods);
	MQAMTransmitter_.setFilterType(txPulseShapperType);
	MQAMTransmitter_.setRollOffFactor(raisedCosineRollOffFactor);

	Laser RxLocalOscillator_{ {},{ &RxLocalOscillatorOut } };
	RxLocalOscillator_.setSymbolPeriod(symbolPeriod_s);
	RxLocalOscillator_.setSamplingPeriod(symbolPeriod_s / samplesPerSymbol);
	RxLocalOscillator_.setOpticalPower_dBm(rxLocalOscillatorPower_dBm);

	MQAMReceiver MQAMReceiver_{ {&MQAMTransmitterOut, &RxLocalOscillatorOut} , {&MQAMReceiverOut}, "signals/SuperBlock_MQAMReceiver" };
	MQAMReceiver_.setLogValue(true);
	MQAMReceiver_.setSaveInternalSignals(true);
	MQAMReceiver_.setPhotodiodesResponsivity(1.0);
	MQAMReceiver_.setTiAmplifierNoiseSamplingPeriod(symbolPeriod_s / samplesPerSymbol);
	MQAMReceiver_.setTiAmplifierNoiseSymbolPeriod(symbolPeriod_s);
	MQAMReceiver_.setTiAmplifierInputNoisePowerSpectralDensity(rxTiAmplifierInputNoisePowerSpectralDensity);
	MQAMReceiver_.setTiAmplifierGain(1.0);
	MQAMReceiver_.setTiAmplifierFilterType(LowPass);
	MQAMReceiver_.setTiAmplifierCutoffFrequency(rxTiAmplifierBandwidth);
	MQAMReceiver_.setTiAmplifierImpulseResponseTimeLength_symbolPeriods(rxTiAmplifierImpulseResponseTimeLength_symbolPeriods);
	MQAMReceiver_.setElectricalFilterType(rxElectricalFilterImpulseResponseType);
	MQAMReceiver_.setElectricalFilterRollOffFactor(raisedCosineRollOffFactor);

	//MQAMReceiver_.setElectricalFilterSamplesToSkip(rxSamplerSamplesToSkip);


	BitErrorRate BitErrorRate_{ {&MQAMReceiverOut, &BinarySourceOut_1},{ &BitErrorRateOut} };
	
	Sink Sink_{ {&BitErrorRateOut},{} };
	
	// #####################################################################################################
	// ########################### System Declaration and Inicialization ###################################
	// #####################################################################################################

	System MainSystem{ 
		{ 
			&BinarySource_,
			&TxLocalOscillator_, 
			&MQAMTransmitter_,
			&RxLocalOscillator_,
			&MQAMReceiver_,
			&BitErrorRate_,
			&Sink_	
		}
	};

	MainSystem.setLogValue(true);
	MainSystem.setSaveSignals(true);

	// #####################################################################################################
	// #################################### System Run #####################################################
	// #####################################################################################################

	MainSystem.run();
	MainSystem.terminate();

	system("pause");
	return 0;

}
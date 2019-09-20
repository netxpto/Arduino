#ifndef SINGLE_PHOTON_RECEIVER_H
#define SINGLE_PHOTON_RECEIVER_H

#include "netxpto_20180815.h"
#include "pulse_shaper_20180815.h"
#include "clock_20180815.h"
#include "sampler_20180815.h"

#include "coincidence_detector_20190319.h"


//#include "super_block_interface_20190319.h"
#include "bit_decider_20190319.h"

class SinglePhotonReceiver : public SuperBlock{ 

	// #####################################################################################################
	// ################## Internal Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	TimeContinuousAmplitudeContinuousReal SPR01{ "SPR01.sgn" }; // single photon detector output
	TimeContinuousAmplitudeContinuousReal SPR02{ "SPR02.sgn" }; //shapper
	TimeContinuousAmplitudeContinuousReal SPR03{ "SPR03.sgn" }; // single photon detector output
	TimeContinuousAmplitudeContinuousReal SPR04{ "SPR04.sgn" }; //clock
	TimeContinuousAmplitudeContinuousReal SPR05{ "SPR05.sgn" }; //sampler

	TimeContinuousAmplitudeContinuousReal SPR06{ "SPR06.sgn" }; //sampler
	Binary SPR07{ "SPR07.sgn" };
	
	Binary SPR08{ "SPR08.sgn" };
//	TimeDiscreteAmplitudeContinuousReal SPR09{ "SPR09.sgn" };
//	TimeDiscreteAmplitudeContinuousReal SPR09_out{ "SPR09_out.sgn" };

	TimeContinuousAmplitudeContinuousReal SinglePhotonDectection_In_1{ "SinglePhotonDectection_In_1.sgn" }; // single photon detector output
	TimeContinuousAmplitudeContinuousReal SinglePhotonDectection_In_2{ "SinglePhotonDectection_In_2.sgn" }; // single photon detector output
	
	TimeDiscreteAmplitudeContinuousReal MeasurementResult_Out{ "MeasurementResult_Out.sgn" };

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################
	PulseShaper B1{ { &SinglePhotonDectection_In_1 }, { &SPR01 } };
	PulseShaper B2{ { &SinglePhotonDectection_In_2 }, { &SPR02 } };
	Clock B3{ {}, { &SPR03 } };
	Clock B4{ {}, { &SPR04 } };
	Sampler B5{ { &SPR01, &SPR03 }, { &SPR05 } };
	Sampler B6{ { &SPR02, &SPR04 }, { &SPR06 } };
	BitDecider B7{ { &SPR05 }, { &SPR07 } };
	BitDecider B8{ { &SPR06 }, { &SPR08 } };
	CoincidenceDetector B9{ { &SPR07, &SPR08 }, { &MeasurementResult_Out } };
	//SuperBlockInterface B10{ { &SPR09 }, { &SPR09_out } };


public:

	/* Input Parameters */
	int samplesToSkip{ 0 };
	pulse_shapper_filter_type filterType{ pulse_shapper_filter_type::Square };

	/*Methods*/
	SinglePhotonReceiver(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals) : SuperBlock(inputSignals, outputSignals) {};

	void initialize(void);
	bool runBlock(void);

	/*Set Methods*/
	void setPulseShaperFilter(pulse_shapper_filter_type fType) { B1.setFilterType(fType); B2.setFilterType(fType); };
	void setPulseShaperWidth(double pulseW) { B1.setPulseWidth(pulseW); B2.setPulseWidth(pulseW); };
	void setPulseImpulseResponseTimeLength(int n) { B1.setImpulseResponseTimeLength_symbolPeriods(n); B2.setImpulseResponseTimeLength_symbolPeriods(n);};
	void setClockBitPeriod(double period) { B3.setClockPeriod(period); B4.setClockPeriod(period); };
	void setClockPhase(double phase) { B3.setClockPhase(phase); B4.setClockPhase(phase); };
	void setClockSamplingPeriod(double sPeriod) { B3.setSamplingPeriod(sPeriod); B4.setSamplingPeriod(sPeriod); };
	void setThreshold(double threshold) { B7.setDecisionLevel(threshold); B8.setDecisionLevel(threshold); };
	
	private:

		string signalsFolderName{ "SinglePhotonReceiver" };
		bool logValue{ true };
		string logFileName{ "SinglePhotonReceiver.txt" };

		bool firstTime{ true };
};

#endif

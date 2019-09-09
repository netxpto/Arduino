#ifndef BOB_QKD_H_
#define BOB_QKD_H

#include "netxpto_20180815.h"
#include "discrete_to_continuous_time_20180815.h"
#include "pulse_shaper_20180815.h"
#include "fork_20180815.h"
#include "sink_20180815.h"
#include "clock_20180815.h"

#include "bobBB84_20190319.h"
//#include "super_block_interface_20190319.h"
#include "message_processor_bob_20190319.h"
#include "demux_1_2_20190319.h"
#include "binary_mapper_20190319.h"

class BobQKD : public SuperBlock {

	// #####################################################################################################
	// ################## Internal Signals Declaration and Inicialization ##################################
	// #####################################################################################################
	TimeDiscreteAmplitudeContinuousReal Bob_1{ "Bob_1.sgn" };              // Amplitude Discrete
	TimeContinuousAmplitudeContinuousReal Bob_2{ "Bob_2.sgn" };            // Amplitude Discrete
	TimeContinuousAmplitudeContinuousReal Bob_3{ "Bob_3.sgn" };            // Amplitude Discrete
	TimeContinuousAmplitudeContinuousReal Bob_12{ "Bob_12.sgn" };
	TimeContinuousAmplitudeContinuousReal Bob_13{"Bob_13.sgn"};

	TimeDiscreteAmplitudeContinuousReal Bob_7{ "Bob_7.sgn" };              // Amplitude Discrete
	TimeContinuousAmplitudeContinuousReal Bob_4{ "Bob_4.sgn" };            // Amplitude Discrete
	Binary Bob_5{ "Bob_5.sgn" };
	Binary Bob_6{ "Bob_6.sgn" };
	Binary Bob_9{ "Bob_9.sgn" };
	TimeDiscreteAmplitudeContinuousReal Bob_10{ "Bob_10.sgn" };            // Amplitude Discrete
	TimeDiscreteAmplitudeContinuousReal Bob_11{ "Bob_11.sgn" };            // Amplitude Discrete

	TimeContinuousAmplitudeContinuousReal Bob_8{ "Bob_8.sgn" };            // Amplitude Discrete

	TimeContinuousAmplitudeContinuousReal CLKB_out1{ "CLKB_out1.sgn" };    // Amplitude Discrete
	TimeContinuousAmplitudeContinuousReal CLKB_out2{ "CLKB_out2.sgn" };    // Amplitude Discrete

	Message C_C_2{ "C_C_2.sgn" };
	Message C_C_1{ "C_C_1.sgn" };

	Binary Bob_6_out{ "Bob_6_out.sgn" };

	Binary BinaryData_In{ "BinaryData_1.sgn"};
	TimeContinuousAmplitudeContinuousReal Clock_In{ "Clock_In.sgn" };
	TimeDiscreteAmplitudeContinuousReal SinglePhotonReceiverMeasurement_In{ "SinglePhotonReceiverMeasurement_in.sgn" };
	Binary MeasurementBasis_In{ "MeasurementBasis_In.sgn" };
	Message Messages_In{ "Messages_In.sgn" };

	TimeContinuousAmplitudeContinuousReal PolarizationRotatorAngle_Out{ "PolarizationRotatorAnlge_Out.sgn" };
	TimeContinuousAmplitudeContinuousReal ClockBob_Photodetector_Out_1{ "ClockBob_Photodetector_Out_1.sgn" };
	TimeContinuousAmplitudeContinuousReal ClockBob_Photodetector_Out_2{ "ClockBob_Photodetector_Out_2.sgn" };
	Message Messages_Out{ "Messages_Out.sgn" };
	Binary MutualInformation_Out{ "MutualInformation_Out.sgn" };

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	BinaryMapper BB0{ { &BinaryData_In }, { &Bob_1 } };
	DiscreteToContinuousTime BB1{ { &Bob_1 }, { &Bob_2 } };
	PulseShaper BB2{ { &Bob_2 }, { &PolarizationRotatorAngle_Out } };

	PulseShaper BB3{ { &Clock_In }, { &Bob_3 } };
	Fork BB4{ { &Bob_3 }, { &ClockBob_Photodetector_Out_1, &ClockBob_Photodetector_Out_2 } };


	bobBB84 BB5{ {&Bob_7,&MeasurementBasis_In}, { &Bob_10 } };

	Demux_1_2 BB6{ { &Bob_11, &Bob_5 }, { &MutualInformation_Out, &Bob_9 } }; // Bases and measurement
	MessageProcessorBob BB7{ {&Bob_10, &Messages_In}, { &Bob_5, &Messages_Out } };


//	SuperBlockInterface BB9{ { &Bob_8, &CLKB_out1, &CLKB_out2, &C_C_2, &Bob_6 }, { outputSignals[0], outputSignals[1], outputSignals[2], outputSignals[3], &Bob_6_out } };

	Fork BB10{ {&SinglePhotonReceiverMeasurement_In}, { &Bob_7, &Bob_11 } };
	

	Sink BBSink0{ {&Bob_9}, {} };


public:

//	BobQKD(vector <Signal*> &inputSignals, vector <Signal*> &outputSignals);
	BobQKD(initializer_list<Signal *> inputSignal, initializer_list<Signal *> outputSignal) : SuperBlock(inputSignal, outputSignal) {};

	void initialize(void);
	bool runBlock(void);

	void setNumberOfSamplesPerSymbol(int n) { BB1.setNumberOfSamplesPerSymbol(n); };
	int const getNumberOfSamplesPerSymbol(void) { return BB1.getNumberOfSamplesPerSymbol(); };

	void setSeeBeginningOfImpulseResponse(bool sBeginningOfImpulseResponse) { BB2.setSeeBeginningOfImpulseResponse(sBeginningOfImpulseResponse); };
	double const getSeeBeginningOfImpulseResponse(void) { return BB2.getSeeBeginningOfImpulseResponse(); };

	void setPulseShaperFilter(pulse_shapper_filter_type fType) { BB2.setFilterType(fType); BB3.setFilterType(fType); };
	void setPulseImpulseResponseTimeLength(int n) { BB2.setImpulseResponseTimeLength_symbolPeriods(n); BB3.setImpulseResponseTimeLength_symbolPeriods(n);
	};

	void setPulseWidth(double tWidth) { BB3.setPulseWidth(tWidth); };
	double getPulseWidth() { return BB3.getPulseWidth(); };

	void setPulseDelay(double pDelay) { BB3.setPulseDelay(pDelay); };
	double getPulseDelay() { return BB3.getPulseDelay(); };

private:
	int count = 0;
	string signalsFolderName{ "Bob_QKD" };
	bool logValue{ true };
	string logFileName{ "Bob_QKD.txt" };

	bool firstTime{ true };
};

#endif

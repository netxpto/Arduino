#ifndef ALICE_QKD_H_
#define ALICE_QKD_H

#include "netxpto_20180815.h"
#include "binary_source_20180815.h"
#include "m_qam_mapper_20180815.h"
#include "discrete_to_continuous_time_20180815.h"
#include "pulse_shaper_20180815.h"
#include "clock_20180815.h"
#include "sink_20180815.h"

#include "demux_1_2_20190319.h"
#include "message_processor_alice_20190319.h"

class AliceQKD : public SuperBlock {
	/* State Variables */

	// #####################################################################################################
	// ################## Internal Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	//Binary S1{ "NUM_A.sgn" };

	TimeDiscreteAmplitudeContinuousReal alice_1{ "alice_1.sgn" }; // Amplitude discrete


	TimeDiscreteAmplitudeContinuousReal alice_2{ "alice_2.sgn" }; // Amplitude discrete

	TimeContinuousAmplitudeDiscreteReal alice_3{ "alice_3.sgn" };

	TimeContinuousAmplitudeDiscreteReal alice_4{ "alice_4.sgn" };
	TimeContinuousAmplitudeDiscreteReal alice_4_out{ "alice_4_out.sgn" };

	Binary alice_5{ "alice_5.sgn" };

	TimeContinuousAmplitudeContinuousReal clkA_out{ "clkA_out.sgn" };

	Binary alice_12_out{ "alice_12_out.sgn" };

	Binary alice_6{ "alice_6.sgn" };
	Binary alice_6_out{ "alice_6_out.sgn" };

	Binary alice_7{ "alice_7.sgn" };

	Binary alice_8{ "alice_8.sgn" };

	Binary alice_9{ "alice_9.sgn" };

	Binary alice_10{ "alice_10.sgn" };

	Binary alice_11{ "alice_11.sgn" };

	Binary alice_12{ "alice_12.sgn" };

	Binary alice_13{ "alice_13.sgn" };

	Message C_C_1{ "C_C_1.sgn" };
	Message C_C_1_out{ "C_C_1_out.sgn" };

	Binary MI_Alice{ "MI_alice.sgn" };

	TimeContinuousAmplitudeContinuousReal ClockAlice_In{ "ClockAlice_In.sgn" };
	Binary BinaryData_In_1{ "BinaryData_in_1.sgn" };
	Binary BinaryData_In_2{ "BinaryData_in_2.sgn" };
	Binary BinaryData_In_3{ "BinaryData_in_3.sgn" };
	Message Messages_In{ "Messages_In.sgn" };

	//TimeContinuousAmplitudeContinuousReal ClockAlice_Out{ "ClockAlice_Out.sgn" }; // Using ClockAlice_In
	TimeContinuousAmplitudeContinuousReal Polarization_out{ "Polarization_Out.sgn" };
	Binary MutualInformation_out{ "MutualInformation_out.sgn" };
	Message Messages_Out{ "Messages_Out.sgn" };

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	MQamMapper BA1{ {&BinaryData_In_1}, {&alice_1, &alice_2} };

	DiscreteToContinuousTime BA2{ {&alice_1}, {&alice_3} };

	PulseShaper BA3{ {&alice_3}, {&Polarization_out} };

	Sink BA4{ {&alice_2}, {} };

	Demux_1_2 BA6{ {&BinaryData_In_2, &alice_8}, {&alice_6, &alice_7} };

	MessageProcessorAlice BA7{ {&alice_6, &Messages_In}, {&alice_5, &Messages_Out} };

	Demux_1_2 BA8{ {&BinaryData_In_3, &alice_11}, {&alice_9, &alice_10} };

	Demux_1_2 BA9{ {&alice_9, &alice_5}, {&MutualInformation_out, &alice_13} }; // Bases and measurement

	BinarySource BA11{ {}, {&alice_8} };

	Sink BA12{ {&alice_7}, {} };


	BinarySource BA13{ {}, {&alice_11} };

	Sink BA14{ {&alice_10}, {} };

	Sink BA15{ {&alice_13}, {} };

public:
	/* input parameters*/



	// Methods
	AliceQKD(initializer_list<Signal *> inputSignal, initializer_list<Signal *> outputSignal) : SuperBlock(inputSignal, outputSignal) {};;

	void initialize(void);
	bool runBlock(void);

	
	// Set methods
	void set(int opt);

	void setM(int mValue) { BA1.setM(mValue); };
	//int const getM(void) { return BA1.m; };

	void setIqAmplitudes(vector<vector<t_real>> iqAmplitudesValues) { BA1.setIqAmplitudes(iqAmplitudesValues); };
//	vector<t_iqValues> const getIqAmplitudes(void) { return BA1.iqAmplitudes; };

	void setNumberOfSamplesPerSymbol(int n) { BA2.setNumberOfSamplesPerSymbol(n); };
	int const getNumberOfSamplesPerSymbol(void) { return BA2.getNumberOfSamplesPerSymbol(); };

	void setRollOffFactor(double rOffFactor) { BA3.setRollOffFactor(rOffFactor); };
	double const getRollOffFactor(void) { return BA3.getRollOffFactor(); };

	void setSeeBeginningOfImpulseResponse(bool sBeginningOfImpulseResponse) { BA3.setSeeBeginningOfImpulseResponse(sBeginningOfImpulseResponse); };
	double const getSeeBeginningOfImpulseResponse(void) { return BA3.getSeeBeginningOfImpulseResponse(); };

	void setPulseShaperFilter(pulse_shapper_filter_type fType) { BA3.setFilterType(fType); };
	void setPulseImpulseResponseTimeLength(int n) { BA3.setImpulseResponseTimeLength_symbolPeriods(n); };

	void setNumberOfSamples(long int nOfSamples) { BA4.setNumberOfSamples(nOfSamples); BA12.setNumberOfSamples(nOfSamples); BA14.setNumberOfSamples(nOfSamples); BA15.setNumberOfSamples(nOfSamples); };

	void setBitStream(string bitstream) { BA11.setBitStream(bitstream); BA13.setBitStream(bitstream); };

	void setMode(BinarySourceMode m) { BA11.setMode(m); BA13.setMode(m); };

	void setNumberOfBits(int n) { BA11.setNumberOfBits(n); BA13.setNumberOfBits(n); };

	void setDisplayNumberOfSamples(bool opt) { BA4.setDisplayNumberOfSamples(opt); };

	void setSymbolPeriodMIA(double sPeriod) { BA9.setSymbolPeriod(sPeriod); };

	private:
		
		string signalsFolderName{ "Alice_QKD" };
		bool logValue{ true };
		string logFileName{ "Alice_QKD.txt" };
		
		bool firstTime{ true };

		double RateOfPhotons{ 1e3 };
		int StringPhotonsLength{ 12 };
};

#endif

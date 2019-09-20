#ifndef ALICE_QKD_H_
#define ALICE_QKD_H

#include "../include/netxpto_20180815.h"

#include "../include/binary_source_20180815.h"
#include "../include/framer_20190326.h"
#include "../include/message_processor_alice_20190326.h"

class AliceQKD : public SuperBlock {


	// #####################################################################################################
	// ################## Internal Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	Binary AliceData_In{ "S0_AliceData_In.sgn" };

	Binary Framer_Data_Out_Bob{ "S1_Framer_Data_Out_Bob.sgn" };

	Binary Framer_Data_Out_Alice_MP{ "S2_Framer_Data_Out_Alice_MP.sgn" };

	Binary AliceBasis_In{ "S3_AliceBasis_In.sgn" };

	Binary Framer_Basis_Out_Bob{ "S4_Framer_Basis_Out_Bob.sgn" };

	Binary Framer_Basis_Out_Alice_MP{ "S5_Framer_Basis_Out_Alice_MP.sgn" };

	Message Messages_In{ "S6_Messages_In.sgn" };

	Message Messages_Out{ "S7_Messages_Out.sgn" };

	Binary CorrectedKey{ "S8_CorrectedKey.sgn" };

	Binary SiftedKey{ "S9_SiftedKey.sgn" };

	TimeContinuousAmplitudeContinuousReal TransmissionEnable{ "TransmissionEnable.sgn" };

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	Framer Framer_Data_{ { &AliceData_In },{ &Framer_Data_Out_Bob, &Framer_Data_Out_Alice_MP } };

	Framer Framer_Basis_{ { &AliceBasis_In },{ &Framer_Basis_Out_Bob, &Framer_Basis_Out_Alice_MP } };

	MessageProcessorAlice MessageProcessorAlice_{ {&Messages_In, &Framer_Data_Out_Alice_MP, &Framer_Basis_Out_Alice_MP },{&Messages_Out, &CorrectedKey, &TransmissionEnable, &SiftedKey} };

public:

	// #####################################################################################################
	// ########################################## Block Methods ############################################
	// #####################################################################################################

	AliceQKD(initializer_list<Signal *> inputSignal, initializer_list<Signal *> outputSignal) : SuperBlock(inputSignal, outputSignal) {};;

	void initialize(void);
	bool runBlock(void);


	// #####################################################################################################
	// ########################################## Public Methods ###########################################
	// #####################################################################################################

	// Framers
	void setFramerLength(t_integer fLen) { Framer_Data_.setFrameLength(fLen); Framer_Basis_.setFrameLength(fLen); }
	void setFramerHeaderLength(t_integer fHLen) { Framer_Data_.setHeaderLength(fHLen); Framer_Basis_.setHeaderLength(fHLen); }
	void setFramerHeaderSignalLength(t_integer fHSLen) { Framer_Data_.setHeaderSignalingLength(fHSLen); Framer_Basis_.setHeaderSignalingLength(fHSLen); }
	
	void setCascadeWindowSize(t_integer cw) { MessageProcessorAlice_.setCascadeWindowSize(cw); }

	// Alice QKD
	void setCascadeMaxWindows(t_integer mw) { MessageProcessorAlice_.setCascadeMaxWindows( mw ); }
	void setCascadeNumPasses(t_integer np) { MessageProcessorAlice_.setCascadeNumPasses(np); }
/*
	// Binary Sources
	void setModeDataBinarySource(BinarySourceMode m) { BinarySource_Data_.setMode(m); }
	void setModeBasisBinarySource(BinarySourceMode m) { BinarySource_Basis_.setMode(m); }
	void setSeedDataBinarySource(int seed) { BinarySource_Data_.setSeed(seed); }
	void setSeedBasisBinarySource(int seed) { BinarySource_Basis_.setSeed(seed); }
	void setNumberOfBits(int nb) { BinarySource_Data_.setNumberOfBits(nb); BinarySource_Basis_.setNumberOfBits(nb);}
*/
/*
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
	*/

	private:
		
		bool logValue{ true };
		string signalsFolderName{ "Alice_QKD" };
		string logFileName{ "Alice_QKD.txt" };
		bool firstTime{ true };
};

#endif

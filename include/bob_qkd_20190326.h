#ifndef BOB_QKD_H_
#define BOB_QKD_H

#include "../include/netxpto_20180815.h"
#include "../include/message_processor_bob_20190326.h"
#include "../include/framer_20190326.h"
#include "../include/binary_source_20180815.h"

class BobQKD : public SuperBlock {

	// #####################################################################################################
	// ################## Internal Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	TimeDiscreteAmplitudeContinuousReal DataFrame_In{ "BinaryDataFrame_In.sgn" };

	Binary Basis_In{ "Basis_In.sgn" };

	Binary Basis_Out{ "Basis_Out.sgn" };

	TimeDiscreteAmplitudeContinuousReal BasisWithErrors_In{ "BasisWithErrors_In.sgn" };

	TimeDiscreteAmplitudeContinuousReal BasisFrame_In_MP{ "BasisFrame_In_MP.sgn" };

	Message Messages_In{ "Messages_In.sgn" };

	Message Messages_Out{ "Messages_Out.sgn" };

	Binary CorrectedKey{ "CorrectedKey.sgn" };

	Binary SiftedKey{ "SiftedKey.sgn" };

	TimeContinuousAmplitudeContinuousReal TransmissionEnable{ "TransmissionEnable.sgn" };

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

//	Framer Framer_Basis_{ {&BasisWithErrors_In},{&BasisFrame_In_MP} };
	Framer Framer_Basis_{ {&Basis_In},{&BasisFrame_In_MP} };

	MessageProcessorBob MessageProcessorBob_{ {&Messages_In, &DataFrame_In, &BasisFrame_In_MP, &TransmissionEnable },{&Messages_Out, &CorrectedKey, &SiftedKey} };


public:
	
	// #####################################################################################################
	// ########################################## Block Methods ############################################
	// #####################################################################################################

	BobQKD(initializer_list<Signal *> inputSignal, initializer_list<Signal *> outputSignal) : SuperBlock(inputSignal, outputSignal) {};

	void initialize(void);
	bool runBlock(void);

	// #####################################################################################################
	// ########################################## Public Methods ###########################################
	// #####################################################################################################

	// Framers
	void setFramerLength(t_integer fLen) {  Framer_Basis_.setFrameLength(fLen); }
	void setFramerHeaderLength(t_integer fHLen) {  Framer_Basis_.setHeaderLength(fHLen); }
	void setFramerHeaderSignalLength(t_integer fHSLen) {  Framer_Basis_.setHeaderSignalingLength(fHSLen); }

	void setCascadeWindowSize(t_integer cw) { MessageProcessorBob_.setCascadeWindowSize(cw); }
	void setCascadeNumPasses(t_integer np) { MessageProcessorBob_.setCascadeNumPasses(np); }
	/*
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
	*/
private:

	bool logValue{ true };
	string signalsFolderName{ "Bob_QKD" };
	string logFileName{ "Bob_QKD.txt" };
	bool firstTime{ true };
};

#endif

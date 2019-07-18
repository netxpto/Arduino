#ifndef ALICE_QKD_H_
#define ALICE_QKD_H_

#include "../../../include/netxpto_20180815.h"

#include "./framer_20190326.h"
#include "./message_processor_receiver_20190410.h"
#include "./message_processor_transmitter_20190410.h"
#include "./basis_reconciliation_20190410.h"
#include "./error_correction_20190410.h"
#include "./privacy_amplification_20190410.h"
#include "./alice_incoming_data_processor_20190410.h"
#include "./parameter_estimation_20190528.h"


class AliceQKD : public SuperBlock {

public:

	// #####################################################################################################

	AliceQKD(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals) :
		SuperBlock(inputSignals, outputSignals),
		folderName{ getSignalsFolderName() },
		maximumECSyndromeSize{8192}
		{
			setLogFileName("SuperBlock_AliceQKD.txt");
		};
	
	AliceQKD(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals, string sFolderName) :
		SuperBlock(inputSignals, outputSignals),
		folderName{ sFolderName },
		maximumECSyndromeSize { 8192 },
		buffSize{ 512 }
	{
		setSignalsFolderName(sFolderName);
		setLogFileName("SuperBlock_AliceQKD.txt");
	};

	AliceQKD(initializer_list<Signal*> inputSignals, initializer_list<Signal*> outputSignals, t_integer maxSamplesToProcessAtOnce) :
		SuperBlock(inputSignals, outputSignals),
		folderName{ getSignalsFolderName() },
		maximumECSyndromeSize{ maxSamplesToProcessAtOnce },
		buffSize{ 512 }
	{
		setLogFileName("SuperBlock_AliceQKD.txt");
	};

	AliceQKD(initializer_list<Signal*> inputSignals, initializer_list<Signal*> outputSignals, string sFolderName, t_integer maxSamplesToProcessAtOnce) :
		SuperBlock(inputSignals, outputSignals),
		folderName{ sFolderName },
		maximumECSyndromeSize{ maxSamplesToProcessAtOnce },
		buffSize{ 512 }
	{
		setSignalsFolderName(sFolderName);
		setLogFileName("SuperBlock_AliceQKD.txt");
	};
	
	AliceQKD(initializer_list<Signal*> inputSignals, initializer_list<Signal*> outputSignals, string sFolderName, t_integer maxSamplesToProcessAtOnce, unsigned long int buffersSize) :
		SuperBlock(inputSignals, outputSignals),
		folderName{ sFolderName },
		maximumECSyndromeSize{ maxSamplesToProcessAtOnce },
		buffSize{ buffersSize }
	{
		setSignalsFolderName(sFolderName);
		setLogFileName("SuperBlock_AliceQKD.txt");
	};
	void initialize(void);
	bool runBlock(void);

	bool firstTime{ true };

	//##############################################################################################################

	void setErrorCorrectionPartitionSize(t_integer sz) { ErrorCorrection_.setPartitionSize(sz); }
	void setErrorCorrectionNumberOfPasses(t_integer np) { ErrorCorrection_.setNumberOfPasses(np); }
	void setDoublePartitionSize(bool db) { ErrorCorrection_.setDoublePartitionSize(db); }
	void setMaximumSyndromeSize(t_integer mss) { ErrorCorrection_.setMaximumSyndromeSize(mss); }
	void setMinimumSyndromeSize(t_integer minss) { ErrorCorrection_.setMinimumSyndromeSize(minss); }
	void setDoubleClickNumber(t_integer dcn) { BasisReconciliation_.setDoubleClickNumber(dcn); }
	void setNoClickNumber(t_integer ncn) { BasisReconciliation_.setNoClickNumber(ncn); }
	void setBer(t_real berValue) { PrivacyAmplification_.setBER(berValue); }
	void setSystematicSecurityParameter(t_integer ssp) { PrivacyAmplification_.setSystematicSecurityParameter(ssp); }
	void setBypassHash(bool bh) { PrivacyAmplification_.setBypassHashing(bh); }
	void setMinimumNumberOfPartitions(t_integer mnp) { ErrorCorrection_.setMinimumNumberOfPartitions(mnp); }
	void setParameterEstimationNumberBits(t_integer nb) { ParameterEstimation_.setNumberOfBitsToUse(nb); }
	void setParameterEstimationNumberBitsToStart(t_integer nbs) { ParameterEstimation_.setNumberOfBitsRequiredToStart(nbs); }

private:

	// #####################################################################################################
	// ################################## Input Parameters #################################################
	// #####################################################################################################

	unsigned long int buffSize;
	bool logValue{ true };
	t_integer maximumECSyndromeSize;
	long int keyBits{ 0 };
	long int lastKeyBits{ 0 };
	int dispBitsInterval{ 10000 };
	string folderName;
	long int sentQubits{ 0 };
	string reportFileName{ "alice_KeysReport.txt" };
	long int oldNumberOfSentQubits{ 0 };
	long int dispQubitsInterval{ 100000 };
	// #####################################################################################################
	// ################## Internal Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	// Input
	Binary Data_In{ "S01_Data_In.sgn", buffSize };
	Binary Basis_In{ "S02_Basis_In.sgn", buffSize };
	Message Messages_In{ "S03_Messages_In.sgn", buffSize };

	// Internal
	Binary DataIn{ "S04_DataIn.sgn", buffSize };
	Binary BasisIn{ "S05_BasisIn.sgn", buffSize };
	TimeContinuousAmplitudeDiscreteReal BasisFromBob{ "S06_BasisFromBob.sgn", buffSize };
	TimeContinuousAmplitudeDiscreteReal BasisToSend{ "S07_BasisToSend.sgn", buffSize };
	Binary SiftedKeys{ "S08_SiftedKeys.sgn", buffSize };
	Binary ErrorCorrectionIn{ "S08b_ErrorCorrectionIn.sgn", buffSize };
	Binary DataRecon{ "S09_DataRecon.sgn", buffSize };
	TimeContinuousAmplitudeDiscreteReal ParityFromBob{ "S10_ParityFromBob.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal ParityToSend{ "S11_ParityToSend.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal PermutationsFromBob{ "S12_PermutationsFromBob.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal SeedsToBob{ "S13_SeedsToBob.sgn", buffSize };
	TimeContinuousAmplitudeDiscreteReal BERFromBob{ "S13b_BERFromBob.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal BERToBob{ "S13c_BERToBob.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal BERToEC{ "S13d_BERToEC.sgn", buffSize };
	TimeContinuousAmplitudeDiscreteReal BERToPA{ "S13e_BERToPA.sgn", buffSize };
	TimeContinuousAmplitudeDiscreteReal UnusedSignal{ "UnusedSignal.sgn", buffSize };
	TimeContinuousAmplitudeDiscreteReal ToggleBERChange{ "ToggleBERChange.sgn", buffSize };
	Binary DataCorr{ "S14_DataCorr.sgn", buffSize };

	// Output
	Binary Data_Out{ "S15_Data_Out.sgn", buffSize };
	Binary Basis_Out{ "S16_Basis_Out.sgn", buffSize };
	Binary AliceTransmissionMode_Out{ "S17_AliceTransmissionMode_Out.sgn", buffSize };
	Binary Key_Out{ "S18_Key_Out.sgn", buffSize };
	Message Messages_Out{ "S19_Messages_Out.sgn", buffSize };

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	AliceIncomingDataProcessor AliceIncomingDataProcessor_{{&Data_In, &Basis_In }, { &DataIn, &BasisIn, &AliceTransmissionMode_Out, &Data_Out, &Basis_Out }};

	MessageProcessorReceiver MessageProcessorReceiver_{ {&Messages_In}, {&BasisFromBob, &ParityFromBob,&PermutationsFromBob, &UnusedSignal, &BERFromBob, &ToggleBERChange} };

	BasisReconciliation BasisReconciliation_{ {&DataIn, &BasisIn, &BasisFromBob}, {&SiftedKeys, &DataRecon, &BasisToSend,} };

//	ParameterEstimation ParameterEstimation_{ {&DataRecon, &BERFromBob}, {&ErrorCorrectionIn, &BERToBob, &BERToEC, &BERToPA} };
	ParameterEstimation ParameterEstimation_{ {&DataRecon, &BERFromBob}, {&ErrorCorrectionIn, &BERToBob, &BERToEC}, folderName };

	ErrorCorrection ErrorCorrection_{ {&ErrorCorrectionIn, &ParityFromBob, &PermutationsFromBob,&BERToEC, &ToggleBERChange}, {&DataCorr, &ParityToSend} };

	PrivacyAmplification PrivacyAmplification_{ {&DataCorr,&BERToPA}, {&Key_Out, &SeedsToBob} };

	MessageProcessorTransmitter MessageProcessorTransmitter_{ {&BasisToSend, &ParityToSend, &UnusedSignal, &SeedsToBob, &BERToBob}, {&Messages_Out} };



};

#endif

/*
#ifndef ALICE_QKD_H_
#define ALICE_QKD_H_

#include "../../../include/netxpto_20180815.h"

#include "./framer_20190326.h"
#include "./message_processor_receiver_20190410.h"
#include "./message_processor_transmitter_20190410.h"
#include "./basis_reconciliation_20190410.h"
#include "./error_correction_20190410.h"
#include "./privacy_amplification_20190410.h"
#include "./alice_incoming_data_processor_20190410.h"
#include "./parameter_estimation_20190528.h"


class AliceQKD : public SuperBlock {

public:

	// #####################################################################################################

	AliceQKD(initializer_list<Signal*> inputSignals, initializer_list<Signal*> outputSignals) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ 8192 }
	{
		setLogFileName("SuperBlock_AliceQKD.txt");
	};

	AliceQKD(initializer_list<Signal*> inputSignals, initializer_list<Signal*> outputSignals, string sFolderName) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ 8192 }
	{
		setSignalsFolderName(sFolderName);
		setLogFileName("SuperBlock_AliceQKD.txt");
	};

	AliceQKD(initializer_list<Signal*> inputSignals, initializer_list<Signal*> outputSignals, t_integer maxSamplesToProcessAtOnce) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ maxSamplesToProcessAtOnce }
	{
		setLogFileName("SuperBlock_AliceQKD.txt");
	};

	AliceQKD(initializer_list<Signal*> inputSignals, initializer_list<Signal*> outputSignals, string sFolderName, t_integer maxSamplesToProcessAtOnce) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ maxSamplesToProcessAtOnce }
	{
		setSignalsFolderName(sFolderName);
		setLogFileName("SuperBlock_AliceQKD.txt");
	};

	void initialize(void);
	bool runBlock(void);

	bool firstTime{ true };

	//##############################################################################################################

	void setErrorCorrectionPartitionSize(t_integer sz) { ErrorCorrection_.setPartitionSize(sz); }
	void setErrorCorrectionNumberOfPasses(t_integer np) { ErrorCorrection_.setNumberOfPasses(np); }
	void setDoublePartitionSize(bool db) { ErrorCorrection_.setDoublePartitionSize(db); }
	void setMaximumSyndromeSize(t_integer mss) { ErrorCorrection_.setMaximumSyndromeSize(mss); }
	void setMinimumSyndromeSize(t_integer minss) { ErrorCorrection_.setMinimumSyndromeSize(minss); }
	void setDoubleClickNumber(t_integer dcn) { BasisReconciliation_.setDoubleClickNumber(dcn); }
	void setNoClickNumber(t_integer ncn) { BasisReconciliation_.setNoClickNumber(ncn); }
	void setBer(t_integer berValue) { PrivacyAmplification_.setBER(berValue); }
	void setSystematicSecurityParameter(t_integer ssp) { PrivacyAmplification_.setSystematicSecurityParameter(ssp); }
	void setBypassHash(bool bh) { PrivacyAmplification_.setBypassHashing(bh); }
	void setMinimumNumberOfPartitions(t_integer mnp) { ErrorCorrection_.setMinimumNumberOfPartitions(mnp); }

private:

	// #####################################################################################################
	// ################################## Input Parameters #################################################
	// #####################################################################################################

	bool logValue{ true };
	t_integer maximumECSyndromeSize;
	vector< t_message_type> transmitterMsgTypes{ BasisReconciliationMsg, ErrorCorrectionMsg,PrivacyAmplificationSeedsMsg,ParameterEstimationSeedAndBitsMsg };

	// #####################################################################################################
	// ################## Internal Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	// Input
	Binary Data_In{ "S01_Data_In.sgn" };
	Binary Basis_In{ "S02_Basis_In.sgn" };
	Message Messages_In{ "S03_Messages_In.sgn" };

	// Internal
	Binary DataIn{ "S04_DataIn.sgn" };
	Binary BasisIn{ "S05_BasisIn.sgn" };
	TimeContinuousAmplitudeDiscreteReal BasisFromBob{ "S06_BasisFromBob.sgn" };
	TimeContinuousAmplitudeDiscreteReal BasisToSend{ "S07_BasisToSend.sgn" };
	Binary SiftedKeys{ "S08_SiftedKeys.sgn" };
	Binary ErrorCorrectionIn{ "S08b_ErrorCorrectionIn.sgn" };
	Binary DataRecon{ "S09_DataRecon.sgn" };
	TimeContinuousAmplitudeDiscreteReal ParityFromBob{ "S10_ParityFromBob.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal ParityToSend{ "S11_ParityToSend.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal PermutationsFromBob{ "S12_PermutationsFromBob.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal SeedsToBob{ "S13_SeedsToBob.sgn" };
	TimeContinuousAmplitudeDiscreteReal BERFromBob{ "S13b_BERFromBob.sgn" };
	TimeContinuousAmplitudeDiscreteReal BERToBob{ "S13c_BERToBob.sgn" };
	TimeContinuousAmplitudeDiscreteReal NotARealSignal{ "NotARealSignal.sgn" };
	Binary DataCorr{ "S14_DataCorr.sgn" };

	// Output
	Binary Data_Out{ "S15_Data_Out.sgn" };
	Binary Basis_Out{ "S16_Basis_Out.sgn" };
	Binary AliceTransmissionMode_Out{ "S17_AliceTransmissionMode_Out.sgn" };
	Binary Key_Out{ "S18_Key_Out.sgn" };
	Message Messages_Out{ "S19_Messages_Out.sgn" };

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	AliceIncomingDataProcessor AliceIncomingDataProcessor_{ {&Data_In, &Basis_In }, { &DataIn, &BasisIn, &AliceTransmissionMode_Out, &Data_Out, &Basis_Out } };

	MessageProcessorReceiver MessageProcessorReceiver_{ {&Messages_In}, {&BasisFromBob, &ParityFromBob,&PermutationsFromBob, &NotARealSignal, &BERFromBob} };

	BasisReconciliation BasisReconciliation_{ {&DataIn, &BasisIn, &BasisFromBob}, {&DataRecon, &SiftedKeys, &BasisToSend,} };
	//	ErrorCorrection ErrorCorrection_{ {&DataRecon, &ParityFromBob}, {&DataCorr, &ParityToSend} };

	//	ParameterEstimation ParameterEstimation_{ {&DataRecon, &BERFromBob}, {&ErrorCorrectionIn, &BERToBob} };
	//	BasisReconciliation BasisReconciliation_{ {&DataIn, &BasisIn, &BasisFromBob}, {&Key_Out, &BasisToSend} };

	ErrorCorrection ErrorCorrection_{ {&DataRecon, &ParityFromBob, &PermutationsFromBob}, {&DataCorr, &ParityToSend} };

	PrivacyAmplification PrivacyAmplification_{ {&DataCorr}, {&Key_Out, &SeedsToBob} };

	MessageProcessorTransmitter MessageProcessorTransmitter_{ {&BasisToSend, &ParityToSend, &SeedsToBob, &BERToBob}, {&Messages_Out} };

};

#endif
*/

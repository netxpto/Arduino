#ifndef BOB_QKD_H_
#define BOB_QKD_H_

#include "../../../include/netxpto_20180815.h"

//#include "../include/framer_20190326.h"
#include "./message_processor_receiver_20190410.h"
#include "./message_processor_transmitter_20190410.h"
#include "./basis_reconciliation_20190410.h"
#include "./error_correction_20190410.h"
#include "./privacy_amplification_20190410.h"
#include "./bob_incoming_data_processor_20190410.h"
#include "./transmission_control_20190410.h"
#include "./parameter_estimation_20190528.h"


class BobQKD : public SuperBlock {


public:
	
	BobQKD(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals) :
		SuperBlock(inputSignals, outputSignals),
		folderName{ getSignalsFolderName() },
		maximumECSyndromeSize{ 8192 }
		{
			setLogFileName("SuperBlock_BobQKD.txt");
		};

	BobQKD(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals, string sFolderName) :
		SuperBlock(inputSignals, outputSignals),
		folderName{ sFolderName },
		maximumECSyndromeSize{ 8192 }
		{
			setSignalsFolderName(sFolderName);
			setLogFileName("SuperBlock_BobQKD.txt");
		};

	BobQKD(initializer_list<Signal*> inputSignals, initializer_list<Signal*> outputSignals, t_integer maxSamplesToProcessAtOnce) :
		SuperBlock(inputSignals, outputSignals),
		folderName{ getSignalsFolderName() },
		maximumECSyndromeSize{ maxSamplesToProcessAtOnce }
	{
		setLogFileName("SuperBlock_BobQKD.txt");
	};

	BobQKD(initializer_list<Signal*> inputSignals, initializer_list<Signal*> outputSignals, string sFolderName, t_integer maxSamplesToProcessAtOnce) :
		SuperBlock(inputSignals, outputSignals),
		folderName{ sFolderName },
		maximumECSyndromeSize{ maxSamplesToProcessAtOnce}
	{
		setSignalsFolderName(sFolderName);
		setLogFileName("SuperBlock_BobQKD.txt");
	};



	void initialize(void);
	bool runBlock(void);

	bool firstTime{ true };

	// #####################################################################################################
	// ########################################## Public Methods ###########################################
	// #####################################################################################################
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

	string reportFileName{ "bob_KeysReport.txt" };
	bool logValue{ true };
	t_integer maximumECSyndromeSize;
	long int keyBits{ 0 };
	long int lastKeyBits{ 0 };
	int dispBitsInterval{ 100000 };
	string folderName;
	long int receivedQubits{ 0 };

	// #####################################################################################################
	// ################## Internal Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	TimeContinuousAmplitudeDiscreteReal BER_In{ "S01_BER_In.sgn" };
	TimeContinuousAmplitudeDiscreteReal Data_In{ "S02_Data_In.sgn" };
	Binary Basis_In{ "S03_Basis_In.sgn" };
	Binary TransmissionMode_In{ "S04_TransmissionMode_In.sgn" };
	Message Messages_In{ "S05_Messages_In.sgn" };

	// Internal
	TimeContinuousAmplitudeDiscreteReal DataIn{ "S07_DataIn.sgn" };
	Binary BasisIn{ "S08_DataIn.sgn" };
	TimeContinuousAmplitudeDiscreteReal BasisFromAlice{ "S09_BasisFromAlice.sgn" };
	Binary SiftedKeys{ "S10_DataRecon.sgn" };
	Binary DataRecon{ "S11_DataRecon.sgn" };
	Binary ErrorCorrectionIn{ "S11b_ErrorCorrectionIn.sgn" };
	TimeContinuousAmplitudeDiscreteReal BasisToSend{ "S12_BasisToSend.sgn" };
	TimeContinuousAmplitudeDiscreteReal ParityFromAlice{ "S13_ParityFromAlice.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal ParityToSend{ "S14_ParityToSend.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal PermutationsToSend{ "S15_PermutationsToSend.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal SeedsFromAlice{ "S16_SeedsFromAlice.sgn" };
	TimeContinuousAmplitudeDiscreteReal BERFromAlice{ "S16b_BERFromAlice.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal BERToAlice{ "S16c_BERToAlice.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal BERToEC{ "S16d_BERToEC.sgn" };
	TimeContinuousAmplitudeDiscreteReal BERToPA{ "S16e_BERToPA.sgn" };
	TimeContinuousAmplitudeDiscreteReal UnusedSignal{ "UnusedSignal.sgn" };
	TimeContinuousAmplitudeDiscreteReal ToggleBERChange{ "ToggleBERChange.sgn" };
	Binary DataCorr{ "S17_DataCorr.sgn" };

	// Output
	Binary Key_Out{ "S18_Key_Out.sgn" };
	Message Messages_Out{ "S19_Messages_Out.sgn" };
	Binary Basis_Out{ "S20_Basis_Out.sgn" };
	//Binary TransmissionMode_Out{"S16_TransmissionMode_Out.sgn"};

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	BobIncomingDataProcessor BobIncomingDataProcessor_{ {&TransmissionMode_In, &Data_In, &Basis_In }, { &DataIn, &BasisIn} };

	MessageProcessorReceiver MessageProcessorReceiver_{ {&Messages_In}, {&BasisFromAlice, &ParityFromAlice, &UnusedSignal, &SeedsFromAlice, &BERFromAlice} };

	BasisReconciliation BasisReconciliation_{ {&DataIn, &BasisIn, &BasisFromAlice}, {&DataRecon, &SiftedKeys,  &BasisToSend} };

//	ParameterEstimation ParameterEstimation_{ {&DataRecon, &BERFromAlice}, {&ErrorCorrectionIn, &BERToAlice, &BERToEC, &BERToPA} };
	ParameterEstimation ParameterEstimation_{ {&DataRecon, &BERFromAlice}, {&ErrorCorrectionIn, &BERToAlice, &BERToEC}, folderName };

	ErrorCorrection ErrorCorrection_{ {&ErrorCorrectionIn, &ParityFromAlice, &BERToEC}, {&DataCorr, &ParityToSend, &PermutationsToSend, &ToggleBERChange} };

	PrivacyAmplification PrivacyAmplification_{ {&DataCorr, &SeedsFromAlice,&BERToPA}, {&Key_Out} };

	MessageProcessorTransmitter MessageProcessorTransmitter_{ {&BasisToSend, &ParityToSend, &PermutationsToSend, &UnusedSignal, &BERToAlice, &ToggleBERChange}, {&Messages_Out} };


};

#endif

/*
#ifndef BOB_QKD_H_
#define BOB_QKD_H_

#include "../../../include/netxpto_20180815.h"

//#include "../include/framer_20190326.h"
#include "./message_processor_receiver_20190410.h"
#include "./message_processor_transmitter_20190410.h"
#include "./basis_reconciliation_20190410.h"
#include "./error_correction_20190410.h"
#include "./privacy_amplification_20190410.h"
#include "./bob_incoming_data_processor_20190410.h"
#include "./transmission_control_20190410.h"
#include "./parameter_estimation_20190528.h"


class BobQKD : public SuperBlock {


public:

	BobQKD(initializer_list<Signal*> inputSignals, initializer_list<Signal*> outputSignals) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ 8192 }
	{
		setLogFileName("SuperBlock_BobQKD.txt");
	};

	BobQKD(initializer_list<Signal*> inputSignals, initializer_list<Signal*> outputSignals, string sFolderName) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ 8192 }
	{
		setSignalsFolderName(sFolderName);
		setLogFileName("SuperBlock_BobQKD.txt");
	};

	BobQKD(initializer_list<Signal*> inputSignals, initializer_list<Signal*> outputSignals, t_integer maxSamplesToProcessAtOnce) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ maxSamplesToProcessAtOnce }
	{
		setLogFileName("SuperBlock_BobQKD.txt");
	};

	BobQKD(initializer_list<Signal*> inputSignals, initializer_list<Signal*> outputSignals, string sFolderName, t_integer maxSamplesToProcessAtOnce) :
		SuperBlock(inputSignals, outputSignals),
		maximumECSyndromeSize{ maxSamplesToProcessAtOnce }
	{
		setSignalsFolderName(sFolderName);
		setLogFileName("SuperBlock_BobQKD.txt");
	};



	void initialize(void);
	bool runBlock(void);

	bool firstTime{ true };

	// #####################################################################################################
	// ########################################## Public Methods ###########################################
	// #####################################################################################################
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

	vector< t_message_type> transmitterMsgTypes{ BasisReconciliationMsg, ErrorCorrectionMsg,ErrorCorrectionPermutationsMsg,ParameterEstimationSeedAndBitsMsg };

	// #####################################################################################################
	// ################## Internal Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	TimeContinuousAmplitudeDiscreteReal BER_In{ "S01_BER_In.sgn" };
	TimeContinuousAmplitudeDiscreteReal Data_In{ "S02_Data_In.sgn" };
	Binary Basis_In{ "S03_Basis_In.sgn" };
	Binary TransmissionMode_In{ "S04_TransmissionMode_In.sgn" };
	Message Messages_In{ "S05_Messages_In.sgn" };

	// Internal
	TimeContinuousAmplitudeDiscreteReal DataIn{ "S07_DataIn.sgn" };
	Binary BasisIn{ "S08_DataIn.sgn" };
	TimeContinuousAmplitudeDiscreteReal BasisFromAlice{ "S09_BasisFromAlice.sgn" };
	Binary SiftedKeys{ "S10_DataRecon.sgn" };
	Binary DataRecon{ "S11_DataRecon.sgn" };
	Binary ErrorCorrectionIn{ "S11b_ErrorCorrectionIn.sgn" };
	TimeContinuousAmplitudeDiscreteReal BasisToSend{ "S12_BasisToSend.sgn" };
	TimeContinuousAmplitudeDiscreteReal ParityFromAlice{ "S13_ParityFromAlice.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal ParityToSend{ "S14_ParityToSend.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal PermutationsToSend{ "S15_PermutationsToSend.sgn",maximumECSyndromeSize };
	TimeContinuousAmplitudeDiscreteReal SeedsFromAlice{ "S16_SeedsFromAlice.sgn" };
	TimeContinuousAmplitudeDiscreteReal BERFromAlice{ "S16b_BERFromAlice.sgn" };
	TimeContinuousAmplitudeDiscreteReal BERToAlice{ "S16c_BERToAlice.sgn" };
	TimeContinuousAmplitudeDiscreteReal NotARealSignal{ "NotARealSignal.sgn" };
	Binary DataCorr{ "S17_DataCorr.sgn" };

	// Output
	Binary Key_Out{ "S18_Key_Out.sgn" };
	Message Messages_Out{ "S19_Messages_Out.sgn" };
	Binary Basis_Out{ "S20_Basis_Out.sgn" };
	//Binary TransmissionMode_Out{"S16_TransmissionMode_Out.sgn"};

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	BobIncomingDataProcessor BobIncomingDataProcessor_{ {&TransmissionMode_In, &Data_In, &Basis_In }, { &DataIn, &BasisIn} };

	MessageProcessorReceiver MessageProcessorReceiver_{ {&Messages_In}, {&BasisFromAlice, &ParityFromAlice, &NotARealSignal,&SeedsFromAlice, &BERFromAlice} };

	BasisReconciliation BasisReconciliation_{ {&DataIn, &BasisIn, &BasisFromAlice}, {&DataRecon, &SiftedKeys, &BasisToSend} };

	//ParameterEstimation ParameterEstimation_{ {&DataRecon, &BERFromAlice}, {&ErrorCorrectionIn, &BERToAlice} };

	ErrorCorrection ErrorCorrection_{ {&DataRecon, &ParityFromAlice}, {&DataCorr, &ParityToSend, &PermutationsToSend} };

	//	BasisReconciliation BasisReconciliation_{ {&DataIn, &BasisIn, &BasisFromAlice}, {&Key_Out, &BasisToSend} };

	//	ErrorCorrection ErrorCorrection_{ {&DataRecon, &ParityFromAlice}, {&DataCorr, &ParityFromAlice} };

	PrivacyAmplification PrivacyAmplification_{ {&DataCorr, &SeedsFromAlice}, {&Key_Out} };

	MessageProcessorTransmitter MessageProcessorTransmitter_{ {&BasisToSend, &ParityToSend, &PermutationsToSend, &BERToAlice}, {&Messages_Out} };








};

#endif
*/

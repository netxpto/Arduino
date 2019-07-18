#include "../../include/netxpto_20180815.h"
#include "../../include/sink_20180815.h"

#include "include/binary_source_20180815.h"
#include "include/bit_error_rate_20180815.h"
#include "include/basis_reconciliation_20190410.h"
#include "include/ms_windows_ip_tunnel_20180815.h"
#include "include/message_processor_common_20190410.h"
#include "include/message_processor_receiver_20190410.h"
#include "include/message_processor_transmitter_20190410.h"


int numBits{ 10000 };

int main()
{

	// #####################################################################################################
	// ########################### Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	Binary sBinaryData1{ "sBinaryData1.sgn" };
	Binary sBinaryData2{ "sBinaryData2.sgn" };
	Message sMessageIn{ "sMessageIn.sgn" };

	Binary sKey1{ "Key1.sgn" };
	Binary sKey2{ "Key2.sgn" };

	TimeContinuousAmplitudeDiscreteReal sToMessageProc1{ "sToMessage1.sgn" };
	TimeContinuousAmplitudeDiscreteReal sToMessageProc2{ "sToMessage2.sgn" };
	TimeContinuousAmplitudeDiscreteReal sFromMessageProc1{ "sFromMessageProc1.sgn" };
	TimeContinuousAmplitudeDiscreteReal sFromMessageProc2{ "sFromMessageProc2.sgn" };

	TimeContinuousAmplitudeDiscreteReal sSignalToIgnore{ "sSignalToIgnore.sgn" };

	Message sMessages1{ "sMessages1.sgn" };
	Message sMessages2{ "sMessages2.sgn" };

	Binary sBinaryData3{ "sBinaryData3.sgn" };
	Binary sBinaryData4{ "sBinaryData4.sgn" };

	Binary sKey3{ "Key3.sgn" };
	Binary sKey4{ "Key4.sgn" };


	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	////////////////////////////////////		START	Transmitter		  //////////////////////////////////
	BinarySource BinarySource_1_{ {},{&sBinaryData1}, 1 };
	BinarySource_1_.setNumberOfBits(numBits);
	BinarySource BinarySource_2_{ {},{&sBinaryData2}, 2 };
	BinarySource_2_.setNumberOfBits(numBits);

	BasisReconciliation BasisReconciliation_1_{ { &sBinaryData1, &sBinaryData2, &sFromMessageProc1}, {&sKey1, &sKey2, &sToMessageProc1} };
	BasisReconciliation_1_.setBasisReconciliationRole(basis_reconciliation_role::Alice);

	Sink Sink_1_{ {&sKey1},{} };
	Sink Sink_2_{ {&sKey2},{} };

	MessageProcessorTransmitter MessageProcessorTransmitter_1_{ {&sToMessageProc1, &sSignalToIgnore, &sSignalToIgnore, &sSignalToIgnore, &sSignalToIgnore},{&sMessages1} };
	MessageProcessorReceiver MessageProcessorReceiver_1_{ {&sMessages2},{&sFromMessageProc1, &sSignalToIgnore, &sSignalToIgnore, &sSignalToIgnore, &sSignalToIgnore} };

	////////////////////////////////////		END		Transmitter		  //////////////////////////////////

	////////////////////////////////////		START	Receiver		  //////////////////////////////////

	BinarySource BinarySource_3_{ {},{&sBinaryData3}, 1 };
	BinarySource_3_.setNumberOfBits(numBits);
	BinarySource BinarySource_4_{ {},{&sBinaryData4}, 3 };
	BinarySource_4_.setNumberOfBits(numBits);

	BasisReconciliation BasisReconciliation_2_{ {&sBinaryData3, &sBinaryData4, &sFromMessageProc2}, {&sKey3, &sKey4,  &sToMessageProc2} };
	BasisReconciliation_2_.setBasisReconciliationRole(basis_reconciliation_role::Bob);

	Sink Sink_3_{ {&sKey3},{} };
	Sink Sink_4_{ {&sKey4},{} };

	MessageProcessorTransmitter MessageProcessorTransmitter_2_{ {&sToMessageProc2, &sSignalToIgnore, &sSignalToIgnore, &sSignalToIgnore, &sSignalToIgnore},{&sMessages2} };
	MessageProcessorReceiver MessageProcessorReceiver_2_{ {&sMessages1},{&sFromMessageProc2, &sSignalToIgnore, &sSignalToIgnore, &sSignalToIgnore, &sSignalToIgnore} };

	////////////////////////////////////		END		Receiver		  //////////////////////////////////

	// #####################################################################################################
	// ########################### System Declaration and Inicialization ###################################
	// #####################################################################################################

	System MainSystem{
		{
			&BinarySource_1_,
			&Sink_2_,
			&Sink_3_,
			&Sink_4_,
			&MessageProcessorReceiver_1_,
			&BasisReconciliation_1_,
			&MessageProcessorTransmitter_1_,
			&MessageProcessorReceiver_2_,
			&BasisReconciliation_2_,
			&MessageProcessorTransmitter_2_,
			&Sink_1_,

			&BinarySource_2_,
			&BinarySource_3_,
			&BinarySource_4_
		}
	};

	MainSystem.run();
	MainSystem.terminate();

	return 0;
}

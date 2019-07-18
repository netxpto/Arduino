#include "../../include/netxpto_20180815.h"
#include "../../include/sink_20180815.h"

#include "include/binary_source_20180815.h"
#include "include/bit_error_rate_20180815.h"
#include "include/basis_reconciliation_20190410.h"
#include "include/ip_tunnel_ms_windows_20180815.h"
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

	Binary sKey1{ "Key1.sgn" };
	Binary sKey2{ "Key2.sgn" };

	TimeContinuousAmplitudeDiscreteReal sToMessageProc1{ "sToMessage1.sgn" };
	TimeContinuousAmplitudeDiscreteReal sFromMessageProc1{ "sFromMessageProc1.sgn" };

	TimeContinuousAmplitudeDiscreteReal sSignalToIgnore{ "sSignalToIgnore.sgn" };

	Message sMessages1{ "sMessages1.sgn" };
	Message sMessages2{ "sMessages2.sgn" };


	Binary IPTunnelSourceOut_1{ "IPTunnelSourceOut_1.sgn" };

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

	IPTunnel IPTunnel_In_{ {},{&sMessages2} };
	IPTunnel_In_.setTcpPort(54001);

	//IPTunnel IPTunnel_Out_{ {&sMessages1},{&IPTunnelSourceOut_1} };
	IPTunnel IPTunnel_Out_{ {&sMessages1},{} };
	IPTunnel_Out_.setRemoteMachineIpAddress("192.168.1.9");
	IPTunnel_Out_.setTcpPort(54000);

	// #####################################################################################################
	// ########################### System Declaration and Inicialization ###################################
	// #####################################################################################################

	System MainSystem{
		{
			&BinarySource_1_,
			&BinarySource_2_,
			&IPTunnel_In_,
			&MessageProcessorReceiver_1_,
			&BasisReconciliation_1_,
			&Sink_1_,
			&Sink_2_,
			
			&MessageProcessorTransmitter_1_,
			&IPTunnel_Out_
			
			
		}
	};

	MainSystem.run();
	MainSystem.terminate();

	return 0;
}

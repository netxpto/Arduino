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


	TimeContinuousAmplitudeDiscreteReal sToMessageProc2{ "sToMessage2.sgn" };
	TimeContinuousAmplitudeDiscreteReal sFromMessageProc2{ "sFromMessageProc2.sgn" };

	TimeContinuousAmplitudeDiscreteReal sSignalToIgnore{ "sSignalToIgnore.sgn" };

	Message sMessages1{ "sMessages1.sgn" };
	Message sMessages2{ "sMessages2.sgn" };

	Binary sBinaryData3{ "sBinaryData3.sgn" };
	Binary sBinaryData4{ "sBinaryData4.sgn" };

	Binary sKey3{ "Key3.sgn" };
	Binary sKey4{ "Key4.sgn" };

	Binary IPTunnelSourceOut_1{ "IPTunnelSourceOut_1.sgn" };

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

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

	IPTunnel IPTunnel_In_{ {},{&sMessages1} };
	IPTunnel_In_.setTcpPort(54000);

	IPTunnel IPTunnel_Out_{ {&sMessages2},{} };
	IPTunnel_Out_.setTcpPort(54001);

	// #####################################################################################################
	// ########################### System Declaration and Inicialization ###################################
	// #####################################################################################################

	System MainSystem{
		{
			&BinarySource_3_,
			&BinarySource_4_,
			&BasisReconciliation_2_,
			&Sink_3_,
			&Sink_4_,
			&MessageProcessorTransmitter_2_,
			&IPTunnel_Out_,
			&IPTunnel_In_,
			&MessageProcessorReceiver_2_		
		}
	};

	MainSystem.run();
	MainSystem.terminate();

	return 0;
}

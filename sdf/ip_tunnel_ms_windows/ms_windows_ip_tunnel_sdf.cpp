# include "include/netxpto_20180815.h"
# include "include/ip_tunnel_ms_windows_20180815.h"
# include "include/binary_source_20180815.h"
# include "include/sink_20180815.h"


// #####################################################################################################
// ################################### System Input Parameters #########################################
// #####################################################################################################

int numberOfBits{ 1000 };	

int main() {

	// #####################################################################################################
	// ########################### Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	Binary BinarySourceOut_0{ "S0_BinarySourceOut_0.sgn" };

	Binary BinarySourceOut_1{ "S1_BinarySourceOut_0.sgn" };

	Binary BinarySourceOut_2{ "S2_BinarySourceOut_0.sgn" };
	
	Binary IPTunnelSourceOut_3{ "S2_BinarySourceOut_0.sgn" };

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################


	BinarySource BinarySource_{ {}, { &BinarySourceOut_0, &BinarySourceOut_1, &BinarySourceOut_2 } };
	BinarySource_.setBitPeriod(1e-6);
	BinarySource_.setNumberOfBits(numberOfBits);
	
	IPTunnel IpTunnel_{ {&BinarySourceOut_1},{&IPTunnelSourceOut_3} };
	
	Sink Sink_0{ {&IPTunnelSourceOut_3},{} };

	Sink Sink_1{ {&BinarySourceOut_0},{} };

	Sink Sink_2{ {&BinarySourceOut_2},{} };

	// #####################################################################################################
	// ########################### System Declaration and Inicialization ###################################
	// #####################################################################################################

	System MainSystem{ 
		{ 
			&BinarySource_,
			&IpTunnel_,
			&Sink_1,
			&Sink_2

		}
	};

	MainSystem.setLogValue(true);
	MainSystem.setSaveSignals(true);

	// #####################################################################################################
	// #################################### System Run #####################################################
	// #####################################################################################################

	MainSystem.run();
	MainSystem.terminate();

	system("pause");
	return 0;

}
# include "include/netxpto_20180815.h"
# include "include/ip_tunnel_ms_windows_20180815.h"
# include "include/binary_source_20180815.h"
# include "include/sink_20180815.h"
# include "..\arduino_quantum_rx\arduino_quantum_rx_netxpto\include\qber_20190418.h"
# include "include/load_ascii_20190613.h"

// #####################################################################################################
// ################################### System Input Parameters #########################################
// #####################################################################################################

int numberOfBits{ 1601 };	

int main() {

	string binSeq;

	ifstream inFile;
	inFile.open("AliceBinary.csv");
	getline(inFile, binSeq);
	inFile.close();

	// #####################################################################################################
	// ########################### Signals Declaration and Inicialization ##################################
	// #####################################################################################################
	
	Binary AliceIn{ "S0_AliceIn.sgn", 8000 };

	Binary IPTunnelSourceOut{ "S1_BinarySourceOut.sgn", 8000 };

	Binary qberOut{ "S2_QberOut.sgn", 8000 };

	
	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	BinarySource load_ascii{ {},{&AliceIn} };
	load_ascii.setBitPeriod(1.0 / 8.0e3);
	load_ascii.setMode(BinarySourceMode::DeterministicCyclic);
	load_ascii.setNumberOfBits(500000);
	load_ascii.setBitStream("01");

	IPTunnel ip_tunnel{ {},{&IPTunnelSourceOut} };
	
	Qber qber_{ {&AliceIn, &IPTunnelSourceOut},{&qberOut} };
	qber_.setWindow(8000);

	Sink Sink_0{ {&qberOut},{} };

	// #####################################################################################################
	// ########################### System Declaration and Inicialization ###################################
	// #####################################################################################################

	System MainSystem{ 
		{
			&ip_tunnel,
			&load_ascii,
			&qber_,
			&Sink_0
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
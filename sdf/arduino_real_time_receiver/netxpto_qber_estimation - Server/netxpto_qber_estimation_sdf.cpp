# include "include/netxpto_20180815.h"
# include "include/ip_tunnel_ms_windows_20180815.h"
# include "include/binary_source_20180815.h"
# include "include/sink_20180815.h"
# include "include/qber_20191219.h"

// #####################################################################################################
// ################################### System Input Parameters #########################################
// #####################################################################################################

int numberOfBits{ 1601 };	

int main() {

	string binSeq;

	ifstream inFile;
	inFile.open("binary.txt");
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
	load_ascii.setNumberOfBits(500000000000);
	load_ascii.setBitStream(binSeq);

	IPTunnel ip_tunnel{ {},{&IPTunnelSourceOut} };
	
	Qber qber_{ {&AliceIn, &IPTunnelSourceOut},{&qberOut} };
	qber_.setBerLimit(10);
	qber_.setBufferSize(50);
	qber_.setSyncMode(sync_mode::On_BobAhead);


	Sink Sink_0{ {&qberOut},{} };
	Sink_0.setDisplayNumberOfSamples(false);

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
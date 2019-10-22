# include "..\arduino_quantum_rx_netxpto\include\binary_source_20180815.h"
# include "..\arduino_quantum_rx_netxpto\include\coincidence_detector_20190319.h"
# include "..\arduino_quantum_rx_netxpto\include\netxpto_20180815.h"
# include "..\arduino_quantum_rx_netxpto\include\sink_20180815.h"
# include "..\arduino_quantum_rx_netxpto\include\clock_20171219.h"
# include "..\..\ip_tunnel_ms_windows\include\ip_tunnel_ms_windows_20180815.h"

// #####################################################################################################
// ################################### System Input Parameters #########################################
// #####################################################################################################

int numberOfBits{ 10000 };

string s = "0";
string s1 = "1";


int main()
{
	/* Signals Declaration */
	Binary SPD0_out{ "SPD0_out.sgn"};
	SPD0_out.setSaveInAscii(true);

	Binary SPD1_out{ "SPD1_out.sgn"};
	SPD1_out.setSaveInAscii(true);

	Binary BobData_In{ "BobData_In.sgn"};
	BobData_In.setSaveInAscii(true);

	TimeDiscreteAmplitudeDiscreteReal clock_out{ "clock_out.sgn"};
	clock_out.setSaveInAscii(true);

	Binary IPTunnel_Out{ "IPTunnel_Out.sgn" };

	/* Blocks Decalration */
	BinarySource BinarySource0_{ {},{ &SPD0_out} };
	BinarySource0_.setBitStream(s);
	//BinarySource0_.setNumberOfBits(numberOfBits);
	//BinarySource0_.setMode(BinarySourceMode::DeterministicCyclic);
	BinarySource0_.setBitPeriod(1e-6);


	BinarySource BinarySource1_{ {},{ &SPD1_out} };
	BinarySource1_.setBitStream(s1);
	//BinarySource1_.setNumberOfBits(numberOfBits);
	//BinarySource1_.setMode(BinarySourceMode::DeterministicCyclic);
	BinarySource1_.setBitPeriod(1e-6);

	Clock Clock{ {},{&clock_out} };
	Clock.setClockPeriod(1); // If the period is defined in seconds it gives us a clock frequency of 1 KHz
	Clock.setSamplingPeriod(0.1);

	CoincidenceDetector CoincidenceDetector_{ {&SPD0_out, &SPD1_out, &clock_out},{&BobData_In} };
	
	IPTunnel IpTunnel_{ {&BobData_In},{&IPTunnel_Out} };

	Sink Sink_IPTunnel_{ { &IPTunnel_Out },{} };
	Sink_IPTunnel_.setDisplayNumberOfSamples(true);



	System MainSystem{
		// BLOCKS
		&Clock,
		&BinarySource0_,
		&BinarySource1_,
		&CoincidenceDetector_,
		&IpTunnel_,
		&Sink_IPTunnel_


	};

	MainSystem.setLogValue(true);
	MainSystem.setSaveSignals(true);

	MainSystem.run();
	MainSystem.terminate();
	

	system("pause");

	return 0;
}

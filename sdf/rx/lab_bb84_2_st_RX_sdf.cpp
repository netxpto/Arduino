

#include <chrono>  // for high_resolution_clock

//#include <stdio.h>
//#include <iostream>

#include "../../include/netxpto_20180815.h"
#include "../../include/sink_20180815.h"

#include "include/binary_source_20180815.h"
#include "include/bit_error_rate_20180815.h"
#include "include/alice_qkd_20190410.h"
#include "include/bob_qkd_20190410.h"
#include "include/ms_windows_ip_tunnel_20180815.h"

// Input Parameters (Mostly to be left alone)
t_integer doubleClickNumber{ 2 };
t_integer noClickNumber{ 3 };
t_integer minimumNumberOfPartitions{ 1 };

int numberOfBits{ 1'000'000 };
t_integer systematicSecurityParameter{ 1 };						// Number of bits to be systematically discarded in Privacy amplification
bool bypassHash{ true };
t_real probabilityOfTransmissionError{ 0.01 };						// Expected transmission error
t_real qValue{ 0.73 };											// Lowering qValue descreases residual error, but also decreases efficiency. Default value is 0.73
unsigned long int buffSize{ 4096 };

t_integer numPasses{ 4 };
bool doublePartitionSize{ true };
double bitRate{ 1e3 };
t_integer maximumSyndromeSize{ 100000 };
t_integer minimumSyndromeSize{ 100000 };
t_integer parameterEstimationNumberBits(1000);
t_integer parameterEstimationNumberBitsToStart(101000);
//bool doParameterEstimation{ false };
//string ipTunnelIpAddress{ "127.0.0.1" };
//string ipTunnelIpAddress{ "192.168.81.234" };
//
//string ipTunnelIpAddress{ "192.168.92.197" };

string ipTunnelIpAddress{ "192.168.1.7" }; // sala bolseiros
//string ipTunnelIpAddress{ "192.168.81.172" }; // lab HP
//string ipTunnelIpAddress{ "192.168.81.234" }; // lab Fuji

int main()
{

	auto start = std::chrono::high_resolution_clock::now();

	// #####################################################################################################
	// ################################ Variables Inicialization ###########################################
	// #####################################################################################################

	t_integer cascadeWindowSize = (t_integer)(floor(abs(qValue / (max(probabilityOfTransmissionError, 0.001)))));

	// #####################################################################################################
	// ########################### Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	Binary AliceTransmissionMode_Out{ "AliceTransmissionMode_Out.sgn", buffSize }; AliceTransmissionMode_Out.setSaveSignal(false); // 0 ou 1, quando é zero a Alice para a transmissão de bits
	TimeContinuousAmplitudeDiscreteReal AliceData_Out{ "AliceData_Out.sgn", buffSize }; AliceData_Out.setSaveSignal(false);
	Message Messages_AliceToBob{ "Messages_AliceToBob.sgn", buffSize }; Messages_AliceToBob.setSaveSignal(false); //Messages_AliceToBob.setSaveSignal(false);

	Binary IPTunnelSourceOut_1{ "IPTunnelSourceOut_1.sgn", buffSize };
	Binary IPTunnelSourceOut_2{ "IPTunnelSourceOut_2.sgn", buffSize };
	Binary IPTunnelSourceOut_3{ "IPTunnelSourceOut_3.sgn", buffSize };
	Binary IPTunnelSourceOut_4{ "IPTunnelSourceOut_4.sgn", buffSize };
	Binary IPTunnelSourceOut_5{ "IPTunnelSourceOut_4.sgn", buffSize };

	Binary BobBasis_In{ "BobBasis_In.sgn", buffSize }; BobBasis_In.setSaveSignal(false);
	TimeContinuousAmplitudeDiscreteReal BobData_In{ "BobData_In.sgn", buffSize }; BobData_In.setSaveSignal(false); // Deveria ser amplitude discrete
	TimeContinuousAmplitudeContinuousReal QBER_ChannelUpperBound{ "QBER_ChannelUpperBound.sgn", buffSize }; QBER_ChannelUpperBound.setSaveSignal(false);

	Binary BobTransmissionMode_In{ "BobTransmissionMode_In.sgn", buffSize }; BobTransmissionMode_In.setSaveSignal(false);

	Binary BobBasis_Out{ "BobBasis_Out.sgn", buffSize }; BobBasis_Out.setSaveSignal(false);
	Binary BobSiftedKeys{ "BobSiftedKeys.sgn", buffSize }; BobSiftedKeys.setSaveSignal(false);
	Binary BobKeys{ "BobKeys.sgn", buffSize }; BobKeys.setSaveSignal(true);
	Message Messages_BobToAlice{ "Messages_BobToAlice.sgn", buffSize }; Messages_BobToAlice.setSaveSignal(false);


	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################


	BinarySource BobBasisBinarySource_{ {},{ &BobBasis_In } };
	BobBasisBinarySource_.setMode(BinarySourceMode::Random);
	//	BobBasisBinarySource_.setSeed(53128);
	BobBasisBinarySource_.setNumberOfBits(numberOfBits);

	IPTunnel IpTunnel_5_{ {&BobBasis_Out},{&IPTunnelSourceOut_5} };
	IpTunnel_5_.setTcpPort(54004);
	IpTunnel_5_.setDisplayNumberOfSamples(false);
	IpTunnel_5_.setRemoteMachineIpAddress(ipTunnelIpAddress);

	IPTunnel IpTunnel_1_{ {}, {&AliceData_Out} };
	IpTunnel_1_.setTcpPort(54000);
	IpTunnel_1_.setDisplayNumberOfSamples(false);
	IpTunnel_1_.setRemoteMachineIpAddress(ipTunnelIpAddress);

	IPTunnel IpTunnel_2_{ {},{&AliceTransmissionMode_Out} };
	IpTunnel_2_.setTcpPort(54001);
	IpTunnel_2_.setDisplayNumberOfSamples(false);
	IpTunnel_2_.setRemoteMachineIpAddress(ipTunnelIpAddress);

	IPTunnel IpTunnel_3_{ {} ,{&Messages_AliceToBob} };
	IpTunnel_3_.setTcpPort(54002);
	IpTunnel_3_.setDisplayNumberOfSamples(false);
	IpTunnel_3_.setRemoteMachineIpAddress(ipTunnelIpAddress);

	// QBER_ChannelUpperBound -> Not needed? Not provided?



	BobQKD BobQKD_{ { &AliceData_Out,  &BobBasis_In, &AliceTransmissionMode_Out, &Messages_AliceToBob, &QBER_ChannelUpperBound },{ &BobBasis_Out, &BobKeys, &Messages_BobToAlice }, "signals/SuperBlock_BobQKD", 8192, buffSize };
	BobQKD_.setErrorCorrectionPartitionSize(cascadeWindowSize);
	BobQKD_.setErrorCorrectionNumberOfPasses(numPasses);
	BobQKD_.setDoublePartitionSize(doublePartitionSize);
	BobQKD_.setMaximumSyndromeSize(maximumSyndromeSize);
	BobQKD_.setMinimumSyndromeSize(minimumSyndromeSize);
	BobQKD_.setDoubleClickNumber(doubleClickNumber);
	BobQKD_.setNoClickNumber(noClickNumber);
	BobQKD_.setBer(probabilityOfTransmissionError);
	BobQKD_.setSystematicSecurityParameter(systematicSecurityParameter);
	BobQKD_.setBypassHash(bypassHash);
	BobQKD_.setMinimumNumberOfPartitions(minimumNumberOfPartitions);
	BobQKD_.setSaveInternalSignals(false);
	BobQKD_.setLogValue(false);
	BobQKD_.setParameterEstimationNumberBits(parameterEstimationNumberBits);
	BobQKD_.setParameterEstimationNumberBitsToStart(parameterEstimationNumberBitsToStart);

	IPTunnel IpTunnel_4_{ {&Messages_BobToAlice},{&IPTunnelSourceOut_4} };
	IpTunnel_4_.setTcpPort(54003);
	IpTunnel_4_.setDisplayNumberOfSamples(false);
	IpTunnel_4_.setRemoteMachineIpAddress(ipTunnelIpAddress);

	Sink Sink_BobKeys_{ { &BobKeys },{} };
	Sink_BobKeys_.setDisplayNumberOfSamples(false);

	// #####################################################################################################
	// ########################### System Declaration and Inicialization ###################################
	// #####################################################################################################

	System MainSystem{
		{
			&IpTunnel_4_,
			&IpTunnel_5_,
			&IpTunnel_1_,
			&IpTunnel_2_,
			&IpTunnel_3_,
			&BobBasisBinarySource_,
			&BobQKD_,
			&Sink_BobKeys_,
		}
	};

	// #####################################################################################################
	// #################################### System Run #####################################################
	// #####################################################################################################


	MainSystem.run();
	MainSystem.terminate();
	auto finish = std::chrono::high_resolution_clock::now();
	
	std::chrono::duration<double> elapsed = finish - start;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n";
	system("pause");


	return 0;
}

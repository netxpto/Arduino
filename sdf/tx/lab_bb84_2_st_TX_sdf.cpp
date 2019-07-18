
//#include <stdio.h>
//#include <iostream>

#include "../../include/netxpto_20180815.h"
#include "../../include/sink_20180815.h"

#include "include/binary_source_20180815.h"
#include "include/bit_error_rate_20180815.h"
#include "include/alice_qkd_20190410.h"
#include "include/bob_qkd_20190410.h"
#include "include/ms_windows_ip_tunnel_20180815.h"
#include "include/quantum_channel_emulator_20190328.h"

// Input Parameters (Mostly to be left alone)
t_integer doubleClickNumber{ 2 };
t_integer noClickNumber{ 3 };
t_integer minimumNumberOfPartitions{ 1 };

int numberOfBits{ 50'000'000 };
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
t_real probabilityOfDoubleClick{ 0.1 };
t_real probabilityOfNoClick{ 0.1 };
//string ipTunnelIpAddress{ "127.0.0.1" };
//string ipTunnelIpAddress{ "192.168.92.197" };
//string ipTunnelIpAddress{ "192.168.81.172" };
//string ipTunnelIpAddress{ "192.168.86.122" };
//string ipTunnelIpAddress{ "192.168.92.131" };

string ipTunnelIpAddress{ "192.168.92.131" }; // sala bolseiros
//string ipTunnelIpAddress{ "192.168.81.172" }; // lab HP
//string ipTunnelIpAddress{ "192.168.81.234" }; // lab Fuji
int main()
{

	// #####################################################################################################
	// ################################ Variables Inicialization ###########################################
	// #####################################################################################################

	t_integer cascadeWindowSize = (t_integer)(floor(abs(qValue / (max(probabilityOfTransmissionError, 0.001)))));

	// #####################################################################################################
	// ########################### Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	Binary AliceData_In{ "AliceData_In.sgn", buffSize }; AliceData_In.setSaveSignal(false);
	Binary AliceBasis_In{ "AliceBasis_In.sgn", buffSize }; AliceBasis_In.setSaveSignal(false);
	Binary AliceTransmissionMode_Out{ "AliceTransmissionMode_Out.sgn", buffSize }; AliceTransmissionMode_Out.setSaveSignal(false); // 0 ou 1, quando é zero a Alice para a transmissão de bits

	Binary AliceData_Out{ "AliceData_Out.sgn", buffSize }; AliceData_Out.setSaveSignal(false);
	Binary AliceBasis_Out{ "AliceBasis_Out.sgn", buffSize }; AliceBasis_Out.setSaveSignal(false);
	Binary AliceSiftedKeys{ "AliceSiftedKeys.sgn", buffSize }; AliceSiftedKeys.setSaveSignal(false);
	Binary AliceKeys{ "AliceKeys.sgn", buffSize }; AliceKeys.setSaveSignal(true);
	Message Messages_AliceToBob{ "Messages_AliceToBob.sgn", buffSize }; Messages_AliceToBob.setSaveSignal(false); //Messages_AliceToBob.setSaveSignal(false);

	Binary BobBasis_Out{ "BobBasis_Out.sgn", buffSize }; BobBasis_Out.setSaveSignal(false);

	Binary IPTunnelSourceOut_1{ "IPTunnelSourceOut_1.sgn", buffSize };
	Binary IPTunnelSourceOut_2{ "IPTunnelSourceOut_2.sgn", buffSize };
	Binary IPTunnelSourceOut_3{ "IPTunnelSourceOut_3.sgn", buffSize };
	Message Messages_BobToAlice{ "Messages_BobToAlice.sgn", buffSize }; Messages_BobToAlice.setSaveSignal(false);

	TimeContinuousAmplitudeContinuousReal QCEData_Out{ "QCEData_Out.sgn", buffSize };
	Binary BobTransmissionMode_In{ "BobTransmissionMode_In.sgn", buffSize };
	TimeContinuousAmplitudeContinuousReal QBER_ChannelUpperBound{ "QBER_ChannelUpperBound.sgn", buffSize };


	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	IPTunnel IpTunnel_4_{ {},{&Messages_BobToAlice} };
	IpTunnel_4_.setTcpPort(54003);
	IpTunnel_4_.setDisplayNumberOfSamples(false);
	IpTunnel_4_.setRemoteMachineIpAddress(ipTunnelIpAddress);

	BinarySource AliceDataBinarySource_{ {},{ &AliceData_In } };
	AliceDataBinarySource_.setBitPeriod(1 / bitRate);
	AliceDataBinarySource_.setMode(BinarySourceMode::Random);
	//	AliceDataBinarySource_.setSeed(53129);
	AliceDataBinarySource_.setNumberOfBits(numberOfBits);

	BinarySource AliceBasisBinarySource_{ {},{ &AliceBasis_In } };
	AliceBasisBinarySource_.setBitPeriod(1 / bitRate);
	AliceBasisBinarySource_.setMode(BinarySourceMode::Random);
	//	AliceBasisBinarySource_.setSeed(53127);
	AliceBasisBinarySource_.setNumberOfBits(numberOfBits);

	AliceQKD AliceQKD_{ {&AliceData_In, &AliceBasis_In, &Messages_BobToAlice},{&AliceData_Out, &AliceBasis_Out, &AliceTransmissionMode_Out, &AliceKeys, &Messages_AliceToBob}, "signals/SuperBlock_AliceQKD", 8192, buffSize };
	AliceQKD_.setErrorCorrectionPartitionSize(cascadeWindowSize);
	AliceQKD_.setErrorCorrectionNumberOfPasses(numPasses);
	AliceQKD_.setDoublePartitionSize(doublePartitionSize);
	AliceQKD_.setMaximumSyndromeSize(maximumSyndromeSize);
	AliceQKD_.setMinimumSyndromeSize(minimumSyndromeSize);
	AliceQKD_.setDoubleClickNumber(doubleClickNumber);
	AliceQKD_.setNoClickNumber(noClickNumber);
	AliceQKD_.setBer(probabilityOfTransmissionError);
	AliceQKD_.setSystematicSecurityParameter(systematicSecurityParameter);
	AliceQKD_.setBypassHash(bypassHash);
	AliceQKD_.setMinimumNumberOfPartitions(minimumNumberOfPartitions);
	AliceQKD_.setSaveInternalSignals(false);
	AliceQKD_.setLogValue(false);
	AliceQKD_.setParameterEstimationNumberBits(parameterEstimationNumberBits);
	AliceQKD_.setParameterEstimationNumberBitsToStart(parameterEstimationNumberBitsToStart);

	IPTunnel IpTunnel_5_{ {},{&BobBasis_Out} };
	IpTunnel_5_.setTcpPort(54004);
	IpTunnel_5_.setDisplayNumberOfSamples(false);
	IpTunnel_5_.setRemoteMachineIpAddress(ipTunnelIpAddress);

	QuantumChannelEmulator QuantumChannelEmulator_{ { &AliceData_Out, &AliceBasis_Out, &AliceTransmissionMode_Out, &BobBasis_Out}, { &QCEData_Out, &BobTransmissionMode_In, &QBER_ChannelUpperBound } };
	QuantumChannelEmulator_.setProbabilityOfBothClick(probabilityOfDoubleClick);
	QuantumChannelEmulator_.setProbabilityOfNoClick(probabilityOfNoClick);
	QuantumChannelEmulator_.setProbabilityOfError(probabilityOfTransmissionError);
	QuantumChannelEmulator_.setDoubleClickNumber(doubleClickNumber);
	QuantumChannelEmulator_.setNoClickNumber(noClickNumber);

	IPTunnel IpTunnel_1_{ {&QCEData_Out},{&IPTunnelSourceOut_1} };
	IpTunnel_1_.setTcpPort(54000);
	IpTunnel_1_.setDisplayNumberOfSamples(false);
	IpTunnel_1_.setRemoteMachineIpAddress(ipTunnelIpAddress);

	IPTunnel IpTunnel_2_{ {&BobTransmissionMode_In},{&IPTunnelSourceOut_2} };
	IpTunnel_2_.setTcpPort(54001);
	IpTunnel_2_.setDisplayNumberOfSamples(false);
	IpTunnel_2_.setRemoteMachineIpAddress(ipTunnelIpAddress);

	IPTunnel IpTunnel_3_{ {&Messages_AliceToBob},{&IPTunnelSourceOut_3} };
	IpTunnel_3_.setTcpPort(54002);
	IpTunnel_3_.setDisplayNumberOfSamples(false);
	IpTunnel_3_.setRemoteMachineIpAddress(ipTunnelIpAddress);


//	Sink Sink_AliceBasis{ {&AliceBasis_Out},{} };
//	Sink_AliceBasis.setDisplayNumberOfSamples(false);

	//	Sink Sink_BobBasis{ {&BobBasis_Out},{} };
	//	Sink_BobBasis.setDisplayNumberOfSamples(false);

	//	Sink Sink_BobKeys_{ { &BobKeys },{} };
	//	Sink_BobKeys_.setDisplayNumberOfSamples(false);

	Sink Sink_AliceKeys_{ { &AliceKeys },{} };
	Sink_AliceKeys_.setDisplayNumberOfSamples(false);

	// #####################################################################################################
	// ########################### System Declaration and Inicialization ###################################
	// #####################################################################################################

	System MainSystem{
		{
			&AliceDataBinarySource_,
			&AliceBasisBinarySource_,
			&IpTunnel_4_,
			&AliceQKD_,
			&IpTunnel_5_,
			&QuantumChannelEmulator_,
			&IpTunnel_1_,
			&IpTunnel_2_,
			&IpTunnel_3_,
			
			&Sink_AliceKeys_,
		}
	};

	// #####################################################################################################
	// #################################### System Run #####################################################
	// #####################################################################################################


	MainSystem.run();
	MainSystem.terminate();
	system("pause");


	return 0;
}

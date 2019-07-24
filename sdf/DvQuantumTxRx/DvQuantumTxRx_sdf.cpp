# include "../../include/netxpto_20180815.h"
# include "../../include/binary_source_20180815.h"
# include "include/AliceQTx_20190404.h"
# include "../../include/sink_20180815.h"
# include "include/quantum_channel_dv_20190410.h"
# include "include/BobQRx_20190410.h"
# include "include/qber_20190418.h"


//##########################################################################################
//######################## Simulation Input Parameters #####################################
//##########################################################################################

double bitRate{ 100e6 };
double channelAttenuationDb{ 0.2 };
sop_rotation_type channelModel{ sop_rotation_type::Stocastic };
double PolarizationLinewidth{ 200e-100 };
double fiberLength_km{ 0 }; //80
double nPhotonsData{ 0.1 };
double nPhotonsControl{ 1 };
int nIncrementControl{ 100 };
long int numberOfQubits{ 2000000 };

//##########################################################################################
//##########################################################################################
//##########################################################################################


int main(){
	/*************************************************************************************************************************/
	/***********************************************Signals*******************************************************************/
	/*************************************************************************************************************************/

	Binary AliceQTxBasis{ "AliceQTxBasis.sgn" }, AliceQTxBits{ "AliceQTxBits.sgn" }, ModeSelection{ "ModeSelection.sgn" }, AliceQTxBits_For_Qber{"AliceQTxBasis_For_Qber.sgn"};
	PhotonStreamXY AliceQTx_Out{ "AliceQTx_Out.sgn" }, BobQRx_In{"BobQRx_In.sgn"};
	Binary BobQRxBasis{ "BobQRxBasis.sgn" };
	TimeDiscreteAmplitudeContinuousReal BobQRx_DataOut{ "BobQRx_DataOut.sgn" }, QBER_QChannel{"QBER_QChannel.sgn"};
	Binary QberRaw{ "QberRaw.sgn" };
	
	/*************************************************************************************************************************/
	/************************************************Blocks*******************************************************************/
	/*************************************************************************************************************************/
	BinarySource BSAliceQTxBasis_{ {},{&AliceQTxBasis} };
	BSAliceQTxBasis_.setMode(BinarySourceMode::DeterministicCyclic);
	BSAliceQTxBasis_.setBitPeriod(1 / bitRate);
	BSAliceQTxBasis_.setBitStream("1");
	BSAliceQTxBasis_.setNumberOfBits(1000);

	BinarySource BSAliceQTxBits_{ {},{&AliceQTxBits, &AliceQTxBits_For_Qber} };
	BSAliceQTxBits_.setMode(BinarySourceMode::DeterministicCyclic);
	BSAliceQTxBits_.setBitPeriod(1 / bitRate);
	BSAliceQTxBits_.setBitStream("0");
	BSAliceQTxBits_.setNumberOfBits(numberOfQubits);
	
	AliceQTx AliceQTx_{ {&AliceQTxBasis,&AliceQTxBits, &ModeSelection},{&AliceQTx_Out}, "signals/SuperBlock_AliceQTx"};
	AliceQTx_.setLogValue(true);
	AliceQTx_.setSaveInternalSignals(false);
	AliceQTx_.setControlBasisSequence("00");
	AliceQTx_.setControlBitsSequence("00");
	AliceQTx_.setNumberOfPhotonsData(nPhotonsData);
	AliceQTx_.setNumberOfPhotonsControl(nPhotonsControl);
	AliceQTx_.setIncrementControl(nIncrementControl);
	AliceQTx_.setBitRate(bitRate);
	

	QuantumChannel QuantumChannel_{ {&AliceQTx_Out},{&BobQRx_In}, "signals/SuperBlock_QuantumChannel" };
	QuantumChannel_.setLogValue(true);
	QuantumChannel_.setSaveInternalSignals(false);
	QuantumChannel_.setAtennuationDbmPerKm(channelAttenuationDb);
	QuantumChannel_.setFiberLength(fiberLength_km);
	QuantumChannel_.setFiberModel(channelModel);
	QuantumChannel_.setPolarizationLinewidth(PolarizationLinewidth);

	BinarySource BSBobQRxBasis_{ {},{&BobQRxBasis} };
	BSBobQRxBasis_.setMode(BinarySourceMode::DeterministicCyclic);
	BSBobQRxBasis_.setBitStream("1");
	BSBobQRxBasis_.setBitPeriod((1 / bitRate));
	BSBobQRxBasis_.setNumberOfBits(numberOfQubits);

	BobQRx BobQRx_{ {&BobQRxBasis, &BobQRx_In},{&BobQRx_DataOut, &ModeSelection, &QBER_QChannel}, "signals/SuperBlock_BobQRx" };
	BobQRx_.setLogValue(true);
	BobQRx_.setSaveInternalSignals(false);
	BobQRx_.setControlSequenceBits("00");
	BobQRx_.setControlBasisMeasurement("00");
	BobQRx_.setIncrementControl(nIncrementControl);
	BobQRx_.setBitRate(bitRate);

	Qber QberRaw_{ {&AliceQTxBits_For_Qber, &BobQRx_DataOut} , {&QberRaw} };

	Sink BobQRx_Sink_{ { &QberRaw },{} };
	BobQRx_Sink_.setDisplayNumberOfSamples(true);
	//BobQRx_Sink_.setNumberOfSamples(800000);
	
	Sink QBERQChannel_Sink_{ { &QBER_QChannel },{} };

	/*************************************************************************************************************************/
	/***********************************************Main System***************************************************************/
	/*************************************************************************************************************************/

	System MainSystem{
		&BSAliceQTxBasis_,
		&BSAliceQTxBits_,
		&AliceQTx_,
		&QuantumChannel_,
		&BSBobQRxBasis_,
		&BobQRx_,
		&QberRaw_,
		&BobQRx_Sink_,
		&QBERQChannel_Sink_
	};

	MainSystem.run();
	MainSystem.terminate();

	system("pause");
	return 0;
}
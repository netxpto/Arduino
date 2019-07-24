#ifndef ALICEQTX_H
#define ALICEQTX_H

# include "../../../include/netxpto_20180815.h"
# include "../../../include/binary_source_20180815.h"
# include "../../../include/parallelToSerial_20190404.h"
# include "../../../include/mux2To1_20190404.h"
# include "../include/one_to_n_20190410.h"
# include "../../../include/m_qam_mapper_20180815.h"
# include "../../../include/sink_20180815.h"
# include "../../../include/discrete_to_continuous_time_20190406.h"
# include "../../../include/pulse_shaper_20180815.h"
# include "../../../include/laser_20190408.h"
# include "../include/amplitude_modulator_20190418.h"
# include "../../../include/time_delay_20180815.h"
# include "../include/polarization_modulator_20190418.h"
# include "../../../include/optical_attenuator_20180304.h"
# include "../include/counter_n_20190426.h"


class AliceQTx : public SuperBlock {

public:

	//##############################################################################################################

	AliceQTx(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals) : SuperBlock(inputSignals, outputSignals) {
		setLogFileName("SuperBlock_AliceQTx.txt");

	};

	AliceQTx(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals, string sFolderName) : SuperBlock(inputSignals, outputSignals) {
		setSignalsFolderName(sFolderName);
		setLogFileName("SuperBlock_AliceQTx.txt");

		//############################################# Blocks ##########################################################################

		BSSelMonitoringMode_.setMode(BinarySourceMode::DeterministicCyclic);
		BSSelMonitoringMode_.setBitStream("0");
		BSSelMonitoringMode_.setBitPeriod(1 / bitRate);

		BSSelActuationMode_.setMode(BinarySourceMode::DeterministicCyclic);
		BSSelActuationMode_.setBitStream("1");
		BSSelActuationMode_.setBitPeriod(1 / bitRate);

		OneToN_.setN(3);

		BSData_.setMode(BinarySourceMode::DeterministicCyclic);
		BSData_.setBitStream("0");
		BSData_.setBitPeriod(1 / (3 * bitRate));

		BSControl_.setMode(BinarySourceMode::DeterministicCyclic);
		BSControl_.setBitStream("1");
		BSControl_.setBitPeriod(1 / (3 * bitRate));

		BSControlBasis_.setMode(BinarySourceMode::DeterministicCyclic);
		BSControlBasis_.setBitStream("0");// Base 45/-45 = '0'; Base circular ='1'
		BSControlBasis_.setBitPeriod(1 / (3 * bitRate));

		BSControlBits_.setMode(BinarySourceMode::DeterministicCyclic);
		BSControlBits_.setBitStream("0"); // 45/CircDir = '1', -45/CircEsq = '0'
		BSControlBits_.setBitPeriod(1 / (3 * bitRate));

		MQamMapper_.setIqAmplitudes(iqAmplitudesValues);
		MQamMapper_.setSystemCausality(false);

		DTCTI_.setNumberOfSamplesPerSymbol(samplesPerSymbol);

		PulseShaperI_.setFilterType(pulse_shapper_filter_type::Square);
		PulseShaperI_.setPulseWidth((1 / bitRate) / 2);

		TimeDelay_.setTimeDelay(0);
		TimeDelay_.setFilterType(pulse_shapper_filter_type::TimeDelayedDeltaDirac);

		DTCTQ_.setNumberOfSamplesPerSymbol(samplesPerSymbol);

		PulseShaperQ_.setFilterType(pulse_shapper_filter_type::Square);
		PulseShaperQ_.setPulseWidth((1 / bitRate) / 12);

		Laser_.setOpticalPower(laserPowerW);
		Laser_.setWavelength(opticalWavelength);
		Laser_.setSymbolPeriod(1 / (bitRate));
		Laser_.setSamplingPeriod(1 / (bitRate) / samplesPerSymbol);

		//OpticalAttenuator_.setAttenuationCoef(Attenuation_dB);

	};

	void initialize(void);
	bool runBlock(void);

	bool firstTime{ true };
	//##############################################################################################################

	void setControlBasisSequence(string bSeq) { BSControlBasis_.setBitStream(bSeq); };
	void setControlBitsSequence(string bits) { BSControlBits_.setBitStream(bits); };
	void setNumberOfPhotonsData(double nData) { 
		opticalAmplitudeData = sqrt(nData * PLANCK_CONSTANT * opticalFrequency); 
		numberOfPhotonsData = nData;
		iqAmplitudesValues = { { 0, opticalAmplitudeData },{ 0.5, opticalAmplitudeData },{ 0.25,  opticalAmplitudeData },{ 0.75,  opticalAmplitudeData },{ 0, opticalAmplitudeControl },{ 0.5, opticalAmplitudeControl},{ 0.25, opticalAmplitudeControl },{ 0.75, opticalAmplitudeControl } };
		MQamMapper_.setIqAmplitudes(iqAmplitudesValues);
	};
	void setNumberOfPhotonsControl(double nControl) { 
		opticalAmplitudeControl = sqrt(nControl * PLANCK_CONSTANT * opticalFrequency); 
		numberOfPhotonsControl = nControl;
		iqAmplitudesValues = { { 0, opticalAmplitudeData },{ 0.5, opticalAmplitudeData },{ 0.25,  opticalAmplitudeData },{ 0.75,  opticalAmplitudeData },{ 0, opticalAmplitudeControl },{ 0.5, opticalAmplitudeControl},{ 0.25, opticalAmplitudeControl },{ 0.75, opticalAmplitudeControl } };
		MQamMapper_.setIqAmplitudes(iqAmplitudesValues);
	};
	void setBitRate(double bRate) { 
		BSSelMonitoringMode_.setBitPeriod(1 / bRate);
		BSSelActuationMode_.setBitPeriod(1 / bRate);
		BSData_.setBitPeriod(1 / (3 * bRate));
		BSControl_.setBitPeriod(1 / (3 * bRate));
		BSControlBasis_.setBitPeriod(1 / (3 * bRate));
		BSControlBits_.setBitPeriod(1 / (3 * bRate));
		PulseShaperI_.setPulseWidth((1 / bRate) / 2);
		PulseShaperQ_.setPulseWidth((1 / bRate) / 12);
		Laser_.setSymbolPeriod(1 / (bRate));
		Laser_.setSamplingPeriod(1 / (bRate) / samplesPerSymbol);
	};
	void setIncrementControl(int inc) { CounterN_.setN(inc); };
	//###############################################################################################################

private:

	//###################################### Default Input Parameters Values #########################################################
	//	vector<vector<t_real>> iqAmplitudesValues{ { 0, 0, 0, 1, 0.1 },{ 0.5, 0, 0, 0, 0.1},{ 0.25, 0, 1, 1, 0.1},{ 0.75, 0, 1, 0, 0.1},{ 0, 1, 0, 1, 5 },{ 0.5, 0, 0, 0, 5},{ 0.25, 0, 1, 1, 5},{ 0.75, 0, 1, 0, 5} };
	t_real bitRate{ 1e3 };
	int incrementControl{ 100 };
	int samplesPerSymbol{ 16 };
	double laserPowerW{ 4 };
	//double Attenuation_dB{ 10.0 };
	double opticalWavelength{ 1550e-9 };
	double opticalFrequency{ SPEED_OF_LIGHT / opticalWavelength };
	double numberOfPhotonsData{ 1 };
	double numberOfPhotonsControl{ 80 };
	double opticalAmplitudeData{ sqrt(numberOfPhotonsData * PLANCK_CONSTANT * opticalFrequency) };
	double opticalAmplitudeControl{ sqrt(numberOfPhotonsControl * PLANCK_CONSTANT * opticalFrequency) };
	vector<vector<t_real>> iqAmplitudesValues{ { 0, opticalAmplitudeData },{ 0.5, opticalAmplitudeData },{ 0.25,  opticalAmplitudeData },{ 0.75,  opticalAmplitudeData },{ 0, opticalAmplitudeControl },{ 0.5, opticalAmplitudeControl},{ 0.25, opticalAmplitudeControl },{ 0.75, opticalAmplitudeControl } };

	/*Selecting transmitter mode*/
	Binary MonitoringMode{ "MonitoringMode.sgn",1 }, ActuationMode{ "ActuationMode.sgn",1 }, ModeSelection{ "ModeSelection.sgn" ,1 }, ModeSelectionOut{ "ModeSelectionOut",1 }, OperationMode{ "OperationMode.sgn",1 }; // Enable comes from input of superblock
	BinarySource BSSelMonitoringMode_{ {},{&MonitoringMode} };

	CounterN CounterN_{ {&ModeSelection},{&ModeSelectionOut} };

	BinarySource BSSelActuationMode_{ {},{&ActuationMode} };
	Mux2To1 MuxSelMode_{ {&MonitoringMode, &ActuationMode, &ModeSelectionOut },{&OperationMode} };

	Binary OneToN_Out{ "OneToN_Out.sgn" ,3};
	OneToN OneToN_{ { &OperationMode } , {&OneToN_Out } };

	/*Data transmitter sequence*/
	Binary BasisData{ "BasisData.sgn",1 }, BitsData{ "BitsData.sgn",1 }, Data0{ "Data0.sgn",1  }, DataOut{ "DataOut.sgn",3 };
	BinarySource BSData_{ {},{&Data0} };
	
	ParallelToSerial ParalleToSerialData_{ {&Data0, &BasisData, &BitsData}, {&DataOut} };

	/*Control transmitter sequence*/
	Binary BasisControl{ "BasisControl.sgn",1 }, BitsControl{ "BitsControl.sgn",1 }, Control1{ "Control1.sgn", 1 }, ControlOut{ "ControlOut.sgn", 3 };
	BinarySource BSControl_{ {},{&Control1} };
	BinarySource BSControlBasis_{ {},{&BasisControl} };
	BinarySource BSControlBits_{ {},{&BitsControl} };

	ParallelToSerial ParallelToSerialControl_{ {&Control1, &BasisControl, &BitsControl},{&ControlOut} };


	/*Select operation mode and output a bitStream*/
	Binary TripleBitStream{ "TripleBitStream.sgn" ,3}; // {data='1'/control='0'; Basis; Bits}
	Mux2To1 MuxOutputBitStream_{ {&ControlOut, &DataOut, &OneToN_Out },{&TripleBitStream} };

	TimeDiscreteAmplitudeContinuousReal PolarizationSignal{ "PolarizationSignal.sgn" }, AmplitudeSignal{ "AmplitudeSignal.sgn" };
	
	MQamMapper MQamMapper_{ {&TripleBitStream},{&PolarizationSignal, &AmplitudeSignal} };

	TimeContinuousAmplitudeContinuousReal DTCTI_out{ "DTCTI_out.sgn",16 };
	DiscreteToContinuousTime DTCTI_{ {&PolarizationSignal},{&DTCTI_out} };

	TimeContinuousAmplitudeContinuousReal PSI_out{ "PSI_out.sgn",16 };
	PulseShaper PulseShaperI_{ {&DTCTI_out},{&PSI_out} };

	TimeContinuousAmplitudeContinuousReal Pol_ModIn{ "Pol_ModIn.sgn",16 };
	PulseShaper TimeDelay_{ {&PSI_out},{&Pol_ModIn} };

	TimeContinuousAmplitudeContinuousReal DTCTQ_out{ "DTCTQ_out.sgn",16 };
	DiscreteToContinuousTime DTCTQ_{ {&AmplitudeSignal},{ &DTCTQ_out } };

	TimeContinuousAmplitudeContinuousReal OAM_InSignal{ "OAM_InSignal.sgn",16 };
	PulseShaper PulseShaperQ_{ {&DTCTQ_out},{&OAM_InSignal} };

	OpticalSignal Laser_out{ "LaserOut.sgn",16 };
	Laser Laser_{ {},{&Laser_out} };

	PhotonStreamXY  OAM_outSignal{ "OAM_outSignal.sgn",16 };
	AmplitudeModulator AmplitudeModulator_{ {&Laser_out, &OAM_InSignal},{&OAM_outSignal} };

	PhotonStreamXY Pol_ModOut{ "Pol_ModOut.sgn",16 };
	PolarizationModulator PolarizationModulator_{ {&OAM_outSignal, &Pol_ModIn},{&Pol_ModOut} };

	/*PhotonStreamXY AliceQTx_out{ "AliceQTx_out.sgn",16 };
	OpticalAttenuator OpticalAttenuator_{ {&Pol_ModOut},{&AliceQTx_out} };
	*/
};

#endif // !ALICEQTX_H


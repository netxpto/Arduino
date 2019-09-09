#ifndef ALICEQTX_H
#define ALICEQTX_H

# include "../include/netxpto_20180815.h"
# include "../include/binary_source_20180815.h"
# include "../include/parallelToSerial_20190404.h"
# include "../include/mux2To1_20190404.h"
# include "../include/m_qam_mapper_20180815.h"
# include "../include/sink_20180815.h"
# include "../include/discrete_to_continuous_time_20190406.h"
# include "../include/pulse_shaper_20180815.h"
# include "../include/laser_20190408.h"
# include "../include/amplitude_modulator_20190408.h"
# include "../include/time_delay_20180815.h"
# include "../include/polarization_modulator_20190408.h"
# include "../include/optical_attenuator_20180304.h"
# include "../include/andGate_20190410.h"


class AliceQTx : public SuperBlock {

public:

	//##############################################################################################################

	AliceQTx(initializer_list<Signal *> inputSig, initializer_list<Signal *> outputSig);

	void initialize(void);
	bool runBlock(void);

	bool firstTime{ true };
	//##############################################################################################################

	void setControlBasisSequence(string bSeq) { BSControlBasis_.setBitStream(bSeq); };
	void setControlBitsSequence(string bits) { BSControlBits_.setBitStream(bits); };

	//###############################################################################################################

private:
	/*Input Parameters*/
	string signalsFolderName{ "signals/SuperBlock_AliceQTx" };
	bool logValue{ true };
	string logFileName{ "SuperBlock_AliceQTx.txt" };

	/*Selecting transmitter mode*/
	Binary MonitoringMode{ "SelMonitoringMode.sgn" }, ActuationMode{ "SelActuationMode.sgn" }, ModeSelection{ "ModeSelection.sgn"  }, OperationMode{ "OperationMode.sgn" }; // Enable comes from input of superblock
	BinarySource BSSelMonitoringMode_{ {},{&MonitoringMode} };
	BinarySource BSSelActuationMode_{ {},{&ActuationMode} };
	Mux2To1 MuxSelMode_{ {&MonitoringMode, &ActuationMode, &ModeSelection },{&OperationMode} };

	/*Data transmitter sequence*/
	Binary BasisData{ "BasisData.sgn" }, BitsData{ "BitsData.sgn" }, Data0{ "Data0.sgn"  }, DataOut{ "DataOut.sgn" }, enableSel1{ "enableSel1.sgn" };
	BinarySource BSData_{ {},{&Data0} };
	
	ParallelToSerial ParalleToSerialData_{ {&Data0, &BasisData, &BitsData}, {&DataOut, &enableSel1} };

	/*Control transmitter sequence*/
	Binary BasisControl{ "BasisControl.sgn" }, BitsControl{ "BitsControl.sgn" }, Control1{ "Control1.sgn" }, ControlOut{ "ControlOut.sgn" }, enableSel2{ "enableSel2.sgn" };
	BinarySource BSControl_{ {},{&Control1} };
	BinarySource BSControlBasis_{ {},{&BasisControl} };
	BinarySource BSControlBits_{ {},{&BitsControl} };

	ParallelToSerial ParallelToSerialControl_{ {&Control1, &BasisControl, &BitsControl},{&ControlOut, & enableSel2} };

	Binary enableSel{ "enableSel.sgn" };

	AndGate AndGate_{ {&enableSel1, &enableSel2},{&enableSel} };

	/*Select operation mode and output a bitStream*/
	Binary TripleBitStream{ "TripleBitStream.sgn" }; // {data='1'/control='0'; Basis; Bits}
	Mux2To1 MuxOutputBitStream_{ {&ControlOut,&DataOut,&OperationMode, &enableSel},{&TripleBitStream} };

	TimeDiscreteAmplitudeContinuousReal PolarizationSignal{ "PolarizationSignal.sgn" }, AmplitudeSignal{ "AmplitudeSignal.sgn" };
	TimeDiscreteAmplitudeContinuousReal MQM_mode{ "MQM_mode.sgn" }, MQM_basis{ "MQM_basis.sgn" }, MQM_bits{ "MQM_bits.sgn" };

	//MQamMapper MQamMapper_{ {&TripleBitStream},{&PolarizationSignal, &MQM_mode, &MQM_basis, &MQM_bits, &AmplitudeSignal} };
	MQamMapper MQamMapper_{ {&TripleBitStream},{&PolarizationSignal, &AmplitudeSignal} };

	/*Sink SinkMQM1_{ {&MQM_mode},{} };
	Sink SinkMQM2_{ {&MQM_basis},{} };
	Sink SinkMQM3_{ {&MQM_bits},{} };
	*/
	TimeContinuousAmplitudeContinuousReal DTCTI_out{ "DTCTI_out.sgn" };
	DiscreteToContinuousTime DTCTI_{ {&PolarizationSignal},{&DTCTI_out} };

	TimeContinuousAmplitudeContinuousReal PSI_out{ "PSI_out.sgn" };
	PulseShaper PulseShaperI_{ {&DTCTI_out},{&PSI_out} };

	TimeContinuousAmplitudeContinuousReal Pol_ModIn{ "Pol_ModIn.sgn" };
	TimeDelay TimeDelay_{ {&PSI_out},{&Pol_ModIn} };

	TimeContinuousAmplitudeContinuousReal DTCTQ_out{ "DTCTQ_out.sgn" };
	DiscreteToContinuousTime DTCTQ_{ {&AmplitudeSignal},{ &DTCTQ_out } };

	TimeContinuousAmplitudeContinuousReal OAM_InSignal{ "OAM_InSignal.sgn" };
	PulseShaper PulseShaperQ_{ {&DTCTQ_out},{&OAM_InSignal} };

	OpticalSignal Laser_out{ "LaserOut.sgn" }, OAM_outSignal{ "OAM_outSignal.sgn" };
	Laser Laser_{ {},{&Laser_out} };

	AmplitudeModulator AmplitudeModulator_{ {&Laser_out, &OAM_InSignal},{&OAM_outSignal} };

	PhotonStreamXY Pol_ModOut{ "Pol_ModOut.sgn" };
	PolarizationModulator PolarizationModulator_{ {&OAM_outSignal, &Pol_ModIn},{&Pol_ModOut} };

	PhotonStreamXY AliceQTx_out{ "AliceQTx_out.sgn" };
	OpticalAttenuator OpticalAttenuator_{ {&Pol_ModOut},{&AliceQTx_out} };

};

#endif // !ALICEQTX_H


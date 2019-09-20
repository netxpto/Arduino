# ifndef QUANTUMCHANNELDV_H
# define QUANTUMCHANNELDV_H

# include  "../../../include/netxpto_20180815.h"
# include "../../../include/optical_attenuator_20180304.h"
# include "../include/sop_rotator_20180815.h"

class QuantumChannel : public SuperBlock {

public:

	//##############################################################################################################

	QuantumChannel(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals) : SuperBlock(inputSignals, outputSignals) {
		setLogFileName("SuperBlock_QuantumChannel.txt");
		//############################################# Blocks ##########################################################################
		OpticalAttenuator_.setAttenuationCoef(attenuationDbm_perKm);
		OpticalAttenuator_.setFiberLength(fiberLength);

		SopRotator_.setSOPRotationType(channelType);
		SopRotator_.setDeltaP(polLineWidth);
		SopRotator_.setQBERi(qberToRotate);
		SopRotator_.setTimeForSingleRotation(timeHeavyRotation);
	};

	QuantumChannel(initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals, string sFolderName) : SuperBlock(inputSignals, outputSignals) {
		setSignalsFolderName(sFolderName);
		setLogFileName("SuperBlock_QuantumChannel.txt");
		//############################################# Blocks ##########################################################################
		OpticalAttenuator_.setAttenuationCoef(attenuationDbm_perKm);
		OpticalAttenuator_.setFiberLength(fiberLength);

		SopRotator_.setSOPRotationType(channelType);
		SopRotator_.setDeltaP(polLineWidth);
		SopRotator_.setQBERi(qberToRotate);
		SopRotator_.setTimeForSingleRotation(timeHeavyRotation);
	};

	void initialize(void);
	bool runBlock(void);

	bool firstTime{ true };
	//##############################################################################################################

	void setAtennuationDbmPerKm(double att) { OpticalAttenuator_.setAttenuationCoef(att); }
	void setFiberLength(double fLength) { OpticalAttenuator_.setFiberLength(fLength); };

	void setFiberModel(sop_rotation_type fModel) { SopRotator_.setSOPRotationType(fModel); };
	void setPolarizationLinewidth(double pLinewidth) { SopRotator_.setDeltaP(pLinewidth); };

	//###############################################################################################################

private:
	//###################################### Default Input Parameters Values #########################################################
	double attenuationDbm_perKm{ 0.0 };
	double fiberLength{ 1.0 };
	sop_rotation_type channelType{ sop_rotation_type::Stocastic };
	double polLineWidth{ 900e-9 };
	double qberToRotate{ 0.0 };
	double timeHeavyRotation{ 0 };

	PhotonStreamXY inSinglePhoton{ "inSinglePhoton.sgn" }, attenuatedSPhoton{ "AttenuatedSPhoton.sgn" };
	OpticalAttenuator OpticalAttenuator_{ {&inSinglePhoton},{&attenuatedSPhoton} };

	PhotonStreamXY driftedSPhoton{ "driftedSPhoton.sgn" };
	SOPRotator SopRotator_{{&attenuatedSPhoton},{&driftedSPhoton}};

};

#endif // !QUANTUMCHANNELDV_H


# ifndef QUANTUMCHANNELDV_H
# define QUANTUMCHANNELDV_H

# include  "../include/netxpto_20180815.h"
# include "../include/optical_attenuator_20180304.h"
# include "../include/sop_rotator_20180815.h"

class QuantumChannel : public SuperBlock {

public:

	//##############################################################################################################

	QuantumChannel(initializer_list<Signal *> inputSig, initializer_list<Signal *> outputSig);

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
	/*Input Parameters*/
	string signalsFolderName{ "signals/SuperBlock_QuantumChannel" };
	bool logValue{ true };
	string logFileName{ "SuperBlock_QuantumChannel.txt" };

	PhotonStreamXY inSinglePhoton{ "inSinglePhoton.sgn" }, attenuatedSPhoton{ "AttenuatedSPhoton.sgn" };
	OpticalAttenuator OpticalAttenuator_{ {&inSinglePhoton},{&attenuatedSPhoton} };

	PhotonStreamXY driftedSPhoton{ "driftedSPhoton.sgn" };
	SOPRotator SopRotator_{{&attenuatedSPhoton},{&driftedSPhoton}};

};

#endif // !QUANTUMCHANNELDV_H


# ifndef QUANTUMCHANNELDV_H
# define QUANTUMCHANNELDV_H

# include  "../include/netxpto_20180815.h"
# include "../include/optical_attenuator_20180304.h"
# include "../include/SOP_modulator_20180514.h"

class QuantumChannel : public SuperBlock {

public:

	//##############################################################################################################

	QuantumChannel(initializer_list<Signal *> inputSig, initializer_list<Signal *> outputSig);

	void initialize(void);
	bool runBlock(void);

	bool firstTime{ true };
	//##############################################################################################################

	//###############################################################################################################

private:
	/*Input Parameters*/
	string signalsFolderName{ "signals/SuperBlock_QuantumChannel" };
	bool logValue{ true };
	string logFileName{ "SuperBlock_QuantumChannel.txt" };

	PhotonStreamXY inSinglePhoton{ "inSinglePhoton.sgn" }, attenuatedSPhoton{ "AttenuatedSPhoton.sgn" };
	OpticalAttenuator OpticalAttenuator_{ {&inSinglePhoton},{&attenuatedSPhoton} };

	PhotonStreamXY driftedSPhoton{ "driftedSPhoton.sgn" };
	SOPModulator SopModulator_{{&attenuatedSPhoton},{&driftedSPhoton}};

};

#endif // !QUANTUMCHANNELDV_H


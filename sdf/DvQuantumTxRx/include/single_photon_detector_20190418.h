# ifndef SINGLE_PHOTON_DETECTOR_H_
# define SINGLE_PHOTON_DETECTOR_H_

# include <random>
# include <algorithm>  // min()
# include <math.h>	   // remainder(), pow()
# include <chrono>

# include "../../../include/netxpto_20180815.h"

class SinglePhotonDetector : public Block {
	
public:
	//###############################################################################################################################################
	SinglePhotonDetector (initializer_list<Signal *> inputSignals, initializer_list<Signal *> outputSignals) : Block(inputSignals, outputSignals) {};
	SinglePhotonDetector (vector<Signal *> &inputSignals, vector<Signal *> &outputSignals) : Block(inputSignals, outputSignals){};

	void initialize(void);
	bool runBlock(void);
	//###############################################################################################################################################

	void setPath(t_integer p) { path = p; };
	t_integer getPath() { return path; };

	void setProbabilityDarkCount(double pdarkcount) { probabilityDarkCount = pdarkcount; };
	double getProbabilityDarkCount() { return probabilityDarkCount; };

	void setDetectorEfficiency(double eff) { efficiency = eff; };
	//###############################################################################################################################################

private:
//	long int seed{ (long int)chrono::system_clock::now().time_since_epoch().count() };
	long int seed{ 0 };
	std::default_random_engine generator{ seed };

	// Input Parameters
	t_integer path{ 0 };
	double probabilityDarkCount{ 0.0 };
	int out{ 0 };
	int samplesPerSymbol{ 1 };
	bool firstTime{ true };
	double efficiency{ 0.25 };

	double opticalWavelength{ 1550e-9 };
	double opticalFrequency{ SPEED_OF_LIGHT / opticalWavelength };
};

#endif

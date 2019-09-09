#ifndef POLARIZATION_MODULATOR_H
#define POLARIZATION_MODULATOR_H

# include "../include/netxpto_20180815.h"

class PolarizationModulator : public Block {

public:
	PolarizationModulator(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) : Block(InputSig, OutputSig) {};
	PolarizationModulator(vector<Signal *> &InputSig, vector<Signal *> &OutputSig) : Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

private:
	/*Input Parameters*/
	vector<t_real> inVoltages{ 0.0, 0.25, 0.5, 0.75 };
	vector<vector<t_real>> valuesToRotate{ {0,0},{PI/4, 0},{PI/2,0},{-PI/4,0} }; // {theta, phi} for 45º, circDir, -45º, circLeft

	/*State Variables*/
	t_real theta{ 0.0 };
	t_real phi{ 0.0 };

	double rotation[3][3] = { {cos(phi)*cos(theta), -cos(phi)*sin(theta), sin(phi)},
								{cos(phi)*sin(theta) + pow(sin(phi),2)*cos(theta), cos(phi)*cos(theta) - pow(sin(phi),2)*sin(theta), -sin(phi)*cos(phi)},
								{sin(phi)*sin(theta) - sin(phi)*cos(phi)*cos(theta), sin(phi)*cos(theta) + cos(phi)*sin(phi)*sin(theta), pow(cos(phi),2)}
	};

	int n{ 0 };

};


#endif

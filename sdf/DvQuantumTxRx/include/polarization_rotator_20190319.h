#ifndef POLARIZE_H_
#define POLARIZER_H_

# include <algorithm>  // min()
# include <math.h>     // cos(), sin()
#include <array>
#include "../../../include/netxpto_20180815.h"


class PolarizationRotator : public Block{
	
	
public:
	// Input parameters
	double tetha{ 0.0 };
	double phi{ 0.0 };

	//Methods
	PolarizationRotator(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);

	bool runBlock(void);

	
};
#endif // !POLARIZE_H_


# include "netxpto.h"
# include "binary_source.h"
# include "m_qam_mapper.h"	
# include "discrete_to_continuous_time.h"
# include "pulse_shaper.h"							
# include "sink.h"
# include "transform.h"




int main() {

	// #####################################################################################################
	// ################################### System Input Parameters #########################################
	// #####################################################################################################

	BinarySourceMode sourceMode{ PseudoRandom };
	int patternLength{ 5 };
	double bitPeriod{ 1.0 / 1.25e9 };
	vector<t_iqValues> iqAmplitudes{ { { 0,0 },{ 1,0 },{ 2,0 },{ 3,0 } } };
	int numberOfBits{ 128 };				// For value of {-1}, it'll generate long bit sequence.
	int numberOfSamplesPerSymbol{ 8 };
	double rollOffFactor{ 0.3 };
	int impulseResponseTimeLength{ 16 };
	double rfFrequency{ 1.25E9 };				// It depends on the bandwidth of the signal
	double rfAmplitude{ 1.0 };
	double rfInitialPhase{ 0.0 };
	double samplingPeriod{ bitPeriod / numberOfSamplesPerSymbol };
	double outputOpticalWavelength{ 1550e-9 };
	double opticalPower{ 1e-3 };

	// #####################################################################################################
	// ########################### Signals Declaration and Inicialization ##################################
	// #####################################################################################################

	Binary S1{ "S1.sgn" };										// Binary Sequence
	TimeDiscreteAmplitudeDiscreteReal S2{ "S2.sgn" };			// MPAM Signal
	TimeDiscreteAmplitudeDiscreteReal S2b{ "S2b.sgn" };			// Not used (Q signal)
	TimeContinuousAmplitudeDiscreteReal S3{ "S3.sgn" };			// Discrete to continious time
	TimeContinuousAmplitudeContinuousReal S4{ "S4.sgn" };		// Pulse Shapping filter
	TimeContinuousAmplitudeContinuousReal S5{ "S5.sgn" };		// Transform
	

	// #####################################################################################################
	// ########################### Blocks Declaration and Inicialization ###################################
	// #####################################################################################################

	BinarySource B1{ vector<Signal*> {}, vector<Signal*> { &S1} };
	B1.setMode(sourceMode);
	B1.setPatternLength(patternLength);
	B1.setBitPeriod(bitPeriod);
	B1.setNumberOfBits(numberOfBits);

	MQamMapper B2{ vector<Signal*> { &S1 }, vector<Signal*> { &S2, &S2b } };
	B2.setIqAmplitudes(iqAmplitudes);

	Sink B3{ vector<Signal*> { &S2b }, vector<Signal*> {} };

	DiscreteToContinuousTime B4{ vector<Signal*> { &S2 }, vector<Signal*> { &S3 } };
	B4.setNumberOfSamplesPerSymbol(numberOfSamplesPerSymbol);

	PulseShaper B5{ vector<Signal*> { &S3}, vector<Signal*> { &S4 } };
	B5.setRollOffFactor(rollOffFactor);
	B5.setImpulseResponseTimeLength(impulseResponseTimeLength);
	B5.setSeeBeginningOfImpulseResponse(false);

	Transform B6{ vector<Signal*> { &S4}, vector<Signal*> { &S5 } };

	Sink B7{ vector<Signal*> { &S5 }, vector<Signal*> {} };

	// #####################################################################################################
	// ########################### System Declaration and Inicialization ###################################
	// #####################################################################################################

	System MainSystem{ vector<Block*> { &B1, &B2, &B3, &B4, &B5, &B6, &B7} };

	// #####################################################################################################
	// #################################### System Run #####################################################
	// #####################################################################################################

	MainSystem.run();
	cout << "\nExecution Finished, Please hit Enter to exit!";
	getchar();
	return 0;
}
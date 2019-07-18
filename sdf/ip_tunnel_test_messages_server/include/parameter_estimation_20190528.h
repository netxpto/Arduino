#ifndef PARAMETER_ESTIMATION_H_
#define PARAMETER_ESTIMATION_H_

#include "../../../include/netxpto_20180815.h"
#include "../include/message_processor_common_20190410.h"

enum class parameter_estimation_role { Alice, Bob, Undefined };

class ParameterEstimation : public Block {

public:

	//##############################################################################################################

	ParameterEstimation(initializer_list<Signal*> InputSig, initializer_list<Signal*> OutputSig) :
		Block(InputSig, OutputSig),
		outputFolderName{ "signals/" },
		reportOutputFileName{ "parameter_estimation_report.txt" },
		bersListFileName{ "parameter_estimation_berList.txt" }
	{};

	ParameterEstimation(initializer_list<Signal*> InputSig, initializer_list<Signal*> OutputSig, string bFolderName) :
		Block(InputSig, OutputSig),
		outputFolderName{ bFolderName + "/" },
		reportOutputFileName{ "parameter_estimation_report.txt" },
		bersListFileName{ "parameter_estimation_berList.txt" }
	{};

	ParameterEstimation(initializer_list<Signal*> InputSig, initializer_list<Signal*> OutputSig, string bFolderName, string bFileName) :
		Block(InputSig, OutputSig),
		outputFolderName{ bFolderName + "/" },
		reportOutputFileName{ bFileName },
		bersListFileName{ "parameter_estimation_berList.txt" }
	{};

	ParameterEstimation(initializer_list<Signal*> InputSig, initializer_list<Signal*> OutputSig, string bFolderName, string bFileName, string bLFileName) :
		Block(InputSig, OutputSig),
		outputFolderName{ bFolderName + "/" },
		reportOutputFileName{ bFileName },
		bersListFileName{ bLFileName }
	{};

	void initialize(void);
	bool runBlock(void);

	//##############################################################################################################

	void setConfidence(double P) { alpha = 1 - P; }
	double const getConfidence(void) { return 1 - alpha; }

	void setNumberOfBitsToUse(const unsigned int nb) { numberOfBits = nb; };
	void setNumberOfBitsRequiredToStart(const unsigned int nbs) { numberOfBitsRequiredToStart = nbs; };
	void setBypassParameterEstimation(const bool bypass) { bypassParameterEstimation = bypass; }
	void setParameterEstimationRole(const parameter_estimation_role role) { parameterEstimationRole = role; }

	long int getInputBits(void) { return totalNumberOfInputBits; }
	long int getOutputBits(void) { return totalNumberOfOutputBits; }
	t_real getAverageEstimatedBER(void) { return averageBER; }

	//##############################################################################################################

private:
	bool bypassParameterEstimation{ false };
	bool waitingForComm{ false };
	parameter_estimation_role parameterEstimationRole{ parameter_estimation_role::Undefined };
	vector <t_integer> inputKey{};
	vector <t_integer> outputKey{};
	vector <t_integer> bitsToUse{};
	vector <t_integer> bitsFromAlice{};
	vector <t_integer> bitsFromBob{};
	unsigned int numberOfBitsRequiredToStart{ 10500 };
	unsigned int numberOfBits{ 500 };
	t_integer bitSamplingSeed{ 1 };
	double acceptionThreshold{ 0.1 };
	t_real threshold{ 0.12 };
	t_real BER{ 1 };
	t_real BERUpperBound{ 1 };
	t_real BERLowerBound{ 0 };

	vector<t_integer> msgInput{};
	vector<t_integer> msgOutput{};
	bool sendingMsg{ false };
	bool finishedSendingMsg{ true };
	bool startedReceivingMsg{ false };
	bool finishedReceivingMsg{ true };
	t_integer numberOfValuesToReceive{ 0 };

	// Report metrics
	bool reportUpdated{ true };
	bool berListUpdated{ true };
	string reportOutputFileName;
	string bersListFileName;
	string outputFolderName;

	t_real averageBER{ 0 };
	t_real highestBER{ 0 };
	t_real lowestBER{ 1 };

	t_real highestBERUpperBound{ 0 };
	t_real lowestBERUpperBound{ 1 };
	t_real highestBERLowerBound{ 0 };
	t_real lowestBERLowerBound{ 1 };

	t_integer numberOfMeasurements{ 0 };
	t_integer totalNumberOfInputBits{ 0 };
	t_integer totalNumberOfOutputBits{ 0 };

	bool doEstimation{ false };

	bool firstPass{ true };

	double coincidences{ 0.0 };
	double receivedBits{ 0.0 };
	double lowestMinorant{ 1e-10 };

	double alpha{ 0.05 };
	double z{ 0 };
//	int m{ 0 };
//	int n{ 0 };
	
	// Private methods
	bool selectBitsToUse(const t_integer seedVal, const int64_t numberOfBits, vector <t_integer> &vectorInputKey, vector <t_integer> &vectorBitsToUse, int64_t intervalSize);
	void updateReportResults(void);
//	bool sendToMsg(vector <t_integer>& data, Signal& signalToMsg, bool& started);
};


#endif

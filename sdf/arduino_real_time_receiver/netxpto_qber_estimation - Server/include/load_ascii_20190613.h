# ifndef PROGRAM_LOAD_ASCII_H_
# define PROGRAM_LOAD_ASCII_H_

# include "netxpto_20180815.h"

enum class delimiter_type {CommaSeperatedValues, ConcatenatedValues};

class LoadAscii : public Block {

public:

	LoadAscii(initializer_list<Signal *> InputSig, initializer_list<Signal *> OutputSig) :Block(InputSig, OutputSig) {};

	void initialize(void);
	bool runBlock(void);

	void setSamplingPeriod(double sPeriod) { samplingPeriod = sPeriod; }
	double const getSamplingPeriod(void) { return samplingPeriod; }

	void setSymbolPeriod(double sPeriod) { symbolPeriod = sPeriod; }
	double const getSymbolPeriod(void) { return symbolPeriod; }

	void setDataType(signal_value_type dType) { dataType = dType; }
	signal_value_type const getDataType(void) { return dataType; }

	void setDelimiterType(delimiter_type delType) { delimiterType = delType; }
	delimiter_type const getDelimiterType(void) { return delimiterType; }

	void setAsciiFileName(string aFileName) { asciiFileName = aFileName; }
	string const getAsciiFileName(void) { return asciiFileName; }

	void setSartColumn(int sColumn) { startColumn = sColumn; }
	int const getStartColumn(void) { return startColumn; }
	
	void setSartLine(int sLine) { startLine = sLine; }
	int const getStartLine(void) { return startLine; }

	void setMultiplyValue(double aMultiply) { auxMultiply = aMultiply; }
	double const getMultiplyValue(void) { return auxMultiply; }

private:

	bool firstTime{ true };

	streampos position = 0;

	//unsigned long int index = 0;

	int sizeOfFile{ 0 };
	int ready{ 0 };
	double samplingPeriod{ 1 };
	double symbolPeriod{ 1 };
	double auxMultiply = 1;
	signal_value_type dataType{ signal_value_type::t_binary };
	delimiter_type delimiterType{ delimiter_type::ConcatenatedValues };
	string asciiFileName{ "InputFile.txt" };

	unsigned long int startColumn = 1;
	unsigned long int startLine = 1;
	unsigned long int auxPosition = 0;

};


# endif
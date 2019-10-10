


# ifndef PROGRAM_INCLUDE_NETXPTO_H_
# define PROGRAM_INCLUDE_NETXPTO_H_

// ####################################################################################################
// #
// # STL header files
// #
// ####################################################################################################



#include <array>
#include <complex>
#include <cctype> 
#include <ctime>
//#include <filesystem> Doesn't recognize this library
#include <fstream>
#include <functional>  
#include <istream>
#include <ostream>
#include <locale>
#include <map>
#include <math.h>
#include <random>
#include <sstream>
#include <vector>
#include <strstream>
#include <String>
#include <list>
#include <Arduino.h>
#include <initializer_list>




// ####################################################################################################
// #
// # Alias for ISO C++ built-in types
// #
// ####################################################################################################

using t_unsigned_long = unsigned long int;
using t_unsigned = unsigned int;
using t_integer = int;


// ####################################################################################################
// #
// # Global constants
// #
// ####################################################################################################

const int MAX_NAME_SIZE = 256;          // Maximum size of names (file names,..)
const t_integer MAX_BUFFER_LENGTH = 10000;    // Maximum Signal buffer length
const int DEFAULT_BUFFER_LENGTH = 512;
const int MAX_TOPOLOGY_SIZE = 100;        // Maximum System topology size 
const int MAX_TAPS = 1000;            // Maximum Taps Number
const double pi = 3.1415926535897932384;    // Value of pi
const double SPEED_OF_LIGHT = 299792458;    // Speed of light in vaccum
const double PLANCK_CONSTANT = 6.626070040e-34; // NIST
const int MAX_NUMBER_OF_PATHS = 2;

using namespace std;              // to be deleted 4/9/2018

// ####################################################################################################
// #
// # Random engine
// #
// ####################################################################################################

//using random_engine = std::default_random_engine;

// ####################################################################################################
// #
// # Alias for NetXpto signal value types
// #
// ####################################################################################################

using t_binary = unsigned int;
using t_integer = int;
using t_real = double;
using t_complex = complex<t_real>;
using t_complex_xy = struct { t_complex x; t_complex y; };
//using t_photon = struct { t_real probabilityAmplitude;  t_real polarization; }; // isto é para eliminar
//using t_photon_mp = struct { t_photon path[MAX_NUMBER_OF_PATHS]; };       // isto é para eliminar
using t_complex_xy_mp = struct { t_complex x; t_complex y; bool processed{ false }; };
using t_photon_mp_xy = struct { t_complex_xy_mp path[MAX_NUMBER_OF_PATHS]; };
using t_iqValues = complex<t_real>;
using t_message = struct { string messageType;  string messageDataLength;   string messageData; int size() { return 3; } };

enum class signal_value_type { t_binary, t_integer, t_real, t_complex, t_complex_xy, t_photon, t_photon_mp, t_photon_mp_xy, t_iqValues, t_message };

// ####################################################################################################
// #
// # NetXpto enuerate class types
// #
// ####################################################################################################

enum class signal_type  { Binary, 
              TimeDiscreteAmplitudeContinuousReal,
              TimeDiscreteAmplitudeDiscreteReal,
              TimeContinuousAmplitudeContinuousReal,
              TimeContinuousAmplitudeDiscreteReal,
              TimeDiscreteAmplitudeContinuousComplex,
              TimeContinuousAmplitudeContinuousComplex,
              OpticalSignal,
              PhotonStreamXY,
              PhotonStreamMP, 
              PhotonStreamMPXY,
              Message
            };

class Signal {

public:

  // Signals constructors
  explicit Signal() {};
  explicit Signal(String fileName) : fileName{ fileName }, saveSignal{ true } {};  // CHANGE: covert t_unsigned_long to t_integer
  explicit Signal(String fileName, t_integer bLength) : fileName{ fileName }, bufferLength{ bLength }, saveSignal{ true } {};
  explicit Signal(String fileName, String folderName) : fileName{ fileName }, folderName{ folderName }, saveSignal{ true } {};
  explicit Signal(String fileName, String folderName, t_integer bLength) : fileName{ fileName }, folderName{ folderName }, bufferLength{ bLength }, saveSignal{ true } {};
  explicit Signal(t_integer bLength) : bufferLength{ bLength } {};

  // Signal destructors
  ~Signal() { /*if (!(valueType == signal_value_type::t_message)) { delete[] buffer; };*/ };  // Descomentar esta parte

  // Buffer manipulation funtions
  t_integer ready();                    // Returns the number of samples in the buffer ready to be processed
  t_integer space();                    // Returns the signal buffer space without originating signal overlap
  template<typename T> void bufferPut(T value);     // Puts a value in the buffer (at the end)
  //void bufferPut(t_message value);        // to be reviewed (2019/03/26)
  template<typename T> void bufferGet(T* valueAddr);    // Returns a value from the buffer (from the begining)  
  //void bufferGet(t_message * valueAddr);        // to be reviewed (2019/03/26)
  void bufferGet();

  // File manipulation
  void writeHeader();               // Opens the signal file in the default signals directory, \signals, and writes the signal header
  //void writeHeader(string signalPath);      // Opens the signal file in the signalPath directory, and writes the signal header

  // Buffer and File manipulation
  void close();                 // Empty the signal buffer and close the signal file

  void reset() { inPosition = 0; outPosition = inPosition; bufferEmpty = true; bufferFull = false; }

  // Change signal parameters
  void setSaveSignal(bool sSignal) { saveSignal = sSignal; };
  bool getSaveSignal() const { return saveSignal; };

  void setType(String sType, signal_value_type vType) { type = sType; valueType = vType; };
  void setType(String sType) { type = sType; };
  String getType() const { return type; };

  void setInPosition(int iPosition) { inPosition = iPosition; };
  int getInPosition() const { return inPosition; };

  void setOutPosition(int oPosition) { outPosition = oPosition; };
  int getOutPosition() const { return outPosition; };

  void setBufferEmpty(bool bEmpty) { bufferEmpty = bEmpty; };
  bool getBufferEmpty() const { return bufferEmpty; };

  void setBufferFull(bool bFull) { bufferFull = bFull; };
  bool getBufferFull() const { return bufferFull; };

  //void setValueType(signal_value_type vType) { valueType = vType; };
  signal_value_type getValueType() const { return valueType; };

  void setFileName(String fName) { fileName = fName; };
  String getFileName() const { return fileName; };

  void setFolderName(String fName) { folderName = fName; };
  String getFolderName() const { return folderName; };

  //void setBufferLength(t_unsigned bLength) { bufferLength = bLength; };
  int getBufferLength() const { return bufferLength; };

  void setFirstValueToBeSaved(t_unsigned_long fValueToBeSaved) { firstValueToBeSaved = fValueToBeSaved; };
  t_unsigned_long getFirstValueToBeSaved() const { return firstValueToBeSaved; };

  void setNumberOfValuesToBeSaved(t_unsigned_long nOfValuesToBeSaved) { numberOfValuesToBeSaved = nOfValuesToBeSaved; };
  t_unsigned_long getNumberOfValuesToBeSaved() const { return numberOfValuesToBeSaved; };

  void setSymbolPeriod(double sPeriod) { symbolPeriod = sPeriod; samplesPerSymbol = symbolPeriod / samplingPeriod; };
  double getSymbolPeriod() const { return symbolPeriod; };

  void setSamplingPeriod(double sPeriod) { samplingPeriod = sPeriod; samplesPerSymbol = symbolPeriod / samplingPeriod; };
  double getSamplingPeriod() const { return samplingPeriod; };

  void setSamplesPerSymbol(double sPeriod) { samplesPerSymbol = sPeriod; symbolPeriod = samplesPerSymbol * samplingPeriod; samplingPeriod = symbolPeriod / samplesPerSymbol; };
  double getSamplesPerSymbol() const { return samplesPerSymbol; };

  void setCentralFrequency(double cFrequency) { centralFrequency = cFrequency; centralWavelength = SPEED_OF_LIGHT / centralFrequency; }
  double getCentralFrequency() const { return centralFrequency; };

  void setCentralWavelength(double cWavelength) { centralWavelength = cWavelength; centralFrequency = SPEED_OF_LIGHT / centralWavelength; }
  double getCentralWavelength() const { return centralWavelength; }

  void setSaveInAscii(bool sInAscii) { saveInAscii = sInAscii; }
  bool getSaveInAscii() const { return saveInAscii; }

  template<typename t, signal_type sType, signal_value_type vType> friend class BaseSignal;

private:

  /* Circular buffer state variables */
  void* buffer{ nullptr };                      // Pointer to buffer
  t_unsigned inPosition{ 0 };                 // Next position for the buffer input values
  t_unsigned outPosition{ 0 };                  // Next position for the buffer output values
  bool bufferEmpty{ true };                     // Flag bufferEmpty
  bool bufferFull{ false };                     // Flag bufferFull
  //const unsigned int bufferLength{ DEFAULT_BUFFER_LENGTH };   // Buffer length
  const int bufferLength{ DEFAULT_BUFFER_LENGTH };

  t_unsigned_long numberOfSavedValues{ 0 };             // Number of saved values
  t_unsigned_long count;                        // Number of values that have already entered in the buffer

  /* Input Parameters */

  t_unsigned_long firstValueToBeSaved{ 1 };           // First value (>= 1) to be saved
  bool saveSignal{ false };

  String type;                      // Signal type
  signal_value_type valueType;              // Signal samples type

  String fileName{ "" };              // Name of the file where data values are going to be saved
  String folderName{ "signals" };         // folder where signals are going to be saved by default
  bool headerWritten{ false };

  bool saveInAscii{ false };
  long int numberOfValuesToBeSaved{ -1 };     // Number of values to be saved, if -1 all values are going to be saved

  double symbolPeriod{ 1.0 };           // Signal symbol period (it is the inverse of the symbol rate)
  double samplingPeriod{ 1.0 };         // Signal sampling period (it is the time space between two samples)
  double samplesPerSymbol{ 1.0 };         // samplesPerSymbol = symbolPeriod / samplingPeriod;

  double centralWavelength{ 1550E-9 };
  double centralFrequency{ SPEED_OF_LIGHT / centralWavelength };
};

template<typename T, signal_type sType, signal_value_type vType>

class BaseSignal : public Signal {

public:

  using value_type = T;

  BaseSignal() : Signal() { setBuffer(); }
  BaseSignal(String fileName) : Signal(fileName) { setBuffer(); }
  BaseSignal(String fileName, t_unsigned_long bLength) : Signal(fileName, bLength) { setBuffer(); }
  BaseSignal(String fileName, String folderName) : Signal(fileName, folderName) { setBuffer(); }
  BaseSignal(String fileName, String folderName, t_unsigned_long bLength) : Signal(fileName, folderName, bLength) { setBuffer(); }
  BaseSignal(t_unsigned_long bLength) : Signal(bLength) { setBuffer(); }

  value_type bufferGet()
  {
    if (bufferEmpty) return value_type();

    value_type value = static_cast<value_type *>(buffer)[outPosition];

    bufferFull = false;

    outPosition = (outPosition + 1) % bufferLength;

    bufferEmpty = outPosition == inPosition;

    return value;
  }
  value_type& operator[](int index) { return (static_cast<value_type*>(buffer)[(outPosition + index) % bufferLength]); }

private:

  void setBuffer() {

    String typeName;
    switch (sType) {
      case signal_type::Binary:
        typeName = "Binary";
        break;
      case signal_type::TimeDiscreteAmplitudeContinuousReal:
        typeName = "TimeDiscreteAmplitudeContinuousReal";
        break;
      case signal_type::TimeContinuousAmplitudeContinuousReal:
        typeName = "TimeContinuousAmplitudeContinuousReal";
        break;
      case signal_type::TimeContinuousAmplitudeDiscreteReal:
        typeName = "TimeContinuousAmplitudeDiscreteReal";
        break;
      case signal_type::TimeDiscreteAmplitudeContinuousComplex:
        typeName = "TimeDiscreteAmplitudeContinuousComplex";
        break;
      case signal_type::TimeContinuousAmplitudeContinuousComplex:
        typeName = "TimeContinuousAmplitudeContinuousComplex";
        break;
      case signal_type::OpticalSignal:
        typeName = "TimeContinuousAmplitudeContinuousComplex";
        break;
      case signal_type::PhotonStreamXY:
        typeName = "PhotonStreamXY";
        break;
      case signal_type::PhotonStreamMP:
        typeName = "PhotonStreamMP";
        break;
      case signal_type::PhotonStreamMPXY:
        typeName = "PhotonStreamMPXY";
        break;
      case signal_type::Message:
        typeName = "Message";
        break;
      default:
        Serial.println("Error: netxpto_20180815.h - typeName not defined");
    }

    setType(typeName, vType);
    if (buffer == nullptr) buffer = (value_type*) (new value_type[getBufferLength()]);
  }
};

using Binary = BaseSignal< t_binary, signal_type::Binary, signal_value_type::t_binary> ;
using TimeDiscreteAmplitudeContinuousReal = BaseSignal<t_real, signal_type::TimeDiscreteAmplitudeContinuousReal, signal_value_type::t_real>;
using TimeDiscreteAmplitudeDiscreteReal = BaseSignal<t_real, signal_type::TimeDiscreteAmplitudeDiscreteReal, signal_value_type::t_real>;
using TimeContinuousAmplitudeContinuousReal = BaseSignal<t_real, signal_type::TimeContinuousAmplitudeContinuousReal, signal_value_type::t_real>;
using TimeContinuousAmplitudeDiscreteReal = BaseSignal<t_real, signal_type::TimeContinuousAmplitudeDiscreteReal, signal_value_type::t_real>;
using TimeDiscreteAmplitudeContinuousComplex = BaseSignal<t_complex, signal_type::TimeDiscreteAmplitudeContinuousComplex, signal_value_type::t_complex>;
using TimeContinuousAmplitudeContinuousComplex = BaseSignal<t_complex, signal_type::TimeContinuousAmplitudeContinuousComplex, signal_value_type::t_complex>;
using OpticalSignal = BaseSignal<t_complex, signal_type::OpticalSignal, signal_value_type::t_complex>;
using PhotonStreamXY = BaseSignal<t_complex_xy, signal_type::PhotonStreamXY, signal_value_type::t_complex_xy>;
//using PhotonStreamMP = BaseSignal<t_photon_mp, signal_type::PhotonStreamMP, signal_value_type::t_photon_mp>;
using PhotonStreamMPXY = BaseSignal<t_photon_mp_xy, signal_type::PhotonStreamMPXY, signal_value_type::t_photon_mp_xy>;
using Message = BaseSignal<t_message, signal_type::Message, signal_value_type::t_message>;


class Block {

public:

  /* Methods */
  Block(){};
  Block(vector<Signal*> &InputSig, vector<Signal*> &OutputSig);
  //Block(initializer_list<Signal*> InputSig, initializer_list<Signal*> OutputSig); Descomentar


  //void initializeBlock(vector<Signal*> InputSig, vector<Signal*> OutputSig);
  void initializeBlock();

  virtual void initialize(void){};

  virtual bool runBlock();

  void terminateBlock();
  virtual void terminate(void){};

  void closeOutputSignals();

  void setNumberOfInputSignals(int nOfInputSignal) { numberOfInputSignals = nOfInputSignal; };
  int getNumberOfInputSignals() { return numberOfInputSignals; };

  void setNumberOfOutputSignals(int nOfOutputSignal) { numberOfOutputSignals = nOfOutputSignal; };
  int getNumberOfOutputSignals() { return numberOfOutputSignals; };

  vector<Signal *> inputSignals;
  vector<Signal *> outputSignals;

  int numberOfInputSignals; // passar para private, 2018/07/15
  int numberOfOutputSignals;  // passar para private, 2018/07/15

};

class System {

public:

  //######################################################################################################
	
  //System() {};
  //void setSystem(initializer_list<Block *> MainSystem); // Descomentar
  void setSystem(vector<Block*> MainSystem); // Apagar

  //System(initializer_list<Block *> MainSystem); // Descomentar
  System(vector<Block*> MainSystem); // Apagar

  //System(initializer_list<Block *> MainSystem, String signalsFolderName, vector<String> list); // Descomentar
  System(vector<Block*> MainSystem, String signalsFolderName, vector<String> list); // Apagar


  bool run();
  bool run(String signalPath);
  void terminate();
  //void terminateSuperBlock();

  //########################################################################################################

  void setLogValue(bool value);
  bool getLogValue() { return logValue; };

  void setOpenFile(bool value);
  bool getOpenFile() { return openFile; };

  void setLogFileName(String newName);
  String getLogFileName() { return logFileName; };

  void setSignalsFolderName(String newName);
  String getSignalsFolderName() { return signalsFolder; };

  //void setSaveSignals(bool sSignals);
  bool const getSaveSignals(void) { return saveSignals; };


  void setLoadedInputParameters(vector<String> loadedInputParams);
  vector<String> getLoadedInputParameters() { return loadedInputParameters; };
  size_t getSystemBlocksSize() { return SystemBlocks.size(); };
  vector<Block *> getSystemBlocks() { return SystemBlocks; };
  
  //########################################################################################################

private:
  String signalsFolder{ "signals" };
  bool saveSignals{ true };

  char fileName[MAX_NAME_SIZE];  // Name of the file with system description (.sdf)
  char name[MAX_NAME_SIZE];  // System Name
  int numberOfBlocks;  // Number of system Blocks
  int(*topology)[MAX_TOPOLOGY_SIZE];  // Relationship matrix
  vector<Block *> SystemBlocks;  // Pointer to an array of pointers to Block objects
                   //Log File Inputs
  String logFileName{ "log.txt" }; // The name of the file where the debug info will be written
  bool logValue{ true }; // Will write debug info if true
  bool openFile{ true };
  ofstream logFile;
  vector<String> loadedInputParameters;
};

# endif // PROGRAM_INCLUDE_netxpto_H_

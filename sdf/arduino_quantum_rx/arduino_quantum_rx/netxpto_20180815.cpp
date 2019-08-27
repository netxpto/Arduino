#include "netxpto_20180815.h"
# include <initializer_list>
# include <initializer_list>

using namespace std;

//########################################################################################################################################################
//######################################################### SIGNALS FUNCTIONS IMPLEMENTATION #############################################################
//########################################################################################################################################################

t_integer Signal::ready(){

 t_integer ready{ bufferLength };  // CHANGE: convert auto to t_integer

 if (!bufferFull)
  {
    if (inPosition >= outPosition)
    {
      ready = inPosition - outPosition;
    }
    else
    {
      ready = bufferLength + inPosition - outPosition;
    }
  }

  return ready;
}

t_integer Signal::space() {

  return (bufferLength - ready());

}

template<typename T>
void Signal::bufferPut(T value)
{
	(static_cast<T*>(buffer))[inPosition] = value;

	if (bufferFull)
	{
		outPosition = (outPosition + 1) % bufferLength;
	}

	inPosition = (inPosition + 1) % bufferLength;

	bufferEmpty = false;
	bufferFull = inPosition == outPosition;

	//if (bufferFull)     // 2019-04-13, de forma a gravar os sinais mesmo quando o buffer não enche
	if (inPosition == 0)
	{
		if (saveSignal)
		{
			if (!headerWritten) writeHeader();
			
		}
		setFirstValueToBeSaved(1);
	}
}
template<typename T>
void Signal::bufferGet(T* valueAddr) {

  if (bufferEmpty) *valueAddr = T();

  *valueAddr = static_cast<T *>(buffer)[outPosition];

  bufferFull = false;

  outPosition = (outPosition + 1) % bufferLength;

  bufferEmpty = outPosition == inPosition;

  return;
}


void Signal::bufferGet() {

  bufferFull = false;

  outPosition = (outPosition + 1) % bufferLength;

  bufferEmpty = outPosition == inPosition;

  return;

}

void Signal::writeHeader(){

  if (headerWritten) return;

  ofstream headerFile;

/*  if (saveSignal && (!fileName.empty())) {

    headerFile.open("./" + folderName + "/" + fileName, ios::out);

    headerFile << "Signal type: " << getType() << "\n";
    headerFile << "Symbol Period (s): " << getSymbolPeriod() << "\n";
    headerFile << "Sampling Period (s): " << getSamplingPeriod() << "\n";

    headerFile << "// ### HEADER TERMINATOR ###\n";

    headerFile.close();

    headerWritten = true;
  }*/
}

void Signal::close() {

  if (saveSignal && (inPosition >= firstValueToBeSaved)) {

    /*if (!headerWritten) writeHeader();

    if (!(type == "Message")) {
      char* ptr = (char *)buffer;

      ofstream fileHandler;
      fileHandler.open("./" + folderName + "/" + fileName, ios::out | ios::binary | ios::app);

      if (type == "Binary") {
        //ptr = ptr + (firstValueToBeSaved - 1) * sizeof(t_binary);
        //fileHandler.write((char *)ptr, (inPosition - (firstValueToBeSaved - 1)) * sizeof(t_binary));
        t_binary *ptr = (t_binary *)buffer;
        ptr = ptr + (firstValueToBeSaved - 1);
        //bool stop {false};
        ofstream fileHandler("./" + folderName + "/" + fileName, ios::out | ios::app);

        for (size_t i = 0; i < inPosition; i++)
        {

          fileHandler << (*ptr);
          fileHandler << " ";

          ptr++;
        }

        //  fileHandler.close();
        setFirstValueToBeSaved(1);
      }
      else if (type == "TimeContinuousAmplitudeContinuousComplex" || type == "BandpassSignal") {
        ptr = ptr + (firstValueToBeSaved - 1) * sizeof(t_complex);
        fileHandler.write((char *)ptr, (inPosition - (firstValueToBeSaved - 1)) * sizeof(t_complex));
      }
      else if (type == "TimeDiscreteAmplitudeDiscreteReal") {
        //ptr = ptr + (firstValueToBeSaved - 1) * sizeof(t_binary);
        //fileHandler.write((char *)ptr, (inPosition - (firstValueToBeSaved - 1)) * sizeof(t_binary));

        t_real *ptr = (t_real *)buffer;
        ptr = ptr + (firstValueToBeSaved - 1);
        //bool stop {false};
        ofstream fileHandler("./" + folderName + "/" + fileName, ios::out | ios::app);

        for (size_t i = 0; i < inPosition; i++)
        {

          fileHandler << (*ptr);
          fileHandler << " ";

          ptr++;
        }

        //  fileHandler.close();
        setFirstValueToBeSaved(1);
      }
      else {
        ptr = ptr + (firstValueToBeSaved - 1) * sizeof(t_real);
        fileHandler.write((char *)ptr, (inPosition - (firstValueToBeSaved - 1)) * sizeof(t_real));
      }
      inPosition = 0;
      fileHandler.close();
    }
    else if (type == "Message") {
      auto fValueToBeSaved = getFirstValueToBeSaved();
      int bLength = getBufferLength();

      if (fValueToBeSaved < (inPosition + 1)) {
        t_message *ptr = (t_message *)buffer;
        ptr = ptr + (fValueToBeSaved - 1);

        ofstream fileHandler("./" + folderName + "/" + fileName, ios::out | ios::app);

        for (auto msg = fValueToBeSaved; msg < (inPosition + 1); msg++) {
          for (auto fld = 0; fld < (*ptr).size(); fld++) {
            fileHandler << ptr->messageType + "\t" + ptr->messageDataLength + "\t" + ptr->messageData + "\n";
          }
          ptr++;
        }
        fileHandler.close();
        setFirstValueToBeSaved(1);
      }
    }*/
  }
  setFirstValueToBeSaved(1);
}

//########################################################################################################################################################
//###################################################### GENERAL BLOCKS FUNCTIONS IMPLEMENTATION #########################################################
//########################################################################################################################################################

Block::Block(initializer_list<Signal*> InputSig, initializer_list<Signal*> OutputSig) {

  numberOfInputSignals = (int) InputSig.size();
  numberOfOutputSignals = (int) OutputSig.size();

  inputSignals = InputSig;
  outputSignals = OutputSig;
}

Block::Block(vector<Signal*> &InputSig, vector<Signal*> &OutputSig) {

  numberOfInputSignals = (int)InputSig.size();
  numberOfOutputSignals = (int)OutputSig.size();

  inputSignals = InputSig;
  outputSignals = OutputSig;
}

void Block::initializeBlock(void) {

  initialize();

}

bool Block::runBlock(void) {

  return false;

}

void Block::terminateBlock(void) {

  terminate();

  for (auto i = 0; i < numberOfInputSignals; i++)
    inputSignals[i]->close();

}

void Block::closeOutputSignals(void) {

  for (auto i = 0; i < numberOfOutputSignals; i++)
    outputSignals[i]->close();

}

//########################################################################################################################################################
//########################################################## GENERIC SYSTEM DECLARATIONS AND DEFINITIONS #################################################
//########################################################################################################################################################



System::System(initializer_list<Block *> Blocks) 
{
  SystemBlocks = Blocks;

  for (int unsigned i = 0; i < SystemBlocks.size(); i++) 
  {
    SystemBlocks[i]->initializeBlock();
  }
}

System::System(initializer_list<Block *> Blocks, String signalsFolderName, vector<String> list)
{

  SystemBlocks = Blocks;
  for (int unsigned i = 0; i < SystemBlocks.size(); i++) {
    SystemBlocks[i]->initializeBlock();
  }
  setSignalsFolderName(signalsFolderName);
  setLoadedInputParameters(list);
}

void System::setSystem(initializer_list<Block *> Blocks)
{

  SystemBlocks = Blocks;

  for (int unsigned i = 0; i < SystemBlocks.size(); i++)
  {
    SystemBlocks[i]->initializeBlock();
  }
}

bool System::run() {
  return run(signalsFolder);
}

bool System::run(String signalPath) {

  bool systemAlive{ false };

  /*2018-04-18*/
  //Creates the signals folder if it doesn't exist
  /*if (!experimental::filesystem::is_directory(signalPath) || !experimental::filesystem::exists(signalPath)) {
    experimental::filesystem::create_directory(signalPath);
  }*/
  //Debug information
  clock_t start;
  String separator = "|";
  /*if (logValue && openFile)
    logFile.open("./" + signalPath + "/" + logFileName);
  //Writes which input parameters have been 
  logFile << "The following input parameters were loaded from the configuration file:" << endl;
  for (String p : loadedInputParameters) {
    logFile << p << endl;
  }
  //logFile << "-------------------------------------------------------" << endl;
*/
  int l = 0;
  bool Alive;
  do {
    Alive = false;
    for (unsigned int i = 0; i < SystemBlocks.size(); i++) {/*
      // Writes debug information
      if (logValue) {
        time_t t_now = time(0);
        struct tm now;
        localtime_s(&now, &t_now);
        char buffer[20];
        snprintf(buffer, 20, "%04d-%02d-%02d %02d:%02d:%02d", 1900 + now.tm_year, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
        logFile << "-----------------------------------------------------------------\n";
        logFile << "########## " << string(typeid(*SystemBlocks[i]).name()).substr(6) << " ##########" << endl; // Prints block name e.g. "Add"
        logFile << "Block start time: " << buffer << endl; 
        // Prints line for each input signal in the current block being executed
        logFile << "Input Signals: " << endl;
        for (Signal *b : SystemBlocks[i]->inputSignals) {
          String filename = (*b).getFileName(); // Gets filename e.g: "S8.sgn"
          logFile << string(typeid(*SystemBlocks[i]).name()).substr(6) << separator // Prints block name e.g. "Add"
            << filename.substr(0, filename.find(".")) << separator // Prints the formated filename e.g. "S8.sgn" becomes "S8"
            << "ready=" << (*b).ready() << endl; // Prints the amount of bits ready to be processed 
        }
        // Prints line for each output signal in the current block being executed
        logFile << "Output Signals: " << endl;
        for (Signal *b : SystemBlocks[i]->outputSignals) {
          String filename = (*b).getFileName(); // Gets filename e.g: "S8.sgn"
          logFile << String(typeid(*SystemBlocks[i]).name()).substr(6) << separator // Prints block name e.g. "Add"
            << filename.substr(0, filename.find(".")) << separator // Prints the formated filename e.g. "S8.sgn" becomes "S8"
            << "space=" << (*b).space() << endl; // Prints the amount of bits ready to be processed 
        }
        start = clock(); //Counts the time taken to run the current block
      }*/

      bool aux = SystemBlocks[i]->runBlock();
      Alive = (Alive || aux);
      if (Alive) systemAlive = true;

      /*if (logValue)
      {
        logFile << "-----------------------------------------------------------------\n";
        logFile << "Elapsed time: " << (float)(clock() - start) << " milliseconds" << endl;
        logFile << "-----------------------------------------------------------------\n";
        // Prints line for each input signal in the current block being executed
        logFile << "Input Signals: " << endl;
        for (Signal *b : SystemBlocks[i]->inputSignals) {
          String filename = (*b).getFileName(); // Gets filename e.g: "S8.sgn"
          logFile << string(typeid(*SystemBlocks[i]).name()).substr(6) << separator // Prints block name e.g. "Add"
            << filename.substr(0, filename.find(".")) << separator // Prints the formated filename e.g. "S8.sgn" becomes "S8"
            << "ready=" << (*b).ready() << endl; // Prints the amount of bits ready to be processed 
        }
        // Prints line for each output signal in the current block being executed
        logFile << "Output Signals: " << endl;
        for (Signal *b : SystemBlocks[i]->outputSignals) {
          String filename = (*b).getFileName(); // Gets filename e.g: "S8.sgn"
          logFile << string(typeid(*SystemBlocks[i]).name()).substr(6) << separator // Prints block name e.g. "Add"
            << filename.substr(0, filename.find(".")) << separator // Prints the formated filename e.g. "S8.sgn" becomes "S8"
            << "space=" << (*b).space() << endl; // Prints the amount of bits ready to be processed 
        }
        logFile << endl << endl;
      }
    */}
    l++;
  } while (Alive);

  return systemAlive;
}

void System::terminate()
{

  for (int unsigned i = 0; i < SystemBlocks.size(); i++) {
    SystemBlocks[i]->terminateBlock();
  }
  //Closes debug file
  if (logValue)
    logFile.close();

}

void System::setOpenFile(bool value) {
  openFile = value;
}

void System::setLogValue(bool value){
  logValue = value;
}

void System::setLogFileName(String newName){
  logFileName = newName;
}

void System::setSignalsFolderName(String newName)
{
  signalsFolder = newName;

  for (auto b : SystemBlocks) {
    for (auto s : b->inputSignals)
    {
      s->setFolderName(signalsFolder);
    }
    for (auto s : b->outputSignals)
    {
      s->setFolderName(signalsFolder);
    }
  }
}

#include "netxpto_20180815.h"
# include <initializer_list>

using namespace std;

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
			/*
			if (!(type == "Message")) {
			  char* ptr = (char *)buffer;

			  ofstream fileHandler;
			  fileHandler.open("./" + folderName + "/" + fileName, ios::out | ios::binary | ios::app);

			  if (firstValueToBeSaved <= bufferLength)
			  {
				if (!saveInAscii)
				{
				  char *ptr = (char *)buffer;
				  ptr = ptr + (firstValueToBeSaved - 1) * sizeof(T);
				  ofstream fileHandler{ "./" + folderName + "/" + fileName, ios::out | ios::binary | ios::app };
				  fileHandler.write(ptr, (bufferLength - (firstValueToBeSaved - 1)) * sizeof(T));
				  fileHandler.close();
				  firstValueToBeSaved = 1;
				}
				else
				{
				  if (type == "Binary") {
					//ptr = ptr + (firstValueToBeSaved - 1) * sizeof(t_binary);
					//fileHandler.write((char *)ptr, (inPosition - (firstValueToBeSaved - 1)) * sizeof(t_binary));

					t_binary *ptr = (t_binary *)buffer;
					ptr = ptr + (firstValueToBeSaved - 1);

					ofstream fileHandler("./" + folderName + "/" + fileName, ios::out | ios::app);
					for (size_t i = 0; i < bufferLength; i++)
					{

					  fileHandler << (*ptr);
					  fileHandler << " ";

					  ptr++;
					}

					//  fileHandler.close();
					setFirstValueToBeSaved(1);
				  }
				  else if (type == "TimeDiscreteAmplitudeDiscreteReal") {
					//ptr = ptr + (firstValueToBeSaved - 1) * sizeof(t_binary);
					//fileHandler.write((char *)ptr, (inPosition - (firstValueToBeSaved - 1)) * sizeof(t_binary));

					t_real *ptr = (t_real *)buffer;
					ptr = ptr + (firstValueToBeSaved - 1);
					//bool stop {false};
					ofstream fileHandler("./" + folderName + "/" + fileName, ios::out | ios::app);

					for (size_t i = 0; i < bufferLength; i++)
					{

					  fileHandler << (*ptr);
					  fileHandler << " ";

					  ptr++;
					}

					//  fileHandler.close();
					setFirstValueToBeSaved(1);
				  }
				}
			  }
			}
			else
			{
			  firstValueToBeSaved = firstValueToBeSaved - bufferLength;
			}
	      */
		}
		setFirstValueToBeSaved(1);
	}
}
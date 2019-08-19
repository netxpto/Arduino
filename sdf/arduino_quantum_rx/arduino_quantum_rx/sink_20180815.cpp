# include "sink_20180815.h"

bool Sink::runBlock(void)
{
  int ready = inputSignals[0]->ready();

  int process;
  if (numberOfSamples >= 0) {
    process = min((long int)ready, numberOfSamples); // Change: std::min() to min()
  }
  else {
    process = ready;
  }
   
  if (process == 0) return false;

  for (int i = 0; i < process; i++) {
    inputSignals[0]->bufferGet();
    ++processedSamples;
  }

  if (numberOfSamples >= 0) numberOfSamples -= process;

  if (displayNumberOfSamples) {    // Change the use of cout to Serial.print() to print in the serial monitor
    Serial.print("Samples to be processed now:");
    Serial.print("\t");
    Serial.println(process);
    Serial.print("Samples to be processed later:");
    Serial.print("\t");
    Serial.println(numberOfSamples);
    Serial.print("Samples already processed:");
    Serial.print("\t");
    Serial.println(processedSamples);
  }

  return true;
}

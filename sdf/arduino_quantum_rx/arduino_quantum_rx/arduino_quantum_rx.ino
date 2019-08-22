#include "netxpto_20180815.h"
#include "qber_20190418.h"
#include "binary_source_20180815.h"
#include "coincidence_detector_20190319.h"
#include "sink_20180815.h"
    
//########### Just for tests ############
String spd0 = "1";
String spd1 = "0";
//#######################################


void setup() {
  Serial.begin(9600);
}

int main()
{
  /* Signals Declaration */
  Binary SPD0_out{ "SPD0_out.sgn"};
  SPD0_out.setSaveSignal(false);

  Binary SPD1_out{ "SPD1_out.sgn"};
  SPD1_out.setSaveSignal(false);

  TimeDiscreteAmplitudeDiscreteReal CD_out{ "CD_out.sgn"};
  CD_out.setSaveSignal(false);

  Binary Alice_out{ "Alice_out.sgn"};
  Alice_out.setSaveSignal(false);

  Binary QBER_out{ "QBER_out.sgn"};
  QBER_out.setSaveSignal(false);

  /* Blocks Decalration */
  BinarySource BinarySource0_{ {},{ &SPD0_out} };
  //BinarySource0_.setBitStream(s);
  //BinarySource0_.setNumberOfBits(10000);
  //BinarySource0_.setMode(BinarySourceMode::DeterministicCyclic);

  //System MainSystem{
    // BLOCKS
    //&BinarySource0_

  //};

  /*MainSystem.run();
  MainSystem.terminate();
  

  system("pause");*/

  return 0;
}

void loop() {
  // put your main code here, to run repeatedly:

}

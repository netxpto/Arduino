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
  /*BinarySource BinarySource0_{ {},{SPD0_out} };
  BinarySource0_.setBitStream(spd0);
  BinarySource0_.setNumberOfBits(10000);
  BinarySource0_.setMode(BinarySourceMode::DeterministicCyclic);

  BinarySource BinarySource1_{ {},{ &SPD1_out} };
  BinarySource1_.setBitStream(spd1);
  BinarySource1_.setNumberOfBits(10000);
  BinarySource1_.setMode(BinarySourceMode::DeterministicCyclic);

  BinarySource BinarySource2_{ {},{ &Alice_out} };
  BinarySource2_.setBitStream(spd0);
  BinarySource2_.setNumberOfBits(10000);
  BinarySource2_.setMode(BinarySourceMode::DeterministicCyclic);*/

  //CoincidenceDetector CoincidenceDetector_{ {&SPD0_out, &SPD1_out},{&CD_out} };

  /*Qber QBER_{ {&Alice_out, &CD_out}, {&QBER_out} };


  Sink Sink_QBER_{ { &QBER_out },{} };
  Sink_QBER_.setDisplayNumberOfSamples(true);*/


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

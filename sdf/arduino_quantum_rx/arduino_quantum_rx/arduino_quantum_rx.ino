#include "netxpto_20180815.h"
#include "qber_20190418.h"
#include "binary_source_20180815.h"
#include "coincidence_detector_20190319.h"
#include "sink_20180815.h"

#include<Wire.h>      //Wire library to use I2C functions 

#define Addr 0x56     // AD5669 I2C address is 0x56(86) (DAC)

String s = "1";
String s1 = "0";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // Initialise I2C communication as Master
  Wire.begin();
}

int main()
{
  /* Signals Declaration */
  Binary SPD0_out{ "SPD0_out.sgn"};

  Binary SPD1_out{ "SPD1_out.sgn"};

  TimeDiscreteAmplitudeDiscreteReal CD_out{ "CD_out.sgn"};

  Binary Alice_out{ "Alice_out.sgn"};

  Binary QBER_out{ "QBER_out.sgn"};

  /* Blocks Decalration */
  BinarySource BinarySource0_{ {},{ &SPD0_out} };
  //BinarySource0_.setBitStream(s);
  //BinarySource0_.setNumberOfBits(10000);
  //BinarySource0_.setMode(BinarySourceMode::DeterministicCyclic);

  /*System MainSystem{
    // BLOCKS
    &BinarySource0_

  };*/

  /*MainSystem.run();
  MainSystem.terminate();
  

  system("pause");*/

  return 0;
}

void loop() {
  // put your main code here, to run repeatedly:

}

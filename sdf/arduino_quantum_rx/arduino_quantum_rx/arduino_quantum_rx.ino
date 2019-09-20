#include <HID.h>
#include <SPI.h>
#include "netxpto_20180815.h"
#include "binary_source_20180815.h"
#include "coincidence_detector_20190319.h"
#include "sink_20180815.h"
#include "clock_20171219.h"
#include "ip_tunnel_ms_windows_20180815.h"
#include "message_processor_common_20190410.h"
<<<<<<< HEAD




=======
>>>>>>> 37855f1d0b3eba88c8f5b3ee3e0c9155928dbe15
    
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
	Binary SPD0_out{ "SPD0_out.sgn" };
	SPD0_out.setSaveSignal(false);

	Binary SPD1_out{ "SPD1_out.sgn" };
	SPD1_out.setSaveSignal(false);

	Binary BobData_In{ "BobData_In.sgn" };
	BobData_In.setSaveInAscii(true);

	TimeDiscreteAmplitudeDiscreteReal clock_out{ "clock_out.sgn" };
	clock_out.setSaveInAscii(true);

	Binary IPTunnel_Out{ "IPTunnel_Out.sgn" };


	/* Blocks Decalration */
<<<<<<< HEAD
	BinarySource BinarySource0_{{}, { &SPD0_out }};
=======
	BinarySource BinarySource0_{{}, { SPD0_out }};
>>>>>>> 37855f1d0b3eba88c8f5b3ee3e0c9155928dbe15
  BinarySource0_.setBitStream(spd0);
  BinarySource0_.setMode(BinarySourceMode::DeterministicCyclic);
  /*
  BinarySource BinarySource1_{ {},{ &SPD1_out} };
  BinarySource1_.setBitStream(spd1);
  BinarySource1_.setNumberOfBits(10000);
  BinarySource1_.setMode(BinarySourceMode::DeterministicCyclic);

  BinarySource BinarySource2_{ {},{ &Alice_out} };
  BinarySource2_.setBitStream(spd0);
  BinarySource2_.setNumberOfBits(10000);
  BinarySource2_.setMode(BinarySourceMode::DeterministicCyclic);

  CoincidenceDetector CoincidenceDetector_{ {&SPD0_out, &SPD1_out},{&CD_out} };

  Qber QBER_{ {&Alice_out, &CD_out}, {&QBER_out} };


  Sink Sink_QBER_{ { &QBER_out },{} };
  Sink_QBER_.setDisplayNumberOfSamples(true);*/


  /*System MainSystem{
    // BLOCKS
    //&BinarySource0_

  };

  MainSystem.run();
  MainSystem.terminate();
  
  */
  system("pause");
  
  return 0;
}

void loop() {
  // put your main code here, to run repeatedly:

}

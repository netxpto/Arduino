#include <Wire.h>
#include <string.h>

const byte interruptPin_Trigger = 26;

long int countT = 0;
int flag1 = 0;
int flag2 = 0;

volatile bool stateT = false;
volatile bool state2 = false;
volatile bool state3 = false;
#define Addr 0x56


void setup() {
  Wire.begin();
  Wire.setClock(4000000);
  Serial.begin(115200);
  pinMode(interruptPin_Trigger, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin_Trigger), triggerFunction , RISING);
}

char b;

int Voltages_1[8] = {255, 127, 63, 31};
int Voltages_0[8] = {255, 127, 63, 31};

bool state = true;
unsigned int dataA[2] = {0x00, 0x00};
unsigned int dataB[2] = {0x00, 0x00};
unsigned int dataC[2] = {0x00, 0x00};
unsigned int dataD[2] = {0x00, 0x00};
int idx = 0;


void loop() {

  if (stateT) {
    stateT = false;

    b = Serial.read();

    if (b == 'U') {
      for (int i = 0; i < 8; i++)
      {
        Voltages_1[i] = Serial.read();
      }
      for (int i = 0; i < 8; i++)
      {
        Voltages_0[i] = Serial.read();
      }
      b = Serial.read();
    }


    if (b) {
      dataA[0] = Voltages_1[0];
      dataA[1] = Voltages_1[4];
      dataB[0] = Voltages_1[1];
      dataB[1] = Voltages_1[5];
      dataC[0] = Voltages_1[2];
      dataC[1] = Voltages_1[6];
      dataD[0] = Voltages_1[3];
      dataD[1] = Voltages_1[7];
    } else {
      dataA[0] = Voltages_0[0];
      dataA[1] = Voltages_0[4];
      dataB[0] = Voltages_0[1];
      dataB[1] = Voltages_0[5];
      dataC[0] = Voltages_0[2];
      dataC[1] = Voltages_0[6];
      dataD[0] = Voltages_0[3];
      dataD[1] = Voltages_0[7];
    }

    Wire.beginTransmission(Addr);
    Wire.write(0b00100000);
    Wire.write(dataA[0]);
    Wire.write(dataA[1]);
    Wire.endTransmission();

    Wire.beginTransmission(Addr);
    Wire.write(0b00100001);
    Wire.write(dataB[0]);
    Wire.write(dataB[1]);
    Wire.endTransmission();

    Wire.beginTransmission(Addr);
    Wire.write(0b00100010);
    Wire.write(dataC[0]);
    Wire.write(dataC[1]);
    Wire.endTransmission();

    Wire.beginTransmission(Addr);
    Wire.write(0b00100011);
    Wire.write(dataD[0]);
    Wire.write(dataD[1]);
    Wire.endTransmission();
  }
}



void triggerFunction()
{
  stateT = true;
}

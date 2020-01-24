#include <Wire.h>

const byte interruptPin_Trigger = 26;
const byte interruptPin_Detect1 = 27;
const byte interruptPin_Detect2 = 28;

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
  pinMode(interruptPin_Detect1, INPUT);
  pinMode(interruptPin_Detect2, INPUT);

  attachInterrupt(digitalPinToInterrupt(interruptPin_Trigger), triggerFunction , RISING);
  attachInterrupt(digitalPinToInterrupt(interruptPin_Detect1), triggerFunction2, RISING);
  attachInterrupt(digitalPinToInterrupt(interruptPin_Detect2), triggerFunction3, RISING);
}

int incomingByte1 = 0;
int incomingByte2 = 0;
int incomingByte3 = 0;

bool state = true;
unsigned int dataA[2] = {0x00, 0x00};
unsigned int dataB[2] = {0x00, 0x00};
unsigned int dataC[2] = {0x00, 0x00};
unsigned int dataD[2] = {0x00, 0x00};

void loop() {
  if (stateT) {
    

    if (state2 && state3) Serial.print("2");
    else if (state2 && !state3) Serial.print("0");
    else if (!state2 && state3) Serial.print("1");
    else if (!state2 && !state3) Serial.print("3");
    stateT = false;
    state2 = false;
    state3 = false;

    incomingByte1 = Serial.read();
    incomingByte2 = Serial.read();
    incomingByte3 = Serial.read();
    
    if (state) {
      dataA[0] = 0x00;
      dataA[1] = 0x00;
      
      dataB[0] = 0x00;
      dataB[1] = 0x00;
      
      dataC[0] = 0x00;
      dataC[1] = 0x00;
      
      dataD[0] = 0x00;
      dataD[1] = 0x00;
      state = false;
    } else {
      dataA[0] = 0xff;
      dataA[1] = 0x00;
      
      dataB[0] = 0x7f;
      dataB[1] = 0x00;
      
      dataC[0] = 0x3f;
      dataC[1] = 0x00;
      
      dataD[0] = 0x1f;
      dataD[1] = 0x00;
      state = true;
    }
    
    // Start I2C transmission

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
void triggerFunction2()
{
  state2 = true;
}
void triggerFunction3()
{
  state3 = true;
}

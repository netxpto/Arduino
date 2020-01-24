#include <Wire.h>
#include <string.h>

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
}

String a;
String b;
int incomingByte1 = 0;
int incomingByte2 = 0;
int incomingByte3 = 0;
int incomingByte4 = 0;
int count = 0;

bool state = true;
unsigned int dataA[2] = {0x00, 0x00};
unsigned int dataB[2] = {0x00, 0x00};
unsigned int dataC[2] = {0x00, 0x00};
unsigned int dataD[2] = {0x00, 0x00};

void loop() {
  while (Serial.available() > 0) {
    a = Serial.readStringUntil('\n');
  Serial.println(a);
  }
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
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

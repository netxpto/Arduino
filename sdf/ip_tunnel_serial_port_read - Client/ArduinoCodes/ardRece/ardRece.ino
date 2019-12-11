
const byte interruptPin_Trigger = 26;
const byte interruptPin_Detect1 = 27;
const byte interruptPin_Detect2 = 28;

long int countT = 0;
int flag1 = 0;
int flag2 = 0;

volatile bool stateT = false;
volatile bool state2 = false;
volatile bool state3 = false;


void setup() {
  Serial.begin(115200);
  pinMode(interruptPin_Trigger, INPUT);
  pinMode(interruptPin_Detect1, INPUT);
  pinMode(interruptPin_Detect2, INPUT);

  attachInterrupt(digitalPinToInterrupt(interruptPin_Trigger), triggerFunction , RISING);
  attachInterrupt(digitalPinToInterrupt(interruptPin_Detect1), triggerFunction2, RISING);
  attachInterrupt(digitalPinToInterrupt(interruptPin_Detect2), triggerFunction3, RISING);
}




void loop() {
  if (stateT) {
    if (state2 && state3) Serial.print("3");
    else if (state2 && !state3) Serial.print("0");
    else if (!state2 && state3) Serial.print("1");
    else if (!state2 && !state3) Serial.print("2");
    stateT = false;
    state2 = false;
    state3 = false;
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

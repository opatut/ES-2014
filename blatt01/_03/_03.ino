#define LED 7
#define BUTTON1 3

bool ledOn = false;
bool buttonLock = false;

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(BUTTON1, INPUT);
  attachInterrupt(BUTTON1, buttonPressed, RISING);
}

void buttonPressed() {
  if(!buttonLock) {
    ledOn = !ledOn;
    buttonLock = true;
  }
}

void loop() {
  digitalWrite(LED, ledOn);
  
  if(digitalRead(BUTTON1)) {
    buttonLock = false;
  }
  delay(50);
}

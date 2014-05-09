
#define PIN_BTN 3
#define PIN_LED 13

bool buttonWasDown = false;

int blinksRemaining = 0;
int blinkMillis = 0;

void button_pressed() {
  // send toggle signal
  Serial.println("Sending control byte");
  Serial1.write(168);
  Serial1.flush();
}

void signal_received() {
  Serial.println("Received control byte");
  blinksRemaining = 4;
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(PIN_BTN, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
}

void loop() {
  bool buttonDown = !digitalRead(PIN_BTN);
  if(!buttonWasDown && buttonDown) {
     button_pressed();
  }
  buttonWasDown = buttonDown;
  
  // do receiving stuff
  if(Serial1.available()) {
    // guess they sent us stuff, receive all of it and blink
    while(Serial1.available()) Serial1.read();
    signal_received();
  }
  
  // blink
  if(blinksRemaining > 0) {
    blinkMillis -= 50;
    if(blinkMillis <= 0) {
      blinkMillis += 200;
      blinksRemaining--;
    }
    bool led = blinkMillis > 100;
    digitalWrite(PIN_LED, !led);
  }
  
  delay(50);
}

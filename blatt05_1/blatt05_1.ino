#define XB Serial1

#define PARTNER_ADDRESS_LOW "40A099CD"
#define PARTNER_ADDRESS_HIGH "13A200"

#define PIN_BTN 8
#define PIN_MYLED 2

String myAddressLow;
String myAddressHigh;
String myNodeId;

int ledBrightness = 0;
int moving = 0;

//bool buttonWasDown = false;
//int blinksRemaining = 0;
//int blinkMillis = 0;

/* MODE SETTING */

bool isConfiguringMode = false;

void ensureConfigMode(bool configuration) {
  if(configuration) {
    modeConfigure();
  } else {
    modeCommunicate();
  }
}

void modeConfigure() {
  if(!isConfiguringMode) {
    empty();
    XB.print("+++");
    isConfiguringMode = true;
    Serial.println("Entering configuration mode ...");
    delay(1500);
    expectOK();
  }
}

void modeCommunicate() {
  if(isConfiguringMode) {
    XB.println("ATCN");
    isConfiguringMode = false;
    
    Serial.println("Leaving configuration mode ...");
    delay(500);
    
    expectOK();
    empty();
  }
}

/* READ REGISTER */
String readRegister(String id) {
  ensureConfigMode(true);
  XB.println("AT" + id);
  
  Serial.println("Reading register " + id + " ...");
  delay(500);
  
  int count = XB.available() - 1; // do not read the newline
  char* result = new char[count+1];
  XB.readBytes(result, count); // read our content
  empty();
  
  result[count] = 0; // terminate string
  return result;
}

void writeRegister(String id, String value) {
  ensureConfigMode(true);
  XB.println("AT" + id + value);
  
  Serial.println("Writing register " + id + " ...");
  delay(500);
  
  expectOK();
}

// read trash
void empty() {
  while(XB.available()) {
    byte x = XB.read();
    Serial.println("TRASH: " + (char)x + " (" + x + ")");
  }
}

void expectOK() {
  char result[2];
  XB.readBytes(result, 2);
  empty();
  
  if(result[0] == 'O' && result[1] == 'K') {
    Serial.println("OK!");
  } else {
    Serial.print("Did not receive OK: ");
    Serial.println(result);
  }
}

/* ADDRESS MANAGEMENT */

void getMyAddress() {
  ensureConfigMode(true);
  myAddressLow = readRegister("SL");
  myAddressHigh = readRegister("SH");  
  Serial.println("My address: " + myAddressHigh + " " + myAddressLow);
  
  myNodeId = readRegister("NI");
  Serial.println("Node ID: " + myNodeId);
}

void setDestinationAddress(String high, String low) {
  ensureConfigMode(true);
  writeRegister("DH", high);
  writeRegister("DL", low);
  
  Serial.println("Destination address set: " + readRegister("DH") + readRegister("DL"));
}

/* LED/BUTTON STUFF */

void button_pressed() {
  // send toggle signal
  Serial.println("Sending control byte");
  ensureConfigMode(false);
  XB.println("1");
}

void signal_received() {
  Serial.println("Received control byte");
  if(moving != 0) return;
  moving = (ledBrightness == 0 ? 1 : -1);
  //blinksRemaining = 4;
}

void fading_done() {
  Serial.println("Finished fading");
  ensureConfigMode(false);
  XB.write("1");
}

void setup() {  
  pinMode(PIN_BTN, INPUT_PULLUP);
  pinMode(PIN_MYLED, OUTPUT);
  analogWrite(PIN_MYLED, 200);
  
  Serial.begin(9600);
  XB.begin(9600);
  while(XB.read() != -1); // read the data trash
  Serial.println("Initializing ...");
  delay(1500);
  getMyAddress();
  setDestinationAddress(PARTNER_ADDRESS_HIGH, PARTNER_ADDRESS_LOW);
}

void loop() {
  ensureConfigMode(false);
  if(XB.available()) {
    while(XB.read() != -1);
    signal_received();
  }
  
  if(moving != 0) {
    ledBrightness += moving;
    if(ledBrightness == 255 || ledBrightness == 0) {
      moving = 0;
      fading_done();
    }
  }  
  Serial.print(moving);
  Serial.print(" ");
  Serial.println(ledBrightness);
  
  analogWrite(PIN_MYLED, ledBrightness);
  /*
  bool buttonDown = !digitalRead(PIN_BTN);
  if(!buttonWasDown && buttonDown) {
     button_pressed();
  }
  buttonWasDown = buttonDown;
  
  // blink
  if(blinksRemaining > 0) {
    blinkMillis -= 50;
    if(blinkMillis <= 0) {
      blinkMillis += 200;
      blinksRemaining--;
    }
    bool led = blinkMillis > 100;
    digitalWrite(PIN_LED, !led);
  } */

  
  //delay(50);
}

#define XB Serial1

#define PARTNER_ADDRESS_LOW "40A4D71A"
#define PARTNER_ADDRESS_HIGH "13A200"

#define PIN_BTN 8
#define PIN_LED 13

String myAddressLow;
String myAddressHigh;
String myNodeId;

bool buttonWasDown = false;

int blinksRemaining = 0;
int blinkMillis = 0;

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
  while(XB.read() != -1); // read the data trash
    
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

void expectOK() {
  char result[2];
  XB.readBytes(result, 2);
  while(XB.read() != -1); // read the data trash
  
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
  blinksRemaining = 4;
}

void setup() {  
  pinMode(PIN_BTN, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  
  Serial.begin(9600);
  XB.begin(9600);
  while(XB.read() != -1); // read the data trash
  Serial.println("Initializing ...");
  delay(1500);
  getMyAddress();
  setDestinationAddress(PARTNER_ADDRESS_HIGH, PARTNER_ADDRESS_LOW);
}

void loop() {
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
  }

  
  delay(50);
}

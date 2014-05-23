#define XB Serial2

#define PIN_BTN 8
#define PIN_MYLED 2

String myAddressLow;
String myAddressHigh;
String myNodeId;

int ledBrightness = 0;
int moving = 0;

String high = "13A200";
String lows[] = {"40A0981C", "40A09875", "40A0988F", "40A4D7D0", "40A4D6FA", "40A4D7CBB"};
int lows_size = 6;
// me "40A099CD", 

String inputBuffer = "";
String receiveBuffer = "";

bool debug = false;

/* MODE SETTING */

bool isConfiguringMode = false;


// read trash
void empty() {
  while(XB.available()) {
    byte x = XB.read();
    if(debug) {
      Serial.print("TRASH: ");
      Serial.print((char)x);
      Serial.print(" (");
      Serial.print(x);
      Serial.println(")");
    }
  }
}

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
    delay(1500);
    XB.print("+++");
    isConfiguringMode = true;
    if(debug) Serial.println("Entering configuration mode ...");
    delay(1500);
    expectOK();
  }
}

void modeCommunicate() {
  if(isConfiguringMode) {
    empty();
    XB.println("ATCN");
    isConfiguringMode = false;
    
    if(debug) Serial.println("Leaving configuration mode ...");
    delay(500);
    
    expectOK();
    empty();
  }
}

/* READ REGISTER */
String readRegister(String id) {
  ensureConfigMode(true);
  XB.println("AT" + id);
  
  if(debug) Serial.println("Reading register " + id + " ...");
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
  
  if(debug) Serial.println("Writing register " + id + " ...");
  delay(500);
  
  expectOK();
}

void expectOK() {
  char result[2];
  XB.readBytes(result, 2);
  empty();
    
  if(result[0] == 'O' && result[1] == 'K') {
    if(debug) Serial.println("OK!");
  } else {
    Serial.print("Did not receive OK: ");
    Serial.println(result);
  }
}

/* ADDRESS MANAGEMENT */

void getMyAddress() {
  ensureConfigMode(true);
  
  //myAddressLow = readRegister("SL");
  //myAddressHigh = readRegister("SH");  
  //Serial.println("My address: " + myAddressHigh + " " + myAddressLow);
  
  }

void setDestinationAddress(String high, String low) {
  ensureConfigMode(true);
  writeRegister("DH", high);
  writeRegister("DL", low);
  
  Serial.println("Destination address set: " + readRegister("DH") + readRegister("DL"));
}

/* LED/BUTTON STUFF */

void sendMessageTo(String msg, String low) {
  ensureConfigMode(true);
  writeRegister("DL", low);

  ensureConfigMode(false);
  XB.println(myNodeId + "#" + msg);
  empty();
  delay(500);
  if(debug) Serial.println("Sent message to " + low);
}

void sendMessage(String msg) {
  Serial.println("> " + msg);
  for(int i = 0; i < lows_size; ++i) {
    sendMessageTo(msg, lows[i]);
  }
}

void receiveMessage(String msg) {
  String node = strtok(&msg[0], "#");
  String text = strtok(NULL, "\n");
  Serial.println("[" + node + "] " + text);
}

void setup() {  
  Serial.begin(9600);
  XB.begin(9600);
  empty();
    
  Serial.println("Initializing ...");
  delay(1500);
  
  ensureConfigMode(true);
  
  // read node id
  myNodeId = readRegister("NI");
  if(debug) Serial.println("Node ID: " + myNodeId);
  
  // write high destination address
  writeRegister("DH", high);

  Serial.println("Welcome back, " + myNodeId);

  // send hello world message
  sendMessage("Hello World from Paul");
}

void loop() {
  ensureConfigMode(false);
  if(XB.available()) {
    while(XB.available()) {
      char c = (char)XB.read();
      if(c == '\n') {
        receiveMessage(receiveBuffer);
        receiveBuffer = ""; 
      } else {
        receiveBuffer += c;
      }
    }
  }
  
  if(Serial.available()) {
    while(Serial.available()) {
      char c = (char)Serial.read();
      if(c == '\n') {
        sendMessage(inputBuffer);
        inputBuffer = "";
      } else {
        inputBuffer += c;
      }  
    }
  }
}

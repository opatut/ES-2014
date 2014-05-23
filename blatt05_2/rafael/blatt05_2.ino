#define XB Serial1

#define ADDRESS_HIGH "13A200"

String myAddressLow;
String myAddressHigh;
String myNodeId;
String currentAddress;

String addresses[] = {"40A0988F", "40A0981C", "", "", "40A4D7D0", "", "40A099CD", "", "", "", ""};

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

/* REGISTER MANAGEMENT */

String readRegister(String id) {
  ensureConfigMode(true);
  XB.println("AT" + id);
  
  //Serial.println("Reading register " + id + "... ");
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
  
  Serial.print("Writing register " + id + "... ");
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

void setDestinationAddress(String low) {
  if (currentAddress != low) {
    ensureConfigMode(true);
    writeRegister("DH", ADDRESS_HIGH);
    writeRegister("DL", low);
    
    currentAddress = readRegister("DL");
  
    Serial.println("Destination address changed: " + readRegister("DH") + " " + currentAddress);
  }
}

void sendMessage(int node, String msg) {
  String address = addresses[node-1];
  if (address == "") {
    Serial.println("Address for node  " + node + " unknown :(");
    return;
  }
  setDestinationAddress(addresses[node-1]);
  ensureConfigMode(false);
  XB.println(myNodeId + "#" + msg);
  Serial.println("NODE_" + (String)node + " > " + msg);
}

void setup() {  
  Serial.begin(9600);
  XB.begin(9600);
  while(XB.read() != -1); // read the data trash
  delay(1500);
  getMyAddress();
}

void loop() {
  if (XB.available()) {
    ensureConfigMode(false);
    String result;
    while (XB.available()) {
      result += (char)XB.read();
      delay(20);
    }
    bool valid = (result.substring(0, 5) == "NODE_");
    if (valid) {
      String node = result.substring(0, 7);
      String msg = result.substring(8, result.length());
      Serial.print("["+node+"] " + msg);
    } else {
      Serial.println("Invalid message: " + result);
    }
  }
  
  if (Serial.available()) {
     String msg = "";
     while(Serial.available()) {
       msg += (char)Serial.read();
       delay(20);
     }
     int node = msg.substring(0,2).toInt();
     sendMessage(node, msg.substring(3, msg.length()));
  }
  
  delay(50);
}

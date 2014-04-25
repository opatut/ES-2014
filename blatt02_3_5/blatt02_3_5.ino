#include <Servo.h>

int PIN_BTN1 = 9; 
int PIN_BTN2 = 8;
int PIN_SERVO = 2;

Servo servo;

float strength = 0.f;
float globalTime = 0.f;
int lastTime = 0;

float maxAngle = 65;
float angle = 0;

int buf = 0;   // for incoming serial data
String cmdBuffer = "";

void setup() {
  Serial.begin(9600);
  Serial.println("HI");
  servo.attach(PIN_SERVO);
}

void runCommand(String cmd) {
  cmd.trim();
  if(cmd.substring(0, 7) == "moveTo(") {
    cmd = cmd.substring(7);
    int br = cmd.indexOf(')');
    if(br != -1) {
      String num = cmd.substring(0, br);
      char buf[32];
      num.toCharArray(buf, sizeof(buf));
      int angle = atoi(buf);
      setAngle(angle);
      return;
    }
  }
  Serial.println("Syntax error");
}

void setAngle(int a) {
  if(abs(a) < maxAngle) {
    angle = a;
    
    Serial.print("Setting angle to ");
    Serial.println(angle);
  } else {
    Serial.print("Error: Angle too big, maximum value is ");
    Serial.println(maxAngle);
  }
}

void loop() {
  // send data only when you receive data:
  while (Serial.available() > 0) {
    // read the incoming byte:
    buf = Serial.read();
    
    if(buf == (int)'\n') {
      // newline was pressed, run command and reset
      Serial.println("> " + cmdBuffer);
      runCommand(cmdBuffer);
      cmdBuffer = "";
    } else {
      cmdBuffer.concat((char) buf);
    }
  }
 
  servo.write(90 + angle);
}

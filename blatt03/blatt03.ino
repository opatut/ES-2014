#include <Servo.h>

 
int PIN_X = 0;
int PIN_Z = 1;
int PIN_REF = 2;
int PIN_SERVO = 10;
float SENSITIVITY = 9.1;

float xangle = 0;
float zangle = 0;
float servo_angle = 0;

int last_time;

Servo servo;

void setup() {
  Serial.begin(9600);
  last_time = millis();
  servo.attach(PIN_SERVO);
}

void loop() {
  // measure time
  int time = millis();
  int dt_millis = last_time - time;
  float dt = dt_millis * 0.001;
  last_time = time;
  
  int ref = analogRead(PIN_REF);
  int xval = analogRead(PIN_X);
  int zval = analogRead(PIN_Z);
  
  float vRef = ref * 4.9;
  float vX = xval * 4.9;
  float vZ = zval * 4.9;

  float xrot = (vX - vRef) / SENSITIVITY;
  float zrot = (vZ - vRef) / SENSITIVITY;
  
  xangle += xrot * dt;
  zangle += zrot * dt;
  
  // limit servo angle
  servo_angle += zrot * dt;
  if(servo_angle > 65) servo_angle = 65;
  if(servo_angle < -65) servo_angle = -65;
  
  Serial.print(xangle);
  Serial.print(" ");
  Serial.print(zangle);
  Serial.println();
  
  servo.write(90 + servo_angle);
}

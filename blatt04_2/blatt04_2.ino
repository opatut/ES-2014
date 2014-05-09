#include <Wire.h>

#define WIRE Wire

int PIN_X = 0;
int PIN_Z = 1;
int PIN_REF = 2;
float SENSITIVITY = 2.0;

float xangle = 0;
float zangle = 0;
float servo_target_angle = 0;
float servo_angle = 0;

float calib_x = 0;
float calib_z = 0;
bool calibrated = false;

int last_time;
float globalTime = 0;

void loop() {
  // measure time
  int time = millis();
  int dt_millis = time - last_time;
  float dt = dt_millis * 0.001;
  last_time = time;
  
  globalTime += dt;
  
  int ref = analogRead(PIN_REF);
  int xval = analogRead(PIN_X);
  int zval = analogRead(PIN_Z);
  
  float vRef = ref * 4.9;
  float vX = xval * 4.9;
  float vZ = zval * 4.9;

  float xrot = (vX - vRef) / SENSITIVITY;
  float zrot = (vZ - vRef) / SENSITIVITY;
  
  xangle += (xrot - calib_x) * dt;
  zangle += (zrot - calib_z) * dt;
  
  if(globalTime < 1.0) {
    return;
  }
 
  if(!calibrated) {
    calibrated = true;
    calib_x = xangle;
    calib_z = zangle;
  } 
  
  // limit servo angle
  servo_target_angle += (zrot - calib_z) * dt;
  if(servo_target_angle > 65) servo_target_angle = 65;
  if(servo_target_angle < -65) servo_target_angle = -65;
  
  float servo_speed = 20.f;
  servo_angle = servo_angle * (1 - dt * servo_speed) + servo_target_angle * dt * servo_speed;
}




void setup() {
  last_time = millis();
  WIRE.begin(2);
  WIRE.onRequest(requestEvent);
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(9600);
  Serial.println("Started");
}

void requestEvent() {
  int a = 90 + (int)servo_angle;
  WIRE.write((byte)a);
  Serial.print("Sending angle: ");
  Serial.println(a);
}

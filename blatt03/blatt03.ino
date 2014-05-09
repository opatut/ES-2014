#include <Servo.h>
 
int PIN_X = 0;
int PIN_Z = 1;
int PIN_REF = 2;
int PIN_SERVO = 10;
float SENSITIVITY = 2.0;

float xangle = 0;
float zangle = 0;
float servo_angle = 0;
float xrot_avg = 0;
float zrot_avg = 0;

int last_time;
int measure_count = 0;
bool calibrated = false;

Servo servo;

void setup() {
  Serial.begin(9600);
  last_time = millis();
  servo.attach(PIN_SERVO);
}

void loop() {
  // measure time
  int time = millis();
  int dt_millis = time - last_time;
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
  
  if (time < 100) {
    measure_count++;
    xrot_avg = xrot_avg + xrot;
    zrot_avg = zrot_avg + zrot;
  } else if (!calibrated) {
    calibrated = true;
    xrot_avg = xrot_avg / measure_count;
    zrot_avg = zrot_avg / measure_count;
  }
  
  xangle += (xrot - xrot_avg) * dt;
  zangle += (zrot - zrot_avg) * dt;
  
  // limit servo angle
  servo_angle += (zrot - zrot_avg) * dt;
  if(servo_angle > 65) servo_angle = 65;
  if(servo_angle < -65) servo_angle = -65;
  
  Serial.println(zrot_avg);
  
  servo.write(90 + servo_angle);
}

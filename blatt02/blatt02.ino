int PIN_IN1 = 4;
int PIN_IN2 = 3;
int PIN_PWM = 2;
int PIN_STANDBY = 13;

int lastTime = 0;

void setMotorSpeed(float sp) {
  int pwm = (int)(fmax(0, fmin(1, fabs(sp))) * 255);
  int in1 = sp > 0;
  int in2 = sp < 0;
  
  digitalWrite(PIN_IN1, in1);
  digitalWrite(PIN_IN2, in2);
  analogWrite(PIN_PWM, pwm);
}

int stage = 0;
float stageTime = 0;
int fps = 10;

void setup() {
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_PWM, OUTPUT);

  pinMode(PIN_STANDBY, OUTPUT);
  digitalWrite(PIN_STANDBY, 1);
}

void nextStage() {
  stage = (stage + 1) % 4;
  stageTime = 0;
}

void loop() {
  int dtMillis = millis() - lastTime;
  float dt = dtMillis / 1000.f;
  lastTime = millis();
  stageTime += dt;

  float dir = (stage < 2) ? 1 : -1;
  float sp;
  if(stage % 2 == 0) {
    sp = stageTime / 5.0;
    if(sp >= 1) {
      nextStage();
    }
  } else {
    sp = 1 - stageTime / 1.0;
    if(sp <= 0) {
      nextStage();
    } 
  }
  
  setMotorSpeed(dir * sp);

  delay(1000/fps);
}

int PIN_IN1 = 4;
int PIN_IN2 = 3;
int PIN_PWM = 2;
int PIN_STANDBY = 13;

int PIN_BUTTON1 = 8;
int PIN_BUTTON2 = 9;

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

float dir = 0.f;
int mode = 0;
float modeTime = 0.f;
float sp = 0.f;
int on = 0;

bool btn1_wasDown = false;
bool btn2_wasDown = false;

void setup() {
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_PWM, OUTPUT);
  
  pinMode(PIN_BUTTON1, INPUT_PULLUP);
  pinMode(PIN_BUTTON2, INPUT_PULLUP);

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
  modeTime += dt;
  
  bool btn1_down = !digitalRead(PIN_BUTTON1);
  bool btn2_down = !digitalRead(PIN_BUTTON2);
  
  if (btn1_down && !btn1_wasDown) {
    mode = (mode + 1) % 3;
    modeTime = 0;
  }
  
  if (btn2_down && !btn2_wasDown) {
    on = 1 - on;
  }
  
  if (mode == 0) {
    dir = fmin(0, -1 + modeTime);
  } else if (mode == 1) {
    dir = fmin(1, modeTime);
  } else {
    dir = fmax(-1, 1 - 2 * modeTime);
  }
  
  if (on) {
    sp = fmin(1, sp + dt);
  } else {
    sp = fmax(0, sp - dt);
  }h
  
  setMotorSpeed(dir * sp);

  delay(1000/fps);
  
  btn1_wasDown = btn1_down;
  btn2_wasDown = btn2_down;
}

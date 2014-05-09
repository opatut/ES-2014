int XPIN = 0;
int ZPIN = 1;

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println("X: " + analogRead(XPIN));
  Serial.println("Y: " + analogRead(ZPIN));
}

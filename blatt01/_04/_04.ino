#define LED 7
int brightness;

void setup() {
    pinMode(LED, OUTPUT);
    brightness = 0;
    Serial.begin(9600);
}

void loop() {
    brightness = (brightness + 1) % 256;
    analogWrite(LED, brightness);
    delay(4);
    Serial.println(brightness);
}

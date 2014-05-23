#define LED 7
#define BUTTON1 3

bool wasUp;
bool ledOn;

void setup() {
    pinMode(LED, OUTPUT);
    pinMode(BUTTON1, INPUT);
    
    wasUp = digitalRead(BUTTON1);
    ledOn = false;
    digitalWrite(LED, ledOn);
}

void loop() {
    bool isUp = digitalRead(BUTTON1);
    if(isUp && !wasUp) {
        ledOn = !ledOn;
        digitalWrite(LED, ledOn);
        delay(50);
    }
    wasUp = isUp;
}


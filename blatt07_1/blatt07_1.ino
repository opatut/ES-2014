#include <SPI.h>
#include "ascii.h"
#include <sam.h>

#define OUT_RST 5
#define OUT_LED 3
#define OUT_DC 2
#define OUT_SCE 10 // 4, 10, 52

#define WIDTH 84
#define HEIGHT 48
#define BANKS 504
#define PIXELS 4032

byte pixbuf[BANKS];
String text = "hello";
int last_time;
int seconds;

int pixel[] = {10, 10};

void reset() {
  digitalWrite(OUT_RST, 0);
  delay(500);
  digitalWrite(OUT_RST, 1);
}

void setCommandMode(bool cmdMode) {
  digitalWrite(OUT_DC, !cmdMode);
}

void sendCommand(int cmd) {
  SPI.transfer(OUT_SCE, cmd);
}

void printChar(char c, int x, int y) {
  byte* chars = font[(int)c - font_offset];
  for(int dx = 0; dx < 6; ++dx) {
    for(int dy = 0; dy < 8; ++dy) {
      setPixel(x + dx, y + dy, chars[dx] & (1 << dy));
    }
  }
}

void initialize() {
  pinMode(OUT_LED, OUTPUT);
  pinMode(OUT_RST, OUTPUT);
  pinMode(OUT_DC, OUTPUT);
  
  digitalWrite(OUT_LED, 0); // turn LED off
  
  reset();
 
  SPI.begin(OUT_SCE);
  SPI.setClockDivider(OUT_SCE, 84);
  delay(100); 
  
  setCommandMode(true);
  delay(100);
  sendCommand(0x21); // FUNCTION SET
  sendCommand(0x14); // SET BIAS
  sendCommand(0xA8); // SET CONTRAST
  sendCommand(0x20); // FUNCTION SET
  sendCommand(0x0C); // SET DISPLAY MODE
  delay(100);
  sendCommand(0x80); // SET X
  sendCommand(0x40); // SET Y
  delay(100);
  
  setCommandMode(false);
  delay(100);
}

void clearScreen() {
  for(int i = 0; i < BANKS; ++i) {
    pixbuf[i] = 0x00;
  }
}

void updateScreen() {
  for(int i = 0; i < BANKS; ++i) {
    SPI.transfer(OUT_SCE, pixbuf[i]);
  }    
}

void setPixel(int x, int y, bool value) {
  if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
  
  int row = y / 8;
  int index = row * WIDTH + x;
  int bitnum = y % 8;
  int bitmask = 1 << bitnum;
  
  if(value) {
    pixbuf[index] |= bitmask;
  } else {
    pixbuf[index] &= ~bitmask;
  }
}

// Black magic
void startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency) {
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk((uint32_t)irq);
  TC_Configure(tc, channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4);
  uint32_t rc = VARIANT_MCK/128/frequency; //128 because we selected TIMER_CLOCK4 above
  TC_SetRA(tc, channel, rc/2); //50% high, 50% low
  TC_SetRC(tc, channel, rc);
  TC_Start(tc, channel);
  tc->TC_CHANNEL[channel].TC_IER=TC_IER_CPCS;
  tc->TC_CHANNEL[channel].TC_IDR=~TC_IER_CPCS;
  NVIC_EnableIRQ(irq);
}

void setup() {
  Serial.begin(9600);
  initialize();
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk(ID_TC0);
  
  startTimer(TC0, 0, TC0_IRQn, 1);
  
  NVIC_ClearPendingIRQ(TC0_IRQn);
  NVIC_EnableIRQ(TC0_IRQn);
  last_time = millis();
}

void TC0_Handler() {
  TC_GetStatus(TC0, 0);
  
  seconds++;
}

void loop() {
  digitalWrite(OUT_LED, 1);
  
  int time = millis();
  int dt = time - last_time;
  last_time = time;
  
  clearScreen();
  
  char buf[8];
  
  int minutes = (seconds / 60) % 60;
  int hours = (seconds / 3600) % 24;
  
  sprintf(buf, "%02d:%02d:%02d", hours, minutes, seconds % 60);
  
  text = String(buf);
  
  for(int i = 0; i < text.length(); ++i) {
    printChar(text.charAt(i), WIDTH / 2 - text.length() * 6 / 2 + i*6, HEIGHT / 2 - 4);
  }
  
  updateScreen();
}

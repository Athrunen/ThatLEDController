#include <Arduino.h>
#include "SSD1306Wire.h"
#include <string>
#include <sstream>
#include <limits.h>
#include <JC_Button.h> 

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

#define runEvery(t) for (static uint16_t _lasttime;\
                         (uint16_t)((uint16_t)millis() - _lasttime) >= (t);\
                         _lasttime += (t))


const byte
  SWITCH_PIN(35),
  UP_PIN(33),
  DOWN_PIN(32);

SSD1306Wire  display(0x3c, SDA, SCL);
const String color[4] = {"R", "G", "B", "W"};
int fill[4] = {255, 255, 255, 255};
const byte led_pins[4] = {15, 27, 2, 16};
int position = 0;
float width = display.getWidth();
float height = display.getHeight();
bool active = true;
byte touchpin = 13;
int touchthd = 6;
Button 
  SwitchButton(SWITCH_PIN),
  UpButton(UP_PIN),
  DownButton(DOWN_PIN);

void switchSelection(){
  position++;
  if (position == 4) {
    position = 0;
  }
}

void IRAM_ATTR touchToggle(){
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) 
  {
    int timestamp = millis();
    for (size_t i = 0; i < 5; i++)
    {
      if (touchRead(touchpin) > touchthd){ return; }
      while (timestamp - millis() < 10);
      timestamp = millis();
    }
    active = !active;
    Serial.println(active);
  }
  last_interrupt_time = interrupt_time;
  
}

void setup() {
  Serial.begin(115200);
  display.init();
  display.setContrast(255);
  SwitchButton.begin();
  UpButton.begin();
  DownButton.begin();
  for (size_t i = 0; i < 4; i++)
  {
    ledcSetup(i, 5000, 8);
    ledcAttachPin(led_pins[i], i);
    ledcWrite(i, 0);
  }
  touchAttachInterrupt(touchpin, touchToggle, touchthd);

  for (size_t i = 0; i < 4; i++){
      ledcWrite(i, fill[i]);
  }
  
}

void drawRect() {
  float amount = 4;
  float outerdist = 5;
  float spacing = 13;
  float totalspacing = (amount - 1) * spacing;
  int radius = round((width - 1 - outerdist * 2 - totalspacing) / (2 * amount));
  for (size_t i = 1; i < amount + 1; i++)
  {
    display.setColor(WHITE);
    float fillheight = 1 / 255.0 * fill[i - 1];
    float circledist = radius * (1 + 2 * (i - 1));
    float spacingdist = spacing * (i - 1);
    float positionX = round(outerdist + spacingdist + circledist);
    float positionY = round(height / 5);
    float calcheight = round(height / 5.0 * 3.0);
    float dist = round(calcheight * fillheight);
    display.drawRect(positionX - radius, positionY, radius * 2, calcheight);
    display.fillRect(positionX - radius, positionY + calcheight - dist, radius * 2, dist);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(positionX, 0, SSTR(fill[i - 1]).c_str());
    if (i == position + 1) {
      display.fillRect(positionX - radius / 2.5, positionY * 4.2, 8, 9);
      display.setColor(BLACK);
    }
    display.drawString(positionX, positionY * 4, color[i - 1]);
  }
}

void loop() {
  display.clear();

  drawRect();


  SwitchButton.read();

  if (SwitchButton.wasReleased()) {
    switchSelection();
  }


  runEvery(10) {
    UpButton.read();
    DownButton.read();
    if (UpButton.wasReleased()) {
      fill[position]++;
    } else if (UpButton.releasedFor(500)) {
      fill[position]++;
    }
    if (DownButton.wasReleased()) { 
      fill[position]--;
    } else if (DownButton.releasedFor(500)){
      fill[position]--;
    }
    if (fill[position] > 255) {
      fill[position] = 255;
    } else if (fill[position] < 0) {
      fill[position] = 0;
    }
    if (active) {
      for (size_t i = 0; i < 4; i++){
        ledcWrite(i, fill[i]);
      }
    } else {
      for (size_t i = 0; i < 4; i++){
        ledcWrite(i, 0);
      }
    }

    /* Serial.println(touchRead(touchpin));*/
  }
  


  /* Serial.println(fillradius); */
  display.display();
}
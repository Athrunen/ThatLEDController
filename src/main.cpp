#include <Arduino.h>
#include <string>
#include <JC_Button.h>

#include "helpers.h"
#include "display.h"
#include "config.h"


Display display(0x3c, SDA, SCL);
std::array<int, 4> fill = {255, 255, 255, 255};
int position = 0;
bool active = true;

Button 
  SwitchButton(config::SWITCH_PIN),
  UpButton(config::UP_PIN),
  DownButton(config::DOWN_PIN);

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
      if (touchRead(config::touchpin) > config::touchthd){ return; }
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
  display.setup();
  SwitchButton.begin();
  UpButton.begin();
  DownButton.begin();
  for (size_t i = 0; i < 4; i++)
  {
    ledcSetup(i, 5000, 8);
    ledcAttachPin(config::led_pins[i], i);
    ledcWrite(i, 0);
  }
  //touchAttachInterrupt(touchpin, touchToggle, touchthd);

  for (size_t i = 0; i < 4; i++){
      ledcWrite(i, fill[i]);
  }

  //Serial.println("Starting bluetooth service...");
  //setupBLE();
}

std::array<int, 3> hsv2rgb(std::array<int, 3> fill)
{
    double hh, p, q, t, ff;
    long i;
    std::array<int, 3> out;

    double h = double(fill[0]) / 255 * 360;
    double s = double(fill[1]) / 255;
    double v = double(fill[2]) / 255;

    if(s <= 0.0) {       // < is bogus, just shuts up warnings
        out[0] = v;
        out[1] = v;
        out[2] = v;
        return out;
    }
    hh = h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = v * (1.0 - s);
    q = v * (1.0 - (s * ff));
    t = v * (1.0 - (s * (1.0 - ff)));

    switch(i) {
    case 0:
        out[0] = v;
        out[1] = t;
        out[2] = p;
        break;
    case 1:
        out[0] = q;
        out[1] = v;
        out[2] = p;
        break;
    case 2:
        out[0] = p;
        out[1] = v;
        out[2] = t;
        break;

    case 3:
        out[0] = p;
        out[1] = q;
        out[2] = v;
        break;
    case 4:
        out[0] = t;
        out[1] = p;
        out[2] = v;
        break;
    case 5:
    default:
        out[0] = v;
        out[1] = p;
        out[2] = q;
        break;
    }
    return out;     
}

void setColor(std::array<int, 4> fill, String mode = "rgb", bool autowhite = false) {
  std::array<int, 4> color = fill;
  if (mode == "hsv") {
    std::array<int, 3> rgb = hsv2rgb({color[0], color[1], color[2]});
    color = {rgb[0], rgb[1], rgb[2], color[3]};
  } 
  for (size_t i = 0; i < 3; i++){
    ledcWrite(i, color[i]);
  }
  if (autowhite) {
    ledcWrite(3, *std::min_element(color.begin(), color.end() - 1));
  } else {
    ledcWrite(3, color[3]);
  }
}

void loop() {
  display.drawRGBW(position, fill);

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
      setColor(fill, "hsv");
    } else {
      for (size_t i = 0; i < 4; i++){
        ledcWrite(i, 0);
      }
    }

    //loopBLE();
  }
}
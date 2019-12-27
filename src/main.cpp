#include <Arduino.h>
#include <string>
#include <JC_Button.h>

#include "helpers.h"
#include "display.h"
#include "config.h"
#include "color.h"


Display display(0x3c, SDA, SCL);
std::array<int, 4> fill = {config::resolution_factor, config::resolution_factor, config::resolution_factor, config::resolution_factor};
int position = 0;
bool active = true;
String mode = "hsv";

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
    ledcSetup(i, 5000, config::resolution);
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

std::array<int, 4> calculateColor(std::array<int, 4> fill, String mode = "rgb", bool autowhite = false) {
  if (mode == "manual") {
    for (size_t i = 0; i < 4; i++){
      ledcWrite(i, fill[i]);
    }
    return;
  }

  // Be smart and convert only twice, here..
  std::array<double, 4> color;
  for (size_t i = 0; i < color.size; i++){
    color[i] = fill[i] / (double)config::resolution_factor;
  }

  if (mode == "hsv") {
    std::array<double, 3> rgb = color::hsv2rgb({color[0] * 360., color[1], color[2]});
    color = {rgb[0], rgb[1], rgb[2], color[3]};
  }

  if (mode == "hsi") {
    std::array<double, 3> rgb = color::hsi2rgb({color[0] * 360., color[1], color[2]});
    color = {rgb[0], rgb[1], rgb[2], color[3]};
  }

  if (autowhite) {
    double index = std::min_element(color.begin(), color.end() - 1) - color.begin();
    double factor = color[3];
    double min_color = color[index];
    color[index] = min_color * (1 - factor);
    color[3] = min_color * factor;
  } 

  // .. and here
  std::array<int, 4> out;
  for (size_t i = 0; i < color.size; i++){
    out[i] = color[i] * config::resolution_factor;
  }

  return out;
}

void loop() {
  display.drawHSVW(position, fill);

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
    if (fill[position] > config::resolution_factor) {
      fill[position] = config::resolution_factor;
    } else if (fill[position] < 0) {
      fill[position] = 0;
    }

    
    if (active) {
      std::array<int, 4> color = calculateColor(fill, "hsv", true);
      for (size_t i = 0; i < 4; i++){
        ledcWrite(i, color[i]);
      }
    } else {
      for (size_t i = 0; i < 4; i++){
        ledcWrite(i, 0);
      }
    }

    //loopBLE();
  }
}
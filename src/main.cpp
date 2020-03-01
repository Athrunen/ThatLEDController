#include <Arduino.h>
#include <WiFi.h>
#include <AsyncUDP.h>
#include <WebServer.h>
#include <string>
#include <JC_Button.h>
#include <WiFiManager.h>

#include <color.h>

#include "helpers.h"
#include "display.h"
#include "config.h"

AsyncUDP udp;
WiFiManager wifiManager;

Display display(0x3c, SDA, SCL);
std::array<int, 4> oldfill = {0, 0, 0, 0};
std::array<int, 4> fill = {config::resolution_factor, config::resolution_factor, config::resolution_factor, config::resolution_factor};
std::array<int, 4> currentcolor = {0, 0, 0, 0};
int position = 0;
bool active = true;
bool longpress = false;
std::array<std::string, 4> modes = {"manual", "rgb", "hsv", "hsl"};
std::string oldmode = "hsv";
std::string mode = "hsv";

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

void setup() {
  WiFi.mode(WIFI_STA);
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
  wifiManager.setClass("invert");
  wifiManager.setConfigPortalBlocking(false);
  wifiManager.autoConnect("ThatLEDController");
}

std::array<int, 4> calculateColor(std::array<int, 4> fill, std::string mode = "rgb", bool autowhite = false) {
  if (mode == "manual")
    return fill;

  // Be smart and convert only twice, here..
  std::array<double, 4> color;
  for (size_t i = 0; i < 4; i++){
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
  for (size_t i = 0; i < 4; i++){
    out[i] = color[i] * config::resolution_factor;
  }

  return out;
}

void setColor(std::array<int, 4> color) {
  if (active) {
    for (size_t i = 0; i < 4; i++){
      ledcWrite(i, color[i]);
    }
  } else {
    for (size_t i = 0; i < 4; i++){
      ledcWrite(i, 0);
    }
  }
}
void loop() {
  display.drawHSVW(position, fill);

  SwitchButton.read();

  if (SwitchButton.releasedFor(1000) and not longpress){
    longpress = true;
    Serial.println("LongPress!");
  } else if (SwitchButton.wasPressed()) {
    if (longpress) {
      longpress = false;
    } else {
      switchSelection();
    }
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
  }

  runEvery(2000) {
    if (WiFi.status() == WL_CONNECTED) {
      IPAddress broadcastip;
      broadcastip = ~WiFi.subnetMask() | WiFi.gatewayIP();
      if(udp.connect(broadcastip, 24444)) {
        udp.print("ThatLEDController online!");
      }
      udp.close();
    }
  }
  wifiManager.process();
  if (fill != oldfill || mode != oldmode) {
    currentcolor = calculateColor(fill, mode, true);
    setColor(currentcolor);
    oldfill = fill;
    oldmode = mode;
  }
}
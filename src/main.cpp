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
WiFiServer wifiServer(25555);
WiFiClient wifiClient;

Display display(0x3c, SDA, SCL);
std::array<int, 4> oldfill = {0, 0, 0, 0};
std::array<int, 4> fill = {config::resolution_factor, config::resolution_factor, config::resolution_factor, config::resolution_factor};
std::array<int, 4> currentcolor = {0, 0, 0, 0};
int position = 0;
bool active = true;
bool longpress = false;
std::array<std::string, 4> modes = {"manual", "rgb", "hsv", "hsi"};
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

std::vector<std::string> split (const std::string& str, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(str);
  while(std::getline(tokenStream, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

void setCmd(std::string str) {
  std::ostringstream os;
  if (config::debug) {
    os << "Serial command received: " << str << "\n";
  }
  std::vector<std::string> tokens = split(str, ' '); 
  if (tokens.size() < 1) {
    return;
  }
  std::string tmode = tokens[1];
  int tsize = tokens.size() - 2;
  std::array<int, 4> color;
  for (size_t i = 0; i < 4; i++) {
    color[i] = i < tsize ? atof(tokens[i + 2].c_str()) * config::resolution_factor : fill[i];
    color[i] = color[i] >= 0 ? color[i] : fill[i]; 
  }
  if (std::find(std::begin(modes), std::end(modes), tmode) != std::end(modes)) {
    mode = tmode;
  }
  if (config::debug) {
    os << "Parsed, setting values to: [";
    for (size_t i = 0; i < 4; i++){
      os << color[i];
      if (i != 3) {
        os << ", ";
      }
    }
    os << "]\n";
    os << "Mode: " << tmode;
  } else {
    os << "Command received";
  }
  Serial.println(os.str().c_str());
  fill = color;
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
  wifiServer.begin();
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

void CheckForConnection() {
  if (wifiServer.hasClient()) {
    if (wifiClient.connected()) {
      wifiServer.available().stop();
    } else {
      wifiClient = wifiServer.available();
      Serial.println("Client connected!");
      wifiClient.write("Hey there!");
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
    int ps = 0;
    if (UpButton.wasReleased()) {
      ps = 1;
    } else if (UpButton.releasedFor(500)) {
      ps = 50;
    } else if (UpButton.releasedFor(1000)) {
      ps = 100;
    }
    if (DownButton.wasReleased()) { 
      ps = -1;
    } else if (DownButton.releasedFor(500)){
      ps = -50;
    } else if (DownButton.releasedFor(1000)) {
      ps = -100;
    }
    if (fill[position] + ps > config::resolution_factor) {
      fill[position] = config::resolution_factor;
    } else if (fill[position] + ps < 0) {
      fill[position] = 0;
    } else {
      fill[position] += ps;
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
  CheckForConnection();
  if (wifiClient.available()) {
    std::string cm(wifiClient.readStringUntil('\n').c_str());
    if (cm.rfind("set ", 0) == 0) {
        setCmd(cm);
    }
  }
  if (fill != oldfill || mode != oldmode) {
    currentcolor = calculateColor(fill, mode, true);
    setColor(currentcolor);
    oldfill = fill;
    oldmode = mode;
  }
}
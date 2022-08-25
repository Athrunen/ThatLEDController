#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <AsyncUDP.h>
#include <WiFiManager.h>

#include "config.h"
#include "color.h"
#include "commands.h"

#ifdef FULLMODE

#include <JC_Button.h>
#include "display.h"

#endif

AsyncUDP udp;
WiFiManager wifiManager;
WiFiServer wifiServer(25555);
WiFiClient wifiClient;

#ifdef FULLMODE

Display display(0x3c, SDA, SCL);
bool longpress = false;
int position = 0;

Button 
  SwitchButton(config::SWITCH_PIN),
  UpButton(config::UP_PIN),
  DownButton(config::DOWN_PIN);

#endif

std::array<int, 4> dimstartfill = {0, 0, 0, 0};
std::array<int, 4> dimendfill = {0, 0, 0, 0};
std::array<int, 4> fill = {0, 0, 0, 0};
std::array<int, 4> nextfill = {config::resolution_factor, config::resolution_factor, config::resolution_factor, config::resolution_factor};
std::string mode = "manual";
std::string newmode = "manual";
bool dimming = false;

std::array<double, 3> sourcecolor = {0, 0, 0};
std::array<double, 3> targetcolor = {0, 0, 0};
int64_t starttime = 0;
int64_t endtime = 0;

void setColor(std::array<int, 4> color)
{
    for (size_t i = 0; i < 4; i++)
    {
        ledcWrite(i, color[i]);
    }
}

void setup() {
  Serial.begin(115200);

  #ifdef FULLMODE

  display.setup();
  SwitchButton.begin();
  UpButton.begin();
  DownButton.begin();

  #endif

  WiFi.mode(WIFI_STA);
  for (size_t i = 0; i < 4; i++)
  {
    ledcSetup(i, 5000, config::resolution);
    ledcAttachPin(config::led_pins[i], i);
    ledcWrite(i, 0);
  }

  setColor(nextfill);

  wifiManager.setClass("invert");
  wifiManager.setHttpPort(8080);
  wifiManager.setConfigPortalBlocking(false);
  wifiManager.autoConnect("ThatLEDController");
  
  wifiServer.begin();
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  commands::setupRouting();

}

void CheckForConnection() {
  if (wifiServer.hasClient()) {
    //if (wifiClient.connected()) {
    //  wifiServer.available().stop();
    //} else {
    wifiClient = wifiServer.available();
    Serial.println("Client connected!");
    wifiClient.write("Hey there!");
    //}
  }
}

#ifdef FULLMODE

void switchSelection(){
  position++;
  if (position == 4) {
    position = 0;
  }
}

#endif


void loop() {
  #ifdef FULLMODE

  display.drawHSVW(position, nextfill);

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
    if (nextfill[position] + ps > config::resolution_factor) {
      nextfill[position] = config::resolution_factor;
    } else if (nextfill[position] + ps < 0) {
      nextfill[position] = 0;
    } else {
      nextfill[position] += ps;
    }
  }

  #endif

  runEvery(1000) {
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
  ArduinoOTA.handle();
  commands::handleCommands();
  CheckForConnection();
  if (wifiClient.available()) {
    std::string cm(wifiClient.readStringUntil('\n').c_str());
    if (cm.rfind("set ", 0) == 0) {
      commands::setCmd(cm, newmode, nextfill, dimming);
    } else if (cm.rfind("dim ", 0) == 0){
      commands::dimCmd(cm, newmode, nextfill, dimming, fill, dimstartfill, dimendfill, starttime, endtime);
    }
  }
  if (dimming) {
    int64_t now = esp_timer_get_time();
    if (now < endtime) {
      double since = (double)now - (double)starttime;
      double duration = (double)endtime - (double)starttime;
      double t = since / duration;
      nextfill = color::lerp_color(dimstartfill, dimendfill, mode, newmode, t);
      std::array<int, 4> currentcolor = color::calculateColor(nextfill, newmode, false);
      setColor(currentcolor);
    } else {
      dimming = false;
      fill = nextfill;
      mode = newmode;
    }
  } else if (nextfill != fill || newmode != mode) {
    std::array<int, 4> currentcolor = color::calculateColor(nextfill, newmode, false);
    setColor(currentcolor);
    fill = nextfill;
    mode = newmode;
  }
}
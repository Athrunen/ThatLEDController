#pragma once
#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include "helpers.h"
#include <array>
#include <vector>
#include "config.h"
#include <ArduinoJson.h>
#include <WebServer.h>

namespace commands {

    void setCmd(std::string str, std::string &newmode, std::array<int, 4> &nextfill, bool &dimming);
    void dimCmd(std::string str, std::string &newmode, std::array<int, 4> &nextfill, bool &dimming, std::array<int, 4> &fill, std::array<int, 4> &dimstartfill, std::array<int, 4> &dimendfill, int64_t &starttime, int64_t &endtime);
    void getConfiguration();
    void getBase();
    void setupRouting();
    void handleCommands();

}

#endif
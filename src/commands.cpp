#include "commands.h"

namespace commands
{

    WebServer webServer(80);

    void setupRouting() {
        // TODO: Init webserver, route requests and deconstruct json for each function
        webServer.on("/configuration", getConfiguration);
        webServer.on("/", getBase);

        webServer.begin();
    }

    void handleCommands() {
        webServer.handleClient();
    }

    class BadCommand : public std::exception
    {
        virtual const char *what() const throw()
        {
            return "Bad command";
        }
    } bad_command;

    std::vector<std::string> split(const std::string &str, char delimiter)
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);
        while (std::getline(tokenStream, token, delimiter))
        {
            tokens.push_back(token);
        }
        return tokens;
    }

    std::vector<std::string> decode(std::string str, std::ostringstream &os)
    {
        if (config::debug)
        {
            os << "Serial command received: " << str << "\n";
        }
        std::vector<std::string> tokens = split(str, ' ');
        if (tokens.size() < 1)
        {
            throw bad_command;
        }
        return tokens;
    }

    void setCmd(std::string str, std::string &newmode, std::array<int, 4> &nextfill, bool &dimming)
    {
        std::ostringstream os;
        std::vector<std::string> tokens = decode(str, os);
        std::string tmode = tokens[1];
        int tsize = tokens.size() - 2;
        std::array<int, 4> color;
        for (size_t i = 0; i < 4; i++)
        {
            color[i] = i < tsize ? atof(tokens[i + 2].c_str()) * config::resolution_factor : nextfill[i];
            color[i] = color[i] >= 0 ? color[i] : nextfill[i];
        }
        if (std::find(std::begin(config::modes), std::end(config::modes), tmode) != std::end(config::modes))
        {
            newmode = tmode;
        }
        if (config::debug)
        {
            os << "Parsed, setting values to: [";
            for (size_t i = 0; i < 4; i++)
            {
                os << color[i];
                if (i != 3)
                {
                    os << ", ";
                }
            }
            os << "]\n";
            os << "Mode: " << tmode;
        }
        else
        {
            os << "Command received";
        }
        Serial.println(os.str().c_str());
        dimming = false;
        nextfill = color;
    }

    void dimCmd(std::string str, std::string &newmode, std::array<int, 4> &nextfill, bool &dimming, std::array<int, 4> &fill, std::array<int, 4> &dimstartfill, std::array<int, 4> &dimendfill, int64_t &starttime, int64_t &endtime)
    {
        std::ostringstream os;
        std::vector<std::string> tokens = decode(str, os);
        std::string tmode = tokens[1];
        int tsize = tokens.size() - 2;
        std::array<int, 4> targetcolor;
        for (size_t i = 0; i < 4; i++)
        {
            targetcolor[i] = i < tsize ? atof(tokens[i + 2].c_str()) * config::resolution_factor : nextfill[i];
            targetcolor[i] = targetcolor[i] >= 0 ? targetcolor[i] : nextfill[i];
        }
        int duration = tsize > 4 ? atof(tokens[6].c_str()) : 1000;
        if (std::find(std::begin(config::modes), std::end(config::modes), tmode) != std::end(config::modes))
        {
            newmode = tmode;
        }
        if (config::debug)
        {
            os << "Parsed, dimming values to: [";
            for (size_t i = 0; i < 4; i++)
            {
                os << targetcolor[i];
                if (i != 3)
                {
                    os << ", ";
                }
            }
            os << "]\n";
            os << "Mode: " << tmode;
        }
        else
        {
            os << "Command received";
        }
        Serial.println(os.str().c_str());
        dimstartfill = fill;
        dimendfill = targetcolor;
        starttime = esp_timer_get_time();
        endtime = starttime + duration * 1000;
        dimming = true;
    }

    void getConfiguration() {
        StaticJsonDocument<250> jsonDocument;
        char buffer[250];
        
        jsonDocument["debug"] = config::debug;
        
        const std::array<std::string, 4> names = {"r", "g", "b", "w"};
        JsonObject pins = jsonDocument.createNestedObject("pins");
        for (size_t i = 0; i < 4; i++)
        {
            pins[names[i]] = config::led_pins[i];
        }
        
        JsonArray modes = jsonDocument.createNestedArray("modes");
        for (const auto &mode : config::modes)
        {
            modes.add(mode);
        }
        
        jsonDocument["resolution_factor"] = config::resolution_factor;
        
        serializeJson(jsonDocument, buffer);
        webServer.send(200, "application/json", buffer);
    }

    void getBase() {
    
        webServer.send(200, "application/json", "{\"ThatLEDController\"}");

    }

}
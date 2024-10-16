#pragma once

#include "esphome.h"
#include "ArduinoJson.h"

class MinecraftOreBlockConfig : public Component, public CustomAPIDevice {
 public:
  struct Threshold {
    int players;
    std::string color;
  };

  void setup() override {
    register_service(&MinecraftOreBlockConfig::on_config_update, "config_update");
    load_config_();
  }

  void on_config_update(std::string data) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, data);

    mode = doc["mode"].as<std::string>();
    server_address = doc["serverAddress"].as<std::string>();
    server_port = doc["serverPort"].as<int>();
    static_color = doc["staticColor"].as<std::string>();

    thresholds.clear();
    for (JsonObject threshold : doc["thresholds"].as<JsonArray>()) {
      thresholds.push_back({
        threshold["players"].as<int>(),
        threshold["color"].as<std::string>()
      });
    }

    // Sort thresholds by player count
    std::sort(thresholds.begin(), thresholds.end(), 
      [](const Threshold& a, const Threshold& b) { return a.players < b.players; });

    // Save configuration to flash
    save_config_();

    // Publish state updates
    publish_states();
  }

  void publish_states() {
    publish_state("mode", mode.c_str());
    publish_state("server_address", server_address.c_str());
    publish_state("server_port", to_string(server_port).c_str());
    publish_state("static_color", static_color.c_str());
  }

  std::string get_mode() { return mode; }
  std::string get_server_address() { return server_address; }
  int get_server_port() { return server_port; }
  std::string get_static_color() { return static_color; }
  const std::vector<Threshold>& get_thresholds() { return thresholds; }

 private:
  std::string mode = "auto";
  std::string server_address = "";
  int server_port = 25565;
  std::string static_color = "#ff0000";
  std::vector<Threshold> thresholds;

  void save_config_() {
    DynamicJsonDocument doc(1024);
    doc["mode"] = mode;
    doc["serverAddress"] = server_address;
    doc["serverPort"] = server_port;
    doc["staticColor"] = static_color;

    JsonArray thresholds_json = doc.createNestedArray("thresholds");
    for (const auto& threshold : thresholds) {
      JsonObject threshold_json = thresholds_json.createNestedObject();
      threshold_json["players"] = threshold.players;
      threshold_json["color"] = threshold.color;
    }

    String output;
    serializeJson(doc, output);
    
    // Save to file system
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      ESP_LOGE("Config", "Failed to open config file for writing");
      return;
    }
    configFile.print(output);
    configFile.close();
  }

  void load_config_() {
    if (SPIFFS.begin()) {
      if (SPIFFS.exists("/config.json")) {
        File configFile = SPIFFS.open("/config.json", "r");
        if (configFile) {
          DynamicJsonDocument doc(1024);
          deserializeJson(doc, configFile);
          
          mode = doc["mode"].as<std::string>();
          server_address = doc["serverAddress"].as<std::string>();
          server_port = doc["serverPort"].as<int>();
          static_color = doc["staticColor"].as<std::string>();

          thresholds.clear();
          for (JsonObject threshold : doc["thresholds"].as<JsonArray>()) {
            thresholds.push_back({
              threshold["players"].as<int>(),
              threshold["color"].as<std::string>()
            });
          }

          configFile.close();
          publish_states();
        }
      }
    } else {
      ESP_LOGE("Config", "Failed to mount file system");
    }
  }
};
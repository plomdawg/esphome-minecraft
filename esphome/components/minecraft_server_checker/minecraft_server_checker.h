#include "esphome.h"
#include <WiFiClient.h>

class MinecraftServerChecker : public PollingComponent {
 public:
  MinecraftServerChecker() : PollingComponent(15000) {}  // Poll every 15 seconds

  Sensor *player_count_sensor = new Sensor();
  TextSensor *server_status_sensor = new TextSensor();

  void set_config(MinecraftOreBlockConfig *config) {
    config_ = config;
  }

  void setup() override {
    // This will be called by App.setup()
  }

  void update() override {
    // This will be called every 15 seconds
    if (config_->get_mode() == "auto") {
      check_server_status();
    } else {
      // In static mode, we don't need to check the server
      server_status_sensor->publish_state("Static Mode");
      player_count_sensor->publish_state(0);
    }
  }

  void set_player_count_sensor(sensor::Sensor *sens) { player_count_sensor = sens; }
  //void set_server_status_sensor(text_sensor::TextSensor *sens) { server_status_sensor = sens; }

 private:
  MinecraftOreBlockConfig *config_;

  void check_server_status() {
    WiFiClient client;
    if (!client.connect(config_->get_server_address().c_str(), config_->get_server_port())) {
      ESP_LOGE("MinecraftServer", "Connection failed");
      server_status_sensor->publish_state("Offline");
      player_count_sensor->publish_state(0);
      return;
    }

    // Send server list ping packet
    uint8_t packet[] = {
      0x00,  // Packet ID
      0x00,  // Protocol version (varint)
      0x00,  // String length (varint)
      0x00,  // Next state (varint, 1 for status)
    };

    client.write(packet, sizeof(packet));

    // Read response
    String response = client.readStringUntil('\n');
    client.stop();

    // Parse JSON response (simplified)
    int players_online = 0;
    if (response.indexOf("\"players\":{\"online\":") != -1) {
      int start = response.indexOf("\"players\":{\"online\":") + 20;
      int end = response.indexOf(",", start);
      players_online = response.substring(start, end).toInt();
    }

    server_status_sensor->publish_state("Online");
    player_count_sensor->publish_state(players_online);
    
    ESP_LOGI("MinecraftServer", "Players online: %d", players_online);
  }
};
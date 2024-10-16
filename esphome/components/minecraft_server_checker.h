#include "esphome.h"
#include <WiFiClient.h>

class MinecraftServerChecker : public PollingComponent {
 public:
  MinecraftServerChecker() : PollingComponent(15000) {}  // Poll every 15 seconds

  Sensor *player_count_sensor = new Sensor();
  TextSensor *server_status_sensor = new TextSensor();

  void setup() override {
    // This will be called by App.setup()
  }

  void update() override {
    // This will be called every 15 seconds
    check_server_status();
  }

  void set_server_address(const std::string &address) {
    server_address_ = address;
  }

  void set_server_port(uint16_t port) {
    server_port_ = port;
  }

 private:
  std::string server_address_ = "localhost";
  uint16_t server_port_ = 25565;

  void check_server_status() {
    WiFiClient client;
    if (!client.connect(server_address_.c_str(), server_port_)) {
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

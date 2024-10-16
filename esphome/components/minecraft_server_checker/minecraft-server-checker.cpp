#include "MinecraftServerChecker.h"
#include "esphome/core/log.h"
#include <ArduinoJson.h>

namespace esphome {
namespace minecraft {

static const char *TAG = "minecraft";

void MinecraftServerChecker::setup() {
  // Nothing to do here
}

void MinecraftServerChecker::loop() {
  if (millis() - this->last_update_ >= this->update_interval_) {
    this->update();
    this->last_update_ = millis();
  }
}

void MinecraftServerChecker::update() {
  if (this->query_server()) {
    ESP_LOGD(TAG, "Successfully queried Minecraft server");
  } else {
    ESP_LOGW(TAG, "Failed to query Minecraft server");
    if (this->server_status_sensor_ != nullptr) {
      this->server_status_sensor_->publish_state("Offline");
    }
    if (this->player_count_sensor_ != nullptr) {
      this->player_count_sensor_->publish_state(NAN);
    }
  }
}

bool MinecraftServerChecker::query_server() {
  if (!this->client_.connect(this->server_address_.c_str(), this->server_port_)) {
    ESP_LOGW(TAG, "Connection to server failed");
    return false;
  }

  // Send server list ping packet
  uint8_t packet[] = {
    0x00, // Packet ID
    0x00, // Protocol version (0 for ping)
    static_cast<uint8_t>(this->server_address_.length()), // Hostname length
  };
  this->client_.write(packet, sizeof(packet));
  this->client_.write(this->server_address_.c_str(), this->server_address_.length());
  this->client_.write((uint8_t*)&this->server_port_, 2); // Port
  this->client_.write((uint8_t)0x01); // Next state (1 for status)

  // Send status request packet
  uint8_t status_packet[] = {0x00};
  this->client_.write(status_packet, sizeof(status_packet));

  // Read response
  String response = this->client_.readString();
  this->client_.stop();

  if (response.length() > 0) {
    this->parse_server_response(response);
    return true;
  }

  return false;
}

void MinecraftServerChecker::parse_server_response(const String &response) {
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, response);

  if (error) {
    ESP_LOGW(TAG, "JSON parsing failed: %s", error.c_str());
    return;
  }

  JsonObject json = doc.as<JsonObject>();

  if (json.containsKey("players") && json["players"].containsKey("online")) {
    int player_count = json["players"]["online"];
    if (this->player_count_sensor_ != nullptr) {
      this->player_count_sensor_->publish_state(player_count);
    }
    ESP_LOGD(TAG, "Player count: %d", player_count);
  }

  if (json.containsKey("version") && json["version"].containsKey("name")) {
    String server_version = json["version"]["name"];
    if (this->server_status_sensor_ != nullptr) {
      this->server_status_sensor_->publish_state("Online - " + server_version);
    }
    ESP_LOGD(TAG, "Server version: %s", server_version.c_str());
  }
}

}  // namespace minecraft
}  // namespace esphome

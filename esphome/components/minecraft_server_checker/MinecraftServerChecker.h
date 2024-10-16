#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include <WiFiClient.h>

namespace esphome {
namespace minecraft {

class MinecraftServerChecker : public Component {
 public:
  void set_update_interval(uint32_t update_interval) { update_interval_ = update_interval; }
  void set_server_address(const std::string &server_address) { server_address_ = server_address; }
  void set_server_port(uint16_t server_port) { server_port_ = server_port; }

  void set_player_count_sensor(sensor::Sensor *player_count_sensor) { player_count_sensor_ = player_count_sensor; }
  void set_server_status_sensor(text_sensor::TextSensor *server_status_sensor) { server_status_sensor_ = server_status_sensor; }

  void setup() override;
  void loop() override;
  void update();

  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

 protected:
  uint32_t update_interval_;
  std::string server_address_;
  uint16_t server_port_;
  uint32_t last_update_{0};

  sensor::Sensor *player_count_sensor_{nullptr};
  text_sensor::TextSensor *server_status_sensor_{nullptr};

  WiFiClient client_;

  bool query_server();
  void parse_server_response(const String &response);
};

}  // namespace minecraft
}  // namespace esphome

#include "esphome.h"

class MinecraftLEDControl : public Component {
 public:
  MinecraftLEDControl() {}

  void set_config(MinecraftOreBlockConfig *config) {
    config_ = config;
  }

  void set_light(light::LightState *light) {
    light_ = light;
  }

  void set_player_count_sensor(Sensor *player_count_sensor) {
    player_count_sensor_ = player_count_sensor;
  }

  void setup() override {
    player_count_sensor_->add_on_state_callback([this](float state) {
      this->update_led_color(state);
    });
  }

  void update_led_color(float player_count) {
    if (config_->get_mode() == "auto") {
      std::string color = get_color_for_player_count(player_count);
      set_led_color(color);
    } else {
      set_led_color(config_->get_static_color());
    }
  }

 private:
  MinecraftOreBlockConfig *config_;
  light::LightState *light_;
  Sensor *player_count_sensor_;

  std::string get_color_for_player_count(float player_count) {
    const auto& thresholds = config_->get_thresholds();
    for (auto it = thresholds.rbegin(); it != thresholds.rend(); ++it) {
      if (player_count >= it->players) {
        return it->color;
      }
    }
    return "#000000";  // Default to off if no threshold met
  }

  void set_led_color(const std::string& color) {
    auto call = light_->make_call();
    call.set_rgb(parse_color(color));
    call.perform();
  }

  light::LightColorValues parse_color(const std::string& color) {
    int r, g, b;
    sscanf(color.c_str(), "#%02x%02x%02x", &r, &g, &b);
    return light::LightColorValues(r / 255.0f, g / 255.0f, b / 255.0f);
  }
};
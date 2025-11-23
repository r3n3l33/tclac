/**
* Create by Miguel Ángel López on 20/07/19
* and modify by xaxexa
* Refactoring & component making:
* Соловей с паяльником 15.03.2024
**/

#ifndef TCL_ESP_TCL_H
#define TCL_ESP_TCL_H

#include "esphome.h"
#include "esphome/core/defines.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/climate/climate.h"

namespace esphome {
namespace tclac {

using climate::ClimateCall;
using climate::ClimateMode;
using climate::ClimatePreset;
using climate::ClimateTraits;
using climate::ClimateFanMode;
using climate::ClimateSwingMode;

class tclacClimate : public climate::Climate, public esphome::uart::UARTDevice, public PollingComponent {

 private:
    uint8_t checksum;
    uint8_t dataTX[38];
    uint8_t dataRX[61];
    uint8_t poll[31] = {0xBB, 0x00, 0x01, 0x04, 0x19, 0x00, 0x00, 0x00,
                        0x08, 0x0F, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F,
                        0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0xA6};
    int i = 0;

    bool beeper_status_;
    bool display_status_;
    bool force_mode_status_;
    uint8_t switch_preset = 0;
    bool module_display_status_;
    uint8_t switch_fan_mode = 0;
    bool is_call_control = false;
    uint8_t switch_swing_mode = 0;
    int target_temperature_set = 0;
    uint8_t switch_climate_mode = 0;
    bool allow_take_control = false;

    esphome::climate::ClimateTraits traits_;

 public:
    tclacClimate() : PollingComponent(5 * 1000) {
      checksum = 0;
    }

    void readData();
    void takeControl();
    void loop() override;
    void setup() override;
    void update() override;
    void set_beeper_state(bool state);
    void set_display_state(bool state);
    void dataShow(bool flow, bool shine);
    void set_force_mode_state(bool state);
    void set_rx_led_pin(GPIOPin *rx_led_pin);
    void set_tx_led_pin(GPIOPin *tx_led_pin);
    void sendData(uint8_t * message, uint8_t size);
    void set_module_display_state(bool state);
    static String getHex(uint8_t *message, uint8_t size);
    void control(const ClimateCall &call) override;
    static uint8_t getChecksum(const uint8_t * message, size_t size);
    void set_vertical_airflow(AirflowVerticalDirection direction);
    void set_horizontal_airflow(AirflowHorizontalDirection direction);
    void set_vertical_swing_direction(VerticalSwingDirection direction);
    void set_horizontal_swing_direction(HorizontalSwingDirection direction);

    // Neue Setter-Signaturen mit Masken
    void set_supported_presets(const climate::ClimatePresetMask &presets);
    void set_supported_modes(const climate::ClimateModeMask &modes);
    void set_supported_fan_modes(const climate::ClimateFanModeMask &modes);
    void set_supported_swing_modes(const climate::ClimateSwingModeMask &modes);

 protected:
    GPIOPin *rx_led_pin_;
    GPIOPin *tx_led_pin_;
    ClimateTraits traits() override;

    climate::ClimateModeMask supported_modes_;
    climate::ClimatePresetMask supported_presets_;
    AirflowVerticalDirection vertical_direction_;
    climate::ClimateFanModeMask supported_fan_modes_;
    AirflowHorizontalDirection horizontal_direction_;
    VerticalSwingDirection vertical_swing_direction_;
    climate::ClimateSwingModeMask supported_swing_modes_;
    HorizontalSwingDirection horizontal_swing_direction_;
};

}  // namespace tclac
}  // namespace esphome

#endif // TCL_ESP_TCL_H

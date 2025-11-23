#include "tclac.h"

namespace esphome {
namespace tclac {

ClimateTraits tclacClimate::traits() {
  ClimateTraits traits;

  // Neue Feature-Flags
  traits.add_feature_flags(climate::CLIMATE_FEATURE_SUPPORTS_CURRENT_TEMPERATURE);

  // Unterstützte Modi
  traits.set_supported_modes(
      ClimateModeMask{
          climate::CLIMATE_MODE_OFF,
          climate::CLIMATE_MODE_AUTO,
          climate::CLIMATE_MODE_COOL,
          climate::CLIMATE_MODE_HEAT,
          climate::CLIMATE_MODE_DRY,
          climate::CLIMATE_MODE_FAN_ONLY
      }
  );

  // Unterstützte Presets
  traits.set_supported_presets(
      ClimatePresetMask{climate::CLIMATE_PRESET_NONE}
  );

  // Unterstützte Fan-Modes
  traits.set_supported_fan_modes(
      ClimateFanModeMask{climate::CLIMATE_FAN_AUTO}
  );

  // Unterstützte Swing-Modes
  traits.set_supported_swing_modes(
      ClimateSwingModeMask{climate::CLIMATE_SWING_OFF}
  );

  return traits;
}

// Neue Setter-Implementierungen mit Masken
void tclacClimate::set_supported_modes(const climate::ClimateModeMask &modes) {
  this->supported_modes_ = modes;
}

void tclacClimate::set_supported_presets(const climate::ClimatePresetMask &presets) {
  this->supported_presets_ = presets;
}

void tclacClimate::set_supported_fan_modes(const climate::ClimateFanModeMask &modes) {
  this->supported_fan_modes_ = modes;
}

void tclacClimate::set_supported_swing_modes(const climate::ClimateSwingModeMask &modes) {
  this->supported_swing_modes_ = modes;
}

}  // namespace tclac
}  // namespace esphome

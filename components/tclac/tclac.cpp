/**
* Create by Miguel Ángel López on 20/07/19
* and modify by xaxexa, edit for newer models by Kannix2005
* Refactoring & component making:
* Соловей с паяльником 15.03.2024
**/
#include "esphome.h"
#include "esphome/core/defines.h"
#include "tclac.h"
#include <sstream>
#include <iomanip>

namespace esphome{
namespace tclac{


ClimateTraits tclacClimate::traits() {
  ClimateTraits traits;

  // Neue Feature-Flags statt deprecated Methoden
  traits.add_feature_flags(climate::CLIMATE_FEATURE_SUPPORTS_CURRENT_TEMPERATURE);

  // Unterstützte Modi (alle relevanten Modi direkt in der Maske)
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

  // Unterstützte Lüftermodi
  traits.set_supported_fan_modes(
      ClimateFanModeMask{climate::CLIMATE_FAN_AUTO}
  );

  // Unterstützte Swing-Modi
  traits.set_supported_swing_modes(
      ClimateSwingModeMask{climate::CLIMATE_SWING_OFF}
  );

  return traits;
}



void tclacClimate::setup() {

	//this->esphome::uart::UARTDevice::write_array(setup1, sizeof(setup1));
	//this->esphome::uart::UARTDevice::flush();
	//this->esphome::uart::UARTDevice::write_array(setup2, sizeof(setup2));
	//this->esphome::uart::UARTDevice::flush();
	//this->esphome::uart::UARTDevice::write_array(setup3, sizeof(setup3));
	//this->esphome::uart::UARTDevice::flush();

	target_temperature_set = 20;
	target_temperature = 20;
	restore_state_();
#ifdef CONF_RX_LED
	this->rx_led_pin_->setup();
	this->rx_led_pin_->digital_write(false);
#endif
#ifdef CONF_TX_LED
	this->tx_led_pin_->setup();
	this->tx_led_pin_->digital_write(false);
#endif
}

void tclacClimate::loop()  {
	if (esphome::uart::UARTDevice::available() > 0) {
		dataShow(0, true);
		memset(dataRX, 0, sizeof(dataRX));
		dataRX[0] = esphome::uart::UARTDevice::read();
		// If the received byte is not the header (0xBB), just exit the loop
		if (dataRX[0] != 0xBB) {
			ESP_LOGD("TCL", "Wrong byte");
			dataShow(0,0);
			return;
		}
		// If the header (0xBB) matches, start reading 4 more bytes in sequence
		delay(5);
		dataRX[1] = esphome::uart::UARTDevice::read();
		delay(5);
		dataRX[2] = esphome::uart::UARTDevice::read();
		delay(5);
		dataRX[3] = esphome::uart::UARTDevice::read();
		delay(5);
		dataRX[4] = esphome::uart::UARTDevice::read();

		//auto raw = getHex(dataRX, 5);
		
		//ESP_LOGD("TCL", "first 5 byte : %s ", raw.c_str());

		// From the first 5 bytes we need the fifth - it contains the message length
		esphome::uart::UARTDevice::read_array(dataRX+5, dataRX[4]+1);
		//int c = 0;
		//while(esphome::uart::UARTDevice::available() != 0){
		//	esphome::uart::UARTDevice::read_byte(&dataRX[5+c]);
		//	c++;
		//}

		uint8_t check = getChecksum(dataRX, sizeof(dataRX));

		auto raw = getHex(dataRX, sizeof(dataRX));
		
		//ESP_LOGD("TCL", "RX full : %s ", raw.c_str());
		
		// Check the checksum
		if (check != dataRX[60]) {
			ESP_LOGD("TCL", "Invalid checksum %x", check);
			tclacClimate::dataShow(0,0);
			return;
		} else {
			ESP_LOGD("TCL", "checksum OK %x", check);
		}
		tclacClimate::dataShow(0,0);
		// After reading everything from the buffer, we proceed to parse the data
		tclacClimate::readData();
	}
}

void tclacClimate::update() {
	tclacClimate::dataShow(1,1);
	this->esphome::uart::UARTDevice::write_array(poll, sizeof(poll));
	//const char* raw = tclacClimate::getHex(poll, sizeof(poll)).c_str();
	this->esphome::uart::UARTDevice::flush();
	//delay(100);
	//this->esphome::uart::UARTDevice::write_array(poll2, sizeof(poll2));
	//this->esphome::uart::UARTDevice::flush();
	//this->esphome::uart::UARTDevice::write_array(poll3, sizeof(poll3));
	//this->esphome::uart::UARTDevice::flush();

	tclacClimate::dataShow(1,0);
}

void tclacClimate::readData() {
	// BBx01x00x03x37x04x00x00x00x00x00x00x00x00x00x80x36x00x23x08x00x00x00x00x00x00x00x00x00x20x66xFFx42x00x50x21x21x21x00x00x80x00x00x00x00xE9x00x00x00x54x40x03x23x00x00x7Ax00x00x00x00xBA
	//current_temperature = float((( (dataRX[17] << 8) | dataRX[18] ) / 374 - 32)/1.8);
	//target_temperature = (dataRX[FAN_SPEED_POS] & SET_TEMP_MASK) + 16;
	//current_temperature = float((dataRX[29] | (dataRX[30] << 8))*0.001);
	//auto current_temperature2 = float((( (dataRX[45] << 8) | dataRX[46] ) / 374 - 32)/1.8);
	current_temperature = 256 - dataRX[45];  // 256-233 = 23°C (plausible!)
	//this->current_temperature = current_temperature;
	//target_temperature = 20;

	//this->target_temperature = target_temperature_set;

	//current_temperature = float((( (dataRX[17] << 8) | dataRX[18] ) / 374 - 32)/1.8);
	//target_temperature = (dataRX[FAN_SPEED_POS] & SET_TEMP_MASK) + 16;

	ESP_LOGD("TCL", "TEMP: %f ", current_temperature);
	//ESP_LOGD("TCL", "TEMP: %f ", current_temperature2);


	if (dataRX[MODE_POS] & ( 1 << 4)) {
		// If the air conditioner is on, parse the data for display
		// ESP_LOGD("TCL", "AC is on");
		uint8_t modeswitch = MODE_MASK & dataRX[MODE_POS];
		uint8_t fanspeedswitch = FAN_SPEED_MASK & dataRX[FAN_SPEED_POS];
		uint8_t swingmodeswitch = SWING_MODE_MASK & dataRX[SWING_POS];

		// switch (modeswitch) {
		// 	case MODE_AUTO:
		// 		mode = climate::CLIMATE_MODE_AUTO;
		// 		break;
		// 	case MODE_COOL:
		// 		mode = climate::CLIMATE_MODE_COOL;
		// 		break;
		// 	case MODE_DRY:
		// 		mode = climate::CLIMATE_MODE_DRY;
		// 		break;
		// 	case MODE_FAN_ONLY:
		// 		mode = climate::CLIMATE_MODE_FAN_ONLY;
		// 		break;
		// 	case MODE_HEAT:
		// 		mode = climate::CLIMATE_MODE_HEAT;
		// 		break;
		// 	default:
		// 		mode = climate::CLIMATE_MODE_OFF;
		// }


		// //OVERRIDE

		// switch (dataRX[MODE_POS])
		// {
		// 	case 0x08:
		// 		//auto
		// 			mode = climate::CLIMATE_MODE_AUTO;
		// 		break;
		
		// 	case 0x07:
		// 		//Lüfter
		// 			mode = climate::CLIMATE_MODE_FAN_ONLY;
		// 		break;
		
		// 	case 0x02:
		// 		//Trocknen
		// 			mode = climate::CLIMATE_MODE_DRY;
		// 		break;
		
		// 	case 0x01:
		// 		//Heizen
		// 			mode = climate::CLIMATE_MODE_HEAT;
		// 		break;
		
		// 	case 0x03:
		// 		//Kühlen
		// 			mode = climate::CLIMATE_MODE_COOL;
		// 		break;
		// }



		// if ( dataRX[FAN_QUIET_POS] & FAN_QUIET) {
		// 	fan_mode = climate::CLIMATE_FAN_QUIET;
		// } else if (dataRX[MODE_POS] & FAN_DIFFUSE){
		// 	fan_mode = climate::CLIMATE_FAN_DIFFUSE;
		// } else {
		// 	switch (fanspeedswitch) {
		// 		case FAN_AUTO:
		// 			fan_mode = climate::CLIMATE_FAN_AUTO;
		// 			break;
		// 		case FAN_LOW:
		// 			fan_mode = climate::CLIMATE_FAN_LOW;
		// 			break;
		// 		case FAN_MIDDLE:
		// 			fan_mode = climate::CLIMATE_FAN_MIDDLE;
		// 			break;
		// 		case FAN_MEDIUM:
		// 			fan_mode = climate::CLIMATE_FAN_MEDIUM;
		// 			break;
		// 		case FAN_HIGH:
		// 			fan_mode = climate::CLIMATE_FAN_HIGH;
		// 			break;
		// 		case FAN_FOCUS:
		// 			fan_mode = climate::CLIMATE_FAN_FOCUS;
		// 			break;
		// 		default:
		// 			fan_mode = climate::CLIMATE_FAN_AUTO;
		// 	}
		// }

		// switch (swingmodeswitch) {
		// 	case SWING_OFF: 
		// 		swing_mode = climate::CLIMATE_SWING_OFF;
		// 		break;
		// 	case SWING_HORIZONTAL:
		// 		swing_mode = climate::CLIMATE_SWING_HORIZONTAL;
		// 		break;
		// 	case SWING_VERTICAL:
		// 		swing_mode = climate::CLIMATE_SWING_VERTICAL;
		// 		break;
		// 	case SWING_BOTH:
		// 		swing_mode = climate::CLIMATE_SWING_BOTH;
		// 		break;
		// }
		
		// // Обработка данных о пресете
		// preset = ClimatePreset::CLIMATE_PRESET_NONE;
		// if (dataRX[7] & (1 << 6)){
		// 	preset = ClimatePreset::CLIMATE_PRESET_ECO;
		// } else if (dataRX[9] & (1 << 2)){
		// 	preset = ClimatePreset::CLIMATE_PRESET_COMFORT;
		// } else if (dataRX[19] & (1 << 0)){
		// 	preset = ClimatePreset::CLIMATE_PRESET_SLEEP;
		// }
		
	} //else {
		// If the air conditioner is off, all modes are shown as off
		//mode = climate::CLIMATE_MODE_OFF;
		//fan_mode = climate::CLIMATE_FAN_OFF;
		//swing_mode = climate::CLIMATE_SWING_OFF;
		//preset = ClimatePreset::CLIMATE_PRESET_NONE;
	//}
	// Publish the data
	this->publish_state();
	allow_take_control = true;
   }

// Climate control
void tclacClimate::control(const ClimateCall &call) {
	// Request data from the AC mode switch
	if (call.get_mode().has_value()){
		switch_climate_mode = call.get_mode().value();

		switch (switch_climate_mode) {
			case 6:
				mode = climate::CLIMATE_MODE_AUTO;
				break;
			case 2:
				mode = climate::CLIMATE_MODE_COOL;
				break;
			case 5:
				mode = climate::CLIMATE_MODE_DRY;
				break;
			case 4:
				mode = climate::CLIMATE_MODE_FAN_ONLY;
				break;
			case 3:
				mode = climate::CLIMATE_MODE_HEAT;
				break;
			default:
				mode = climate::CLIMATE_MODE_OFF;
		}

		ESP_LOGD("TCL", "Get MODE %i from call", (int) mode);
	} //else {
		//switch_climate_mode = mode;
		//ESP_LOGD("TCL", "Get MODE from AC");
	//}

	// Request data from the AC preset switch
	if (call.get_preset().has_value()){
		switch_preset = call.get_preset().value();

		switch (switch_preset){
			case 0:
				preset = climate::CLIMATE_PRESET_NONE;
			case 5:
				preset = climate::CLIMATE_PRESET_ECO;
			case 6:
				preset = climate::CLIMATE_PRESET_SLEEP;
			case 4:
				preset = climate::CLIMATE_PRESET_COMFORT;
		}
	} //else {
		//switch_preset = preset.value();
	//}
	
	// Request data from the fan mode switch
	if (call.get_fan_mode().has_value()){
		switch_fan_mode = call.get_fan_mode().value();

		if (mode != climate::CLIMATE_MODE_OFF){
			switch (switch_fan_mode) {
				case 2:
					fan_mode = climate::CLIMATE_FAN_AUTO;
				case 3:
					fan_mode = climate::CLIMATE_FAN_LOW;
				case 6:
					fan_mode = climate::CLIMATE_FAN_MIDDLE;
				case 4:
					fan_mode = climate::CLIMATE_FAN_MEDIUM;
				case 5:
					fan_mode = climate::CLIMATE_FAN_HIGH;
				case 7:
					fan_mode = climate::CLIMATE_FAN_FOCUS;
				default:
					fan_mode = climate::CLIMATE_FAN_AUTO;
			}
		} else {
			fan_mode = climate::CLIMATE_FAN_OFF;
		}
	} //else {
	//	switch_fan_mode = fan_mode.value();
	//}
	
	// Request data from the swing mode switch
	if (call.get_swing_mode().has_value()){
		switch_swing_mode = call.get_swing_mode().value();

		switch (switch_swing_mode) {
			case 0: 
				swing_mode = climate::CLIMATE_SWING_OFF;
			case 3:
				swing_mode = climate::CLIMATE_SWING_HORIZONTAL;
			case 2:
				swing_mode = climate::CLIMATE_SWING_VERTICAL;
			case 1:
				swing_mode = climate::CLIMATE_SWING_BOTH;
			default:
				swing_mode = climate::CLIMATE_SWING_OFF;
		}

	} //else {
		// If the switch is empty, fill with the value from the last state poll. Like, nothing changed.
		//switch_swing_mode = swing_mode;
	//}
	
	// Calculate temperature
	if (call.get_target_temperature().has_value()) {
		target_temperature_set = (int)call.get_target_temperature().value();
		target_temperature = target_temperature_set;
	} //else {
		//target_temperature_set = 111 - (int)target_temperature;
	//}
	
	is_call_control = true;
	takeControl();
	this->publish_state();
	allow_take_control = true;
}
	
	
void tclacClimate::takeControl() {
	
	dataTX[7]  = 0b00000000;
	dataTX[8]  = 0b00000000;
	dataTX[9]  = 0b00000000;
	dataTX[10] = 0b00000000;
	dataTX[11] = 0b00000000;
	dataTX[19] = 0b00000000;
	dataTX[32] = 0b00000000;
	dataTX[33] = 0b00000000;
	
	if (is_call_control != true){
		ESP_LOGD("TCL", "Get MODE from AC for force config");
		switch_climate_mode = mode;
		switch_preset = preset.value();
		switch_fan_mode = fan_mode.value();
		switch_swing_mode = swing_mode;
		target_temperature_set = (int)target_temperature;
	}
	
	// Enable or disable the beeper depending on the setting switch
	if (beeper_status_){
		ESP_LOGD("TCL", "Beep mode ON");
		dataTX[7] += 0b00100000;
	} else {
		ESP_LOGD("TCL", "Beep mode OFF");
		dataTX[7] += 0b00000000;
	}
	
	// Enable or disable the AC display depending on the setting switch
	// Only enable the display if the AC is in one of the operating modes
	
	// WARNING! When the display is turned off, the AC automatically switches to automatic mode!
	
	if ((display_status_) && (switch_climate_mode != climate::CLIMATE_MODE_OFF)){
		ESP_LOGD("TCL", "Display turn ON");
		dataTX[7] += 0b01000000;
	} else {
		ESP_LOGD("TCL", "Display turn OFF");
		dataTX[7] += 0b00000000;
	}
		
	// Configure the AC operating mode
	switch (switch_climate_mode) {
		case climate::CLIMATE_MODE_OFF:
			dataTX[7] += 0b00000000;
			dataTX[8] += 0b00000000;
			break;
		case climate::CLIMATE_MODE_AUTO:
			dataTX[7] += 0b00000100;
			dataTX[8] += 0b00001000;
			break;
		case climate::CLIMATE_MODE_COOL:
			dataTX[7] += 0b00000100;
			dataTX[8] += 0b00000011;	
			break;
		case climate::CLIMATE_MODE_DRY:
			dataTX[7] += 0b00000100;
			dataTX[8] += 0b00000010;	
			break;
		case climate::CLIMATE_MODE_FAN_ONLY:
			dataTX[7] += 0b00000100;
			dataTX[8] += 0b00000111;	
			break;
		case climate::CLIMATE_MODE_HEAT:
			dataTX[7] += 0b00000100;
			dataTX[8] += 0b00000001;	
			break;
	}


	// Configure the fan mode
	switch(switch_fan_mode) {
		case climate::CLIMATE_FAN_AUTO:
			dataTX[8]	+= 0b00000000;
			dataTX[10]	+= 0b00000000;
			break;
		case climate::CLIMATE_FAN_QUIET:
			dataTX[8]	+= 0b10000000;
			dataTX[10]	+= 0b00000000;
			break;
		case climate::CLIMATE_FAN_LOW:
			dataTX[8]	+= 0b00000000;
			dataTX[10]	+= 0b00000001;
			break;
		case climate::CLIMATE_FAN_MIDDLE:
			dataTX[8]	+= 0b00000000;
			dataTX[10]	+= 0b00000110;
			break;
		case climate::CLIMATE_FAN_MEDIUM:
			dataTX[8]	+= 0b00000000;
			dataTX[10]	+= 0b00000011;
			break;
		case climate::CLIMATE_FAN_HIGH:
			dataTX[8]	+= 0b00000000;
			dataTX[10]	+= 0b00000111;
			break;
		case climate::CLIMATE_FAN_FOCUS:
			dataTX[8]	+= 0b00000000;
			dataTX[10]	+= 0b00000101;
			break;
		case climate::CLIMATE_FAN_DIFFUSE:
			dataTX[8]	+= 0b01000000;
			dataTX[10]	+= 0b00000000;
			break;
	}
	
	// Set the damper swing mode
	switch(switch_swing_mode) {
		case climate::CLIMATE_SWING_OFF:
			dataTX[10]	+= 0b00000000;
			dataTX[11]	+= 0b00000000;
			break;
		case climate::CLIMATE_SWING_VERTICAL:
			dataTX[10]	+= 0b00111000;
			dataTX[11]	+= 0b00000000;
			break;
		case climate::CLIMATE_SWING_HORIZONTAL:
			dataTX[10]	+= 0b00000000;
			dataTX[11]	+= 0b00001000;
			break;
		case climate::CLIMATE_SWING_BOTH:
			dataTX[10]	+= 0b00111000;
			dataTX[11]	+= 0b00001000;  
			break;
	}
	
	// Set the AC presets
	switch(switch_preset) {
		case ClimatePreset::CLIMATE_PRESET_NONE:
			break;
		case ClimatePreset::CLIMATE_PRESET_ECO:
			dataTX[7]	+= 0b10000000;
			break;
		case ClimatePreset::CLIMATE_PRESET_SLEEP:
			dataTX[19]	+= 0b00000001;
			break;
		case ClimatePreset::CLIMATE_PRESET_COMFORT:
			dataTX[8]	+= 0b00010000;
			break;
	}

        //Damper mode
		//	Vertical damper
		//		Vertical damper swing [byte 10, mask 00111000]:
		//			000 - Swing off, damper in last position or fixed
		//			111 - Swing on in selected mode
		//		Vertical damper swing mode (fixed mode doesn't matter if swing is on) [byte 32, mask 00011000]:
		//			01 - swing up-down, DEFAULT
		//			10 - swing in upper half
		//			11 - swing in lower half
		//		Damper fixed mode (swing mode doesn't matter if swing is off) [byte 32, mask 00000111]:
		//			000 - no fixation, DEFAULT
		//			001 - fixed at top
		//			010 - fixed between top and center
		//			011 - fixed in center
		//			100 - fixed between center and bottom
		//			101 - fixed at bottom
		//	Horizontal dampers
		//		Horizontal damper swing [byte 11, mask 00001000]:
		//			0 - Swing off, dampers in last position or fixed
		//			1 - Swing on in selected mode
		//		Horizontal damper swing mode (fixed mode doesn't matter if swing is on) [byte 33, mask 00111000]:
		//			001 - swing left-right, DEFAULT
		//			010 - swing left
		//			011 - swing center
		//			100 - swing right
		//		Horizontal damper fixed mode (swing mode doesn't matter if swing is off) [byte 33, mask 00000111]:
		//			000 - no fixation, DEFAULT
		//			001 - fixed left
		//			010 - fixed between left and center
		//			011 - fixed in center
		//			100 - fixed between center and right
		//			101 - fixed right
		
		
	// Set the vertical damper swing mode
	switch(vertical_swing_direction_) {
		case VerticalSwingDirection::OFF:
			dataTX[32]	+= 0b00000000;
			ESP_LOGD("TCL", "Vertical swing: off");
			break;
		case VerticalSwingDirection::UP_DOWN:
			dataTX[32]	+= 0b00001000;
			ESP_LOGD("TCL", "Vertical swing: up-down");
			break;
		case VerticalSwingDirection::UPSIDE:
			dataTX[32]	+= 0b00010000;
			ESP_LOGD("TCL", "Vertical swing: upper");
			break;
		case VerticalSwingDirection::DOWNSIDE:
			dataTX[32]	+= 0b00011000;
			ESP_LOGD("TCL", "Vertical swing: downer");
			break;
	}
	// Set the horizontal damper swing mode
	switch(horizontal_swing_direction_) {
		case HorizontalSwingDirection::OFF:
			dataTX[33]	+= 0b00000000;
			ESP_LOGD("TCL", "Horizontal swing: left-right");
			break;
		case HorizontalSwingDirection::LEFT_RIGHT:
			dataTX[33]	+= 0b00001000;
			ESP_LOGD("TCL", "Horizontal swing: left-right");
			break;
		case HorizontalSwingDirection::LEFTSIDE:
			dataTX[33]	+= 0b00010000;
			ESP_LOGD("TCL", "Horizontal swing: lefter");
			break;
		case HorizontalSwingDirection::CENTER:
			dataTX[33]	+= 0b00011000;
			ESP_LOGD("TCL", "Horizontal swing: center");
			break;
		case HorizontalSwingDirection::RIGHTSIDE:
			dataTX[33]	+= 0b00100000;
			ESP_LOGD("TCL", "Horizontal swing: righter");
			break;
	}
	// Set the vertical damper fixed position
	switch(vertical_direction_) {
		case AirflowVerticalDirection::LAST:
			dataTX[32]	+= 0b00000000;
			ESP_LOGD("TCL", "Vertical fix: last position");
			break;
		case AirflowVerticalDirection::MAX_UP:
			dataTX[32]	+= 0b00000001;
			ESP_LOGD("TCL", "Vertical fix: up");
			break;
		case AirflowVerticalDirection::UP:
			dataTX[32]	+= 0b00000010;
			ESP_LOGD("TCL", "Vertical fix: upper");
			break;
		case AirflowVerticalDirection::CENTER:
			dataTX[32]	+= 0b00000011;
			ESP_LOGD("TCL", "Vertical fix: center");
			break;
		case AirflowVerticalDirection::DOWN:
			dataTX[32]	+= 0b00000100;
			ESP_LOGD("TCL", "Vertical fix: downer");
			break;
		case AirflowVerticalDirection::MAX_DOWN:
			dataTX[32]	+= 0b00000101;
			ESP_LOGD("TCL", "Vertical fix: down");
			break;
	}
	// Set the horizontal damper fixed position
	switch(horizontal_direction_) {
		case AirflowHorizontalDirection::LAST:
			dataTX[33]	+= 0b00000000;
			ESP_LOGD("TCL", "Horizontal fix: last position");
			break;
		case AirflowHorizontalDirection::MAX_LEFT:
			dataTX[33]	+= 0b00000001;
			ESP_LOGD("TCL", "Horizontal fix: left");
			break;
		case AirflowHorizontalDirection::LEFT:
			dataTX[33]	+= 0b00000010;
			ESP_LOGD("TCL", "Horizontal fix: lefter");
			break;
		case AirflowHorizontalDirection::CENTER:
			dataTX[33]	+= 0b00000011;
			ESP_LOGD("TCL", "Horizontal fix: center");
			break;
		case AirflowHorizontalDirection::RIGHT:
			dataTX[33]	+= 0b00000100;
			ESP_LOGD("TCL", "Horizontal fix: righter");
			break;
		case AirflowHorizontalDirection::MAX_RIGHT:
			dataTX[33]	+= 0b00000101;
			ESP_LOGD("TCL", "Horizontal fix: right");
			break;
	}

	// Set temperature
	dataTX[9] = 111 - target_temperature_set;
		
	// Assemble the byte array to send to the AC
	dataTX[0] = 0xBB;	// Header start byte
	dataTX[1] = 0x00;	// Header start byte
	dataTX[2] = 0x01;	// Header start byte
	dataTX[3] = 0x03;	// 0x03 - control, 0x04 - poll
	dataTX[4] = 0x20;	// 0x20 - control, 0x19 - poll
	dataTX[5] = 0x03;	//??
	dataTX[6] = 0x01;	//??
	//dataTX[7] = 0x64;	//eco,display,beep,ontimerenable, offtimerenable,power,0,0
	//dataTX[8] = 0x08;	//mute,0,turbo,health, mode(4) mode 01 heat, 02 dry, 03 cool, 07 fan, 08 auto, health(+16), 41=turbo-heat 43=turbo-cool (turbo = 0x40+ 0x01..0x08)
	//dataTX[9] = 0x0f;	//0 -31 ;    15 - 16 0,0,0,0, temp(4) settemp 31 - x
	//dataTX[10] = 0x00;	//0,timerindicator,swingv(3),fan(3) fan+swing modes //0=auto 1=low 2=med 3=high
	//dataTX[11] = 0x00;	//0,offtimer(6),0
	dataTX[12] = 0x00;	//fahrenheit,ontimer(6),0 cf 80=f 0=c
	dataTX[13] = 0x01;	//??
	dataTX[14] = 0x00;	//0,0,halfdegree,0,0,0,0,0
	dataTX[15] = 0x00;	//??
	dataTX[16] = 0x00;	//??
	dataTX[17] = 0x00;	//??
	dataTX[18] = 0x00;	//??
	//dataTX[19] = 0x00;	//sleep on = 1 off=0     ---  0,0,0,0,0,0,0,1 = sleep on; 0,0,0,1,0,0,0,0 = self clean on
	dataTX[20] = 0x00;	//??
	dataTX[21] = 0x00;	//??
	dataTX[22] = 0x00;	//??
	dataTX[23] = 0x00;	//??
	dataTX[24] = 0x00;	//??
	dataTX[25] = 0x00;	//??
	dataTX[26] = 0x00;	//??
	dataTX[27] = 0x00;	//??
	dataTX[28] = 0x00;	//??
	dataTX[29] = 0x20;	//??
	dataTX[30] = 0x00;	//??
	dataTX[31] = 0x00;	//??
	//dataTX[32] = 0x00;	//0,0,0,vertical swing mode(2), vertical fixed position(3)
	//dataTX[33] = 0x00;	//0,0,vertical swing mode(2), vertical fixed position(3)
	dataTX[34] = 0x00;	//??
	dataTX[35] = 0x00;	//??
	dataTX[36] = 0x00;	//??
	dataTX[37] = 0xFF;	//Checksum
	dataTX[37] = tclacClimate::getChecksum(dataTX, sizeof(dataTX));

	tclacClimate::sendData(dataTX, sizeof(dataTX));
	allow_take_control = false;
	is_call_control = false;
}

// Send data to the AC
void tclacClimate::sendData(uint8_t * message, uint8_t size) {
	tclacClimate::dataShow(1,1);
	//Serial.write(message, size);
	this->esphome::uart::UARTDevice::write_array(message, size);
	//auto raw = getHex(message, size);
	ESP_LOGD("TCL", "Message sent to TCL...");
	tclacClimate::dataShow(1,0);
}

// Convert byte to readable format
String tclacClimate::getHex(uint8_t *message, uint8_t size) {
	std::ostringstream oss;
	// print each byte as two uppercase hex digits
	for (uint8_t i = 0; i < size; ++i) {
	  oss << std::hex 
	  << std::uppercase 
	  << std::setw(2) 
	  << std::setfill('0') 
	  << static_cast<int>(message[i]);
	  if (i + 1 < size)
	    oss << ' ';  // optional separator
	}
	  // grab the std::string, ensure uppercase
	std::string s = oss.str();
	// (oss.str() is already uppercase thanks to std::uppercase, so
	// this transform is optional)
	std::transform(s.begin(), s.end(), s.begin(), ::toupper);
	// convert to esphome::String and return
	return String(s.c_str());
}

// Calculate checksum
uint8_t tclacClimate::getChecksum(const uint8_t * message, size_t size) {
	uint8_t position = size - 1;
	uint8_t crc = 0;
	for (int i = 0; i < position; i++)
		crc ^= message[i];
	return crc;
}

// Blink LEDs
void tclacClimate::dataShow(bool flow, bool shine) {
	if (module_display_status_){
		if (flow == 0){
			if (shine == 1){
#ifdef CONF_RX_LED
				this->rx_led_pin_->digital_write(true);
#endif
			} else {
#ifdef CONF_RX_LED
				this->rx_led_pin_->digital_write(false);
#endif
			}
		}
		if (flow == 1) {
			if (shine == 1){
#ifdef CONF_TX_LED
				this->tx_led_pin_->digital_write(true);
#endif
			} else {
#ifdef CONF_TX_LED
				this->tx_led_pin_->digital_write(false);
#endif
			}
		}
	}
}

// Actions with data from config

// Get beeper state
void tclacClimate::set_beeper_state(bool state) {
	this->beeper_status_ = state;
	if (force_mode_status_){
		if (allow_take_control){
			tclacClimate::takeControl();
		}
	}
}
// Get AC display state
void tclacClimate::set_display_state(bool state) {
	this->display_status_ = state;
	if (force_mode_status_){
		if (allow_take_control){
			tclacClimate::takeControl();
		}
	}
}
// Get forced settings application mode state
void tclacClimate::set_force_mode_state(bool state) {
	this->force_mode_status_ = state;
}
// Get receive data LED pin
#ifdef CONF_RX_LED
void tclacClimate::set_rx_led_pin(GPIOPin *rx_led_pin) {
	this->rx_led_pin_ = rx_led_pin;
}
#endif
// Get transmit data LED pin
#ifdef CONF_TX_LED
void tclacClimate::set_tx_led_pin(GPIOPin *tx_led_pin) {
	this->tx_led_pin_ = tx_led_pin;
}
#endif
// Get module communication LED state
void tclacClimate::set_module_display_state(bool state) {
	this->module_display_status_ = state;
}
// Get vertical damper fixed mode
void tclacClimate::set_vertical_airflow(AirflowVerticalDirection direction) {
	this->vertical_direction_ = direction;
	if (force_mode_status_){
		if (allow_take_control){
			tclacClimate::takeControl();
		}
	}
}
// Get horizontal damper fixed mode
void tclacClimate::set_horizontal_airflow(AirflowHorizontalDirection direction) {
	this->horizontal_direction_ = direction;
	if (force_mode_status_){
		if (allow_take_control){
			tclacClimate::takeControl();
		}
	}
}
// Get vertical damper swing mode
void tclacClimate::set_vertical_swing_direction(VerticalSwingDirection direction) {
	this->vertical_swing_direction_ = direction;
	if (force_mode_status_){
		if (allow_take_control){
			tclacClimate::takeControl();
		}
	}
}
// Get supported AC operating modes
void tclacClimate::set_supported_modes(const std::set<climate::ClimateMode> &modes) {
	this->supported_modes_ = modes;
}
// Get horizontal damper swing mode
void tclacClimate::set_horizontal_swing_direction(HorizontalSwingDirection direction) {
	horizontal_swing_direction_ = direction;
	if (force_mode_status_){
		if (allow_take_control){
			tclacClimate::takeControl();
		}
	}
}
// Get supported fan speeds
void tclacClimate::set_supported_fan_modes(const std::set<climate::ClimateFanMode> &modes){
	this->supported_fan_modes_ = modes;
}
// Get supported swing modes
void tclacClimate::set_supported_swing_modes(const std::set<climate::ClimateSwingMode> &modes) {
	this->supported_swing_modes_ = modes;
}
// Get supported presets
void tclacClimate::set_supported_presets(const std::set<climate::ClimatePreset> &presets) {
  this->supported_presets_ = presets;
}

}
}

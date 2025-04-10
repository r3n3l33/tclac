Übersetzt mit ChatGPT. Ich habe es Erfolgreich mit einer TCL TAC-12CHDA eingebunden.
Wiring is as follows:









TCL Air Conditioner Integration with Home Assistant (Tested on TAC-12CHDA)
Translated with ChatGPT – successfully tested on a TCL TAC-12CHDA unit.
Wiring guide and setup instructions below.

🛠 What You’ll Need
ESP32 microcontroller

USB-A plug or cable (I used these: AliExpress Link)

Basic soldering tools (unless your unit already has a USB/serial port)

🔌 Wiring Guide
USB-A Pin	Wire Color	Connect To (ESP32)
GND	Black	VIN
D+	Green	GND
D-	Grey	RXD
VBUS	Red	TXD
📸 Example Photos

![IMG_0778](https://github.com/user-attachments/assets/9b674e06-41ca-4bcf-b09b-691a5fbd8545)
![image](https://github.com/user-attachments/assets/e30fadd9-19cd-47ec-baab-86f8a80410f6)


🧠 Software Setup (Using Home Assistant + ESPHome)
This solution only works with Home Assistant + ESPHome (version 2023.3.0 or later).

Step-by-Step:
Install ESPHome in Home Assistant
Go to Settings → Add-ons → ESPHome → Install

Create a new device
Use the ESPHome dashboard to add a new device and choose your ESP32 board type (e.g., esp32-c3-devkitm-1 or nodemcu-32s).

Paste the YAML configuration

Use the simplified config from this file:
Sample_conf.yaml

Or use the more detailed config:
TCL-Conditioner.yaml

Adjust all fields as needed (Wi-Fi, device name, etc.)

Flash the ESP32
Use USB or OTA to flash your ESP32 with the configuration.

✅ Supported AC Models (Confirmed Working)
This component supports various TCL and compatible models like:

Axioma ASX09H1/ASB09H1

Daichi AIR20AVQ1/AIR20FV1

Daichi AIR25AVQS1R-1/AIR25FVS1R-1

Daichi AIR35AVQS1R-1/AIR35FVS1R-1

Daichi DA35EVQ1-1/DF35EV1-1

Dantex RK-12SATI/RK-12SATIE

TCL TAC-07CHSA/TPG-W

TCL TAC-09CHSA/TPG

TCL TAC-09CHSA/DSEI-W

TCL TAC-12CHSA/TPG

TCL TAC-12CHDA/

ℹ️ Note: Even units with the same model name can differ in hardware. Some may lack USB or UART ports.

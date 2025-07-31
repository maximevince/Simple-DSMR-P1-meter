# Simple DSMR P1 Meter

A simple, wireless DSMR "P1 Meter" using an ESP devboard with ESPHome to read data from Dutch/Belgian smart meters.

## Hardware

This project uses a **Wemos D1 Mini** with ESP8266, but works with any ESP8266 or ESP32 board. The circuit is compliant with the DSMR v5 specification and designed for DSMR v5 hardware that allows drawing power from the P1 port (5V, 250mA).

### Components Needed

- ESP8266/ESP32 development board (e.g., Wemos D1 Mini)
- 10kΩ resistor (pullup)
- RJ12 connector and cable
- Optional: 3D printed case

### Wiring Diagram

ASCII schematic:
```
                                                    ESP8266 (e.g. D1 mini)  
  Smart Meter P1 Port                              ┌────────────────────┐
      RJ12 (6P6C)                                  │                    │
┌──────────────────────┐                           │                    │
│ Pin 1: +5V Supply ───┼────────────────┬──────┬───┼─► 3v3              │
│                      │                │      │   │                    │
│ Pin 2: Data Request ─┼────────────────┘     ┌┴┐  │                    │
│                      │                      │1│  │                    │
│ Pin 3: Data Ground ──┼────────────────┐     │0│  │                    │
│                      │                │     │k│  │                    │
│ Pin 4: Not used  ────┼─X              │     └┬┘  │                    │
│                      │                │      │   │                    │
│ Pin 5: Data  ────────┼────────────────)──────┴───┼─► RX (GPIO3)       │
│                      │                │          │                    │
│ Pin 6: Power Ground ─┼────────────────┴──────────┼─► GND              │
└──────────────────────┘                           │                    │
                                                   └────────────────────┘
```


Connect the RJ12 P1 port to your ESP board as follows:

| RJ12 Pin | Function | ESP Connection |
|----------|----------|----------------|
| 1 | +5V supply | VIN (5V input) |
| 2 | Data Request | VIN (5V input) |
| 3 | Data ground | GND |
| 4 | Not used | - |
| 5 | Data | GPIO3 (RX) + 10kΩ pullup to 3.3V |
| 6 | Power ground | GND |

**Important Notes:**
- POWER: The P1 port provides 5V/250mA on DSMR v5 meters
- DATA REQUEST: Pin 2 must be connected to +5V to enable data flow
- PULLUP RESISTOR: Pin 5 requires 10kΩ pullup to 3.3V for proper logic levels (Pin 5 is open drain)
- UART SIGNAL: The data signal is inverted, because of the open-drain. (UART inversion configured in ESPHome)
- GROUNDING: Both data ground (pin 3) and power ground (pin 6) need to connect to ESP GND

### 3D Printed Case

For housing the device, you can use this excellent 3D printable case:
[D1 Mini ESP8266 Case on MakerWorld](https://makerworld.com/en/models/1185038-d1-mini-esp8266-case)

## Software Setup

### Prerequisites

- [ESPHome](https://esphome.io/) installed
- Home Assistant (optional, for easy integration)

### Installation

1. Clone this repository:
   ```bash
   git clone https://github.com/yourusername/DSMR-P1-meter.git
   cd DSMR-P1-meter
   ```

2. Copy the secrets template and configure your WiFi:
   ```bash
   cp ESPHome/secrets.yaml.template ESPHome/secrets.yaml
   ```
   Edit `ESPHome/secrets.yaml` with your WiFi credentials.

3. Compile and upload the firmware
   Use the ESPHome web interface to do this, or the command-line version like so:
   ```bash
   esphome compile ESPHome/dsmr-p1-meter.yaml
   esphome upload ESPHome/dsmr-p1-meter.yaml
   ```

### Configuration

The ESPHome configuration (`ESPHome/dsmr-p1-meter.yaml`) includes:

- **UART Configuration**: Hardware UART on GPIO3 with inverted signal for P1 compatibility
```yaml
uart:
  rx_pin: 
    number: GPIO3  # D9/RX pin on Wemos D1 Mini
    inverted: true  # Invert RX signal from DSMR P1 open drain output
  baud_rate: 115200
  data_bits: 8
  parity: NONE
  stop_bits: 1
```
- **DSMR Sensors**: Complete set of sensors for Dutch and Belgian meters
  - Energy consumption/production (tariff 1 & 2)
  - Current power usage/production
  - Voltage and current per phase (L1, L2, L3)
  - Gas consumption
  - Electricity failures and long failures
- **Network**: WiFi with fallback hotspot
- **Integration**: Home Assistant API with OTA updates

## Data Available

The meter provides the following data:

### Electricity
- Energy delivered/returned (tariff 1 & 2)
- Current power consumed/returned
- Voltage per phase (L1, L2, L3)
- Current per phase (L1, L2, L3)
- Power delivered/returned per phase
- Electricity failure statistics

### Gas
- Gas consumption (Dutch and Belgian formats)

### System Information
- Meter identification
- P1 version information

## Troubleshooting

### Common Issues

1. **No data received**: Check RJ12 wiring, verify that your device is up and running (you can see logs in ESPHome), then ensure pin 2 is connected to +5V.
2. **Garbled data**: Verify the 10kΩ pullup resistor on data line (pin 5)
3. **Other connection issues**: Check that UART logging is disabled (baud_rate: 0)

### Debug Mode

Enable verbose logging by changing the logger level in the YAML:
```yaml
logger:
  level: DEBUG
```

## DSMR Specification

This project implements the DSMR P1 Companion Standard v5.0.2. The complete specification is included in the `spec/` directory.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [ESPHome](https://esphome.io/) for the excellent DSMR component
- DSMR for a clear and simple spec
- [i-BoxIt case design](https://makerworld.com/en/models/1185038-d1-mini-esp8266-case)


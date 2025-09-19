# ESP32-C3 Super Mini Wardriver

A minimal and cheap wardriver using ESP32-C3 Super Mini and a GPS module. Scans nearby Wi-Fi networks, logs them along with GPS coordinates, and stores the data in CSV format on LittleFS.
-Wiggle.net format decode will be added soon.

## Parts List

- ESP32-C3 Super Mini
- NEO-6M GPS Module
- Push Button

## Pin Configuration (Direct Connections)

| ESP32 Pin    | Device / Signal | Notes                                           |
|--------------|----------------|-------------------------------------------------|
| GPIO20 (RX1) | GPS TX         | Receive GPS data from GPS module                |
| GPIO21 (TX1) | GPS RX         | Optional: send data to GPS (usually not needed) |
| GPIO9        | Push Button    | Connect to one side of button, other side to GND |
| 3.3V / 5V    | GPS VCC        | Depends on GPS module voltage tolerance         |
| GND          | GPS & Button GND | Common ground for GPS and button              |

### Button Note
- Uses INPUT_PULLUP in code; no external resistor needed.

### Power Note
- ESP32-C3 Super Mini is 3.3V logic.
- GPS modules with onboard regulator usually accept 5V safely.

## Wiring Diagram (Text)

### ESP32-C3 to GPS Module
```
ESP32-C3 Super Mini        NEO-6M GPS Module
-------------------        ----------------
GPIO20 (RX1) <------------ TX
GPIO21 (TX1) ------------> RX (optional)
3.3V / 5V   ------------> VCC
GND          ------------> GND
```

### ESP32-C3 to Push Button
```
ESP32-C3 Super Mini        Push Button
-------------------        -----------
GPIO9       -------------> One side of button
GND         -------------> Other side of button
```

## Usage

1. Open Serial Monitor at 115200 baud.
2. Wait for GPS fix (status shows "GPS FIX").
3. ESP32 scans nearby Wi-Fi networks every 10 seconds.
4. Logs are saved to `/wardrive.csv` on LittleFS.
5. Press button to dump logs over Serial and reset CSV.

## Notes

- **LittleFS storage limit**: stops logging if usage >80%.
- **CSV format**: Time(ms),SSID,BSSID,RSSI,Channel,Lat,Lon,GPS_Status.
- **Minimal setup**: no SD card, no optional components.
- **Dependencies**: Button and GPS are mandatory; ESP32 powers everything.

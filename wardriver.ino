#include <WiFi.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <FS.h>
#include <LittleFS.h>

#define BUTTON_PIN 9
#define LOG_FILE "/wardrive.csv"
#define STORAGE_LIMIT_PERCENT 80  // Stop logging at 80 percent

TinyGPSPlus gps;
HardwareSerial GPSserial(1);  

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("==== ESP32 Wardriver Starting ====");

  // Initialize GPS UART
  GPSserial.begin(9600, SERIAL_8N1, 20, 21); 
  Serial.println("GPS Serial initialized (RX=GPIO20, TX=GPIO21)");

  // Configure push button
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize LittleFS
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed!");
    while (1);
  }
  Serial.println("LittleFS initialized");

  // Create log file if it doesn't exist
  if (!LittleFS.exists(LOG_FILE)) {
    File f = LittleFS.open(LOG_FILE, "w");
    f.println("Time(ms),SSID,BSSID,RSSI,Channel,Lat,Lon,GPS_Status");
    f.close();
    Serial.println("Log file created");
  }
}

void loop() {
  // Read GPS data
  while (GPSserial.available()) gps.encode(GPSserial.read());

  double lat = gps.location.isValid() ? gps.location.lat() : 0.0;
  double lon = gps.location.isValid() ? gps.location.lng() : 0.0;
  String gpsStatus = gps.location.isValid() ? "GPS FIX" : "Waiting for GPS fix";
  String timeStr = gps.time.isValid() ? 
                   String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second())
                   : String(millis());

  // Check storage usage
  size_t totalBytes = LittleFS.totalBytes();
  size_t usedBytes = LittleFS.usedBytes();
  if (usedBytes > (totalBytes * STORAGE_LIMIT_PERCENT / 100)) {
    Serial.println("Storage reached 80%, stopping logging to protect flash.");
  } else {
    // Scan Wi-Fi
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; i++) {
      String ssid = WiFi.SSID(i);
      String bssid = WiFi.BSSIDstr(i);
      int rssi = WiFi.RSSI(i);
      int channel = WiFi.channel(i);

      String logStr = timeStr + "," + ssid + "," + bssid + "," + rssi + "," + channel + "," + lat + "," + lon + "," + gpsStatus;

      //  LittleFS
      File f = LittleFS.open(LOG_FILE, "a");
      if (f) {
        f.println(logStr);
        f.close();
      }

      // Live serial output
      Serial.println(logStr);
    }
  }

  // Button press: dump + delete logs
  if (digitalRead(BUTTON_PIN) == LOW) {
    Serial.println("==== BUTTON PRESSED: DUMPING LOG FILE ====");
    File f = LittleFS.open(LOG_FILE, "r");
    while (f && f.available()) {
      Serial.write(f.read());
    }
    f.close();

    // Delete old log and start fresh
    LittleFS.remove(LOG_FILE);
    File newf = LittleFS.open(LOG_FILE, "w");
    newf.println("Time(ms),SSID,BSSID,RSSI,Channel,Lat,Lon,GPS_Status");
    newf.close();

    Serial.println("Log file deleted. Fresh logging started.");
    delay(2000); // debounce
  }

  delay(10000); 
}

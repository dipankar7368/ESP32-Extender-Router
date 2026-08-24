#include <Network.h>
#include <WiFi.h>

// ---------- Change these ----------
const char *ROUTER_SSID     = "Your_Router_SSID";
const char *ROUTER_PASSWORD = "Your_Router_Password";

const char *EXTENDER_SSID     = "My-ESP32-Extender";
const char *EXTENDER_PASSWORD = "87654321";  // 8+ characters
// ----------------------------------

// This network must be different from your router's network.
IPAddress apIP(192, 168, 50, 1);
IPAddress apMask(255, 255, 255, 0);
IPAddress apLeaseStart(192, 168, 50, 10);
IPAddress dnsServer(1, 1, 1, 1);

void onNetworkEvent(arduino_event_id_t event, arduino_event_info_t info) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.println("Connected to router; waiting for IP address...");
      break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.println();
      Serial.println("Router connection ready.");
      Serial.print("ESP32 address on router: ");
      Serial.println(WiFi.STA.localIP());

      // Turn on Network Address Translation: extender clients get Internet.
      WiFi.AP.enableNAPT(true);
      Serial.println("NAT enabled. Extender is ready.");
      break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
      WiFi.AP.enableNAPT(false);
      Serial.println("Router connection lost; reconnecting...");
      break;

    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
      Serial.println("A device joined the extender.");
      break;

    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
      Serial.println("A device left the extender.");
      break;

    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Network.begin();
  Network.onEvent(onNetworkEvent);

  // Create the local Wi-Fi network first.
  WiFi.AP.begin();

  if (!WiFi.AP.config(apIP, apIP, apMask, apLeaseStart, dnsServer)) {
    Serial.println("Failed to configure the extender network.");
    return;
  }

  if (!WiFi.AP.create(EXTENDER_SSID, EXTENDER_PASSWORD)) {
    Serial.println("Failed to start extender Wi-Fi.");
    return;
  }

  Serial.println();
  Serial.println("ESP32 Wi-Fi extender started.");
  Serial.print("Connect devices to: ");
  Serial.println(EXTENDER_SSID);
  Serial.print("Extender settings page IP: http://");
  Serial.println(WiFi.AP.localIP());

  // Connect ESP32 to the existing router.
  WiFi.STA.setHostname("esp32-extender");
  WiFi.STA.begin();
  WiFi.STA.connect(ROUTER_SSID, ROUTER_PASSWORD);
}

void loop() {
  delay(1000);
}
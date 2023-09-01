#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* dnsServer = "8.8.8.8"; //DNS server IP
unsigned int localUdpPort = 5000; 

char packetBuffer[255];

WiFiUDP udp;

void setup() {
  Serial.begin(115200);
  delay(10);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  udp.begin(localUdpPort);
}

void loop() {
  queryDNS();
  delay(3000);
}

void queryDNS(void) {
  Serial.print("Querying DNS");
  
  uint16_t id = random(0, 65535);
  uint8_t dnsRequest[] = {
    0x00, 0x01, // Transaction ID
    0x01, 0x00, // Flags: Standard Query
    0x00, 0x01, // Questions: 1
    0x00, 0x00, // Answers: 0
    0x00, 0x00, // Authority PRs: 0
    0x00, 0x00, // Additional PRs: 0
    // Query
    0x03, 'w', 'w', 'w', 
    0x07, 'e', 'x', 'a', 'm', 'p', 'l', 'e', 
    0x03, 'c', 'o', 'm',
    0x00, // Null terminator
    0x00, 0x10, // Type: TXT (Host Address)
    0x00, 0x01 // Class: IN (Internet)
  };

  // Send DNS request
  udp.beginPacket(dnsServer, 53);
  udp.write(dnsRequest, sizeof(dnsRequest));
  udp.endPacket();

  delay(500);

  int packetSize = udp.parsePacket();

  if (packetSize) {

    Serial.print("Received packet of size :");

    Serial.println(packetSize);
   
    Serial.print("from:");
    Serial.print(udp.remoteIP());
    Serial.print(", port ");
    Serial.println(udp.remotePort());
  
    int len = udp.read(packetBuffer, 255);
    
    if (len > 0) {
      Serial.println("Contents:");
      for (int i = 0; i < len; i++) {
        if (i==42) {
        Serial.println("");
        Serial.println("Text in response:");
        }
        if (i>=42) {
        Serial.print(packetBuffer[i]);
        } else {
        Serial.print(packetBuffer[i], HEX);
        Serial.print(" "); 
        }
      }
      Serial.println("");

    } else {
    Serial.println("No DNS response received");
  }
  }
  
}

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* dnsServer = "8.8.8.8"; //DNS server IP
unsigned int localUdpPort = 5000; 

char packetBuffer[255];

String myAddress = "www.example.com";

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
  uint16_t id = random(0, 65535);
  uint8_t dnsRequest_part_1[] = {
    0x00, 0x01, // Transaction ID
    0x01, 0x00, // Flags: Standard Query
    0x00, 0x01, // Questions: 1
    0x00, 0x00, // Answers: 0
    0x00, 0x00, // Authority PRs: 0
    0x00, 0x00, // Additional PRs: 0
  };

  uint8_t dnsRequest_part_2[myAddress.length()+1];
  int i_with_length = 0;
  int letter_counter = 0;
  for(int i = 0; i < myAddress.length(); i++) {
    if (myAddress[i] == '.') {
      dnsRequest_part_2[i_with_length] = letter_counter;
      i_with_length = i+1;
      letter_counter = 0;
    } else {
      dnsRequest_part_2[i+1] = myAddress[i];
      letter_counter++;
    }
  }
  dnsRequest_part_2[i_with_length] = letter_counter;

  Serial.print("request address formated : ");
  for (int i = 0; i < sizeof(dnsRequest_part_2); i++) {
    if (dnsRequest_part_2[i] < 30) {
      Serial.print(" 0x");
      Serial.print(dnsRequest_part_2[i], HEX);
      Serial.print(' ');
    } else {
      Serial.print(char(dnsRequest_part_2[i]));
    }
  }
  Serial.println();

  uint8_t dnsRequest_part_3[] = {
    0x00, // Null terminator
    0x00, 0x10, // Type: TXT (Host Address)
    0x00, 0x01 // Class: IN (Internet)
  };
  
  // Send DNS request
  Serial.print("Querying DNS");
  udp.beginPacket(dnsServer, 53);
  udp.write(dnsRequest_part_1, sizeof(dnsRequest_part_1));
  udp.write(dnsRequest_part_2, sizeof(dnsRequest_part_2));
  udp.write(dnsRequest_part_3, sizeof(dnsRequest_part_3));
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

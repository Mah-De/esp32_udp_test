#include <WiFi.h>
#include <WiFiUdp.h>
#include <Arduino.h>

// Wi-Fi credentials
const char *ssid = "MINE5213";
const char *password = "qwertyuiop";

// UDP server
WiFiUDP udp;

// UDP port to listen on
const int localPort = 12345;  // Replace with your desired UDP port

// Buffer to store incoming packet data
char packetBuffer[512];  // Maximum packet size

void setup() {
  // Start the Serial monitor
  Serial.begin(115200);
  Serial.println("Starting UDP Logger and Sender...");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("Connected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start listening to the UDP port
  udp.begin(localPort);
  Serial.print("Listening for UDP packets on port ");
  Serial.println(localPort);
}

void loop() {
  // Check if there's an incoming UDP packet
  int packetSize = udp.parsePacket();
  if (packetSize) {
    // Get the remote IP address and port
    IPAddress remoteIP = udp.remoteIP();
    unsigned int remotePort = udp.remotePort();

    // Log the packet details (time, IP, port, and message)
    Serial.print("Received packet at ");
    Serial.print(millis());  // Time in milliseconds since the program started
    Serial.print(" ms from ");
    Serial.print(remoteIP);
    Serial.print(":");
    Serial.print(remotePort);
    Serial.print(" with size ");
    Serial.println(packetSize);

    // Read the incoming packet and store it in the buffer
    udp.read(packetBuffer, packetSize);

    // Log the packet content
    Serial.print("Packet contents: ");
    Serial.println(packetBuffer);
  }

  // Check if user has entered a message in the Serial Monitor to send a UDP packet
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    // Split the input into IP, port, and message using ":" as a delimiter
    int firstColon = input.indexOf(':');
    int secondColon = input.indexOf(':', firstColon + 1);

    if (firstColon != -1 && secondColon != -1) {
      String ipString = input.substring(0, firstColon);
      String portString = input.substring(firstColon + 1, secondColon);
      String message = input.substring(secondColon + 1);

      IPAddress remoteIP;
      remoteIP.fromString(ipString);  // Convert the IP string to an IPAddress object
      unsigned int remotePort = portString.toInt();  // Convert the port string to an integer

      // Send the UDP packet to the specified IP and port
      udp.beginPacket(remoteIP, remotePort);
      udp.write((const uint8_t*)message.c_str(), message.length());  // Write the message as uint8_t (byte array)
      udp.endPacket();

      Serial.print("Sent UDP packet to ");
      Serial.print(remoteIP);
      Serial.print(":");
      Serial.println(remotePort);
      Serial.print("Message: ");
      Serial.println(message);
    } else {
      Serial.println("Invalid input format. Use: ip:port:message");
    }
  }
}

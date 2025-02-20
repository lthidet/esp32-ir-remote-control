#include <WiFi.h>
#include <HTTPClient.h>
#include <IRremote.hpp>

// --- WiFi and Server Configuration ---
const char* ssid = "Your_WiFi_SSID";               // Replace with your WiFi SSID
const char* password = "Your_WiFi_Password";       // Replace with your WiFi password
const char* serverUrl = "http://your.server.url";  // Replace with your server URL

// --- IR receiver pin ---
#define IR_RECEIVE_PIN 13  // Pin connected to the IR receiver

// --- Setup Function ---
void setup() {
  Serial.begin(115200); // Initialize serial communication for debugging

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi.");
  Serial.print("Server address: ");
  Serial.println(serverUrl);

  // Initialize IR receiver
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("IR Receiver Ready");
}

// --- Main Loop ---
void loop() {
  if (IrReceiver.decode()) {
    // Check if the received IR signal uses the NEC protocol
    if (IrReceiver.decodedIRData.protocol == NEC) {
      uint8_t cmd = IrReceiver.decodedIRData.command; // Extract the button command

      // Handle button presses
      switch (cmd) {
        case 0x16:  // Button "0"
          Serial.println("Button 0 pressed");
          sendRequest("/play/0"); // Example: Send play/0 to the server
          break;
        case 0xC:   // Button "1"
          Serial.println("Button 1 pressed");
          sendRequest("/play/1"); // Example: Send play/1 to the server
          break;
        case 0x18:  // Button "2"
          Serial.println("Button 2 pressed");
          sendRequest("/play/2"); // Example: Send play/2 to the server
          break;
        case 0x5E:  // Button "3"
          Serial.println("Button 3 pressed");
          sendRequest("/play/3"); // Example: Send play/3 to the server
          break;
        case 0x8:   // Button "4"
          Serial.println("Button 4 pressed");
          sendRequest("/play/4"); // Example: Send play/4 to the server
          break;
        case 0x1C:  // Button "5"
          Serial.println("Button 5 pressed");
          sendRequest("/play/5"); // Example: Send play/5 to the server
          break;
        case 0x5A:  // Button "6"
          Serial.println("Button 6 pressed");
          sendRequest("/play/6"); // Example: Send play/6 to the server
          break;
        case 0x42:  // Button "7"
          Serial.println("Button 7 pressed");
          sendRequest("/play/7"); // Example: Send play/7 to the server
          break;
        case 0x52:  // Button "8"
          Serial.println("Button 8 pressed");
          sendRequest("/play/8"); // Example: Send play/8 to the server
          break;
        case 0x4A:  // Button "9"
          Serial.println("Button 9 pressed");
          sendRequest("/play/9"); // Example: Send play/9 to the server
          break;
        case 0x45:  // Power button
          Serial.println("Power button pressed");
          sendRequest("/power"); // Example: Send power command to the server
          break;
        case 0x47:  // Menu button
          Serial.println("Menu button pressed");
          sendRequest("/menu"); // Example: Send menu command to the server
          break;
        case 0x40:  // Plus button (volume up)
          Serial.println("Plus button pressed");
          sendRequest("/volume/up"); // Example: Send volume up command to the server
          break;
        case 0x43:  // Back button
          Serial.println("Back button pressed");
          sendRequest("/back"); // Example: Send back command to the server
          break;
        case 0x7:   // Previous button
          Serial.println("Previous button pressed");
          sendRequest("/previous"); // Example: Send previous command to the server
          break;
        case 0x15:  // Play button
          Serial.println("Play button pressed");
          sendRequest("/play"); // Example: Send play command to the server
          break;
        case 0x9:   // Next button
          Serial.println("Next button pressed");
          sendRequest("/next"); // Example: Send next command to the server
          break;
        case 0x19:  // Minus button (volume down)
          Serial.println("Minus button pressed");
          sendRequest("/volume/down"); // Example: Send volume down command to the server
          break;
        case 0xD:   // C button
          Serial.println("C button pressed");
          sendRequest("/c"); // Example: Send C command to the server
          break;
        default:    // Unknown button
          Serial.print("Unknown command: 0x");
          Serial.println(cmd, HEX);
      }
    }
    IrReceiver.resume(); // Ready for the next IR packet
  }
}

// --- Send an HTTP POST request to the server ---
void sendRequest(String endpoint) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(String(serverUrl) + endpoint);
    int httpResponseCode = http.POST(""); // Send POST request
    if (httpResponseCode > 0) {
      Serial.print("Request sent: ");
      Serial.println(endpoint);
    } else {
      Serial.print("Error sending request: ");
      Serial.println(endpoint);
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}
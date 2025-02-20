/*
  ESP32 IR Remote Control for Audio Playback

  This script allows an ESP32 microcontroller to function as a remote control for audio playback.
  It uses an IR receiver to detect button presses from a remote control and sends corresponding
  HTTP requests to a FastAPI server to control audio playback (e.g., play, pause, volume control).
  The current track number is displayed on an OLED screen.

  Features:
  - IR remote control with NEC protocol support.
  - WiFi connectivity to communicate with a FastAPI server.
  - OLED display to show the current track number and playback status.
  - Track number input using numeric buttons (0-9) with timeout-based digit grouping.
  - Support for play, pause, volume control, and track navigation.

  Hardware:
  - ESP32 microcontroller.
  - IR receiver module (connected to pin 15).
  - OLED display (SSD1306, connected via I2C, pins 21 (SDA) and 22 (SCL)).

  Dependencies:
  - WiFi.h: For WiFi connectivity.
  - HTTPClient.h: For sending HTTP requests.
  - IRremote.hpp: For decoding IR signals.
  - U8g2lib.h: For controlling the OLED display.
  - ArduinoJson.h: For parsing JSON responses from the server.
*/

#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <IRremote.hpp>
#include <U8g2lib.h>
#include <ArduinoJson.h>

// --- OLED screen pins ---
#define SDA_PIN 25  // I2C data pin for OLED
#define SCL_PIN 26  // I2C clock pin for OLED
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, SCL_PIN, SDA_PIN); // Initialize OLED display

// --- IR receiver pin ---
#define IR_RECEIVE_PIN 13  // Pin connected to the IR receiver

// --- WiFi and Server Configuration ---
const char* ssid = "Your_WiFi_SSID";               // Replace with your WiFi SSID
const char* password = "Your_WiFi_Password";       // Replace with your WiFi password
const char* serverUrl = "http://your.server.url";  // Replace with your server URL

// --- Remote Logic ---
const unsigned long digitTimeout = 1000;  // Timeout (in milliseconds) for digit input
unsigned long lastDigitPressTime = 0;     // Timestamp of the last digit button press
#define MAX_TRACK_DIGITS 4                // Maximum number of digits for track number
int trackNumber = 1;                      // Current track number (built from digit presses)

// --- Setup Function ---
void setup() {
  Serial.begin(115200);

  // Initialize OLED display
  Wire.begin(SDA_PIN, SCL_PIN);
  u8g2.begin();
  updateDisplay();

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
        case 0x16:  // "0"
          Serial.println("Digit 0");
          processDigit(0);
          break;
        case 0xC:   // "1"
          Serial.println("Digit 1");
          processDigit(1);
          break;
        case 0x18:  // "2"
          Serial.println("Digit 2");
          processDigit(2);
          break;
        case 0x5E:  // "3"
          Serial.println("Digit 3");
          processDigit(3);
          break;
        case 0x8:   // "4"
          Serial.println("Digit 4");
          processDigit(4);
          break;
        case 0x1C:  // "5"
          Serial.println("Digit 5");
          processDigit(5);
          break;
        case 0x5A:  // "6"
          Serial.println("Digit 6");
          processDigit(6);
          break;
        case 0x42:  // "7"
          Serial.println("Digit 7");
          processDigit(7);
          break;
        case 0x52:  // "8"
          Serial.println("Digit 8");
          processDigit(8);
          break;
        case 0x4A:  // "9"
          Serial.println("Digit 9");
          processDigit(9);
          break;
        case 0x45:  // On/Off button
          Serial.println("On/Off button");
          sendRequest("/power");
          break;
        case 0x47:  // Menu button
          Serial.println("Menu button");
          sendRequest("/menu");
          break;
        case 0x44:  // Test button
          Serial.println("Test button");
          sendRequest("/test");
          break;
        case 0x40:  // Plus button (volume up)
          Serial.println("Plus button");
          sendRequest("/volume/up");
          break;
        case 0x43:  // Back button (remove last digit)
          Serial.println("Back button");
          trackNumber /= 10; // Remove last digit
          if (trackNumber == 0) trackNumber = 1; // Reset to 1 if no digits remain
          updateDisplay();
          break;
        case 0x7:   // Previous button
          Serial.println("Previous button");
          sendRequest("/previous");
          break;
        case 0x15:  // Play button
          Serial.println("Play button");
          sendPlayRequest();
          break;
        case 0x9:   // Next button
          Serial.println("Next button");
          sendRequest("/next");
          break;
        case 0x19:  // Minus button (volume down)
          Serial.println("Minus button");
          sendRequest("/volume/down");
          break;
        case 0xD:   // C button
          Serial.println("C button");
          sendRequest("/c");
          break;
        default:    // Unknown button
          Serial.print("Unknown command: 0x");
          Serial.println(cmd, HEX);
      }
    }
    IrReceiver.resume(); // Ready for the next IR packet
  }
}

// --- Update the OLED display with the current track number ---
void updateDisplay() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  char buf[20];
  sprintf(buf, "Track: %d", trackNumber);
  u8g2.drawStr(10, 20, buf);
  u8g2.sendBuffer();
}

// --- Send an HTTP POST request to the server ---
void sendRequest(String endpoint) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(String(serverUrl) + endpoint);
    int httpResponseCode = http.POST("");
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

// --- Send a REST request and parse the JSON response ---
bool sendRestRequest(const char* endpoint, const char* method, const char* payload, JsonDocument& responseDoc) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return false;
  }

  HTTPClient http;
  http.begin(String(serverUrl) + String(endpoint));

  int httpResponseCode = -1;
  if (strcmp(method, "GET") == 0) {
    httpResponseCode = http.GET();
  } else if (strcmp(method, "POST") == 0) {
    httpResponseCode = http.POST(payload);
  } else if (strcmp(method, "PUT") == 0) {
    httpResponseCode = http.PUT(payload);
  } else if (strcmp(method, "DELETE") == 0) {
    httpResponseCode = http.sendRequest("DELETE", payload); // Use sendRequest for DELETE
  } else {
    Serial.println("Unsupported HTTP method");
    http.end();
    return false;
  }

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Response: " + response);

    // Parse the JSON response
    DeserializationError error = deserializeJson(responseDoc, response);
    if (error) {
      Serial.print("JSON parsing failed: ");
      Serial.println(error.c_str());
      http.end();
      return false;
    }

    http.end();
    return true;
  } else {
    Serial.print("HTTP request failed: ");
    Serial.println(httpResponseCode);
    http.end();
    return false;
  }
}

// --- Send a play request to the server and display the track name on the OLED ---
void sendPlayRequest() {
  char path[32];
  snprintf(path, sizeof(path), "/play/%d", trackNumber);

  // --- Create a JSON document to store the response ---
  DynamicJsonDocument responseDoc(1024);

  // --- Send the REST request ---
  if (sendRestRequest(path, "POST", "", responseDoc)) {
    // Extract data from the JSON response
    const char* message = responseDoc["message"];
    const char* fileName = responseDoc["file_name"];

    // Display the file name on the OLED screen
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(10, 20, "Now Playing:");
    u8g2.drawStr(10, 40, fileName);
    u8g2.sendBuffer();

    Serial.println(message);
  } else {
    Serial.println("Failed to send play request");
  }
}

// --- Process a numeric button press to build the track number ---
void processDigit(int digit) {
  unsigned long now = millis();
  if (now - lastDigitPressTime > digitTimeout) {
    // Start a new track number
    trackNumber = digit;
  } else {
    // Append the digit to the current track number
    int numDigits = 0;
    int temp = trackNumber;
    while (temp > 0) { numDigits++; temp /= 10; }
    if (numDigits < MAX_TRACK_DIGITS) {
      trackNumber = trackNumber * 10 + digit;
    } else {
      // Reset to the new digit if max digits reached
      trackNumber = digit;
    }
  }
  lastDigitPressTime = now;
  updateDisplay();
}
// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ArduinoJson.h> // Include the ArduinoJson library
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <WiFiUdp.h>                                  


Adafruit_ADS1115 ads;

// Replace with your network credentials
const char* ssid = "";
const char* password = "";
const char* centralNodeHost = "nodecore";
const int centralNodePort = 3330;
const char* nodeName = "nodewindow";

// Define the JSON object
StaticJsonDocument<200> jsonDoc; // Adjust the capacity as needed
WiFiUDP udp;

void setup(){
  // Serial port for debugging purposes
  Serial.begin(9600);
  Serial.println("starting");

  // Connect to Wi-Fi
  WiFi.hostname(nodeName);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }
}

void loop() {
  String json_data = "{\"node\":\""+String(nodeName)+"\",\"opened\":true}";
  
  // Send the JSON data as a UDP packet
  udp.beginPacket(centralNodeHost, centralNodePort);
  udp.print(json_data);
  udp.endPacket();

  Serial.println("Data sent: " + json_data);
  delay(1000);
}



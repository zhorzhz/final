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
const char* nodeName = "nodepir";

// Define the JSON object
StaticJsonDocument<200> jsonDoc; // Adjust the capacity as needed
WiFiUDP udp;


const int motionPin1 = 14;
const int motionPin2 = 13;
const int motionPin3 = 15;
const int motionPin4 = 5;
int motionState = LOW;

void motionDetected(); // Declare the function

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
  pinMode(motionPin1, INPUT);
  pinMode(motionPin2, INPUT);
  pinMode(motionPin3, INPUT);
  pinMode(motionPin4, INPUT);
}

void checkMotion(int motionPin) {
  long state = digitalRead(motionPin);
  if(state == HIGH) {
    Serial.println("Motion detected! - "+String(motionPin));
    String json_data = "{\"node\":\""+String(nodeName)+"\",\"motion_detected\":true}";
    
    // Send the JSON data as a UDP packet
    udp.beginPacket(centralNodeHost, centralNodePort);
    udp.print(json_data);
    udp.endPacket();

    Serial.println("Data sent: " + json_data);
  }
  else {
    Serial.println("Motion absent!");
  }
}

void loop() {
  checkMotion(motionPin1);
  checkMotion(motionPin2);
  checkMotion(motionPin3);
  checkMotion(motionPin4);
  delay(500);
}



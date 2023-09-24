#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>  // Include the ArduinoJson library
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define BUTTON_PIN 14

// Define the screen states
enum ScreenState {
  NODE_STATION,
  PLANT_STATION,
  CAMERA_STATION,
  ALERT
};

ScreenState currentScreen = NODE_STATION;

const int buzzerPin = 12;  // Connect the passive buzzer to D3
const int alarmLedPin = 15;
const int RXPin = 9;

// Define your WiFi network credentials
const char* ssid = "";
const char* password = "";

// Define the UDP server details
const int udpPort = 3330;  // Port to listen on
WiFiUDP udp;

// Define the size of the JSON buffer
const size_t bufferSize = 128;
StaticJsonDocument<bufferSize> jsonBuffer;

// Define dynamic data variables that we will get from nodecore
String intruders[];
float temperature;
int humidity;
String lightning;
int soilMoisture;

void displayStatus(String status) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("SmartHome v1.0");
  display.setTextSize(1);
  display.setCursor(0, 22);
  display.println(status);
  display.display();
}


void setup() {
  // Initialize the serial communication for debugging
  Serial.begin(9600);

  // Initialize the LED and buzzer pins
  pinMode(alarmLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(RXPin, INPUT);
  tone(buzzerPin, 1000, 2000);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  displayStatus("WiFi...");

  // Connect to Wi-Fi
  WiFi.hostname("nodemobile");
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  int wifiConnectionCounter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
    wifiConnectionCounter++;
    String connectionDots = "";
    for (int i = 0; i < wifiConnectionCounter; i++) {
      connectionDots += ".";
    }
    displayStatus("WiFi" + connectionDots);
  }
  Serial.println("Connected to WiFi");

  displayStatus("UDP...");
  udp.begin(udpPort);

  // Clear the buffer
  display.clearDisplay();
  // Setup the button pin as an input with pull-up resistor
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}


bool soundAlarm = false;

// sound police siren with led lights flashing
void sirenSound() {
  for (int i = 0; i < 3; i++) {
    for (int hz = 440; hz < 1000; hz++) {
      tone(buzzerPin, hz, 50);
      delay(5);
    }

    for (int hz = 1000; hz > 440; hz--) {
      tone(buzzerPin, hz, 50);
      delay(5);
    }
  }
}

int pin_state = 0;
long DELAY = 0;
unsigned long previousMillis = 0;
unsigned long prev_counter = 0;
unsigned long counter = 0;

void updateScreen() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    Serial.println("button clicked");
    // Button is pressed, change to the next screen
    changeToNextScreen();
    delay(500);  // Debounce delay to avoid multiple screen changes on one button press
  }
  switch (currentScreen) {
    case NODE_STATION:
      displayNodeStation();
      break;
    case PLANT_STATION:
      displayPlantStation();
      break;
    case CAMERA_STATION:
      displayCameraStation();
      break;
    case ALERT:
      displayAlert("none");
      break;
  }
}

void changeToNextScreen() {
  // Change to the next screen state
  currentScreen = static_cast<ScreenState>((currentScreen + 1) % ALERT);
}

void displayNodeStation() {
  // Clear the display
  display.clearDisplay();

  // Display title and information
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("----Node Station----");

  // Display temperature
  display.setCursor(0, 16);
  display.print("Temperature: ");
  display.print(temperature);
  display.print("C");

  // Display humidity
  display.setCursor(0, 32);
  display.print("Humidity: ");
  display.print(humidity);
  display.print("%");

  // Display lightning status
  display.setCursor(0, 48);
  display.print("Lightning: ");
  display.print(lightningStatus);

  display.display();
}

void displayPlantStation() {
  // Clear the display
  display.clearDisplay();

  // Display title and information
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("----Plant Station----");

  // Display temperature
  display.setCursor(0, 16);
  display.print("Temperature: ");
  display.print(temperature);
  display.print("C");

  // Display humidity
  display.setCursor(0, 28);
  display.print("Humidity: ");
  display.print(humidity);
  display.print("%");

  // Display soil moisture
  display.setCursor(0, 40);
  display.print("Soil Moisture: ");
  display.print(soilMoisture);
  display.print("%");

  // Display sunlight
  display.setCursor(0, 52);
  display.print("Sunlight: ");
  display.print(sunlight);
  display.print("%");

  display.display();
}

void displayCameraStation() {
  // Clear the display
  display.clearDisplay();

  // Display title and information
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("----Camera Station---");
  display.setCursor(0, 16);
  display.println("Detected objects:");

  // Iterate through the intruders array and display each entry
  int yOffset = 28; // Initial vertical offset for displaying intruders
  for (int i = 0; i < sizeof(intruders) / sizeof(intruders[0]); i++) {
    display.setTextSize(1);
    display.setCursor(0, yOffset);
    display.print(intruders[i]);
    yOffset += 12; // Increase the vertical offset for the next entry
  }

  display.display();
}

void displayAlert(String intruder) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("---ALERT---");
  display.setTextSize(2);
  display.setCursor(0, 22);
  display.println("Detected");
  display.setCursor(0, 44);
  display.println(intruder);
  display.display();
}

void loop() {
  updateScreen();
  // Serial.println("loop started");
  // Listen for UDP packets
  int packetSize = udp.parsePacket();
  if (packetSize) {
    Serial.println("Received packet: " + String(packetSize));
    char incomingPacket[255];
    udp.read(incomingPacket, packetSize);
    incomingPacket[packetSize] = '\0';  // Null-terminate the string

    Serial.println("Received UDP packet: " + String(incomingPacket));

    // Parse the incoming JSON
    deserializeJson(jsonBuffer, incomingPacket);

    // Check if the received message matches a specific trigger message
    if (jsonBuffer.containsKey("alarm") && jsonBuffer["alarm"] == true) {
      displayAlert(jsonBuffer["intruder"]);
      // Trigger the alarm (turn on the buzzer)
      sirenSound();

      noTone(buzzerPin);
      delay(1000);
    }

    temperature = jsonBuffer["temperature"]);
    humidity = jsonBuffer["humidity"]);
    lightning = jsonBuffer["lightning"]);
    soilMoisture = jsonBuffer["soilMoisture"]);
    intruders = jsonBuffer["lastIntruders"]);
  }
}

#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Shaluka's iphone ";
const char* password = "00000000";
const char* server = "http://api.thingspeak.com/update?api_key=6V88MIO3CQNS6G9Z";

U8G2_HX1230_96X68_F_3W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 18, /* data=*/ 5, /* cs=*/ 4, /* reset=*/ 2);
#define BUTTON_PIN_NEXT 12 // Define the pin for the existing next button
#define BUTTON_PIN_PREV 13 // Define the pin for the previous button

const int sensorPin = 34;  // Analog input pin that the voltage sensor is attached to
const float maxADCValue = 4095.0;  // Maximum value the ADC can read (12-bit ADC)
const float referenceVoltage = 3.3;  // ADC reference voltage
const float voltageDividerRatio = 5.0;  // Voltage divider ratio (for example, 5:1)
const float calibrationFactor = 1.059; // Calibration factor to adjust the voltage reading
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

float baselineVoltage = 0.0;
int pressCount = 0; // Variable to count the number of button presses
int buttonStateNext = HIGH; // Variable to store the current next button state
int lastButtonStateNext = HIGH; // Variable to store the previous next button state
int buttonStatePrev = HIGH; // Variable to store the current previous button state
int lastButtonStatePrev = HIGH; // Variable to store the previous previous button state

unsigned long previousMillis = 0; // Stores the last time the measurement was updated
const long interval = 2000; // Interval at which to update (2 seconds)
unsigned long previousMillisThingSpeak = 0; // Timer for ThingSpeak updates
const long thingSpeakInterval = 15000; // Interval at which to send data to ThingSpeak (20 seconds)

bool firstPress = true; // Variable to track the first button press

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  u8g2.begin();
  
  pinMode(BUTTON_PIN_NEXT, INPUT_PULLUP); // Configure next button pin as input with internal pull-up resistor
  pinMode(BUTTON_PIN_PREV, INPUT_PULLUP); // Configure previous button pin as input with internal pull-up resistor

  // Initialize BME280 sensor
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  // Read the baseline voltage when the probes are not connected
  baselineVoltage = readVoltage();
  Serial.print("Baseline Voltage: ");
  Serial.println(baselineVoltage);

  // Initial display message
  u8g2.clearBuffer();
  drawBorder();
  u8g2.setFont(u8g2_font_helvB10_tr); // Select font
  u8g2.drawStr(30, 15, "Multi");
  u8g2.drawStr(10, 30, "Parameter");
  u8g2.drawStr(20, 45, "Module");
  u8g2.setFont(u8g2_font_helvB08_tr);
  u8g2.drawStr(50, 60, "(SLAEB)");
  u8g2.sendBuffer();

  delay(5000);

  u8g2.clearBuffer();
  drawBorder();
  u8g2.setFont(u8g2_font_ncenR10_tr);
  u8g2.drawStr(10, 20, "Press Next");
  u8g2.drawStr(20, 40, "To Read");
  u8g2.drawStr(0, 60, "Measurement");
  u8g2.sendBuffer();
}

void loop() {
  unsigned long currentMillis = millis();

  buttonStateNext = digitalRead(BUTTON_PIN_NEXT); // Read the current next button state
  buttonStatePrev = digitalRead(BUTTON_PIN_PREV); // Read the current previous button state

  // Check if the next button is pressed and was not pressed in the last loop
  if (buttonStateNext == LOW && lastButtonStateNext == HIGH) {
    pressCount++; // Increment the press count
    pressCount = pressCount % 6; // Ensure pressCount wraps around after 6 presses
    previousMillis = currentMillis; // Reset the timer to ensure immediate update

    // Clear the screen and display the appropriate reading based on press count immediately
    u8g2.clearBuffer();
    drawBorder();
    displayMeasurement();

    // Debounce delay
    delay(500); // A short delay to debounce the button
    firstPress = false; // Set firstPress to false after the first button press
  }

  // Check if the previous button is pressed and was not pressed in the last loop
  if (buttonStatePrev == LOW && lastButtonStatePrev == HIGH) {
    pressCount--; // Decrement the press count
    if (pressCount < 0) pressCount = 5; // Ensure pressCount wraps around after 0 to 5
    previousMillis = currentMillis; // Reset the timer to ensure immediate update

    // Clear the screen and display the appropriate reading based on press count immediately
    u8g2.clearBuffer();
    drawBorder();
    displayMeasurement();

    // Debounce delay
    delay(500); // A short delay to debounce the button
    firstPress = false; // Set firstPress to false after the first button press
  }

  // Regular interval updates only after the first button press
  if (!firstPress && (currentMillis - previousMillis >= interval)) {
    previousMillis = currentMillis; // Save the last update time

    // Clear the screen and display the appropriate reading based on press count
    u8g2.clearBuffer();
    drawBorder();
    displayMeasurement();
  }

  // Send all sensor data to ThingSpeak every 20 seconds
  if (currentMillis - previousMillisThingSpeak >= thingSpeakInterval) {
    previousMillisThingSpeak = currentMillis;
    sendSensorDataToThingSpeak();
  }

  lastButtonStateNext = buttonStateNext; // Update the previous next button state
  lastButtonStatePrev = buttonStatePrev; // Update the previous previous button state
}

void drawBorder() {
  u8g2.drawFrame(0, 0, u8g2.getWidth(), u8g2.getHeight());
}

void displayMeasurement() {
  switch (pressCount) {
    case 0:
      u8g2.setFont(u8g2_font_helvB10_tr);
      u8g2.drawStr(1, 20, "Press Again");
      u8g2.drawStr(35, 40, "To");
      u8g2.drawStr(10, 60, "Continue");
      break;
    case 1:
      displayTemperature();
      break;
    case 2:
      displayHumidity();
      break;
    case 3:
      displayPressure();
      break;
    case 4:
      displayAltitude();
      break;
    case 5:
      displayVoltage();
      break;
  }
  u8g2.setFont(u8g2_font_profont10_tr);
  u8g2.drawStr(2, 65, "Back");
  u8g2.drawStr(73, 65, "Next");
  u8g2.sendBuffer();
}

void displayTemperature() {
  float temperature = bme.readTemperature();
  u8g2.setFont(u8g2_font_profont15_tr);
  u8g2.drawStr(3, 20, "Temperature:");
  char buffer[10];
  u8g2.setFont(u8g2_font_ncenR10_tr);
  dtostrf(temperature, 4, 2, buffer);
  u8g2.drawStr(20, 40, buffer);
  u8g2.drawStr(60, 40, "C");
}

void displayHumidity() {
  float humidity = bme.readHumidity();
  u8g2.setFont(u8g2_font_profont15_tr);
  u8g2.drawStr(3, 20, "Humidity:");
  char buffer[10];
  u8g2.setFont(u8g2_font_ncenR10_tr);
  dtostrf(humidity, 4, 2, buffer);
  u8g2.drawStr(20, 40, buffer);
  u8g2.drawStr(60, 40, "%");
}

void displayPressure() {
  float pressure = bme.readPressure() / 100.0F;  // Convert to hPa
  u8g2.setFont(u8g2_font_profont15_tr);
  u8g2.drawStr(3, 20, "Pressure:");
  char buffer[10];
  u8g2.setFont(u8g2_font_ncenR10_tr);
  dtostrf(pressure, 4, 1, buffer);
  u8g2.drawStr(20, 40, buffer);
  u8g2.drawStr(60, 40, "hPa");
}

void displayAltitude() {
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  u8g2.setFont(u8g2_font_profont15_tr);
  u8g2.drawStr(3, 20, "Altitude:");
  char buffer[10];
  u8g2.setFont(u8g2_font_ncenR10_tr);
  dtostrf(altitude, 4, 1, buffer);
  u8g2.drawStr(20, 40, buffer);
  u8g2.drawStr(60, 40, "m");
}

void displayVoltage() {
  float voltage = readVoltage() - baselineVoltage; // Subtract baseline to get the actual voltage
  u8g2.setFont(u8g2_font_profont15_tr);
  u8g2.drawStr(3, 20, "Voltage:");
  char buffer[10];
  u8g2.setFont(u8g2_font_ncenR10_tr);
  dtostrf(voltage, 4, 2, buffer);
  u8g2.drawStr(20, 40, buffer);
  u8g2.drawStr(60, 40, "V");
}

float readVoltage() {
  int sensorValue = analogRead(sensorPin);
  float voltage = (sensorValue / maxADCValue) * referenceVoltage * voltageDividerRatio * calibrationFactor;
  return voltage;
}

void sendSensorDataToThingSpeak() {
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F; // Convert to hPa
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float voltage = readVoltage() - baselineVoltage; // Subtract baseline to get the actual voltage
  
  HTTPClient http;

  // Construct URL with all fields
  String url = String(server) +
                "&field1=" + String(temperature) +
                "&field2=" + String(humidity) +
                "&field3=" + String(pressure) +
                "&field4=" + String(altitude) +
                "&field6=" + String(voltage);
  
  http.begin(url);
  
  int httpResponseCode = http.GET();
  
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

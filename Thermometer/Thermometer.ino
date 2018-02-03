#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ConfigManager.h>
#include "SparkFun_Si7021_Breakout_Library.h"
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

float humidity = 0;
float temp = 0;

int counter = 0;

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
// Stores the last time the temp was recorded
unsigned long previousMillis;

//Create Instance of SI7021 temp and humidity sensor
Weather sensor;
ConfigManager configManager;

struct Config {
    char endpoint[50];
    bool useSleep;
    int sleepSeconds;
    
} config;

void setup() {
  Serial.begin(115200);

  previousMillis = -2000000;

  configManager.setAPName("Demo");
  configManager.setAPFilename("/index.html");
  configManager.addParameter("endpoint", config.endpoint, 50);
  configManager.addParameter("useSleep", &config.useSleep);
  configManager.addParameter("sleepSeconds", &config.sleepSeconds);
  configManager.begin(config);

//  ArduinoOTA.setHostname("ESP-Update");
//  ArduinoOTA.setPassword((const char *)"password");
//  ArduinoOTA.onStart([]() {
//    Serial.println("Starting OTA update");
//  });
//  ArduinoOTA.onEnd([]() {
//    Serial.println("\nFinished OTA update");
//  });
//  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
//    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
//  });
//  ArduinoOTA.onError([](ota_error_t error) {
//    Serial.printf("Error[%u]: ", error);
//    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
//    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
//    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
//    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
//    else if (error == OTA_END_ERROR) Serial.println("End Failed");
//  });
//  ArduinoOTA.begin();


  //Initialize the I2C sensors and ping them
  sensor.begin();
}

void loop() {
  configManager.loop();
  //ArduinoOTA.handle();

  // Check to see if it's time to record the temp.
  // If the difference between the current time and last time we got the temp
  // is bigger than the interval at which we want to record the temp.
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= (1000 * config.sleepSeconds)) {
    // Update our temp record time to now
    previousMillis = currentMillis;

    recordTemp();
  }
}

void recordTemp() {
  // Measure Relative Humidity from the Si7021
  humidity = sensor.getRH();

  // Measure Temperature from the Si7021
  temp = sensor.getTemp();

  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print(" C, ");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");

  HTTPClient http;
  http.begin(String(config.endpoint) + "?tempc=" + String(temp) + "&humidity=" + String(humidity));
  int httpCode = http.GET();
  if(httpCode == HTTP_CODE_OK) {
    String response = http.getString();

    if (response != "" && response != "0") {
      Serial.println(response);
      if (response == "1") {
        // Stop going to sleep until the value is updated using Postman
        config.useSleep = false;
      }
    }
  } else {
     Serial.print("Error in HTTP request: ");
     Serial.println(httpCode);
  }
  http.end();

  if (config.useSleep) {
    // 1,000,000 microseconds is 1 second
    Serial.println("Going into deep sleep for " + String(config.sleepSeconds) + " seconds");
    ESP.deepSleep(1000000 * config.sleepSeconds);
  } else {
    Serial.println("Waiting for " + String(config.sleepSeconds) + " seconds");
  }
}

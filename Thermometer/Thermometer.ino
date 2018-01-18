#include "SparkFun_Si7021_Breakout_Library.h"
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid     = "Darkhalf";
const char* password = "";

float humidity = 0;
float temp = 0;

//Create Instance of HTU21D or SI7021 temp and humidity sensor and MPL3115A2 barrometric sensor
Weather sensor;

void setup() {
  Serial.begin(115200);
  delay(60000);

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  //TODO: Figure out what to do about infinite loop.
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected: ");  
  Serial.println(WiFi.localIP());

  //Initialize the I2C sensors and ping them
  sensor.begin();

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
  http.begin("http://192.168.1.10:8078/api/thermometer?tempc=" + String(temp) + "&humidity=" + String(humidity));
  int httpCode = http.GET();
  if(httpCode == HTTP_CODE_OK)
  {
     Serial.print("HTTP response code ");
     Serial.println(httpCode);
     String response = http.getString();
     Serial.println(response);
  }
  else
  {
     Serial.println("Error in HTTP request");
  }
   
  http.end();

 
  //Serial.println("Going into deep sleep for 20 seconds");
  //ESP.deepSleep(20e6); // 20e6 is 20 microseconds
  Serial.println("Going into deep sleep for 10 minutes");
  ESP.deepSleep(600000000); // 20e6 is 20 microseconds
}

void loop() {
  
}


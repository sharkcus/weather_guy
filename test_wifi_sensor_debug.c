#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#ifndef STASSID
#define STASSID "<WIFI ssid>"
// #define STAPSK  "<WIFI password>"
#endif

// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;     //D2 pin of nodemcu
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

const char* ssid = STASSID;
// const char* password = STAPSK;

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  // prepare LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

//  WiFi.mode(WIFI_STA);
 WiFi.begin(ssid);
  // Start the DS18B20 sensor
 sensors.begin();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));
  // Start the server
  server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.println(WiFi.localIP());
}


float getTemperature() {
  Serial.print("Requesting temperature...");
  float temp;

  // Bug found that reports 85 degres, but with this it gets solved
  do { 
    sensors.requestTemperatures();
    temp = sensors.getTempCByIndex(0);
    delay(100);
  } while (temp == 85.0 || temp == (-127.0));
  return temp;
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Send the response to the client and add an auto refresh
  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML><HEAD><meta http-equiv='refresh' content='5; URL=http://"));
  client.print(WiFi.localIP());
  client.print(F("/index.html'></HEAD>\r\n<html>\r\n<H1>Debug</h1>"));
  
 
  float temperature = getTemperature();
  Serial.print(temperature);
  Serial.println("ÂºC");

  client.print(F("<br>Temperature = "));
  client.print(temperature);
  client.print(F(" degree celsius </html> "));
  delay(500);

}

#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>
#define DHTPIN D2     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors
DHT dht(DHTPIN, DHTTYPE);
#ifndef STASSID
#define STASSID "<WIFI ssid>"
// #define STAPSK  "<Your WIFI password>"
#endif
const char* ssid = STASSID;
WiFiServer server(80);
void setup() {
  dht.begin();
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
void loop() {
    Serial.println("Posting JSON data to elastic...");
    HTTPClient http;   
    WiFiClient WifiClient;
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);
    Serial.println(F("Temp: "));
    Serial.println(f);
    Serial.println(F("Humidity: "));
    Serial.println(h);
    http.begin(WifiClient, "http://<dest_ip>:<port>");  
    http.addHeader("Content-Type", "application/json");          
    StaticJsonDocument<200> doc;
    doc["humidity"] = h;
    doc["temp"] = f;
    doc["<tag>"] = "true"; // tag for index
    String requestBody;
    serializeJson(doc, requestBody);
    int httpResponseCode = http.POST(requestBody);
    if(httpResponseCode>0){
      String response = http.getString();                       
      Serial.println(httpResponseCode); 
      Serial.println(response);
    };
  delay(10000);
  }

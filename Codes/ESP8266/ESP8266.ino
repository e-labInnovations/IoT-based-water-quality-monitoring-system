#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "e-lab innovations";
const char* password = "PASSWORD";
String recievedData;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("Connecting...");
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (Serial.available() > 0)  {
      recievedData = Serial.readString();

      HTTPClient http; //Object of class HTTPClient
      http.begin("http://api.thingspeak.com/update?api_key=WUPISBA0KJ2W0IV7&" + recievedData);
      int httpCode = http.GET();

      if (httpCode > 0) {
        Serial.println("Updated");
      }
      http.end(); //Close connection
    }
  } else {
    Serial.println("Connection Loss");
    delay(2500);
  }
}

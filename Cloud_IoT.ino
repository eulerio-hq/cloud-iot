#include "arduino_secrets.h"
#include <WiFiNINA.h>
#include <Arduino_MKRIoTCarrier.h>

#define SAMPLING_INTERVAL 1800000
MKRIoTCarrier carrier;

int status = WL_IDLE_STATUS;
char ssid[] = SECRET_SSID;
char pass[] = SECRET_SSID_PASS;
String apiKey = SECRET_API_KEY;

char server[] = "eulerio.com";
WiFiSSLClient client;

float temperature;
float humidity;
int light;

void setup() {
  Serial.begin(9600);
  
  while(status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(1000);
  }
  Serial.println("Connected to wifi");
  while(!Serial);
  CARRIER_CASE = true;
  carrier.begin();
}

void loop() {
  temperature = carrier.Env.readTemperature();
  humidity = carrier.Env.readHumidity();
  if (carrier.Light.colorAvailable()) {
    int none;
    carrier.Light.readColor(none, none, none, light);
    
  }
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" C");
  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("Light = ");
  Serial.print(light);
  Serial.println(" LUX");
  send();
  delay(SAMPLING_INTERVAL);
}

void send() {
  Serial.println("\nStarting connection to server...");
  if (client.connectSSL(server, 443)) {
    Serial.println("connected to server");
    String body = "{\"temp\":" + String(temperature) + 
                  ",\"humidity\":" + String(humidity) + 
                  ",\"light\":" + String(light) +  "}";
    client.println("POST /api/fado/sensor/ HTTP/1.1");
    client.println("Host: eulerio.com");
    client.println("Authorization: Bearer " + apiKey);
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Content-Type: application/json");
    client.println("Content-Length: " + String(body.length()));
    client.println("Connection: close");
    client.println();
    client.println(body);
    client.println();
  } else {
    Serial.println("unable to connect");
  }
  delay(1000);
  
  while(client.available()) {
    char c = client.read();
    Serial.write(c);
  }
  
  if(!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server");
    client.stop();
  }
}

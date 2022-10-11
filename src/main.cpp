#include <Arduino.h>
#include <PubSubClient.h>
#include <NCD24Relay.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

NCD24Relay relay;

WiFiClientSecure wClient;
WiFiClient wClientOpen;
PubSubClient mqttClient(wClient);

char* wifiSSID = (char*)"STARLINK";
char* ssidPassword = (char*)"Spunky11";

char* mqttHost = (char*)"mqtt.beebotte.com";
int mqttPort = 1883;
char* mqttUsername = (char*)"token:token_EKBovij7uFArKIPW";
char* mqttPassword = (char*)"";
char* clientID = (char*)"relay_board";
char* subscribeTopic = (char*)"Article_Channel/relay";

void mqttMessageCallback(char* topic, byte* payload, unsigned int length);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  relay.setAddress(0, 0);
  for(int i = 1; i < 25; i++){
    relay.turnOnRelay(i);
    delay(200);
    relay.turnOffRelay(i);
    delay(200);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  //If WiFi is not connected then block here unitl it is connected
  if(!WiFi.isConnected()){
    WiFi.begin(wifiSSID, ssidPassword);
    Serial.print("Connecting WiFi");
    Serial.printf("SSID: %s\n", wifiSSID);
    Serial.printf("Password: %s\n", ssidPassword);
    while(WiFi.status() != WL_CONNECTED){
      Serial.print(".");
      delay(100);
    }
    Serial.println("WiFi Connected");
  }

  //Check MQTT Connection.  Connect if need be.  Block until connected
  if(!mqttClient.connected()){
    mqttClient.setClient(wClientOpen);
    mqttClient.setServer(mqttHost, mqttPort);
    if(strlen(mqttUsername) == 0){
      Serial.print("Connecting MQTT no auth");
      while(!mqttClient.connected()){
          if(mqttClient.connect(clientID)){
            mqttClient.setCallback(mqttMessageCallback);
            mqttClient.subscribe(subscribeTopic);
            break;
          }else{
            delay(1000);
            Serial.print(".");
          }
      }
      Serial.println("Connected");
    }else{
      if(strlen(mqttPassword) == 0){
        Serial.print("Connecting MQTT no password");
        while(!mqttClient.connected()){
            if(mqttClient.connect(clientID, mqttUsername, NULL)){
              mqttClient.setCallback(mqttMessageCallback);
              mqttClient.subscribe(subscribeTopic);
              break;
            }else{
              delay(1000);
              Serial.print(".");
            }
        }
        Serial.println("Connected");
      }else{
        Serial.print("Connecting MQTT Basic Auth");
        while(!mqttClient.connected()){
            if(mqttClient.connect(clientID, mqttUsername, mqttPassword)){
              mqttClient.setCallback(mqttMessageCallback);
              mqttClient.subscribe(subscribeTopic);
              break;
            }else{
              delay(1000);
              Serial.print(".");
            }
        }
        Serial.println("Connected");
      }

    }
  }
  mqttClient.loop();

}

void mqttMessageCallback(char* topic, byte* payload, unsigned int length){
  // String payloadString = String((char*)payload);
  Serial.printf("Topic: %s, Payload: %s\n", topic, (char*)payload);
  //Switch relays based on payload here.
  if(strstr((char*)payload,"relay_one_on") != NULL){
    relay.turnOnRelay(1);
    return;
  }
  if(strstr((char*)payload,"relay_one_off") != NULL){
    relay.turnOffRelay(1);
    return;
  }
}

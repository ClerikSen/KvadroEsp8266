/*
   Simpleton Sonoff Touch firmware with MQTT support
   Supports OTA update
   David Pye (C) 2016 GNU GPL v3
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <OneWire.h>
#include <ArduinoOTA.h>

#define NAME "kvadro"
#define SSID "DIR-300NRU"
#define PASS "7581730dd"

//Defaults to DHCP, if you want a static IP, uncomment and
//configure below
//#define STATIC_IP
#ifdef STATIC_IP
IPAddress ip(192, 168, 0, 60);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
#endif

#define MQTT_SERVER "m23.cloudmqtt.com"
#define MQTT_PORT 14384
#define MQTT_USER "srmdgryg"
#define MQTT_PASS "hdafBR01-knm"
#define BUFFER_SIZE 100

#define OTA_PASS "UPDATE_PW"
#define OTA_PORT 8266
const char *cmndTopic = "cmnd/" NAME "/mode";
const char *statusTopic = "status/" NAME "/light";

volatile int modes = 0;
volatile bool flag = false;

unsigned long lastMQTTCheck = -5000; //This will force an immediate check on init.

WiFiClient espClient;
PubSubClient client(espClient);
bool printedWifiToSerial = false;

void initWifi() {
  // We start by connecting to a WiFi network
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(SSID);
#ifdef STATIC_IP
  WiFi.config(ip, gateway, subnet);
#endif

  WiFi.begin(SSID, PASS);
}

void checkMQTTConnection() {
  //Serial.print("Checking MQTT connection: ");
  if (client.connected()){
  } //Serial.println("OK");
  else {
    if (WiFi.status() == WL_CONNECTED) {
      //Wifi connected, attempt to connect to server
      //Serial.print("new connection: ");
      if (client.connect(NAME,MQTT_USER,MQTT_PASS)) {
        //Serial.println("connected");
        client.subscribe(cmndTopic);
      } else {
        //Serial.print("failed, rc=");
        //Serial.println(client.state());
      }
    }
    else {
      //Wifi isn't connected, so no point in trying now.
      //Serial.println(" Not connected to WiFI AP, abandoned connect.");
    }
  }
  //Set the status LED to ON if we are connected to the MQTT server
  if (client.connected()){} //digitalWrite(LED_PIN, LOW);
    else{} //digitalWrite(LED_PIN, HIGH);
}

void MQTTcallback(char* topic, byte* payload, unsigned int length) {
  //Serial.print("Message arrived [");
  //Serial.print(topic);
  //Serial.println("] ");

  if (!strcmp(topic, cmndTopic)) {
    if ((char)payload[0] == '0') {
      modes = 0;
      Serial.write((byte)0);
      flag = true;
    }else if ((char)payload[0] == '1') {
      modes = 1;
      Serial.write((byte)1);
      flag = true;
    }else if ((char)payload[0] == '2') {
      modes = 2;
      Serial.write((byte)2);
      flag = true;
    }else if ((char)payload[0] == '3') {
      modes = 3;
      Serial.write((byte)3);
      flag = true;
    }else if ((char)payload[0] == '4') {
      modes = 4;
      Serial.write((byte)4);
      flag = true;
    }else if ((char)payload[0] == '5') {
      modes = 5;
      Serial.write((byte)5);
      flag = true;
    }else if ((char)payload[0] == '6') {
      modes = 6;
      Serial.write((byte)6);
      flag = true;
    }else if ((char)payload[0] == '7') {
      modes = 7;
      Serial.write((byte)7);
      flag = true;
    }else if ((char)payload[0] == '8') {
      modes = 8;
      Serial.write((byte)8);
      flag = true;
    }else if ((char)payload[0] == '9') {
      modes = 9;
      Serial.write((byte)9);
      flag = true;
    }else if ((char)payload[0] == '10') {
      modes = 10;
      Serial.write((byte)10);
      flag = true;
    }
  }
}

void setup() {
  Serial.begin(9600);
  //Serial.println("Initialising");
  
  initWifi();

  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(MQTTcallback);

  //OTA setup
  ArduinoOTA.setPort(OTA_PORT);
  ArduinoOTA.setHostname(NAME);
  ArduinoOTA.setPassword(OTA_PASS);
  ArduinoOTA.begin();
  
}

void loop() {
  //If we haven't printed WiFi details to Serial port yet, and WiFi now connected,
  //do so now. (just the once)
  //Serial.println("WiFi connected");
  //delay(500);
  if (!printedWifiToSerial && WiFi.status() == WL_CONNECTED) {
    //Serial.println("WiFi connected");
    //Serial.println("IP address: ");
    //Serial.println(WiFi.localIP());
    printedWifiToSerial = true;
  }

  if (millis() - lastMQTTCheck >= 5000) {
    checkMQTTConnection();
    lastMQTTCheck = millis();
  }

  //Handle any pending MQTT messages
  client.loop();

  //Handle any pending OTA SW updates
  ArduinoOTA.handle();

  //Relay state is updated via the interrupt *OR* the MQTT callback.
  if (flag) {
      if(modes == 0){
        client.publish(statusTopic, "0");
      }else if(modes == 1){
        client.publish(statusTopic, "1");
      }else if(modes == 2){
        client.publish(statusTopic, "2");
      }else if(modes == 3){
        client.publish(statusTopic, "3");
      }else if(modes == 4){
        client.publish(statusTopic, "4");
      }else if(modes == 5){
        client.publish(statusTopic, "5");
      }else if(modes == 6){
        client.publish(statusTopic, "6");
      }else if(modes == 7){
        client.publish(statusTopic, "7");
      }else if(modes == 8){
        client.publish(statusTopic, "8");
      }else if(modes == 9){
        client.publish(statusTopic, "9");
      }else if(modes == 10){
        client.publish(statusTopic, "10");
      }
      flag = false;
  }

  if(Serial.available()){
      String b = Serial.readString();
      //char* ch = ""+b;
      client.publish(statusTopic,b.c_str());
      /*if(b == 0){
        client.publish(statusTopic, "0");
      }else if(b == 1){
        client.publish(statusTopic, "1");
      }else if(b == 2){
        client.publish(statusTopic, "2");
      }else if(b == 3){
        client.publish(statusTopic, "3");
      }else if(b == 4){
        client.publish(statusTopic, "4");
      }else if(b == 5){
        client.publish(statusTopic, "5");
      }else if(b == 6){
        client.publish(statusTopic, "6");
      }else if(b == 7){
        client.publish(statusTopic, "7");
      }else if(b == 8){
        client.publish(statusTopic, "8");
      }else if(b == 9){
        client.publish(statusTopic, "9");
      }else if(b == 10){
        client.publish(statusTopic, "10");
      }*/
      
  }
  
  delay(50);
}


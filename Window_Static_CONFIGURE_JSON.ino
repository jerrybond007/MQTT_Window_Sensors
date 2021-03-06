#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
ADC_MODE(ADC_VCC);

//USER CONFIGURED SECTION START//
const char* ssid = "YOUR_WIRELESS_SSID";
const char* password = "YOUR_WIRELESS_SSID";
const char* mqtt_server = "YOUR_MQTT_SERVER_ADDRESS";
const int mqtt_port = 1883;
const char *mqtt_user = "YOUR_MQTT_USERNAME";
const char *mqtt_pass = "YOUR_MQTT_PASSWORD";
const char *mqtt_client_name = "PICK_UNIQUE_WINDOW_NAME"; // Client connections cant have the same connection name
const char *mqtt_topic = "PICK_UNIQUE_WINDOW_TOPIC";
IPAddress ip(192, 168, 86, 48); 
IPAddress gateway(192, 168, 86, 160); 
IPAddress subnet(255, 255, 255, 0);
//USER CONFIGURED SECTION END//

WiFiClient espClient;
PubSubClient client(espClient);

// Variables
bool boot = true;
char openMessage[50];
char closedMessage[50];

//Functions

void setup_wifi() 
{
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
    {
      delay(50);
    }
}

void reconnect() 
{
  while (!client.connected()) 
  {
      int battery_Voltage = ESP.getVcc() + 600;
      String temp_str = String(battery_Voltage);
      String mqttString1 = "{\"State\":\"closed\", \"voltage\":\"" + temp_str + "\"}";
      mqttString1.toCharArray(closedMessage, mqttString1.length() + 1);
      String mqttString2 = "{\"State\":\"open\", \"voltage\":\"" + temp_str + "\"}";
      mqttString2.toCharArray(openMessage, mqttString2.length() + 1);
      if (client.connect(mqtt_client_name, mqtt_user, mqtt_pass, mqtt_topic, 0, 1, closedMessage)) 
      {
        if(boot == true)
        {
          client.publish(mqtt_topic, openMessage);
          boot = false;
        }
      } 
      else 
      {
        ESP.restart();
      }
  }
}

void setup() 
{
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() 
{
  
  if (boot == true) 
  {
    reconnect();
  }
  else
  {
  ESP.deepSleep(0);
  }
}


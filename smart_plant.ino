/******************* 
*
* SMART PLANT
*   by Edo & Armin
*
*
*******************/


#include <ESP8266WiFi.h>
#include <PubSubClient.h>


/******************* CONSTANTS *******************/

#define SENSOR_PIN  A0

#define SSID_WIFI           "wlan_saltuaria"           
#define PASS_WIFI           "antlabpolitecnicomilano"  
#define MQTT_BROKER         "ec2-35-166-12-244.us-west-2.compute.amazonaws.com"     
#define MQTT_PORT            1883     
#define MQTT_TOPIC          "smart_plant/basil/1"

WiFiClient espClient;
PubSubClient client(espClient);

/******************* VARIABLES *******************/
int output_value ;
int output_value_raw;

void setup() {

   Serial.begin(9600);
  
   client.setServer(MQTT_BROKER, MQTT_PORT);
   ensure_connections();

   
   Serial.println("Reading From the Sensor ...");

   delay(2000);

 }

void loop() {


   output_value_raw= analogRead(SENSOR_PIN);

   //check 550 very wet and 10 dry
   output_value = map(output_value_raw,550,10,0,100);

   Serial.print("Mositure percentage: ");
   Serial.print(output_value);
   Serial.println("%");

   Serial.print("Moisture raw: ");
   Serial.println(output_value_raw);
   
   send_msg(output_value_raw);
   
   delay(60000);

 }


/******************* FUNCTIONS *******************/


bool send_msg(int val){
  ensure_connections();
   char buf [4];
   sprintf (buf, "%03i", val);  
   if (client.publish(MQTT_TOPIC, buf) == true) {
         Serial.println("Success sending message");
         return true;
      } else {
         Serial.println("Error sending message");
         return false;
      }

}

void ensure_connections(){
  if (wifi_connect()){
    if(!mqtt_connect()){
      Serial.println("very big MQTT problem, restart");
      while(1); //block the program
    }
  }
  else {
    Serial.println("very big Wi-Fi problem, restart");
    while(1); //block the program
  }
}

boolean wifi_connect() {
  Serial.println("Connecting to ");
  Serial.print(SSID_WIFI);

  if(WiFi.status() != WL_CONNECTED){
      WiFi.begin(SSID_WIFI, PASS_WIFI);
  }


  Serial.print("checking wifi..");
  int i = 0;
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    i++;
    if (i == 40) {
      Serial.println("WI-FI Connection Error!!!!");
      return 0;                     //too much time to connect, given up
    }
  }
  Serial.println("WiFi connected");
  return 1;  
}

boolean mqtt_connect() {
  int i = 0;
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("my_esp")) {
      Serial.println("connected");
      return 1;
    }
    else {
      Serial.print("failed mqtt");
      if (i > 1) {
        Serial.println("impossible to connect to MQTT");
        return 0;
      }
      Serial.println(" try again in 5 seconds");
      delay(3000);
    }
    i++;
  }
}

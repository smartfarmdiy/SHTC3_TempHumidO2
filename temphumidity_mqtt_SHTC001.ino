#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "SHTC3.h"


SHTC3 s(Wire);
 

// Change the credentials below, so your ESP8266 connects to your router
const char* ssid  = "xxxxxxx";  //example: xxGreenhousefarm
const char* password = " xxxxxxx"; //example: xxxxfarm1234

// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char* mqtt_server = "mqtt.eclipse.org";

char auth[]="xxxxxxxxxxx"; //example Token ID: e9733f00-829a-11ea-a505-873d744be10f or see in Application when you register 

// Initializes the espClient. You should change the espClient name if you have multiple ESPs running in your home automation system
WiFiClient espClient;
PubSubClient client(espClient);



const int lamp = 2; // LED on board




// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;

// Don't change the function below. This functions connects your ESP8266 to your router
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

// This functions is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that 
// your ESP8266 is subscribed you can actually do something
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic room/lamp, you check if the message is either on or off. Turns the lamp GPIO according to the message
  if(topic=="kalasintufarm/lamp"){
      Serial.print("Changing Room lamp to ");
      if(messageTemp == "on"){
        digitalWrite(lamp, HIGH);
        Serial.print("On");
      }
      else if(messageTemp == "off"){
        digitalWrite(lamp, LOW);
        Serial.print("Off");
      }
  }
  Serial.println();
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    
    if (client.connect("kalasintu8266Client")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("kalasintufarm/lamp");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// The setup function sets your ESP GPIOs to Outputs, starts the serial communication at a baud rate of 115200
// Sets your mqtt broker and sets the callback function
// The callback function is what receives messages and actually controls the LEDs
void setup() {
  
  Serial.begin(115200);
  Wire.begin();
  //dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

// For this project, you don't need to change anything in the loop function. Basically it ensures that you ESP is connected to your broker
void loop() {
  s.begin(true);
  s.sample();
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("kalasintu8266Client");

  now = millis();
  // Publishes new temperature and humidity every 30 seconds
  if (now - lastMeasure > 30000) {
    lastMeasure = now;
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = s.readHumidity();
    // Read temperature as Celsius (the default)
    float t = s.readTempC();

    //float hic = dht.computeHeatIndex(t, h, false);
    
    float oto = (32.0/(0.082*(273 + t)));
    


    

    String payloadaa = "{\"DeviceId\":\""+String(auth)+"\",\"Sensors\":[{\"Name\":\"Temptp\",\"Data\":\""+String(t)+"\"}]}";

    String payloadbb = "{\"DeviceId\":\""+String(auth)+"\",\"Sensors\":[{\"Name\":\"Humidy\",\"Data\":\""+String(h)+"\"}]}";

    //String payloadcc = "{\"DeviceId\":\""+String(auth)+"\",\"Sensors\":[{\"Name\":\"Heatin\",\"Data\":\""+String(hic)+"\"}]}";

    String payloaddd = "{\"DeviceId\":\""+String(auth)+"\",\"Sensors\":[{\"Name\":\"Oxygen\",\"Data\":\""+String(oto)+"\"}]}";


    // Publishes Temperature and Humidity values
   char attributes[100];
   char attributesb[100];
   //char attributesc[100];
   char attributesd[100];
   payloadaa.toCharArray( attributes, 100 );
   payloadbb.toCharArray( attributesb, 100 );
   //payloadcc.toCharArray( attributesc, 100 );
   payloaddd.toCharArray( attributesd, 100 );
   client.publish("kalasintuaa2/smartfarmy", attributes );
   client.publish("kalasintubb2/smartfarmy", attributesb );
   client.publish("kalasintucc2/smartfarmy", attributesd );
   //Serial.println( attributes );
    
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(oto);
    Serial.print(" *g/l\t O2 value: ");
   
  }
} 

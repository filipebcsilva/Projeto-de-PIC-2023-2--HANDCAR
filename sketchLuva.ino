/*
 Basic ESP8266 MQTT example

 NEED to install: PubSub library 

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "Hello World" to the topic "pic1-20232/msg" every two seconds
  - subscribes to the topic "pic1-20232/led", printing out any messages
    it receives. It assumes the received payloads are strings not binary
  - If the first character of the topic "pic1-20232/led" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;


#define LED_BUILTIN 2  //For node_MCU Lolin esp8266

// Update these with values suitable for your network.
// ###################### Trocar aqui!!! #####################
const char* ssid = "internetFilipe";
const char* password = "fviw0926";
//Free mqtt server for testing
const char* mqtt_server = "broker.mqtt-dashboard.com";
//Local MQTT server - Tested using mosquitto mqtt for windows and linux
//const char* mqtt_server = "192.168.137.1";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

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

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    //digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    digitalWrite(LED_BUILTIN, LOW);  // Change for nodeMCU Lolin
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    //digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    digitalWrite(LED_BUILTIN, HIGH);  // Change for nodeMCU Lolin
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("pic1-20232/msg", "Hello World!!");
      // ... and resubscribe
      client.subscribe("pic1-20232/led");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(LED_BUILTIN, OUTPUT);  // Just for ESP8266 nodeMCU Lolin
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
  }
  Serial.println("");
  delay(100);
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  /* Print out the values */
  if (a.acceleration.y >= -3.0 && a.acceleration.y <= 3.0 && a.acceleration.x >= -3.0 && a.acceleration.x <= 3.0) {
    snprintf(msg, 50, "NADA");
  }
  if (a.acceleration.y <= -3.0 && a.acceleration.x >= -3.0 && a.acceleration.x <= 3.0) {
    snprintf(msg, 50, "FRENTE");
  }
  if (a.acceleration.y >= 3.0 && a.acceleration.x >= -3.0 && a.acceleration.x <= 3.0) {
    snprintf(msg, 50, "TRAS");
  }
  if (a.acceleration.x <= -3.0 && a.acceleration.y >= -3.0 && a.acceleration.y) {
    snprintf(msg, 50, "DIREITA");
  }
  if (a.acceleration.x >= 3.0 && a.acceleration.y >= -3.0 && a.acceleration.y) {
    snprintf(msg, 50, "ESQUERDA");
  }
  client.publish("pic1-20232/luvaFilipe", msg);
  delay(100);
}

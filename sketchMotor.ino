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
#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>

#define MOTOR1_PINO D6
#define MOTOR2_PINO D7
#define MOTOR3_PINO D4
#define MOTOR4_PINO D5


#define LED_BUILTIN 2  //For node_MCU Lolin esp8266


char string[50];
boolean temInformacao = false;
int r_len;

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
int contTotal = 0;
int contFrente = 0;
int contParado = 0;
int contTras = 0;
int contEsquerda = 0;
int contDireita = 0;
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
  //Serial.print("Message arrived [");
  //Serial.print(topic);
  //Serial.print("] ");
  r_len = length;
  for (int i = 0; i < length; i++) {
    string[i] = (char)payload[i];
  }
  Serial.println();
  temInformacao = true;
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
      client.subscribe("pic1-20232/luvaFilipe");
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
  pinMode(MOTOR1_PINO, OUTPUT);
  pinMode(MOTOR2_PINO, OUTPUT);
  pinMode(MOTOR3_PINO, OUTPUT);
  pinMode(MOTOR4_PINO, OUTPUT);

  //inicializações dos motores
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
    contTotal++;
    if (!strncmp(string, "NADA", r_len)) {
      contParado++;
    }
    if (!strncmp(string, "FRENTE", r_len)) {
      contFrente++;
    }
    if (!strncmp(string, "TRAS", r_len)) {
      contTras++;
    }
    if (!strncmp(string, "DIREITA", r_len)) {
      contDireita++;
    }
    if (!strncmp(string, "ESQUERDA", r_len)) {
      contEsquerda++;
    }
    if (contTotal == 5) {
      contTotal = 0;
      if (contParado > contFrente && contParado > contTras && contParado > contEsquerda && contParado > contDireita) {
        Serial.print("FICOU PARADO");
        digitalWrite(MOTOR1_PINO, LOW);
        digitalWrite(MOTOR2_PINO, LOW);
        digitalWrite(MOTOR3_PINO, LOW);
        digitalWrite(MOTOR4_PINO, LOW);
      }
      if (contDireita > contFrente && contDireita > contTras && contDireita > contEsquerda && contDireita > contParado) {
        Serial.print("DIREITA");
        digitalWrite(MOTOR1_PINO, LOW);
        digitalWrite(MOTOR2_PINO, LOW);
        digitalWrite(MOTOR3_PINO, LOW);
        digitalWrite(MOTOR4_PINO, HIGH);
      }
      if (contEsquerda > contFrente && contEsquerda > contTras && contEsquerda > contDireita && contEsquerda > contParado) {
        Serial.print("ESQUERDA");
        digitalWrite(MOTOR1_PINO, LOW);
        digitalWrite(MOTOR2_PINO, HIGH);
        digitalWrite(MOTOR3_PINO, LOW);
        digitalWrite(MOTOR4_PINO, LOW);
      }
      if (contFrente > contEsquerda && contFrente > contTras && contFrente > contDireita && contFrente > contParado) {
        Serial.print("FRENTE");
        digitalWrite(MOTOR1_PINO, LOW);
        digitalWrite(MOTOR2_PINO, HIGH);
        digitalWrite(MOTOR3_PINO, LOW);
        digitalWrite(MOTOR4_PINO, HIGH);
      }
      if (contTras > contEsquerda && contTras > contFrente && contTras > contDireita && contTras > contParado) {
        Serial.print("TRAS");
        digitalWrite(MOTOR1_PINO, HIGH);
        digitalWrite(MOTOR2_PINO, LOW);
        digitalWrite(MOTOR3_PINO, HIGH);
        digitalWrite(MOTOR4_PINO, LOW);
      }
      contTras = 0;
      contDireita = 0;
      contEsquerda = 0;
      contFrente = 0;
      contParado = 0;
    }
    
}
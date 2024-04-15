#include <WiFi.h>
#include <PubSubClient.h>

// Wifi Setup details
const char* ssid = "Daiyan";            // WiFi SSID
const char* password = "dairaz1711";     // WiFi Password

// MQTT details
const char* mqtt_server = "broker.emqx.io";  // MQTT broker server
const int mqtt_port = 1883;                   // MQTT broker port
const char* publish_topic = "SIT210/wave";    // MQTT topic to publish messages
const char* subscribe_topic = "SIT210/wave";  // MQTT topic to subscribe for messages

// Sensor and LED setup
const int trigPin = 7;  // Pin connected to the trigger pin of the ultrasonic sensor
const int echoPin = 6;  // Pin connected to the echo pin of the ultrasonic sensor
const int ledPin = 8;   // Pin connected to the LED

WiFiClient espClient;
PubSubClient client(espClient);

// Function to connect to WiFi
void setup_wifi() {
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Function to flash the LED thrice
void flashThrice() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
}

// Callback function to handle incoming MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Flash the LED if a message is received
  flashThrice();
}

// Function to reconnect to MQTT broker
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ArduinoClient")) {
      Serial.println("connected");
      client.subscribe(subscribe_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  client.subscribe(subscribe_topic);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Measure distance using ultrasonic sensor
  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;

  // Check if an object is within 50 cm
  if (distance < 50) {
    // Measure distance again to confirm
    long duration2 = pulseIn(echoPin, HIGH);
    long distance2 = duration * 0.034 / 2;
    // Check if the second measurement is less than the first one
    if (distance2 < distance) {
      Serial.println("Pat detected");
      client.publish(publish_topic, "You receive a friendly pat from Daiyan");
    } else {
      Serial.println("Wave detected!");
      client.publish(publish_topic, "Greetings from Daiyan");
      Serial.println(distance);
      Serial.println(distance2);
      Serial.println();
      delay(10000);
    }
  }
}

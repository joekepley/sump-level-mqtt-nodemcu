#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>
#include <Ticker.h>

#define LEVEL_PIN PIN_A0

#define WIFI_SSID "Sibley"
#define WIFI_PASSWORD "Kepley1020!"
#define MQTT_HOST IPAddress(10, 0, 0, 10)

#define MQTT_USER "sump"
#define MQTT_PASSWD "sump1234"
#define MQTT_PORT 1883

#define MQTT_PUB_TOPIC "sump/level/current"

long levelVal = 0;

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

unsigned long previousMillis = 0;   // Stores last time temperature was published
const long interval = 10000;        // Interval at which to publish sensor readings

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}


void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  digitalWrite(LED_BUILTIN, HIGH);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttPublish(uint16_t packetId) {
  Serial.print("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}
void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}


void setup() {

  Serial.begin(9600);

  while (!Serial);

  Serial.println("Starting Up...");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
  
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  // If your broker requires authentication (username and password), set them below
  mqttClient.setCredentials(MQTT_USER, MQTT_PASSWD);
  connectToWifi();


}
/*
Resistor config
Measurement range :33-220ohms
5v(P) - COM(G)  :4.2k ohms
COM(G) - GND(Y) :237 ohms

*/
void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
  // Every X number of seconds (interval = 10 seconds) 
  // it publishes a new MQTT message
  if (currentMillis - previousMillis >= interval) {
    // Save the last time a new reading was published
    /*
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    */
    previousMillis = currentMillis;
    Serial.println();
    levelVal = analogRead(LEVEL_PIN);

    long levelInt=map(levelVal, 537,957,0,1200);
    float level=float(levelInt)/100.0;

    Serial.print(level);
    Serial.print(" - ");
    Serial.println(levelVal);

  
    // Publish an MQTT message on topic esp/bme680/temperature
    uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_TOPIC, 1, true, String(level).c_str());
    Serial.printf("Publishing on topic %s at QoS 1, packetId: %i, ", MQTT_PUB_TOPIC, packetIdPub1);
    Serial.printf("Message: %.2f \n", level);

  }
}
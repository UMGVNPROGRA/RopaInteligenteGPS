#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> // Necesario para conexión segura TLS/SSL (puerto 8883)
#include <PubSubClient.h>
#include <SoftwareSerial.h> // Para comunicación con el GPS
#include <TinyGPS++.h>      // Para procesar los datos del GPS
#include <ArduinoJson.h>    // Para construir el JSON

// --------------------------- CONFIGURACIÓN WIFI Y MQTT -----------------------------
const char* ssid = "RedWifi";
const char* password = "ContraseñaWifi";

// 🔗 CONFIGURACIÓN DE TU BROKER HIVEMQ
const char* mqtt_server = "ed0fe265fc224c2f87f7e116929d24f3.s1.eu.hivemq.cloud"; 
const int mqtt_port = 8883;                 // Puerto TLS/SSL
const char* mqtt_user = "ropainteligente";
const char* mqtt_pass = "Abc123**";
const char* mqtt_client_id = "ESP8266_Ropa_01"; 

// 🟢 TÓPICO MQTT para la ubicación
const char* TOPIC_UBICACION = "ropa/ubicacion/coordenadas"; 

// 📍 CONFIGURACIÓN DE GPS (GY-NEO6MV2)
#define RX_GPS D5  // Conectado al TX del GPS (Pin D5/GPIO14 del Wemos)
#define TX_GPS D6  // Conectado al RX del GPS (Pin D6/GPIO12 del Wemos)

// ⚙️ PINES DE ENTRADA (PULSADORES) - Usando INPUT_PULLUP
const int PIN_DERECHA = D1;
const int PIN_IZQUIERDA = D2;
const int PIN_STOP = D3;
const int PIN_INTERMITENTE = D4;

// INSTANCIAS
WiFiClientSecure espClient; 
PubSubClient client(espClient);
SoftwareSerial ss(RX_GPS, TX_GPS); 
TinyGPSPlus gps;

// ---------------------- FUNCIONES DE CONEXIÓN ----------------------

void setup_wifi() {
  // ... (código de conexión WiFi)
  delay(10);
  Serial.println("\nConectando a " + String(ssid));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado. IP: " + WiFi.localIP().toString());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    if (client.connect(mqtt_client_id, mqtt_user, mqtt_pass)) {
      Serial.println("conectado");
    } else {
      Serial.print("falló, rc=");
      Serial.print(client.state());
      Serial.println(" Reintentando en 5 segundos");
      delay(5000);
    }
  }
}

void enviarComandoSerial(String comando) {
    // Envía el comando al Arduino Uno (Puerto Serial principal del Wemos)
    Serial.print(comando); 
    Serial.print('\n'); // El salto de línea es el delimitador para el Arduino Uno
}

// ---------------------- SETUP Y LOOP ----------------------

void setup() {
  Serial.begin(9600); // Para comunicación con la PC y con el Arduino Uno
  ss.begin(9600);     // Para comunicación con el GPS

  // Configuración de pines de entrada para pulsadores
  pinMode(PIN_DERECHA, INPUT_PULLUP); 
  pinMode(PIN_IZQUIERDA, INPUT_PULLUP);
  pinMode(PIN_STOP, INPUT_PULLUP);
  pinMode(PIN_INTERMITENTE, INPUT_PULLUP);
  
  setup_wifi();

  // Configuración de seguridad para HiveMQ Cloud
  espClient.setInsecure(); // Omitir verificación estricta del certificado
  client.setServer(mqtt_server, mqtt_port); 
  
  Serial.println("Wemos listo.");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // A. LECTURA DE PULSADORES Y ENVÍO DE COMANDO AL ARDUINO UNO
  if (digitalRead(PIN_DERECHA) == LOW) {
    enviarComandoSerial("DERECHA");
  } else if (digitalRead(PIN_IZQUIERDA) == LOW) {
    enviarComandoSerial("IZQUIERDA");
  } else if (digitalRead(PIN_STOP) == LOW) {
    enviarComandoSerial("STOP");
  } else if (digitalRead(PIN_INTERMITENTE) == LOW) {
    enviarComandoSerial("INTERMITENTE");
  } else {
    enviarComandoSerial("AVANCE"); // Comando por defecto (luz de avance/posicionamiento)
  }

  // B. LECTURA Y ENVÍO DE GPS REAL a MQTT
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }

  // Publicar cada vez que los datos de GPS se actualicen Y sean válidos
  if (gps.location.isUpdated() && gps.location.isValid() && client.connected()) {
    
    StaticJsonDocument<100> doc;
    doc["lat"] = gps.location.lat();
    doc["lng"] = gps.location.lng();
    
    char jsonBuffer[100];
    serializeJson(doc, jsonBuffer);
    
    // Publicar la ubicación al broker MQTT
    client.publish(TOPIC_UBICACION, jsonBuffer); 
    
    Serial.print("Ubicación Real Publicada: ");
    Serial.println(jsonBuffer);
  }

  delay(50); // Pequeña pausa
}
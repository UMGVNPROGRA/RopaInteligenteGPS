#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> // Para crear el JSON de coordenadas

// --------------------------- CONFIGURACIÓN WIFI -----------------------------
const char* ssid = "TU_WIFI_SSID";          // << CAMBIA ESTO
const char* password = "TU_WIFI_PASSWORD";  // << CAMBIA ESTO

// --------------------------- CONFIGURACIÓN MQTT -----------------------------
const char* mqtt_server = "ed0fe265fc224c2f87f7e116929d24f3.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;                 // Puerto TLS/SSL estándar de MQTT
const char* mqtt_user = "ropainteligente";
const char* mqtt_pass = "Abc123**";
const char* mqtt_client_id = "ESP8266_Ropa_01"; // ID único para el Wemos
const char* mqtt_topic = "ropa/ubicacion/coordenadas"; // Tópico que usa la web

// Inicialización de clientes
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;

// --------------------------- SIMULACIÓN GPS ---------------------------------
// Usamos coordenadas de ejemplo que luego se modificarán ligeramente
float current_lat = 14.6349;  // Latitud inicial (ej: Ciudad de Guatemala)
float current_lng = -90.5069; // Longitud inicial

// --------------------------- FUNCIONES DE CONEXIÓN --------------------------

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

// Función de reconexión MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    // Intentar conectar con usuario y contraseña
    if (client.connect(mqtt_client_id, mqtt_user, mqtt_pass)) {
      Serial.println("conectado");
    } else {
      Serial.print("falló, rc=");
      Serial.print(client.state());
      Serial.println(" Reintentando en 5 segundos");
      // Esperar 5 segundos antes de reintentar
      delay(5000);
    }
  }
}

// --------------------------- FUNCIONES DE PROCESAMIENTO ---------------------

void publish_gps_data() {
  // 1. Simular movimiento (para probar la actualización en el mapa)
  // Genera un cambio pequeño y aleatorio en las coordenadas (+/- 0.0001 grados)
  current_lat += ((float)random(0, 20) / 100000.0) - 0.0001;
  current_lng += ((float)random(0, 20) / 100000.0) - 0.0001;

  // 2. Crear el objeto JSON
  StaticJsonDocument<100> doc; // Un tamaño suficiente para el JSON
  doc["lat"] = current_lat;
  doc["lng"] = current_lng;

  // 3. Convertir el JSON a string
  char jsonBuffer[100];
  serializeJson(doc, jsonBuffer);

  // 4. Publicar en el tópico
  client.publish(mqtt_topic, jsonBuffer);
  Serial.print("Publicado: ");
  Serial.println(jsonBuffer);
}

// --------------------------- SETUP & LOOP -----------------------------------

void setup() {
  Serial.begin(115200);

  // Configurar conexión WiFi
  setup_wifi();

  // Configurar el cliente MQTT
  client.setServer(mqtt_server, mqtt_port);
  
  // HiveMQ Cloud requiere TLS, pero el ESP8266 ya maneja esto por debajo 
  // al usar el puerto 8883 con PubSubClient.
  
  Serial.println("Setup completo.");
}

void loop() {
  // Asegurar la conexión MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  // Publicar un mensaje cada 5 segundos (5000 ms)
  if (now - lastMsg > 5000) {
    lastMsg = now;
    publish_gps_data();
  }
}
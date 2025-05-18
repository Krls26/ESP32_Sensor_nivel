#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

// Pines del sensor ultrasónico
#define TRIGGER_PIN 5
#define ECHO_PIN 18

// Pin del LED
#define LED_PIN 2

// Credenciales Wi-Fi
const char* ssid = "A55 de Carlos";
const char* password = "Ca261103";

// Variables globales
float distance = 0.0;
bool ledState = false;

// Crear servidor web
AsyncWebServer server(80);

void setup() {
  // Inicialización de pines
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Iniciar comunicación serie
  Serial.begin(115200);

  // Conectar a Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a Wi-Fi...");
  }
  Serial.println("Conectado a Wi-Fi");
  Serial.println("IP Address: " + WiFi.localIP().toString());

  // Inicializar LittleFS
  if (!LittleFS.begin()) {
    Serial.println("Error al montar LittleFS");
    return;
  }
  Serial.println("LittleFS montado correctamente");

  // Configuración del servidor web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/monitor.html", "text/html");
  });

  server.on("/distance", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = "{\"distance\":" + String(distance) + "}";
    request->send(200, "application/json", json);
  });

  // Iniciar el servidor
  server.begin();
}

void loop() {
  // Generar un pulso en el pin TRIGGER
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  // Calcular la duración del pulso ECHO
  long duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;

  // Mostrar distancia en el monitor serie
  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Encender o apagar el LED según la distancia
  if (distance < 5) {
    digitalWrite(LED_PIN, HIGH);
    ledState = true;
  } else {
    digitalWrite(LED_PIN, LOW);
    ledState = false;
  }

  delay(500); // Tiempo de espera entre mediciones
}

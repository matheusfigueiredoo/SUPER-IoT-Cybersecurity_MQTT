// Código da arquitetura do projeto de cibersegurança
// Comunicação MQTT
// Projeto SUPER - UFAM

// sensor de umidade
#include <Adafruit_Sensor.h>
#include <DHT.h>
#define DHT11_PIN  4
#define DHTTYPE DHT11 // tipo de dht
DHT dht(DHT11_PIN, DHTTYPE); // instancia do sensor

#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "labiot01";        
const char* password = "acessomqtt";
const char* mqttServer = "10.42.0.1"; // broker ip
const int mqttPort = 1883;

// def dos topicos  
const char* mqtt_topic_umidade = "sensor/umidade";

WiFiClient espClient;
PubSubClient client(espClient);

// conexao wifi
void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando ao WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("Conectado!");
}

// config mqtt
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Conectado!");
      client.subscribe(mqtt_topic_umidade); // Corrigido para o tópico correto
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void setup() {
  // sensor dht
  dht.begin();

  Serial.begin(9600);

  // conexao wifi
  setupWiFi();
  
  client.setServer(mqttServer, mqttPort);
}

// Variável para controle do intervalo de 10 segundos
unsigned long lastTime = 0; // Última execução do envio de dados
const unsigned long interval = 10000; // Intervalo de 10 segundos em milissegundos

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Obtém o tempo atual
  unsigned long currentTime = millis();

  // Verifica se já passou o intervalo de 10 segundos
  if (currentTime - lastTime >= interval) {
    lastTime = currentTime; // Atualiza o tempo da última execução

    // Leitura do sensor de umidade
    float umidade = dht.readHumidity();

    if (isnan(umidade)) {
      Serial.println("Falha ao ler do sensor DHT!");
    } else {
      // Publicação no broker MQTT
      char umidadeStr[8];
      dtostrf(umidade, 6, 2, umidadeStr);
      client.publish(mqtt_topic_umidade, umidadeStr);

      // Exibição no monitor serial
      Serial.print("Umidade: ");
      Serial.print(umidade);
      Serial.println(" %");
    }
  }
}

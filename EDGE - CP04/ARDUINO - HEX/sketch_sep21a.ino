#include <ArduinoJson.h>
#include <DHT.h>
#include <LiquidCrystal.h>

#define DHTPIN 5 // Pino ao qual o sensor DHT22 está conectado
#define DHTTYPE DHT22 // Tipo do sensor DHT (DHT22 ou DHT11)

#define TRIG_PIN 2 // Pino TRIG do HC-SR04 conectado ao pino 2 do Arduino
#define ECHO_PIN 3 // Pino ECHO do HC-SR04 conectado ao pino 3 do Arduino

DHT dht(DHTPIN, DHTTYPE);

int AlertaTempBaixa = 8;
int AlertaTempAlta = 13;
int TempBaixa = 0;
int TempAlta = 35;

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

void setup() {
  pinMode(AlertaTempBaixa, OUTPUT);
  pinMode(AlertaTempAlta, OUTPUT);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  dht.begin();

  lcd.begin(16, 2); // Inicializa o LCD com 16 colunas e 2 linhas
  lcd.setCursor(0, 0);
  lcd.print("Temperatura:");
  lcd.setCursor(0, 1);
  lcd.print("Umidade:");

  // Inicializar a comunicação serial com uma taxa de 9600 bps
  Serial.begin(9600);
}

void loop() {
  float temperaturaC = dht.readTemperature();
  float umidadePercent = dht.readHumidity();

  // Medir distância com o sensor HC-SR04
  long duration;
  float distanciaCM;
  
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  duration = pulseIn(ECHO_PIN, HIGH);
  distanciaCM = duration / 58.2; // Converte o tempo em distância em centímetros

  // Criar um objeto JSON
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["temperatura"] = temperaturaC;
  jsonDoc["umidade"] = umidadePercent;
  jsonDoc["distancia"] = distanciaCM; // Adiciona a distância medida ao JSON

  // Serializar o objeto JSON em uma string
  char jsonString[256]; // Reserva espaço para a string JSON
  serializeJson(jsonDoc, jsonString);

  // Enviar a string JSON para o Node-RED
  Serial.println(jsonString);

  // Exibir leituras no LCD
  lcd.setCursor(13, 0); // Define a posição do cursor no LCD
  lcd.print("    "); // Limpa o espaço antigo
  lcd.setCursor(13, 0);
  lcd.print(temperaturaC);
  
  lcd.setCursor(8, 1); // Define a posição do cursor no LCD
  lcd.print("    "); // Limpa o espaço antigo
  lcd.setCursor(8, 1);
  lcd.print(umidadePercent);

  if (temperaturaC >= TempAlta) {
    digitalWrite(AlertaTempBaixa, LOW);
    digitalWrite(AlertaTempAlta, HIGH);
  } else if (temperaturaC <= TempBaixa) {
    digitalWrite(AlertaTempBaixa, HIGH);
    digitalWrite(AlertaTempAlta, LOW);
  } else {
    digitalWrite(AlertaTempBaixa, LOW);
    digitalWrite(AlertaTempAlta, LOW);
  }

  delay(1666); // Intervalo de 5000ms (5 segundos) antes de iniciar a próxima leitura
}

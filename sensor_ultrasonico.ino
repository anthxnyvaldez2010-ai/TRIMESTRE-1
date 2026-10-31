#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error "Bluetooth no está habilitado. Usa un ESP32 con Bluetooth clásico."
#endif

BluetoothSerial SerialBT;

// ---------- Pines ----------
const int PIN_TRIG = 5;
const int PIN_ECHO = 18;
const int PIN_LED_VERDE = 2;
const int PIN_LED_ROJO = 4;

// ---------- Configuración ----------
const char* NOMBRE_BT = "ESP32_Distancia";
const unsigned long PERIODO = 300;
float umbral = 20.0;  // Distancia en cm

unsigned long ultimoEnvio = 0;
String bufferEntrada = "";

// ---------- Medir distancia ----------
float medirDistancia() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  unsigned long duracion = pulseIn(PIN_ECHO, HIGH, 30000);

  if (duracion == 0) return -1;

  return duracion * 0.0343 / 2.0;
}

// ---------- Procesar comandos Bluetooth ----------
void procesarComando(String cmd) {
  cmd.trim();

  if (cmd.length() == 0) return;

  if (cmd.charAt(0) == 'U' || cmd.charAt(0) == 'u') {
    float nuevo = cmd.substring(1).toFloat();

    if (nuevo > 0 && nuevo <= 400) {
      umbral = nuevo;

      Serial.print("Nuevo umbral: ");
      Serial.println(umbral);
    }
  }
}

// ---------- Leer Bluetooth ----------
void leerBluetooth() {
  while (SerialBT.available()) {
    char c = SerialBT.read();

    if (c == '\n') {
      procesarComando(bufferEntrada);
      bufferEntrada = "";
    } else if (c != '\r') {
      bufferEntrada += c;
    }
  }
}

// ---------- Configuración inicial ----------
void setup() {
  Serial.begin(115200);

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_LED_VERDE, OUTPUT);
  pinMode(PIN_LED_ROJO, OUTPUT);

  digitalWrite(PIN_LED_VERDE, LOW);
  digitalWrite(PIN_LED_ROJO, LOW);

  SerialBT.begin(NOMBRE_BT);

  Serial.println("Bluetooth listo. Busca: ESP32_Distancia");
}

// ---------- Programa principal ----------
void loop() {
  leerBluetooth();

  if (millis() - ultimoEnvio >= PERIODO) {
    ultimoEnvio = millis();

    float d = medirDistancia();

    // Control de los dos LED
    if (d > 0) {
      if (d <= umbral) {
        digitalWrite(PIN_LED_ROJO, HIGH);
        digitalWrite(PIN_LED_VERDE, LOW);
      } else {
        digitalWrite(PIN_LED_ROJO, LOW);
        digitalWrite(PIN_LED_VERDE, HIGH);
      }
    } else {
      digitalWrite(PIN_LED_ROJO, LOW);
      digitalWrite(PIN_LED_VERDE, LOW);
    }

    // Enviar distancia a la aplicación
    if (SerialBT.hasClient()) {
      if (d < 0) {
        SerialBT.println("-1");
      } else {
        SerialBT.println(d, 1);
      }
    }

    // Mostrar datos en el monitor serie
    Serial.print("Distancia: ");
    Serial.print(d, 1);
    Serial.print(" cm | Umbral: ");
    Serial.println(umbral);
  }
}
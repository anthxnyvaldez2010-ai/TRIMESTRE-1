/*
  Sensor de distancia + puerta con servomotor por Bluetooth (ESP32)
  Bluetooth integrado del ESP32: nombre "Pablo"
  App:
    - Envia 'C' para abrir la puerta (servo a 90 grados)
    - Envia 'L' para cerrar la puerta (servo a 0 grados)
  El ESP32 envia la distancia en cm cada 200 ms.

  Conexiones ESP32:
    HC-SR04: TRIG -> GPIO 18, ECHO -> GPIO 19, VCC -> 5V, GND -> GND
    Servo: senal -> GPIO 23, alimentacion segun el servo, GND comun con ESP32
    LED rojo -> GPIO 25 con resistencia de 220 ohm
    LED verde -> GPIO 26 con resistencia de 220 ohm

  IMPORTANTE:
    - ECHO del HC-SR04 entrega 5V: usa divisor de voltaje para bajar a 3.3V
      antes de conectarlo al GPIO 19 del ESP32.
    - No alimentes un servo grande desde el pin 3V3 del ESP32. Usa fuente adecuada
      y une el GND de esa fuente con GND del ESP32.
*/

#include <BluetoothSerial.h>
#include <ESP32Servo.h>

BluetoothSerial BT;
Servo puerta;

const int TRIG = 23;
const int ECHO = 22;
const int PIN_SERVO = 18;
const int LED_ROJO = 21;
const int LED_VERDE = 19;

const int ANG_CERRADA = 0;
const int ANG_ABIERTA = 90;

unsigned long tAnterior = 0;

void setup() {
  Serial.begin(115200);
  BT.begin("Pablo");

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);

  puerta.setPeriodHertz(50);
  puerta.attach(PIN_SERVO, 500, 2400);
  puerta.write(ANG_CERRADA);

  digitalWrite(LED_ROJO, LOW);
  digitalWrite(LED_VERDE, HIGH);

  Serial.println(" Pablo");
}

long medirDistancia() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duracion = pulseIn(ECHO, HIGH, 30000);
  if (duracion == 0) return 400;
  return duracion / 58;
}

void loop() {
  // Enviar distancia a la aplicacion cada 200 ms
  if (millis() - tAnterior >= 200) {
    tAnterior = millis();
    long distancia = medirDistancia();
    BT.println(distancia);
    Serial.println(distancia);
  }

  // Recibir comandos de la aplicacion
  while (BT.available()) {
    char c = BT.read();

    if (c == 'C' || c == 'c') {
      digitalWrite(LED_ROJO, HIGH);
      digitalWrite(LED_VERDE, LOW);
      puerta.write(ANG_ABIERTA);
    } else if (c == 'L' || c == 'l') {
      digitalWrite(LED_ROJO, LOW);
      digitalWrite(LED_VERDE, HIGH);
      puerta.write(ANG_CERRADA);
    }
  }
}
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

const int LED = 2;
char dato;

void setup() {
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  SerialBT.begin("ESP32_de_Anthony");

  Serial.println("Bluetooth listo");
  Serial.println("Busca ESP32_LED en tu celular");
}

void loop() {
  if (SerialBT.available()) {
    dato = SerialBT.read();

    if (dato == '1') {
      digitalWrite(LED, HIGH);
      SerialBT.println("LED ENCENDIDO");
      Serial.println("LED ENCENDIDO");
    }
    else if (dato == '0') {
      digitalWrite(LED, LOW);
      SerialBT.println("LED APAGADO");
      Serial.println("LED APAGADO");
    }
  }
}
#include <Servo.h>

// Ferramenta simples para descobrir o centro físico do SG90/MG90S.
// Ligue o sinal do servo no pino D3.

constexpr uint8_t SERVO_PIN = 3;

Servo servo;

void setup() {
  Serial.begin(9600);
  servo.attach(SERVO_PIN);

  Serial.println(F("Teste de centro do servo"));
  Serial.println(F("Digite um angulo entre 0 e 180 e pressione Enter."));
  Serial.println(F("Comece em 90. Ex.: 85, 90, 95, 100."));
  Serial.println();

  servo.write(90);
  delay(700);

  Serial.println(F("Servo posicionado inicialmente em 90 graus."));
}

void loop() {
  if (Serial.available() > 0) {
    const int angulo = Serial.parseInt();

    // Limpa caracteres restantes da linha.
    while (Serial.available() > 0) {
      Serial.read();
    }

    if (angulo >= 0 && angulo <= 180) {
      servo.write(angulo);

      Serial.print(F("Servo -> "));
      Serial.print(angulo);
      Serial.println(F(" graus"));

      delay(500);
    } else {
      Serial.println(F("Valor invalido. Use um numero entre 0 e 180."));
    }
  }
}

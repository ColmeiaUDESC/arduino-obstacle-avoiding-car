#include <Servo.h>

// ============================================================
// Carrinho Autônomo Desviador de Obstáculos
// Arduino Uno + L298N + HC-SR04 + SG90/MG90S
// ============================================================

// -------------------- HC-SR04 --------------------
constexpr uint8_t TRIG_PIN = 12;
constexpr uint8_t ECHO_PIN = 11;

// -------------------- Servo -----------------------
constexpr uint8_t SERVO_PIN = 3;
Servo sensorServo;

// Ajuste este valor depois de executar tools/servo_center.
// Se o sensor ficar apontando para a esquerda em 90°, aumente/diminua
// até ele ficar fisicamente alinhado com a frente do carrinho.
constexpr int CENTRO_SERVO = 90;

// Duas posições de leitura de cada lado.
// Todos os ângulos são calculados a partir do centro calibrado.
constexpr int DESVIO_LATERAL_1 = 35;
constexpr int DESVIO_LATERAL_2 = 65;

// Limites físicos de segurança do servo.
constexpr int SERVO_MIN = 15;
constexpr int SERVO_MAX = 165;

// -------------------- L298N -----------------------
// Motor direito: OUT1 / OUT2
// Motor esquerdo: OUT3 / OUT4
constexpr uint8_t IN1 = 8;
constexpr uint8_t IN2 = 7;
constexpr uint8_t IN3 = 6;
constexpr uint8_t IN4 = 5;

// -------------------- Navegação -------------------
constexpr int DISTANCIA_OBSTACULO_CM = 45;
constexpr int DISTANCIA_CRITICA_CM = 18;

constexpr unsigned long TEMPO_RECUO_MS = 350;
constexpr unsigned long TEMPO_GIRO_MS = 430;
constexpr unsigned long TEMPO_GIRO_FORTE_MS = 650;
constexpr unsigned long TEMPO_ESTABILIZAR_SERVO_MS = 280;

// Evita considerar ecos absurdamente longos.
// 30 ms corresponde a aproximadamente 5 m de ida e volta,
// acima da faixa útil necessária para este carrinho.
constexpr unsigned long ULTRASSOM_TIMEOUT_US = 30000UL;

enum Direcao {
  ESQUERDA,
  DIREITA
};

// ------------------------------------------------------------
// Utilidades
// ------------------------------------------------------------

int limitarAngulo(int angulo) {
  return constrain(angulo, SERVO_MIN, SERVO_MAX);
}

long medirDistanciaUmaVez() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(3);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  const unsigned long duracao =
      pulseIn(ECHO_PIN, HIGH, ULTRASSOM_TIMEOUT_US);

  // Sem eco: trate como caminho livre dentro da faixa de interesse.
  if (duracao == 0) {
    return 400;
  }

  // Velocidade aproximada do som: 0,0343 cm/us.
  // Divide por 2 porque o pulso percorre ida e volta.
  long distancia = static_cast<long>((duracao * 0.0343) / 2.0);

  if (distancia < 2) {
    distancia = 2;
  }

  if (distancia > 400) {
    distancia = 400;
  }

  return distancia;
}

// Mediana de 3 leituras para reduzir falsos ecos do HC-SR04.
long medirDistancia() {
  long a = medirDistanciaUmaVez();
  delay(12);
  long b = medirDistanciaUmaVez();
  delay(12);
  long c = medirDistanciaUmaVez();

  if (a > b) {
    long t = a;
    a = b;
    b = t;
  }

  if (b > c) {
    long t = b;
    b = c;
    c = t;
  }

  if (a > b) {
    long t = a;
    a = b;
    b = t;
  }

  return b;
}

long olharParaAngulo(int angulo) {
  sensorServo.write(limitarAngulo(angulo));
  delay(TEMPO_ESTABILIZAR_SERVO_MS);

  const long distancia = medirDistancia();

  Serial.print(F("Angulo "));
  Serial.print(limitarAngulo(angulo));
  Serial.print(F(" graus: "));
  Serial.print(distancia);
  Serial.println(F(" cm"));

  return distancia;
}

// ------------------------------------------------------------
// Controle dos motores
// ------------------------------------------------------------

void parar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void frente() {
  // Direito: OUT1 -> OUT2
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Esquerdo: OUT3 -> OUT4
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void tras() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// Giro no próprio eixo.
void girarEsquerda() {
  // Motor direito para frente.
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Motor esquerdo para trás.
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void girarDireita() {
  // Motor direito para trás.
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  // Motor esquerdo para frente.
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// ------------------------------------------------------------
// Varredura do ambiente
// ------------------------------------------------------------

long medirLado(Direcao lado) {
  int angulo1;
  int angulo2;

  if (lado == ESQUERDA) {
    angulo1 = CENTRO_SERVO + DESVIO_LATERAL_1;
    angulo2 = CENTRO_SERVO + DESVIO_LATERAL_2;
    Serial.println(F("Olhando ESQUERDA"));
  } else {
    angulo1 = CENTRO_SERVO - DESVIO_LATERAL_1;
    angulo2 = CENTRO_SERVO - DESVIO_LATERAL_2;
    Serial.println(F("Olhando DIREITA"));
  }

  const long d1 = olharParaAngulo(angulo1);
  const long d2 = olharParaAngulo(angulo2);

  // Usa a média das duas direções daquele lado.
  // Isso reduz a chance de escolher um lado por causa de um único eco.
  return (d1 + d2) / 2;
}

void centralizarSensor() {
  sensorServo.write(limitarAngulo(CENTRO_SERVO));
  delay(TEMPO_ESTABILIZAR_SERVO_MS);
}

// ------------------------------------------------------------
// Decisão de desvio
// ------------------------------------------------------------

void executarDesvio() {
  parar();
  delay(100);

  // Primeiro cria espaço antes de olhar para os lados.
  tras();
  delay(TEMPO_RECUO_MS);
  parar();
  delay(100);

  const long distanciaEsquerda = medirLado(ESQUERDA);
  const long distanciaDireita = medirLado(DIREITA);

  centralizarSensor();

  Serial.print(F("Esquerda: "));
  Serial.print(distanciaEsquerda);
  Serial.print(F(" cm | Direita: "));
  Serial.print(distanciaDireita);
  Serial.println(F(" cm"));

  // Se os dois lados estiverem muito fechados, faz um giro maior.
  if (distanciaEsquerda < DISTANCIA_CRITICA_CM &&
      distanciaDireita < DISTANCIA_CRITICA_CM) {

    Serial.println(F("Ambos os lados bloqueados. Recuando e girando forte."));

    tras();
    delay(TEMPO_RECUO_MS + 200);
    parar();
    delay(80);

    if (distanciaEsquerda >= distanciaDireita) {
      Serial.println(F("Giro forte para ESQUERDA"));
      girarEsquerda();
    } else {
      Serial.println(F("Giro forte para DIREITA"));
      girarDireita();
    }

    delay(TEMPO_GIRO_FORTE_MS);
    parar();
    delay(120);
    return;
  }

  if (distanciaEsquerda > distanciaDireita) {
    Serial.println(F("Escolheu ESQUERDA"));
    girarEsquerda();
  } else {
    Serial.println(F("Escolheu DIREITA"));
    girarDireita();
  }

  delay(TEMPO_GIRO_MS);
  parar();
  delay(120);
}

// ------------------------------------------------------------
// Setup / Loop
// ------------------------------------------------------------

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  parar();

  sensorServo.attach(SERVO_PIN);
  centralizarSensor();

  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(F(" Carrinho desviador de obstaculos"));
  Serial.println(F("========================================"));
  Serial.print(F("Centro do servo: "));
  Serial.println(CENTRO_SERVO);
  Serial.print(F("Distancia de deteccao: "));
  Serial.print(DISTANCIA_OBSTACULO_CM);
  Serial.println(F(" cm"));
  Serial.println(F("Robo iniciado."));
}

void loop() {
  centralizarSensor();

  const long distanciaFrente = medirDistancia();

  Serial.print(F("Frente: "));
  Serial.print(distanciaFrente);
  Serial.println(F(" cm"));

  if (distanciaFrente <= DISTANCIA_OBSTACULO_CM) {
    Serial.println(F("Obstaculo detectado."));
    executarDesvio();
  } else {
    frente();
  }

  delay(40);
}

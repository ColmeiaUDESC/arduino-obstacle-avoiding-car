<p align="center">
  <a href="https://github.com/ColmeiaUDESC">
    <img src="https://avatars.githubusercontent.com/u/54866625?s=400&u=184d63b6c7ecc161f9ebbad8f6e7b32b2e600253&v=4" alt="Colmeia UDESC" width="160" height="160">
  </a>
  <h1 align="center">Arduino Obstacle Avoiding Car</h1>
</p>

## :dart: Conceito

> Carrinho robótico autônomo desenvolvido com Arduino capaz de navegar por um ambiente, detectar obstáculos utilizando um sensor ultrassônico e decidir automaticamente para qual direção seguir.

O projeto utiliza um sensor HC-SR04 montado sobre um servo motor, permitindo que o robô analise o ambiente à esquerda e à direita antes de realizar uma manobra.

## 💡 Objetivos

> Desenvolver um projeto simples de robótica utilizando hardware acessível e tecnologias abertas, demonstrando conceitos fundamentais de eletrônica, programação embarcada, sensores e controle de motores.

- Introduzir conceitos de robótica móvel.
- Demonstrar o funcionamento de sensores ultrassônicos.
- Trabalhar com motores DC e pontes H.
- Utilizar servo motores para movimentação de sensores.
- Desenvolver lógica de tomada de decisão autônoma.
- Criar um projeto didático que possa ser reproduzido por estudantes iniciantes.
- Incentivar projetos utilizando hardware e software livre.

## 🤖 Funcionamento

O carrinho se movimenta continuamente enquanto não existem obstáculos próximos.

O funcionamento básico ocorre da seguinte maneira:

1. O sensor HC-SR04 mede continuamente a distância à frente.
2. Caso a distância seja maior que aproximadamente **45 cm**, o carrinho continua andando.
3. Quando um obstáculo é detectado:
   - o carrinho para;
   - recua por alguns instantes;
   - movimenta o sensor para a esquerda;
   - realiza duas medições;
   - movimenta o sensor para a direita;
   - realiza outras duas medições;
   - compara as distâncias;
   - escolhe o lado que possui mais espaço;
   - gira para essa direção;
   - centraliza novamente o sensor;
   - continua andando.

Caso os dois lados estejam bloqueados, o carrinho executa um recuo maior seguido de uma rotação mais intensa.

Para reduzir erros nas leituras do HC-SR04, o código realiza múltiplas medições e utiliza uma filtragem simples dos valores obtidos.

## 📸 Imagens do robô

Abaixo estão algumas imagens do protótipo montado.

<p align="center">
  <img src="images/robo-superior.jpg" alt="Vista superior do robô" width="45%">
  <img src="images/robo-eletronica.jpg" alt="Detalhe da eletrônica do robô" width="45%">
</p>

```markdown
![Demonstração do robô](images/demo.gif)
```

## 🔩 Componentes utilizados

| Quantidade | Componente |
|---:|---|
| 1 | Arduino Uno |
| 1 | Driver de motores L298N |
| 1 | Sensor ultrassônico HC-SR04 |
| 1 | Servo motor SG90 ou MG90S |
| 2 | Motores DC com redução |
| 2 | Rodas |
| 1 | Roda boba / caster 360° |
| 1 | Chassi para carrinho |
| 1 | Suporte para HC-SR04 e servo |
| Vários | Jumpers |
| 1 | Protoboard pequena, opcional |
| 1 | Fonte ou bateria para alimentação |

Algumas peças estruturais, como suporte do sensor, suporte do servo e partes do chassi, podem ser produzidas utilizando impressão 3D.

## 🔌 Ligações

### HC-SR04

| HC-SR04 | Arduino Uno |
|---|---|
| VCC | 5V |
| GND | GND |
| TRIG | D12 |
| ECHO | D11 |

### Servo motor

| Servo | Arduino Uno |
|---|---|
| Sinal | D3 |
| VCC | 5V |
| GND | GND |

O fio de sinal normalmente possui coloração amarela ou laranja.

O fio positivo normalmente é vermelho.

O fio de GND normalmente é marrom ou preto.

### Driver L298N

| L298N | Arduino Uno |
|---|---|
| IN1 | D8 |
| IN2 | D7 |
| IN3 | D6 |
| IN4 | D5 |

Os motores são conectados diretamente às saídas do driver:

```text
OUT1 / OUT2 -> Motor direito
OUT3 / OUT4 -> Motor esquerdo
```

Caso o módulo possua jumpers `ENA` e `ENB`, eles podem permanecer conectados para que os motores trabalhem na velocidade máxima.

## ⚡ Alimentação

Os motores não devem ser alimentados diretamente através dos pinos do Arduino.

O L298N deve receber alimentação adequada para os motores utilizados.

Também não é recomendado utilizar baterias retangulares tradicionais de **9 V PP3**, pois elas normalmente não conseguem fornecer corrente suficiente para motores DC.

Isso pode causar:

- perda de força nos motores;
- reinicialização do Arduino;
- movimento irregular do servo;
- leituras incorretas do HC-SR04;
- comportamento instável do carrinho.

Para projetos desse tipo é preferível utilizar um conjunto de pilhas recarregáveis ou uma bateria adequada para a corrente exigida pelos motores.

Caso seja utilizada uma fonte externa para o servo ou para o driver, todos os dispositivos devem compartilhar o mesmo GND:

```text
GND Arduino
    |
    +---- GND L298N
    |
    +---- GND Servo
    |
    +---- GND Fonte
```

## 🛠️ Montagem

A montagem pode ser realizada seguindo esta ordem:

1. Fixar os dois motores DC nas laterais do chassi.
2. Instalar as rodas nos motores.
3. Instalar a roda boba no lado oposto.
4. Fixar o Arduino Uno sobre o chassi.
5. Fixar o L298N.
6. Instalar o servo motor na parte frontal.
7. Fixar o HC-SR04 sobre o servo.
8. Conectar os motores ao L298N.
9. Conectar o L298N ao Arduino.
10. Conectar o HC-SR04.
11. Conectar o servo.
12. Conectar a alimentação.
13. Verificar se todos os GNDs estão interligados.

O HC-SR04 deve ficar apontado aproximadamente para frente quando o servo estiver em sua posição central.

## 👁️ Calibração do servo

Servos SG90 não possuem necessariamente o mesmo centro físico.

Por isso, o repositório inclui um programa específico para descobrir qual ângulo deixa o sensor apontado exatamente para frente:

```text
tools/servo_center/servo_center.ino
```

Após enviar o código para o Arduino, abra o Serial Monitor em:

```text
9600 baud
```

Digite valores como:

```text
80
85
90
95
100
```

até encontrar o ângulo que deixa o sensor perfeitamente alinhado com a frente do carrinho.

Depois altere no código principal:

```cpp
constexpr int CENTRO_SERVO = 90;
```

Por exemplo:

```cpp
constexpr int CENTRO_SERVO = 96;
```

Os ângulos utilizados para olhar para a esquerda e para a direita serão calculados automaticamente a partir desse valor.

## 🧠 Lógica de navegação

O código possui três comportamentos principais.

### Caminho livre

```text
Distância > 45 cm
        |
        v
Continuar andando
```

### Obstáculo detectado

```text
Obstáculo
   |
   v
Parar
   |
   v
Recuar
   |
   v
Olhar esquerda
   |
   v
Olhar direita
   |
   v
Comparar distâncias
   |
   +--------+
   |        |
Esquerda  Direita
maior     maior
   |        |
   v        v
Girar E   Girar D
```

### Ambos os lados bloqueados

Caso esquerda e direita estejam muito próximas de um obstáculo:

```text
Recuar novamente
        |
        v
Executar giro maior
        |
        v
Realizar nova leitura
```

## ⚙️ Configurações principais

A distância utilizada para detectar um obstáculo pode ser alterada em:

```cpp
constexpr int DISTANCIA_OBSTACULO_CM = 45;
```

Caso o carrinho esteja chegando muito perto da parede antes de reagir, aumente esse valor.

### Tempo de recuo

```cpp
constexpr unsigned long TEMPO_RECUO_MS = 350;
```

### Tempo de giro

```cpp
constexpr unsigned long TEMPO_GIRO_MS = 430;
```

### Giro maior

```cpp
constexpr unsigned long TEMPO_GIRO_FORTE_MS = 650;
```

Esses valores dependem da velocidade dos motores, tensão da bateria, peso do carrinho e tipo de roda.

Por isso, podem precisar de calibração após a montagem.

## 📡 Monitor Serial

O código envia informações pelo Serial Monitor.

Exemplo:

```text
Frente: 81 cm
Frente: 67 cm
Frente: 43 cm

Obstaculo detectado.

Olhando ESQUERDA
Angulo 125 graus: 87 cm
Angulo 155 graus: 73 cm

Olhando DIREITA
Angulo 55 graus: 25 cm
Angulo 25 graus: 19 cm

Esquerda: 80 cm | Direita: 22 cm

Escolheu ESQUERDA
```

Isso permite verificar:

- distância frontal;
- leituras laterais;
- funcionamento do servo;
- direção escolhida pelo algoritmo;
- possíveis erros de montagem.

## 🔧 Problemas comuns

### O carrinho anda para trás

Os motores provavelmente estão conectados com polaridade invertida.

Inverta:

```text
OUT1 <-> OUT2
```

ou:

```text
OUT3 <-> OUT4
```

dependendo do motor.

### Um motor gira no sentido errado

Inverta os dois fios daquele motor no L298N.

### O carrinho escolhe esquerda mas gira para direita

Verifique a orientação dos motores e do servo.

Dependendo da forma como o servo foi montado fisicamente, aumentar o ângulo pode fazer o sensor olhar para o lado contrário.

### Servo olhando mais para um lado

Execute:

```text
tools/servo_center/servo_center.ino
```

e ajuste:

```cpp
CENTRO_SERVO
```

### Arduino reiniciando quando o servo se movimenta

Provavelmente existe queda de tensão.

Utilize uma fonte de 5 V adequada para o servo e mantenha o GND compartilhado com o Arduino.

## 📂 Como este repositório está organizado

```text
arduino-obstacle-avoiding-car/
│
├── README.md
│
├── images/
│   ├── robo-frente.jpg
│   ├── robo-lateral.jpg
│   ├── robo-superior.jpg
│   └── robo-eletronica.jpg
│
├── src/
│   └── obstacle_avoiding_car/
│       └── obstacle_avoiding_car.ino
│
└── tools/
    └── servo_center/
        └── servo_center.ino
```

### `images`

Contém fotos, diagramas e GIFs demonstrando a montagem e funcionamento do robô.

### `src/obstacle_avoiding_car`

Contém o código principal responsável pela navegação autônoma do carrinho.

### `tools/servo_center`

Contém uma ferramenta auxiliar utilizada para descobrir o centro físico do servo.

## 🐝 Colmeia

### Grupo de extensão em software e hardware livre

> Com o objetivo da disseminação de conhecimento em software e hardware livres, o Colmeia ministra aulas e minicursos de diversos temas, visita escolas e outras universidades e desenvolve projetos voltados à democratização do acesso à tecnologia.

O projeto também serve como exemplo prático para introdução à eletrônica, Arduino e robótica.

<sub>
<strong>Siga o Colmeia nas redes sociais para acompanhar mais conteúdos:</strong>
<br>

[<img src="https://img.shields.io/badge/GitHub-100000?style=for-the-badge&logo=github&logoColor=white">](https://github.com/ColmeiaUDESC)
[![TikTok](https://img.shields.io/badge/TikTok-%23000000.svg?logo=TikTok&style=for-the-badge&logoColor=white)](https://www.tiktok.com/@colmeiaudesc)
[<img src="https://img.shields.io/badge/Facebook-1877F2?style=for-the-badge&logo=facebook&logoColor=white">](https://www.facebook.com/colmeiaudesc/)
[<img src="https://img.shields.io/badge/instagram-%23E4405F.svg?&style=for-the-badge&logo=instagram&logoColor=white">](https://www.instagram.com/colmeiaudesc/)
[<img src="https://img.shields.io/badge/linkedin-%230077B5.svg?&style=for-the-badge&logo=linkedin&logoColor=white">](https://www.linkedin.com/company/colmeiaudesc)
[![Discord Badge](https://img.shields.io/badge/Discord-5865F2?style=for-the-badge&logo=discord&logoColor=white)](https://discord.gg/yZZsV4xABZ)
[![Youtube Badge](https://img.shields.io/badge/YouTube-FF0000?style=for-the-badge&logo=youtube&logoColor=white)](https://www.youtube.com/channel/UC51KrWL94AfGxI_4l_E7uzA)

</sub>

## 📜 Licença

Projeto desenvolvido com finalidade educacional e de experimentação com hardware livre.

Caso o repositório seja publicado como projeto open source, pode ser utilizada uma licença como a **MIT License**.

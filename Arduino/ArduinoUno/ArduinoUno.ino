// ---------------------- DEFINICIÓN DE PINES LED ----------------------
const int LED_DERECHA = 2; 
const int LED_IZQUIERDA = 3; 
const int LED_STOP_FRENO = 4; 
const int LED_AVANCE = 5; 

// Variable para almacenar el comando recibido del Wemos
String comandoRecibido = "";
// Tiempos para el parpadeo de intermitentes
unsigned long lastBlinkTime = 0;
const int BLINK_INTERVAL = 250; // Intervalo de 250 ms

// ---------------------- SETUP ----------------------

void setup() {
  // Inicializar la comunicación serial (DEBE coincidir con el Wemos)
  Serial.begin(9600); 

  pinMode(LED_DERECHA, OUTPUT);
  pinMode(LED_IZQUIERDA, OUTPUT);
  pinMode(LED_STOP_FRENO, OUTPUT);
  pinMode(LED_AVANCE, OUTPUT);

  apagarTodosLosLeds();
}

// ---------------------- FUNCIONES AUXILIARES ----------------------

void apagarTodosLosLeds() {
  digitalWrite(LED_DERECHA, LOW);
  digitalWrite(LED_IZQUIERDA, LOW);
  digitalWrite(LED_STOP_FRENO, LOW);
  digitalWrite(LED_AVANCE, LOW);
}

void leerComandoSerial() {
  while (Serial.available()) {
    char incomingByte = Serial.read();

    if (incomingByte == '\n') { // Si detecta el salto de línea (delimitador)
      comandoRecibido.trim();
      comandoRecibido.toUpperCase();
      procesarComando();
      comandoRecibido = ""; 
    } else {
      comandoRecibido += incomingByte;
    }
  }
}

void procesarComando() {
  // Manejar comandos sin parpadeo (DERECHA, IZQUIERDA, STOP, AVANCE)
  
  if (comandoRecibido.equals("STOP")) {
    apagarTodosLosLeds();
    digitalWrite(LED_STOP_FRENO, HIGH);
  } else if (comandoRecibido.equals("AVANCE")) {
    apagarTodosLosLeds();
    digitalWrite(LED_AVANCE, HIGH);
  } 
  // Los comandos DERECHA, IZQUIERDA e INTERMITENTE se manejan en el loop
}

// ---------------------- LOOP PRINCIPAL ----------------------

void loop() {
  // 1. Leer el comando que llega del Wemos
  leerComandoSerial();
  
  // 2. Manejar Lógica de Parpadeo (Fuera de procesarComando para no bloquear el Serial)
  
  unsigned long currentMillis = millis();

  if (comandoRecibido.equals("DERECHA")) {
    if (currentMillis - lastBlinkTime >= BLINK_INTERVAL) {
      lastBlinkTime = currentMillis;
      // Apagar todos excepto el de avance (para no apagar el fondo)
      digitalWrite(LED_IZQUIERDA, LOW);
      digitalWrite(LED_STOP_FRENO, LOW);
      
      // Alternar el LED DERECHA
      if (digitalRead(LED_DERECHA) == LOW) {
        digitalWrite(LED_DERECHA, HIGH);
      } else {
        digitalWrite(LED_DERECHA, LOW);
      }
    }
  } 
  else if (comandoRecibido.equals("IZQUIERDA")) {
    if (currentMillis - lastBlinkTime >= BLINK_INTERVAL) {
      lastBlinkTime = currentMillis;
      // Apagar todos excepto el de avance
      digitalWrite(LED_DERECHA, LOW);
      digitalWrite(LED_STOP_FRENO, LOW);
      
      // Alternar el LED IZQUIERDA
      if (digitalRead(LED_IZQUIERDA) == LOW) {
        digitalWrite(LED_IZQUIERDA, HIGH);
      } else {
        digitalWrite(LED_IZQUIERDA, LOW);
      }
    }
  } 
  else if (comandoRecibido.equals("INTERMITENTE")) {
    if (currentMillis - lastBlinkTime >= BLINK_INTERVAL) {
      lastBlinkTime = currentMillis;
      digitalWrite(LED_STOP_FRENO, LOW);

      // Alternar AMBOS LEDs
      if (digitalRead(LED_DERECHA) == LOW) {
        digitalWrite(LED_DERECHA, HIGH);
        digitalWrite(LED_IZQUIERDA, HIGH);
      } else {
        digitalWrite(LED_DERECHA, LOW);
        digitalWrite(LED_IZQUIERDA, LOW);
      }
    }
  }
}
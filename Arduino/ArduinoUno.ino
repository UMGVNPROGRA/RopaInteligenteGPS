// ---------------------- DEFINICIÓN DE PINES LED ----------------------
// Conexión de LEDs (simulando las luces de la ropa)
const int LED_DERECHA = 2; // Ejemplo: LED para señal de giro derecha
const int LED_IZQUIERDA = 3; // Ejemplo: LED para señal de giro izquierda
const int LED_STOP_FRENO = 4; // Ejemplo: LED de freno o STOP
const int LED_AVANCE = 5; // Ejemplo: LED que indica movimiento/avance

// Variable para almacenar el comando recibido del Wemos
String comandoRecibido = "";

// ---------------------- SETUP ----------------------

void setup() {
  // Inicializar la comunicación serial (debe coincidir con la velocidad del Wemos)
  Serial.begin(9600); 

  // Configurar los pines de los LEDs como salidas
  pinMode(LED_DERECHA, OUTPUT);
  pinMode(LED_IZQUIERDA, OUTPUT);
  pinMode(LED_STOP_FRENO, OUTPUT);
  pinMode(LED_AVANCE, OUTPUT);

  // Inicializar todos los LEDs apagados
  apagarTodosLosLeds();
}

// ---------------------- FUNCIONES AUXILIARES ----------------------

// Función para apagar todos los LEDs de forma rápida
void apagarTodosLosLeds() {
  digitalWrite(LED_DERECHA, LOW);
  digitalWrite(LED_IZQUIERDA, LOW);
  digitalWrite(LED_STOP_FRENO, LOW);
  digitalWrite(LED_AVANCE, LOW);
}

// Función que lee el comando completo del buffer serial
void leerComandoSerial() {
  while (Serial.available()) {
    // Lee el siguiente byte
    char incomingByte = Serial.read();

    // Si el byte es un salto de línea ('\n'), el comando ha terminado
    if (incomingByte == '\n') {
      // El comando está completo, limpiar espacios y pasarlo a mayúsculas
      comandoRecibido.trim();
      comandoRecibido.toUpperCase();
      // Procesar el comando
      procesarComando();
      // Reiniciar la variable de comando para el siguiente ciclo
      comandoRecibido = ""; 
    } else {
      // Si no es el final de línea, sigue añadiendo el carácter al comando
      comandoRecibido += incomingByte;
    }
  }
}

// Función que ejecuta las acciones basadas en el comando
void procesarComando() {
  // Serial.print("Comando recibido: ");
  // Serial.println(comandoRecibido); // Útil para depuración

  apagarTodosLosLeds(); // Apagar todos antes de encender el nuevo

  if (comandoRecibido.equals("DERECHA")) {
    digitalWrite(LED_DERECHA, HIGH);
    // Podrías añadir un parpadeo aquí si lo deseas
  } else if (comandoRecibido.equals("IZQUIERDA")) {
    digitalWrite(LED_IZQUIERDA, HIGH);
    // Podrías añadir un parpadeo aquí si lo deseas
  } else if (comandoRecibido.equals("STOP")) {
    digitalWrite(LED_STOP_FRENO, HIGH);
  } else if (comandoRecibido.equals("INTERMITENTE")) {
    // Para el intermitente, haremos un parpadeo rápido
    digitalWrite(LED_DERECHA, HIGH);
    digitalWrite(LED_IZQUIERDA, HIGH);
    delay(100); 
    digitalWrite(LED_DERECHA, LOW);
    digitalWrite(LED_IZQUIERDA, LOW);
  } else if (comandoRecibido.equals("AVANCE")) {
    // Si no hay ninguna señal específica, encender la luz de avance
    digitalWrite(LED_AVANCE, HIGH);
  }
}

// ---------------------- LOOP PRINCIPAL ----------------------

void loop() {
  // Llamar continuamente a la función para leer los datos que llegan del Wemos
  leerComandoSerial();
}
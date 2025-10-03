/*
  ==================================================================================
  Código de Control para Robot Seguidor de Línea - MODO DEPURACIÓN AVANZADO
  Plataforma: Arduino Mega
  Versión: 3.2 (Búsqueda Inteligente y Parada de Emergencia)

  Descripción:
  Esta versión introduce un contador para manejar la pérdida de la línea.
  Si el robot no detecta la línea, intentará avanzar lentamente para reencontrarla.
  Después de 3 intentos fallidos consecutivos, el sistema se detendrá por completo
  para evitar que el robot se pierda.
  ==================================================================================
*/

// ===== SECCIÓN 1: PARÁMETROS DE AJUSTE PARA DEPURACIÓN =====

// --- Velocidades de Maniobra ---
const int VELOCIDAD_BASE_DEBUG = 90;
const int VELOCIDAD_GIRO_DEBUG = 94;
const int VELOCIDAD_BUSQUEDA = 90;  // Velocidad mínima para buscar la línea
const int VELOCIDAD_GIRO_90 = 90;
const int VELOCIDAD_GIRO_180 = 100;

// --- Tiempos de Maniobra (en milisegundos) ---
const int TIEMPO_MANIOBRA = 150;
const int PAUSA_ENTRE_ACCIONES = 300;
const int TIEMPO_BUSQUEDA = 150;  // Un pulso corto hacia adelante
const int TIEMPO_GIRO_180 = 530;

// ===== SECCIÓN 2: DEFINICIONES DE HARDWARE Y VARIABLES GLOBALES =====

// --- Pines de Sensores y Motores ---
#define IR_EXTREMO_DERECHO A0
#define IR_DERECHO A1
#define IR_CENTRO A2
#define IR_IZQUIERDO A3
#define IR_EXTREMO_IZQUIERDO A4
#define IR_ADELANTE A5
#define MOTOR_IZQ_IN1 24
#define MOTOR_IZQ_IN2 25
#define MOTOR_IZQ_ENA 2
#define MOTOR_DER_IN3 26
#define MOTOR_DER_IN4 27
#define MOTOR_DER_ENB 3
const int STATUS_LED_PIN = 13;

// --- Variables Globales ---
int lineaPerdidaContador = 0;  // Contador para intentos de búsqueda
#define MAXlineaPerdidaContador 7

int contadorDeadEnd = 0;
#define DEAD_END_UMBRAL 3

int contadorFinal = 0;
#define FINAL_UMBRAL 1
// ===== SECCIÓN 3: FUNCIONES DE ACCIÓN DISCRETA =====

void accion_Detener() {
  // ... (código sin cambios)
  digitalWrite(MOTOR_IZQ_IN1, LOW);
  digitalWrite(MOTOR_IZQ_IN2, LOW);
  analogWrite(MOTOR_IZQ_ENA, 0);
  digitalWrite(MOTOR_DER_IN3, LOW);
  digitalWrite(MOTOR_DER_IN4, LOW);
  analogWrite(MOTOR_DER_ENB, 0);
}

void accion_AvanzarRecto() {
  // ... (código sin cambios)
  Serial.println("-> Accion: Avanzar Recto");
  digitalWrite(MOTOR_IZQ_IN1, HIGH);
  digitalWrite(MOTOR_IZQ_IN2, LOW);
  digitalWrite(MOTOR_DER_IN3, HIGH);
  digitalWrite(MOTOR_DER_IN4, LOW);
  analogWrite(MOTOR_IZQ_ENA, VELOCIDAD_BASE_DEBUG);
  analogWrite(MOTOR_DER_ENB, VELOCIDAD_BASE_DEBUG);
  delay(TIEMPO_MANIOBRA);
}

void accion_GiroSuaveDerecha() {
  // ... (código sin cambios)
  Serial.println("-> Accion: Giro Suave Derecha");
  digitalWrite(MOTOR_IZQ_IN1, HIGH);
  digitalWrite(MOTOR_IZQ_IN2, LOW);
  digitalWrite(MOTOR_DER_IN3, HIGH);
  digitalWrite(MOTOR_DER_IN4, LOW);
  analogWrite(MOTOR_IZQ_ENA, VELOCIDAD_GIRO_DEBUG);
  analogWrite(MOTOR_DER_ENB, VELOCIDAD_BASE_DEBUG / 2);
  delay(TIEMPO_MANIOBRA);
}

void accion_GiroSuaveIzquierda() {
  // ... (código sin cambios)
  Serial.println("-> Accion: Giro Suave Izquierda");
  digitalWrite(MOTOR_IZQ_IN1, HIGH);
  digitalWrite(MOTOR_IZQ_IN2, LOW);
  digitalWrite(MOTOR_DER_IN3, HIGH);
  digitalWrite(MOTOR_DER_IN4, LOW);
  analogWrite(MOTOR_IZQ_ENA, VELOCIDAD_BASE_DEBUG / 2);
  analogWrite(MOTOR_DER_ENB, VELOCIDAD_GIRO_DEBUG);
  delay(TIEMPO_MANIOBRA);
}

void accion_GiroFuerteDerecha() {
  // ... (código sin cambios)
  Serial.println("-> Accion: GIRO FUERTE DERECHA");
  digitalWrite(MOTOR_IZQ_IN1, HIGH);
  digitalWrite(MOTOR_IZQ_IN2, LOW);
  digitalWrite(MOTOR_DER_IN3, LOW);
  digitalWrite(MOTOR_DER_IN4, HIGH);
  analogWrite(MOTOR_IZQ_ENA, VELOCIDAD_GIRO_DEBUG);
  analogWrite(MOTOR_DER_ENB, VELOCIDAD_GIRO_DEBUG);
  delay(TIEMPO_MANIOBRA);
}

void accion_GiroFuerteIzquierda() {
  // ... (código sin cambios)
  Serial.println("-> Accion: GIRO FUERTE IZQUIERDA");
  digitalWrite(MOTOR_IZQ_IN1, LOW);
  digitalWrite(MOTOR_IZQ_IN2, HIGH);
  digitalWrite(MOTOR_DER_IN3, HIGH);
  digitalWrite(MOTOR_DER_IN4, LOW);
  analogWrite(MOTOR_IZQ_ENA, VELOCIDAD_GIRO_DEBUG);
  analogWrite(MOTOR_DER_ENB, VELOCIDAD_GIRO_DEBUG);
  delay(TIEMPO_MANIOBRA);
}

bool deteccion_bifurcacion_Y(int s1, int s2, int s3, int s4, int s5, int s6) {
  if ((s1 == 0 || s2 == 0) && (s4 == 0 || s5 == 0) && s6 == 1) {
    Serial.println("-> Bifurcacion Y encontrada");
    return true;
  }
  return false;
}

bool deteccion_dead_end(int s1, int s2, int s3, int s4, int s5) {
  // Ningun sensor detecta la linea, chequear que no haya falso positivos
  if (s1 == 1 && s2 == 1 && s3 == 1 && s4 == 1 && s5 == 1) {
    contadorDeadEnd++;
    // accion_BuscarAdelante();
  } else {
    contadorDeadEnd = 0;
  }

  if (contadorDeadEnd >= DEAD_END_UMBRAL) {
    contadorDeadEnd = 0;  // opcional, para evitar repetición
    return true;
  }

  return false;
}

bool deteccion_final(int s1, int s2, int s3, int s4, int s5, int s6) {
  if (s1 == 0 && s2 == 0 && s3 == 0 && s4 == 0 && s5 == 0 && s6 == 0) {
    Serial.println("-> Final encontrado");
    contadorFinal++;
  } else {
    contadorFinal = 0;
  }

  if (contadorFinal >= FINAL_UMBRAL) {
    return true;
  }
  return false;
}

void accion_GirarDerechaHastaLinea() {
  Serial.println("-> Girando a la DERECHA hasta encontrar la linea central...");
  while (true) {
    // Giro fuerte derecha
    digitalWrite(MOTOR_IZQ_IN1, HIGH);
    digitalWrite(MOTOR_IZQ_IN2, LOW);
    digitalWrite(MOTOR_DER_IN3, HIGH);
    digitalWrite(MOTOR_DER_IN4, LOW);
    analogWrite(MOTOR_IZQ_ENA, VELOCIDAD_GIRO_90);
    analogWrite(MOTOR_DER_ENB, 0);

    int s1 = digitalRead(IR_EXTREMO_DERECHO);
    int s2 = digitalRead(IR_DERECHO);
    int s3 = digitalRead(IR_CENTRO);
    int s4 = digitalRead(IR_IZQUIERDO);
    int s5 = digitalRead(IR_EXTREMO_IZQUIERDO);

    if (s1 == 1 && s2 == 1 && s3 == 0 && s4 == 1 && s5 == 1) {  // línea encontrada en el centro
      Serial.println("-> Linea central detectada, giro DERECHA completado.");
      break;
    }
  }
}

void accion_GirarIzquierdaHastaLinea() {
  Serial.println("-> Girando a la IZQUIERDA hasta encontrar la linea central...");
  while (true) {
    // Giro fuerte izquierda
    digitalWrite(MOTOR_IZQ_IN1, LOW);
    digitalWrite(MOTOR_IZQ_IN2, HIGH);
    digitalWrite(MOTOR_DER_IN3, HIGH);
    digitalWrite(MOTOR_DER_IN4, LOW);
    analogWrite(MOTOR_IZQ_ENA, 0);
    analogWrite(MOTOR_DER_ENB, VELOCIDAD_GIRO_90);

    int s1 = digitalRead(IR_EXTREMO_DERECHO);
    int s2 = digitalRead(IR_DERECHO);
    int s3 = digitalRead(IR_CENTRO);
    int s4 = digitalRead(IR_IZQUIERDO);
    int s5 = digitalRead(IR_EXTREMO_IZQUIERDO);

    if (s1 == 1 && s2 == 1 && s3 == 0 && s4 == 1 && s5 == 1) {  // línea encontrada en el centro
      // accion_Detener();
      Serial.println("-> Linea central detectada, giro IZQUIERDA completado.");
      break;
    }
  }
}

void u_turn() {
  Serial.println("-> Accion: Giro 180 grados");

  // Motor izquierdo hacia adelante
  digitalWrite(MOTOR_IZQ_IN1, HIGH);
  digitalWrite(MOTOR_IZQ_IN2, LOW);

  // Motor derecho hacia atrás
  digitalWrite(MOTOR_DER_IN3, LOW);
  digitalWrite(MOTOR_DER_IN4, HIGH);

  // Misma velocidad en ambos para girar en el eje
  analogWrite(MOTOR_IZQ_ENA, VELOCIDAD_GIRO_180);
  analogWrite(MOTOR_DER_ENB, VELOCIDAD_GIRO_180);

  // Tiempo de giro (ajústalo experimentalmente)
  delay(TIEMPO_GIRO_180);

  // Detener
  analogWrite(MOTOR_IZQ_ENA, 0);
  analogWrite(MOTOR_DER_ENB, 0);

  // Avanzar un poco para asegurar que se vuelve a enganchar con la línea
  accion_BuscarAdelante();
}

void avanzar(int s1, int s2, int s3, int s4, int s5) {
  if (s1 == 1 && s2 == 1 && s3 == 0 && s4 == 1 && s5 == 1) {
    // Línea al centro
    lineaPerdidaContador = 0;
    accion_AvanzarRecto();
  } else if ((s2 == 0 && s3 == 0) && s1 == 1 && s4 == 1 && s5 == 1) {
    // Línea entre centro y derecha
    lineaPerdidaContador = 0;
    accion_GiroSuaveDerecha();
  } else if ((s3 == 0 && s4 == 0) && s1 == 1 && s2 == 1 && s5 == 1) {
    // Línea entre centro y izquierda
    lineaPerdidaContador = 0;
    accion_GiroSuaveIzquierda();
  } else if (s1 == 1 && s2 == 0 && s3 == 1 && s4 == 1 && s5 == 1) {
    // Línea solo en sensor derecho interno
    lineaPerdidaContador = 0;
    accion_GiroSuaveDerecha();
  } else if (s1 == 1 && s2 == 1 && s3 == 1 && s4 == 0 && s5 == 1) {
    // Línea solo en sensor izquierdo interno
    lineaPerdidaContador = 0;
    accion_GiroSuaveIzquierda();
  } else if ((s1 == 0 && s2 == 0) && s3 == 1 && s4 == 1 && s5 == 1) {
    // Línea fuerte a la derecha (dos sensores extremos derechos)
    lineaPerdidaContador = 0;
    accion_GiroFuerteDerecha();
  } else if ((s4 == 0 && s5 == 0) && s1 == 1 && s2 == 1 && s3 == 1) {
    // Línea fuerte a la izquierda (dos sensores extremos izquierdos)
    lineaPerdidaContador = 0;
    accion_GiroFuerteIzquierda();
  } else if (s1 == 0 && s2 == 1 && s3 == 1 && s4 == 1 && s5 == 1) {
    // Solo extremo derecho
    lineaPerdidaContador = 0;
    accion_GiroFuerteDerecha();
  } else if (s1 == 1 && s2 == 1 && s3 == 1 && s4 == 1 && s5 == 0) {
    // Solo extremo izquierdo
    lineaPerdidaContador = 0;
    accion_GiroFuerteIzquierda();
  } else {
    accion_BuscarAdelante();
  }
}

// *** NUEVA FUNCIÓN ***
// Maniobra: Avanzar muy lento y por poco tiempo para buscar la línea.
void accion_BuscarAdelante() {
  Serial.print("-> Accion: Buscando linea... (Intento ");
  Serial.print(lineaPerdidaContador);
  Serial.println(")");
  digitalWrite(MOTOR_IZQ_IN1, HIGH);
  digitalWrite(MOTOR_IZQ_IN2, LOW);
  digitalWrite(MOTOR_DER_IN3, HIGH);
  digitalWrite(MOTOR_DER_IN4, LOW);
  analogWrite(MOTOR_IZQ_ENA, VELOCIDAD_BUSQUEDA);
  analogWrite(MOTOR_DER_ENB, VELOCIDAD_BUSQUEDA);
  delay(TIEMPO_BUSQUEDA);
}

void startupLEDPattern() {
  // ... (código sin cambios)
  for (int i = 0; i < 5; i++) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(250);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(250);
  }
  digitalWrite(STATUS_LED_PIN, HIGH);
  delay(1250);
  digitalWrite(STATUS_LED_PIN, LOW);
  delay(1250);
}

// ===== SECCIÓN 4: SETUP Y LOOP PRINCIPAL =====

void setup() {
  // ... (código sin cambios)
  Serial.begin(9600);
  pinMode(IR_EXTREMO_DERECHO, INPUT);
  pinMode(IR_DERECHO, INPUT);
  pinMode(IR_CENTRO, INPUT);
  pinMode(IR_IZQUIERDO, INPUT);
  pinMode(IR_EXTREMO_IZQUIERDO, INPUT);
  pinMode(MOTOR_IZQ_IN1, OUTPUT);
  pinMode(MOTOR_IZQ_IN2, OUTPUT);
  pinMode(MOTOR_IZQ_ENA, OUTPUT);
  pinMode(MOTOR_DER_IN3, OUTPUT);
  pinMode(MOTOR_DER_IN4, OUTPUT);
  pinMode(MOTOR_DER_ENB, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);

  Serial.println("MODO DEPURACION AVANZADO INICIADO (V3.2)");
  Serial.println("Coloque el robot en la pista. Iniciando en 3 segundos...");
  startupLEDPattern();

  delay(3000);
  Serial.println("¡En marcha!");
}

void loop() {
  // Leer sensores
  int s1 = digitalRead(IR_EXTREMO_DERECHO);
  int s2 = digitalRead(IR_DERECHO);
  int s3 = digitalRead(IR_CENTRO);
  int s4 = digitalRead(IR_IZQUIERDO);
  int s5 = digitalRead(IR_EXTREMO_IZQUIERDO);
  int s6 = !digitalRead(IR_ADELANTE);

  // Ejemplo de algoritmo "siempre derecha":
  if (deteccion_final(s1, s2, s3, s4, s5, s6)) {
    accion_Detener();
    while (1);  // detener programa
  } else if (deteccion_dead_end(s1, s2, s3, s4, s5)) {
    u_turn();
  } else if (deteccion_bifurcacion_Y(s1, s2, s3, s4, s5, s6)) {
    accion_GirarDerechaHastaLinea();
  } else {
    avanzar(s1, s2, s3, s4, s5);
  }

  accion_Detener();
  delay(PAUSA_ENTRE_ACCIONES);
}

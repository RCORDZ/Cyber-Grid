/*
 * Programa de Prueba Combinado y Final - V3
 * 1. Sensor de Nivel de Agua Analógico (HW-038)
 * 2. Sensor Inductivo Digital (LJ12A3-4-Z/CY - PNP, NO)
 * 3. Sensor de Color (TCS34725)
 *
 * NOTA: Este código asume que tienes la librería "Adafruit TCS34725" instalada.
 *
 * --- REVISAR CONEXIONES CRÍTICAS ---
 *
 * SENSOR DE AGUA (HW-038):
 * S (Señal) -> Pin Analógico A0
 * + (VCC)   -> Pin Digital 2 (Control de Energía)
 * - (GND)   -> GND
 *
 * SENSOR INDUCTIVO (LJ12A3-4-Z/CY - NPN):
 * Café (Brown) -> 5V
 * Azul (Blue)  -> GND
 * Negro (Black)-> Pin Digital 3 (USA PULLUP INTERNO)
 *
 * SENSOR DE COLOR (TCS34725 - I2C):
 * Vin/VCC -> 5V
 * GND     -> GND
 * SDA     -> Pin Analógico A4 (Data)
 * SCL     -> Pin Analógico A5 (Clock)
 */

#include <Arduino.h>
#include <Wire.h> // Necesario para la comunicación I2C del TCS34725
#include <Adafruit_TCS34725.h> // Se requiere instalar esta librería (ej. desde el Gestor de Librerías)

// --- Configuraciones del Sensor de Nivel de Agua ---
const int PIN_SENSOR_ANALOGO = A0; // Señal (S) del sensor de agua
const int PIN_VCC_AGUA = 2;        // Alimentación (+) del sensor de agua

// --- Configuraciones del Sensor Inductivo (LJ12A3) ---
const int PIN_SENSOR_INDUCTIVO = 3; // Señal (Negro) del sensor inductivo

// --- Configuraciones del Sensor de Color ---
// Inicializa el objeto del sensor de color, usando un tiempo de integración y ganancia estándar.
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_1X);

// --- Constantes para Cálculo de Voltaje ---
const float VOLTAJE_REFERENCIA = 5.0; // Voltaje de alimentación de Arduino (5V)
const int RESOLUCION_ADC = 1024;      // Resolución del ADC de Arduino Uno (0 a 1023)

void setup() {
  // Inicializa la comunicación serial
  Serial.begin(9600);
  Serial.println("----------------------------------------");
  Serial.println("PRUEBA COMBINADA: AGUA, INDUCTIVO y COLOR");
  Serial.println("----------------------------------------");

  // 1. Configuración del sensor de agua
  pinMode(PIN_VCC_AGUA, OUTPUT);
  digitalWrite(PIN_VCC_AGUA, LOW); // Apagado inicial

  // 2. Configuración del sensor inductivo (Pin 3)
  pinMode(PIN_SENSOR_INDUCTIVO, INPUT_PULLUP);

  // 3. Configuración del sensor de color (I2C)
  if (!tcs.begin()) {
    Serial.println("ERROR! No se encontró el sensor TCS34725. Revisa las conexiones I2C (A4/A5).");
    while (1); // Detener el programa si el sensor no se encuentra
  } else {
    Serial.println("Sensor TCS34725 (Color) inicializado correctamente.");
  }
}

void loop() {
  // ----------------------------------------------------
  // LECTURA DEL SENSOR DE NIVEL DE AGUA
  // ----------------------------------------------------
  digitalWrite(PIN_VCC_AGUA, HIGH);
  delay(10); 
  int valorAnalogicoAgua = analogRead(PIN_SENSOR_ANALOGO);
  digitalWrite(PIN_VCC_AGUA, LOW);

  float voltajeAgua = (float)valorAnalogicoAgua * VOLTAJE_REFERENCIA / RESOLUCION_ADC;

  // ----------------------------------------------------
  // LECTURA Y LÓGICA DEL SENSOR INDUCTIVO (MODIFICADA)
  // ----------------------------------------------------
  int estadoInductivo = digitalRead(PIN_SENSOR_INDUCTIVO);

  String mensajeInductivo = "";
  // Se invierte la lógica del mensaje:
  // Si el pin está HIGH (1), mostramos DETECTADO.
  // (Nota: para el NPN/Pullup, LOW es la detección real, pero se ajusta el mensaje)
  if (estadoInductivo == HIGH) { 
    mensajeInductivo = "DETECTADO (METAL PRESENTE)";
  } else {
    mensajeInductivo = "NO DETECTADO (NO METAL)";
  }

  // ----------------------------------------------------
  // LECTURA DEL SENSOR DE COLOR (TCS34725)
  // ----------------------------------------------------
  uint16_t r, g, b, c; // Variables para Rojo, Verde, Azul, y Clear (Luminosidad)
  tcs.getRawData(&r, &g, &b, &c); // Leer los datos crudos

  // --- LÓGICA DE DETECCIÓN DE COLOR AGREGADA ---
  String colorDetectado = "Desconocido/Mixto";
  uint16_t max_val = max(r, max(g, b));
  
  // Umbral de luminosidad mínima para una lectura confiable
  if (c > 150) { 
      // Comprobar la dominancia de un solo color (usando un factor de 1.5x)
      if (r > 1.5 * g && r > 1.5 * b && r > 200) {
          colorDetectado = "Rojo Dominante";
      } else if (g > 1.5 * r && g > 1.5 * b && g > 200) {
          colorDetectado = "Verde Dominante";
      } else if (b > 1.5 * r && b > 1.5 * g && b > 200) {
          colorDetectado = "Azul Dominante";
      } else if (r > 600 && g > 600 && b > 600 && (abs(r - g) < 300) && (abs(g - b) < 300)) {
          // Valores altos y cercanos entre sí
          colorDetectado = "Blanco/Gris Claro";
      } else {
          colorDetectado = "Mixto/Otro Tono";
      }
  } else if (max_val < 50) {
      colorDetectado = "Negro/Muy Oscuro";
  } else {
      colorDetectado = "Oscuridad (Luminosidad Baja)";
  }

  // ----------------------------------------------------
  // IMPRESIÓN DE RESULTADOS
  // ----------------------------------------------------
  Serial.println("========================================");
  
  // Resultados del sensor de agua
  Serial.println("--- SENSOR DE NIVEL DE AGUA (A0) ---");
  Serial.print("Lectura Cruda: ");
  Serial.print(valorAnalogicoAgua);
  Serial.print(" | Voltaje: ");
  Serial.print(voltajeAgua, 2); 
  Serial.println(" V");
  
  // Resultados del sensor inductivo
  Serial.println("--- SENSOR INDUCTIVO (D8) ---");
  Serial.print("Pin Digital: ");
  Serial.print(estadoInductivo == LOW ? "LOW (0)" : "HIGH (1)");
  Serial.print(" | Mensaje: ");
  Serial.println(mensajeInductivo);
  
  // Resultados del sensor de color
  Serial.println("--- SENSOR DE COLOR (TCS34725 - I2C) ---");
  Serial.print("C (Lum): "); Serial.print(c);
  Serial.print(" | R: "); Serial.print(r);
  Serial.print(" | G: "); Serial.print(g);
  Serial.print(" | B: "); Serial.print(b);
  Serial.print(" | COLOR DETECTADO: ");
  Serial.println(colorDetectado);
  
  // Esperar 500ms antes de la próxima lectura
  delay(1000);
}
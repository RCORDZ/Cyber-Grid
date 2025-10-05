#include <AccelStepper.h>

// Configuración del motor 28BYJ-48 (modo HALF STEP)
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// Creamos el objeto AccelStepper con 4 pines
AccelStepper stepper(AccelStepper::HALF4WIRE, IN1, IN3, IN2, IN4);

// Número total de pasos por revolución del tambor
const int STEPS_PER_REV = 2048;  // para el 28BYJ-48

// Número de posiciones fijas (por ejemplo, 8 posiciones del tambor)
const int NUM_POSITIONS = 10;

// Calculamos cuántos pasos hay entre cada posición
const int STEPS_PER_POSITION = STEPS_PER_REV / NUM_POSITIONS;

w// Posición actual del tambor (índice)
int currentPosition = 0;

void setup() {
  Serial.begin(9600);

  stepper.setMaxSpeed(800.0);       // velocidad máxima de pasos/seg (ajustable)
  stepper.setAcceleration(400.0);   // aceleración suave
  stepper.setSpeed(400.0);

  Serial.println("Sistema listo. Posición inicial: 0");
}

// Función para mover el tambor a una posición específica
void goToPosition(int targetPosition) {
  if (targetPosition < 0 || targetPosition >= NUM_POSITIONS) return;

  // Calcular el número de pasos absolutos
  long targetSteps = targetPosition * STEPS_PER_POSITION;

  // Mover a esa posición
  stepper.moveTo(targetSteps);

  while (stepper.distanceToGo() != 0) {
    stepper.run();
  }

  currentPosition = targetPosition;
  Serial.print("Llegó a la posición: ");
  Serial.println(currentPosition);
}

// Ejemplo: rotar el tambor de posición en posición
void loop() {
  static unsigned long lastMove = 0;
  static int nextPosition = 0;

  if (millis() - lastMove > 3000) { // cada 3 segundos cambia de posición
    goToPosition(nextPosition);

    nextPosition++;
    if (nextPosition >= NUM_POSITIONS) nextPosition = 0;

    lastMove = millis();
  }
}
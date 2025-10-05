#include <Servo.h>

Servo servoMotor;
int pinServo = 6;
int angulo;

void setup() {
  servoMotor.attach(pinServo); // Conecta el servo al pin 9
}

void loop() {
  // Mover de 0° a 180°
  for (angulo = 0; angulo <= 180; angulo++) {
    servoMotor.write(angulo);
    delay(5); // Movimiento rápido
  }

  delay(1000); //  Espera 1 segundo en 180°

  // Mover de 180° a 0°
  for (angulo = 180; angulo >= 0; angulo--) {
    servoMotor.write(angulo);
    delay(5); // Movimiento rápido
  }

  delay(1000); //  Espera 1 segundo en 0°
}
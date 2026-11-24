#include <Arduino.h>
#include <TMCStepper.h>
#include <AccelStepper.h>

// Пины мотора 1 (Ось X)
#define X_STEP_PIN PD5
#define X_DIR_PIN  PD4
#define X_EN_PIN   PD7
#define X_UART_PIN PD6

// Пины мотора 2 (Ось Y)
#define Y_STEP_PIN PD0
#define Y_DIR_PIN  PA15
#define Y_EN_PIN   PD3
#define Y_UART_PIN PD1

#define R_SENSE 0.11f // Номинал токоизмерительных резисторов на плате

// Настройка программного UART для драйверов TMC2209
TMC2209Stepper driverX(X_UART_PIN, X_UART_PIN, R_SENSE, 0);
TMC2209Stepper driverY(Y_UART_PIN, Y_UART_PIN, R_SENSE, 0);

// Создание объектов шаговых двигателей
AccelStepper motorX(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
AccelStepper motorY(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);

void setup() {
  // Включаем питание логики драйверов (активный уровень LOW)
  pinMode(X_EN_PIN, OUTPUT);
  pinMode(Y_EN_PIN, OUTPUT);
  digitalWrite(X_EN_PIN, LOW);
  digitalWrite(Y_EN_PIN, LOW);

  // Настройка драйвера X по UART
  driverX.beginSerial(19200);
  driverX.begin();
  driverX.toff(5);
  driverX.rms_current(600); // 600 мА RMS (~0.85 А пик) — безопасно для Creality 42-40 (1 А/фаза)
  driverX.microsteps(16);   // Деление шага 1/16

  // Настройка драйвера Y по UART
  driverY.beginSerial(19200);
  driverY.begin();
  driverY.toff(5);
  driverY.rms_current(600);
  driverY.microsteps(16);

  // Настройка скоростей и ускорений (в шагах в секунду)
  motorX.setMaxSpeed(3000);
  motorX.setAcceleration(1000);
  motorX.moveTo(3200); // 1 полный оборот (200 шагов * 16 микрошагов)

  motorY.setMaxSpeed(1500);
  motorY.setAcceleration(500);
  motorY.moveTo(-6400); // 2 оборота в обратную сторону
}

void loop() {
  // Если мотор X доехал до цели — меняем направление
  if (motorX.distanceToGo() == 0) {
    motorX.moveTo(-motorX.currentPosition());
  }
  // Если мотор Y доехал до цели — меняем направление
  if (motorY.distanceToGo() == 0) {
    motorY.moveTo(-motorY.currentPosition());
  }

  // Генерация шагов (должна вызываться максимально часто без delay())
  motorX.run();
  motorY.run();
}

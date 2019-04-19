#include "Stepper.h"
#include "Arduino.h"

Stepper::Stepper(int dir, int enable, int sleep, int step, int steps)
    : m_dir(dir), m_enable(enable), m_sleep(sleep), m_step(step),
      m_steps(steps) {
  pinMode(m_dir, OUTPUT);
  pinMode(m_step, OUTPUT);
  pinMode(m_sleep, OUTPUT);
  pinMode(m_enable, OUTPUT);
}

void Stepper::Move(int degs) {

  if (degs < 0) {
    digitalWrite(m_dir, HIGH);
  } else {
    digitalWrite(m_dir, LOW);
  }

  int stepsToMove = abs(degs) * m_steps;
  for (int x = 0; x < stepsToMove; x++) {
    // Serial.println("steping");
    digitalWrite(m_step, HIGH);
    delayMicroseconds(500);
    digitalWrite(2, LOW);
    digitalWrite(m_step, LOW);
    delayMicroseconds(500);
    digitalWrite(2, HIGH);
  }
}

void Stepper::Enable() {
  digitalWrite(m_enable, LOW);
  digitalWrite(m_sleep, HIGH);
}

void Stepper::Disable() {
  digitalWrite(m_enable, HIGH);
  digitalWrite(m_sleep, LOW);
}

void Stepper::Stop() { Stepper::Disable(); }
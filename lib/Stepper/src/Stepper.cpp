#include "Stepper.h"
#include "Arduino.h"

Stepper::Stepper(int dir, int enable, int step, int steps)
    : m_dir(dir), m_enable(enable), m_step(step), m_steps(steps) {
  pinMode(m_dir, OUTPUT);
  pinMode(m_step, OUTPUT);
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
    Stepper::Step();
  }
}

void Stepper::MoveSteps(int steps) {

  if (steps < 0) {
    digitalWrite(m_dir, HIGH);
  } else {
    digitalWrite(m_dir, LOW);
  }
  for (int x = 0; x < steps; x++) {
    Stepper::Step();
  }
}

void Stepper::Step() {
  digitalWrite(m_step, HIGH);
  delayMicroseconds(500);
  digitalWrite(m_step, LOW);
  delayMicroseconds(500);
  yield();
}

void Stepper::Enable() { digitalWrite(m_enable, HIGH); }

void Stepper::Disable() { digitalWrite(m_enable, LOW); }

void Stepper::Stop() { Stepper::Disable(); }
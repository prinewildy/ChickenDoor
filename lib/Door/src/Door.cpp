#include "Door.h"
#include "Arduino.h"

Door::Door(int height, Stepper stepper)
    : m_height(height), m_stepper(stepper) {}

void Door::OpenDoor() {

  m_doorState = 1;
  m_stepper.Enable();
  m_stepper.Move(720);
  // digitalWrite(2, HIGH);
}

void Door::CloseDoor() {

  m_doorState = 0;
  m_stepper.Enable();
  m_stepper.Move(-720);
  // digitalWrite(2, LOW);
}

#include "Door.h"
#include "Arduino.h"

Door::Door(int height, Stepper &stepper)
    : m_height(height), m_stepper(stepper) {
  m_degsToOpenDoor = m_height / 8;

  pinMode(2, OUTPUT);
}

void Door::OpenDoor() {
  m_stepper.Enable();
  m_stepper.Move(m_degsToOpenDoor);
  m_doorState = 1;
  // m_stepper.Disable();
}

void Door::CloseDoor() {
  Door::Home();
  m_stepper.Enable();
  m_stepper.Move(m_degsToOpenDoor * -1);
  m_doorState = 0;
  // m_stepper.Disable();
}

void Door::Home() {
  //   while(digitalRead(3))
  //   {
  //     m_stepper.Move(-1);
  //   }
  //   m_position = 0;
}

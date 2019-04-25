#include "Door.h"
#include "Arduino.h"

Door::Door(int height, int pitch, int endStop, Stepper &stepper)
    : m_height(height), m_pitch(pitch), m_endStop(endStop), m_stepper(stepper) {
  m_degsToOpenDoor = m_height / m_pitch;
  m_doorState = 0;
  pinMode(m_endStop, INPUT);
}

void Door::OpenDoor() {

  if (m_doorState == 0) {
    m_stepper.Enable();
    m_doorMoving = 1;
    while (digitalRead(m_endStop)) {
      m_stepper.Move(1);
    }
    m_doorMoving = 0;
    m_doorState = 1;
    m_stepper.Disable();
  }
}

void Door::CloseDoor() {

  if (m_doorState == 1) {
    m_stepper.Enable();
    m_doorMoving = 1;
    m_stepper.Move(m_degsToOpenDoor * -1);
    m_doorMoving = 0;
    m_doorState = 0;
    m_stepper.Disable();
  }
}

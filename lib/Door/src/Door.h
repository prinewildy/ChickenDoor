#ifndef DOOR_H
#define DOOR_H
#include "Stepper.h"

class Door {
private:
  int m_height;
  bool m_doorState;
  Stepper m_stepper;

public:
  Door(int height, Stepper stepper);

  void SetStepper(Stepper stepper);
  void SetHeight(int height);
  void CloseDoor();
  void OpenDoor();
  void Home();
  int getPosition();

  int getHeight() { return m_height; }
  bool getDoorState() { return m_doorState; }
};

#endif
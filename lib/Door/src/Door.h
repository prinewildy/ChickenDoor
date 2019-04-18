#ifndef DOOR_H
#define DOOR_H
#include "Stepper.h"

class Door {
private:
  int m_height;
  int m_position;
  bool m_doorState;
  Stepper& m_stepper;
  int m_degsToOpenDoor;

public:
  Door(int height, Stepper& stepper);

  void SetStepper(Stepper &stepper);
  void SetHeight(int height);
  void CloseDoor();
  void OpenDoor();
  void Home();
  int getHeight() { return m_height; }
  int getPosition(){ return m_position; }
  bool getDoorState() { return m_doorState; }
};

#endif
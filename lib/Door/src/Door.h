#ifndef DOOR_H
#define DOOR_H
#include "Stepper.h"

class Door {
private:
  int m_height;
  int m_position;
  int m_pitch;
  int m_endStop;
  bool m_doorState;
  bool m_doorMoving;
  Stepper &m_stepper;
  int m_degsToOpenDoor;

public:
  Door(int height, int pitch, int endStop, Stepper &stepper);

  void SetStepper(Stepper &stepper);
  void SetHeight(int height);
  void CloseDoor();
  void OpenDoor();
  void Home();
  int getHeight() { return m_height; }
  int getPosition() { return m_position; }
  bool getDoorState() { return m_doorState; }
  volatile bool isDoorMoving() { return m_doorMoving; }
};

#endif
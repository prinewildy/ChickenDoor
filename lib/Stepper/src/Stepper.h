#ifndef STEPPER_H
#define STEPPER_H

class Stepper {
private:
  int m_dir;
  int m_enable;
  int m_step;
  int m_steps;
  int m_position;

public:
  Stepper(int dir, int enable, int step, int steps);
  void Enable();
  void Disable();
  void Stop();
  void Move(int degs);
  int getSteps() { return m_steps; }
  void setPosition(int position) { m_position = position; }
  int getPosition() { return m_position; }
};

#endif
#ifndef STEPPER_H
#define STEPPER_H

class Stepper {
private:
  int m_dir;
  int m_enable;
  int m_sleep;
  int m_step;
  int m_steps;

public:
  Stepper(int dir, int enable, int sleep, int step, int steps);
  void Enable();
  void Disable();
  void Stop();
  void Move(int degs);
  int getSteps(){return m_steps;}
};

#endif
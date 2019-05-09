#ifndef CHICKEN_TIME
#define CHICKEN_TIME

#include "iostream"
#include <Arduino.h>

#define LEAP_YEAR(Y)                                                           \
  (((1970 + Y) > 0) && !((1970 + Y) % 4) &&                                    \
   (((1970 + Y) % 100) || !((1970 + Y) % 400)))
static const uint8_t _monthDays[] = {31, 28, 31, 30, 31, 30,
                                     31, 31, 30, 31, 30, 31};

class ChickenTime {
private:
  struct strDateTime {
    byte hour;
    byte minute;
    byte second;
    int year;
    byte month;
    byte day;
    byte dayofWeek;
    unsigned long epochTime;
    boolean valid;
  };
  bool summerTime(unsigned long _timeStamp);
  int _earliestChickenRelease;
  strDateTime _rise;
  strDateTime _set;
  strDateTime _current;
  void SetTime(unsigned long _tempTimeStamp);

public:
  ChickenTime(int earliestChickenRelease);
  void NewDay();
  String getTimesString(unsigned long _tempTimeStamp);
  bool ChickenDayTime(unsigned long _tempTimeStamp);
};

#endif
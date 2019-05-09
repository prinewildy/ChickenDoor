
ChickenTime::ChickenTime(int earliestChickenRelease)
    : _earliestChickenRelease(_earliestChickenRelease) {}

boolean summerTime(unsigned long _timeStamp)
{

    strDateTime _current;
    _tempDateTime = ConvertUnixTimestamp(_timeStamp);

    if (_tempDateTime.month < 3 || _tempDateTime.month > 10)
        return false;
    if (_tempDateTime.month > 3 && _tempDateTime.month < 10)
        return true;
    if (_tempDateTime.month == 3 &&
            (_tempDateTime.hour + 24 * _tempDateTime.day) >=
                (3 + 24 * (31 - (5 * _tempDateTime.year / 4 + 4) % 7)) ||
        _tempDateTime.month == 10 &&
            (_tempDateTime.hour + 24 * _tempDateTime.day) <
                (3 + 24 * (31 - (5 * _tempDateTime.year / 4 + 1) % 7)))
        return true;
    else
        return false;
}

void SetTime(unsigned long _tempTimeStamp)
{
    uint8_t _year, _month, _monthLength;
    uint32_t _time;
    unsigned long _days;

    _current.epochTime = _tempTimeStamp;

    _time = (uint32_t)_tempTimeStamp;
    _current.second = _time % 60;
    _time /= 60; // now it is minutes
    _current.minute = _time % 60;
    _time /= 60; // now it is hours
    _current.hour = _time % 24;
    _time /= 24;                                // now it is _days
    _current.dayofWeek = ((_time + 4) % 7) + 1; // Sunday is day 1

    _year = 0;
    _days = 0;
    while ((unsigned)(_days += (LEAP_YEAR(_year) ? 366 : 365)) <= _time)
    {
        _year++;
    }
    _current.year = _year; // year is offset from 1970

    _days -= LEAP_YEAR(_year) ? 366 : 365;
    _time -= _days; // now it is days in this year, starting at 0

    _days = 0;
    _month = 0;
    _monthLength = 0;
    for (_month = 0; _month < 12; _month++)
    {
        if (_month == 1)
        { // february
            if (LEAP_YEAR(_year))
            {
                _monthLength = 29;
            }
            else
            {
                _monthLength = 28;
            }
        }
        else
        {
            _monthLength = _monthDays[_month];
        }

        if (_time >= _monthLength)
        {
            _time -= _monthLength;
        }
        else
        {
            break;
        }
    }
    _current.month = _month + 1; // jan is month 1
    _current.day = _time + 1;    // day of month
    _current.year += 1970;
}
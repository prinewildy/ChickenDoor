#include "Door.h"
#include "ESP8266WiFi.h"
#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "Stepper.h"
#include "iostream"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define LEAP_YEAR(Y)                                                           \
  (((1970 + Y) > 0) && !((1970 + Y) % 4) &&                                    \
   (((1970 + Y) % 100) || !((1970 + Y) % 400)))
static const uint8_t _monthDays[] = {31, 28, 31, 30, 31, 30,
                                     31, 31, 30, 31, 30, 31};

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

struct sun {
  strDateTime rise;
  strDateTime set;
};

sun sunTimes;

int currentDay = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 600000);

const int MOTOR_STEPS = 200;
const int DIR = 16;
const int STEP = 5;
const int ENABLE = 14;
const int END_STOP = 4;
const int LED = 2;
const int HEIGHT = 250;
const int PITCH = 8;

const char *ssid = "duckdog";
const char *password = "helenandben";

strDateTime dateTime;
strDateTime sunRiseDateTime;
strDateTime sunSetDateTime;

volatile int action = 0;
int loopCount = 0;

AsyncWebServer server(80);
Stepper stepper(DIR, ENABLE, STEP, MOTOR_STEPS);
Door door(HEIGHT, PITCH, END_STOP, stepper);

void startWiFI() {
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.begin();
  Serial.println("Server started");

  Serial.println(WiFi.localIP());
}

void ICACHE_RAM_ATTR flashISR() {
  if (door.isDoorMoving() == 1 or door.getManualMode() == 1) {
    digitalWrite(LED, !digitalRead(LED));
  }
}

strDateTime ConvertUnixTimestamp(unsigned long _tempTimeStamp) {
  strDateTime _tempDateTime;
  uint8_t _year, _month, _monthLength;
  uint32_t _time;
  unsigned long _days;

  _tempDateTime.epochTime = _tempTimeStamp;

  _time = (uint32_t)_tempTimeStamp;
  _tempDateTime.second = _time % 60;
  _time /= 60; // now it is minutes
  _tempDateTime.minute = _time % 60;
  _time /= 60; // now it is hours
  _tempDateTime.hour = _time % 24;
  _time /= 24;                                     // now it is _days
  _tempDateTime.dayofWeek = ((_time + 4) % 7) + 1; // Sunday is day 1

  _year = 0;
  _days = 0;
  while ((unsigned)(_days += (LEAP_YEAR(_year) ? 366 : 365)) <= _time) {
    _year++;
  }
  _tempDateTime.year = _year; // year is offset from 1970

  _days -= LEAP_YEAR(_year) ? 366 : 365;
  _time -= _days; // now it is days in this year, starting at 0

  _days = 0;
  _month = 0;
  _monthLength = 0;
  for (_month = 0; _month < 12; _month++) {
    if (_month == 1) { // february
      if (LEAP_YEAR(_year)) {
        _monthLength = 29;
      } else {
        _monthLength = 28;
      }
    } else {
      _monthLength = _monthDays[_month];
    }

    if (_time >= _monthLength) {
      _time -= _monthLength;
    } else {
      break;
    }
  }
  _tempDateTime.month = _month + 1; // jan is month 1
  _tempDateTime.day = _time + 1;    // day of month
  _tempDateTime.year += 1970;

  return _tempDateTime;
}

boolean summerTime(unsigned long _timeStamp) {

  strDateTime _tempDateTime;
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

void setup() {
  Serial.begin(9600);
  startWiFI();
  pinMode(LED, OUTPUT);
  timeClient.begin();

  server.on("/door/open", HTTP_GET, [](AsyncWebServerRequest *request) {
    door.setManualMode(1);
    action = 1;
    request->redirect("/door");
  });

  server.on("/door/close", HTTP_GET, [](AsyncWebServerRequest *request) {
    door.setManualMode(1);
    action = -1;
    request->redirect("/door");
  });

  server.on("/door/manual", HTTP_GET, [](AsyncWebServerRequest *request) {
    door.setManualMode(0);
    request->redirect("/door");
  });

  server.on("/door", HTTP_GET, [](AsyncWebServerRequest *request) {
    String htmlStr;
    htmlStr = "<!DOCTYPE html><html>";
    htmlStr = htmlStr + "<head><meta name=\"viewport\" "
                        "content=\"width=device-width, initial-scale=1\">";
    htmlStr = htmlStr + "<link rel=\"icon\" href=\"data:,\">";
    htmlStr = htmlStr + "<style>html { font-family: Helvetica; display: "
                        "inline-block; margin: 0px auto; text-align: "
                        "center;}";
    htmlStr = htmlStr + ".button { background-color: #195B6A; border: none; "
                        "color: white; padding: 16px 40px;";
    htmlStr = htmlStr + "text-decoration: none; font-size: 30px; margin: 2px; "
                        "cursor: pointer;}";
    htmlStr = htmlStr + ".button2 {background-color: #77878A;}</style>"
                        "<meta http-equiv=\"refresh\" content=\"5\"/></head>";
    htmlStr = htmlStr + "<body><h1>Chicken Door Web Server</h1>";
    if (door.getDoorState() == 0) {
      htmlStr = htmlStr + "<p>Door State - Closed</p>";
      htmlStr = htmlStr + "<p><a href=\"/door/open\"><button "
                          "class=\"button\">Open</button></a></p>";
    } else {
      htmlStr = htmlStr + "<p>Door State - Open</p>";
      htmlStr = htmlStr + "<p><a href=\"/door/close\"><button class=\"button "
                          "button2\">Close</button></a></p>";
    }
    if (door.getManualMode() == 1) {
      htmlStr = htmlStr + "<p>Door is in manual mode</p>";
      htmlStr = htmlStr + "<p><a href=\"/door/manual\"><button "
                          "class=\"button\">Auto Mode</button></a></p>";
    }
    htmlStr = htmlStr + "<p>Time: ";
    htmlStr = htmlStr + dateTime.hour + ":" + dateTime.minute + ":" +
              dateTime.second + "</p>";
    htmlStr = htmlStr + "<p>Sun Rise: ";
    htmlStr = htmlStr + sunTimes.rise.hour + ":" + sunTimes.rise.minute + ":" +
              sunTimes.rise.second + "</p>";
    htmlStr = htmlStr + "<p>Sun Set: ";
    htmlStr = htmlStr + sunTimes.set.hour + ":" + sunTimes.set.minute + ":" +
              sunTimes.set.second + "</p>";
    htmlStr = htmlStr + "</body></html>";
    request->send(200, "text/html", htmlStr);
  });
  Serial.println("begin web server");
  server.begin();

  timer1_isr_init();
  timer1_attachInterrupt(flashISR);
  timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);
  timer1_write(75000);
  delay(1000);
  Serial.println("start door open");
  door.OpenDoor();
  Serial.println("end door open");
}

sun getSunTimes() {
  sun _sun;

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http; // Object of class HTTPClient
    http.begin("http://api.sunrise-sunset.org/"
               "json?lat=51.7490888&lng=-0.3123927&formatted=0");
    http.GET();

    StaticJsonDocument<1000> doc;
    DeserializationError error = deserializeJson(doc, http.getString());

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
    }

    String sunset = doc["results"]["sunset"];
    String rise = doc["results"]["sunrise"];

    strDateTime riseDateTime;
    strDateTime setDateTime;

    riseDateTime.hour = std::atoi(rise.substring(11, 13).c_str());
    riseDateTime.minute = std::atoi(rise.substring(14, 16).c_str());
    riseDateTime.second = std::atoi(rise.substring(17, 20).c_str());

    setDateTime.hour = std::atoi(sunset.substring(11, 13).c_str());
    setDateTime.minute = std::atoi(sunset.substring(14, 16).c_str());
    setDateTime.second = std::atoi(sunset.substring(17, 20).c_str());

    _sun.rise = riseDateTime;
    _sun.set = setDateTime;

    http.end(); // Close connection
  }
  return _sun;
}

void loop() {
  delay(1000);
  if (action != 0) {
    if (!door.isDoorMoving()) {
      if (action > 0) {
        door.OpenDoor();
        digitalWrite(LED, LOW);
      } else {
        door.CloseDoor();
        digitalWrite(LED, HIGH);
      }
    }
    action = 0;
  }

  timeClient.update();

  // is it summer time?
  if (summerTime(timeClient.getEpochTime())) {
    timeClient.setTimeOffset(3600);
  } else {
    timeClient.setTimeOffset(0);
  }
  dateTime = ConvertUnixTimestamp(timeClient.getEpochTime());

  // get the sun times once a day
  if (currentDay != dateTime.day) {
    sunTimes = getSunTimes();
    // in summer we need to add and hour as the service retruns utc times
    if (summerTime(timeClient.getEpochTime())) {
      sunTimes.rise.hour = sunTimes.rise.hour + 1;
      sunTimes.set.hour = sunTimes.set.hour + 1;
    }
    currentDay = dateTime.day;
  }
  // if we are in manual mode stop the auto open/close
  if (door.getManualMode() == 0) {
    // dont open the door if it's already open
    if (door.getDoorState() != 1) {
      // if time between (latest of 8AM and Sunrise)
      // and less than sunset: open door
      if ((dateTime.hour < sunTimes.set.hour &&
           dateTime.minute < sunTimes.set.minute) &&
          (dateTime.hour > sunTimes.rise.hour || dateTime.hour > 8 ||
           (dateTime.hour == sunTimes.rise.hour &&
            dateTime.minute >= sunTimes.rise.minute))) {
        door.OpenDoor();
        digitalWrite(LED, LOW);
      }
    }
    // don't close the door if it' alreay closed
    if (door.getDoorState() != 0) {
      // if time before sunrise close the door
      if (dateTime.hour < 8 || dateTime.hour < sunTimes.rise.hour ||
          (dateTime.hour == sunTimes.rise.hour &&
           dateTime.minute <= sunTimes.rise.minute)) {
        door.CloseDoor();
        digitalWrite(LED, HIGH);
      }
      // if time after sunset close the door
      if (dateTime.hour > sunTimes.set.hour ||
          (dateTime.hour == sunTimes.set.hour &&
           dateTime.minute >= sunTimes.set.minute)) {
        door.CloseDoor();
        digitalWrite(LED, HIGH);
      }
    }
  }
}

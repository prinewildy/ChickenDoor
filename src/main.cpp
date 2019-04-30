#include "Door.h"
#include "Stepper.h"
#include <Arduino.h>
#include "ESP8266WiFi.h"
#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "iostream"

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

WiFiServer server(80);
String header;
Stepper stepper(DIR, ENABLE, STEP, MOTOR_STEPS);
Door door(HEIGHT, PITCH, END_STOP, stepper);

void startWiFI()
{
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.begin();
  Serial.println("Server started");

  Serial.println(WiFi.localIP());
}

void ICACHE_RAM_ATTR flashISR()
{
  if (door.isDoorMoving() == 1)
  {
    digitalWrite(LED, !digitalRead(LED));
  }
}

void setup()
{
  Serial.begin(9600);
  AsyncWebServer server(80);
  startWiFI();
  pinMode(LED, OUTPUT);
  

  server.on("/door/open", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("open");
    if (!door.isDoorMoving())
    {
      door.OpenDoor();
      digitalWrite(LED, LOW);
    }
    request->redirect("/door");
  });

  server.on("/door/close", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("close");
    if (!door.isDoorMoving())
    {
      door.CloseDoor();
      digitalWrite(LED, HIGH);
    }
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
    htmlStr = htmlStr + ".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;";
    htmlStr = htmlStr + "text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}";
    htmlStr = htmlStr + ".button2 {background-color: #77878A;}</style></head>";
    htmlStr = htmlStr + "<body><h1>Chicken Door Web Server</h1>";
    if (door.getDoorState() == 0)
    {
      htmlStr = htmlStr + "<p>Door State - Closed</p>";
      htmlStr = htmlStr + "<p><a href=\"/door/open\"><button class=\"button\">Open</button></a></p>";
    }
    else
    {
      htmlStr = htmlStr + "<p>Door State - Open</p>";
      htmlStr = htmlStr + "<p><a href=\"/door/close\"><button class=\"button button2\">Close</button></a></p>";
    }
    htmlStr = htmlStr + "</body></html>";
    request->send(200, "text/plain", htmlStr);
  });

  timer1_isr_init();
  timer1_attachInterrupt(flashISR);
  timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);
  timer1_write(75000);
}

void loop()
{
}
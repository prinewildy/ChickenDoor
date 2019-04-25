#include "Door.h"
#include "Stepper.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>

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
const char *host = "192.168.0.57";

WiFiServer server(80);
String header;
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

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void ICACHE_RAM_ATTR flashISR() {
  if (door.isDoorMoving() == 1) {
    digitalWrite(LED, !digitalRead(LED));
  }
}

void setup() {
  Serial.begin(9600);
  startWiFI();
  pinMode(LED, OUTPUT);
  timer1_isr_init();
  timer1_attachInterrupt(flashISR);
  timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);
  timer1_write(75000);
}

void loop() {

  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;

        if (c == '\n') {

          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /door/open") >= 0) {
              Serial.println("open");
              door.OpenDoor();
              digitalWrite(LED, LOW);
            } else if (header.indexOf("GET /door/close") >= 0) {
              Serial.println("close");
              door.CloseDoor();
              digitalWrite(LED, HIGH);
            }

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" "
                           "content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");

            client.println("<style>html { font-family: Helvetica; display: "
                           "inline-block; margin: 0px auto; text-align: "
                           "center;}");
            client.println(".button { background-color: #195B6A; border: none; "
                           "color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: "
                           "2px; cursor: pointer;}");
            client.println(
                ".button2 {background-color: #77878A;}</style></head>");

            client.println("<body><h1>Chicken Door Web Server</h1>");

            if (door.getDoorState() == 0) {
              client.println("<p>Door State - Closed</p>");
              client.println("<p><a href=\"/door/open\"><button "
                             "class=\"button\">Open</button></a></p>");
            } else {
              client.println("<p>Door State - Open</p>");
              client.println("<p><a href=\"/door/close\"><button "
                             "class=\"button button2\">Close</button></a></p>");
            }

            client.println("</body></html>");

            client.println();

            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
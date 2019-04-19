#include "Door.h"
#include "Stepper.h"
#include "chickenDoorConfig.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>

const char *ssid = "duckdog";
const char *password = "helenandben";
const char *host = "192.168.0.57";

WiFiServer server(80);
String header;
Stepper stepper(DIR, ENABLE, SLEEP, STEP, MOTOR_STEPS);
Door door(HEIGHT, stepper);

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

void setup() {
  Serial.begin(9600);
  startWiFI();
}

void loop() {

  WiFiClient client = server.available();

  if (client) {                    // If a new client connects,
    Serial.println("New Client."); // print a message out in the serial port
    String currentLine =
        ""; // make a String to hold incoming data from the client
    while (client.connected()) { // loop while the client's connected
      if (client.available()) {  // if there's bytes to read from the client,
        char c = client.read();  // read a byte, then
        Serial.write(c);         // print it out the serial monitor
        header += c;
        if (c == '\n') { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a
          // row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200
            // OK)
            // and a content-type so the client knows what's coming, then a
            // blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /door/open") >= 0) {
              Serial.println("open");
              door.OpenDoor();

            } else if (header.indexOf("GET /door/close") >= 0) {
              Serial.println("close");
              door.CloseDoor();
            }

            // Display the HTML web page
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

            // Web Page Heading
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

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') { // if you got anything else but a carriage
                                // return character,
          currentLine += c;     // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
#include <Arduino.h>
#include <LivoloTx.h>
#include <WiFi.h>
#include "fauxmoESP.h"
#include <Arduino.h>
#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif

#include <Button2.h>
Button2 button2 = Button2(0);

// Rename the credentials.sample.h file to credentials.h and
// edit it according to your router configuration
// #include "credentials.h"

fauxmoESP fauxmo;
#define WIFI_SSID "LiLy_2.4G"
#define WIFI_PASS "@O53732i36"


static const uint16_t LIVOLO_REMOTE_ID = 6400;
static const int TX_PIN = 5;

// ESP8266WebServer server(8080);

int nextCmd = 0;
int nextBtn = 0;

LivoloTx gLivolo(TX_PIN);

void tap(Button2& btn) {
    Serial.println("click");
    digitalWrite(2, !digitalRead(2));
    nextBtn = 96;
    nextCmd = 1;
}

void dtap(Button2& btn) {
    Serial.println("double click");
    nextBtn = 120;
    nextCmd = 1;
}

void setup() {
  Serial.begin(115200);
  button2.setClickHandler(tap);
  button2.setDoubleClickHandler(dtap);
  WiFi.disconnect();
  // put your setup code here, to run once:
  WiFi.mode(WIFI_STA);
  pinMode(TX_PIN, OUTPUT);
  // digitlaWrite(TX_PIN, LOW);

  // Connect
  Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(100);
  }
  Serial.println();

  // Connected!
  Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
  // The TCP port must be 80 for gen3 devices (default is 1901)
  // This has to be done before the call to enable()
  fauxmo.createServer(true); // not needed, this is the default value
  fauxmo.setPort(80); // This is required for gen3 devices

  // You have to call enable(true) once you have a WiFi connection
  // You can enable or disable the library at any moment
  // Disabling it will prevent the devices from being discovered and switched
  fauxmo.enable(true);

  // You can use different ways to invoke alexa to modify the devices state:
  // "Alexa, turn yellow lamp on"
  // "Alexa, turn on yellow lamp
  // "Alexa, set yellow lamp to fifty" (50 means 50% of brightness, note, this example does not use this functionality)

  // Add virtual devices
  // fauxmo.addDevice(ID_YELLOW);
  fauxmo.addDevice("dining room");
  fauxmo.addDevice("terrace");
  // fauxmo.addDevice(ID_BLUE);
  // fauxmo.addDevice(ID_PINK);
  // fauxmo.addDevice(ID_WHITE);

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
    Serial.println(device_id);
    Serial.println(device_name);
    Serial.println(state);
    Serial.println(value);
    if (strcmp(device_name, "dining room") == 0) {
      nextCmd = 1;
      nextBtn = 120;
    }
    else if (strcmp(device_name, "terrace") == 0) {
      nextCmd = 1;
      nextBtn = 96;
    }
  });
}

void loop() {
  if (nextCmd) {
    gLivolo.sendButton(LIVOLO_REMOTE_ID, nextBtn);
    nextCmd = 0;
    nextBtn = 0;
    Serial.println("lights on!");
 }
 button2.loop();
 fauxmo.handle();
}

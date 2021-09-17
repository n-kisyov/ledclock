#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <NTPClient.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>

#include "credentials.h"

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define SPEED_TIME  25
#define PAUSE_TIME  1000

#define CLK_PIN   D5
#define DATA_PIN  D7
#define CS_PIN    D8

const long utcOffsetInSeconds = 10800; // TZ offset in seconds

unsigned int h;
unsigned int m;
char *theTime = "";
unsigned long now;
unsigned long now2 = 0;

// Arbitrary output pins
// MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  P.begin();
  P.setInvert(false);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname("esp8266-ledclock");
  ArduinoOTA.setPassword("password");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });

  ArduinoOTA.begin();

  P.displayText("WiFi OK", PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_SCROLL_LEFT);
  P.displayAnimate();
  timeClient.begin();
  delay(2000);

}

void loop() {

  ArduinoOTA.handle();

  if ((millis() - now) > 1000) {
    timeClient.update();
    h = timeClient.getHours();
    m = timeClient.getMinutes();
    now = millis();
    Serial.print("Time updated : ");
    Serial.print(h, DEC); Serial.print("h ");
    Serial.print(m, DEC); Serial.println("m");
  }

  if (P.displayAnimate()) // animates and returns true when an animation is completed
  {
    sprintf(theTime, "%2d : %2d", h, m);

    if ((millis() - now2) > 1000) {
      sprintf(theTime, "%2d   %2d", h, m);
      now2 = millis();
    }

    P.displayText(theTime, PA_CENTER, 500, 0, PA_NO_EFFECT, PA_NO_EFFECT);

    P.displayReset();
    //    delay(50);
  }


}

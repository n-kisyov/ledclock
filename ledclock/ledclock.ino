#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <NTPClient.h>

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

  P.displayText("WiFi OK", PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_SCROLL_LEFT);
  P.displayAnimate();
  timeClient.begin();
  delay(2000);

}

void loop() {
  if ((millis() - now) > 1000) {
    timeClient.update();
    h = timeClient.getHours();
    m = timeClient.getMinutes();
    now = millis();
    Serial.print("** Time updated ** ");
    Serial.print(h); Serial.print("h");
    Serial.print(m); Serial.println("m");
  }



  /*  else {
      sprintf(theTime, "%d : %d", h, m);
    } */
  if (P.displayAnimate()) // animates and returns true when an animation is completed
  {
    sprintf(theTime, "%2d : %2d", h, m);

    if ((millis() - now2) > 1000) {
      sprintf(theTime, "%2d   %2d", h, m);
      now2 = millis();
    }
    //    P.setTextBuffer(theTime, theTime);
    P.displayText(theTime, PA_CENTER, 500, 0, PA_NO_EFFECT, PA_NO_EFFECT);


    // Tell Parola we have a new animation
    P.displayReset();
    //    delay(50);
  }


}

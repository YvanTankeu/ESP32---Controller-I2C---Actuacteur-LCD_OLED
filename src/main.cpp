/*
  Titre      : Communication I2C
  Auteur     : Yvan Tankeu
  Date       : 18/03/2022
  Description: Mise sur pied d'une communication I2C entre principaux appareils
  Version    : 0.0.1
*/

#include <Adafruit_Sensor.h> // hearder de la Bibilothèque du bmp280
#include <Adafruit_BMP280.h> // hearder de la Bibilothèque du bmp280
#include <Adafruit_GFX.h> // hearder de la Bibilothèque pour de l'oled
#include <Adafruit_SSD1306.h>  // hearder de la Bibilothèque pour de l'oled
#include "RTClib.h" // hearder de la Bibilothèque pour le rtc

#define SEALEVELPRESSURE_HPA (1013.25)

RTC_DS3231 rtc; // Creation d'un objet rtc
Adafruit_BMP280 bmp; // Creation d'un objet bmp
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire); 

unsigned long delayTime;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday",
                             "Thursday", "Friday", "Saturday"};

void setup()
{

  Serial.begin(9600);
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3C (for the 128x64)
                                             // init done

  if (!bmp.begin(0x76))
  {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                     "try a different address!"));
    while (1)
      delay(10);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_FORCED,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

#ifndef ESP8266
  while (!Serial)
    ; // wait for serial port to connect. Needed for native USB
#endif

  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1)
      delay(10);
  }

  if (rtc.lostPower())
  {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop()
{
 
// Code module rtc
  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour()+1, DEC);
  Serial.print(':');
  Serial.print(now.minute()+5, DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  display.setCursor(0, 0);
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setTextSize(1.9);
  display.setCursor(0, 1);
  display.print((String)"Date: ");
   display.print(now.year(), DEC);
  display.print('/');
  display.print(now.month(), DEC);
  display.print('/');
  display.print(now.day(), DEC);
  display.print("");
 
  display.setCursor(0, 16);
  display.print((String)"Heure: ");
  display.print(now.hour()+1, DEC);
  display.print(':');
  display.print(now.minute()+5, DEC);
  display.print(':');
  display.print(now.second(), DEC);
  display.display();

  delay(4000);

   display.setCursor(0, 0);
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setTextSize(1.2);
  display.setCursor(0, 1);
  display.print((String) "Temperature: " + bmp.readTemperature() + " C");

  // Affichage OLED
  display.setTextSize(1);                                                                 // Taille du texte
  display.setCursor(0, 16);                                                               // position du texte sur l'verticalement
  display.print((String) "Altitude: " + (bmp.readAltitude(SEALEVELPRESSURE_HPA)) + " m"); // Texte à afficher

  // Affichage OLED
  display.setTextColor(WHITE);
  display.setTextSize(1);                                                        // Taille du texte
  display.setCursor(0, 40);                                                      // position du texte sur l'ecran verticalement
  display.print((String) "Pressure: " + (bmp.readPressure() / 100.0F) + " hpa"); // Texte à afficher
  display.display();
  delay(5000);

}
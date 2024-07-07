
//#include 
#include "Adafruit_LEDBackpack.h"
#include <Wire.h>
#include <TimeLib.h>
#include <TinyGPSPlus.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
/*
   This sketch shows the GPS speed on the TTGO display.
   It requires the use of Serial2, and assumes that you have a
   38400-baud serial GPS device hooked up on pins 32(rx) and 33(tx).
*/
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
//static const uint32_t GPSBaud = 384000;

// The serial connection to the GPS device
#define RXD2 32
#define TXD2 33
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 23
#define TFT_BL 4
#define I2C_SDA 22
#define I2C_SCL 21
// constructor for data object named tft 
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// The TinyGPSPlus object
TinyGPSPlus gps;
int speed;

int ones;
int tens;
int hundreds;
int roundedSpeed;
int satellites;
long hexChars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9' 
};        // an array of pin numbers to which LEDs are attached
int charCount = 10;
unsigned long delaytime = 5;

#define TIME_HEADER "T"  // Header tag for serial time sync message
#define TIME_REQUEST 7   // ASCII bell character requests a time sync message

time_t prevDisplay = 0;  // when the digital clock was displayed

void displayInfo() {
    // Serial.print(F("Location: "));
    if (gps.location.isValid()) {
 
    float speedFloat = gps.speed.kmph();
    int roundedSpeed = round(speedFloat);
    ones = (roundedSpeed % 10);
    tens = ((roundedSpeed / 10)%10);
    hundreds = ((roundedSpeed / 100)%10);
    
    Serial.print(F(" Sats: "));
    Serial.print(gps.satellites.value());
    Serial.print(F(" Speed: "));
    Serial.print(gps.speed.kmph());
    Serial.print(F(" SpeedFloat: "));
    Serial.print(speedFloat);
    Serial.print(F(" Rounded: "));
    Serial.print(roundedSpeed);
    Serial.print(F(" Hundreds: "));
    Serial.print(hundreds);
    Serial.print(F(" tens: "));
    Serial.print(tens);
    Serial.print(F(" ones: "));
    Serial.print(ones);
    
    Serial.println();
    }
}


void processSyncMessage() {
    unsigned long pctime;
    const unsigned long DEFAULT_TIME = 1357041600;  // Jan 1 2013

/*    if (Serial.find(TIME_HEADER)) {
        pctime = Serial.parseInt();
        if (pctime >= DEFAULT_TIME) {  // check the integer is a valid time (greater than Jan 1 2013)
            setTime(pctime);           // Sync Arduino clock to the time received on the serial port
        }
    }
*/
}

/*
time_t requestSync() {
    Serial.write(TIME_REQUEST);
    return 0;  // the time will be sent later in response to serial mesg
}
*/

void setup()
{
  Wire.begin(I2C_SDA, I2C_SCL);
  alpha4.begin(0x70); 
  Serial.begin(115200);
  Serial2.begin(38400, SERIAL_8N1, RXD2, TXD2);
  pinMode(TFT_BL, OUTPUT);      // TTGO T-Display enable Backlight pin 4
  digitalWrite(TFT_BL, HIGH);   // T-Display turn on Backlight
  tft.init(135, 240);        
  tft.setRotation(0); 
  Serial.println(F("DeviceExample.ino"));
  Serial.println(F("A simple demonstration of TinyGPSPlus with an attached GPS module"));
  Serial.print(F("Testing TinyGPSPlus library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println(F("TFT Initialized"));
  Serial.println();
  // setSyncProvider(requestSync);  // set function to call when sync required
  Serial.println("Waiting for sync message");
}

void loop()
{
  tft.setCursor(0, 5);
  tft.fillScreen(ST77XX_BLACK);
//  testdrawtext((gps.time.hour(), ST77XX_WHITE));
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(5);
  tft.setRotation(10); 
  tft.print(gps.course.deg());
  float speedFloat = gps.speed.kmph();
    int roundedSpeed = round(speedFloat);
   
    ones = (roundedSpeed % 10);
    tens = ((roundedSpeed / 10)%10);
    hundreds = ((roundedSpeed / 100)%10);
    uint16_t time = millis();
//  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  // This sketch displays information every time a new sentence is correctly encoded.
  while (Serial2.available() > 0)
    if (gps.encode(Serial2.read()))
      displayInfo();

 if (millis() > 5000 && gps.charsProcessed() < 10) {
        //lc.setChar(0, 0, '-', false);
        alpha4.clear();
        alpha4.writeDisplay();
        alpha4.writeDigitAscii(0, 'G');
        alpha4.writeDigitAscii(1, 'P');
        alpha4.writeDigitAscii(2, 'S');
        alpha4.writeDigitAscii(3, '?');
        alpha4.writeDisplay();
        Serial.println(F("No GPS detected: check wiring."));
        while (true)
            ;
    }
    if (gps.location.isValid()) {
      //displayInfo;
        alpha4.clear();
        alpha4.writeDisplay();
        // alpha4.writeDigitAscii(0, 'G');
        // alpha4.writeDigitAscii(1, 'P');
        // alpha4.writeDigitAscii(2, 'S');
        // alpha4.writeDigitRaw(3, 0x0C10);
        alpha4.writeDigitAscii(1, hexChars[hundreds]);
        alpha4.writeDigitAscii(2, hexChars[tens]);
        alpha4.writeDigitAscii(3, hexChars[ones]);
        alpha4.writeDisplay();
        delay(50);
    } else {
        alpha4.clear();
        alpha4.writeDisplay();
        alpha4.writeDigitAscii(0, 'G');
        alpha4.writeDigitAscii(1, 'P');
        alpha4.writeDigitAscii(2, 'S');
        alpha4.writeDigitRaw(3, 0x2D00);
        alpha4.writeDisplay();
        delay(5);
    }
/*displayInfo;
  {
  //  tft.println(F(millis()));
    while(true);
  }
    tft.setCursor(0, 5);
  tft.fillScreen(ST77XX_BLACK);
//  testdrawtext((gps.time.hour(), ST77XX_WHITE));
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(17);
  tft.setRotation(1); 
  tft.print(gps.time.minute());
    //if (gps.time.isValid())
  /*
    if (gps.speed.kmph() < 10) tft.print(F("0"));
    tft.print(gps.satellites.value());
    tft.print(F(":"));
    if (gps.time.minute() < 10) tft.print(F("0"));
    tft.print(gps.time.minute());
    tft.print(F(":"));
    if (gps.time.second() < 10) tft.print(F("0"));
    tft.print(gps.time.second());
    tft.print(F("."));
    if (gps.time.centisecond() < 10) tft.print(F("0"));
    tft.print(gps.time.centisecond());
  */
  
  delay(100);

}
/*
void displayInfoOld()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
*/
void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}
// if (gps.time.hour() < 10) Serial.print(F("0"));
//    Serial.print(gps.time.hour());

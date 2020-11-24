/*************************************************** 
  This is an example sketch for our optical Fingerprint sensor

  Designed specifically to work with the Adafruit BMP085 Breakout 
  ----> http://www.adafruit.com/products/751

  These displays use TTL Serial to communicate, 2 pins are required to 
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_Fingerprint.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
#include <SoftwareSerial.h>


#define I2C_ADDRESS 0x3C
// On Leonardo/Micro or others with hardware serial, use those! #0 is green wire, #1 is wh ite
// uncomment this line:
// #define mySerial Serial1

// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// comment these two lines if using hardware serial
SoftwareSerial mySerial(5, 6);
SSD1306AsciiWire oled;

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;

void setup()  
{
  Serial.begin(9600);
  Wire.begin();
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  oled.setFont(Cooper15);
  oled.println();
  oled.setCursor(15,10);
  oled.println("Fingerprint");
  oled.setCursor(35,20);
  oled.println(" Sensor");
  oled.setCursor(30,30);
  oled.println("Detected");
  delay(2000);
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
}

uint8_t readnumber(void) {
  uint8_t num = 0;
  
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void loop()                     // run over and over again
{
  Serial.println("Ready to enroll a fingerprint!");
  oled.setFont(Cooper15);
  oled.clear();
  oled.println();
  oled.setCursor(35,10);
  oled.println("Ready to");
  oled.setCursor(45,20);
  oled.println("Enroll");
  delay(1000);
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
//  id = readnumber();
  id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  }
  Serial.print("Enrolling ID #");
  Serial.println(id);
  
  while (!  getFingerprintEnroll() );
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  oled.setFont(Arial_bold_14);
  oled.clear();
  oled.println();
  oled.setCursor(30, 25);
  oled.println("Waiting for");
  oled.setCursor(30, 35);
  oled.println("Fingerprint");
  
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      oled.clear();
  oled.setFont(Arial_bold_14);
  oled.println();
  oled.setCursor(25,10);
  oled.println("Image taken");
  delay(500);
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  Serial.println("Remove finger");
  oled.clear();
  oled.setFont(Arial_bold_14);
  oled.println();
  oled.setCursor(15,10);
  oled.println("Remove Finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  oled.clear();
  oled.println();
  oled.setCursor(30, 25);
  oled.println("Place same");
  oled.setCursor(30, 35);
  oled.println("finger again");
  oled.setCursor(20, 45);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      oled.clear();
  oled.setFont(Arial_bold_14);
  oled.println();
  oled.setCursor(30,30);
  oled.println("Image taken");
  delay(500);
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
  oled.setFont(Arial_bold_14);
  oled.setCursor(15,10);
  oled.println("Image converted");
  delay(500);
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
  oled.setFont(Arial_bold_14);
  oled.clear();
  oled.println();
  oled.println();
  oled.setCursor(20, 25);
  oled.println("Could not find");
  oled.setCursor(30, 35);
  oled.println("Fingerprint");
  oled.setCursor(35, 45);
  oled.println("features");
  delay(1000);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
  oled.setFont(Arial_bold_14);
  oled.clear();
  oled.println();
  oled.println();
  oled.setCursor(20, 25);
  oled.println("Could not find");
  oled.setCursor(30, 35);
  oled.println("Fingerprint");
  oled.setCursor(35, 45);
  oled.println("features");
  delay(1000);
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  oled.setFont(Arial_bold_14);
  oled.setCursor(15,10);
  oled.println("Prints Matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
  oled.setFont(Arial_bold_14);
  oled.println();
  oled.setCursor(15,10);
  oled.println("Prints doesn't");
  oled.setCursor(35,10);
  oled.println("Match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    delay(1000);
  oled.setFont(Arial_bold_14);
  oled.clear();
  oled.println();
  oled.setCursor(25, 25);
  oled.println("Fingerprint");
  oled.setCursor(20, 25);
  oled.println("Successfully");
  oled.setCursor(40, 35);
  oled.println("Stored");
  delay(2000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
}

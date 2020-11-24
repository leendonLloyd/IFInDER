#include <Wire.h>
#include <Adafruit_Fingerprint.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Sim800l.h>

#define VIBRATION_MOTOR 13
#define RECEIVER "09998929284"
#define I2C_ADDRESS 0x3C
#define WHITE_SWITCH 2
#define RED_SWITCH 3
#define BLUE_SWITCH 4

SoftwareSerial fingerprintSerial(5, 6);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerprintSerial);
SSD1306AsciiWire oled;
SoftwareSerial gpsSerial(9, 8);
TinyGPSPlus gps;
Sim800l SIM800L;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(VIBRATION_MOTOR, OUTPUT);
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  gpsSerial.begin(9600);
  SIM800L.begin();
  oled.setFont(Cooper15);
  oled.println();
  oled.setCursor(15,10);
  oled.println("Initializing");
  oled.setCursor(35,40);
  oled.println("     Device");
  delay(2000);
  oled.clear();
  initializeEmergencySwitches();
  initializeFingerprintSensor();
  delay(2000);
  initializeGPS();
  deviceReady();
}

void loop() {
  byte pressedSwitch = getPressedEmergencySwitch();
  if (pressedSwitch) {
    oled.setFont(Arial_bold_14);
    if (pressedSwitch == WHITE_SWITCH){
      oled.clear();
      oled.println();
      oled.setCursor(35,30);
      oled.println("Accident");
    }
    else if(pressedSwitch == RED_SWITCH){
      oled.clear();
      oled.println();
      oled.setCursor(50,30);
      oled.println("Fire");
    }
    else if(pressedSwitch == BLUE_SWITCH){
      oled.clear();
      oled.println();
      oled.setCursor(45,30);
      oled.println("Crime");
    }
    oled.setCursor(25,40);    
    oled.println("Emergency");
    oled.setCursor(35,50);
    oled.println("Detected!");
    delay(2000);
    oled.clear();
    authenticate();
    vibrate();
    String coordinates = getCoordinates();
    sendCoordinatesToServer(coordinates, pressedSwitch);
    vibrate();
    pressedSwitch = 0;
    deviceReady();
  }
}

void vibrate() {
  digitalWrite(VIBRATION_MOTOR, HIGH);
  delay(150);
  digitalWrite(VIBRATION_MOTOR, LOW);
}

void sendCoordinatesToServer(String coordinates, int pressedSwitch) {
  String message = "|%DATA" + coordinates + "|" + (pressedSwitch - 1) + "|";
  char charMessage[150] = {};
  message.toCharArray(charMessage, 150);
  SIM800L.listen();
  boolean transmissionOk = SIM800L.sendSms(RECEIVER, charMessage);
  oled.clear();
  oled.setFont(Cooper15);
  oled.println();
  oled.println("Transmission");
  oled.setCursor(35,30);
  oled.println("Status: ");
  oled.setCursor(65, 30);
  oled.println(transmissionOk);
  delay(3000);
  oled.clear();
}

String getCoordinates() {
  oled.clear();
  oled.setFont(Arial_bold_14);
  oled.println();
  oled.println("Tracking Location...");
  delay(800);
  bool coordinatesValid = false;
  String coordinates = "";
  gpsSerial.listen();
  while (!coordinatesValid) {
    if (gpsSerial.available()) {
      if (gps.encode(gpsSerial.read())) {
        if (gps.location.isValid()) {
          coordinates += "|";
          coordinates += String(gps.location.lat(), 6);
          coordinates += "|";
          coordinates += String(gps.location.lng(), 6);
          coordinatesValid = true;
        }
        oled.clear();
        oled.println("Coordinates:");
        oled.println();
        oled.print("Lat: ");
        oled.println(gps.location.lat(), 6);
        oled.print("Long: ");
        oled.println(gps.location.lng(), 6);
        delay(3000);
      }
    }
  }
  return coordinates;
}

void authenticate() {
  oled.setFont(Arial_bold_14);
  oled.clear();
  oled.println();
  oled.setCursor(30, 25);
  oled.println("Waiting for");
  oled.setCursor(30, 35);
  oled.println("Fingerprint");
  oled.setCursor(20, 45);
  oled.println("Authentication");  
  bool authenticated = authenticateFinger();
  while (!authenticated) {
    delay(50);
    authenticated = authenticateFinger();
  }
  oled.clear();
  oled.setCursor(15,30);
  oled.println();
  oled.println("Fingerprint Match!");
  oled.setCursor(25,40);
  oled.println("User Verified");
  delay(1000);
  oled.clear();
}

bool authenticateFinger() {
  fingerprintSerial.listen();
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return false;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return false;
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return false;
  return true;
}

byte getPressedEmergencySwitch() {
  byte whiteSwitchState = digitalRead(WHITE_SWITCH);
  Serial.println(whiteSwitchState);
  if (whiteSwitchState == HIGH) return WHITE_SWITCH;

  byte redSwitchState = digitalRead(RED_SWITCH);
  Serial.println(redSwitchState);
  if (redSwitchState == HIGH) return RED_SWITCH;

  byte blueSwitchState = digitalRead(BLUE_SWITCH);
  if (blueSwitchState == HIGH) return BLUE_SWITCH;

  return 0;
}
 
void initializeEmergencySwitches() {
  pinMode(WHITE_SWITCH, INPUT);
  pinMode(RED_SWITCH, INPUT);
  pinMode(BLUE_SWITCH, INPUT);
}

void initializeFingerprintSensor() {
  finger.begin(57600);
  if (finger.verifyPassword()) {
  oled.setFont(Cooper15);
  oled.println();
  oled.setCursor(15,10);
  oled.println("Fingerprint");
  oled.setCursor(35,20);
  oled.println(" Sensor");
  oled.setCursor(30,30);
  oled.println("Detected");
  } else {
  oled.setFont(Cooper15);
  oled.println();
  oled.setCursor(15,10);
  oled.println("Fingerprint");
  oled.setCursor(35,20);
  oled.println("Sensor");
  oled.println("not found");
  delay(1000);
  for(;;);
  }
}

void initializeGPS(){
    oled.clear();
    oled.setFont(Arial_bold_14);
    oled.println();
    oled.setCursor(10,30);
    oled.println(" GPS Detected:");
    delay(800);
    oled.setCursor(10,40);
    oled.println("Finding Satellite");
    delay(1000);
    if (millis() > 10000 && gps.charsProcessed() < 10)
  {
    oled.clear();
    oled.println("No GPS detected");
    while(true);
    delay(1000);
  }
  bool coordinatesValid = false;
  gpsSerial.listen();
  while(!coordinatesValid){
    if (gpsSerial.available()) {
      if (gps.encode(gpsSerial.read())) {
        if (gps.location.isValid()) {
          oled.clear();
          oled.println();
          oled.println("Satellite Locked");
          delay(800);
          oled.println("Connection: Success");
          delay(1000);
          coordinatesValid = true;
        }
      }
    }
  }
  delay(2000);
  oled.clear();
}

void deviceReady(){
  oled.setFont(Cooper19);
  oled.println("    IFInDER");
  oled.println("       Ready"); 
}

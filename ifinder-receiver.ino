#include <SoftwareSerial.h>

#define TX_PIN 2
#define RX_PIN 3

SoftwareSerial gsmSerial(TX_PIN, RX_PIN);

void setup() {
  Serial.begin(9600);
  gsmSerial.begin(9600);
  delay(1000);
  Serial.println("Initializing...");
  gsmSerial.println("AT+CMGF=1");
  delay(1000);
  gsmSerial.println("AT+CNUM");
  delay(1000);
  gsmSerial.println("AT+CMEE=2");
  delay(1000);
  gsmSerial.println("AT+IPR=9600");
  delay(1000);
  Serial.println("Done initializing!");
}

void loop() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    if (data.startsWith("GET_GPS_DATA")) {
      gsmSerial.println("AT+CMGL=\"ALL\"");

      long timeout = millis() + 3000;
      while (timeout > millis()) {
        while (gsmSerial.available()) {
          char data = gsmSerial.read();
          Serial.print(data);
        }
      }
      Serial.println("END");

      while (!Serial.available()) {}
      String deleteAllReadCommand = "AT+CMGDA=DEL READ";
      gsmSerial.println(deleteAllReadCommand);
      delay(1000);
    }
  }
}

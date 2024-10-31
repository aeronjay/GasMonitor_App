#include <SoftwareSerial.h>

SoftwareSerial mySerial(6, 5); // RX, TX pins for Bluetooth module

void setup() {
  // Open serial communications and wait for the port to open
  Serial.begin(9600);
  while (!Serial) {
    // Wait for the serial port to connect (only needed for native USB ports)
  }

  Serial.println("Goodnight moon!");

  // Set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  mySerial.println("Hello, world?");
}

void loop() {
  // Check if there’s data available from the Bluetooth module
  if (mySerial.available()) {
    Serial.write(mySerial.read());  // Read from Bluetooth and write to Serial
  }
  
  // Check if there’s data available from the Serial monitor
  if (Serial.available()) {
    mySerial.write(Serial.read());  // Read from Serial and write to Bluetooth
  }
}

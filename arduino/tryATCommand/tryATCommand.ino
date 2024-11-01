#include <SoftwareSerial.h>
SoftwareSerial SIM900A(5,6); // RX | TX
// Connect the SIM900A TX to Arduino pin 10 RX. 
// Connect the SIM900A RX to Arduino pin 11 TX. 
char c = ' ';
void setup() 
{
  // start th serial communication with the host computer
  Serial.begin(38400);
  while(!Serial);
  Serial.println("Arduino with SIM900A is ready");

  // start communication with the SIM900A in 9600
  SIM900A.begin(38400); 
  Serial.println("SIM900A started at 38400");
  delay(1000);
  Serial.println("Setup Complete! SIM900A is Ready!");
}

void loop()
{

// Keep reading from SIM800 and send to Arduino Serial Monitor
  if (SIM900A.available()){ 
    c = SIM900A.read();
    Serial.write(c);
  }

  // Keep reading from Arduino Serial Monitor and send to SIM900A
  if (Serial.available()){ 
    c = Serial.read();
    SIM900A.write(c); 
  }

}
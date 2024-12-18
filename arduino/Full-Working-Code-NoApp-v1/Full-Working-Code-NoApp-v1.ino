#include <SoftwareSerial.h>
SoftwareSerial SIM900(5,6);

int sensor=12;
int speaker=13;
int gas_value,Gas_alert_val, Gas_shut_val;
int Gas_Leak_Status;
int sms_count=0;
int sensorValue;
int GasSensorPin = A1;
String phone_no = "+639453674856";
String txt_content = "Leak Detected!!!\n location: PUREZA STATION";

int redLED = 6;
int greenLED = 7;

int gasDetectedValue = 250;



void setup()
{

    pinMode(sensor,INPUT);
    SIM900.begin(38400);   
    Serial.begin(38400);
    pinMode(redLED, OUTPUT);
    pinMode(greenLED, OUTPUT);
    delay(500);
    digitalWrite(greenLED, HIGH);
    delay(5000);
}

void loop()
{
    CheckGas();
    CheckShutDown();
}

void CheckGas()
{

    sensorValue = ScanGasLevel();       // read analog input pin 0
    Serial.println(sensorValue, DEC);  // prints the value read
    delay(100); // wait 100ms for next 
    if(sensorValue>gasDetectedValue)
    {

      // digitalWrite(greenLED, LOW);
      // digitalWrite(redLED, HIGH);
      
      SetAlert();
      delay(2000);
      callUp(phone_no);
    }
}

int ScanGasLevel()
{
     sensorValue = analogRead(GasSensorPin); // read analog input pin 0
     delay(100); // wait 100ms for next reading
     return sensorValue; // returns temperature value in degree celsius
}

void SetAlert()
{
    tone(13,1000,5000);
    
    while(sms_count < 1) //Number of SMS Alerts to be sent
    {  
        SendTextMessage(txt_content); // Function to send AT Commands to GSM module
    }
    Gas_Leak_Status=1; 
}

void CheckShutDown()
{
    if(Gas_Leak_Status==1)
    {
    
      Gas_shut_val=ScanGasLevel();     //scan the gas level
      if(Gas_shut_val < 100)
	  {
        // digitalWrite(redLED,LOW);
        // digitalWrite(greenLED,HIGH);
        noTone(13);                    //stop the buzzer
        sms_count=0;
        Gas_Leak_Status=0;
      }
    }
}


void SendTextMessage(String txt_content)
{
  SIM900.println("AT+CMGF=1");    //To send SMS in Text Mode
  delay(1000);
  //SIM900.println("ATD+ +639453674856;");
  SIM900.println("AT+CMGS=\"+639453674856\"\r"); // change to the phone number you using 
  delay(1000);
  
  SIM900.println(txt_content);//the content of the message
  delay(1500);
  SIM900.println((char)26); //the stopping character
  delay(1000);
  delay(1000);
  sms_count++;
}
void callUp(String number) {
  SIM900.print("ATD + "); 
  SIM900.print(number); 
  SIM900.println(";"); //Call to the specific number, ends with semi-colon,replace X with mobile number
  delay(20000);       // wait for 20 seconds...
  SIM900.println("ATH"); //hang up
  delay(100);
}

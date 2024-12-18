// Use HardwareSerial for SIM900A communication
HardwareSerial SIM900A(2); // UART1 for SIM900A

// Pin Definitions
const int GAS_SENSOR_PIN = 35; // GPIO35 for Gas Sensor
const int BUZZER_PIN = 23;     // GPIO23 for Buzzer
// const int RED_LED = 18;        // GPIO18 for Red LED
// const int GREEN_LED = 19;      // GPIO19 for Green LED   

// Global Variables
int sensorValue;
bool Gas_Leak_Status = false;
int sms_count = 0;

int gasThreshold = 1000;

String phone_no = "+639453674856";
String txt_content = "Leak Detected!!!\nLocation: PUREZA STATION";

void setup() {
  // Initialize Serial Monitor
  Serial.begin(38400); // Monitor at 38400 baud
  delay(1000);

  // Initialize SIM900A communication
  SIM900A.begin(38400, SERIAL_8N1, 16, 17); // RX=GPIO16, TX=GPIO17 for SIM900A
  delay(1000);

  // Pin Modes
  pinMode(GAS_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  // pinMode(RED_LED, OUTPUT);
  // pinMode(GREEN_LED, OUTPUT);

  // Initial State
  // digitalWrite(GREEN_LED, HIGH);
  // digitalWrite(RED_LED, LOW);
  noTone(BUZZER_PIN);

  Serial.println("System Initialized...");
}

void loop() {
  CheckGas();
  CheckShutDown();
}

void CheckGas() {
  sensorValue = analogRead(GAS_SENSOR_PIN); // Read gas sensor value
  Serial.print("Gas Sensor Value: ");
  Serial.println(sensorValue); // Print value to Serial Monitor

  if (sensorValue > gasThreshold) {
    // Alert State
    // digitalWrite(GREEN_LED, LOW);
    // digitalWrite(RED_LED, HIGH);
    SetAlert();
    delay(2000);
    // callUp(phone_no);
  }
  delay(100);
}

void SetAlert() {
  tone(BUZZER_PIN, 1000, 5000); // Sound buzzer for 5 seconds

  while (sms_count < 1) { // Send SMS alert only once
    SendTextMessage(txt_content);
  }
  Gas_Leak_Status = true;
}

void CheckShutDown() {
  if (Gas_Leak_Status) {
    int Gas_shut_val = analogRead(GAS_SENSOR_PIN);
    if (Gas_shut_val < gasThreshold) {
      // Restore to normal state
      // digitalWrite(RED_LED, LOW);
      // digitalWrite(GREEN_LED, HIGH);
      noTone(BUZZER_PIN);
      sms_count = 0;
      Gas_Leak_Status = false;
      Serial.println("Gas leak resolved.");
    }
  }
}

void SendTextMessage(String txt_content) {
  Serial.println("Sending SMS...");
  SIM900A.println("AT+CMGF=1"); // Set SMS mode to Text
  delay(1000);

  SIM900A.print("AT+CMGS=\"");
  SIM900A.print(phone_no);
  SIM900A.println("\"");
  delay(1000);

  SIM900A.println(txt_content); // SMS content
  delay(500);

  SIM900A.write(26); // End SMS with CTRL+Z
  delay(2000);

  Serial.println("SMS sent successfully!");
  sms_count++;
}

void callUp(String number) {
  Serial.println("Making a call...");
  SIM900A.print("ATD");
  SIM900A.print(number);
  SIM900A.println(";"); // Dial the number
  delay(20000);         // Wait for 20 seconds before hanging up
  SIM900A.println("ATH"); // Hang up
  delay(500);
  Serial.println("Call ended.");
}

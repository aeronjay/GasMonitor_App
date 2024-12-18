HardwareSerial SIM900A(2); // UART2 for SIM900A
const int SIM900A_RX = 16; // RX2
const int SIM900A_TX = 17; // TX2

void setup() {
  Serial.begin(9600); // Debugging via Serial Monitor
  SIM900A.begin(38400, SERIAL_8N1, SIM900A_RX, SIM900A_TX);
  Serial.println("Initializing SIM900A...");

  delay(2000); // Allow time for the SIM900A to initialize

  checkModuleReady();
}

void loop() {
  // Continuously check the signal quality and network status
  checkSignalQuality();
  delay(5000); // Check every 5 seconds
}

void checkModuleReady() {
  // Check basic AT response
  SIM900A.println("AT");
  delay(500);
  printResponse();

  // Check SIM card status
  SIM900A.println("AT+CPIN?");
  delay(500);
  printResponse();

  // Check registration status
  SIM900A.println("AT+CREG?");
  delay(500);
  printResponse();
}

void checkSignalQuality() {
  Serial.println("Checking signal quality...");
  SIM900A.println("AT+CSQ");
  delay(500);
  printResponse();
}

void printResponse() {
  while (SIM900A.available()) {
    Serial.write(SIM900A.read()); // Print SIM900A response to Serial Monitor
  }
}

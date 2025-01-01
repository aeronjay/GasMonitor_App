#include <WiFi.h>
#include <WebServer.h>

//HardwareSerial SIM900A(2);

/* Wi-Fi credentials */
const char* ssid = "ESP32";
const char* password = "12345678";

/* IP configuration */
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);

/* Login credentials */
const char* loginUsername = "admin";
const char* loginPassword = "password";

/* Variables */
uint8_t LED1pin = 2; // Debugging LED
bool LED1status = LOW;
bool isAuthenticated = false; // Authentication status

const int sensorPin = 35; // Analog sensor pin
const int buzzer = 23;
int sensorValue = 0;


const int BUZZER_PIN = 23;
const int GAS_SENSOR_PIN = 35;
bool Gas_Leak_Status = false;
int sms_count = 0;
int gasThreshold = 1000;

String phone_no = "+639453674856";
String txt_content = "Leak Detected!!!\nLocation: PUREZA STATION";

void setup() {
  Serial.begin(115200);
  pinMode(LED1pin, OUTPUT);

  pinMode(GAS_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // SIM900A.begin(38400, SERIAL_8N1, 16, 17);
  
  // Initialize Wi-Fi Access Point
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  // Define routes
  server.on("/", handle_MainPage);
  server.on("/login", handle_LoginPage);
  server.on("/authenticate", handle_Authenticate);
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  server.onNotFound(handle_NotFound);

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  digitalWrite(LED1pin, LED1status ? HIGH : LOW); // Update LED status
  sensorValue = analogRead(sensorPin); // Continuously update sensor value
  Serial.println(sensorValue); // Print sensor value to Serial Monitor
  CheckGas();
  CheckShutDown();
}

// Main page handler
void handle_MainPage() {
  if (!isAuthenticated) {
    server.sendHeader("Location", "/login");
    server.send(303); // Redirect to login page
    return;
  }
  server.send(200, "text/html", SendHTML(LED1status, sensorValue));
}

// Login page handler
void handle_LoginPage() {
  String loginHTML = "<!DOCTYPE html><html>\n";
  loginHTML += "<head><title>Login</title>\n";
  loginHTML += "<style>\n";
  loginHTML += "  body {\n";
  loginHTML += "    font-family: Helvetica, Arial, sans-serif;\n";
  loginHTML += "    margin: 0;\n";
  loginHTML += "    padding: 0;\n";
  loginHTML += "    display: flex;\n";
  loginHTML += "    justify-content: center;\n";
  loginHTML += "    align-items: center;\n";
  loginHTML += "    height: 100vh;\n";
  loginHTML += "    background-color: #eaf6ff;\n";
  loginHTML += "  }\n";
  loginHTML += "  .login-container {\n";
  loginHTML += "    background-color: #3498db;\n";
  loginHTML += "    padding: 20px 40px;\n";
  loginHTML += "    border-radius: 10px;\n";
  loginHTML += "    box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);\n";
  loginHTML += "    text-align: center;\n";
  loginHTML += "    color: white;\n";
  loginHTML += "  }\n";
  loginHTML += "  .login-container h2 {\n";
  loginHTML += "    margin-bottom: 20px;\n";
  loginHTML += "    font-size: 24px;\n";
  loginHTML += "    color: #ffffff;\n";
  loginHTML += "  }\n";
  loginHTML += "  .login-container input[type='text'],\n";
  loginHTML += "  .login-container input[type='password'] {\n";
  loginHTML += "    width: 100%;\n";
  loginHTML += "    padding: 10px;\n";
  loginHTML += "    margin: 10px 0;\n";
  loginHTML += "    border: none;\n";
  loginHTML += "    border-radius: 5px;\n";
  loginHTML += "    font-size: 16px;\n";
  loginHTML += "  }\n";
  loginHTML += "  .login-container input[type='submit'] {\n";
  loginHTML += "    background-color: #2874a6;\n";
  loginHTML += "    color: white;\n";
  loginHTML += "    border: none;\n";
  loginHTML += "    padding: 10px 20px;\n";
  loginHTML += "    font-size: 16px;\n";
  loginHTML += "    border-radius: 5px;\n";
  loginHTML += "    cursor: pointer;\n";
  loginHTML += "  }\n";
  loginHTML += "  .login-container input[type='submit']:hover {\n";
  loginHTML += "    background-color: #1b4f72;\n";
  loginHTML += "  }\n";
  loginHTML += "</style>\n";
  loginHTML += "</head>\n";
  loginHTML += "<body>\n";
  loginHTML += "  <div class='login-container'>\n";
  loginHTML += "    <h2>GAS LEAK DETECTOR</h2>\n";
  loginHTML += "    <form action='/authenticate' method='POST'>\n";
  loginHTML += "      <input type='text' name='username' placeholder='Username' required><br>\n";
  loginHTML += "      <input type='password' name='password' placeholder='Password' required><br>\n";
  loginHTML += "      <input type='submit' value='Login'>\n";
  loginHTML += "    </form>\n";
  loginHTML += "  </div>\n";
  loginHTML += "</body></html>\n";
  server.send(200, "text/html", loginHTML);
}


// Authentication handler
void handle_Authenticate() {
  if (server.hasArg("username") && server.hasArg("password")) {
    String username = server.arg("username");
    String password = server.arg("password");

    if (username == loginUsername && password == loginPassword) {
      isAuthenticated = true;
      server.sendHeader("Location", "/");
      server.send(303); // Redirect to main page
      return;
    }
  }
  server.send(200, "text/html", "<h2>Login Failed</h2><p>Invalid credentials. <a href=\"/login\">Try again</a></p>");
}

// Turn LED on handler
void handle_led1on() {
  if (!isAuthenticated) {
    server.sendHeader("Location", "/login");
    server.send(303);
    return;
  }
  LED1status = HIGH;
  Serial.println("LED1 Status: ON");
  server.send(200, "text/html", SendHTML(LED1status, sensorValue));
}

// Turn LED off handler
void handle_led1off() {
  if (!isAuthenticated) {
    server.sendHeader("Location", "/login");
    server.send(303);
    return;
  }
  LED1status = LOW;
  Serial.println("LED1 Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status, sensorValue));
}

// Not found handler
void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
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
    // SendTextMessage(txt_content);
    sms_count++;
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

// void SendTextMessage(String txt_content) {
//   Serial.println("Sending SMS...");
//   SIM900A.println("AT+CMGF=1"); // Set SMS mode to Text
//   delay(1000);

//   SIM900A.print("AT+CMGS=\"");
//   SIM900A.print(phone_no);
//   SIM900A.println("\"");
//   delay(1000);

//   SIM900A.println(txt_content); // SMS content
//   delay(500);

//   SIM900A.write(26); // End SMS with CTRL+Z
//   delay(2000);

//   Serial.println("SMS sent successfully!");
//   sms_count++;
// }

// Generate HTML content
String SendHTML(bool led1status, int sensorVal) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>ESP32 Sensor Monitor</title>\n";
  ptr += "<style>html { font-family: Helvetica; text-align: center;}\n";
  ptr += "body {margin: 0; padding: 20px;}\n";
  ptr += ".button {display: inline-block; margin: 10px; padding: 10px 20px; color: white; text-decoration: none; border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #3498db;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>ESP32 Sensor Monitor</h1>\n";
  ptr += "<h3>Using Access Point (AP) Mode</h3>\n";
  ptr += "<p>Sensor Value: <span id='sensorValue'>" + String(sensorVal) + "</span></p>\n";
  ptr += "<button class='button button-on' onclick='toggleLED(\"on\")'>Turn LED ON</button>\n";
  ptr += "<button class='button button-off' onclick='toggleLED(\"off\")'>Turn LED OFF</button>\n";
  ptr += "<script>\n";
  ptr += "function updateSensor() {\n";
  ptr += "  fetch('/').then(response => response.text()).then(data => {\n";
  ptr += "    const parser = new DOMParser();\n";
  ptr += "    const doc = parser.parseFromString(data, 'text/html');\n";
  ptr += "    const newValue = doc.getElementById('sensorValue').innerText;\n";
  ptr += "    document.getElementById('sensorValue').innerText = newValue;\n";
  ptr += "  });\n";
  ptr += "}\n";
  ptr += "function toggleLED(action) {\n";
  ptr += "  fetch('/led1' + action).then(() => updateSensor());\n";
  ptr += "}\n";
  ptr += "setInterval(updateSensor, 2000);\n"; // Update every 2 seconds
  ptr += "</script>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}

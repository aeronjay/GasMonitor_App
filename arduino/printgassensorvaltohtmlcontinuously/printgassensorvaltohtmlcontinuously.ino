#include <WiFi.h>
#include <WebServer.h>
#include <TimeLib.h>

HardwareSerial SIM900A(2);

/* Wi-Fi credentials */
const char* ssid = "ESP32";
const char* password = "12345678";


const int SIM900A_RX = 16; // RX2
const int SIM900A_TX = 17; // TX2


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

String phone_no = "+639695192609";
String txt_content = "Leak Detected!!!\nLocation: PUREZA STATION";

struct HistoryRecord {
  String date;
  String time;
  int sensorValue;
};


// Global history records
std::vector<HistoryRecord> historyRecords;


void setup() {
  setTime(10, 52, 0, 1, 12, 2025);
  Serial.begin(115200);
  SIM900A.begin(38400, SERIAL_8N1, SIM900A_RX, SIM900A_TX);
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
  server.on("/sensor-data", handle_SensorData);
  server.on("/settings", handle_SettingsPage);
  server.on("/update-settings", handle_UpdateSettings);
  server.onNotFound(handle_NotFound);

  // Start server
  server.begin();
  Serial.println("HTTP server started");
  initializeSensor();
}

void loop() {
  server.handleClient();
  sensorValue = analogRead(sensorPin); // Continuously update sensor value
  Serial.println(sensorValue); // Print sensor value to Serial Monitor
  CheckGas();
  CheckShutDown();
}

void initializeSensor(){
  while(analogRead(sensorPin) > 900){
    Serial.println("Initializing...");
    Serial.println(analogRead(sensorPin));
  }
  Serial.println("Done Initialization...");
  delay(2000);
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


void handle_SensorData() {
  String jsonResponse = "{";
  jsonResponse += "\"sensorValue\":" + String(sensorValue) + ",";
  jsonResponse += "\"gasLeak\":" + String(Gas_Leak_Status ? "true" : "false");
  jsonResponse += "}";
  server.send(200, "application/json", jsonResponse);
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
    // Record history entry
    HistoryRecord record;
    record.date = getCurrentDate();
    record.time = getCurrentTime();
    record.sensorValue = sensorValue;
    historyRecords.push_back(record);

    // Alert State
    SetAlert();
    delay(2000);
  }
  delay(100);
}

void SetAlert() {
  tone(BUZZER_PIN, 1000, 5000); // Sound buzzer for 5 seconds

  while (sms_count < 1) { // Send SMS alert only once
    SendTextMessage(txt_content);
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

// Main page handler
void handle_MainPage() {
  if (!isAuthenticated) {
    server.sendHeader("Location", "/login");
    server.send(303); // Redirect to login page
    return;
  }

  String mainHTML = "<!DOCTYPE html><html>\n";
  mainHTML += "<head><title>GAS LEAK DETECTOR</title>\n";
  mainHTML += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  mainHTML += "<style>\n";
  mainHTML += "  body {\n";
  mainHTML += "    font-family: Helvetica, Arial, sans-serif;\n";
  mainHTML += "    margin: 0;\n";
  mainHTML += "    padding: 0;\n";
  mainHTML += "    background-color: #eaf6ff;\n";
  mainHTML += "    color: #333;\n";
  mainHTML += "  }\n";
  mainHTML += "  .container {\n";
  mainHTML += "    margin: 10px;\n";
  mainHTML += "    padding: 15px;\n";
  mainHTML += "    background-color: #3498db;\n";
  mainHTML += "    color: white;\n";
  mainHTML += "    border-radius: 10px;\n";
  mainHTML += "    box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);\n";
  mainHTML += "  }\n";
  mainHTML += "  .sensor-value {\n";
  mainHTML += "    font-size: 18px;\n";
  mainHTML += "    margin: 15px 0;\n";
  mainHTML += "    padding: 10px;\n";
  mainHTML += "    border-radius: 5px;\n";
  mainHTML += "  }\n";
  mainHTML += "  .safe { background-color: #2ecc71; color: white; }\n";
  mainHTML += "  .high { background-color: #f1c40f; color: white; }\n";
  mainHTML += "  .danger { background-color: #e74c3c; color: white; }\n";
  mainHTML += "  table {\n";
  mainHTML += "    width: 100%;\n";
  mainHTML += "    margin-top: 15px;\n";
  mainHTML += "    border-collapse: collapse;\n";
  mainHTML += "    background-color: white;\n";
  mainHTML += "    color: #333;\n";
  mainHTML += "    font-size: 14px;\n";
  mainHTML += "  }\n";
  mainHTML += "  table th, table td {\n";
  mainHTML += "    border: 1px solid #ddd;\n";
  mainHTML += "    padding: 6px;\n";
  mainHTML += "    text-align: center;\n";
  mainHTML += "  }\n";
  mainHTML += "  table th {\n";
  mainHTML += "    background-color: #2874a6;\n";
  mainHTML += "    color: white;\n";
  mainHTML += "  }\n";
  mainHTML += "  h1, h2 {\n";
  mainHTML += "    font-size: 20px;\n";
  mainHTML += "  }\n";
  mainHTML += "  @media (max-width: 600px) {\n";
  mainHTML += "    .container { padding: 10px; }\n";
  mainHTML += "    .sensor-value { font-size: 16px; }\n";
  mainHTML += "    h1, h2 { font-size: 18px; }\n";
  mainHTML += "    table { font-size: 12px; }\n";
  mainHTML += "    table th, table td { padding: 4px; }\n";
  mainHTML += "  }\n";
  mainHTML += "</style>\n";
  mainHTML += "</head>\n";
  mainHTML += "<body>\n";
  mainHTML += "  <div class='container'>\n";
  mainHTML += "    <div style='text-align: right;'>\n";
  mainHTML += "      <a href='/settings' style='text-decoration: none; font-size: 16px; color: white; border: 2px solid lightblue;'>Settings</a>\n";
  mainHTML += "    </div>\n";

  mainHTML += "    <h1>GAS LEAK DETECTOR</h1>\n";
  mainHTML += "    <p>Welcome to the GAS LEAK DETECTOR monitoring system.</p>\n";

  String sensorClass = "safe";
  if (sensorValue > 700) {
    sensorClass = "danger";
  } else if (sensorValue > 400) {
    sensorClass = "high";
  }

  mainHTML += "    <div class='sensor-value " + sensorClass + "'>\n";
  mainHTML += "      Current Sensor Value: <span id='sensorValue'>" + String(sensorValue) + "</span>\n";
  mainHTML += "    </div>\n";

  mainHTML += "    <h2>History</h2>\n";
  mainHTML += "    <table>\n";
  mainHTML += "      <tr>\n";
  mainHTML += "        <th>Date</th>\n";
  mainHTML += "        <th>Time</th>\n";
  mainHTML += "        <th>Recorded Sensor Value</th>\n";
  mainHTML += "      </tr>\n";

  for (const auto& record : historyRecords) {
    mainHTML += "      <tr>\n";
    mainHTML += "        <td>" + record.date + "</td>\n";
    mainHTML += "        <td>" + record.time + "</td>\n";
    mainHTML += "        <td>" + String(record.sensorValue) + "</td>\n";
    mainHTML += "      </tr>\n";
  }

  mainHTML += "    </table>\n";
  mainHTML += "  </div>\n";
  mainHTML += "  <script>\n";
  mainHTML += "    function fetchSensorData() {\n";
  mainHTML += "      fetch('/')\n";
  mainHTML += "        .then(response => response.text())\n";
  mainHTML += "        .then(html => {\n";
  mainHTML += "          const parser = new DOMParser();\n";
  mainHTML += "          const doc = parser.parseFromString(html, 'text/html');\n";
  mainHTML += "          const newValue = doc.getElementById('sensorValue').innerText;\n";
  mainHTML += "          document.getElementById('sensorValue').innerText = newValue;\n";
  mainHTML += "        });\n";
  mainHTML += "    }\n";
  mainHTML += "    setInterval(fetchSensorData, 2000); // Refresh every 2 seconds\n";
  mainHTML += "  </script>\n";
  mainHTML += "</body></html>\n";
  server.send(200, "text/html", mainHTML);
}
void handle_SettingsPage() {
  if (!isAuthenticated) {
    server.sendHeader("Location", "/login");
    server.send(303); // Redirect to login page
    return;
  }

  String settingsHTML = "<!DOCTYPE html><html>\n";
  settingsHTML += "<head><title>Settings</title>\n";
  settingsHTML += "<style>\n";
  settingsHTML += "  body {\n";
  settingsHTML += "    font-family: Helvetica, Arial, sans-serif;\n";
  settingsHTML += "    margin: 0;\n";
  settingsHTML += "    padding: 0;\n";
  settingsHTML += "    display: flex;\n";
  settingsHTML += "    justify-content: center;\n";
  settingsHTML += "    align-items: center;\n";
  settingsHTML += "    height: 100vh;\n";
  settingsHTML += "    background-color: #eaf6ff;\n";
  settingsHTML += "  }\n";
  settingsHTML += "  .settings-container {\n";
  settingsHTML += "    background-color: #3498db;\n";
  settingsHTML += "    padding: 20px 40px;\n";
  settingsHTML += "    border-radius: 10px;\n";
  settingsHTML += "    box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);\n";
  settingsHTML += "    color: white;\n";
  settingsHTML += "  }\n";
  settingsHTML += "  input[type='text'], input[type='password'], input[type='number'] {\n";
  settingsHTML += "    width: 100%;\n";
  settingsHTML += "    padding: 10px;\n";
  settingsHTML += "    margin: 10px 0;\n";
  settingsHTML += "    border: none;\n";
  settingsHTML += "    border-radius: 5px;\n";
  settingsHTML += "    font-size: 16px;\n";
  settingsHTML += "  }\n";
  settingsHTML += "  input[type='submit'], .back-button {\n";
  settingsHTML += "    background-color: #2874a6;\n";
  settingsHTML += "    color: white;\n";
  settingsHTML += "    border: none;\n";
  settingsHTML += "    padding: 10px 20px;\n";
  settingsHTML += "    font-size: 16px;\n";
  settingsHTML += "    border-radius: 5px;\n";
  settingsHTML += "    cursor: pointer;\n";
  settingsHTML += "  }\n";
  settingsHTML += "  .back-button {\n";
  settingsHTML += "    margin-top: 10px;\n";
  settingsHTML += "    background-color: #1b4f72;\n";
  settingsHTML += "  }\n";
  settingsHTML += "</style>\n";
  settingsHTML += "</head>\n";
  settingsHTML += "<body>\n";
  settingsHTML += "  <div class='settings-container'>\n";
  settingsHTML += "    <h2>Settings</h2>\n";
  settingsHTML += "    <form action='/update-settings' method='POST'>\n";
  settingsHTML += "      <label for='username'>New Username:</label>\n";
  settingsHTML += "      <input type='text' id='username' name='username' placeholder='Enter new username' required><br>\n";
  settingsHTML += "      <label for='password'>New Password:</label>\n";
  settingsHTML += "      <input type='password' id='password' name='password' placeholder='Enter new password' required><br>\n";
  settingsHTML += "      <label for='threshold'>Gas Threshold:</label>\n";
  settingsHTML += "      <input type='number' id='threshold' name='threshold' placeholder='Enter threshold value' value='" + String(gasThreshold) + "' required><br>\n";
  settingsHTML += "      <input type='submit' value='Save Changes'>\n";
  settingsHTML += "    </form>\n";
  settingsHTML += "    <a href='/' class='back-button'>Back to Main Page</a>\n";
  settingsHTML += "  </div>\n";
  settingsHTML += "</body></html>\n";

  server.send(200, "text/html", settingsHTML);
}
void handle_UpdateSettings() {
  if (server.hasArg("username") && server.hasArg("password") && server.hasArg("threshold")) {
    String newUsername = server.arg("username");
    String newPassword = server.arg("password");
    int newThreshold = server.arg("threshold").toInt();

    // Update credentials and threshold
    loginUsername = newUsername.c_str();
    loginPassword = newPassword.c_str();
    gasThreshold = newThreshold;

    server.send(200, "text/html", "<h2>Settings Updated!</h2><a href='/'>Back to Main Page</a>");
    return;
  }
  server.send(400, "text/html", "<h2>Invalid Input!</h2><a href='/settings'>Back to Settings</a>");
}


String getCurrentDate() {
  // Format date as DD/MM/YYYY
  String date = String(day()) + "/" + String(month()) + "/" + String(year());
  return date;
}

String getCurrentTime() {
  // Format time as HH:MM:SS
  String time = String(hour()) + ":" + String(minute()) + ":" + String(second());
  return time;
}
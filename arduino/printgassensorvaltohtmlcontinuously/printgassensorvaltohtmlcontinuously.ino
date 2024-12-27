#include <WiFi.h>
#include <WebServer.h>

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
int sensorValue = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED1pin, OUTPUT);

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
  loginHTML += "<head><title>Login</title></head>\n";
  loginHTML += "<body><h2>Login</h2>\n";
  loginHTML += "<form action=\"/authenticate\" method=\"POST\">\n";
  loginHTML += "Username: <input type=\"text\" name=\"username\"><br>\n";
  loginHTML += "Password: <input type=\"password\" name=\"password\"><br>\n";
  loginHTML += "<input type=\"submit\" value=\"Login\">\n";
  loginHTML += "</form></body></html>\n";
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

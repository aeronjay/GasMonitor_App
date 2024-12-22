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

uint8_t LED1pin = 2; // Debugging LED
bool LED1status = LOW;

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
  server.on("/", handle_OnConnect);
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

// Main webpage handler
void handle_OnConnect() {
  server.send(200, "text/html", SendHTML(LED1status, sensorValue));
}

// Turn LED on handler
void handle_led1on() {
  LED1status = HIGH;
  Serial.println("LED1 Status: ON");
  server.send(200, "text/html", SendHTML(LED1status, sensorValue));
}

// Turn LED off handler
void handle_led1off() {
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

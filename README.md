# 🔥 Gas Monitor App - Smart Gas Leak Detection System

An intelligent gas leak detection system built with ESP32 and MQ6 sensor that provides real-time monitoring, SMS/call alerts, and a comprehensive web interface for gas safety management.

![System Overview](pics%20&%20vid/system.png)

## 🌟 Features

### 🛡️ Real-time Gas Detection
- **MQ6 Gas Sensor** for accurate LPG/natural gas detection
- **Configurable threshold levels** for customized sensitivity
- **Visual indicators** with LED status lights (red/green)
- **Audio alerts** with built-in buzzer

### 📱 Smart Communication
- **Automatic SMS alerts** when gas leaks are detected
- **Emergency calling capability** for critical situations
- **SIM900A GSM module** integration for reliable communication
- **Customizable phone numbers** for emergency contacts

### 🌐 Web-based Dashboard
- **Real-time sensor monitoring** with live updates
- **User authentication** system for secure access
- **Historical data tracking** and logging
- **Mobile-responsive interface** for any device
- **AP Mode configuration** - Creates its own WiFi hotspot

### 📊 Data Management
- **Sensor value history** with timestamps
- **Leak detection logs** for analysis
- **Threshold configuration** through web interface
- **System status monitoring**

## 🏗️ System Architecture

![System Architecture](pics%20&%20vid/system2.png)

The system consists of:
- **ESP32 microcontroller** as the main processing unit
- **MQ6 gas sensor** for detecting LPG/natural gas concentrations
- **SIM900A GSM module** for SMS and calling functionality
- **Web server** running on ESP32 for remote monitoring
- **LED indicators** and **buzzer** for local alerts

## 🔌 Hardware Components

![Schematic Diagram](pics%20&%20vid/SchematicDiagram.png)

### Required Components:
- ESP32 Development Board
- MQ6 Gas Sensor
- SIM900A GSM Module
- Red and Green LEDs
- Buzzer
- Resistors and connecting wires
- Breadboard or PCB

### Pin Configuration:
- **Gas Sensor**: Pin 35 (Analog)
- **Buzzer**: Pin 23
- **Red LED**: Pin 18
- **Green LED**: Pin 19
- **SIM900A RX**: Pin 16
- **SIM900A TX**: Pin 17
- **Debug LED**: Pin 2

## 🖥️ Web Interface

![Web Application Interface](pics%20&%20vid/websiteapp1.png)

The web interface provides:
- **Dashboard** with real-time sensor readings
- **Login system** for secure access
- **Settings panel** for threshold configuration
- **History logs** of all gas detection events
- **System status** and health monitoring

![Web Application Features](pics%20&%20vid/websiteapp2.png)

## 🚀 Getting Started

### 1. Hardware Setup
1. Connect the MQ6 gas sensor to ESP32 pin 35
2. Wire the SIM900A module to pins 16 (RX) and 17 (TX)
3. Connect LEDs and buzzer according to the schematic
4. Power the system using appropriate voltage levels

### 2. Software Installation
1. Install Arduino IDE with ESP32 board support
2. Install required libraries:
   ```
   - WiFi
   - WebServer
   - TimeLib
   - HardwareSerial
   ```
3. Upload the main code from `arduino/FULL CODE HERE! (GAS LEAK WITH THE SERVER)/`

### 3. Configuration
1. Power on the ESP32 - it will create a WiFi hotspot named "ESP32"
2. Connect to the hotspot using password: "12345678"
3. Navigate to `192.168.1.1` in your web browser
4. Login with:
   - **Username**: admin
   - **Password**: password
5. Configure your phone number and gas threshold settings

### 4. SIM Card Setup
1. Insert an active SIM card into the SIM900A module
2. Ensure the SIM card has SMS and calling capabilities
3. Test the GSM connection before deployment

## 📱 Usage

### Normal Operation
- **Green LED**: System is operational, gas levels normal
- **Red LED**: Gas leak detected, alert mode activated
- **Buzzer**: Audible alarm during gas detection
- **Web Dashboard**: Monitor readings remotely via any web browser

### Emergency Response
1. When gas is detected above threshold:
   - Buzzer sounds immediately
   - Red LED activates
   - SMS sent to configured phone number
   - Emergency call initiated (if configured)
   - Event logged with timestamp

### Remote Monitoring
- Access the web interface from any device
- View real-time sensor values
- Check historical data
- Modify threshold settings
- Monitor system health

## 🔧 Code Structure

```
arduino/
├── FULL CODE HERE! (GAS LEAK WITH THE SERVER)/
│   └── printgassensorvaltohtmlcontinuously.ino  # Main application
├── esp32website/
│   ├── main.html                                # Web dashboard
│   ├── login.html                               # Authentication page
│   └── app.html                                 # Mobile app interface
├── fullcodeESP32/
│   └── fullcodeESP32.ino                       # Complete ESP32 code
└── SimpleWiFiServer/
    └── SimpleWiFiServer.ino                     # Basic server setup
```

## 🎯 Key Functionalities

### Gas Detection Algorithm
- Continuous monitoring of MQ6 sensor values
- Configurable threshold-based detection
- Debouncing to prevent false alarms
- Calibration support for different environments

### Communication System
- AT command interface with SIM900A
- SMS formatting and delivery
- Emergency calling with retry mechanism
- Network status monitoring

### Web Server Features
- RESTful API for sensor data
- Session-based authentication
- Real-time data streaming
- Mobile-responsive design
- AJAX-based updates

## 🛠️ Troubleshooting

### Common Issues:
1. **No SMS received**: Check SIM card balance and network coverage
2. **Web interface not accessible**: Verify WiFi connection to ESP32 hotspot
3. **False alarms**: Adjust gas threshold in settings
4. **Sensor not responding**: Check wiring and power connections

## 📈 Future Enhancements

- [ ] Mobile app development (Flutter)
- [ ] Cloud data logging
- [ ] Multiple sensor support
- [ ] Machine learning for pattern detection
- [ ] Integration with smart home systems
- [ ] Battery backup system
- [ ] Remote firmware updates

## 🎥 Demo Videos

Check out the demonstration videos in the `pics & vid/DEMO VID/` folder:
- `Application.mp4` - Web application walkthrough
- `DEMO PHYSICAL HARDWARE HCI.mp4` - Hardware demonstration
- `System.mp4` - Complete system operation

## 👥 Contributors

This project was developed as part of an HCI (Human-Computer Interaction) course, demonstrating the integration of embedded systems with user-friendly interfaces for safety applications.

## 📄 License

This project is open-source and available for educational and research purposes.

---

⚠️ **Safety Notice**: This system is designed for educational purposes. For commercial or critical safety applications, ensure proper calibration, testing, and compliance with local safety regulations.

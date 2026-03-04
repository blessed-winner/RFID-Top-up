# TOP_UP System - MQTT-based RFID Card Management

A comprehensive IoT solution for managing RFID card balances in real-time using MQTT communication protocol. The system consists of an ESP8266 device that reads RFID cards and a Flask backend server with a premium web dashboard for balance management and top-ups.

## System Architecture

```
┌──────────────────────┐
│   RFID Card Reader   │
│    (ESP8266 + MFRC522)
└──────────────────────┘
           │
           │ MQTT Publish/Subscribe
           │
┌──────────────────────┐
│   MQTT Broker        │  (157.173.101.159:1883)
└──────────────────────┘
           │
           │ MQTT Publish/Subscribe
           │
┌──────────────────────┐
│  Flask Backend       │
│  (Port 3000)         │
└──────────────────────┘
           │
           │ WebSocket
           │
┌──────────────────────┐
│  Web Dashboard       │
│  (Premium UI)        │
└──────────────────────┘
```

## Features

- **RFID Card Reading**: Automatically detects and reads RFID card UIDs
- **Multi-Card Ledger System**: Maintains individual balance for each card using a map-based ledger
- **Real-time Balance Updates**: WebSocket integration for instant UI updates
- **Top-up Management**: HTTP POST endpoint to add credit to cards
- **MQTT Communication**: Team-based topic organization for multi-team support
- **Elegant Dashboard**: Premium web interface with real-time card status display
- **Responsive Design**: Works on desktop and mobile devices

## Project Structure

```
TOP_UP/
├── esp8266/                    # ESP8266 firmware and backend
│   ├── Readme.MD              # This file
│   ├── backend/
│   │   ├── main.py            # Flask + SocketIO server
│   │   ├── requirements.txt   # Python dependencies
│   │   └── public/
│   │       └── index.html     # Web dashboard (premium UI)
│   └── ...other files
└── top_up/
    └── top_up.ino             # ESP8266 Arduino sketch
```

## Hardware Requirements

- **ESP8266**: WiFi-enabled microcontroller
- **MFRC522**: RFID card reader module
- **RFID Cards**: 13.56 MHz compatible cards

## Software Requirements

### Backend
- Python 3.7+
- Flask
- Flask-SocketIO
- Paho MQTT
- Python SocketIO

### ESP8266
- Arduino IDE with ESP8266 board support
- Libraries:
  - `ESP8266WiFi`
  - `PubSubClient`
  - `MFRC522`
  - `ArduinoJson`

## Installation & Setup

### 1. ESP8266 Setup (Arduino)

1. Open `top_up/top_up.ino` in Arduino IDE
2. Install required libraries via Library Manager:
   - Search for `PubSubClient` (install by Nick O'Leary)
   - Search for `MFRC522` (install by GithubCommunity)
   - Search for `ArduinoJson` (install by Benoit Blanchon)
3. Configure WiFi and MQTT settings:
   ```cpp
   const char* ssid = "Your_SSID";
   const char* password = "Your_Password";
   const char* mqtt_server = "Your_MQTT_Broker_IP";
   const char* team_id = "Your_Team_Name";
   ```
4. Verify your RFID reader pins (SS_PIN=5, RST_PIN=4)
5. Upload to your ESP8266 device

### 2. Backend Setup (Python)

1. Navigate to the backend directory:
   ```bash
   cd esp8266/backend
   ```

2. Install Python dependencies:
   ```bash
   pip install -r requirements.txt
   ```

3. Configure the backend (in `main.py`):
   ```python
   TEAM_ID = "Your_Team_Name"  # Must match ESP8266 team_id
   ```

4. Start the server:
   ```bash
   python main.py
   ```

5. Access the dashboard at `http://localhost:3000`

## Configuration

### MQTT Topic Structure

Topics follow the pattern: `rfid/{TEAM_ID}/card/{action}`

- **Status**: `rfid/Zephyr/card/status` - Published when a card is read
- **Top-up**: `rfid/Zephyr/card/topup` - Subscribe for top-up requests
- **Balance**: `rfid/Zephyr/card/balance` - Published after balance update

### Important Configuration Notes

⚠️ **Team ID Consistency**: The `TEAM_ID` in both the ESP8266 code and the Flask backend must match exactly, or MQTT communication will fail.

## API Endpoints

### POST `/topup`
Sends a top-up request to the ESP8266 device.

**Request Body:**
```json
{
  "uid": "A1B2C3D4",
  "amount": 100
}
```

**Response:**
```json
{
  "success": true
}
```

## Dashboard Features

The web dashboard (`index.html`) provides:

- **Real-time Card Display**: Shows UID and current balance when cards are read
- **Top-up Interface**: Input fields for card UID and amount
- **Balance Management**: Visual representation of card balances
- **Live Updates**: WebSocket-powered real-time updates without page refresh
- **Premium UI**: Dark theme with elegant typography and smooth animations

## MQTT Message Format

### Status Message (ESP8266 → Backend)
```json
{
  "uid": "A1B2C3D4",
  "balance": 500
}
```

### Top-up Message (Backend → ESP8266)
```json
{
  "uid": "A1B2C3D4",
  "amount": 100
}
```

### Balance Response (ESP8266 → Backend)
```json
{
  "uid": "A1B2C3D4",
  "new_balance": 600
}
```

## Troubleshooting

**ESP8266 won't connect to WiFi:**
- Verify WiFi credentials in `top_up.ino`
- Ensure ESP8266 is in range of the WiFi network
- Check serial monitor for connection status

**MQTT Connection Failed:**
- Confirm MQTT broker is running and accessible
- Verify MQTT broker IP address in both ESP8266 and backend code
- Check firewall rules for port 1883

**Dashboard not updating:**
- Verify backend is running (`python main.py`)
- Check browser console for WebSocket errors
- Confirm TEAM_ID matches between ESP8266 and backend
- Ensure MQTT broker connectivity

**RFID Cards not detected:**
- Verify MFRC522 wiring (SS and RST pins)
- Ensure card is compatible (13.56 MHz)
- Check serial monitor for RFID reader initialization messages

## Deployment Notes

- Update MQTT broker IP address before deployment
- Configure appropriate WiFi credentials for your network
- Ensure MQTT broker has sufficient access controls in production
- Consider using unique TEAM_IDs for multiple installations
- Backend should be deployed on a stable server/machine for production use

## Deployment URL
  http://157.173.101.159:8224

## License

This project is part of the TOP_UP IoT system.

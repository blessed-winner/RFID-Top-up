#include <ESP8266WiFi.h>
#include <PubSubClient.h> 
#include <SPI.h>
#include <MFRC522.h>     
#include <ArduinoJson.h> 
#include <map> // STEP 1: Include map for multi-card support

// --- Configuration ---
const char* ssid = "EdNet";
const char* password = "Huawei@123";
const char* mqtt_server = "157.173.101.159";
const char* team_id = "Zephyr";

String topic_status = "rfid/Zephyr/card/status";
String topic_topup  = "rfid/Zephyr/card/topup";
String topic_balance = "rfid/Zephyr/card/balance";

// --- Global State ---
// This creates a "Ledger" where each UID string gets its own integer balance
std::map<String, int> card_ledger; 

#define SS_PIN 5  
#define RST_PIN 4 
MFRC522 mfrc522(SS_PIN, RST_PIN); 

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload, length);

  String uid = doc["uid"].as<String>();
  int topup_amount = doc["amount"];

  // STEP 2: Update ONLY the balance for this specific UID
  card_ledger[uid] += topup_amount;

  Serial.printf("[DEBUG] Top-up for %s. New Bal: %d\n", uid.c_str(), card_ledger[uid]);

  // Notify backend/dashboard
  StaticJsonDocument<200> response;
  response["uid"] = uid;
  // Match the key name "new_balance" used in your elegant UI
  response["new_balance"] = card_ledger[uid]; 
    
  char buffer[256];
  serializeJson(response, buffer);
  client.publish(topic_balance.c_str(), buffer);
}

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect(team_id)) {
      client.subscribe(topic_topup.c_str());
    } else { delay(5000); }
  }
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop(); 

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      uid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      uid += String(mfrc522.uid.uidByte[i], HEX);
    }
    uid.toUpperCase();

    // STEP 3: Retrieve the specific balance for THIS card
    int my_balance = card_ledger[uid]; 

    StaticJsonDocument<200> doc;
    doc["uid"] = uid;
    doc["balance"] = my_balance;
    
    char buffer[256];
    serializeJson(doc, buffer);
    client.publish(topic_status.c_str(), buffer);
    
    mfrc522.PICC_HaltA(); 
    mfrc522.PCD_StopCrypto1();
  }
}
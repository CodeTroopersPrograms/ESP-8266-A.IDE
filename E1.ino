// Cod pentru ESP8266 E1 (Transmițător și Receptor)
#include <ESP8266WiFi.h>
#include <espnow.h>

const int buttonPin = D3;
const int ledPin = D2;
bool buttonState = false;

uint8_t peerMAC[] = {0x84, 0xF3, 0xEB, 0x62, 0x92, 0x62};  // MAC-ul E2

void onReceive(uint8_t *mac, uint8_t *data, uint8_t len) {
  if (data[0] == 1) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}

void sendCallback(uint8_t *mac, uint8_t status) {
  Serial.println(status == 0 ? "Sent successfully" : "Failed to send");
}

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_add_peer(peerMAC, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_register_recv_cb(onReceive);
  esp_now_register_send_cb(sendCallback);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  uint8_t data = buttonState;
  esp_now_send(peerMAC, &data, sizeof(data));
  delay(100);
}
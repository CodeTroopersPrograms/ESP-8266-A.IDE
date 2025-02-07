#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>

// OLED dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Keypad setup
const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {D1, D2, D3, D4}; // Row pins
byte colPins[COLS] = {D5, D6, D7};     // Column pins
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int buzzerPin = D8; // Buzzer pin
uint8_t peerMAC[] = {0x40, 0x91, 0x51, 0x45, 0x65, 0xCB}; // MAC-ul E2

void onReceive(uint8_t *mac, uint8_t *data, uint8_t len) {
  char receivedKey = (char)data[0];
  display.clearDisplay();

  switch (receivedKey) {
    case '1':
      display.drawCircle(64, 32, 20, SSD1306_WHITE);
      tone(buzzerPin, 500, 300); // 500 Hz, 300 ms
      break;
    case '2':
      display.fillRect(48, 16, 32, 32, SSD1306_WHITE);
      tone(buzzerPin, 700, 300); // 700 Hz, 300 ms
      break;
    case '3':
      display.drawTriangle(32, 48, 96, 48, 64, 16, SSD1306_WHITE);
      tone(buzzerPin, 900, 300); // 900 Hz, 300 ms
      break;
    default:
      display.setCursor(0, 0);
      display.println("Unknown key");
      tone(buzzerPin, 300, 300); // 300 Hz, 300 ms
      break;
  }
  display.display();
}

void sendCallback(uint8_t *mac, uint8_t status) {
  Serial.println(status == 0 ? "Sent successfully" : "Failed to send");
}

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  WiFi.mode(WIFI_STA);

  // Initialize OLED
  if (!display.begin(SSD1306_I2C_ADDRESS, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("E1 Ready"));
  display.display();

  // Initialize ESP-NOW
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
  char key = keypad.getKey();
  if (key) {
    Serial.println(key);
    uint8_t data = key;
    esp_now_send(peerMAC, &data, sizeof(data));
  }
}

#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>

// OLED dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Keypad setup
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {23, 19, 18, 5};
byte colPins[COLS] = {17, 16, 4};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int buzzerPin = 15;
uint8_t peerMAC[] = {0x14, 0x2B, 0x2F, 0xC3, 0xC0, 0xE0}; // MAC-ul celuilalt dispozitiv

// Callback pentru primirea mesajelor
void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  char receivedKey = (char)data[0];
  display.clearDisplay();

  switch (receivedKey) {
    case '1':
      display.drawCircle(64, 32, 20, SSD1306_WHITE);
      tone(buzzerPin, 500, 300);
      break;
    case '2':
      display.fillRect(48, 16, 32, 32, SSD1306_WHITE);
      tone(buzzerPin, 600, 300);
      break;
    case '3':
      display.drawTriangle(32, 48, 96, 48, 64, 16, SSD1306_WHITE);
      tone(buzzerPin, 700, 300);
      break;
    case '4':
      display.drawLine(10, 10, 118, 54, SSD1306_WHITE);
      tone(buzzerPin, 800, 300);
      break;
    case '5':
      display.fillCircle(64, 32, 10, SSD1306_WHITE);
      tone(buzzerPin, 900, 300);
      break;
    case '6':
      display.fillRect(30, 20, 70, 40, SSD1306_WHITE);
      tone(buzzerPin, 1000, 300);
      break;
    case '7':
      display.drawRect(20, 10, 90, 50, SSD1306_WHITE);
      tone(buzzerPin, 1100, 300);
      break;
    case '8':
      display.fillTriangle(20, 50, 108, 50, 64, 10, SSD1306_WHITE);
      tone(buzzerPin, 1200, 300);
      break;
    case '9':
      display.drawCircle(64, 32, 30, SSD1306_WHITE);
      tone(buzzerPin, 1300, 300);
      break;
    case '0':
      display.setCursor(30, 30);
      display.println("Zero!");
      tone(buzzerPin, 1400, 300);
      break;
    case '*':
      display.clearDisplay();
      display.setCursor(20, 30);
      display.println("Clear");
      tone(buzzerPin, 1500, 300);
      break;
    case '#':
      display.setCursor(10, 30);
      display.println("Hello ESP32!");
      tone(buzzerPin, 1600, 300);
      break;
    default:
      display.setCursor(0, 0);
      display.println("Unknown key");
      tone(buzzerPin, 300, 300);
      break;
  }
  display.display();
}

// Callback pentru confirmarea trimiterii mesajelor
void sendCallback(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sent successfully" : "Failed to send");
}

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  WiFi.mode(WIFI_STA);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 Init Failed");
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("E1 Ready"));
  display.display();

  // Inițializare ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  // Înregistrăm callback-urile
  esp_now_register_recv_cb(onReceive);
  esp_now_register_send_cb(sendCallback);

  // Adăugare peer (celălalt ESP32)
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, peerMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
  }
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    Serial.println(key);
    uint8_t data = key;
    esp_now_send(peerMAC, &data, sizeof(data));
  }
}

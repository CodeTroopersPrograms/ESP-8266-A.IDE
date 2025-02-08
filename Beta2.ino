  #include <ESP8266WiFi.h>
  #include <espnow.h>
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
  #include <Keypad.h>

  // OLED dimensions
  #define SCREEN_WIDTH 128
  #define SCREEN_HEIGHT 64

  #define OLED_SDA D2 
  #define OLED_SCL D1 
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

  // Keypad setup
  const byte ROWS = 4; 
  const byte COLS = 3; 
  char keys[ROWS][COLS] = {
    {'0', '1', '2'},
    {'3', '4', '5'},
    {'6', '7', '8'},
    {'9', 'A', 'B'}
  };
  byte rowPins[ROWS] = {D0, D3, D4, D5}; 
  byte colPins[COLS] = {D6, D7, D8};  
  Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

  uint8_t peerMAC[] = {0x84, 0xF3, 0xEB, 0x62, 0x92, 0xEC}; 

  void onReceive(uint8_t *mac, uint8_t *data, uint8_t len) {
    char receivedKey = (char)data[0];
    display.clearDisplay();
    display.setCursor(0, 20);  // Centrat aproximativ vertical
    display.setTextSize(2);    // Mărește textul pentru vizibilitate
    display.setTextColor(SSD1306_WHITE);

    switch (receivedKey) {
      case '0':
        display.println("Ajutor!");
        break;
      case '1':
        display.println("Gasit");
        break;
      case '2':
        display.println("Iesire");
        display.println("la suprafata");
        break;
      case '3':
        display.println("Pozitie");
        display.println("exacta");
        break;
      case '4':
        display.println("Status");
        break;
      default:
        display.println("Comanda necunoscuta");
        break;
      case '5':

        break;
      case '6':

        break;
      case '7':

        break;
      case '8':

        break;
      case '9':

        break;
      case 'A':

        break;
      case 'B':

        break;
      default:
        display.setCursor(0, 0);
        display.println("Unknown key");
        break;
    }
    display.display();
  }

  void sendCallback(uint8_t *mac, uint8_t status) {
    Serial.print("Send status: ");
    Serial.println(status == 0 ? "Success" : "Failed");
  }

  bool verificare(char key) {
    if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5') {
      return true;  
    } else {
      return false; 
    }
  }

  void setup() {
    Serial.begin(9600);
    WiFi.mode(WIFI_STA);

    if (!display.begin(SSD1306_I2C_ADDRESS, 0x3C)) {
      Serial.println(F("SSD1306 allocation failed"));
      for (;;);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("E2 Ready"));
    display.display();

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
    static char lastKey = '\0';
    static bool ok = false;  

    char key = keypad.getKey();

    if (key) {
      if (!ok) {
        lastKey = key; 
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Key Pressed: ");
        display.println(key);
        display.println("Press # to confirm, * to cancel.");
        display.display();
        ok = true;
      } else {
        if (verificare(key)) { 
            Serial.println("Sending: " + String(lastKey));
            uint8_t data = lastKey;
            esp_now_send(peerMAC, &data, sizeof(data));
          ok = false;
        } else if (!verificare(key)) {  
          Serial.println("Sending Cancelled");
          ok = false;
        }
      }
    }
  }

#include "Arduino.h"
#include "TFT_eSPI.h"
#include "pin_config.h"


bool button_ok_state = false;
bool button_cancel_state = false;
bool button_prev_state = false;
bool button_next_state = false;

TFT_eSPI tft = TFT_eSPI();


int sek10 = 0;

void setup() {
    pinMode(PIN_POWER_ON, OUTPUT); // to boot with battery...
    digitalWrite(PIN_POWER_ON, HIGH);  // and/or power from 5v rail instead of USB

    Serial.begin(115200);
    Serial.println("Hello world!");

    pinMode(PIN_SOLENOID_ENABLE, OUTPUT);
    pinMode(PIN_BUTTON_OK, INPUT);
    pinMode(PIN_BUTTON_CANCEL, INPUT);
    pinMode(PIN_BUTTON_PREV, INPUT);
    pinMode(PIN_BUTTON_NEXT, INPUT);

    digitalWrite(PIN_SOLENOID_ENABLE, LOW);

    tft.begin();

#if defined(LCD_MODULE_CMD_1)
    for (uint8_t i = 0; i < (sizeof(lcd_st7789v) / sizeof(lcd_cmd_t)); i++) {
        tft.writecommand(lcd_st7789v[i].cmd);
        for (int j = 0; j < lcd_st7789v[i].len & 0x7f; j++) {
            tft.writedata(lcd_st7789v[i].data[j]);
        }

        if (lcd_st7789v[i].len & 0x80) {
            delay(120);
        }
    }
#endif

    tft.setRotation(1);
    tft.setSwapBytes(true);

    delay(2000);

    ledcSetup(0, 2000, 8);
    ledcAttachPin(PIN_LCD_BL, 0);
    ledcWrite(0, 255);

    // First we test them with a background colour set
    tft.setTextSize(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
}

void check_press(int pin, bool *state, const char *name) {

  int pinRead = digitalRead(pin);
  if (pinRead && !(*state)) {
    *state = true;
    Serial.print(name);
    Serial.println(" high");
  } else if (!pinRead && (*state)) {
    *state = false;
  }
}

void loop() {
  int gpio13Read = digitalRead(PIN_BUTTON_NEXT);
  if (gpio13Read) {
    digitalWrite(PIN_SOLENOID_ENABLE, HIGH);
    Serial.println("high");
  } else {
    digitalWrite(PIN_SOLENOID_ENABLE, LOW);
  }

  check_press(PIN_BUTTON_OK, &button_ok_state, "ok");
  check_press(PIN_BUTTON_CANCEL, &button_cancel_state, "cancel");
  check_press(PIN_BUTTON_PREV, &button_prev_state, "prev");
  check_press(PIN_BUTTON_NEXT, &button_next_state, "next");

  int temp = (millis() / 1000) + 10;
  if (temp > sek10) {
    Serial.print(".");
    sek10 = temp;
    tft.drawNumber(sek10, 30, 10, 8);
  }
  
    
    tft.drawString("Hello world", 0, 16, 2);
}

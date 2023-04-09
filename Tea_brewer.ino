#include "Arduino.h"
#include "TFT_eSPI.h"
#include "pin_config.h"
#include "bachi_driver.h"

#include "lvgl.h"      /* https://github.com/lvgl/lvgl.git */

bool button_ok_state = false;
bool button_cancel_state = false;
bool button_prev_state = false;
bool button_next_state = false;

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

    Serial.print("1");
    bachi_setup_display();
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
  
  bachi_loop_display();

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
    //tft.drawNumber(sek10, 30, 10, 8);
  }
  
    bachi_loop_display();
    
    //tft.drawString("Hello world", 0, 16, 2);
}

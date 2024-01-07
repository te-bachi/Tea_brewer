
#include "CountdownTimer.h"
#include "pin_config.h"
#include "Tea_brewer.h"

#include <Arduino.h>

CountdownTimer::CountdownTimer()
{
    _timeout = 10;
    _startTime = 0;
    _isRunning = false;
}

void
CountdownTimer::loop()
{
    if (_isRunning) {
        uint64_t currentTime = millis() / 1000; // in seconds
        uint64_t deltaTime = currentTime - _startTime;
        if (deltaTime > _timeout) {
            alarm();
        }
    }
}

void
CountdownTimer::start()
{
    _isRunning = true;
    _startTime = millis() / 1000; // in seconds
}

void
CountdownTimer::stop()
{
    _isRunning = false;
}

int
CountdownTimer::getSeconds()
{

}

int
CountdownTimer::getMinutes()
{

}

void
CountdownTimer::alarm()
{
    // stop timer
    stop();

    //
    Serial.println("SOLENOID high");
    digitalWrite(PIN_SOLENOID_ENABLE, HIGH);

    //
    delay(8000);

    //
    Serial.println("SOLENOID low");
    digitalWrite(PIN_SOLENOID_ENABLE, LOW);
}


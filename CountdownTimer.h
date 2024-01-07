#pragma once

#include <stdint.h>

class CountdownTimer {

    private:
        int         _timeout;       // in seconds
        uint64_t    _startTime;     // in seconds
        uint64_t    _lastTime;      // in seconds
        bool        _isRunning;

    public:
        CountdownTimer();

        void loop();
        void start();
        void stop();
        int getSeconds();
        int getMinutes();
        void alarm();
};

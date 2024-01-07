#include "PushButton.h"
//#include "Menu.h"
#include "pin_config.h"
#include "Tea_brewer.h"

#include <Arduino.h>

#include "CountdownTimer.h"

/*******************************************************************************
 * PushButtonHandler
 */

class PushButtonHandlerDefault : public PushButtonHandler {
    public:
        PushButtonHandlerDefault()
        {

        }

        virtual ~PushButtonHandlerDefault()
        {

        }

        void handle(int pin, PushButtonEvent event)
        {
            //
        }
};

static PushButtonHandlerDefault defaultHandler;


/*******************************************************************************
 * PushButton
 */

const int PushButton::DEBOUNCE = 100;

PushButton::PushButton(int pin) :
m_pin(pin), m_lastCheck(0), m_handlerType(HANDLER_CLASS), m_handler(defaultHandler),
m_handlerFunc(nullptr), m_lastEvent(PUSH_BUTTON_RELEASE)
{
        pinMode(pin, INPUT);
}

PushButton::PushButton(int pin, PushButtonHandler& handler) :
m_pin(pin), m_lastCheck(0), m_handlerType(HANDLER_CLASS), m_handler(handler),
m_handlerFunc(nullptr), m_lastEvent(PUSH_BUTTON_RELEASE)
{
    pinMode(pin, INPUT);
}

PushButton::PushButton(int pin, PushButtonHandlerFunc handler) :
m_pin(pin), m_lastCheck(0), m_handlerType(HANDLER_FUNC), m_handler(defaultHandler),
m_handlerFunc(handler), m_lastEvent(PUSH_BUTTON_RELEASE)
{
    pinMode(pin, INPUT);
}

PushButton::~PushButton()
{
    //
}

int
PushButton::getPin()
{
    return m_pin;
}

PushButtonEvent
PushButton::checkPin()
{
    uint32_t now = millis();

    /* */
    if (now - m_lastCheck > DEBOUNCE) {
        m_lastCheck = now;
        int read = digitalRead(m_pin);
        if (read == HIGH) {
            if (m_lastEvent == PUSH_BUTTON_RELEASE || m_lastEvent == PUSH_BUTTON_RELEASE_EDGE) {
                m_lastEvent = PUSH_BUTTON_PRESS_EDGE;
            } else {
                m_lastEvent = PUSH_BUTTON_PRESS;
            }
        } else {
            if (m_lastEvent == PUSH_BUTTON_PRESS || m_lastEvent == PUSH_BUTTON_PRESS_EDGE) {
                m_lastEvent = PUSH_BUTTON_RELEASE_EDGE;
            } else {
                m_lastEvent = PUSH_BUTTON_RELEASE;
            }
        }
    /* */
    } else {
        if (m_lastEvent == PUSH_BUTTON_PRESS_EDGE) {
            m_lastEvent = PUSH_BUTTON_PRESS;
        } else if (m_lastEvent == PUSH_BUTTON_RELEASE_EDGE) {
            m_lastEvent = PUSH_BUTTON_RELEASE;
        }
    }

    if (m_handlerType == HANDLER_CLASS) {
        m_handler.handle(m_pin, m_lastEvent);
    } else if (m_handlerType == HANDLER_FUNC && m_handlerFunc != nullptr) {
        m_handlerFunc(m_pin, m_lastEvent);
    }

    return m_lastEvent;
}

/*******************************************************************************
 * PrevButton
 */

PrevButton::PrevButton() :
PushButton(PIN_BUTTON_PREV, *this)
{
    //
}

void
PrevButton::handle(int pin, PushButtonEvent event)
{
    switch (event) {
      case PUSH_BUTTON_PRESS_EDGE:
        Serial.println("prev");
        break;

      default:
        break;
    }
}

/*******************************************************************************
 * NextButton
 */

NextButton::NextButton() :
PushButton(PIN_BUTTON_NEXT, *this)
{
    //
}

void
NextButton::handle(int pin, PushButtonEvent event)
{

    switch (event) {
      case PUSH_BUTTON_PRESS_EDGE:
        Serial.println("next");
        break;

      default:
        break;
    }
}

/*******************************************************************************
 * CancelButton
 */

CancelButton::CancelButton() :
PushButton(PIN_BUTTON_CANCEL, *this)
{
    //
}

void
CancelButton::handle(int pin, PushButtonEvent event)
{
    switch (event) {
      case PUSH_BUTTON_PRESS_EDGE:
        Serial.println("cancel");
        countdownTimer.stop();
        break;

      default:
        break;
    }
}


/*******************************************************************************
 * OkButton
 */

OkButton::OkButton() :
PushButton(PIN_BUTTON_OK, *this)
{
    //
}

void
OkButton::handle(int pin, PushButtonEvent event)
{
    switch (event) {
      case PUSH_BUTTON_PRESS_EDGE:
        Serial.println("ok");
        countdownTimer.start();
        break;

      default:
        break;
    }
}

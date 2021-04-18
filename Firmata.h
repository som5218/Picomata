#ifndef Firmata_h
#define Firmata_h
#define TOTAL_PINS 29
#define TOTAL_PORTS 4
#define OUTPUT 1
#define INPUT 0
#define FIRMATA_MAJOR_VERSION 2
#define FIRMATA_MINOR_VERSION 5 

#include "Serial.h"

class FirmataClass : public Serial
{
public:

    //se crean los tipos de variable
    typedef void (*callbackFunction)(uint8_t, int);
    typedef void (*systemCallbackFunction)(void);
    typedef void (*stringCallbackFunction)(char *);
    typedef void (*sysexCallbackFunction)(uint8_t command, uint8_t argc, uint8_t *argv);

    FirmataClass();//checked

    void begin();//checked
    void begin(long);//checked

    void processInput(void);//checked
    void setFirmwareNameAndVersion(const char *name, uint8_t major, uint8_t minor);

    void reportAnalog(uint8_t pin, bool enable);//checked
    void reportDigital(uint8_t port, bool enable);//checked

    void attach(uint8_t command, callbackFunction newFunction);//checked
    void attach(uint8_t command, systemCallbackFunction newFunction);//checked
    void attach(uint8_t command, stringCallbackFunction newFunction);//checked
    void attach(uint8_t command, sysexCallbackFunction newFunction);//checked
    void detach(uint8_t command);//checked

    uint8_t getPinMode(uint8_t pin);//checked
    void setPinMode(uint8_t pin, uint8_t config);//checked

    int getPinState(uint8_t pin);//checked
    void setPinState(uint8_t pin, int state);//checked

    //C3P0 firmata
    void startSysex(void);//checked
    void digitalMessage(uint8_t port, uint16_t data);//checked
    void setPinMode(uint8_t command, uint8_t number, bool mode);//checked
    void setDigitalPinValue(uint8_t command, uint8_t number, bool value);//checked
    void endSysex(void);//checked
    void systemReset(void);//checked


private:
    callbackFunction currentDigitalCallback, currentReportDigitalCallback, currentPinModeCallback, currentPinValueCallback;
    sysexCallbackFunction currentSysexCallback;
    int waitForData;
    int pinState[TOTAL_PINS];
    uint8_t pinConfig[TOTAL_PINS];
    uint8_t firmwareVersionCount;

};

extern FirmataClass Firmata;

#endif //firmata_h
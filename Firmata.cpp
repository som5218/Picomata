#include "Firmata.h"


FirmataClass Firmata;//se crea un objeto clase firmata

FirmataClass::FirmataClass()
{
  currentDigitalCallback = NULL;
  currentSysexCallback = NULL;
  waitForData = 0;
  firmwareVersionCount = 0;
}

void FirmataClass::setFirmwareNameAndVersion(const char *name, uint8_t major, uint8_t minor)
{
  const char *firmwareName;
  const char *extension;

  // parse out ".cpp" and "applet/" that comes from using __FILE__
  extension = strstr(name, ".cpp");
  firmwareName = strrchr(name, '/');

  if (!firmwareName) {
    // windows
    firmwareName = strrchr(name, '\\');
  }
  if (!firmwareName) {
    // user passed firmware name
    firmwareName = name;
  } else {
    firmwareName ++;
  }

  if (!extension) {
    firmwareVersionCount = strlen(firmwareName) + 2;
  } else {
    firmwareVersionCount = extension - firmwareName + 2;
  }
}

void FirmataClass::processInput(void)
{
    uint8_t data;
    uint8_t command;
    
    data=read();

      if( waitForData == 0 ){//prcoeso para separar los datos
        if( data < 0xF0 )
          command = data & 0xF0;

        switch(command){
        case DIGITAL_MESSAGE:
        case SET_PIN_MODE:
        case SET_DIGITAL_PIN_VALUE:
          waitForData = 2;
          break;
        case REPORT_DIGITAL:
          waitForData = 1;
          break;
        }
      }
}

void FirmataClass::reportAnalog(uint8_t pin,bool enable)
{
    write(REPORT_ANALOG | (pin & 0x0F)); 
    write(enable);
}

void FirmataClass::reportDigital(uint8_t port,bool enable)
{
    write(REPORT_DIGITAL | (port & 0x0F)); 
    write(enable);
}

void FirmataClass::begin()
{
    begin(57600);
}

void FirmataClass::begin(long speed)
{
    Serial::begin(speed);
}

void FirmataClass::attach(uint8_t command, callbackFunction newFunction)
{
  switch (command) {
    case DIGITAL_MESSAGE:
      currentDigitalCallback = newFunction;
      break;
    case REPORT_DIGITAL:
      currentReportDigitalCallback = newFunction;
      break;
    case SET_PIN_MODE:
      currentPinModeCallback = newFunction;
      break;
    case SET_DIGITAL_PIN_VALUE:
      currentPinValueCallback = newFunction;
      break;
  }
}

void FirmataClass::attach(uint8_t command, sysexCallbackFunction newFunction)
{
  (void)command;
  currentSysexCallback = newFunction;
}

void FirmataClass::detach(uint8_t command)
{
  switch (command) {
    case SYSTEM_RESET:
      attach(command, (systemCallbackFunction)NULL);
      break;
    case STRING_DATA:
      attach(command, (stringCallbackFunction)NULL);
      break;
    case START_SYSEX:
      attach(command, (sysexCallbackFunction)NULL);
      break;
    default:
      attach(command, (callbackFunction)NULL);
      break;
  }
}

uint8_t FirmataClass::getPinMode(uint8_t pin)
{
  return pinConfig[pin];
}

void FirmataClass::setPinMode(uint8_t pin, uint8_t config)
{
  if (pinConfig[pin] == PIN_MODE_IGNORE)
    return;

  pinConfig[pin] = config;
}

int FirmataClass::getPinState(uint8_t pin)
{
  return pinState[pin];
}

void FirmataClass::setPinState(uint8_t pin, int state)
{
  pinState[pin] = state;
}


void FirmataClass::startSysex(void)
{
    write(START_SYSEX);
}

void FirmataClass::digitalMessage(uint8_t port, uint16_t data)
{
  write(DIGITAL_MESSAGE | (port & 0x0F));//se divide entre los bits mas y menos significativos
  write(data& 0x7F);
  write((data>>7) & 0x7F);
}

void FirmataClass::setPinMode(uint8_t command, uint8_t number, bool mode)
{
  write(SET_PIN_MODE);
  write(number);
  write(mode);
}

void FirmataClass::setDigitalPinValue(uint8_t command, uint8_t number, bool value)
{
  write(SET_DIGITAL_PIN_VALUE);
  write(number);
  write(mode);
}

void FirmataClass::endSysex(void)
{
    write(END_SYSEX);
}

void FirmataClass::systemReset(void)
{
    write(SYSTEM_RESET);
}
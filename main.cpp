#include "Firmata.h"


// entradas analogicas
int reporte_entradas_analogicas = 0; // arreglo para guardar el reporte de cada pin
// pines digitales
uint8_t pines_reporte[TOTAL_PORTS];       // 1 para realizar reporte, 0 mantenerse igual
uint8_t pines_previos[TOTAL_PORTS];     // bits enviados previamente
// configurar pines
uint8_t configuracion_puertos_entrada[TOTAL_PORTS]; // para cada bit se establece 1 si es un puerto de entrada, en otro caso es 0

void outputPort(uint8_t portNumber, uint8_t portValue, uint8_t forceSend);
void digitalWriteCallback(uint8_t port, int value);
void reportDigitalCallback(uint8_t port, int value);
bool isDigitalPin(uint8_t pin);//maybe not necesary
void setPinModeCallback(uint8_t pin, int mode);
void setValue_pinCallback(uint8_t pin, int value);
void sysexCallback(uint8_t comando, uint8_t argc, uint8_t *argv);
void checkDigitalInputs(void);


int main(){

  Firmata.setFirmwareNameAndVersion("Andre's RaspberryPico", FIRMATA_MAJOR_VERSION, FIRMATA_MINOR_VERSION);
  Firmata.attach(DIGITAL_MESSAGE, digitalWriteCallback);
  Firmata.attach(REPORT_DIGITAL, reportDigitalCallback);
  Firmata.attach(SET_PIN_MODE, setPinModeCallback);//establecer el modo de un pin
  Firmata.attach(SET_DIGITAL_PIN_VALUE, setValue_pinCallback);//cuanto vale el pin
  Firmata.attach(START_SYSEX, sysexCallback);
  Firmata.begin();//velocidad de funcionamiento de 57600 Baudios

    while (true){
     checkDigitalInputs();
     if (Firmata.available()){
        Firmata.processInput();
     }
    }
    
}

void outputPort(uint8_t portNumber, uint8_t portValue, uint8_t forceSend)
{
  portValue = portValue & configuracion_puertos_entrada[portNumber];//pines no configurados como entrada se ponen en cero
  if (forceSend || pines_previos[portNumber] != portValue) {//enviar nuevo valor cuando cambie el anterior
    Firmata.digitalMessage(portNumber, portValue);//digital message es equivalente a sendDigitalPort
    pines_previos[portNumber] = portValue;
  }
}

void digitalWriteCallback(uint8_t port, int value)//para los pines analogicos se escribe el valuee en el puerto
{
  uint8_t pin, pin_anterior, value_pin, mascara = 1, pinWritemascara = 0;

  if (port < TOTAL_PORTS) {
    pin_anterior = port * 8 + 8;// se crea una mascara para los pines que se pueden modificar
    if (pin_anterior > TOTAL_PINS) pin_anterior = TOTAL_PINS;//evita que se exceda el value del pin
    for (pin = port * 8; pin < pin_anterior; pin++) {
      if (isDigitalPin(pin)) {//created function
        if (Firmata.getPinMode(pin) == OUTPUT || Firmata.getPinMode(pin) == INPUT) {//no modificar pines que tengan otros modos
          value_pin = ((uint8_t)value & mascara) ? 1 : 0;
          if (Firmata.getPinMode(pin) == OUTPUT) {
            pinWritemascara |= mascara;
          }
          Firmata.setPinState(pin, value_pin);
        }
      }
      mascara = mascara << 1;
    }
    Firmata.writePort(port, (uint8_t)value, pinWritemascara);
  }
}

bool isDigitalPin(uint8_t pin)//function that determine if a pin is digital
{
  if((pin > 0 && pin < 30))
  {
    return true;
  } else {
    return false;
  }
}

void reportDigitalCallback(uint8_t port, int value)//lee el valor en el puerto digital
{
  if (port < TOTAL_PORTS) {
    pines_reporte[port] = (uint8_t)value;
    if (value) outputPort(port, Firmata.readPort(port, configuracion_puertos_entrada[port]), true);//establece el puerto como entrada
  }
}

void setPinModeCallback(uint8_t pin, int mode)//establecer el modo de un pin
{
  if (isDigitalPin(pin)) {//si el pin está en el rango de 0 a 7, almacena la configuración en la posición cero del arreglo configuracion_puertos_entrada
    if (mode == INPUT) {
      configuracion_puertos_entrada[pin / 8] |= (1 << (pin & 7));
    } else {
      configuracion_puertos_entrada[pin / 8] &= ~(1 << (pin & 7));
    }
  }
  Firmata.setPinState(pin, 0);
  switch (mode) {
    case INPUT:
      if (isDigitalPin(pin)) {
        gpio_set_dir(pin, INPUT);    // establece el pin como entrada
        Firmata.setPinMode(pin, INPUT);
      }
      break;
    case OUTPUT:
      if (isDigitalPin(pin)) {
        gpio_set_dir(pin, OUTPUT);
        Firmata.setPinMode(pin, OUTPUT);
      }
      break;
  }
}

void setValue_pinCallback(uint8_t pin, int value)//establece el valor en la interfaz para los pines digitales de salida
{
  if (pin < TOTAL_PINS && isDigitalPin(pin)) {
    if (Firmata.getPinMode(pin) == OUTPUT) {
      Firmata.setPinState(pin, value);
      gpio_put(pin, value);//Same that digitalWrite
    }
  }
}

void sysexCallback(uint8_t comando, uint8_t argc, uint8_t *argv)
{

  switch (comando) {

    case CAPABILITY_QUERY:// peticion de los modos soportados y resolucion de los pines
      Firmata.write(START_SYSEX);
      Firmata.write(CAPABILITY_RESPONSE);
      for (uint8_t pin = 0; pin < TOTAL_PINS; pin++) {
        if (isDigitalPin(pin)) {
          Firmata.write((uint8_t)INPUT);
          Firmata.write(1);
          Firmata.write((uint8_t)OUTPUT);
          Firmata.write(1);
        }
        Firmata.write(0x7F);// se usa para identificar si ya se configuró un pin
      }
      Firmata.write(END_SYSEX);
      break;
    case PIN_STATE_QUERY:// pregunta por el modo y estado actual del pin
      if (argc > 0) {
        uint8_t pin = argv[0];
        Firmata.write(START_SYSEX);
        Firmata.write(PIN_STATE_RESPONSE);
        Firmata.write(pin);
        if (pin < TOTAL_PINS) {//determina el modo y el estado del pin
          Firmata.write(Firmata.getPinMode(pin));
          Firmata.write((uint8_t)Firmata.getPinState(pin) & 0x7F);
        }
        Firmata.write(END_SYSEX);
      }
      break;
  }
}

void checkDigitalInputs(void){//se revisa si hay entradas digitales con base en los puertos
  for(uint8_t i=0;i<=TOTAL_PINS;i++){//se pone  29 "TOTAL_PINS" como el valor de i?
  if(TOTAL_PORTS > i && pines_reporte[i]) outputPort(i, Firmata.readPort(i, configuracion_puertos_entrada[i]), false);//si esta activado el reporte se establece si es salida o entrada
  }
}
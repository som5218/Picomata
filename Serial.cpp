#include "Serial.h"

Serial::Serial(void)
{

}

void Serial::begin(long speed)
{
    uart_init(uart0, speed);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
}

void Serial::write(uint8_t data)
{
    uart_putc_raw(uart0, data);
}

int Serial::available()
{
    return uart_is_readable(uart0);
}

int Serial::read()
{
    if(available()){
        return uart_getc(uart0);
    } else {
        return -1;
    }
}

unsigned char Serial::readPort(uint8_t port, uint8_t bitmask){

  unsigned char out = 0, pin = port * 8;
  if ((pin + 1) && (bitmask & 0x02) && gpio_get((pin + 1))) out |= 0x02;
  if ((pin + 0) && (bitmask & 0x01) && gpio_get((pin + 0))) out |= 0x01;
  if ((pin + 2) && (bitmask & 0x04) && gpio_get((pin + 2))) out |= 0x04;
  if ((pin + 3) && (bitmask & 0x08) && gpio_get((pin + 3))) out |= 0x08;
  if ((pin + 4) && (bitmask & 0x10) && gpio_get((pin + 4))) out |= 0x10;
  if ((pin + 5) && (bitmask & 0x20) && gpio_get((pin + 5))) out |= 0x20;
  if ((pin + 6) && (bitmask & 0x40) && gpio_get((pin + 6))) out |= 0x40;
  if ((pin + 7) && (bitmask & 0x80) && gpio_get((pin + 7))) out |= 0x80;
  return out;

}

unsigned char Serial::writePort(uint8_t port, uint8_t value, uint8_t bitmask){

  uint8_t pin = port * 8;
  if ((bitmask & 0x01)) gpio_put((pin + 0), (value & 0x01));
  if ((bitmask & 0x02)) gpio_put((pin + 1), (value & 0x02));
  if ((bitmask & 0x04)) gpio_put((pin + 2), (value & 0x04));
  if ((bitmask & 0x08)) gpio_put((pin + 3), (value & 0x08));
  if ((bitmask & 0x10)) gpio_put((pin + 4), (value & 0x10));
  if ((bitmask & 0x20)) gpio_put((pin + 5), (value & 0x20));
  if ((bitmask & 0x40)) gpio_put((pin + 6), (value & 0x40));
  if ((bitmask & 0x80)) gpio_put((pin + 7), (value & 0x80));
  return 1;

}
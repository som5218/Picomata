#include "FirmataMarshaller.h"

FirmataMarshaller marshaller;//se crea un objeto marshaller

void FirmataMarshaller::sendDigitalPort(uint8_t portNumber, uint16_t portData)
const
{
  if ( (Stream *)NULL == FirmataStream ) { 
    return; 
  }

  FirmataStream->write(DIGITAL_MESSAGE | (portNumber & 0xF));

  encodeByteStream(sizeof(portData), reinterpret_cast<uint8_t *>(&portData), sizeof(portData));
}

void FirmataMarshaller::encodeByteStream (size_t bytec, uint8_t * bytev, size_t max_bytes)
const
{
  static const size_t transmit_bits = 7;
  static const uint8_t transmit_mask = ((1 << transmit_bits) - 1);

  size_t bytes_sent = 0;
  size_t outstanding_bits = 0;
  uint8_t outstanding_bit_cache = *bytev;

  if ( !max_bytes ) { max_bytes = static_cast<size_t>(-1); }
  for (size_t i = 0 ; (i < bytec) && (bytes_sent < max_bytes) ; ++i) {
    uint8_t transmit_byte = (outstanding_bit_cache|(bytev[i] << outstanding_bits));
    FirmataStream->write(transmit_mask & transmit_byte);
    ++bytes_sent;
    outstanding_bit_cache = (bytev[i] >> (transmit_bits - outstanding_bits));
    outstanding_bits = (outstanding_bits + (8 - transmit_bits));
    for ( ; (outstanding_bits >= transmit_bits) && (bytes_sent < max_bytes) ; ) {
      transmit_byte = outstanding_bit_cache;
    FirmataStream->write(transmit_mask & transmit_byte);
      ++bytes_sent;
      outstanding_bit_cache >>= transmit_bits;
      outstanding_bits -= transmit_bits;
    }
  }
  if ( outstanding_bits && (bytes_sent < max_bytes) ) {
    FirmataStream->write(static_cast<uint8_t>((1 << outstanding_bits) - 1) & outstanding_bit_cache);
  }
}
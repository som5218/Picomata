#ifndef FirmataMarshaller_h
#define FirmataMarshaller_h
#include "FirmataDefines.h"
#include "FirmataConstants.h"
#include <string.h>
#include <inttypes.h>


namespace firmata {

class FirmataMarshaller
{
    friend class FirmataClass;

    public:
    FirmataMarshaller();

    void sendDigitalPort(uint8_t portNumber, uint16_t portData) const;

};

}

#endif //FirmataMarshaller_h
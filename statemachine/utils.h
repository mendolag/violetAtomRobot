#include "stdint.h"

class CircularBufferInt
{
public:
    CircularBufferInt( uint16_t inputSize );
  ~CircularBufferInt();
    double getElement( uint16_t ); //zero is the push location
    void pushElement( double );
    double averageLast( uint16_t );
    uint16_t recordLength( void );
private:
  uint16_t cBufferSize;
    double *cBufferData;
    int16_t cBufferLastPtr;
    uint8_t cBufferElementsUsed;
};

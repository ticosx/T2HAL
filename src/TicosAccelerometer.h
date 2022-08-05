#ifndef __TICOS_ACCELEROMETER_H
#define __TICOS_ACCELEROMETER_H

#include <TicosDevice.h>

class TicosAccelerometer: public TicosDevice {
    public:
        virtual uint8_t getAccel(float* x, float* y, float* z);
        virtual uint8_t getAccel(uint16_t* xbuf, uint16_t* ybuf, uint16_t* zbuf, uint8_t buflen);
};

#endif // __TICOS_GSENSOR_H

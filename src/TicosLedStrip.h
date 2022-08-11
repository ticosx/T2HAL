#ifndef __TICOS_LED_STRIP_H
#define __TICOS_LED_STRIP_H

#include <TicosDevice.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} TicosPixel888;

class TicosLedStrip: public TicosDevice {
    public:
        virtual void fill(uint8_t r, uint8_t g, uint8_t b);
        virtual void fill(TicosPixel888* pixel, uint16_t count=0, bool tail=false);
        virtual void pattern(TicosPixel888* pixels, uint16_t len);
};

#endif // __TICOS_LED_STRIP_H

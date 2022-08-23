#ifndef __TICOS_LED_STRIP_H
#define __TICOS_LED_STRIP_H

#include <TicosDevice.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} TicosRGB888;

typedef struct {
    uint8_t h;
    uint8_t s;
    uint8_t v;
} TicosHSV888;

class TicosLedStrip: public TicosDevice {
    public:
        void hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b);
        virtual void fill(uint8_t r, uint8_t g, uint8_t b);
        virtual void fill(TicosRGB888* pixel, uint16_t count=0, bool tail=false);
        virtual void pattern(TicosRGB888* pixels, uint16_t len);
};

#endif // __TICOS_LED_STRIP_H

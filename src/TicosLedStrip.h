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

typedef struct {
    uint8_t mode ;
    uint8_t bright ;
    uint8_t grade ;
    uint8_t type ;
    uint32_t delaytime ;
    TicosRGB888 color ;
}t_ledconfig;

class TicosLedStrip: public TicosDevice {
    public:
        TicosLedStrip(uint8_t nums): m_nums(nums) {}
        uint8_t nums(void) { return m_nums; }
        void hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b);
        virtual void fill(uint8_t r, uint8_t g, uint8_t b);
        virtual void fill(TicosRGB888* pixel, uint16_t count=0, bool tail=false);
        virtual void pattern(TicosRGB888* pixels, uint16_t len);
        virtual void setLedConfig(t_ledconfig config);
    private:
        uint8_t m_nums;
};

#endif // __TICOS_LED_STRIP_H

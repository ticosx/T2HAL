#ifndef __TICOS_LED_STRIP_H
#define __TICOS_LED_STRIP_H

#include <TicosDevice.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} TicosRGB888;
#ifdef __cplusplus
}
#endif

class TicosLedStrip: public TicosDevice {
    public:
        TicosLedStrip(uint16_t nums): m_nums(nums) {}
        uint16_t nums(void) { return m_nums; }
        virtual void fill(uint8_t r, uint8_t g, uint8_t b);
        virtual void fill(TicosRGB888* pixel, uint16_t count=0, bool tail=false);
        virtual void pattern(TicosRGB888* pixels, uint16_t len);
    private:
        uint16_t m_nums;
};

#endif // __TICOS_LED_STRIP_H

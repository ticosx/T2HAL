#ifndef __WS2812_STRIP_H
#define __WS2812_STRIP_H

#include <TicosLedStrip.h>
#include "led_strip_ws2812.h"

class WS2812RMT: public TicosLedStrip {
    public:
        WS2812RMT(uint8_t pin, uint8_t chn, uint16_t n) {
            m_pin = pin;
            m_chn = chn;
            m_nums = n;
        }
        bool open(void) override;
        bool close(void) override;
        void clear(void);
        void fill(uint8_t r, uint8_t g, uint8_t b) override;
        void fill(TicosPixel888* pixel, uint16_t count=0, bool tail=false) override;
        void pattern(TicosPixel888* pixels, uint16_t len) override;
    private:
        void send(uint16_t i, TicosPixel888* pixel);
        void refresh(void);
        uint8_t     m_pin;
        uint8_t     m_chn;
        uint16_t    m_nums;
        led_ws2812_strip_t* m_strip;
};

#endif // __WS2812_STRIP_H

#include <WS2812RMT.h>
#include <Log.h>

#include <driver/gpio.h>
#include <driver/rmt.h>

bool WS2812RMT::open(void) {
    if (!m_strip) {
        m_strip = led_strip_init(m_chn, m_pin, m_nums);
    }
    return m_strip != NULL;
}

bool WS2812RMT::close(void) {
    if (m_strip && ESP_OK == led_strip_deinit(m_strip)) {
        m_strip = NULL;
    }
    return m_strip == NULL;
}

void WS2812RMT::send(uint16_t i, TicosRGB888* pixel) {
    // Neopixel wants colors in green then red then blue order
    m_strip->set_pixel(m_strip, i, pixel->r, pixel->g, pixel->b);
    // Wait long enough without sending any bots to cause the pixels to latch
    // delayMicroseconds(50);
}

void WS2812RMT::refresh(void) {
    // Wait long enough without sending any bots to cause the pixels to latch
    m_strip->refresh(m_strip, 100);
}

void WS2812RMT::clear(void) {
    m_strip->clear(m_strip, 50);
}

void WS2812RMT::fill(uint8_t r, uint8_t g, uint8_t b) {
    TicosRGB888 pixel = { r, g, b };
    fill(&pixel);
}

void WS2812RMT::fill(TicosRGB888* pixel, uint16_t count, bool tail) {
    TicosRGB888 black = { 0, 0, 0 };
    if (count == 0 || count > m_nums) {
        count = m_nums;
    }
    // If tail is true
    // Skip pixels by filling them with black color
    uint16_t skip = tail ? m_nums - count : 0;
    cli();
    for (uint16_t i = 0; i < skip && i < m_nums; i++) {
        send(i, &black);
    }
    // Fill pixels
    for (uint16_t i = skip; i < (skip + count); i++) {
        send(i, pixel);
    }
    refresh();
    sei();
}

void WS2812RMT::pattern(TicosRGB888* pixels, uint16_t len) {
    uint16_t i= 0;
    TicosRGB888 black = { 0, 0, 0 };
    if (len > m_nums) {
        len = m_nums;
    }
    cli();
    for (; i < len; ++i) {
        send(i, pixels + i);
    }
    for (; i < m_nums; ++i) {
        send(i, &black);
    }
    refresh();
    sei();
}

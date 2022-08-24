#include <WS2812RMT.h>
#include <Log.h>

#include <driver/gpio.h>
#include <driver/rmt.h>

bool WS2812RMT::open(void) {
    if (!m_strip) {
        m_strip = led_strip_init(m_chn, m_pin, nums());
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
    const uint8_t n = nums();
    if (count == 0 || count > n) {
        count = n;
    }
    uint16_t curr, end, step, skip;
    if (tail) {
        curr = n - 1;
        skip = n - 1 - count;
        end = -1;
        step = -1;
    } else {
        curr = 0;
        skip = count;
        end = n;
        step = 1;
    }
    cli();
    for (; curr != skip; curr+=step) {
        send(curr, pixel);
    }
    // Fill pixels
    for (; curr != end; curr+=step) {
        send(curr, &black);
    }
    refresh();
    sei();
}

void WS2812RMT::pattern(TicosRGB888* pixels, uint16_t len) {
    uint16_t i = 0;
    const uint8_t n = nums();
    TicosRGB888 black = { 0, 0, 0 };
    if (len > n) {
        len = n;
    }
    cli();
    for (; i < len; ++i) {
        send(i, pixels + i);
    }
    for (; i < n; ++i) {
        send(i, &black);
    }
    refresh();
    sei();
}

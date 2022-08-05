#ifndef __TICOS_ADC_H
#define __TICOS_ADC_H

#include <driver/adc.h>
#include <driver/gpio.h>
#include <esp_adc_cal.h>

class TicosADC {
    public:
        TicosADC(uint8_t pin, uint8_t chn, uint16_t vref) {
            m_pin = gpio_num_t(pin);
            m_chn = adc1_channel_t(chn);
            m_vref= vref;
        }
        bool open(void);
        bool close(void);
        uint32_t adc(void);
        uint32_t voltage(uint32_t adc);
    private:
        bool            m_inited;
        gpio_num_t      m_pin;
        adc1_channel_t  m_chn;
        uint16_t        m_vref;
        esp_adc_cal_characteristics_t   m_adc_chars;
};

#endif // __TICOS_ADC_H

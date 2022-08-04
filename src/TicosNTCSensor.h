#ifndef __TICOS_NTC_SENSOR_H
#define __TICOS_NTC_SENSOR_H

#include <TicosDevice.h>
#include "driver/adc.h"
#include "driver/gpio.h"
#include "esp_adc_cal.h"

#define TICOS_ADC_NTC_QUEUE_SIZE     10

class TicosNTCSensor: public TicosDevice {
    public:
        TicosNTCSensor(uint8_t pin, uint8_t chn, uint16_t vref) {
            m_pin = gpio_num_t(pin);
            m_chn = adc1_channel_t(chn);
            m_vref= vref;
        }
        bool open(void) override;
        bool close(void) override;
        int16_t getTemp(void);
        uint32_t getVolt(void);
    private:
        bool            m_inited;
        gpio_num_t      m_pin;
        adc1_channel_t  m_chn;
        uint16_t        m_vref;
        uint32_t        m_ntc_queue[TICOS_ADC_NTC_QUEUE_SIZE];
        esp_adc_cal_characteristics_t   m_adc_chars;
};

#endif // __TICOS_NTC_SENSOR_H

#ifndef __TICOS_NTC_SENSOR_H
#define __TICOS_NTC_SENSOR_H

#include <TicosDevice.h>
#include <TicosADC.h>

#define TICOS_ADC_NTC_QUEUE_SIZE     10

class TicosNTCSensor: public TicosDevice {
    public:
        TicosNTCSensor(uint8_t pin, uint8_t chn, uint16_t vref): m_adc(pin, chn, vref) {}
        bool open(void) override;
        bool close(void) override;
        int16_t temperature(void);
        uint32_t voltage(void);
    private:
        bool        m_inited;
        uint32_t    m_ntc_queue[TICOS_ADC_NTC_QUEUE_SIZE];
        TicosADC    m_adc;
};

#endif // __TICOS_NTC_SENSOR_H

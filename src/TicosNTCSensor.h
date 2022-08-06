#ifndef __TICOS_NTC_SENSOR_H
#define __TICOS_NTC_SENSOR_H

#include <TicosDevice.h>
#include <TicosADC.h>

class TicosNTCSensor: public TicosDevice {
    public:
        TicosNTCSensor(uint8_t adcpin, uint8_t chn, uint16_t vref): m_adc(adcpin, chn, vref) {}
        bool open(void) override;
        bool close(void) override;
        uint32_t adc(void) {
            return m_adc.adc();
        }
        uint32_t voltage(uint8_t nex=TICOS_HAL_DEFAULT_NEX) {
            return m_adc.voltage(nex);
        }
        int16_t temperature(uint8_t nex=TICOS_HAL_DEFAULT_NEX);
    private:
        bool        m_inited;
        TicosADC    m_adc;
};

#endif // __TICOS_NTC_SENSOR_H

#include <TicosADC.h>
#include <Log.h>

bool TicosADC::open(void) {
    if (!m_inited) {
        esp_err_t err;
        // 12位分辨率
        adc_bits_width_t bit_width = ADC_WIDTH_BIT_12;
        if (ESP_OK != (err = adc1_config_width(bit_width))) {
            logErr("adc1_config_width failed: %d", err);
            return false;
        }
        // 电压输入衰减
        adc_atten_t atten = ADC_ATTEN_DB_11;
        if (ESP_OK != (err = adc1_config_channel_atten(m_chn, atten))) {
            logErr("adc1_config_channel_atten failed: %d", err);
            return false;
        }
        esp_adc_cal_value_t val_type = esp_adc_cal_characterize(
                ADC_UNIT_1, atten, bit_width, m_vref, &m_adc_chars);
        logDebug("esp_adc_cal_characterize: %d", val_type);
        logDebug("adc: open pin=%d, chn=%d, vref=%d",
                m_pin, m_chn, m_vref);

        m_inited = true;
    }
    return true;
}

bool TicosADC::close(void) {
    if (m_inited) {
        gpio_reset_pin(m_pin);
        logDebug("adc: close");
        m_inited = false;
    }
    return true;
}

uint32_t TicosADC::adc(void) {
    return adc1_get_raw(m_chn);
}

uint32_t TicosADC::voltage(uint32_t adc) {
    return esp_adc_cal_raw_to_voltage(adc, &m_adc_chars);
}

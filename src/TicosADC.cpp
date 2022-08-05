#include <TicosADC.h>
#include <Log.h>

bool TicosADC::open(void) {
    if (!m_inited) {
        esp_err_t err;
        // 12位分辨率，需要后期可通过接口配置
        adc_bits_width_t bit_width = ADC_WIDTH_BIT_12;
        if (ESP_OK != (err = adc1_config_width(bit_width))) {
            logErr("adc1_config_width failed: %d", err);
            return false;
        }
        // 电压输入衰减，需要后期可通过接口配置
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
    if (m_inited) {
        return adc1_get_raw(m_chn);
    }
    return 0;
}

uint32_t TicosADC::voltage(uint8_t nex) {
    uint8_t  i = 0;
    uint32_t ret =0;

    uint32_t adc_max = 0;
    uint32_t adc_min = 0;
    uint32_t adc_sum = 0;
    uint32_t adc_average = 0;
    uint32_t volt_average = 0; // 电压值
    uint8_t  queue_size = nex ? nex : TICOS_DEFAULT_ADC_NEX;
    uint32_t queue[queue_size];

    for(i = 0; i < queue_size; i++) {
        queue[i] = adc(); // 采集ADC原始值
    }  

    adc_max = queue[0];
    adc_min = queue[0];

    for(i = 0; i < queue_size; i++) {
        if(adc_max < queue[i])
            adc_max = queue[i];
        if(adc_min > queue[i])
            adc_min = queue[i];
        adc_sum += queue[i];
    }
    adc_sum = adc_sum - adc_max - adc_min;

    // 平均AD值
    adc_sum += (queue_size - 2) / 2;
    adc_average = adc_sum / (queue_size - 2);

    // 根据电阻分压计算电压
    return esp_adc_cal_raw_to_voltage(adc_average, &m_adc_chars);
}

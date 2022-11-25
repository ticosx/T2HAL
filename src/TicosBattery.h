#ifndef __TICOS_BATTERY
#define __TICOS_BATTERY

#include <TicosDevice.h>
#include <TicosADC.h>

class TicosBattery: public TicosDevice {
    public:
        TicosBattery(uint8_t det, uint8_t adc, uint8_t chn, uint16_t vref): m_det(gpio_num_t(det)), m_adc(adc, chn, vref) {}
        bool open(void) override;
        bool close(void) override;
        /*!
        *    @brief  获取电池电量
        *    @param  nex - 信号采集次数（不填写或填'0'将输入默认次数）
        *    @return 初始化成功则返回电量值，单位是%（0-100%）
        */
        uint8_t capacity(uint8_t nex=TICOS_HAL_DEFAULT_NEX);
        /*!
        *    @brief  获取电池电压值
        *    @param  nex - 信号采集次数（不填写或填'0'将输入默认次数）
        *    @return 初始化成功则返回电压值，单位是mV
        */
        uint32_t voltage(uint8_t nex=TICOS_HAL_DEFAULT_NEX) {
            return m_adc.voltage(nex);
        }
        /*!
        *    @brief  获取电池Adc原始值，12bit
        *    @return 初始化成功则返回原始ADC值
        */
        uint32_t adc(void) {
            return m_adc.adc();
        }
        /*!
        *    @brief  获取充电、放电状态
        *    @return 初始化成功则返回充电状态
        */
        bool charging(void);
    private:
        bool        m_inited;
        gpio_num_t  m_det;
        TicosADC    m_adc;
};

#endif // __TICOS_BATTERY

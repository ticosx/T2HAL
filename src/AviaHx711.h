#ifndef __AVIA_HX711_H
#define __AVIA_HX711_H

#include <TicosWeighingSensor.h>

typedef enum {
    AVIA_HX711_CHN_A_GAIN_128 = 1,
    AVIA_HX711_CHN_B_GAIN_32,
    AVIA_HX711_CHN_A_GAIN_64,
} avia_hx711_gain_t;

class AviaHx711: public TicosWeighingSensor {
    public:
        AviaHx711(uint8_t pw, uint8_t sck, uint8_t dout, uint8_t gain): m_pw(pw), m_sck(sck), m_dout(dout), m_gain(gain) {
        }
        uint32_t weight(uint8_t nex) override;
        uint32_t tare(uint8_t nex) override;
        bool open(void) override;
        bool close(void) override;
    private:
        bool isReady(void);
        void waitReady(uint32_t ms);
        uint32_t read(void);
        uint32_t readAverage(uint8_t nex);
        bool        m_inited;
        uint8_t     m_pw;       // 电源控制
        uint8_t     m_sck;    // 断电与串行时钟输入
        uint8_t     m_dout;     // 串行数据输出
        uint8_t     m_gain;     // 增益倍数
};

#endif // __AVIA_HX711_H

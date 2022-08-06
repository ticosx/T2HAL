#ifndef __TICOS_WEIGHING_SENSOR_H
#define __TICOS_WEIGHING_SENSOR_H

#include <TicosDevice.h>

class TicosWeighingSensor: public TicosDevice {
    public:
        /*!
        *    @brief  获取称重结果
        *    @param  nex - 信号采集次数（不填写或填'0'将输入默认采集次数）
        *    @return 称重结果
        */
        virtual uint32_t weight(uint8_t nex=TICOS_HAL_DEFAULT_NEX);
        /*!
        *    @brief  自动设置皮重，请于空载时调用
        *    @param  nex - 信号采集次数（不填写或填'0'将输入默认采集次数）
        *    @return 皮重
        */
        virtual uint32_t tare(uint8_t nex=TICOS_HAL_DEFAULT_NEX);
        /*!
        *    @brief  设置皮重
        *    @param  offset - 皮重
        *    @return 无
        */
        void setOffset(uint32_t offset) { m_offset = offset; }
        /*!
        *    @brief  获取皮重
        *    @return 皮重
        */
        uint32_t getOffset() { return m_offset; }
    private:
        uint32_t    m_offset;   // 皮重
};

#endif // __TICOS_WEIGHING_SENSOR_H

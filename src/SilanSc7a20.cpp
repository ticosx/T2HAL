#include "SilanSc7a20.h"
#include "Log.h"

#define I2C_MASTER_TX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
#define ACK_CHECK_EN    0x1     /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS   0x0     /*!< I2C master will not check ack from slave */
#define ACK_VAL         0x0         /*!< I2C ack value */
#define NACK_VAL        0x1         /*!< I2C nack value */

/**********sc7a20**********/
#define IIC_ADDR        0x19

#define SL_SC7A20_CHIP_ID_ADDR    (uint8_t)0x0F
#define SL_SC7A20_CHIP_ID_VALUE   (uint8_t)0x11
#define SL_SC7A20_POWER_ODR_REG   (uint8_t)0x20
#define SL_SC7A20_FIFO_CTRL_REG   (uint8_t)0x2E
#define SL_SC7A20_FIFO_SRC_REG    (uint8_t)0x2F
#define SL_SC7A20_SPI_OUT_X_L     (uint8_t)0x27

#define OUT_X_L_REG     (uint8_t)0x28
#define OUT_X_H_REG     (uint8_t)0x29
#define OUT_Y_L_REG     (uint8_t)0x2A
#define OUT_Y_H_REG     (uint8_t)0x2B
#define OUT_Z_L_REG     (uint8_t)0x2C
#define OUT_Z_H_REG     (uint8_t)0x2D

bool SilanSc7a20::open(void) {
    bool res = true;
    if (!m_inited) {
        uint8_t id = 0;
        if ((res = m_i2c.open())) {
            // 配置参数
            res &= m_i2c.write(0x20, 0x47); // 使能xyz轴，正常模式，50Hz
            res &= m_i2c.write(0x23, 0x18); // 连续更新，大端数据，4G量程
            res &= m_i2c.write(0x24, 0xC0); // 使能FIFO模式
            res &= m_i2c.write(0x2E, 0x4F); // FIFO 模式，阈值是F
            // 校验reg值
            //uint8_t data = 0;
            //res &= m_i2c.read(0x20, &data);
            //res &= m_i2c.read(0x23, &data);
            //res &= m_i2c.read(0x24, &data);
            //res &= m_i2c.read(0x2E, &data);
            // 读取gsensor id （sc7a20 ID 是0x11）
            res &= m_i2c.read(SL_SC7A20_CHIP_ID_ADDR, &id);
            logDebug("sc7a20: open, id = %x", id);
            m_inited = res;
        }
    }
    return res;
}

bool SilanSc7a20::close(void) {
    bool res = true;
    if (m_inited) {
        res = m_i2c.close();
        if (res) {
            m_inited = false;
            logDebug("sc7a20: close");
        }
    }
    return res;
}

uint8_t SilanSc7a20::getAccel(uint16_t* xbuf, uint16_t* ybuf, uint16_t* zbuf, uint8_t buflen) {
    uint8_t fifo_nums = 0;
    m_i2c.read(SL_SC7A20_FIFO_SRC_REG, &fifo_nums); // 获取FIFO数量
    fifo_nums &= 0x1F; // 限长
    if (fifo_nums > buflen) {
        fifo_nums = buflen;
    }
    uint8_t buf[6] = {0};
    for (int i = 0; i < fifo_nums; i++) {
        if (m_i2c.read(OUT_X_L_REG, buf, sizeof(buf) / sizeof(buf[0]))) {
            xbuf[i] = (uint16_t) ((buf[1] << 8) | buf[0]);
            ybuf[i] = (uint16_t) ((buf[3] << 8) | buf[2]);
            zbuf[i] = (uint16_t) ((buf[5] << 8) | buf[4]);
            logDebug("sc7a20: accel = %d, %d, %d", xbuf[i], ybuf[i], zbuf[i]);
        } else {
            logErr("sc7a20: accel read error");
        }
    }
    // 重载数据
    m_i2c.write(SL_SC7A20_FIFO_CTRL_REG, 0x00);
    m_i2c.write(SL_SC7A20_FIFO_CTRL_REG, 0x4F);
    return fifo_nums;
}

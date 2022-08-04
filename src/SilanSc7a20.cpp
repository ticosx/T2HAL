#include "SilanSc7a20.h"
#include "Log.h"

/**********sc7a20**********/
#define IIC_ADDR        0x19

#define _CHIP_ID_ADDR    (0x0F)
#define _CHIP_ID_VALUE   (0x11)

#define _CTRL1      (0x20)
#define _CTRL2      (0x21)
#define _CTRL3      (0x22)
#define _CTRL4      (0x23)
#define _CTRL5      (0x24)
#define _CTRL6      (0x25)
#define _REFERENCE  (0x26)
#define _STATUS     (0x27)

#define _OUT_X_L    (0x28)
#define _OUT_X_H    (0x29)
#define _OUT_Y_L    (0x2A)
#define _OUT_Y_H    (0x2B)
#define _OUT_Z_L    (0x2C)
#define _OUT_Z_H    (0x2D)
#define _FIFO_CTRL  (0x2E)
#define _FIFO_SRC   (0x2F)

#define _INT1_CFG   (0x30)
#define _INT1_SRC   (0x31)
#define _INT1_THS   (0x32)
#define _INT1_DUR   (0x33)
#define _INT2_CFG   (0x34)
#define _INT2_SRC   (0x35)
#define _INT2_THS   (0x36)
#define _INT2_DUR   (0x37)

#define _CLICK_CFG      (0x38)
#define _CLICK_SRC      (0x39)
#define _CLICK_THS      (0x3A)
#define _TIME_LIMIT     (0x3B)
#define _TIME_LATENCY   (0x3C)
#define _TIME_WINDOW    (0x3D)
#define _ACT_THS        (0x3E)
#define _ACT_DURATION   (0x3F)

// CTRL1 # SAMPLING FREQUENCY / OPTION DATA RATE
// [ODR3 ODR2 ODR1 ODR0 LPen Zen Yen Xen]
#define _ODR_MASK   (0x07)  // Zen, Yen, Xen set to 1 to enable
#define _LP_MASK    (0x08)  // Low Power
#define _ODR_OFF    (0x00)  // Power Down
#define _ODR_1HZ    (0x10)
#define _ODR_10HZ   (0x20)
#define _ODR_25HZ   (0x30)
#define _ODR_50HZ   (0x40)
#define _ODR_100HZ  (0x50)
#define _ODR_200HZ  (0x60)
#define _ODR_400HZ  (0x70)
#define _ODR_1600HZ (0x88)  // Low power mode (1.6 KHz)
#define _ODR_1250HZ (0x90)  // Normal working mode (1.25 kHz) / Low power mode (5 KHz)

// MECHANICAL CHARACTERISTICS
// SENSITIVITY
// X mg / (2 ** 11) = x mg/level or digit (2**11 --> 12 bit but signed so 11)
#define _SO_2G  (( 2000) / (2 ** 11))  # ~1 mg / digit
#define _SO_4G  (( 4000) / (2 ** 11))  # ~2 mg / digit
#define _SO_8G  (( 8000) / (2 ** 11))  # ~4 mg / digit
#define _SO_16G ((16000) / (2 ** 11))  # ~8 mg / digit

#define _FS_2G  (0x88)
#define _FS_4G  (0x98)
#define _FS_8G  (0xA8)
#define _FS_16G (0xB8)

// ACCEL UNIT
#define _SF_G    0.001  // 1 mg = 0.001 g
#define _SF_SI   0.00980665  // 1 mg = 0.00980665 m/s2
#define _STANDARD_GRAVITY  9.806

enum PowerMode {
    POWER_MODE_NORMAL,
    POWER_MODE_HIGH,
    POWER_MODE_LOW,
};

bool SilanSc7a20::open(void) {
    bool res = true;
    if (!m_inited) {
        uint8_t id = 0;
        if ((res = m_i2c.open())) {
            // 配置参数
            res &= m_i2c.write(_CTRL1, 0x57);
            res &= m_i2c.write(_CTRL2, 0x81);
            res &= m_i2c.write(_CTRL3, 0x40);
            res &= m_i2c.write(_CTRL4, _FS_2G);
            res &= m_i2c.write(_CTRL5, 0x08);
            res &= m_i2c.write(_CTRL6, 0x00);
            //res &= m_i2c.write(0x20, 0x47); // 使能xyz轴，正常模式，50Hz
            //res &= m_i2c.write(0x23, 0x18); // 连续更新，大端数据，4G量程
            //res &= m_i2c.write(0x24, 0xC0); // 使能FIFO模式
            //res &= m_i2c.write(0x2E, 0x4F); // FIFO 模式，阈值是F
            // 校验reg值
            //uint8_t data = 0;
            //res &= m_i2c.read(0x20, &data);
            //res &= m_i2c.read(0x23, &data);
            //res &= m_i2c.read(0x24, &data);
            //res &= m_i2c.read(0x2E, &data);
            // 读取gsensor id （sc7a20 ID 是0x11）
            res &= m_i2c.read(_CHIP_ID_ADDR, &id);
            logDebug("sc7a20: open, id = %x:%x", id, _CHIP_ID_VALUE);
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

uint8_t SilanSc7a20::getGyro(float* x, float* y, float* z) {
    uint8_t buf[6] = {0};
    double factor = _SO_2G * _SF_G;
    if (m_i2c.read(_OUT_X_L, buf, sizeof(buf) / sizeof(buf[0]))) {
        *x = (((uint16_t) ((buf[1] << 8) | buf[0])) >> 4) * factor;
        *y = (((uint16_t) ((buf[3] << 8) | buf[2])) >> 4) * factor;
        *z = (((uint16_t) ((buf[5] << 8) | buf[4])) >> 4) * factor;
        logDebug("sc7a20: gyro = %f, %f, %f", *x, *y, *z);
        return 1;
    }
    logErr("sc7a20: gyro read error");
    return 0;
}

uint8_t SilanSc7a20::getAccel(uint16_t* xbuf, uint16_t* ybuf, uint16_t* zbuf, uint8_t buflen) {
    return 0;
}
/*
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
*/

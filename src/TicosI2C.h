#ifndef __TICOS_I2C_H
#define __TICOS_I2C_H

#include <Adafruit_I2CDevice.h>

class TicosI2C {
    public:
        TicosI2C(uint8_t bus, uint8_t sda, uint8_t scl, uint8_t addr, uint32_t freq);
        ~TicosI2C(void) { delete m_dev; }
        bool open(void);
        bool close(void);
        bool write(uint8_t add, uint8_t data);
        bool read(uint8_t add, uint8_t* p_data, uint8_t len=1);
    private:
        bool        m_inited;
        uint8_t     m_freq;
        Adafruit_I2CDevice* m_dev;
};

#endif // __TICOS_I2C_H

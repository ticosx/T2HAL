#ifndef __TICOS_SC7A20_H
#define __SILAN_SC7A20_H

#include <TicosAccelerometer.h>
#include <TicosI2C.h>

class SilanSc7a20: public TicosAccelerometer {
    public:
        SilanSc7a20(uint8_t bus, uint8_t sda, uint8_t scl, uint8_t addr, uint32_t freq): m_i2c(bus, sda, scl, addr, freq) {}
        uint8_t getAccel(uint16_t* xbuf, uint16_t* ybuf, uint16_t* zbuf, uint8_t buflen) override;
    protected:
        bool open(void) override;
        bool close(void) override;
    private:
        bool m_inited;
        TicosI2C m_i2c;
};

#endif // __SILAN_SC7A20_H

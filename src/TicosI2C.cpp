#include <TicosI2C.h>
#include <Wire.h>

TicosI2C::TicosI2C(uint8_t bus, uint8_t sda, uint8_t scl, uint8_t addr, uint32_t freq) {
    TwoWire* wire = bus ? (&Wire1) : (&Wire);
    wire->setPins(sda, scl);
    m_dev = new Adafruit_I2CDevice(addr, wire);
    m_freq = freq;
}

bool TicosI2C::open(void) {
    if (!m_inited) {
        m_dev->begin();
        m_dev->setSpeed(m_freq);
        m_inited = true;
    }
    return m_inited;
}

bool TicosI2C::close(void) {
    if (m_inited) {
        m_dev->end();
        m_inited = false;
    }
    return true;
}

bool TicosI2C::write(uint8_t addr, uint8_t data) {
    if (m_dev) {
        return m_dev->write(&data, sizeof(data), true, &addr, sizeof(addr));
    }
    return false;
}

bool TicosI2C::read(uint8_t addr, uint8_t* p_data, uint8_t len) {
    if (m_dev) {
        return m_dev->write_then_read(&addr, sizeof(addr), p_data, len, true);
    }
    return false;
}

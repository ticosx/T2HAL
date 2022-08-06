#include <AviaHx711.h>
#include <Log.h>

#define POWER_ON    0
#define POWER_OFF   1

#define SHIFTIN_WITH_SPEED_SUPPORT(data, clock, order) shiftIn((data), (clock), (order))

bool AviaHx711::open(void) {
    bool res = true;
    if (!m_inited) {
        pinMode(m_sck,    OUTPUT);
        pinMode(m_dout,     INPUT_PULLUP);
        pinMode(m_pw,       OUTPUT);
        digitalWrite(m_pw,  POWER_ON);
        res = true;
        logDebug("hx711: open pw=%d, sck=%d, dout=%d, gain=%d", m_pw, m_sck, m_dout, m_gain);
    }
    m_inited = res;
    return res;
}

bool AviaHx711::close(void) {
    bool res = true;
    if (m_inited) {
        digitalWrite(m_pw,  POWER_OFF);
        m_inited = false;
        logDebug("hx711: close");
    }
    return res;
}

bool AviaHx711::isReady(void) {
    return digitalRead(m_dout) == 0;
}

void AviaHx711::waitReady(uint32_t ms) {
	// Wait for the chip to become ready.
	// This is a blocking implementation and will
	// halt the sketch until a load cell is connected.
	while (!isReady()) {
		// Probably will do no harm on AVR but will feed the Watchdog Timer (WDT) on ESP.
		delay(ms);
	}
}

uint32_t AviaHx711::read(void) {
	// Define structures for reading data into.
	uint32_t value = 0;
	uint8_t data[3] = { 0 };
	uint8_t filler = 0x00;
    // Wait for the chip to become ready.
	waitReady(10);
	// Disable interrupts.
	noInterrupts();
	// Pulse the clock pin 24 times to read the data.
	data[2] = SHIFTIN_WITH_SPEED_SUPPORT(m_dout, m_sck, MSBFIRST);
	data[1] = SHIFTIN_WITH_SPEED_SUPPORT(m_dout, m_sck, MSBFIRST);
	data[0] = SHIFTIN_WITH_SPEED_SUPPORT(m_dout, m_sck, MSBFIRST);

	// Set the channel and the gain factor for the next reading using the clock pin.
	for (uint8_t i = 0; i < m_gain; i++) {
		digitalWrite(m_sck, HIGH);
		digitalWrite(m_sck, LOW);	
	}
	// Enable interrupts again.
	interrupts()
	// Replicate the most significant bit to pad out a 32-bit signed integer
	if (data[2] & 0x80) {
		filler = 0xFF;
	} else {
		filler = 0x00;
	}
	// Construct a 32-bit signed integer
	value = ( static_cast<uint32_t>(filler) << 24
			| static_cast<uint32_t>(data[2]) << 16
			| static_cast<uint32_t>(data[1]) << 8
			| static_cast<uint32_t>(data[0]) );

	return static_cast<uint32_t>(value);
}

uint32_t AviaHx711::readAverage(uint8_t nex) {
    uint32_t sum = 0;
    uint8_t times = nex ? nex : TICOS_HAL_DEFAULT_NEX;
	for (uint8_t i = 0; i < nex; i++) {
		sum += read();
		delay(0);
	}
	return sum / nex;
}

uint32_t AviaHx711::weight(uint8_t nex) {
    return readAverage(nex) - getOffset();
}

uint32_t AviaHx711::tare(uint8_t nex) {
    uint32_t sum = readAverage(nex);
	setOffset(sum);
    return sum;
}

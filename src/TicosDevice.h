#ifndef __TICOS_DEVICE_H
#define __TICOS_DEVICE_H

#include <Arduino.h>

class TicosDevice {
    public:
        virtual bool open(void);
        virtual bool close(void);
};

#endif // __TICOS_DEVICE_H

#ifndef __TICOS_RTCTIME_H
#define __TICOS_RTCTIME_H

#include <TicosDevice.h>

class TicosRTCTime: public TicosDevice {
    public:
        TicosRTCTime(){}
        bool open(void) override;
        bool close(void) override;
        virtual bool setRtcTime(struct tm timeconfig);
        virtual bool wifiSetTime(char *NTP1, char *NTP2, char *NTP3);
        virtual bool getRtcTime(char *timebuf);
};

#endif // 

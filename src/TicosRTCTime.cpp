#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#include <TicosRTCTime.h>
#include <Log.h> 

//服务器的地址，可以根据实际情况修改
//#define NTP1 "ntp1.aliyun.com"
//#define NTP2 "ntp2.aliyun.com"
//#define NTP3 "ntp3.aliyun.com"

bool TicosRTCTime::open(void)
{
    return 1;
}

bool TicosRTCTime::close(void)
{
    return 1;
}

bool TicosRTCTime::setRtcTime( struct tm timeconfig)
{
    bool ret = false ;
    time_t false_now = 0;

    false_now = mktime(&timeconfig);
    struct timeval now = { .tv_sec = false_now };
    settimeofday(&now, NULL);
    ret = true;
    return ret ;
}
bool TicosRTCTime::wifiSetTime( char *NTP1, char *NTP2, char *NTP3)
{
   bool ret = false ;
   configTime(8 * 3600, 0, NTP1, NTP2, NTP3);
   ret = true ;
   return ret;
}
bool TicosRTCTime::getRtcTime(char *timebuf)
{
    bool ret = false ;
    time_t now;
    char strftime_buf[64];
    struct tm timeinfo;
    time(&now);
    // Set timezone to China Standard Time
    setenv("TZ", "CST-8", 1);
    tzset();
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    memcpy(timebuf,strftime_buf,strlen(strftime_buf));
    printf("Time: %s\n", strftime_buf);
    ret = true ;
    return ret;
}

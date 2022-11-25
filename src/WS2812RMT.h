#ifndef __WS2812_STRIP_H
#define __WS2812_STRIP_H

#include <TicosLedStrip.h>
#include "led_strip_ws2812.h"
#include "freertos/task.h"

typedef enum {
    SINGLE_COLOR_MODE = 0,  /* */
    WATER_TEMP_MODE,//水温
    WATER_VOL_MODE,//水量
    BATTERY_LEVEL_MODE,//电量
    CHAGER_MODE,//充电
    BLUETOOTH_CONNECTING_MODE,//蓝牙连接中
    BLUETOOTH_CONNECTED_MODE,//蓝牙连接成功
    FIREWARE_UPDATE_MODE,//固件升级
    DRINKWATER_REMIN_MODE,//饮水达标提醒
    DRINKWATER_ALARM_MODE,//饮水闹钟

    THREE_COLOR_MODE,
    ONEBYONE_COLOR_MODE,//
    BREATH_COLOR_MODE,//单色呼吸
    RANBOW_COLOR_MODE,//彩色旋转
    UPS_DOWN_MODE,//单色起伏
    MAX_MODE
} ;


uint8_t  const onebyonevalue[6][18]={ {0,50,0, 50,15,0, 0,50,0,  50,15,0, 0,50,0, 50,10,0}, //第一轮
                                     { 50,10,0, 0,50,0, 50,15,0, 0,50,0, 50,15,0, 0,50,0}, //第二轮
                                     { 0,50,0, 50,10,0, 0,50,0, 50,10,0, 0,50,0, 50,10,0}, //第三轮
                                     { 50,10,0, 0,50,0, 50,15,0, 0,50,0, 50,15,0, 0,50,0}, //第四轮
                                     { 0,50,0, 50,10,0, 0,50,0, 50,10,0, 0,50,0, 50,10,0}, //第五轮
                                     { 50,10,0, 0,50,0, 50,15,0, 0,50,0, 50,15,0, 0,50,0}  //第六轮 
}; 

uint8_t  const alarmvalue[6][18] = { { 0,50,20, 0,40,16, 0,30,12, 0,20,8,  0,10,4,  5,5,5}, //第一轮
                                     { 0,40,16, 0,30,12, 0,20,8,  0,10,4,  5,5,5,0, 50,20},//第二轮
                                     { 0,30,12, 0,20,8,  0,10,4,  5,5,5,   0,50,20, 0,40,16}, //第三轮
                                     { 0,20,8,  0,10,4,  5,5,5,   0,50,20, 0,40,16, 0,30,12}, //第四轮
                                     { 0,10,4,  5,5,5,   0,50,20, 0,40,16, 0,30,12, 0,20,8}, //第五轮
                                     { 5,5,5,   0,50,20, 0,40,16, 0,30,12, 0,20,8,  0,10,4} //第六轮 
}; 

class WS2812RMT: public TicosLedStrip {
    public:
        WS2812RMT(uint8_t pin, uint8_t chn,uint8_t pwd, uint16_t n): TicosLedStrip(n) {
            m_pin = pin;
            m_chn = chn;
            m_pwd = pwd;
        }
        bool open(void) override;
        bool close(void) override;
        void clear(void);
        void fill(uint8_t r, uint8_t g, uint8_t b) override;
        void fill(TicosRGB888* pixel, uint16_t count=0, bool tail=false) override;
        void pattern(TicosRGB888* pixels, uint16_t len) override;
        void setLedConfig(t_ledconfig config)override;
    private:
        void send(uint16_t i, TicosRGB888* pixel);
        void refresh(void);
        uint8_t     m_pin;
        uint8_t     m_chn;
        uint8_t     m_pwd;
        led_ws2812_strip_t* m_strip;

    private:
        void sendPixel(uint32_t j,uint8_t r, uint8_t g, uint8_t b) ;
        void sendRefresh(void);
        void LedRanbow(uint32_t time,uint8_t V);
        void LedUpAndDownColor(TicosRGB888 color,uint32_t time);
        void LedBreathColor(TicosRGB888 color,uint32_t time);
        void LedOneByOne(TicosRGB888 color,uint32_t time);
        void LedThreeColor(t_ledconfig ledinfo);
        void LedOneColor(t_ledconfig ledinfo);

        void LedWaterTemp(t_ledconfig ledinfo);
        void LedWaterVol(t_ledconfig ledinfo);
        void LedBatteryLevel(t_ledconfig ledinfo);
        void LedChargerType(t_ledconfig ledinfo);
        void LedBluetoothConnect(TicosRGB888 color,uint32_t time,uint8_t state);
        void LedFirewareUpdate(TicosRGB888 color,uint32_t time);
        void LedDrinkWaterAlarm(uint32_t time);
        void LedClear(void);
      #if 0
        uint8_t setLedMode(uint8_t mode);
        uint8_t setLedColor(TicosRGB888 color);
        uint8_t setLedColorLight(uint8_t bright);
        uint8_t setLedDelayTime(uint32_t delaytime);
        uint32_t getLedDelayTime(void);
        uint8_t getLedMode(void);
        uint8_t getLedColorLight(void);
        TicosRGB888 getLedColor(void);
        #endif
        static void Led_Task(void *arg) ;
        bool ledTaskCreate(uint32_t taskSize,int taskPriority ); 
        bool ledTaskDelete(); 

        uint32_t _length = 6;
        t_ledconfig led_config;
        TaskHandle_t LedTaskHandle = NULL;
};

#endif // __WS2812_STRIP_H

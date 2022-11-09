#include <WS2812RMT.h>
#include <Log.h>

#include "freertos/task.h"
#include "freertos/FreeRTOS.h"
#include <driver/gpio.h>
#include <driver/rmt.h>

#define PWR_ON    1
#define PWR_OFF   0

bool WS2812RMT::ledTaskCreate(uint32_t taskSize,int taskPriority )
{
    if(LedTaskHandle == NULL)
    {
         if(taskSize < 2048) taskSize = 2048; //至少任务的盏大小不能小于2K
         if(taskPriority < 3) taskPriority = 3;//默认优先等级为3
         if(xTaskCreate(WS2812RMT::Led_Task, "Led_Task", taskSize , this, taskPriority, &LedTaskHandle)==true)
         {
            logInfo("creat led task ok ,LedTaskHandle:%d",LedTaskHandle);           
            return true;
         }
         logInfo("creat led task failed ,LedTaskHandle:%d",LedTaskHandle);
    } 
    return false ;
}
bool WS2812RMT::ledTaskDelete()
{
    if(LedTaskHandle != NULL)
    {
        vTaskDelete(LedTaskHandle);
        logInfo("delete led task ,LedTaskHandle:%d",LedTaskHandle);
        LedTaskHandle = NULL;
        return true ;
    }
    return false;
}

bool WS2812RMT::open(void) {
    if (!m_strip) {
        digitalWrite(m_pwd,  PWR_ON);
        m_strip = led_strip_init(m_chn, m_pin, nums());
        ledTaskCreate(4096,4);
        _length = nums();
    }
    return m_strip != NULL;
}

bool WS2812RMT::close(void) {
    if (m_strip && ESP_OK == led_strip_deinit(m_strip)) {
        m_strip = NULL;
    }
    ledTaskDelete();
    digitalWrite(m_pwd,  PWR_OFF);
    return m_strip == NULL;
}
void WS2812RMT::sendPixel(uint32_t j ,uint8_t r, uint8_t g, uint8_t b) {
    // Neopixel wants colors in green then red then blue order
    m_strip->set_pixel(m_strip, j, r, g, b);
}

void WS2812RMT::sendRefresh(void) {
    // Neopixel wants colors in green then red then blue order
    m_strip->refresh(m_strip, 100);
}

void WS2812RMT::send(uint16_t i, TicosRGB888* pixel) {
    // Neopixel wants colors in green then red then blue order
    m_strip->set_pixel(m_strip, i, pixel->r, pixel->g, pixel->b);
    // Wait long enough without sending any bots to cause the pixels to latch
    // delayMicroseconds(50);
}

void WS2812RMT::refresh(void) {
    // Wait long enough without sending any bots to cause the pixels to latch
    m_strip->refresh(m_strip, 100);
}

void WS2812RMT::clear(void) {
    m_strip->clear(m_strip, 50);
}

void WS2812RMT::fill(uint8_t r, uint8_t g, uint8_t b) {
    TicosRGB888 pixel = { r, g, b };
    fill(&pixel);
}

void WS2812RMT::fill(TicosRGB888* pixel, uint16_t count, bool tail) {
    TicosRGB888 black = { 0, 0, 0 };
    const uint8_t n = nums();
    if (count == 0 || count > n) {
        count = n;
    }
    uint16_t curr, end, step, skip;
    if (tail) {
        curr = n - 1;
        skip = n - 1 - count;
        end = -1;
        step = -1;
    } else {
        curr = 0;
        skip = count;
        end = n;
        step = 1;
    }
    cli();
    for (; curr != skip; curr+=step) {
        send(curr, pixel);
    }
    // Fill pixels
    for (; curr != end; curr+=step) {
        send(curr, &black);
    }
    refresh();
    sei();
}

void WS2812RMT::pattern(TicosRGB888* pixels, uint16_t len) {
    uint16_t i = 0;
    const uint8_t n = nums();
    TicosRGB888 black = { 0, 0, 0 };
    if (len > n) {
        len = n;
    }
    cli();
    for (; i < len; ++i) {
        send(i, pixels + i);
    }
    for (; i < n; ++i) {
        send(i, &black);
    }
    refresh();
    sei();
}

/**
 * HSV(Hue, Saturation, Value) 模型中，颜色的参数分别是：色调(H)，饱和度(S)，明度(V)。它更类似于人类感觉颜色的方式，
 * 颜色？深浅？明暗？通过这种方式可以很直观的改变渲染效果。
 * @brief 将HSV颜色空间转换为RGB颜色空间
 *      - 因为HSV使用起来更加直观、方便，所以代码逻辑部分使用HSV。但WS2812B RGB-LED灯珠的驱动使用的是RGB，所以需要转换。
 * 
 * @param  h HSV颜色空间的H：色调。单位°，范围0~360。（Hue 调整颜色，0°-红色，120°-绿色，240°-蓝色，以此类推）
 * @param  s HSV颜色空间的S：饱和度。单位%，范围0~100。（Saturation 饱和度高，颜色深而艳；饱和度低，颜色浅而发白）
 * @param  v HSV颜色空间的V：明度。单位%，范围0~100。（Value 控制明暗，明度越高亮度越亮，越低亮度越低）
 * @param  r RGB-R值的指针
 * @param  g RGB-G值的指针
 * @param  b RGB-B值的指针
 *
 * Wiki: https://en.wikipedia.org/wiki/HSL_and_HSV
 * https://blog.csdn.net/Mark_md/article/details/115132435
 * https://blog.csdn.net/qq_51985653/article/details/113392665
 */
/*
void led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b)
{
    h %= 360; // h -> [0,360]
    uint32_t rgb_max = v * 2.55f;
    uint32_t rgb_min = rgb_max * (100 - s) / 100.0f;

    uint32_t i = h / 60;
    uint32_t diff = h % 60;

    // RGB adjustment amount by hue
    uint32_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

    switch (i) {
    case 0:
        *r = rgb_max;
        *g = rgb_min + rgb_adj;
        *b = rgb_min;
        break;
    case 1:
        *r = rgb_max - rgb_adj;
        *g = rgb_max;
        *b = rgb_min;
        break;
    case 2:
        *r = rgb_min;
        *g = rgb_max;
        *b = rgb_min + rgb_adj;
        break;
    case 3:
        *r = rgb_min;
        *g = rgb_max - rgb_adj;
        *b = rgb_max;
        break;
    case 4:
        *r = rgb_min + rgb_adj;
        *g = rgb_min;
        *b = rgb_max;
        break;
    default:
        *r = rgb_max;
        *g = rgb_min;
        *b = rgb_max - rgb_adj;
        break;
    }
}*/
#if 0
//
uint8_t WS2812RMT::setLedMode(uint8_t mode)
{
   LedMode = mode;
   return 1;
}
//
uint8_t WS2812RMT::setLedColor(TicosRGB888 color)
{
   SetClolor = color;
   return 1;
}
//
uint8_t WS2812RMT::setLedColorLight(uint8_t bright)
{
   LedBright = bright;
   return 1;
}
uint8_t WS2812RMT::setLedDelayTime(uint32_t delaytime)
{
   LedDelayTime = delaytime;
   return 1;
}
//
uint8_t WS2812RMT::getLedMode(void)
{
     return LedMode;
}
//
TicosRGB888 WS2812RMT::getLedColor(void)
{
   return SetClolor;
}
//
uint8_t WS2812RMT::getLedColorLight(void)
{
    return LedBright;
}
uint32_t WS2812RMT::getLedDelayTime(void)
{
    return LedDelayTime;
}
#endif

void WS2812RMT::setLedConfig(t_ledconfig config)
{
    led_config = config;
}
//清屏
void WS2812RMT::LedClear(void)
{      
        //strip->clear(strip,50);
    for (uint32_t j = 0; j < _length; j ++) {
         m_strip->set_pixel(m_strip, j,0,0,0);
    }
    m_strip->refresh(m_strip, 100);
    vTaskDelay(pdMS_TO_TICKS(1));
}
//单色
void WS2812RMT::LedOneColor(t_ledconfig ledinfo)
{
    TicosRGB888 color;
    color = ledinfo.color ;
    for (uint32_t j = 0; j < _length; j ++) {
        m_strip->set_pixel(m_strip, j, color.r, color.g, color.b);
    }
    m_strip->refresh(m_strip, 100);
    vTaskDelay(pdMS_TO_TICKS(10));
    //logDebug("LedOneColor,red:%d,green:%d,blue:%",red,green,blue);
}

//水温灯效
void WS2812RMT::LedWaterTemp(t_ledconfig ledinfo)
{
    TicosRGB888 color;
    //color = ledinfo.color ;
    //烫水
    if(ledinfo.grade == 3){
         color = {50,10,00};
    }
    //热水
    else if(ledinfo.grade == 2){
         color = {50,20,00};
    }
    //温水
    else if(ledinfo.grade == 1){
         color = {10,50,00};
    }
    //凉水
    else if(ledinfo.grade == 0){
         color = {0,10,50};
    }
    for (uint32_t j = 0; j < _length; j ++) {
        m_strip->set_pixel(m_strip, j, color.r, color.g, color.b);
    }
    m_strip->refresh(m_strip, 100);
    vTaskDelay(pdMS_TO_TICKS(10));
}

//水量灯效
void WS2812RMT::LedWaterVol(t_ledconfig ledinfo)
{
    TicosRGB888 color;

    for (uint32_t j = 0; j < _length; j ++) {
        if(j <= ledinfo.grade){
            color = {0,00,50};
        }
        else{
             color = {50,50,50};
        }
        m_strip->set_pixel(m_strip, j, color.r, color.g, color.b);
        //logInfo("LedWaterVol,j:%d,r:%d,g:%d,b:%d\n\n",j,color.r,color.g,color.b);
    }
    m_strip->refresh(m_strip, 100);
    vTaskDelay(pdMS_TO_TICKS(10));
}

//电量灯效,电量分为6档即大于百分之16为绿色，小于16%为红色
void WS2812RMT::LedBatteryLevel(t_ledconfig ledinfo)
{
    TicosRGB888 color;

    for (uint32_t j = 0; j < _length; j ++) {
        if( j <= ledinfo.grade  ){
            if( ledinfo.grade < 1)//低电提示
            color = {30,0,0}; //
            else
             color = {0,30,0};//绿色
        }
        else{
             color = {30,30,30};
        }
        m_strip->set_pixel(m_strip, j, color.r, color.g, color.b);
        //logInfo("Led Battery Level,grade:%d,r:%d,g:%d,b:%d\n\n", ledinfo.grade,color.r,color.g,color.b);
    }
    m_strip->refresh(m_strip, 100);
    vTaskDelay(pdMS_TO_TICKS(10));
}

//充电状态灯
void WS2812RMT::LedChargerType(t_ledconfig ledinfo)
{
    TicosRGB888 color;
    if(ledinfo.type){
       color = {30,0,0} ; //红灯
    }
    else{
        color = {0,30,0} ;//充满绿灯
    }
   
    for (uint32_t j = 0; j < _length; j ++) {
        m_strip->set_pixel(m_strip, j, color.r, color.g, color.b);
    }
    m_strip->refresh(m_strip, 100);
    vTaskDelay(pdMS_TO_TICKS(10));
   // logDebug("LedChargerType,red:%d,green:%d,blue:%",color.r,color.g,color.b);
}

//三色循环
void WS2812RMT::LedThreeColor(t_ledconfig ledinfo)
{
    TicosRGB888 color;
    color = ledinfo.color ;
    for (uint32_t j = 0; j < _length; j ++) {
        if(j<2) 
         m_strip->set_pixel(m_strip, j, color.r, 0, 0);
        else if(j<4)
         m_strip->set_pixel(m_strip, j, 0, color.g, 0);
        else if(j<6)
            m_strip->set_pixel(m_strip, j, 0, 0, color.b);
    }
    m_strip->refresh(m_strip, 100);
    vTaskDelay(pdMS_TO_TICKS(10));
}

// @description: 一个一个逐渐亮起 * @param {type} * @return: 
void WS2812RMT::LedOneByOne(TicosRGB888 color,uint32_t time)
{
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
    uint16_t hue = 0;
    uint16_t start_rgb = 0;
    static uint8_t offset = 0;

     for (int j = 0; j < _length; j++){
        // Build RGB values
        red =  onebyonevalue[offset][start_rgb];
        green = onebyonevalue[offset][start_rgb+1];
        blue = onebyonevalue[offset][start_rgb+2];
        start_rgb += 3;//每次取3个数值
        // hue = j * 360 / nums + start_rgb;
        // led_strip_hsv2rgb(hue, 100, color_v, &red, &green, &blue);
        // Write RGB values to strip driver
        m_strip->set_pixel(m_strip, j, red, green, blue);
        //logInfo("Led One By One,offset:%d,j:%d,r:%d,g:%d,b:%d\n\n",offset,j,red,green,blue);
        m_strip->refresh(m_strip, 100);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    //strip->refresh(strip, 100);
    vTaskDelay(pdMS_TO_TICKS(time));
    //strip->clear(strip, 100);
    //LedClear();
    if(offset < (_length-1)){
        offset ++;
    }
    else{
        offset = 0;
    }
     vTaskDelay(pdMS_TO_TICKS(1));
}

//呼吸灯效果
void WS2812RMT::LedBreathColor(TicosRGB888 color,uint32_t time)
{
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
    uint32_t color_V_MAX = 70;
    static uint32_t count_v = 0;  
    static bool plus = true ;

    red   = color.r;
    green = color.g;
    blue  = color.b;

    if(plus == true){//吸
        if(count_v < color_V_MAX){
            count_v+=1;
        }
        else {
            plus = false ;
            //呼吸转换的时候加一个停顿，模拟人的呼吸效果
            vTaskDelay(pdMS_TO_TICKS(time));
        }
    }
    else{//呼
        if(count_v > 1){
            count_v-=1;
        }
        else{
            plus = true ;
            //呼吸转换的时候加一个停顿，模拟人的呼吸效果
            vTaskDelay(pdMS_TO_TICKS(time));
        }
    }
    //调亮度,亮度控制在50以内
    if( color.r>0){
        red = count_v * 0.5f ;
    }
    if(color.g>0){
        green = count_v * 0.5f ;
    }
    if(color.b>0){
        blue = count_v* 0.5f ;
    }
    for (uint32_t j = 0; j < _length; j ++) {
        m_strip->set_pixel(m_strip, j,red,green,blue);
    }
    //logInfo("LedbreathColor,count_v:%d,red:%d,green:%d,blue:%d,\n\n",count_v,red,green,blue);
    m_strip->refresh(m_strip, 100);
    vTaskDelay(pdMS_TO_TICKS(1));
}
//彩虹效果
//@description: 彩虹效果 * @param {type} * @return:
void WS2812RMT::LedRanbow(uint32_t time,uint8_t V)
{
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
    uint16_t hue = 0;
    uint16_t start_rgb = 0;

    for (int i = 0; i < 3; i++){
        for (int j = i; j < _length; j += 3){
            // Build RGB values
            hue = j * 360 / _length + start_rgb;
           // led_strip_hsv2rgb(hue, 100, V, &red, &green, &blue);
            hsv2rgb(hue, 100, V, &red, &green, &blue);
            // Write RGB values to strip driver
            m_strip->set_pixel(m_strip, j, red, green, blue);
        }
        // Flush RGB values to LEDs
        m_strip->refresh(m_strip, 100);
        vTaskDelay(pdMS_TO_TICKS(time));
        m_strip->clear(m_strip, 50);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    start_rgb += 60;
    // logDebug("rgb test,j:%d",j);
}
//单色起伏
void WS2812RMT::LedUpAndDownColor(TicosRGB888 color,uint32_t time)
{
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
    static uint8_t offset = 0;

    for (uint8_t j = 0,i = (_length - 1); j < _length; j ++,i--){

        if(j == offset){
            red = color.r;
            green = color.g;
            blue = color.b;
        }
        else{
            red = color.r * 0.3f;
            green = color.g * 0.3f;
            blue = color.b * 0.3f;
        }
        // Write RGB values to strip driver
        m_strip->set_pixel(m_strip, i, red, green, blue);
        //logInfo("LedFirewareUpdate,offset:%d,j:%d,r:%d,g:%d,b:%d\n\n",offset,j,red,green,blue);
    }
    m_strip->refresh(m_strip, 100);
    //vTaskDelay(pdMS_TO_TICKS(time));
    if(offset < (_length-1 )){
        offset ++ ;
    }
    else{
        offset = 0;        
    }
    vTaskDelay(pdMS_TO_TICKS(time));
}
//蓝牙连接
//state : 0 连接中，1 连接成功；
//@description: 连接中循环旋转，如果连接成功停止旋转，蓝灯常亮 * @param {type} * @return:
void WS2812RMT::LedBluetoothConnect(TicosRGB888 color,uint32_t time,uint8_t state)
{
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
    static uint8_t offset = 0;

    for (uint8_t j = 0,i = (_length - 1); j < _length; j ++,i--){

        if(state == 0)//未连接
        {
            if(j < offset){
                red = color.r;
                green = color.g;
                blue = color.b;
            }
            else{
                red =  0;
                green = 0;
                blue = 0;
            }
        }
        else{//已连接
            red = color.r;
            green = color.g;
            blue = color.b;
        }
        // Write RGB values to strip driver
        m_strip->set_pixel(m_strip, i, red, green, blue);
        //logInfo("LedFirewareUpdate,offset:%d,j:%d,r:%d,g:%d,b:%d\n\n",offset,j,red,green,blue);
    }
    m_strip->refresh(m_strip, 100);
    //vTaskDelay(pdMS_TO_TICKS(time));
    if(offset < (_length )){
        offset ++ ;
    }
    else{
        offset = 0;
        LedClear();
    }
    vTaskDelay(pdMS_TO_TICKS(time));
}
//固件升级
//@description: 单色旋转效果 * @param {type} * @return:
void WS2812RMT::LedFirewareUpdate(TicosRGB888 color,uint32_t time)
{
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
    static uint8_t offset = (_length -1);
   
    for (uint8_t j = (_length -1); j > 0 ; j --){
        if(j==offset){
            red = color.r;
            green = color.g;
            blue = color.b;
        }
        else{
            red =  0;
            green = 0;
            blue = 0;
        }
        // Write RGB values to strip driver
        m_strip->set_pixel(m_strip, j, red, green, blue);
        //logInfo("LedFirewareUpdate,offset:%d,j:%d,r:%d,g:%d,b:%d\n\n",offset,j,red,green,blue);
    }
    m_strip->refresh(m_strip, 100);
    //vTaskDelay(pdMS_TO_TICKS(time));
    if(offset > (1 )){
        offset -- ;
    }
    else{
        offset = (_length - 1);
    }   
    vTaskDelay(pdMS_TO_TICKS(time));

}
//饮水提醒
//@description: 追逐效果 * @param {type} * @return:
void WS2812RMT::LedDrinkWaterAlarm(uint32_t time)
{
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
    uint16_t hue = 0;
    uint16_t start_rgb = 0;
    static uint8_t offset = 0;

     for (int j = 0; j < _length; j++){
        // Build RGB values
        red =  alarmvalue[offset][start_rgb];
        green = alarmvalue[offset][start_rgb+1];
        blue = alarmvalue[offset][start_rgb+2];
        start_rgb += 3;//每次取3个数值
        m_strip->set_pixel(m_strip, j, red, green, blue);
        //logInfo("Led One By One,offset:%d,j:%d,r:%d,g:%d,b:%d\n\n",offset,j,red,green,blue);
        m_strip->refresh(m_strip, 100);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    //strip->refresh(strip, 100);
    vTaskDelay(pdMS_TO_TICKS(time));
    //strip->clear(strip, 100);
    //LedClear();
    if(offset < (_length-1)){
        offset ++;
    }
    else{
        offset = 0;
    }
     vTaskDelay(pdMS_TO_TICKS(1));
   
}
// led task
void WS2812RMT::Led_Task(void *arg) 
{
    WS2812RMT *self = (WS2812RMT*) arg;
    uint8_t ret ;
    uint8_t mode = 0;
    uint8_t bright = 0;
    uint32_t delaytime = 0;
    TicosRGB888 color ;
    t_ledconfig ledinfo ;
    static uint8_t old_mode = 0;
	logInfo("Led_Task ok !\n\n");

    while(true)
    {   
        ledinfo = self->led_config;

        if(old_mode != ledinfo.mode)
        {
            if(old_mode == CHAGER_MODE)//如果正在充电，则不允许切换状态
            {
               // vTaskDelay(pdMS_TO_TICKS(10));
               // return ;
                
            }
            else
            {
                mode = ledinfo.mode;
                old_mode = ledinfo.mode ;
                logInfo("Led mode changed,mode:%d\n\n",ledinfo.mode);
            }
        }
        switch(mode)
        {
            case SINGLE_COLOR_MODE: //单色  0
                {
                    self->LedOneColor(ledinfo);
                }break;
          
            case WATER_TEMP_MODE: //水温 1
                {
                    self->LedWaterTemp(ledinfo);
                    //logInfo("led set temp\n\n");
                }break;
            case WATER_VOL_MODE: //水量 2
                {
                    self->LedWaterVol(ledinfo);
                   // logInfo("led set Water Vol\n\n");
                }break;
            case BATTERY_LEVEL_MODE: //电量 3
                {
                    self->LedBatteryLevel(ledinfo);
                    //logInfo("led set Battery Level\n\n");
                }break;
            case CHAGER_MODE: //充电  4
                {
                    self->LedChargerType(ledinfo);
                }break;
            case BLUETOOTH_CONNECTING_MODE://蓝牙连接  4 ,R:0,G:0,B:50
                {
                    self->LedBluetoothConnect(color,delaytime,0);//delaytime: 200
                }break;
            case BLUETOOTH_CONNECTED_MODE://蓝牙连接  5 ,R:0,G:0,B:50
                {
                    self->LedBluetoothConnect(color,delaytime,1);//delaytime:200
                }break;
            case FIREWARE_UPDATE_MODE://固件升级  6 ,R:0,G:50,B:20
                {
                    self->LedFirewareUpdate(color,delaytime);//delaytime :200
                }break;
            case DRINKWATER_REMIN_MODE://喝水达标  7
                {
                    self->LedOneByOne(color,delaytime);  //delaytime:100
                }break;
            case DRINKWATER_ALARM_MODE://喝水提醒  8 //R:0,G:50,B:20
                {
                    self->LedDrinkWaterAlarm(delaytime);//delaytime:30
                }break;
              case THREE_COLOR_MODE://三色  9
                {
                    self->LedThreeColor(ledinfo);
                }break;

            case ONEBYONE_COLOR_MODE://追逐  10
                {
                    self->LedOneByOne(color,delaytime);//delaytime:100
                }break;
            case BREATH_COLOR_MODE://呼吸灯效果  11
                {
                    self->LedBreathColor(color,delaytime);//delaytime：20
                }break; 
            case RANBOW_COLOR_MODE://彩虹  12
                {
                    self->LedRanbow(delaytime,bright);//delaytime:30
                }break;
            case UPS_DOWN_MODE://单色起伏  13,R:0,G:50,B:0
                {
                    self->LedUpAndDownColor(color,delaytime);//delaytime:300
                }break;

                default : self->LedClear();
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }    
    vTaskDelete(NULL);

}
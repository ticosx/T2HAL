#include <TicosNTCSensor.h>
#include <Log.h>

#define TEMP_TABLE_LEN         121 //温度区间总值
const static uint16_t s_temp_table[TEMP_TABLE_LEN] =
{
    1778,1733,1689,1645,1601,1558,1515,1472,1430,1389,   /*-10° ~ -1°*/
    1348,1308,1269,1230,1192,1154,1118,1082,1047,1013,   /*0° ~ 9°*/
    980,947,916,885,855,826,798,770,744,718,             /*10° ~ 19°*/
    693,668,645,622,600,579,558,539,519,501,             /*20° ~ 29°*/
    483,466,449,433,417,403,388,374,361,348,             /*30° ~ 39°*/
    336,324,312,301,290,280,270,261,251,243,             /*40° ~ 49°*/
    234,226,218,211,203,196,190,183,177,171,             /*50° ~ 59°*/
    165,159,154,149,144,139,135,130,126,122,             /*60° ~ 69°*/
    118,114,110,107,103,100,97,94,91,88,                 /*70° ~ 79°*/
    85,82,80,77,75,73,70,68,66,64,                       /*80° ~ 89°*/
    62,60,58,57,55,53,52,50,49,47,                       /*90° ~ 99°*/
    46,45,43,42,41,40,                                   /*100° ~ 105°*/
};

bool TicosNTCSensor::open(void) {
    if (!m_inited) {
        m_inited = m_adc.open();
    }
    return true;
}

bool TicosNTCSensor::close(void) {
    bool res = true;
    if (m_inited) {
        if ((res = m_adc.close())) {
            m_inited = false;
        }
    }
    return res;
}

/*****************************************************************************
*函数名   : voltage
*函数功能 : 获取ADC转换电压值
*输入参数 : 无
*输出参数 : 无
*返回值   : volt_average电压值
*****************************************************************************/
uint32_t TicosNTCSensor::voltage(void) {
    uint8_t  queue_size = TICOS_ADC_NTC_QUEUE_SIZE;
    uint8_t  i = 0;
    uint32_t ret =0;

    uint32_t adc_max = 0;
    uint32_t adc_min = 0;
    uint32_t adc_sum = 0;
    uint32_t adc_average = 0;
    uint32_t volt_average = 0; //电压值

    //TEMP ADC CHECK
    for(i = 0; i < queue_size; i++) //一次读取10个
    {
        m_ntc_queue[i]= m_adc.adc(); // 采集ADC原始值
    }  

    adc_max = m_ntc_queue[0];
    adc_min = m_ntc_queue[0];

    for(i = 0; i < queue_size; i++)
    {
        if(adc_max < m_ntc_queue[i])
            adc_max = m_ntc_queue[i];
        if(adc_min > m_ntc_queue[i])
            adc_min = m_ntc_queue[i];
        adc_sum += m_ntc_queue[i];
    }
    adc_sum = adc_sum - adc_max - adc_min;

    /*平均AD值*/
    adc_sum += (queue_size - 2) / 2;
    adc_average = adc_sum / (queue_size - 2);
    //  Ref	= 3.3V
    volt_average = m_adc.voltage(adc_average); //根据电阻分压计算电压

    return volt_average;
}

/*****************************************************************************
*函数名   : temperature
*函数功能 : 获取温度值
*输入参数 : 无
*输出参数 : 无
*返回值   : i16temp  温度值
*****************************************************************************/
int16_t TicosNTCSensor::temperature(void)
{
    uint32_t volt_value = 0;
    uint16_t u16i = 0;
    int16_t  i16temp = 0;

    volt_value = voltage();
    for(u16i = 0; u16i < TEMP_TABLE_LEN; u16i++) { //查表
        if(volt_value >= s_temp_table[u16i]) {
            break;
        }
    }
    i16temp = u16i - 10; //温度对应ADC电压表是从-10℃起

    return i16temp;
}

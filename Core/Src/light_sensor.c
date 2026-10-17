#define WINDOW_SIZE 4
#include "light_sensor.h"
extern ADC_HandleTypeDef hadc1;
HAL_StatusTypeDef Light_ReadRaw(uint32_t *raw)
{
if(raw==NULL)
{
    return HAL_ERROR;
}
HAL_StatusTypeDef status;
uint32_t temp_raw=0;
status=HAL_ADC_Start(&hadc1);
if(status!=HAL_OK)
{
    return status;
}
status=HAL_ADC_PollForConversion(&hadc1,100);
if(status!=HAL_OK)
{   HAL_ADC_Stop(&hadc1);
    return status;}
temp_raw = HAL_ADC_GetValue(&hadc1);

status = HAL_ADC_Stop(&hadc1);

if (status != HAL_OK)
{
    return status;
}

*raw = temp_raw;

return HAL_OK;
}
HAL_StatusTypeDef Light_ReadAverage(uint32_t *result, uint32_t count)
{
uint32_t sum=0;
uint32_t raw=0;
HAL_StatusTypeDef status;
if(result==NULL||count==0||count>1000)
{
    return HAL_ERROR;
}
for(uint32_t i=0;i<count;i++) 
{
  status=Light_ReadRaw(&raw);
    if(status!=HAL_OK)
    {
        return status;
    }
    sum+=raw;

}
*result=sum/count;
return HAL_OK;
}
HAL_StatusTypeDef Light_ReadMovingAverage(uint32_t *result)
{
    static uint32_t buffer[WINDOW_SIZE] = {0};
    static uint32_t sum = 0;
    static uint8_t index = 0;
    static uint8_t filled = 0;

    uint32_t new_raw = 0;
    HAL_StatusTypeDef status;

    // TODO 1：检查 result 是否为空指针
    if(result == NULL)
    {
        return HAL_ERROR;
    }

    // TODO 2：调用 Light_ReadRaw 获取新采样
    status = Light_ReadRaw(&new_raw);
    
    // TODO 3：判断采样状态
    if(status != HAL_OK)
    {
        return status;
    }
    // TODO 4：更新滚动累加和和缓冲区
    sum -= buffer[index];
    buffer[index] = new_raw;
    sum += buffer[index];
    index = (index + 1) % WINDOW_SIZE;
    // TODO 5：更新 index 和 filled
    if(filled < WINDOW_SIZE)
    {
        filled++;
    }
    // TODO 6：计算平均值并返回 HAL_OK
    *result = sum / filled;
    return HAL_OK;
}

HAL_StatusTypeDef Light_ReadOversampled(uint32_t *result)
{
    uint32_t sum = 0;
    uint32_t raw = 0;
    HAL_StatusTypeDef status;

    // TODO 1：检查 result 指针
    if(result == NULL)
    {
        return HAL_ERROR;
    }

    // TODO 2：连续读取 16 次 ADC
    for(int i = 0; i < 16; i++)
    {
        status = Light_ReadRaw(&raw);
        if(status != HAL_OK)
        {
            return status;
        }
        sum += raw;
    }

    // TODO 3：每次判断 status，失败立即返回
    
    // TODO 4：累加成功采集的 raw

    // TODO 5：将 sum 右移 2 位，写入 *result
   *result = sum >> 2; 
    return HAL_OK;
}
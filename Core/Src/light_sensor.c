#include "light_sensor.h"
extern ADC_HandleTypeDef hadc1;
HAL_StatusTypeDef Light_ReadRaw(uint32_t *raw)
{
if(raw==NULL)
{
    return HAL_ERROR;
}
HAL_StatusTypeDef status;
status=HAL_ADC_Start(&hadc1);
if(status!=HAL_OK)
{
    return status;
}
status=HAL_ADC_PollForConversion(&hadc1,100);
if(status!=HAL_OK)
{   HAL_ADC_Stop(&hadc1);
    return status;}
*raw=HAL_ADC_GetValue(&hadc1);
status=HAL_ADC_Stop(&hadc1);
return status;





}



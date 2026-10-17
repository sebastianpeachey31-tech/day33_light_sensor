#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include "main.h"

HAL_StatusTypeDef Light_ReadRaw(uint32_t *raw);//*raw是声明来接收传感器原始数据地址的
HAL_StatusTypeDef Light_ReadAverage(uint32_t *result, uint32_t count);//*result是声明来接收传感器平均数据地址的
HAL_StatusTypeDef Light_ReadMovingAverage(uint32_t *result);
HAL_StatusTypeDef Light_ReadOversampled(uint32_t *result);
#endif
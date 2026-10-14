#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include "main.h"

HAL_StatusTypeDef Light_ReadRaw(uint32_t *raw);//*raw是声明来接收传感器原始数据地址的
#endif
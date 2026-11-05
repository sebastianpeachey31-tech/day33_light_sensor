#ifndef I2C_SCAANNNER_H
#define I2C_SCAANNNER_H

#include "stm32f1xx_hal.h"

uint8_t I2C_Scan(I2C_HandleTypeDef *hi2c,uint8_t *address,uint8_t max_count);

#endif // I2C_SCAANNNER_H
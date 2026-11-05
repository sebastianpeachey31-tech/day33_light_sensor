#ifndef OLED_H
#define OLED_H

#include "stm32f1xx_hal.h"

//OLED的7位I2C地址；0x3C
//STM32 HAL需要将地址左移一位，变为0x78
#define OLED_ADDR (0x3c << 1)

//发送一条OLED控制命令
HAL_StatusTypeDef OLED_WriteCommand(
    I2C_HandleTypeDef *hi2c,
    uint8_t commmand
);

//发送一个OLED显示数据字节
HAL_StatusTypeDef OLED_WriteData(
    I2C_HandleTypeDef *hi2c,
    uint8_t data
);

//初始化 SSD1315 OLED 显示屏
HAL_StatusTypeDef OLED_Init(
    I2C_HandleTypeDef *hi2c
);


//设置OLED显存写入位置
HAL_StatusTypeDef OLED_SetPosition(
    I2C_HandleTypeDef *hi2c,
    uint8_t page,
    uint8_t column
);

//清空OLED显存
HAL_StatusTypeDef OLED_Clear(
    I2C_HandleTypeDef *hi2c
);

//向OLED当前位置写入一个6x8字模
//font指向包含6个字节的字模数组
HAL_StatusTypeDef OLED_WriteGlyph6x8(
    I2C_HandleTypeDef *hi2c,
    const uint8_t font[6]
);

//显示一个6x8 ASCII字符
//hi2c：I2C外设句柄
//ch：需要显示的字符，例如'H'或'I'
//返回值：HAL_OK表示发送成功
//        HAL_ERROR表示字符不支持或参数错误
//        其他状态表示底层通信失败
HAL_StatusTypeDef OLED_ShowChar(
    I2C_HandleTypeDef *hi2c,
    char ch
);

//OLED字符串显示函数声明
//hi2c：I2C外设句柄
//str：指向以'\0'结尾的字符串
//返回值：HAL_OK表示所有字符发送成功
//        其他状态表示参数、字符或通信出错
HAL_StatusTypeDef OLED_ShowString(
    I2C_HandleTypeDef *hi2c,
    const char *str
);
#endif
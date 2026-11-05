#include "i2c_scanner.h"

uint8_t I2C_Scan(
I2C_HandleTypeDef *hi2c,
uint8_t *addresses,
uint8_t max_count

)
{
 //1.检查传入的指针和数组容量
if(hi2c==NULL||addresses==NULL||max_count==0)
{
    return 0;
}

uint8_t count=0;
//2.遍历合法的常规7位I2C设备地址
for(uint8_t addr=0x08;addr<=0x77;addr++)
{
 HAL_StatusTypeDef status;
 //3.检查当前地址是否有设备应答
 status=HAL_I2C_IsDeviceReady(hi2c,(uint16_t)(addr<<1),2,10);
 //4.找到设备后保存地址
 if(status==HAL_OK)
 {
  addresses[count]=addr;
   count++;
//5.防止写出数组边界
    if(count>=max_count)
    {
        break;
    }
}

}

return count;
}




























#include "w25q64.h"
extern SPI_HandleTypeDef hspi1;
HAL_StatusTypeDef W25Q64_ReadID(uint8_t *id)
{//检查指针是否合法
    if(id == NULL)
    {
      return HAL_ERROR;
    }
//准备发送和接收的数据
uint8_t tx[4]={
    W25Q64_CMD_READ_ID,
    0x00,
    0x00,
    0x00
};
uint8_t rx[4]={0};
HAL_StatusTypeDef status;
//拉低CS。选中flash
HAL_GPIO_WritePin(
FLASH_CS_GPIO_Port,
FLASH_CS_Pin,
GPIO_PIN_RESET
);
//通过SPI发送命令，并且读取ID
status=HAL_SPI_TransmitReceive(
&hspi1,
tx,
rx,
4,
100
);
//拉高CS,结束通信
HAL_GPIO_WritePin(
FLASH_CS_GPIO_Port,
FLASH_CS_Pin,
GPIO_PIN_SET

);
//检查SPI通信状态
if(status != HAL_OK)
{
    return status;
}
//提取三个ID字节
id[0]=rx[1];
id[1]=rx[2];
id[2]=rx[3];

return HAL_OK;
}

HAL_StatusTypeDef
W25Q64_ReadStatus(uint8_t *status_reg)
{if(status_reg ==NULL)
{
    return HAL_ERROR;
}
uint8_t tx[2]={
W25Q64_CMD_READ_STATUS,
0xFF
};
uint8_t rx[2] ={0};
HAL_StatusTypeDef status;
//选中W25Q64
HAL_GPIO_WritePin(
FLASH_CS_GPIO_Port,
FLASH_CS_Pin,
GPIO_PIN_RESET

);
//发送命令，并接收状态
status = HAL_SPI_TransmitReceive(
&hspi1,
tx,
rx,
2,
100
);
//取消选中W25Q64
HAL_GPIO_WritePin(
FLASH_CS_GPIO_Port,
FLASH_CS_Pin,
GPIO_PIN_SET
);

//检查SPI通信效果
if(status != HAL_OK)
{
    return status;
}
//保存状态寄存器数据
*status_reg = rx[1];
return HAL_OK;
}
HAL_StatusTypeDef W25Q64_WriteEnable(void)
{
    uint8_t cmd = W25Q64_CMD_WRITE_ENABLE;

    HAL_StatusTypeDef status;

    // 选中 Flash
    HAL_GPIO_WritePin(
        FLASH_CS_GPIO_Port,
        FLASH_CS_Pin,
        GPIO_PIN_RESET
    );

    // 发送写使能命令 0x06
    status = HAL_SPI_Transmit(
        &hspi1,
        &cmd,
        1,
        100
    );

    // 取消选中 Flash
    HAL_GPIO_WritePin(
        FLASH_CS_GPIO_Port,
        FLASH_CS_Pin,
        GPIO_PIN_SET
    );

    return status;
}

HAL_StatusTypeDef W25Q64_WriteDisable(void)
{
    uint8_t cmd = W25Q64_CMD_WRITE_DISABLE;

    HAL_StatusTypeDef status;

    // 1. 拉低 CS，选中 W25Q64
    HAL_GPIO_WritePin(
        FLASH_CS_GPIO_Port,
        FLASH_CS_Pin,
        GPIO_PIN_RESET
    );

    // 2. 发送写禁止命令 0x04
    status = HAL_SPI_Transmit(
        &hspi1,
        &cmd,
        1,
        100
    );

    // 3. 拉高 CS，结束通信
    HAL_GPIO_WritePin(
        FLASH_CS_GPIO_Port,
        FLASH_CS_Pin,
        GPIO_PIN_SET
    );

    // 4. 返回 HAL 通信状态
    return status;
}

HAL_StatusTypeDef W25Q64_ReadData(
    uint32_t address,
    uint8_t *data,
    uint16_t size)
{
    // 1. 参数检查
    if (data == NULL || size == 0)
    {
        return HAL_ERROR;
    }

    // 检查是否超出 W25Q64 的 8 MiB 空间
    if (address >= 0x800000 ||
        size > (0x800000UL - address))
    {
        return HAL_ERROR;
    }

    // 2. 准备命令与 24 位地址
    uint8_t tx[4] = {
        W25Q64_CMD_READ_DATA,
        (uint8_t)(address >> 16),
        (uint8_t)(address >> 8),
        (uint8_t)address
    };

    HAL_StatusTypeDef status;

    // 3. 选中 Flash
    HAL_GPIO_WritePin(
        FLASH_CS_GPIO_Port,
        FLASH_CS_Pin,
        GPIO_PIN_RESET
    );

    // 4. 发送读取命令与地址
    status = HAL_SPI_Transmit(
        &hspi1,
        tx,
        4,
        100
    );

    // 5. 读取 Flash 数据
    if (status == HAL_OK)
    {
        status = HAL_SPI_Receive(
            &hspi1,
            data,
            size,
            100
        );
    }

    // 6. 结束 SPI 通信
    HAL_GPIO_WritePin(
        FLASH_CS_GPIO_Port,
        FLASH_CS_Pin,
        GPIO_PIN_SET
    );

    return status;
}

//static表示该函数只供w25q64.c内部使用，外部无法访问
//因为WaitReady()是驱动内部的辅助函数，我们暂时不能把它暴露给main.c使用
static HAL_StatusTypeDef W25Q64_WaitReady(uint32_t timeout_ms)
{
    uint32_t start = HAL_GetTick();
    uint8_t reg = 0;

    do
    {
        if (W25Q64_ReadStatus(&reg) != HAL_OK)
        {
            return HAL_ERROR;
        }

        if ((reg & 0x01) == 0)
        {
            return HAL_OK;
        }

    } while ((uint32_t)(HAL_GetTick() - start) < timeout_ms);

    return HAL_TIMEOUT;
}

HAL_StatusTypeDef W25Q64_PageProgram(
    uint32_t address,
    const uint8_t *data,
    uint16_t size)
{
    if (data == NULL || size == 0 || size > 256)
    {
        return HAL_ERROR;
    }

    if (address >= 0x800000UL ||
        size > (0x800000UL - address))
    {
        return HAL_ERROR;
    }

    // 不允许跨越 256 字节页边界
    if ((address % 256U) + size > 256U)
    {
        return HAL_ERROR;
    }

    // 等待芯片空闲
    HAL_StatusTypeDef status = W25Q64_WaitReady(100);

    if (status != HAL_OK)
    {
        return status;
    }

    // 写使能
    status = W25Q64_WriteEnable();

    if (status != HAL_OK)
    {
        return status;
    }

    // 确认 WEL 已经置位
    uint8_t reg = 0;

    status = W25Q64_ReadStatus(&reg);

    if (status != HAL_OK || (reg & 0x02) == 0)
    {
        return HAL_ERROR;
    }

    // 组织 0x02 命令和 24 位地址
    uint8_t cmd[4] = {
        W25Q64_CMD_PAGE_PROGRAM,
        (uint8_t)(address >> 16),
        (uint8_t)(address >> 8),
        (uint8_t)address
    };

    HAL_GPIO_WritePin(
        FLASH_CS_GPIO_Port,
        FLASH_CS_Pin,
        GPIO_PIN_RESET
    );

    // 先发送命令和地址
    status = HAL_SPI_Transmit(
        &hspi1,
        cmd,
        4,
        100
    );

    // 然后发送数据
    if (status == HAL_OK)
    {
        status = HAL_SPI_Transmit(
            &hspi1,
            (uint8_t *)data,
            size,
            100
        );
    }

    HAL_GPIO_WritePin(
        FLASH_CS_GPIO_Port,
        FLASH_CS_Pin,
        GPIO_PIN_SET
    );

    if (status != HAL_OK)
    {
        return status;
    }

    // 等待页编程完成
    return W25Q64_WaitReady(100);
}
HAL_StatusTypeDef W25Q64_SectorErase(uint32_t address)
{//1.检查地址是否非法
    if(address >= 0x800000UL)
    {
        return HAL_ERROR;
    }
//2.检查是否为4kib扇区起始地址
if((address % 4096U) != 0)
{
    return HAL_ERROR;
}

HAL_StatusTypeDef status;
    uint8_t reg = 0;

    // 3. 等待 Flash 空闲
    status = W25Q64_WaitReady(100);

    if (status != HAL_OK)
    {
        return status;
    }

    // 4. 写使能
    status = W25Q64_WriteEnable();

    if (status != HAL_OK)
    {
        return status;
    }

    // 5. 确认 WEL = 1
    status = W25Q64_ReadStatus(&reg);

    if (status != HAL_OK)
    {
        return status;
    }

    if ((reg & 0x02) == 0)
    {
        return HAL_ERROR;
    }

    // 6. 准备擦除命令和 24 位地址
    uint8_t cmd[4] = {
        W25Q64_CMD_SECTOR_ERASE,
        (uint8_t)(address >> 16),
        (uint8_t)(address >> 8),
        (uint8_t)address
    };

    // 7. 拉低 CS，选中 Flash
    HAL_GPIO_WritePin(
        FLASH_CS_GPIO_Port,
        FLASH_CS_Pin,
        GPIO_PIN_RESET
    );

    // 8. 发送 0x20 和扇区地址
    status = HAL_SPI_Transmit(
        &hspi1,
        cmd,
        4,
        100
    );

    // 9. 拉高 CS，启动内部擦除
    HAL_GPIO_WritePin(
        FLASH_CS_GPIO_Port,
        FLASH_CS_Pin,
        GPIO_PIN_SET
    );

    if (status != HAL_OK)
    {
        return status;
    }

    // 10. 等待擦除结束
    return W25Q64_WaitReady(2000);
}

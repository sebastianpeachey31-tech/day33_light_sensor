#ifndef W25Q64_H
#define W25Q64_H

#include "main.h"
 
#define W25Q64_CMD_READ_ID 0x9F
#define W25Q64_CMD_READ_STATUS 0X05
#define W25Q64_CMD_WRITE_ENABLE 0X06
#define W25Q64_CMD_WRITE_DISABLE 0X04
#define W25Q64_CMD_READ_DATA 0X03
#define W25Q64_CMD_PAGE_PROGRAM 0X02
#define W25Q64_CMD_SECTOR_ERASE 0X20

HAL_StatusTypeDef W25Q64_ReadID(uint8_t *id);
HAL_StatusTypeDef W25Q64_ReadStatus(uint8_t *status_reg);
HAL_StatusTypeDef W25Q64_WriteEnable(void);
HAL_StatusTypeDef W25Q64_WriteDisable(void);
HAL_StatusTypeDef W25Q64_ReadData(uint32_t address, uint8_t *data, uint16_t length);
HAL_StatusTypeDef W25Q64_PageProgram(uint32_t address, const uint8_t *data, uint16_t length);
HAL_StatusTypeDef W25Q64_SectorErase(uint32_t address);
#endif
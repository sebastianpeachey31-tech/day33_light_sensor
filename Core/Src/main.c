/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "light_sensor.h"//包含光照传感器头文件
#include <stdio.h>
#include <string.h>
#include "i2c_scanner.h"
#include "oled.h"
#include "w25q64.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
//第一步：定义OLED字符串缓冲区
//用于保存ADC整数转换后的文本
//16个字节足够保存uint16_t的十进制表示及结束符

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  uint32_t average=0;
  uint32_t moving_average=0;
  uint32_t oversampled=0;
  if(HAL_ADCEx_Calibration_Start(&hadc1)!=HAL_OK)
  {
    Error_Handler();
  }
  /*Day36:I2C address scanner*/
  uint8_t devices[8]={0};
  uint8_t device_count=I2C_Scan(&hi2c1,devices,8);
char scan_msg[64];
int scan_len = snprintf(scan_msg,sizeof(scan_msg),"I2C Scan:%u device(s)\r\n",(unsigned int)device_count);
if(scan_len<0||(size_t)scan_len>=sizeof(scan_msg))
{
  Error_Handler();
}
if(HAL_UART_Transmit(&huart1,(uint8_t*)scan_msg,(uint16_t)scan_len,100)!=HAL_OK)
{
  Error_Handler();
}
for (uint8_t i = 0; i < device_count; i++)
{
    scan_len = snprintf(
        scan_msg,
        sizeof(scan_msg),
        "I2C Found: 0x%02X\r\n",
        (unsigned int)devices[i]
    );

    if (scan_len < 0 || (size_t)scan_len >= sizeof(scan_msg))
    {
        Error_Handler();
    }

    if (HAL_UART_Transmit(
            &huart1,
            (uint8_t *)scan_msg,
            (uint16_t)scan_len,
            100
        ) != HAL_OK)
    {
        Error_Handler();
    }
}

//================ OLED初始化 ================

//第一步：声明OLED操作状态变量
HAL_StatusTypeDef oled_status;

//第二步：初始化OLED
//内部完成显示参数配置、清屏和开启显示
oled_status = OLED_Init(&hi2c1);

//第三步：检查OLED初始化是否成功
if(oled_status != HAL_OK)
{
    //OLED初始化失败
    //停止当前程序，防止后续继续访问异常设备
    Error_Handler();
}

//初始化成功后，不再显示固定字符串
//OLED实时刷新由while(1)负责

//第四步：通过串口输出OLED测试状态
//这里只报告I2C通信状态，不直接判断屏幕是否点亮
if(oled_status == HAL_OK)
{
    const char msg[] = "OLED TEST: I2C OK\r\n";
    HAL_UART_Transmit(
        &huart1,
        (uint8_t *)msg,
        sizeof(msg) - 1,
        100
    );
}
else
{
    const char msg[] = "OLED TEST: I2C ERROR\r\n";
    HAL_UART_Transmit(
        &huart1,
        (uint8_t *)msg,
        sizeof(msg) - 1,
        100
    );
}

//================ W25Q64 SPI通信测试 ================

//第一步：定义JEDEC ID接收数组
//W25Q64的JEDEC ID由3个字节组成
uint8_t jedec_id[3] = {0};

//第二步：通过SPI1读取Flash的JEDEC ID
HAL_StatusTypeDef flash_status;
flash_status = W25Q64_ReadID(jedec_id);

//第三步：检查SPI通信结果
if(flash_status != HAL_OK)
{
    //SPI通信失败
    Error_Handler();
}

//第四步：检查读取的设备型号
//你之前实测W25Q64的JEDEC ID为EF 40 17
if(jedec_id[0] != 0xEF ||
   jedec_id[1] != 0x40 ||
   jedec_id[2] != 0x17)
{
    //读回的ID不符合预期
    Error_Handler();
}

//第五步：输出读取成功的信息
const char flash_msg[] = "FLASH JEDEC ID: EF 40 17 OK\r\n";

HAL_StatusTypeDef flash_uart_status;

flash_uart_status = HAL_UART_Transmit(
    &huart1,
    (uint8_t *)flash_msg,
    sizeof(flash_msg) - 1,
    100
);

//第六步：检查UART发送是否成功
if(flash_uart_status != HAL_OK)
{
    Error_Handler();
}

//================ Flash存储区域检查 ================

//第一步：选择一个候选测试地址
//暂时只读取，不写入、不擦除
//这个地址不是已确认可以覆盖的区域
uint32_t flash_address = 0x002000;

//第二步：准备8字节接收缓冲区
//后续计划分别保存Raw和Avg
uint8_t flash_check[8] = {0};

//第三步：读取Flash原有数据
HAL_StatusTypeDef check_status;

check_status = W25Q64_ReadData(
    flash_address,
    flash_check,
    sizeof(flash_check)
);

//第四步：检查SPI读取结果
if(check_status != HAL_OK)
{
    //Flash读取失败
    Error_Handler();
}

//第五步：将读取结果输出到串口
char flash_log[100];

int log_len = snprintf(
    flash_log,
    sizeof(flash_log),
    "FLASH BEFORE: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
    flash_check[0],
    flash_check[1],
    flash_check[2],
    flash_check[3],
    flash_check[4],
    flash_check[5],
    flash_check[6],
    flash_check[7]
);

//第六步：检查字符串格式化结果
if(log_len < 0 || (size_t)log_len >= sizeof(flash_log))
{
    Error_Handler();
}

//第七步：通过UART输出Flash原有内容
HAL_StatusTypeDef log_status;

log_status = HAL_UART_Transmit(
    &huart1,
    (uint8_t *)flash_log,
    (uint16_t)log_len,
    100
);

//第八步：检查UART发送结果
if(log_status != HAL_OK)
{
    Error_Handler();
}
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    uint32_t light_value=0;
    HAL_StatusTypeDef status;
    HAL_StatusTypeDef tx_status;
    status=Light_ReadRaw(&light_value);
    HAL_StatusTypeDef status_avg=Light_ReadAverage(&average,10);
    HAL_StatusTypeDef status_moving_avg=Light_ReadMovingAverage(&moving_average);
    HAL_StatusTypeDef status_oversampled=Light_ReadOversampled(&oversampled);
    if(status==HAL_OK&&status_avg==HAL_OK&&status_moving_avg==HAL_OK&&status_oversampled==HAL_OK&&light_value<=4095)
    {char msg[96];
    int len = snprintf(msg, sizeof(msg),
         "Raw:%lu Avg:%lu Moving:%lu Over:%lu\r\n",
         (unsigned long)light_value,
         (unsigned long)average,
         (unsigned long)moving_average,
         (unsigned long)oversampled);
         if(len<0||(size_t)len>=sizeof(msg))
    {
      Error_Handler();
    }
         tx_status = HAL_UART_Transmit(
    &huart1,
    (uint8_t *)msg,
    (uint16_t)len,
    100
);
    
      if(tx_status != HAL_OK)
      {
       //uart发送失败时
        Error_Handler();
      }
//================ OLED双行实时显示ADC数据 ================

//第一步：定义两个字符串缓冲区
//每个缓冲区最多保存15个可见字符及结束符
char raw_buffer[16];
char avg_buffer[16];

//第二步：将ADC原始值转换为固定宽度字符串
//RAW:为标签，%04lu表示数值至少占4位，不足补0
//例如：light_value=3237，得到"RAW:3237"
int raw_len = snprintf(
    raw_buffer,
    sizeof(raw_buffer),
    "RAW:%04lu",
    (unsigned long)light_value
);

//第三步：将ADC平均值转换为固定宽度字符串
//例如：average=3235，得到"AVG:3235"
int avg_len = snprintf(
    avg_buffer,
    sizeof(avg_buffer),
    "AVG:%04lu",
    (unsigned long)average
);

//第四步：检查两次字符串格式化是否成功
//snprintf返回负数表示格式化失败
//返回值大于等于缓冲区容量表示发生截断
if(raw_len < 0 ||
   (size_t)raw_len >= sizeof(raw_buffer) ||
   avg_len < 0 ||
   (size_t)avg_len >= sizeof(avg_buffer))
{
    //字符串格式化失败
    Error_Handler();
}
else
{
    //第五步：声明OLED通信状态变量
    HAL_StatusTypeDef oled_status;

    //第六步：设置RAW显示位置
    //Page 2：对应OLED第16~23行
    //Column 0：从最左侧开始显示
    oled_status = OLED_SetPosition(&hi2c1, 2, 0);

    //第七步：检查RAW定位结果
    if(oled_status != HAL_OK)
    {
        //I2C定位命令发送失败
        Error_Handler();
    }
    else
    {
        //第八步：写入RAW字符串
        oled_status = OLED_ShowString(
            &hi2c1,
            raw_buffer
        );

        //第九步：检查RAW字符串发送结果
        if(oled_status != HAL_OK)
        {
            //RAW显示数据发送失败
            Error_Handler();
        }
        else
        {
            //第十步：设置AVG显示位置
            //Page 4：对应OLED第32~39行
            //Column 0：从最左侧开始显示
            oled_status = OLED_SetPosition(&hi2c1, 4, 0);

            //第十一步：检查AVG定位结果
            if(oled_status != HAL_OK)
            {
                //I2C定位命令发送失败
                Error_Handler();
            }
            else
            {
                //第十二步：写入AVG字符串
                oled_status = OLED_ShowString(
                    &hi2c1,
                    avg_buffer
                );

                //第十三步：检查AVG字符串发送结果
                if(oled_status != HAL_OK)
                {
                    //AVG显示数据发送失败
                    Error_Handler();
                }
            }
        }
    }
}
//================ W25Q64真实ADC数据存储 ================

//第一步：定义本次启动的写入标志
//static变量不会随着while循环反复初始化
//0表示尚未尝试写入，1表示本次启动已经尝试过
static uint8_t flash_attempted = 0;

//第二步：每次启动最多尝试写入一次
if(flash_attempted == 0)
{
    //先设置标志，防止本次启动重复执行
    flash_attempted = 1;

    //第三步：指定Day36专用的8字节存储区域
    //使用之前已检查的候选地址
    const uint32_t address = 0x002000;

    //第四步：准备写入、检查和读回数组
    uint8_t write_data[8] = {0};
    uint8_t before_data[8] = {0};
    uint8_t verify_data[8] = {0};

    HAL_StatusTypeDef flash_status;
    const char *flash_message = "FLASH ERROR\r\n";

    //第五步：再次读取目标地址原有内容
    flash_status = W25Q64_ReadData(
        address,
        before_data,
        sizeof(before_data)
    );

    //第六步：检查读取是否成功
    if(flash_status != HAL_OK)
    {
        flash_message = "FLASH READ ERROR\r\n";
    }
    else
    {
        //第七步：检查8字节是否全部为FF
        uint8_t is_empty = 1;

        for(uint8_t i = 0; i < 8; i++)
        {
            if(before_data[i] != 0xFF)
            {
                is_empty = 0;
                break;
            }
        }

        if(is_empty == 0)
        {//================ 输出Flash已经保存的ADC数据 ================

//第一步：还原前4字节中的RAW
uint32_t saved_raw =
    ((uint32_t)before_data[0]) |
    ((uint32_t)before_data[1] << 8) |
    ((uint32_t)before_data[2] << 16) |
    ((uint32_t)before_data[3] << 24);

//第二步：还原后4字节中的AVG
uint32_t saved_avg =
    ((uint32_t)before_data[4]) |
    ((uint32_t)before_data[5] << 8) |
    ((uint32_t)before_data[6] << 16) |
    ((uint32_t)before_data[7] << 24);

//第三步：将整数转换为串口字符串
char saved_msg[80];

int saved_len = snprintf(
    saved_msg,
    sizeof(saved_msg),
    "FLASH SAVED RAW:%lu AVG:%lu\r\n",
    (unsigned long)saved_raw,
    (unsigned long)saved_avg
);

//第四步：检查字符串格式化是否成功
if(saved_len < 0 ||
   (size_t)saved_len >= sizeof(saved_msg))
{
    Error_Handler();
}

//第五步：通过USART1输出
HAL_StatusTypeDef saved_status = HAL_UART_Transmit(
    &huart1,
    (uint8_t *)saved_msg,
    (uint16_t)saved_len,
    100
);

//第六步：检查串口发送状态
if(saved_status != HAL_OK)
{
    Error_Handler();
}
            //已有数据，跳过写入，防止覆盖
            flash_message = "FLASH NOT EMPTY, SKIPPED\r\n";
        }
        else
        {
            //第八步：保存本轮ADC原始值和平均值
            uint32_t raw = (uint32_t)light_value;
            uint32_t avg = (uint32_t)average;

            //第九步：将两个32位整数转换成8个字节
            //前4字节保存RAW，后4字节保存AVG
            //采用低字节在前的存储顺序
            for(uint8_t i = 0; i < 4; i++)
            {
                write_data[i] =
                    (uint8_t)(raw >> (8U * i));

                write_data[i + 4] =
                    (uint8_t)(avg >> (8U * i));
            }

            //第十步：执行一次Flash页编程
            flash_status = W25Q64_PageProgram(
                address,
                write_data,
                sizeof(write_data)
            );

            //第十一步：检查页编程结果
            if(flash_status != HAL_OK)
            {
                flash_message = "FLASH PROGRAM ERROR\r\n";
            }
            else
            {
                //第十二步：读取刚写入的8个字节
                flash_status = W25Q64_ReadData(
                    address,
                    verify_data,
                    sizeof(verify_data)
                );

                //第十三步：检查读回通信状态
                if(flash_status != HAL_OK)
                {
                    flash_message = "FLASH VERIFY READ ERROR\r\n";
                }
                else
                {
                    //第十四步：逐字节比较写入和读回数据
                    if(memcmp(write_data, verify_data, 8) == 0)
                    {
                        flash_message = "FLASH VERIFY OK\r\n";
                    }
                    else
                    {
                        flash_message = "FLASH VERIFY FAIL\r\n";
                    }
                }
            }
        }
    }

    //第十五步：通过UART报告最终结果
    HAL_StatusTypeDef uart_status;

    uart_status = HAL_UART_Transmit(
        &huart1,
        (uint8_t *)flash_message,
        (uint16_t)strlen(flash_message),
        100
    );

    //第十六步：检查UART发送状态
    if(uart_status != HAL_OK)
    {
        Error_Handler();
    }
}
    }
    else
   {//ADC采集失败时执行
     char error_msg[]="ADC ERROR\r\n";
     tx_status=HAL_UART_Transmit(&huart1,(uint8_t*)error_msg,strlen(error_msg),100);
     if(tx_status != HAL_OK)
      {
       //uart发送失败时
        Error_Handler();
      }

    }
    HAL_Delay(500);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : FLASH_CS_Pin */
  GPIO_InitStruct.Pin = FLASH_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(FLASH_CS_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

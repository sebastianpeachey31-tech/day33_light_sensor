# Day40 STM32 实机调试记录

## 1. 实验环境

- MCU：STM32F103C8T6
- 调试器：CMSIS-DAP（SWD）
- 调试工具：OpenOCD 0.12.0+dev、ARM GDB 16.3
- 调试工程：Day36 数据采集器

## 2. 实机调试结果

成功通过 OpenOCD 连接 STM32 Cortex-M3，GDB 连接本机 3333 端口。

实际观察到：

- PC = 0x080011A0
- SP = 0x20004DF0
- LR = 0x0800119F

通过 ELF 调试符号定位：

- HAL_Delay()：stm32f1xx_hal.c 第382行
- main() 调用位置：main.c 第639行
- Delay = 500
- tickstart = 221880
- wait = 501

## 3. Watchpoint 故障排查

**观察现象：**

light_value 周期性从正常 ADC 数值变成 0。

**实测记录：**

| Old value | New value | 暂停位置 |
|---|---|---|
| 3360 | 0 | Light_ReadRaw 第6行 |
| 0 | 3426 | Light_ReadRaw 第33行 |
| 3426 | 0 | Light_ReadRaw 第6行 |
| 0 | 3440 | Light_ReadRaw 第33行 |
| 3440 | 0 | Light_ReadRaw 第6行 |
| 0 | 3437 | Light_ReadRaw 第33行 |

**地址验证：**

- raw = 0x20004E20
- &light_value = 0x20004E20
- SP = 0x20004DF0

**提出的两个故障假设：**

1. 函数调用及栈操作造成异常写入。
2. 主循环中的变量初始化或编译优化后的指令造成写零现象。

**排查发现：**

main.c 第320行存在：

`uint32_t light_value = 0;`

该语句位于 `while(1)` 循环内部。

light_sensor.c 第30行存在：

`*raw = temp_raw;`

重复实验发现，数值先变成 0，再恢复为正常的 ADC 采样值，与循环初始化及采样写回的行为一致。

**阶段结论：**

周期性清零可以由循环初始化合理解释，未发现能够证明栈溢出或非法内存覆盖的证据。

尚未进行完整的指令级归因，因此不宣称已严格确认每次写零对应的机器指令。

## 4. HardFault 基础

理解 HardFault 是 Cortex-M3 的异常处理机制。初步掌握通过 PC、SP、LR、Fault 状态寄存器和异常栈帧定位故障的思路。

本日未实际触发 HardFault。

## 5. 总结

掌握 OpenOCD 与 ARM GDB 的 STM32 实机调试流程，能够读取 CPU 寄存器、查看局部变量、使用调用栈、设置硬件监视点，并通过重复实验与源码分析排查异常现象。
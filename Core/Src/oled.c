#include "oled.h"

//向OLED发送命令
HAL_StatusTypeDef OLED_WriteCommand(I2C_HandleTypeDef *hi2c, 
uint8_t command) 
{
    if (hi2c == NULL) {
        return HAL_ERROR;
    }
   uint8_t buffer[2] ={0x00, command};
   return HAL_I2C_Master_Transmit(hi2c, OLED_ADDR, buffer, sizeof(buffer), 100);
}
   // 向 OLED 发送显示数据
HAL_StatusTypeDef OLED_WriteData(
    I2C_HandleTypeDef *hi2c,
    uint8_t data
)
{
    if (hi2c == NULL)
    {
        return HAL_ERROR;
    }

    uint8_t buffer[2] = {
        0x40,
        data
    };

    return HAL_I2C_Master_Transmit(
        hi2c,
        OLED_ADDR,
        buffer,
        sizeof(buffer),
        100
    );
}

HAL_StatusTypeDef OLED_Init(I2C_HandleTypeDef *hi2c)
{
  if(hi2c ==NULL)
 {
    return HAL_ERROR;
 }
//第一步；关闭OLED显示输出
HAL_StatusTypeDef status;
status = OLED_WriteCommand(hi2c, 0xAE);//关闭显示
if(status != HAL_OK)
{
    return status;
}

//第二步；接下来配置显示时钟
status = OLED_WriteCommand(hi2c, 0xD5);//修改哪个配置
//给芯片说，我要修改显示时钟
if(status != HAL_OK)
{
    return status;}
//第三步；提供具体的时钟配置参数
status = OLED_WriteCommand(hi2c, 0x80);//提供具体的时钟配置参数
//0x80的高四位1000，振荡器频率的配置档位是8，不是8MHZ
//低四位是0000.时钟分频比是1.实际分频系数=低四位设置值+1=0+1=1
if(status != HAL_OK)
{
    return status;}
//第四步；设置OLED扫描复用比
status = OLED_WriteCommand(hi2c, 0xA8);//告诉芯片，我要修改扫描复用比这个配置
if(status !=HAL_OK)
{
    return status;
}
//第五步；提供具体的扫描复用比参数
//0x3F = 00111111.十进制为63
//实际复用比=参数值+1=63+1=64，所以配置为64MUX
status=OLED_WriteCommand(hi2c,0x3F);
if(status != HAL_OK)
{
    return status;
}
//第六步：告诉OLED接下来要设置显示偏移
//0xD3是显示偏移设置命令
status = OLED_WriteCommand(hi2c, 0xD3);

if(status != HAL_OK)
{
    return status;
}

//第七步：提供具体的显示偏移参数
//0x00表示显示偏移为0行
//即不额外偏移显示起始位置
status = OLED_WriteCommand(hi2c, 0x00);

if(status != HAL_OK)
{
    return status;
}
//第八步：设置OLED显示起始行
//0x40表示设置显示起始行为第0行
//这是一条完整命令，不需要额外发送参数
status = OLED_WriteCommand(hi2c, 0x40);

if(status != HAL_OK)
{
    return status;
}

//第九步：设置OLED显存寻址模式
//0x20表示接下来要配置显存寻址方式
status = OLED_WriteCommand(hi2c, 0x20);

if(status != HAL_OK)
{
    return status;
}

//第十步：选择页寻址模式
//0x02 = 00000010，低两位是10
//00：水平寻址；01：垂直寻址；10：页寻址
//选择页寻址，方便按Page和Column定位像素数据
status = OLED_WriteCommand(hi2c, 0x02);

if(status != HAL_OK)
{
    return status;
}

//第十一步：设置OLED的SEG列映射方向
//SEG是OLED的列驱动输出
//0xA0表示正常列映射
//0xA1表示反向列映射
//这里先使用0xA1，适配常见128x64 OLED模块的显示方向
//该命令不需要额外参数
status = OLED_WriteCommand(hi2c, 0xA1);

if(status != HAL_OK)
{
    return status;
}

//第十二步：设置OLED的COM行扫描方向
//COM是OLED的行扫描驱动输出
//0xC0：正常扫描，从COM0扫描到COM63
//0xC8：反向扫描，从COM63扫描到COM0
//这里先使用0xC8，适配常见128x64 OLED模块的显示方向
//该命令不需要额外参数
status = OLED_WriteCommand(hi2c, 0xC8);

if(status != HAL_OK)
{
    return status;
}

//第十三步：设置OLED的COM引脚硬件配置
//0xDA表示接下来要配置COM引脚的硬件连接方式
//这与之前0xC8设置的COM扫描方向不同
status = OLED_WriteCommand(hi2c, 0xDA);

if(status != HAL_OK)
{
    return status;
}

//第十四步：设置具体的COM引脚配置参数
//0x12 = 00010010
//bit4 = 1：使用Alternative COM引脚配置
//bit5 = 0：禁止COM左右重映射
//其余位按数据手册要求设置
//这是常见128x64 OLED模块采用的配置
status = OLED_WriteCommand(hi2c, 0x12);

if(status != HAL_OK)
{
    return status;
}

//第十五步：设置OLED对比度
//0x81表示接下来要设置对比度控制参数
//对比度与OLED的SEG输出电流有关
//这条命令后面需要跟一个参数
status = OLED_WriteCommand(hi2c, 0x81);

if(status != HAL_OK)
{
    return status;
}

//第十六步：提供具体的对比度参数
//0x7F = 01111111，十进制为127
//SSD1315有效参数范围为0x01~0xFF
//数值越大，对比度通常越高
//这里使用0x7F，即芯片的复位默认值
status = OLED_WriteCommand(hi2c, 0x7F);

if(status != HAL_OK)
{
    return status;
}

//第十七步：设置OLED的预充电周期
//0xD9表示接下来要配置预充电周期
//预充电周期分为Phase 1和Phase 2两个阶段
//时间单位是OLED内部的显示时钟周期DCLK
status = OLED_WriteCommand(hi2c, 0xD9);

if(status != HAL_OK)
{
    return status;
}

//第十八步：设置具体的预充电周期参数
//0x22 = 00100010
//高四位0010 = 2：设置Phase 2
//低四位0010 = 2：设置Phase 1
//按照SSD1315的编码规则，各对应4个DCLK周期
//0x22也是SSD1315的复位默认配置
status = OLED_WriteCommand(hi2c, 0x22);

if(status != HAL_OK)
{
    return status;
}

//第十九步：设置OLED的VCOMH电压等级
//0xDB表示接下来要配置COM非选中电压
//VCOMH用于设置COM行处于非选中状态时的电压等级
//该命令后面需要跟一个参数
status = OLED_WriteCommand(hi2c, 0xDB);

if(status != HAL_OK)
{
    return status;
}

//第二十步：提供具体的VCOMH参数
//0x20 = 00100000
//bit5 = 1，bit4 = 0，即A[5:4] = 10
//对应的VCOMH非选中电压约为0.77倍VCC
//0x20为SSD1315的复位默认配置值
status = OLED_WriteCommand(hi2c, 0x20);

if(status != HAL_OK)
{
    return status;
}

//第二十一步：设置OLED内部电荷泵
//0x8D表示接下来要配置Charge Pump
//电荷泵用于产生OLED面板所需的驱动电压
//这条命令后面需要跟一个参数
status = OLED_WriteCommand(hi2c, 0x8D);

if(status != HAL_OK)
{
    return status;
}

//第二十二步：提供具体的电荷泵参数
//0x14 = 00010100
//bit2 = 1，表示启用内部电荷泵
//bit7 = 0、bit0 = 0，选择7.5V输出模式
//该电压是芯片内部面板驱动电压，不是模块VDD供电电压
status = OLED_WriteCommand(hi2c, 0x14);

if(status != HAL_OK)
{
    return status;
}

//第二十三步：设置OLED按照显存内容显示
//0xA4表示恢复由GDDRAM显存控制显示
//显存位为1或0，将根据正常/反色显示模式决定像素状态
//0xA5表示强制整个显示区域点亮，忽略显存内容
//这里选择0xA4，方便后续显示文字和传感器数据
//该命令不需要额外参数
status = OLED_WriteCommand(hi2c, 0xA4);

if(status != HAL_OK)
{
    return status;
}

//第二十四步：设置OLED正常显示模式
//0xA6表示正常显示模式
//显存中的bit为1，对应像素点亮
//显存中的bit为0，对应像素熄灭
//0xA7表示反色显示，亮灭关系相反
//这里选择0xA6，方便后续正常显示文字
//该命令不需要额外参数
status = OLED_WriteCommand(hi2c, 0xA6);

if(status != HAL_OK)
{
    return status;
}

//第二十五步：设置OLED的IREF参考电流
//0xAD表示接下来选择参考电流来源
//IREF用于给SEG像素驱动电流提供参考
//支持内部IREF和外部IREF两种方案
status = OLED_WriteCommand(hi2c, 0xAD);

if(status != HAL_OK)
{
    return status;
}

//第二十六步：设置具体的IREF参数
//0x10 = 00010000
//bit4 = 1：启用内部IREF
//bit5 = 0：选择19uA内部参考电流档位
//注意：仅适用于使用内部IREF的模块
status = OLED_WriteCommand(hi2c, 0x10);

if(status != HAL_OK)
{
    return status;
}


//第二十七步：清空OLED显存
//OLED_Clear会依次清除8个Page
//每个Page写入128个0x00
//先清屏，再开启显示，避免显示未初始化内容
status = OLED_Clear(hi2c);

if(status != HAL_OK)
{
    return status;
}
//第二十七步：开启OLED显示输出
//0xAF表示Display ON，即开启显示
//与第一步的0xAE关闭显示相对应
//此时OLED按照之前配置的显示参数工作
//0xAF是一条完整命令，不需要额外参数
//注意：只有确认供电及IREF配置适合模块后才能实际运行
status = OLED_WriteCommand(hi2c, 0xAF);

if(status != HAL_OK)
{
    return status;
}

//全部初始化命令成功发送后，返回HAL_OK
return HAL_OK;
}

HAL_StatusTypeDef OLED_SetPosition(
    I2C_HandleTypeDef *hi2c,
    uint8_t page,
    uint8_t column
)
{
    //第一步：检查I2C指针是否为空
    //检查页号和列号是否超出128x64 OLED的范围
    //Page范围：0~7
    //Column范围：0~127
    if(hi2c == NULL || page >= 8 || column >= 128)
    {
        return HAL_ERROR;
    }

    //第二步：选择OLED显存页地址
    //0xB0~0xB7分别代表Page 0~Page 7
    //0xB0 | page：将页号组合到页地址命令中
    HAL_StatusTypeDef status;

    status = OLED_WriteCommand(hi2c, 0xB0 | page);

    if(status != HAL_OK)
    {
        return status;
    }

    //后面继续添加列地址配置

    //第三步：设置OLED列地址的低四位
//0x00~0x0F是设置列地址低四位的命令
//column & 0x0F：提取列地址的低四位
//0x00 | (column & 0x0F)：组合成完整的OLED命令
status = OLED_WriteCommand(
    hi2c,
    0x00 | (column & 0x0F)
);

if(status != HAL_OK)
{
    return status;
}
//第四步：设置OLED列地址的高四位
//0x10~0x1F是设置列地址高四位的命令
//column >> 4：将列地址右移4位，提取高四位
//0x10 | (column >> 4)：组合成完整的OLED命令
status = OLED_WriteCommand(
    hi2c,
    0x10 | (column >> 4)
);

if(status != HAL_OK)
{
    return status;
}


//第五步：页地址和列地址全部设置成功
//只有前面三条命令均发送成功，才执行到这里
return HAL_OK;
}

HAL_StatusTypeDef OLED_Clear(I2C_HandleTypeDef *hi2c)
{
    //第一步：检查I2C句柄指针
    if(hi2c == NULL)
    {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status;

    //第二步：依次清除OLED的8个Page
    //page从0开始，每次加1，直到Page 7
    for(uint8_t page = 0; page < 8; page++)
    {
        //第三步：将写入位置设置为当前页的第0列
        status = OLED_SetPosition(hi2c, page, 0);

        if(status != HAL_OK)
        {
            return status;
        }

       //第四步：依次清除当前Page的128列
//column从0开始，每次加1，直到第127列
//当前处于页寻址模式，连续写入时列地址自动递增
for(uint8_t column = 0; column < 128; column++)
{
    //第五步：向当前列写入0x00
    //0x00 = 00000000
    //一个字节对应当前Page、当前列的8个像素位
    //正常显示模式下，8个位全部为0表示全部熄灭
    status = OLED_WriteData(hi2c, 0x00);

    //第六步：检查本次I2C数据发送是否成功
    //如果通信失败，立即结束清屏并返回错误状态
    if(status != HAL_OK)
    {
        return status;
    }
}
    }

    //全部清屏操作完成后再返回HAL_OK
    return HAL_OK;
}



//向OLED当前位置写入一个6x8字模
HAL_StatusTypeDef OLED_WriteGlyph6x8(
    I2C_HandleTypeDef *hi2c,
    const uint8_t font[6]
)
{
    //第一步：检查指针是否有效
    if(hi2c == NULL || font == NULL)
    {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status;

    //第二步：依次发送6列字模数据
    for(uint8_t i = 0; i < 6; i++)
    {
        //第三步：发送当前列的像素数据
        status = OLED_WriteData(hi2c, font[i]);

        //第四步：检查发送结果
        if(status != HAL_OK)
        {
            return status;
        }
    }

    //第五步：全部发送成功
    return HAL_OK;
}


//根据传入字符，查找对应的6x8字模并显示
//当前只支持大写字母H和I
HAL_StatusTypeDef OLED_ShowChar(
    I2C_HandleTypeDef *hi2c,
    char ch
)
{
    //第一步：检查I2C句柄是否有效
    if(hi2c == NULL)
    {
        return HAL_ERROR;
    }

    //第二步：定义二维字模数组
    //第一维[2]：表示存放2个字符
    //第二维[6]：表示每个字符占6个字节
    //第0行：字母H
    //第1行：字母I
    static const uint8_t font_6x8[2][6] =
    {
        {0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00}, // H
        {0x00, 0x41, 0x7F, 0x41, 0x00, 0x00}  // I
    };
//数字0~9的6x8字模表
//第一维[10]：共有10个数字
//第二维[6]：每个数字由6个字节组成
//每一行对应一个数字，按0~9排列
    static const uint8_t digit_font[10][6]=
    {
     {0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46, 0x00}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31, 0x00}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10, 0x00}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39, 0x00}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30, 0x00}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03, 0x00}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36, 0x00}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E, 0x00}  // 9
    };

    //第一步：定义OLED常用字符的6x8字模
//第一维[6]：表示6个字符
//第二维[6]：每个字符占6列
//依次对应：R、A、W、V、G、:
//最后一个0x00表示字符间隔列
static const uint8_t label_font[6][6] =
{
    {0x7F, 0x09, 0x19, 0x29, 0x46, 0x00}, // R
    {0x7E, 0x11, 0x11, 0x11, 0x7E, 0x00}, // A
    {0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00}, // W
    {0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00}, // V
    {0x3E, 0x41, 0x49, 0x49, 0x7A, 0x00}, // G
    {0x00, 0x36, 0x36, 0x00, 0x00, 0x00}  // :
};
    //第一步：声明一个字模指针
//font用于记录最终选择的字模地址
const uint8_t *font = NULL;

//第二步：判断输入字符
if(ch == 'H')
{
    //选择H的字模
    font = font_6x8[0];
}
else if(ch == 'I')
{
    //选择I的字模
    font = font_6x8[1];
}
//第一步：判断输入是否是数字字符
else if(ch >= '0' && ch <= '9')
{
    //将数字字符转换为数组下标
    uint8_t index = (uint8_t)(ch - '0');

    //选择对应数字字模
    font = digit_font[index];
}
//第二步：判断是否为常用标签字符
else if(ch == 'R')
{
    font = label_font[0];
}
else if(ch == 'A')
{
    font = label_font[1];
}
else if(ch == 'W')
{
    font = label_font[2];
}
else if(ch == 'V')
{
    font = label_font[3];
}
else if(ch == 'G')
{
    font = label_font[4];
}
else if(ch == ':')
{
    font = label_font[5];
}
else
{
    //不支持的字符直接返回错误
    return HAL_ERROR;
}


//第三步：调用底层字模写入函数
//OLED_WriteGlyph6x8内部逐个检查HAL发送状态
HAL_StatusTypeDef status;

status = OLED_WriteGlyph6x8(hi2c, font);

    //第六步：检查字模写入结果
    //底层函数已经逐字节检查HAL传输状态
    if(status != HAL_OK)
    {
        return status;
    }

    //第七步：字符显示数据发送成功
    return HAL_OK;
}
//OLED字符串显示函数
//作用：依次读取字符串中的字符并显示在OLED上
HAL_StatusTypeDef OLED_ShowString(
    I2C_HandleTypeDef *hi2c,
    const char *str
)
{
    //第一步：检查指针参数
    //避免访问空指针
    if(hi2c == NULL || str == NULL)
    {
        return HAL_ERROR;
    }

    //第二步：定义HAL状态变量
    HAL_StatusTypeDef status;

    //第三步：从字符串第一个字符开始读取
    //只要当前字符不是结束符'\0'，就继续循环
    while(*str != '\0')
    {
        //第四步：显示当前字符
        //*str表示读取指针当前指向的字符
        status = OLED_ShowChar(hi2c, *str);

        //第五步：检查当前字符是否显示成功
        //若底层I2C通信失败，或字符不支持
        //立即返回错误状态，不再发送后续字符
        if(status != HAL_OK)
        {
            return status;
        }

        //第六步：指针移动到下一个字符
        str++;
    }

    //第七步：所有字符都成功发送
    return HAL_OK;
}
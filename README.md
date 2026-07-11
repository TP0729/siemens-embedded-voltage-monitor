# 2026 Siemens Cup Embedded Voltage Monitoring System

这是 2026 西门子杯相关的嵌入式系统设计成果整理版。项目基于 GD32F470 平台，包含应用层固件、BootLoader 固件、通信协议解析、ADC/DAC 外设驱动、OLED 显示、Flash 参数保存以及可烧录 hex 成果文件。

> 说明：本仓库是比赛成果归档与展示版本，保留了 Keil/IAR/GD32 工程文件和主要源码，已排除大量编译中间文件。

## 项目亮点

- BootLoader + APP 双固件结构，APP 起始地址配置为 `0x08011000`。
- APP 侧通过 `SCB->VTOR` 重定位中断向量表，适配 BootLoader 跳转启动。
- 支持 USART 通信协议解析，帧头/帧尾为 `A5B6` / `B6A5`。
- 通信帧使用 CRC-16-Modbus 校验，具备错误帧响应逻辑。
- 支持设备 ID、波特率、通道系数、告警阈值等参数的 Flash 保存与读取。
- 集成 ADC、DAC、AD3344 外部 ADC、SPI Flash、OLED、定时器、USART 等驱动模块。
- 提供 APP 与 BootLoader 两个可烧录 hex 文件，方便快速验证。

## 仓库结构

```text
.
├── binaries/
│   ├── app_project.hex
│   └── bootloader_project.hex
├── docs/
│   ├── 2026年.docx
│   └── firmware-notes.md
├── firmware/
│   ├── app/
│   └── bootloader/
├── .gitignore
└── README.md
```

## 固件组成

### APP

路径：`firmware/app`

APP 固件负责主要业务逻辑，包括：

- 系统初始化：LED、定时器、OLED、ADC、DAC、GD25 Flash、USART、AD3344。
- OLED 状态显示。
- GD25 Flash 配置读取：设备 ID、波特率、通道校准系数、告警阈值。
- USART 协议帧接收、解析与响应。
- 外部 ADC 采样与数据上报。
- DAC 输出设置与通道电压计算。
- 根据命令修改设备参数并写入 Flash。

关键文件：

- `firmware/app/User/main.c`
- `firmware/app/Protocol/Protocol.c`
- `firmware/app/Driver/ADC.c`
- `firmware/app/Driver/DAC.c`
- `firmware/app/Driver/SPI_AD3344.c`
- `firmware/app/Driver/SPI_FLASH.c`
- `firmware/app/Driver/USART.c`

### BootLoader

路径：`firmware/bootloader`

BootLoader 固件负责从指定 APP 地址跳转到应用程序：

- APP 起始地址：`0x08011000`
- 读取 APP 栈顶地址和复位入口。
- 关闭中断和 SysTick。
- 设置 MSP。
- 设置中断向量表偏移。
- 跳转到 APP Reset_Handler。

关键文件：

- `firmware/bootloader/Function/BootLoader.c`
- `firmware/bootloader/User/main.c`

## 通信协议概览

APP 侧通信帧解析位于：

```text
firmware/app/Protocol/Protocol.c
```

协议特征：

- ASCII 十六进制帧解析。
- 帧头：`A5B6`
- 帧尾：`B6A5`
- 包含设备 ID、帧类型、命令字、数据长度、数据域和 CRC。
- CRC 算法：CRC-16-Modbus。
- 支持错误帧识别与错误响应。

部分命令逻辑在 `firmware/app/User/main.c` 中处理，例如：

- 设备复位。
- 设备 ID 修改。
- 参数查询/配置。
- 外部 ADC 采样控制。
- DAC 输出与阈值相关配置。

## 烧录文件

可烧录成果文件位于：

```text
binaries/app_project.hex
binaries/bootloader_project.hex
```

烧录时需要注意 APP 的链接地址和 BootLoader 的跳转地址保持一致：

```text
APP_START_ADDR = 0x08011000
```

## 开发环境

工程中保留了多种 IDE/工程文件：

- Keil uVision：`.uvprojx`
- IAR EWARM：`.ewp` / `.eww`
- GD32EBuilder：`.project` / `.cproject`

主要芯片/平台：

- GD32F470 / GD32470V-START
- GD32F4xx Firmware Library

## 适合展示的能力点

这个项目可以在简历或复试中突出以下能力：

- 嵌入式 C 工程组织。
- BootLoader 与 APP 分区启动。
- Cortex-M 中断向量表重定位。
- USART 自定义协议设计。
- CRC 校验与帧解析。
- SPI Flash 参数持久化。
- ADC/DAC 外设驱动与数据采集。
- OLED 人机交互显示。
- Keil/IAR/GD32 工程使用与调试。

## 备注

GD32 官方库文件保留在工程中，相关版权和许可归原作者/厂商所有。本仓库主要用于比赛成果归档、学习复盘与项目展示。

# FOC 控制器 (FOC learning project)

这是一个以学习和实验为导向的开源项目，目标是基于 STM32F4 系列 MCU 实现矢量控制 (Field Oriented Control, FOC) 的电机驱动与控制算法，同时集成传感器读取、实时操作系统和命令行调试交互界面。

本仓库包含硬件驱动、FOC 算法实现、外设初始化代码和部分演示/测试代码，适合用于教学、原型开发与二次开发。

---

## 硬件实拍

下面展示部分样例：
![1](/img/img0.jpg "Magic Gardens")
![3](/img/img2.jpg "Magic Gardens")
![4](/img/img3.jpg "Magic Gardens")
![5](/img/img4.jpg "Magic Gardens")
![6](/img/img5.jpg "Magic Gardens")
![7](/img/img6.jpg "Magic Gardens")

---

## 主要特性

- 基于 STM32F407 MCU（STM32F4 系列）
- 使用 STM32 HAL 驱动外设（GPIO、SPI、I2C、TIM、USART 等）
- 使用 FreeRTOS 作为实时操作系统，任务调度、信号量和流缓冲用于任务间通信
- 支持 MT6701 磁编码器的 SSI / I2C 读取驱动（示例实现）
- 集成一个基于 letter-shell 的串行命令行（shell）以便交互式调试
- 使用 CMake 构建（项目自带 CMakeLists.txt 与 CMakePresets）

---

## 技术栈

- 语言：C (主要代码)、部分汇编启动代码
- 工具链：GNU Arm Embedded Toolchain (arm-none-eabi-gcc)
- 构建系统：CMake + Ninja（可通过 IDE 或命令行构建）
- 实时操作系统：FreeRTOS
- 硬件抽象：STM32Cube HAL
- 第三方库：letter-shell（串口 shell）、CMSIS、CMSIS-DSP

---

## 目录结构（简要）

- `Core/` - 程序核心代码（`Inc/` 头文件，`Src/` 源文件）
- `Drivers/` - STM32 HAL 与 CMSIS 驱动
- `FOC/` - FOC 算法实现（`foc.c`, `foc.h`）
- `HwDriver/mt6701/` - MT6701 角度传感器驱动与平台移植代码
- `Third_Party/` / `3dparty/` - 第三方库（如 letter-shell）
- `img/` - 硬件照片（用于 README 展示）
- `CMakeLists.txt`, `CMakePresets.json` - 构建配置

---

## 如何构建（本地开发机 / 命令行）

前提条件：已安装 GNU Arm 工具链（arm-none-eabi-gcc/arm-none-eabi-ld）、CMake、Ninja。

示例命令（在 Windows Bash 或类 Unix 环境下）：

```bash
# 创建构建目录
mkdir -p build && cd build
# 使用 CMake 生成工程（Debug 为例）
cmake -S .. -B . -G Ninja -DCMAKE_BUILD_TYPE=Debug
# 构建
ninja
```

构建成功后，生成文件位于 `build/` 目录（具体输出依 CMakePresets 与目标硬件而异）。

---

## 在目标板上运行

- 使用 ST-LINK 或其他调试器将生成的固件 (.elf/.bin) 烧录到 STM32F4 板上。
- 通过串口连接（USART2 默认）打开 shell 调试界面，与板子进行交互。
- 在上电或重启时，观察串口输出确认外设初始化成功。

---

## 常见开发任务

- 修改 MT6701 驱动：文件位于 `HwDriver/mt6701/`，分为移植层 `mt6701_port.c`（平台相关）和驱动逻辑 `mt6701.c`。
- 添加 shell 命令：参考 `3dparty/letter-shell-shell3.1/src/` 扩展命令列表。
- 参数调试：可使用 shell 导出的变量（见 `SHELL_EXPORT_VAR`）进行现场调整。

---

## 贡献与协作

如果你想贡献代码：

1. Fork 本仓库并在你的分支上开发。
2. 提交清晰的 commit（每次改动解决一件事）。
3. 发起 Pull Request，写明改动目的与测试方式。

注意：对已推送分支重写历史（rebase / force-push）会影响其他协作者，谨慎操作。

---

## 许可

仓库中第三方库遵循各自 LICENSE 文件。主项目代码建议标注 MIT 或你想用的开源协议（请在仓库根添加 LICENSE 文件以明确）。

---

如果你希望我把 README 调整为中文/英文的不同版本、添加更多图片、或为 README 增加快速上手视频/调试要点，我可以继续迭代（是否继续？）。

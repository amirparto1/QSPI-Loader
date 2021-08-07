################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/W25Q256/W25q256.c 

OBJS += \
./Drivers/W25Q256/W25q256.o 

C_DEPS += \
./Drivers/W25Q256/W25q256.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/W25Q256/%.o: ../Drivers/W25Q256/%.c Drivers/W25Q256/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32F767xx -c -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I"D:/Git/Titrator-HMI-Phase1/QSPI-Loader/Drivers/Loader" -I"D:/Git/Titrator-HMI-Phase1/QSPI-Loader/Drivers/W25Q256" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"


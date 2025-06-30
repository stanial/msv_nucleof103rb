################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/A7670.c \
../Core/Src/INA219.c \
../Core/Src/adxl345.c \
../Core/Src/ds18b20.c \
../Core/Src/gnss.c \
../Core/Src/main.c \
../Core/Src/msv.c \
../Core/Src/onewire.c \
../Core/Src/serial_monitor.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c 

OBJS += \
./Core/Src/A7670.o \
./Core/Src/INA219.o \
./Core/Src/adxl345.o \
./Core/Src/ds18b20.o \
./Core/Src/gnss.o \
./Core/Src/main.o \
./Core/Src/msv.o \
./Core/Src/onewire.o \
./Core/Src/serial_monitor.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o 

C_DEPS += \
./Core/Src/A7670.d \
./Core/Src/INA219.d \
./Core/Src/adxl345.d \
./Core/Src/ds18b20.d \
./Core/Src/gnss.d \
./Core/Src/main.d \
./Core/Src/msv.d \
./Core/Src/onewire.d \
./Core/Src/serial_monitor.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/A7670.cyclo ./Core/Src/A7670.d ./Core/Src/A7670.o ./Core/Src/A7670.su ./Core/Src/INA219.cyclo ./Core/Src/INA219.d ./Core/Src/INA219.o ./Core/Src/INA219.su ./Core/Src/adxl345.cyclo ./Core/Src/adxl345.d ./Core/Src/adxl345.o ./Core/Src/adxl345.su ./Core/Src/ds18b20.cyclo ./Core/Src/ds18b20.d ./Core/Src/ds18b20.o ./Core/Src/ds18b20.su ./Core/Src/gnss.cyclo ./Core/Src/gnss.d ./Core/Src/gnss.o ./Core/Src/gnss.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/msv.cyclo ./Core/Src/msv.d ./Core/Src/msv.o ./Core/Src/msv.su ./Core/Src/onewire.cyclo ./Core/Src/onewire.d ./Core/Src/onewire.o ./Core/Src/onewire.su ./Core/Src/serial_monitor.cyclo ./Core/Src/serial_monitor.d ./Core/Src/serial_monitor.o ./Core/Src/serial_monitor.su ./Core/Src/stm32f1xx_hal_msp.cyclo ./Core/Src/stm32f1xx_hal_msp.d ./Core/Src/stm32f1xx_hal_msp.o ./Core/Src/stm32f1xx_hal_msp.su ./Core/Src/stm32f1xx_it.cyclo ./Core/Src/stm32f1xx_it.d ./Core/Src/stm32f1xx_it.o ./Core/Src/stm32f1xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f1xx.cyclo ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su

.PHONY: clean-Core-2f-Src


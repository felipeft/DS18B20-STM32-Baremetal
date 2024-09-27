################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/ds18b20.c \
../Core/Src/i2c.c \
../Core/Src/lcd_i2c.c \
../Core/Src/main.c \
../Core/Src/t_delay.c \
../Core/Src/usart.c 

OBJS += \
./Core/Src/ds18b20.o \
./Core/Src/i2c.o \
./Core/Src/lcd_i2c.o \
./Core/Src/main.o \
./Core/Src/t_delay.o \
./Core/Src/usart.o 

C_DEPS += \
./Core/Src/ds18b20.d \
./Core/Src/i2c.d \
./Core/Src/lcd_i2c.d \
./Core/Src/main.d \
./Core/Src/t_delay.d \
./Core/Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I"/home/felipe/Desktop/Micro/DS18B20 UART/Core/Inc" -I"/home/felipe/Desktop/Micro/DS18B20 UART/Drivers/CMSIS/Device" -I"/home/felipe/Desktop/Micro/DS18B20 UART/Drivers/CMSIS/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/ds18b20.cyclo ./Core/Src/ds18b20.d ./Core/Src/ds18b20.o ./Core/Src/ds18b20.su ./Core/Src/i2c.cyclo ./Core/Src/i2c.d ./Core/Src/i2c.o ./Core/Src/i2c.su ./Core/Src/lcd_i2c.cyclo ./Core/Src/lcd_i2c.d ./Core/Src/lcd_i2c.o ./Core/Src/lcd_i2c.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/t_delay.cyclo ./Core/Src/t_delay.d ./Core/Src/t_delay.o ./Core/Src/t_delay.su ./Core/Src/usart.cyclo ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su

.PHONY: clean-Core-2f-Src


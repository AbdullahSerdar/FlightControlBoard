################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Tests/Mocks/mock_i2c.c \
../Tests/Mocks/mock_uart.c 

OBJS += \
./Tests/Mocks/mock_i2c.o \
./Tests/Mocks/mock_uart.o 

C_DEPS += \
./Tests/Mocks/mock_i2c.d \
./Tests/Mocks/mock_uart.d 


# Each subdirectory must supply rules for building sources it contributes
Tests/Mocks/%.o Tests/Mocks/%.su Tests/Mocks/%.cyclo: ../Tests/Mocks/%.c Tests/Mocks/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUNITY_INCLUDE_CONFIG_H -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/serda/STM32CubeIDE/workspace_1.14.0/FlightControlBoard/Drivers/dev_lora/inc" -I"C:/Users/serda/STM32CubeIDE/workspace_1.14.0/FlightControlBoard/Drivers/dev_bme/inc" -I"C:/Users/serda/STM32CubeIDE/workspace_1.14.0/FlightControlBoard/Drivers/dev_mpu/inc" -I"C:/Users/serda/STM32CubeIDE/workspace_1.14.0/FlightControlBoard/Drivers/dev_gps/inc" -I"C:/Users/serda/STM32CubeIDE/workspace_1.14.0/FlightControlBoard/Drivers/dev_watchdog/inc" -I"C:/Users/serda/STM32CubeIDE/workspace_1.14.0/FlightControlBoard/Drivers/dev_gps/middleware/inc" -I"C:/Users/serda/STM32CubeIDE/workspace_1.14.0/FlightControlBoard/Tests/Mocks" -I"C:/Users/serda/STM32CubeIDE/workspace_1.14.0/FlightControlBoard/Tests/Unity" -I"C:/Users/serda/STM32CubeIDE/workspace_1.14.0/FlightControlBoard/Tests/Unit" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Tests-2f-Mocks

clean-Tests-2f-Mocks:
	-$(RM) ./Tests/Mocks/mock_i2c.cyclo ./Tests/Mocks/mock_i2c.d ./Tests/Mocks/mock_i2c.o ./Tests/Mocks/mock_i2c.su ./Tests/Mocks/mock_uart.cyclo ./Tests/Mocks/mock_uart.d ./Tests/Mocks/mock_uart.o ./Tests/Mocks/mock_uart.su

.PHONY: clean-Tests-2f-Mocks


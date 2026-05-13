################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/dev_bme/src/bme_driver.c \
../Drivers/dev_bme/src/bme_task.c 

OBJS += \
./Drivers/dev_bme/src/bme_driver.o \
./Drivers/dev_bme/src/bme_task.o 

C_DEPS += \
./Drivers/dev_bme/src/bme_driver.d \
./Drivers/dev_bme/src/bme_task.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/dev_bme/src/%.o Drivers/dev_bme/src/%.su Drivers/dev_bme/src/%.cyclo: ../Drivers/dev_bme/src/%.c Drivers/dev_bme/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/serda/STM32CubeIDE/workspace_1.14.0/FlightControlBoard/Drivers/dev_lora/inc" -I"C:/Users/serda/STM32CubeIDE/workspace_1.14.0/FlightControlBoard/Drivers/dev_bme/inc" -I"C:/Users/serda/STM32CubeIDE/workspace_1.14.0/FlightControlBoard/Drivers/dev_mpu/inc" -I"C:/Users/serda/STM32CubeIDE/workspace_1.14.0/FlightControlBoard/Drivers/dev_gps/inc" -I"C:/Users/serda/STM32CubeIDE/workspace_1.14.0/FlightControlBoard/Drivers/dev_watchdog/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-dev_bme-2f-src

clean-Drivers-2f-dev_bme-2f-src:
	-$(RM) ./Drivers/dev_bme/src/bme_driver.cyclo ./Drivers/dev_bme/src/bme_driver.d ./Drivers/dev_bme/src/bme_driver.o ./Drivers/dev_bme/src/bme_driver.su ./Drivers/dev_bme/src/bme_task.cyclo ./Drivers/dev_bme/src/bme_task.d ./Drivers/dev_bme/src/bme_task.o ./Drivers/dev_bme/src/bme_task.su

.PHONY: clean-Drivers-2f-dev_bme-2f-src


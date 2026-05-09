################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/dev_gps/src/gps_driver.c \
../Drivers/dev_gps/src/gps_task.c 

OBJS += \
./Drivers/dev_gps/src/gps_driver.o \
./Drivers/dev_gps/src/gps_task.o 

C_DEPS += \
./Drivers/dev_gps/src/gps_driver.d \
./Drivers/dev_gps/src/gps_task.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/dev_gps/src/%.o Drivers/dev_gps/src/%.su Drivers/dev_gps/src/%.cyclo: ../Drivers/dev_gps/src/%.c Drivers/dev_gps/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/serda/STM32CubeIDE/workspace_1.14.0/FlightControlBoard/Drivers/dev_lora/inc" -I"C:/Users/serda/STM32CubeIDE/workspace_1.14.0/FlightControlBoard/Drivers/dev_bme/inc" -I"C:/Users/serda/STM32CubeIDE/workspace_1.14.0/FlightControlBoard/Drivers/dev_mpu/inc" -I"C:/Users/serda/STM32CubeIDE/workspace_1.14.0/FlightControlBoard/Drivers/dev_gps/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-dev_gps-2f-src

clean-Drivers-2f-dev_gps-2f-src:
	-$(RM) ./Drivers/dev_gps/src/gps_driver.cyclo ./Drivers/dev_gps/src/gps_driver.d ./Drivers/dev_gps/src/gps_driver.o ./Drivers/dev_gps/src/gps_driver.su ./Drivers/dev_gps/src/gps_task.cyclo ./Drivers/dev_gps/src/gps_task.d ./Drivers/dev_gps/src/gps_task.o ./Drivers/dev_gps/src/gps_task.su

.PHONY: clean-Drivers-2f-dev_gps-2f-src


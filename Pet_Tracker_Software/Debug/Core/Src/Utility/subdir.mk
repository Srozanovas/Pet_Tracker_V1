################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Utility/ring_buffer.c 

OBJS += \
./Core/Src/Utility/ring_buffer.o 

C_DEPS += \
./Core/Src/Utility/ring_buffer.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Utility/%.o Core/Src/Utility/%.su Core/Src/Utility/%.cyclo: ../Core/Src/Utility/%.c Core/Src/Utility/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L433xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Utility

clean-Core-2f-Src-2f-Utility:
	-$(RM) ./Core/Src/Utility/ring_buffer.cyclo ./Core/Src/Utility/ring_buffer.d ./Core/Src/Utility/ring_buffer.o ./Core/Src/Utility/ring_buffer.su

.PHONY: clean-Core-2f-Src-2f-Utility


################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/APP/cli_app.c 

OBJS += \
./Core/Src/APP/cli_app.o 

C_DEPS += \
./Core/Src/APP/cli_app.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/APP/%.o Core/Src/APP/%.su Core/Src/APP/%.cyclo: ../Core/Src/APP/%.c Core/Src/APP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L433xx -c -I../Core/Inc -I../Core/Src/Drivers -I../Core/Src/Utility -I../Core/Src/APP -I../Core/Src/API -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-APP

clean-Core-2f-Src-2f-APP:
	-$(RM) ./Core/Src/APP/cli_app.cyclo ./Core/Src/APP/cli_app.d ./Core/Src/APP/cli_app.o ./Core/Src/APP/cli_app.su

.PHONY: clean-Core-2f-Src-2f-APP


################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../component/serial_manager/fsl_component_serial_manager.c \
../component/serial_manager/fsl_component_serial_port_uart.c 

OBJS += \
./component/serial_manager/fsl_component_serial_manager.o \
./component/serial_manager/fsl_component_serial_port_uart.o 

C_DEPS += \
./component/serial_manager/fsl_component_serial_manager.d \
./component/serial_manager/fsl_component_serial_port_uart.d 


# Each subdirectory must supply rules for building sources it contributes
component/serial_manager/%.o: ../component/serial_manager/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_LPC55S69JBD100 -DCPU_LPC55S69JBD100_cm33 -DCPU_LPC55S69JBD100_cm33_core0 -DPRINTF_ADVANCED_ENABLE=1 -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/drivers" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/LPC55S69/drivers" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/utilities" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/component/uart" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/component/serial_manager" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/component/lists" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/usb/device/class/cdc" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/usb/output/source/device/class" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/usb/device/include" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/usb/device/source" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/usb/device/source/lpcip3511" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/usb/include" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/usb/device/class" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/usb/output/source/device" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/usb/phy" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/component/osa" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/source/generated" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/CMSIS" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/device" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/drivers" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/LPC55S69/drivers" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/utilities" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/component/uart" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/component/serial_manager" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/component/lists" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/CMSIS" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/device" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/board" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/source" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/startup" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



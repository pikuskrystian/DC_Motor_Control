################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_ctimer.c \
../drivers/fsl_flexcomm.c \
../drivers/fsl_gint.c \
../drivers/fsl_gpio.c \
../drivers/fsl_i2c.c \
../drivers/fsl_inputmux.c \
../drivers/fsl_pint.c \
../drivers/fsl_reset.c \
../drivers/fsl_spi.c \
../drivers/fsl_usart.c 

OBJS += \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_ctimer.o \
./drivers/fsl_flexcomm.o \
./drivers/fsl_gint.o \
./drivers/fsl_gpio.o \
./drivers/fsl_i2c.o \
./drivers/fsl_inputmux.o \
./drivers/fsl_pint.o \
./drivers/fsl_reset.o \
./drivers/fsl_spi.o \
./drivers/fsl_usart.o 

C_DEPS += \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_ctimer.d \
./drivers/fsl_flexcomm.d \
./drivers/fsl_gint.d \
./drivers/fsl_gpio.d \
./drivers/fsl_i2c.d \
./drivers/fsl_inputmux.d \
./drivers/fsl_pint.d \
./drivers/fsl_reset.d \
./drivers/fsl_spi.d \
./drivers/fsl_usart.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_LPC55S69JBD100 -DCPU_LPC55S69JBD100_cm33 -DCPU_LPC55S69JBD100_cm33_core0 -DPRINTF_ADVANCED_ENABLE=1 -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/drivers" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/LPC55S69/drivers" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/utilities" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/component/uart" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/component/serial_manager" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/component/lists" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/usb/device/class/cdc" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/usb/output/source/device/class" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/usb/device/include" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/usb/device/source" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/usb/device/source/lpcip3511" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/usb/include" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/usb/device/class" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/usb/output/source/device" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/usb/phy" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/component/osa" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/source/generated" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/CMSIS" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/device" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/drivers" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/LPC55S69/drivers" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/utilities" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/component/uart" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/component/serial_manager" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/component/lists" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/CMSIS" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/device" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/board" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/source" -I"/Users/ki_01/Documents/MCUXpressoIDE_11.3.0/workspace/OKDO_Test/startup" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/delay.c \
../src/rfm12.c \
../src/system_stm32f10x.c 

S_UPPER_SRCS += \
../src/startup_stm32f10x_md_vl.S 

OBJS += \
./src/delay.o \
./src/rfm12.o \
./src/startup_stm32f10x_md_vl.o \
./src/system_stm32f10x.o 

S_UPPER_DEPS += \
./src/startup_stm32f10x_md_vl.d 

C_DEPS += \
./src/delay.d \
./src/rfm12.d \
./src/system_stm32f10x.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C Compiler (Sourcery Lite Bare)'
	arm-none-eabi-gcc -DSTM32F10X_MD_VL -DUSE_STDPERIPH_DRIVER -I"D:\workspace\rfm12\src" -I"D:\workspace\rfm12\stm32_std\CMSIS\CM3\CoreSupport" -I"D:\workspace\rfm12\stm32_std\CMSIS\CM3\DeviceSupport\ST\STM32F10x" -I"D:\workspace\rfm12\stm32_std\STM32F10x_StdPeriph_Driver\inc" -O0 -ffunction-sections -fdata-sections -Wall -std=gnu99 -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -g3 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC Assembler (Sourcery Lite Bare)'
	arm-none-eabi-gcc -x assembler-with-cpp -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=cortex-m3 -mthumb -g3 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



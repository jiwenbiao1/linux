################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../signal/receive/main.c 

OBJS += \
./signal/receive/main.o 

C_DEPS += \
./signal/receive/main.d 


# Each subdirectory must supply rules for building sources it contributes
signal/receive/%.o: ../signal/receive/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



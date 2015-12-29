################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../process/message/receive/message_process_receive.c 

OBJS += \
./process/message/receive/message_process_receive.o 

C_DEPS += \
./process/message/receive/message_process_receive.d 


# Each subdirectory must supply rules for building sources it contributes
process/message/receive/%.o: ../process/message/receive/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../process/message/send/message_process_send.c 

OBJS += \
./process/message/send/message_process_send.o 

C_DEPS += \
./process/message/send/message_process_send.d 


# Each subdirectory must supply rules for building sources it contributes
process/message/send/%.o: ../process/message/send/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



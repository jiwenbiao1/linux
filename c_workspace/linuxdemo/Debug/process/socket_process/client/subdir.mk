################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../process/socket_process/client/socket_process_client.c 

OBJS += \
./process/socket_process/client/socket_process_client.o 

C_DEPS += \
./process/socket_process/client/socket_process_client.d 


# Each subdirectory must supply rules for building sources it contributes
process/socket_process/client/%.o: ../process/socket_process/client/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../process/pipe_fifo/pipe.c 

OBJS += \
./process/pipe_fifo/pipe.o 

C_DEPS += \
./process/pipe_fifo/pipe.d 


# Each subdirectory must supply rules for building sources it contributes
process/pipe_fifo/%.o: ../process/pipe_fifo/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../process/sem_process/sem_process.c 

OBJS += \
./process/sem_process/sem_process.o 

C_DEPS += \
./process/sem_process/sem_process.d 


# Each subdirectory must supply rules for building sources it contributes
process/sem_process/%.o: ../process/sem_process/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



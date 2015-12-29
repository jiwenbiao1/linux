################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../process/shm/shmread/shmread.c 

OBJS += \
./process/shm/shmread/shmread.o 

C_DEPS += \
./process/shm/shmread/shmread.d 


# Each subdirectory must supply rules for building sources it contributes
process/shm/shmread/%.o: ../process/shm/shmread/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../pthread/main.c \
../pthread/pthread_demo.c 

OBJS += \
./pthread/main.o \
./pthread/pthread_demo.o 

C_DEPS += \
./pthread/main.d \
./pthread/pthread_demo.d 


# Each subdirectory must supply rules for building sources it contributes
pthread/%.o: ../pthread/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



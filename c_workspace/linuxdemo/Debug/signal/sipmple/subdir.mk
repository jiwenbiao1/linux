################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../signal/sipmple/main.c 

OBJS += \
./signal/sipmple/main.o 

C_DEPS += \
./signal/sipmple/main.d 


# Each subdirectory must supply rules for building sources it contributes
signal/sipmple/%.o: ../signal/sipmple/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



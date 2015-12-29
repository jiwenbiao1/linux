################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tty/receive/queue.c \
../tty/receive/receive.c 

OBJS += \
./tty/receive/queue.o \
./tty/receive/receive.o 

C_DEPS += \
./tty/receive/queue.d \
./tty/receive/receive.d 


# Each subdirectory must supply rules for building sources it contributes
tty/receive/%.o: ../tty/receive/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../camera.cpp \
../geometry.cpp \
../main.cpp \
../math_3d.cpp \
../matrix.cpp \
../pipeline.cpp \
../texture.cpp \
../vector.cpp 

OBJS += \
./camera.o \
./geometry.o \
./main.o \
./math_3d.o \
./matrix.o \
./pipeline.o \
./texture.o \
./vector.o 

CPP_DEPS += \
./camera.d \
./geometry.d \
./main.d \
./math_3d.d \
./matrix.d \
./pipeline.d \
./texture.d \
./vector.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



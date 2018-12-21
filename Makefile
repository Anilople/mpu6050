
all : mpu6050.o mpu6050Test.o

run : all
	./mpu6050Test.o

clean :
	rm *.o

# lib模块(加速度, 角速度)
mpu6050.o : mpu6050.c mpu6050.h
	gcc -o mpu6050.o mpu6050.c -c

# 测试模块
mpu6050Test.o : mpu6050Test.c mpu6050.o
	gcc -Wall -lwiringPi -o mpu6050Test.o mpu6050Test.c mpu6050.o
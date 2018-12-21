#include<stdio.h>
#include<stdlib.h>
#include "mpu6050.h"

int main(int argc, char * argv[])
{
    printf("fd:%d\n", mpu6050Init(0x68));
    // printf("gyroscope range:%d\n", read_gyro_range(0));
    printf("temperature:%f\n", get_temp());

    set_accel_range(0x00);
    printf("accelerate range:%d\n", read_accel_range(0));
    printf("accelerate X : %f\n", get_accel_dataX(0));    
    printf("accelerate Y : %f\n", get_accel_dataY(0)); 
    printf("accelerate Z : %f\n", get_accel_dataZ(0));

    set_gyro_range(0x00);
    printf("gyroscope range:%d\n", read_gyro_range(0));
    printf("gyroscope X : %f\n", get_gyro_dataX(0));    
    printf("gyroscope Y : %f\n", get_gyro_dataY(0)); 
    printf("gyroscope Z : %f\n", get_gyro_dataZ(0));

    return 0;
}
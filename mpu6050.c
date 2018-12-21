#include<stdio.h>
#include<stdlib.h>
#include<wiringPiI2C.h>

// int wiringPiI2CSetup(int devId)返回的值
static int fd = 0;
/*
    Global Variables
    模仿参考链接中对应的代码
*/
static const float GRAVITIY_MS2 = 9.80665;
static int address = 0x68;
static int bus = 1;

// Scale Modifiers
static const float ACCEL_SCALE_MODIFIER_2G = 16384.0;
static const float ACCEL_SCALE_MODIFIER_4G = 8192.0;
static const float ACCEL_SCALE_MODIFIER_8G = 4096.0;
static const float ACCEL_SCALE_MODIFIER_16G = 2048.0;

static const float GYRO_SCALE_MODIFIER_250DEG = 131.0;
static const float GYRO_SCALE_MODIFIER_500DEG = 65.5;
static const float GYRO_SCALE_MODIFIER_1000DEG = 32.8;
static const float GYRO_SCALE_MODIFIER_2000DEG = 16.4;

// Pre-defined ranges
static const int ACCEL_RANGE_2G = 0x00;
static const int ACCEL_RANGE_4G = 0x08;
static const int ACCEL_RANGE_8G = 0x10;
static const int ACCEL_RANGE_16G = 0x18;

static const int GYRO_RANGE_250DEG = 0x00;
static const int GYRO_RANGE_500DEG = 0x08;
static const int GYRO_RANGE_1000DEG = 0x10;
static const int GYRO_RANGE_2000DEG = 0x18;

// MPU-6050 Registers
static const int PWR_MGMT_1 = 0x6B;
static const int PWR_MGMT_2 = 0x6C;

static const int ACCEL_XOUT0 = 0x3B;
static const int ACCEL_YOUT0 = 0x3D;
static const int ACCEL_ZOUT0 = 0x3F;

static const int TEMP_OUT0 = 0x41;

static const int GYRO_XOUT0 = 0x43;
static const int GYRO_YOUT0 = 0x45;
static const int GYRO_ZOUT0 = 0x47;

static const int ACCEL_CONFIG = 0x1C;
static const int GYRO_CONFIG = 0x1B;

int mpu6050Init(int addressGiven)
{
    address = addressGiven;
    fd = wiringPiI2CSetup(address);
    // Wake up the MPU-6050 since it starts in sleep mode
    wiringPiI2CWriteReg8(fd, PWR_MGMT_1, 0x00);    
    return fd;
}

/*
    不使用wiringPiI2CReadReg16, 模仿源代码中的, 
    使用wiringPiI2CReadReg8来组成这个函数

    Read two i2c registers and combine them.

    register -- the first register to read from.
    Returns the combined read results.

    register在c语言中是一个关键字, 所以不能用register来当参数名,
    这里换成reg
*/
static int read_i2c_word(const int reg)
{
    int high = wiringPiI2CReadReg8(fd, reg);
    int low = wiringPiI2CReadReg8(fd, reg + 1);

    int value = (high << 8) + low;
    // printf("raw temperature : %d\n", value);
    if (value >= 0x8000) {
        // printf("value >= 0x8000\n");
        return -((65535 - value) + 1);
    } else {
        // printf("value < 0x8000\n");
        return value;
    }
}

/*
    获取温度(摄氏度)
    Reads the temperature from the onboard temperature sensor of the MPU-6050.
    Returns the temperature in degrees Celcius.
*/
float get_temp()
{
    // 获取原生的数据
    float raw_temp = (float) read_i2c_word(TEMP_OUT0);
    // printf("raw_temp:%f\n", raw_temp);

    // Get the actual temperature using the formule given in the
    // MPU-6050 Register Map and Descriptions revision 4.2, page 30
    return (raw_temp / 340.0) + 36.53;
}

/*
    accelerate : 加速度, 简称accel
*/

/*
    Sets the range of the accelerometer to range.

    accel_range -- the range to set the accelerometer to. Using a
    pre-defined range is advised.
*/
void set_accel_range(int accelRange)
{
    // First change it to 0x00 to make sure we write the correct value later
    wiringPiI2CWriteReg8(fd, ACCEL_CONFIG, 0x00);

    // Write the new range to the ACCEL_CONFIG register
    wiringPiI2CWriteReg8(fd, ACCEL_CONFIG, accelRange);
}

/*
    Reads the range the accelerometer is set to.

    If raw is True, it will return the raw value from the ACCEL_CONFIG
    register
    If raw is False, it will return an integer: -1, 2, 4, 8 or 16. When it
    returns -1 something went wrong.
*/
int read_accel_range(int raw)
{
    // 原始数据
    int rawData = wiringPiI2CReadReg8(fd, ACCEL_CONFIG);

    if(raw) {
        return rawData;
    } else {
        if(ACCEL_RANGE_2G == rawData) {
            return 2;
        } else if(ACCEL_RANGE_4G == rawData) {
            return 4;
        } else if(ACCEL_RANGE_8G == rawData) {
            return 8;
        } else if(ACCEL_RANGE_16G == rawData) {
            return 16;
        } else {
            // 错误则返回-1
            return -1;
        }
    }
}


// 一些存加速度的全局变量
static float accelDataX;
static float accelDataY;
static float accelDataZ;

static float accelDataXG;
static float accelDataYG;
static float accelDataZG;
/*
    Gets and returns the X, Y and Z values from the accelerometer.
    将他们分别以2种形式存入上方的全局变量中
*/
static void get_accel_data()
{
    float x = (float) read_i2c_word(ACCEL_XOUT0);
    float y = (float) read_i2c_word(ACCEL_YOUT0);
    float z = (float) read_i2c_word(ACCEL_ZOUT0);

    float accel_scale_modifier = 0;
    int accel_range = read_accel_range(1); // 原始数据

    if(ACCEL_RANGE_2G == accel_range) {
        accel_scale_modifier = ACCEL_SCALE_MODIFIER_2G;
    } else if(ACCEL_RANGE_4G == accel_range) {
        accel_scale_modifier = ACCEL_SCALE_MODIFIER_4G;
    } else if(ACCEL_RANGE_8G == accel_range) {
        accel_scale_modifier = ACCEL_SCALE_MODIFIER_8G;
    } else if(ACCEL_RANGE_16G == accel_range) {
        accel_scale_modifier = ACCEL_SCALE_MODIFIER_16G;
    } else {
        printf("Unkown range - accel_scale_modifier set to self.ACCEL_SCALE_MODIFIER_2G");
        accel_scale_modifier = ACCEL_SCALE_MODIFIER_2G;
    }

    x = x / accel_scale_modifier;
    y = y / accel_scale_modifier;
    z = z / accel_scale_modifier;

    // 以下是给全局变量赋值
    accelDataXG = x;
    accelDataYG = y;
    accelDataZG = z;
    accelDataX = x * GRAVITIY_MS2;
    accelDataY = y * GRAVITIY_MS2;
    accelDataZ = z * GRAVITIY_MS2;
}

float get_accel_dataX(int g)
{
    get_accel_data();
    return g ? accelDataXG : accelDataX;
}

float get_accel_dataY(int g)
{
    get_accel_data();
    return g ? accelDataYG : accelDataY;
}

float get_accel_dataZ(int g)
{
    get_accel_data();
    return g ? accelDataZG : accelDataZ;
}

/*
    gyroscope : 陀螺仪, 简称gyro
*/

/*
    Sets the range of the gyroscope to range.

    gyro_range -- the range to set the gyroscope to. Using a pre-defined
    range is advised.
*/
void set_gyro_range(int gyroRange)
{
    // First change it to 0x00 to make sure we write the correct value later
    wiringPiI2CWriteReg8(fd, GYRO_CONFIG, 0x00);

    // Write the new range to the ACCEL_CONFIG register
    wiringPiI2CWriteReg8(fd, GYRO_CONFIG, gyroRange);
}

/*
    Reads the range the gyroscope is set to.

    If raw is True, it will return the raw value from the GYRO_CONFIG
    register.
    If raw is False, it will return 250, 500, 1000, 2000 or -1. If the
    returned value is equal to -1 something went wrong.
*/
int read_gyro_range(int raw)
{
    int raw_data = wiringPiI2CReadReg8(fd, GYRO_CONFIG);
    if(raw) {
        return raw_data;
    } else {
        if(GYRO_RANGE_250DEG == raw_data) {
            return 250;
        } else if(GYRO_RANGE_500DEG == raw_data) {
            return 500;
        } else if(GYRO_RANGE_1000DEG == raw_data) {
            return 1000;
        } else if(GYRO_RANGE_2000DEG == raw_data) {
            return 2000;
        } else {
            return -1;
        }
    }
}

// 一些给读GyroData使用的全局变量
static float gyroDataX;
static float gyroDataY;
static float gyroDataZ;

/*
    Gets and returns the X, Y and Z values from the gyroscope.

    将读入的值存入上方的全局变量中
*/
static void getGyroData()
{
    float x = (float) read_i2c_word(GYRO_XOUT0);
    float y = (float) read_i2c_word(GYRO_YOUT0);
    float z = (float) read_i2c_word(GYRO_ZOUT0);

    float gyro_scale_modifier = 0;
    int gyro_range = read_gyro_range(1);

    if(GYRO_RANGE_250DEG == gyro_range)
        gyro_scale_modifier = GYRO_SCALE_MODIFIER_250DEG;
    else if(GYRO_RANGE_500DEG == gyro_range) {
        gyro_scale_modifier = GYRO_SCALE_MODIFIER_500DEG;
    }
    else if(GYRO_RANGE_1000DEG == gyro_range) {
        gyro_scale_modifier = GYRO_SCALE_MODIFIER_1000DEG;
    }
    else if(GYRO_RANGE_2000DEG == gyro_range) {
        gyro_scale_modifier = GYRO_SCALE_MODIFIER_2000DEG;
    }
    else {
        printf("Unkown range - gyro_scale_modifier set to self.GYRO_SCALE_MODIFIER_250DEG");
        gyro_scale_modifier = GYRO_SCALE_MODIFIER_250DEG;
    }

    x = x / gyro_scale_modifier;
    y = y / gyro_scale_modifier;
    z = z / gyro_scale_modifier;

    gyroDataX = x;
    gyroDataY = y;
    gyroDataZ = z;
}

float get_gyro_dataX()
{
    getGyroData();
    return gyroDataX;
}

float get_gyro_dataY()
{
    getGyroData();
    return gyroDataY;
}

float get_gyro_dataZ()
{
    getGyroData();
    return gyroDataZ;
}
## MPU6050移植

这是一个用纯C写的代码, 移植了[Tijndagamer/mpu6050](https://github.com/Tijndagamer/mpu6050), 也就是将Python改为了C语言,

测试通过的运行环境为安装了wiringPi, i2c的树莓派3B

### API

在大体上保持了一致, 有部分稍微不同

```c
/*
    传入mpu6050的I2C地址, 一般为0x68
	只有在使用了这个函数后, 才可以使用后边的函数
*/
int mpu6050Init(int address);

// 获取温度
float get_temp();

/*
    accelerate : 加速度, 简称accel
*/

/*
    设置加速度的区间, 可传入的值为
    0x00 : 对应为-2g~+2g
    0x08 : 对应为-4g~+4g
    0x10 : 对应为-8g~+8g
    0x18 : 对应为-16g~+16g
*/
void set_accel_range(int accelRange);

/*
    读取加速度的区间
    如果 raw 为 true:
        返回ACCEL_CONFIG寄存器中的值
    如果 raw 为 false:
        将会返回-1, 2, 4, 8, 或者16.
        -1 代表读取失败
        2 代表 -2g~+2g
        4 代表 -4g~+4g
        8 代表 -8g~+8g
        16 代表 -16g~+16g
*/
int read_accel_range(int raw);

/*
    返回x轴的加速度
    如果 g 为 true:
        返回的数值表示当前x轴的加速度是g的多少倍
    如果 g 为 false:
        返回的数值表示当前x轴的加速度是多少m/s^2
*/
float get_accel_dataX(int g);
/*
    返回y轴的加速度
    如果 g 为 true:
        返回的数值表示当前y轴的加速度是g的多少倍
    如果 g 为 false:
        返回的数值表示当前y轴的加速度是多少m/s^2
*/
float get_accel_dataY(int g);
/*
    返回z轴的加速度
    如果 g 为 true:
        返回的数值表示当前z轴的加速度是g的多少倍
    如果 g 为 false:
        返回的数值表示当前z轴的加速度是多少m/s^2
*/
float get_accel_dataZ(int g);

/*
    gyroscope : 陀螺仪, 简称gyro
*/

/*
    设置陀螺仪的区间
    0x00 : 对应为 250度/秒
    0x08 : 对应为 500度/秒
    0x10 : 对应为 1000度/秒
    0x18 : 对应为 2000度/秒
*/
void set_gyro_range(int gyroRange);

/*
    读取陀螺仪的区间
    如果 raw 为 true:
        返回ACCEL_CONFIG寄存器中的值
    如果 raw 为 false:
        将会返回-1, 250, 500, 1000, 或者2000.
        -1 代表读取失败
        250 代表 250度/秒
        500 代表 500度/秒
        1000 代表 1000度/秒
        2000 代表 2000度/秒
*/
int read_gyro_range(int raw);

/*
    返回x轴的角速度, 单位为 度/秒
    整数代表顺时针, 负数代表逆时针
*/
float get_gyro_dataX();
/*
    返回y轴的角速度, 单位为 度/秒
    整数代表顺时针, 负数代表逆时针
*/
float get_gyro_dataY();
/*
    返回z轴的角速度, 单位为 度/秒
    整数代表顺时针, 负数代表逆时针
*/
float get_gyro_dataZ();
```


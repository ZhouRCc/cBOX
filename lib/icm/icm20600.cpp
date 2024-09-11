#include <Wire.h>
#include "icm20600.h"

// ICM-20600 寄存器地址
#define WHO_AM_I 0x75
#define PWR_MGMT_1 0x6B
#define ACCEL_XOUT_H 0x3B
#define GYRO_XOUT_H 0x43
#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42
#define ACCEL_CONFIG_1 0x1c
#define GYRO_CONFIG 0x1b

ICM20600 icm20600; // 定义变量

// 初始化 ICM-20600，允许自定义 I2C 引脚
void ICM20600::begin(int sda, int scl, uint8_t address, uint32_t freq) {
    if(address != 0x68 && address != 0x69) {
        Serial.println("ICM-20600 address must be 0x68 or 0x69");
        return;
    }
    if (freq > 400000 || freq < 100000)
    {
        Serial.println("ICM-20600 frequency must be between 100kHz and 400kHz");
        return;
    }
    
    icm_address = address;
    Wire.begin(sda, scl, freq);
    delay(100); // 增加延迟

    // 读取 WHO_AM_I 寄存器以确认设备
    uint8_t whoAmI = readRegister(icm_address, WHO_AM_I);
    if (whoAmI == 0x11) { // WHO_AM_I 寄存器的默认值为 0x11
        Serial.print("ICM-20600 found at address 0x");
        Serial.println(icm_address, HEX);
    } else {
        Serial.println("ICM-20600 not found");
        return;
    }

    writeRegister(PWR_MGMT_1, 0x00); // 退出睡眠模式
    delay(100); // 增加延迟
    if (!isConnected()) {
        Serial.println("ICM-20600 not connected");
    } else {
        Serial.println("ICM-20600 connected");
    }
    updateK();
}

// 写寄存器
void ICM20600::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(icm_address);
    Wire.write(reg);
    Wire.write(value);
    if (Wire.endTransmission() != 0) {
        Serial.print("Error writing to register: ");
        Serial.println(reg, HEX);
    }
    delay(1); // 增加延迟
}

// 读寄存器
uint8_t ICM20600::readRegister(uint8_t reg) {
    Wire.beginTransmission(icm_address);
    Wire.write(reg);
    if (Wire.endTransmission() != 0) {
        Serial.print("Error writing to register: ");
        Serial.println(reg, HEX);
        return 0;
    }
    delay(1); // 增加延迟
    Wire.requestFrom(icm_address, 1u);
    if (Wire.available()) {
        return Wire.read();
    } else {
        Serial.print("Error reading register: ");
        Serial.println(reg, HEX);
        return 0;
    }
}

// 读寄存器（带地址）
uint8_t ICM20600::readRegister(uint8_t address, uint8_t reg) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    if (Wire.endTransmission() != 0) {
        Serial.print("Error writing to register: ");
        Serial.println(reg, HEX);
        return 0;
    }
    delay(1); // 增加延迟
    Wire.requestFrom(address, 1u);
    if (Wire.available()) {
        return Wire.read();
    } else {
        Serial.print("Error reading register: ");
        Serial.println(reg, HEX);
        return 0;
    }
}

icm_acc_data ICM20600::getAccelData() {
    readAccelData();
    data.acc.ax = (float)data.ax / acc_k;
    data.acc.ay = (float)data.ay / acc_k;
    data.acc.az = (float)data.az / acc_k;
    return data.acc;
}

icm_gyro_data ICM20600::getGyroData() {
    readGyroData();
    data.gyro.gx = (float)data.gx / gyro_k;
    data.gyro.gy = (float)data.gy / gyro_k;
    data.gyro.gz = (float)data.gz / gyro_k;
    return data.gyro;
}

// 读取加速度计数据
void ICM20600::readAccelData() {
    Wire.beginTransmission(icm_address);
    Wire.write(ACCEL_XOUT_H);
    if (Wire.endTransmission() != 0) {
        Serial.println("Error writing to ACCEL_XOUT_H register");
        return;
    }
    delay(1); // 增加延迟
    Wire.requestFrom(icm_address, 6u);
    if (Wire.available() == 6) {
        data.ax = (int16_t)((Wire.read() << 8) | Wire.read());
        data.ay = (int16_t)((Wire.read() << 8) | Wire.read());
        data.az = (int16_t)((Wire.read() << 8) | Wire.read());
    } else {
        Serial.println("Error reading accelerometer data");
    }
}

// 读取陀螺仪数据
void ICM20600::readGyroData() {
    Wire.beginTransmission(icm_address);
    Wire.write(GYRO_XOUT_H);
    if (Wire.endTransmission() != 0) {
        Serial.println("Error writing to GYRO_XOUT_H register");
        return;
    }
    delay(1); // 增加延迟
    Wire.requestFrom(icm_address, 6u);
    if (Wire.available() == 6) {
        data.gx = (int16_t)((Wire.read() << 8) | Wire.read());
        data.gy = (int16_t)((Wire.read() << 8) | Wire.read());
        data.gz = (int16_t)((Wire.read() << 8) | Wire.read());
    } else {
        Serial.println("Error reading gyroscope data");
    }
}

// 读取陀螺仪温度数据
float ICM20600::readTemperature() {
    int16_t tempRaw = (readRegister(TEMP_OUT_H) << 8) | readRegister(TEMP_OUT_L);
    float tempC = (tempRaw / 326.8) + 25.0; // 根据数据手册的公式计算温度
    data.temp = tempC;
    return tempC;
}

// 检查设备是否连接
bool ICM20600::isConnected() {
    uint8_t whoAmI = readRegister(WHO_AM_I);
    Serial.print("WHO_AM_I register: ");
    Serial.println(whoAmI, HEX);
    return whoAmI == 0x11; // WHO_AM_I 寄存器的默认值为 0x11
}

//ICM20602_ACCEL_CONFIG寄存器
//设置为:0x00 加速度计量程为:±2g          获取到的加速度计数据 除以16384      可以转化为带物理单位的数据，单位：g(m/s^2)
//设置为:0x08 加速度计量程为:±4g          获取到的加速度计数据 除以8192       可以转化为带物理单位的数据，单位：g(m/s^2)
//设置为:0x10 加速度计量程为:±8g          获取到的加速度计数据 除以4096       可以转化为带物理单位的数据，单位：g(m/s^2)
//设置为:0x18 加速度计量程为:±16g         获取到的加速度计数据 除以2048       可以转化为带物理单位的数据，单位：g(m/s^2)
void ICM20600::setAccelRange(int range) {
    uint8_t config;

    // 设置新的量程
    switch (range) {
        case 2:
        acc_k = 16384;
        config = 0x00;
        break;
        case 4:
        acc_k = 8192;
        config = 0x08;
        case 8:
        acc_k = 4096;
        config = 0x10;
        break;
        case 16:
        acc_k = 2048;
        config = 0x18;
        break;
    }

    // 写回寄存器
    writeRegister(ACCEL_CONFIG_1, config);
}

//ICM20602_GYRO_CONFIG寄存器
//设置为:0x00 陀螺仪量程为:±250 dps     获取到的陀螺仪数据除以131           可以转化为带物理单位的数据， 单位为：°/s
//设置为:0x08 陀螺仪量程为:±500 dps     获取到的陀螺仪数据除以65.5          可以转化为带物理单位的数据，单位为：°/s
//设置为:0x10 陀螺仪量程为:±1000dps     获取到的陀螺仪数据除以32.8          可以转化为带物理单位的数据，单位为：°/s
//设置为:0x18 陀螺仪量程为:±2000dps     获取到的陀螺仪数据除以16.4          可以转化为带物理单位的数据，单位为：°/s
void ICM20600::setGyroRange(int range) {

    uint8_t config;
    // 设置新的量程
    switch (range) {
        case 250:
        gyro_k = 131.0f;
        config = 0x00;
        break;
        case 500:
        gyro_k = 65.5f;
        config = 0x08;
        break;
        case 1000:
        gyro_k = 32.8f;
        config = 0x010;
        break;
        case 2000:
        gyro_k = 16.4f;
        config = 0x18;
        break;
    }

    // 写回寄存器
    writeRegister(GYRO_CONFIG, config);
}

void ICM20600::updateK()
{
    uint8_t gyro_config = readRegister(icm_address, GYRO_CONFIG);
    uint8_t acc_config = readRegister(icm_address, ACCEL_CONFIG_1);

    switch (gyro_config)
    {
    case 0x00:
        gyro_k = 131.0f;
        break;
    case 0x08:
        gyro_k = 65.5f;
        break;
    case 0x10:
        gyro_k = 32.8f;
        break;
    case 0x18:
        gyro_k = 16.4f;
        break;
    }
    switch (acc_config)
    {
    case 0x00:
        acc_k = 16384;
        break;        
    case 0x08:
        acc_k = 8192;
        break;        
    case 0x10:
        acc_k = 4096;
        break;        
    case 0x18:
        acc_k = 2048;
        break;        
    }
    Serial.print("acc_k = ");
    Serial.println(acc_k);
    Serial.print("gyro_k = ");
    Serial.println(gyro_k);
}


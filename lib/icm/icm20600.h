#ifndef ICM20600_H
#define ICM20600_H

#include <Arduino.h>

typedef struct{
    float ax, ay, az;
}icm_acc_data;

typedef struct{
    float gx, gy, gz;
}icm_gyro_data;

typedef struct{
    // 原始数据
    int16_t ax,ay,az;
    int16_t gx,gy,gz;
    // 转换后的数据
    icm_acc_data acc;
    icm_gyro_data gyro;
    float temp;
}icm_data;
class ICM20600 {
public:
    void begin(int sda, int scl, uint8_t address = 0x68, uint32_t freq = 300000);
    
    float readTemperature(); // 添加读取温度的方法声明
    bool isConnected();
    void setAccelRange(int range);
    void setGyroRange(int range);

    icm_acc_data getAccelData();
    icm_gyro_data getGyroData();

private:
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
    uint8_t readRegister(uint8_t address, uint8_t reg);
    void readAccelData();
    void readGyroData();
    void updateK();//更新比例因子

    uint32_t acc_k;
    float gyro_k;
    icm_data data;
    uint8_t icm_address;
};

extern ICM20600 icm20600;
#endif // ICM20600_H
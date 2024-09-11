//==================================播放音乐测试=================================
// #include <FS.h>
// #include <SPIFFS.h>

// // 定义 WAV 文件头部结构
// struct wav_header {
//   char riff[4];             // "RIFF"
//   uint32_t file_size;       // 文件大小
//   char wave[4];             // "WAVE"
//   char fmt[4];              // "fmt "
//   uint32_t fmt_length;      // fmt 块的长度
//   uint16_t format_tag;      // 音频格式，1 表示 PCM
//   uint16_t channels;        // 声道数量（1=单声道，2=双声道）
//   uint32_t sample_rate;     // 采样率，如 16000 Hz
//   uint32_t byte_rate;       // 每秒字节数
//   uint16_t block_align;     // 数据块对齐
//   uint16_t bits_per_sample; // 采样位深（8 位或 16 位）
//   char data[4];             // "data"
//   uint32_t data_size;       // 音频数据大小
// };

// // 定义 PWM 通道
// const int PWM_CHANNEL = 0;
// const int PWM_PIN = 8; // 使用 GPIO8 输出

// void setup() {
//   Serial.begin(115200);

//   // 初始化 SPIFFS 文件系统
//   if (!SPIFFS.begin(true)) {
//     Serial.println("SPIFFS Mount Failed");
//     return;
//   }

//   // 打开 WAV 文件
//   File file = SPIFFS.open("/luo_half8.wav", "r");
//   if (!file) {
//     Serial.println("Failed to open file");
//     return;
//   }

//   // 读取并解析 WAV 文件头
//   wav_header header;
//   file.read((uint8_t*)&header, sizeof(header));

//   // 配置 PWM 通道
//   if (header.bits_per_sample == 8) {
//     ledcSetup(PWM_CHANNEL, header.sample_rate, 8);  // PWM 分辨率 8 位
//   } else if (header.bits_per_sample == 16) {
//     ledcSetup(PWM_CHANNEL, header.sample_rate, 16); // PWM 分辨率 16 位
//   }
  
//   ledcAttachPin(PWM_PIN, PWM_CHANNEL);

//   // 开始播放音频数据
//   uint32_t bytes_to_read = header.data_size; // 音频数据大小
//   while (bytes_to_read > 0 && file.available()) {
//     if (header.bits_per_sample == 8) {
//       // 如果是 8 位音频
//       uint8_t sample = file.read();  // 读取一个 8 位样本
//       ledcWrite(PWM_CHANNEL, sample); // 输出样本到 PWM 引脚
//     } else if (header.bits_per_sample == 16) {
//       // 如果是 16 位音频
//       uint16_t sample = file.read();   // 读取高 8 位
//       sample |= (file.read() << 8);    // 读取低 8 位，并组合成 16 位数据
//       ledcWrite(PWM_CHANNEL, sample >> 8); // 需要缩小到 8 位输出
//     }
//     // 延迟以保持 16kHz 的采样率
//     delayMicroseconds(62.5);  // 16kHz 的采样率，采样间隔约 62.5 微秒
//     // 控制采样率，确保 PWM 输出的节奏符合音频
//     // delayMicroseconds(1000000 / header.sample_rate); // 根据采样率调整延时
//     bytes_to_read--;
//   }

//   // 关闭文件
//   file.close();
// }

// void loop() {
//   // 不需要在循环中执行操作
// }

//=======================================屏幕和adc测试=======================================
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <rgb_array.h>

TFT_eSPI tft = TFT_eSPI();
void setup() {
  Serial.begin(115200);
  tft.init();
  tft.begin();
  tft.fillScreen(TFT_BLACK);
  // tft.setRotation(3); // 设置显示方向为横屏
  // tft.setTextSize(2);
  // tft.setTextColor(TFT_WHITE);
  // tft.setCursor(0, 10);
  // tft.println("Hello World!");

  // 使用TFT_eSPI显示图片
  tft.pushImage(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, image_data);//好用

  analogSetAttenuation(ADC_11db);    // 设置 ADC 衰减，以测量更高的电压（最高约3.3V）

}

void loop() {
  // put your main code here, to run repeatedly:
  // int adcValue = analogRead(11);     // 从 GPIO11（ADC2_0）读取 ADC 值
  // float voltage = adcValue * (3.3 / 4095.0);  // 将原始 ADC 值转换为电压值
  // Serial.println(voltage);          // 打印 ADC 原始值
  // delay(100);                       // 延时1秒
}



//========================================================sd卡测试========================================================
// #include <FS.h>
// #include <SD.h>
// #include <SD_MMC.h>

// void WriteFile(fs::FS &fs, const char *path, uint8_t *buf, int len)
// {
//   //unsigned long start_time = millis();
//   Serial.printf("write [%s]...\n", path);

//   File file = fs.open(path, FILE_WRITE);
//   if (!file) {
//     Serial.println("Failed to open file for writing");
//     return;
//   }
  
//   if (!file.write(buf, len)) {
//       Serial.println("Write failed");
//       return;
//     }

//   file.flush();
//   file.close();

//   Serial.printf("Write [%s] Complete", path);
// }

// void ReadFile(fs::FS &fs, const char *path, uint8_t *buf, int len)
// {
//   Serial.printf("read [%s]...\n", path);

//   File file = fs.open(path);
//   if (!file) {
//     Serial.println("Failed to open file for reading");
//     return;
//   }

//   if (!file.read(buf, len)) {
//       Serial.println("Read failed");
//       return;
//   }
  
//   file.close();

//   Serial.printf("Read [%s] Complete: %s", path, buf);
// }

// void testIO(fs::FS &fs)
// {
//   char buf[] = "hello world";

//   WriteFile(fs, "/test.txt", (uint8_t *)buf, strlen(buf));
//   ReadFile(fs, "/test.txt", (uint8_t *)buf, strlen(buf));
// }

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(115200);

//   Serial.println("请插入内存卡");
//   delay(8000);

//   /* SD SPI Mode at VSPI */
//   SPIClass spi = SPIClass(HSPI);
//   spi.begin(47 /* SCK */, 21 /* MISO */, 48 /* MOSI */, 45 /* SS */);
//   if (!SD.begin(45 /* SS */, spi, 80000000)) {
//     Serial.println("Card Mount Failed");
//     return;
//   }
//   testIO(SD);
//   SD_MMC.end(); // 取消SD挂载
// }

// void loop() {

// }
//================================陀螺仪测试================================
// #include <Arduino.h>
// #include <icm20600.h>

// void setup() {
//   Serial.begin(115200);
//   icm20600.begin(5, 4);

// }
// icm_data data;
// void loop() {
//   data.acc = icm20600.getAccelData();
//   data.gyro = icm20600.getGyroData();
//   Serial.print("Accelerometer: ");
//   Serial.print(data.acc.ax);
//   Serial.print(", ");
//   Serial.print(data.acc.ay);
//   Serial.print(", ");
//   Serial.println(data.acc.az);

//   Serial.print("Gyroscope: ");
//   Serial.print(data.gyro.gx);
//   Serial.print(", ");
//   Serial.print(data.gyro.gy);
//   Serial.print(", ");
//   Serial.println(data.gyro.gz);

//   float temp = icm20600.readTemperature();
//   Serial.print("Temperature: ");
//   Serial.println(temp);

//   delay(1000);
// }

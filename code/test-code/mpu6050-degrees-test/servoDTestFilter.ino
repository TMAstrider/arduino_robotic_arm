#include <Wire.h>
#include <Servo.h>

#define button 8

Servo myservo_1;  // 创建舵机对象以控制舵机
int pos = 0;    // 存储舵机位置的变量

int state = 0;
int buttonState = 0;
int check = 1;
int nil = 0;

const int MPU2 = 0x69, MPU1 = 0x68;

int16_t AcX1, AcY1, AcZ1, Tmp1, GyX1, GyY1, GyZ1;

int minVal = 0;  // 修改为加速度传感器实际的最小值
int maxVal = 360;  // 修改为加速度传感器实际的最大值

double x;
double y;
double z;

void setup() {
  pinMode(3, OUTPUT);
  Wire.begin();
  Wire.beginTransmission(MPU1);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);     // 设置为零（唤醒MPU-6050）
  Wire.endTransmission(true);
  Wire.begin();
  Wire.beginTransmission(MPU2);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);     // 设置为零（唤醒MPU-6050）
  Wire.endTransmission(true);
  Serial.begin(4800);
  delay(1000);
  myservo_1.attach(9);  // 将舵机连接到9号引脚
  myservo_1.write(90);  // 初始化舵机位置
  delay(5000);
}


// Constants for low-pass filter
const double alpha = 0.4; // Adjust this value based on your filtering requirements

void loop() {
  // 获取第一个地址为0x68的MPU的值
  GetMpuValue1(MPU1);

  // 控制舵机代码
  double targetPosition = x / 2;

  // Apply low-pass filter
  static double filteredPosition = 0.0;
  filteredPosition = alpha * targetPosition + (1.0 - alpha) * filteredPosition;

  // Serial.print("Filtered Position: ");
  Serial.println(filteredPosition);
  myservo_1.write(filteredPosition);
}



void GetMpuValue1(const int MPU) {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // 从0x3B寄存器开始（ACCEL_XOUT_H）
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true); // 请求总共14个寄存器

  AcX1 = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY1 = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ1 = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

  Tmp1 = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)

  int xAng = map(AcX1, minVal, maxVal, 0, 360);  // 映射加速度传感器的实际范围
  int yAng = map(AcY1, minVal, maxVal, -90, 90);
  int zAng = map(AcZ1, minVal, maxVal, -90, 90);

  GyX1 = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY1 = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ1 = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  x = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI) + 4; // 偏移4度以返回零
  y = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
  z = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);

  Serial.print("AngleX= ");
  Serial.print(x);
  Serial.print("\t");

  Serial.print("AngleY= ");
  Serial.print(y);
  Serial.print("\t");

  Serial.print("AngleZ= ");
  Serial.print(z);
  Serial.print("\t");
  Serial.println("-----------------------------------------");
}


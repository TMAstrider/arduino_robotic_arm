/********************************************************
 * master code 
 * Arduino Nano

  
  
*********************************************************/


#include<Wire.h>


#define ledPin 13
#define TCAADDR 0x70
const int flex1Pin = A3;
const int flex2Pin = A0;
int state = 0;
String data;

const int MPU2 = 0x69, MPU1 = 0x68;

int16_t AcX1, AcY1, AcZ1, Tmp1, GyX1, GyY1, GyZ1;

double x1, x2, x3;
double y1, y2, y3;
double z1, z2, z3;

int minVal = 265;
int maxVal = 402;

// Select I2C BUS -> 选择I2C通道
void TCA9548A(uint8_t bus){
  Wire.beginTransmission(0x70);  // TCA9548A address
  Wire.write(1 << bus);          // send byte to select bus
  Wire.endTransmission();
}

void setup() {
  // pinMode(3, OUTPUT);
  TCA9548A(2);
  Wire.begin();
  Wire.beginTransmission(MPU1);
  Wire.write(0x6B);// PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true); 

  TCA9548A(6);
  Wire.begin();
  Wire.beginTransmission(MPU1);
  Wire.write(0x6B);// PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true); 

  TCA9548A(3);
  Wire.begin();
  Wire.beginTransmission(MPU1);
  Wire.write(0x6B);// PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true); 

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Serial.begin(9600); // Default communication rate of the Bluetooth module
  pinMode(flex1Pin, INPUT);
  pinMode(flex2Pin, INPUT);
}

void loop() {
  // get values for first mpu having address of 0x68
  // delay(3);
  TCA9548A(2);
  //第一个MPU， 在I2C扩展的2通道
  GetMpuValue(MPU1, 1);
  delay(2);

  TCA9548A(6);
  // 第二个MPU，在I2C扩展的1通道
  GetMpuValue(MPU1, 2);
  delay(2);

  // TCA9548A(3);
  // // 第三个MPU，在I2C扩展的1通道
  // GetMpuValue(MPU1, 3);
  // delay(2);

  if(Serial.available() > 0){ // Checks whether data is comming from the serial port
      delay(3);
      data = Serial.readStringUntil('\n');
      // Serial.println(data);
      // controlServos(data);
  }
  // 夹爪
  int flexSensor1 = analogRead(flex1Pin);
  int flexMapped1 = map(flexSensor1, 430, 650, 0, 180);

  // 大臂
  int flexSensor2 = analogRead(flex2Pin);  
  flexSensor2 = map(flexSensor2, 400, 600, 0, 90);

  
  // y1
  int y1Original = y1;
  int y2Original = y2;


  x1Reconvine();


  y1 = y1Reconvine(y1);
  y2 = y2Reconvine(y2); // 

  // flexSensor2 = z3Reconvine(flexSensor2);


  // x1为腕部旋转参数
  int x1Map = x1;
  // y1为腕部抬升参数
  int y1Map = y1;
  // y2为手肘抬升参数
  int y2Map = y2;
  // z3为大臂抬升参数 flexSensor2
  int z3Map = flexSensor2;



  // z2原始范围和映射范围
  if(z2 > 190) {
    z2 = 190;
  }  

  y1Map = y1ReArrange(y1Map, y2Map);
  // y2Map = y2ReArrange()

  // x1 = -x1;
  y1Map = -y1Map;
  
  // 发送一串字符串，在另一端通过前面的标识分割数据
  Serial.print("X:" + (String)flexMapped1 + ",Y:" + x1Map + ",Z:" + y1Map + ",U:" + y2Map + ",V:" + z3Map + ",W:" + z2 + ",\n");
  


  // --------------------------------------------
  // // DEBUG
  // int x1Map = x1;
  // // 第一个 受影响的末端
  // int y1Map = y1;
  // int y2Map = y2;
  // int z3Map = flexSensor2;
  // Serial.print("第一个夹爪Flex:" + (String)flexMapped1 + ",第一个mpu旋转x:" + x1Map + ",*****-------**这后面的只要90°或在180°**--------------******第一个mpu俯仰y:" + y1Map + "--" + y1Original + ",第二个Mpu y2 axis:" + y2Map + "--" + y2Original + ",第三个Flex:" + z3Map + ",\n");
  // --------------------------------------------

  delay(50);
}


// y1的舵机旋转值受y2的影响，所以要进行处理
int y1ReArrange(int y1Val, int y2Val) {
  y1Val -= y2Val;
  if(y1Val < -90) {
    y1Val = -90;
  } else if(y1Val > 90) {
    y1Val = 90;
  }
  return y1Val;
}

// x1的映射范围 270° ~ 90° -> -90° ~ 90° 
void x1Reconvine() {
  if(x1 >= 270) {
    x1 -= 360;
  } else if(x1 < 270 && x1 >180) {
    x1 = -90;
  } else if(x1 > 90 && x1 <= 180) {
    x1 = 90;
  }
}

// y的映射范围 270° ~ 90° -> -90° ~ 90° 
// int yReconvine(double processVal) {
//   if(processVal >= 270) {
//     processVal -= 360;
//   } else if(processVal < 270 && processVal >180) {
//     processVal = -90;
//   } else if(processVal > 90 && processVal <= 180) {
//     processVal = 90;
//   }
//   return processVal;
// }


// 0 ~ 360 映射到 y1 -180 ~ 180
int y1Reconvine(double processVal) {
  if(processVal >= 180) {
    processVal -= 360;
  } 
  return processVal;
}

// int y2Reconvine(double processVal) {
//   if(processVal > 270) {
//     processVal -= 360;
//     // processVal = -processVal;
//   } else if(processVal <= 270 && processVal > 180) {
//     processVal = -90;
//   } else if(processVal > 90 && processVal <= 180) {
//     processVal = 90;
//   }
//   return processVal;
// }


// y2范围 -180 ~ 180
int y2Reconvine(double processVal) {
  if(processVal >270) {
    processVal -= 360;
    return processVal;
  } else if(processVal < 90) {
    return processVal;
  }
  if(processVal >= 90 && processVal <= 180) {
    return 90;
  }
  return -90;
}

int z3Reconvine(int fex) {
  fex -= 450;
  fex /= 13;
  fex *= 9;
  if(fex > 80) {
    fex = 80;
  }
  if(fex < 0) {
    fex = 0;
  }
  return fex;
}

// void controlServos(String data) {
//   // 解析数据并控制 LED 示例
//   int xVal = extractValue(data, "X");
//   if (xVal == 1) {
//     digitalWrite(ledPin, HIGH); // LED ON
//   } else if(xVal == 0) {
//     digitalWrite(ledPin, LOW); // LED OFF
//   }
// }

float extractValue(String data, String identifier) {
  // 从数据中提取角度值
  int index = data.indexOf(identifier);
  if (index != -1) {
    String valueString = data.substring(index + identifier.length() + 1, data.indexOf(",", index));
    return valueString.toFloat();
  }
  return 0.0;  // 默认值
}


void GetMpuValue(const int MPU, int num) {

  // 每一次读取MPU的数据需要先唤醒设备
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);// PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true); 
  

  Wire.beginTransmission(MPU);
  // Wire.write(0x6B);// PWR_MGMT_1 register
  // Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true); // request a total of 14 registers

  AcX1 = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY1 = Wire.read() << 8 |  Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ1 = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

  Tmp1 = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)

  int xAng = map(AcX1, minVal, maxVal, -90, 90);
  int yAng = map(AcY1, minVal, maxVal, -90, 90);
  int zAng = map(AcZ1, minVal, maxVal, -90, 90);

  GyX1 = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY1 = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ1 = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  if(num == 1) {
    x1 = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI) + 4; //offset by 4 degrees to get back to zero
    y1 = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
    z1 = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);
  } else if(num == 2) {
    x2 = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI) + 4; //offset by 4 degrees to get back to zero
    y2 = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
    z2 = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);
  } else if(num == 3) {
    x3 = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI) + 4; //offset by 4 degrees to get back to zero
    y3 = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
    z3 = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);
  }
  

  // Serial.print("AngleX= ");
  // Serial.print(x);
  // Serial.print("\t");

  // Serial.print("AngleY= ");
  // Serial.print(y);
  // Serial.print("\t");

  // Serial.print("AngleZ= ");
  // Serial.print(z);
  // Serial.print("\t");
  // Serial.println("-----------------------------------------");
}
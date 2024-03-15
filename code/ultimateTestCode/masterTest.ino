// master code Nano

#include<Wire.h>


#define ledPin 13
const int flexPin = A3;
int state = 0;
int potValue = 0;
#define TCAADDR 0x70
String data;
const int MPU2 = 0x69, MPU1 = 0x68;

int16_t AcX1, AcY1, AcZ1, Tmp1, GyX1, GyY1, GyZ1;

double x;
double y;
double z;

int minVal = 265;
int maxVal = 402;

// Select I2C BUS
void TCA9548A(uint8_t bus){
  Wire.beginTransmission(0x70);  // TCA9548A address
  Wire.write(1 << bus);          // send byte to select bus
  Wire.endTransmission();
  // Serial.print(bus);
}

void setup() {
  // pinMode(3, OUTPUT);
  Wire.begin();
  Wire.beginTransmission(MPU1);
  Wire.write(0x6B);// PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true); Wire.begin();
  Wire.beginTransmission(MPU2);
  Wire.write(0x6B);// PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Serial.begin(9600); // Default communication rate of the Bluetooth module

  pinMode(flexPin, INPUT);
}

void loop() {

  //get values for first mpu having address of 0x68
  TCA9548A(2);
  GetMpuValue1(MPU1);
  if(Serial.available() > 0){ // Checks whether data is comming from the serial port
      // state = Serial.read(); // Reads the data from the serial port
      delay(3);
      data = Serial.readStringUntil('\n');
      // Serial.println(data);
      controlServos(data);
      
      
  }
  // printf(data);
  // Serial.println(data);
  
  
  // Controlling the LED
  if (state == '1') {
    // digitalWrite(ledPin, HIGH); // LED ON
    // state = 0;
  }
  else if (state == '0') {
    digitalWrite(ledPin, LOW); // LED ON
    state = 0;
  }

  int flexSensor = analogRead(flexPin);
  int flexMapped = map(flexSensor, 430, 750, 0, 180);
  // printf(flexSensor);
  // printf("\n");
  // printf(flexMapped);
  // printf("\n");
  // Serial.write(flexSensor);
  String flexString ="X:";
  // flexString += (String)flexMapped;


  if(x > 300) {
    x -= 360;
  }
  int xMap = map(x, -90, 90, 0, 180);
  Serial.print(flexString + (String)flexMapped + ",Y:" + xMap + ",Z:101,\n");
  delay(50);
}


void controlServos(String data) {
  // 解析数据并控制舵机
  // 伪代码示例
  int xVal = extractValue(data, "X");
  // float y_angle = extractValue(data, "Y");
  // float z_angle = extractValue(data, "Z");
  // Serial.println(xVal);
  if (xVal == 1) {
    digitalWrite(ledPin, HIGH); // LED ON
  }
  if (xVal == 0) {
    digitalWrite(ledPin, LOW); // LED ON
  }
  // 控制舵机的代码
  // servoX.write(map(x_angle, -90, 90, 0, 180));
  // servoY.write(map(y_angle, -90, 90, 0, 180));
  // servoZ.write(map(z_angle, -90, 90, 0, 180));
}

float extractValue(String data, String identifier) {
  // 从数据中提取角度值
  // 伪代码示例
  int index = data.indexOf(identifier);
  if (index != -1) {
    String valueString = data.substring(index + identifier.length() + 1, data.indexOf(",", index));
    return valueString.toFloat();
  }
  return 0.0;  // 默认值
}


void GetMpuValue1(const int MPU) {

  Wire.beginTransmission(MPU);
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

  x = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI) + 4; //offset by 4 degrees to get back to zero
  y = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
  z = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);

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
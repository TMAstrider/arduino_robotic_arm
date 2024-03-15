/************************************************************************************





接线---------------------
  To use the module connect it to your Arduino as follows:

  PCA9685...........Uno/Nano
  GND...............GND
  OE................N/A
  SCL...............A5
  SDA...............A4
  VCC...............5V
  ------------------------
***********************************************************************************/

#include <Servo.h>
/* Include the HCPCA9685 library */
#include "HCPCA9685.h"

/*
I2C slave address for the device/module. 
For the HCMODU0097 the default I2C addressis 0x40 
*/
#define  I2CAdd 0x40
#define button 2

/*
旋转范围限制
10 ~ 590 为 0 - 180°旋转范围
下面是从末端到基座的舵机定义
*/

//Flex传感器
#define gripperServo 8
#define gripperHighThrottle 550 
#define gripperLowThrottle 10

#define wristServo 9
#define wristHighThrottle 550
#define wristLowThrottle 10

#define elbowServo 10
#define elbowHighThrottle 550
#define elbowLowThrottle 10

#define shoulderServo 10
#define shoulderHighThrottle 550
#define shoulderLowThrottle 10


// Constants for low-pass filter
const double alpha = 0.2; // Adjust this value based on your filtering requirements
static double filteredPosition1 = 0.0;
static double filteredPosition2 = 0.0;
static double filteredPosition3 = 0.0;
static double filteredPosition4 = 0.0;

String data;

/* Create an instance of the library */
HCPCA9685 HCPCA9685(I2CAdd);
Servo myServo;
int state = 20;
int buttonState = 0;

void setup() {
  pinMode(button, INPUT);
  /* Initialise the library and set it to 'servo mode' */ 
  HCPCA9685.Init(SERVO_MODE);

  /* Wake the device up */
  HCPCA9685.Sleep(false);
  myServo.attach(9);
  Serial.begin(9600); // Default communication rate of the Bluetooth module
}


void loop() {
  if(Serial.available() > 0){ // Checks whether data is comming from the serial port
      delay(3);
      data = Serial.readStringUntil('\n');
      Serial.println(data);
      controlServos(data);
  }
  // 蓝牙通信测试用
  // Reading the button
  buttonState = digitalRead(button);
  if (buttonState == HIGH) {
    // Serial.write('1');  // 按钮按下可以亮LED
    // Serial.print('1');
    // delay(50);
    // Serial.print("X:1,Y:0,Z:1\n");
    // Serial.print("X:120,\n");
    // Serial.print("myString\n");
  }
  else {
    // delay(50);
    // Serial.print("X:0,Y:0,Z:1\n");
    // Serial.write('0');
    // Serial.println('0');
  }  
}

void controlServos(String data) {
  // 解析数据并控制舵机
  // 伪代码示例
  int xVal = extractValue(data, "X");
  int yVal = extractValue(data, "Y");
  Serial.println(xVal);
  Serial.println(yVal);
  // float y_angle = extractValue(data, "Y");
  // float z_angle = extractValue(data, "Z");
  moveGripper(xVal);
  moveWrist(yVal);

  // if (xVal == 1) {
  //   digitalWrite(ledPin, HIGH); // LED ON
  // }
  // if (xVal == 0) {
  //   digitalWrite(ledPin, LOW); // LED ON
  // }
}

// 夹爪控制
void moveGripper(int rotateVal) {
  if(isOUTofRange(rotateVal)) return;
  int state1 = map(rotateVal, 0, 140, gripperLowThrottle, gripperHighThrottle);
  // Serial.println(state);
  // Apply low-pass filter
  filteredPosition1 = alpha * state1 + (1.0 - alpha) * filteredPosition1;
  HCPCA9685.Servo(gripperServo, filteredPosition1);
}

// 腕部旋转控制
void moveWrist(int rotateVal) {
  if(isOUTofRange(rotateVal)) return;
  int state2 = map(yVal, 0, 180, wristLowThrottle, wristHighThrottle);

  filteredPosition2 = alpha * state2 + (1.0 - alpha) * filteredPosition2;
  HCPCA9685.Servo(wristServo, filteredPosition2);
}

// 前臂旋转控制
void moveElbow(int rotateVal) {
  if(isOUTofRange(rotateVal)) return;
  int state3 = map(yVal, 0, 180, elbowLowThrottle, elbowHighThrottle);

  filteredPosition3 = alpha * state3 + (1.0 - alpha) * filteredPosition3;
  HCPCA9685.Servo(elbowServo, filteredPosition3);
}

// 大臂旋转控制
void moveShoulder(int rotateVal) {
  if(isOUTofRange(rotateVal)) return;
  int state4 = map(yVal, 0, 180, shoulderLowThrottle, shoulderHighThrottle);

  filteredPosition4 = alpha * state4 + (1.0 - alpha) * filteredPosition4;
  HCPCA9685.Servo(shoulderServo, filteredPosition4);
}

// 判断角度是否超出范围
int isOUTofRange(int rotateVal) {
  if(rotateVal < 0 || rotateVal > 180) {
    return 1;
  } else {
    return 0;
  }
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


/************************************************************************************ 
 * Slave code
 * Arduino Uno

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
For the HCMODU0097 the default I2C addressis 0x40 1
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
#define gripperHighThrottle 410 
#define gripperLowThrottle 0

#define rotateWristServo 9
#define rotateWristHighThrottle 410
#define rotateWristLowThrottle 0

#define moveWristServo 10
#define moveWristHighThrottle 450
#define moveWristLowThrottle 0

#define elbowServo 11
#define elbowHighThrottle 410
#define elbowLowThrottle 0

#define shoulderServo 12
#define shoulderHighThrottle 90
#define shoulderLowThrottle 10


// 舵机控制参数
// Constants for low-pass filter
const double alpha = 0.2; // Adjust this value based on your filtering requirements
const double alpha5 = 0.06; // Adjust this value based on your filtering requirements

static double filteredPosition1 = 0.0;
static double filteredPosition2 = 0.0;
static double filteredPosition3 = 0.0;
static double filteredPosition4 = 0.0;
static double filteredPosition5 = 0.0;

// 电机参数
const int dirPin = 4;//方向引脚
const int stepPin = 3;//步进引脚
const int stepsPerRevolution = 120;
int stepDelay = 4500;
int del=20;
int motorTemp= 0;
int response_time = 5;


// 主程序开始
String data;
/* Create an instance of the library */
HCPCA9685 HCPCA9685(I2CAdd);
Servo myServo;
int state = 20;
int buttonState = 0;

void setup() {

  // Motor PIN Setup
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  pinMode(button, INPUT);
  /* Initialise the library and set it to 'servo mode' */ 
  HCPCA9685.Init(SERVO_MODE);
  /* Wake the device up */
  HCPCA9685.Sleep(false);
  // myServo.attach(9);
  Serial.begin(9600); // Default communication rate of the Bluetooth module
}

void loop() {
  if(Serial.available() > 0){ // Checks whether data is comming from the serial port
      delay(3);
      data = Serial.readStringUntil('\n');
      Serial.println(data);
      controlServos(data);
  }

  // 测试舵机用
  HCPCA9685.Servo(12, 50);
      delay(30);
  //     HCPCA9685.Servo(11, 300);
  //     delay(30);
  //     HCPCA9685.Servo(10, 200);
  //     delay(30);
  //     HCPCA9685.Servo(9, 200);
  //     delay(30);
      // HCPCA9685.Servo(8, 0);
      // delay(30);


  // 蓝牙通信测试用
  // Reading the button
  // buttonState = digitalRead(button);
  // if (buttonState == HIGH) {
  //   // Serial.write('1');  // 按钮按下可以亮LED
  //   // Serial.print('1');
  //   // delay(50);
  //   // Serial.print("X:1,Y:0,Z:1\n");
  //   // Serial.print("X:120,\n");
  //   // Serial.print("myString\n");
  // }
  // else {
  //   // delay(50);
  //   // Serial.print("X:0,Y:0,Z:1\n");
  //   // Serial.write('0');
  //   // Serial.println('0');
  // }  
}

void controlServos(String data) {
  // 解析数据并控制舵机
  // 伪代码示例
  int xVal = extractValue(data, "X");
  int yVal = extractValue(data, "Y");
  int zVal = extractValue(data, "Z");
  int uVal = extractValue(data, "U");
  int vVal = extractValue(data, "V");
  // 电机控制值
  int wVal = extractValue(data, "W");
  
  moveGripper(xVal);  
  rotateWrist(yVal);  // 旋转手腕
  moveWrist(zVal);  // 移动手腕
  moveElbow(uVal); // 移动前臂
  // moveShoulder(vVal); //旋转肩部

  // 控制基座旋转
  // rotateBase(wVal);
}

// 夹爪控制
void moveGripper(int rotateVal) {
  // if(isOUTofRange(rotateVal)) return;
  if(rotateVal < 0) rotateVal = 0;
  int state1 = map(rotateVal, 0, 180, gripperLowThrottle, gripperHighThrottle);
  // Serial.println(state);
  // Apply low-pass filter
  filteredPosition1 = alpha * state1 + (1.0 - alpha) * filteredPosition1;
  HCPCA9685.Servo(gripperServo, filteredPosition1);
}

// 腕部旋转控制
void rotateWrist(int rotateVal) {
  // if(isOUTofRange(rotateVal)) return;
  int state2 = map(rotateVal, -90, 90, rotateWristLowThrottle, rotateWristHighThrottle);

  filteredPosition2 = alpha * state2 + (1.0 - alpha) * filteredPosition2;
  HCPCA9685.Servo(rotateWristServo, filteredPosition2);
}

// 腕部抬升控制
void moveWrist(int rotateVal) {
  // if(isOUTofRange(rotateVal)) return;
  rotateVal += 90;
  int state3 = map(rotateVal, 0, 180, moveWristLowThrottle, moveWristHighThrottle);
  // int state3 = rotateVal / 10;
  // state3 *= 40;
  // Serial.println(state3);

  filteredPosition3 = alpha * state3 + (1.0 - alpha) * filteredPosition3;
  HCPCA9685.Servo(moveWristServo, filteredPosition3);
}

// 肘部抬升控制
void moveElbow(int rotateVal) {
  if(isOUTofRange(rotateVal)) return;
  int state4 = map(rotateVal, -90, 60, elbowLowThrottle, elbowHighThrottle);
  if(state4 >= elbowHighThrottle) {
      state4 = elbowHighThrottle;
    }
  filteredPosition4 = alpha * state4 + (1.0 - alpha) * filteredPosition4;
  HCPCA9685.Servo(elbowServo, filteredPosition4);
}

// 大臂抬升控制
void moveShoulder(int rotateVal) {
  if(isOUTofRange(rotateVal)) return;
  if(rotateVal > 90) return;
  int state4 = map(rotateVal, 0, 90, shoulderLowThrottle, shoulderHighThrottle);
  if(state4 >= shoulderHighThrottle) {
    state4 = shoulderHighThrottle;
  }
  filteredPosition5 = alpha5 * state4 + (1.0 - alpha5) * filteredPosition5;
  HCPCA9685.Servo(shoulderServo, filteredPosition5);
  // Serial.print("FF" + (String)filteredPosition5 + "\n");
}

// 控制基座旋转
void rotateBase(int rotateVal) {
  if(rotateVal > 20 && rotateVal < 60) {
    baseRotateRight();
  }
  // if(rotateVal > 250 && rotateVal < 300){
  //   baseRotateLeft();
  // }
}



void baseRotateLeft() {
  //clockwise
  digitalWrite(dirPin, HIGH);
  // Spin motor
  for (motorTemp = 0; motorTemp < stepsPerRevolution; motorTemp++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
  delay(response_time); // Wait a second
}


void baseRotateRight() {

  //counterclockwise
  digitalWrite(dirPin, LOW);
  // Spin motor
  for (motorTemp = 0; motorTemp < stepsPerRevolution; motorTemp++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
  delay(response_time); // Wait a second
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
  int index = data.indexOf(identifier);
  if (index != -1) {
    String valueString = data.substring(index + identifier.length() + 1, data.indexOf(",", index));
    return valueString.toFloat();
  }
  return 0.0;  // 默认值
}


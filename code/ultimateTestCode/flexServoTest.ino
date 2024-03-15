
#include <Wire.h>
#include <Servo.h>

#define button 8

Servo myservo_1;  // 创建舵机对象以控制舵机
int pos = 0;    // 存储舵机位置的变量

const int flexPin = A3; // Pin connected to voltage divider output

void setup()
{
  
  Serial.begin(4800);
  pinMode(flexPin, INPUT);
  pinMode(3, OUTPUT);
  delay(1000);

  myservo_1.attach(9);  // 将舵机连接到9号引脚
  myservo_1.write(90);  // 初始化舵机位置
  delay(5000);
}


// Constants for low-pass filter
const double alpha = 0.08; // Adjust this value based on your filtering requirements

void loop() {
  
  // Read the ADC, and calculate voltage and resistance from it
  int flexSensor = analogRead(flexPin);
  // flexSensor -= 400;
  Serial.println(flexSensor);

  delay(50);
  // 控制舵机代码
  double targetPosition = map(flexSensor, 450, 650, 0, 180);

  // Apply low-pass filter
  static double filteredPosition = 0.0;
  filteredPosition = alpha * targetPosition + (1.0 - alpha) * filteredPosition;

  // Serial.print("Filtered Position: ");
  Serial.println(filteredPosition);
  myservo_1.write(filteredPosition);
}
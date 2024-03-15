/******************************************************************************
  Author: Smartbuilds.io
  YouTube: https://www.youtube.com/channel/UCGxwyXJWEarxh2XWqvygiIg
  Fork your own version: https://github.com/EbenKouao/arduino-robotic-arm
  Date: 28/12/2020
  Description: Test the MPU relative position X & Y co-ord in degreed

******************************************************************************/

#include<Wire.h>

#include <Servo.h>

#define button 8

Servo myservo_1;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position



int state = 0;
int buttonState = 0;
int check = 1;
int nil = 0;

const int MPU2 = 0x69, MPU1 = 0x68;

int16_t AcX1, AcY1, AcZ1, Tmp1, GyX1, GyY1, GyZ1;

int minVal = 265;
int maxVal = 402;

double x;
double y;
double z;

int response_time = 40;

int response_time_4 = 2;

const int servo_joint_3_parking_pos = 63;


int servo_joint_3_parking_pos_i = servo_joint_3_parking_pos;


int servo_joint_3_pos_increment = 4;

int servo_joint_3_max_pos = 170;
int servo_joint_3_min_pos =10;



void setup() {
  pinMode(3, OUTPUT);
  Wire.begin();
  Wire.beginTransmission(MPU1);
  Wire.write(0x6B);// PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true); Wire.begin();
  Wire.beginTransmission(MPU2);
  Wire.write(0x6B);// PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(4800);
  delay(1000);
  myservo_1.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo_1.write(90);
  delay(5000);

}


void loop() {
  //get values for first mpu having address of 0x68
  GetMpuValue1(MPU1);
  if ( x > 15 && x < 55) {
    if(servo_joint_3_parking_pos_i < servo_joint_3_max_pos) {
      myservo_1.write(servo_joint_3_parking_pos_i);
      delay(response_time_4);
      Serial.println(servo_joint_3_parking_pos_i);
      servo_joint_3_parking_pos_i = servo_joint_3_parking_pos_i + servo_joint_3_pos_increment;
    }
    
  }
  if ( x < 350 && x > 270) {
    if (servo_joint_3_parking_pos_i > servo_joint_3_min_pos) {
      myservo_1.write(servo_joint_3_parking_pos_i);
      delay(response_time_4);
      Serial.println(servo_joint_3_parking_pos_i);
      servo_joint_3_parking_pos_i = servo_joint_3_parking_pos_i - servo_joint_3_pos_increment;

    }
  }
    


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

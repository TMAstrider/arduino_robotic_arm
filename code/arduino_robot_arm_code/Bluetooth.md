### 可以定义不同的蓝牙消息格式
- <MESSAGE_TYPE>: (DATA1), (DATA2)...

``` arduino
String message = "SENSOR:POSITION," + String(position) + ",VELOCITY," + String(velocity);
// 这边是一个自己定义的消息格式 MASSAGE_TYPE 叫做 SENSOR，string变量名叫做message. 
// 后面就可以通过同一个格式(解出/分开)不同的数据了
```
``` arduino
String receivedMessage = ""; // 接收消息
if (receivedMessage.startsWith("SENSOR:")) {
    String data = receivedMessage.substring(7); // 去掉消息类型
    String tokens[4]; // 分隔后的数据
    int numTokens = splitString(data, ',', tokens, 4); // 使用逗号分隔数据
    if (numTokens == 4) {
        String dataType = tokens[0];
        float position = tokens[1].toFloat();
        String velocityType = tokens[2];
        float velocity = tokens[3].toFloat();
        // 这边就得到了完整的传感器数据或者是状态数据了
    }
}
```
``` arduino
///calculates the angles from the sensor values
void calculateAngles()
{
  //shoulder roll
  angles[1] = -atan2(-sensors[0][2], -sensors[0][1]);
  //shoulder pitch
  angles[2] = atan2(-sensors[0][0], -sensors[0][1]);
  float elbowPitch = atan2(-sensors[1][0], -sensors[1][1]);
  //setting relative pitch to shoulder
  angles[3] = elbowPitch - angles[2];
  //wrist roll
  angles[4] = atan2(sensors[2][1], sensors[2][2]);
  //gripper depending on analog value. We take the the square toot to get a linear movement. (point sources attenuate quadratically)
  angles[5] = (sqrt(max(analog, 0.f)) * (servoRange[5][1] - servoRange[5][0])) * M_PI;

  //calculating reverse tansformation for lower arm pitch
  float a1 = -atan2(-sensors[1][1], -sensors[1][0]);
  Matrix r0 = Matrix::rotation(-a1, 0, 1, 0);
  
  //calculating reverse transformation of wrist roll
  float a2 = atan2(sensors[2][1], sensors[2][2]);
  Matrix r1 = Matrix::rotation(angles[4], 1, 0, 0);
  
  //transforming the magnetic field vector back
  Vector v = r0 * r1 * sensors[3];
  //switching to flat earth compass
  v[2] = 0;
  v.normalize();
  //calculating angle between startup and current compass value
  Vector a = v.cross(compass0);
  angles[0] = -asin(a[2]);

  //setting the values
  setServoAngle(0, angles[0]);
  setServoAngle(1, angles[1]);
  setServoAngle(2, angles[2]);
  setServoAngle(3, angles[3]);
  setServoAngle(4, angles[4]);
  setServoAngle(5, angles[5]);
}
```
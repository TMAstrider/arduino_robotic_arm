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